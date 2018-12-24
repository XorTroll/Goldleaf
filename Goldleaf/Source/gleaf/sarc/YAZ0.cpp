#include <gleaf/sarc/YAZ0.hpp>
using namespace std;

namespace gleaf::sarc
{
    vector<u8> YAZ0::Decompress(vector<u8> &Data) 
    {
        u32 leng = (u32)(Data[4] << 24 | Data[5] << 16 | Data[6] << 8 | Data[7]);
        vector<u8> _Result(leng);
        u8* ResPtr = _Result.data();	
        int Offs = 16;
        u32 dstoffs = 0;
        while (true)
        {
            u8 header = Data[Offs++];
            for (int i = 0; i < 8; i++)
            {
                if ((header & 0x80) != 0) ResPtr[dstoffs++] = Data[Offs++];
                else
                {
                    u8 b = Data[Offs++];
                    int offs = ((b & 0xF) << 8 | Data[Offs++]) + 1;
                    int length = (b >> 4) + 2;
                    if (length == 2) length = Data[Offs++] + 0x12;
                    for (int j = 0; j < length; j++)
                    {
                        ResPtr[dstoffs] = ResPtr[dstoffs - offs];
                        dstoffs++;
                    }
                }
                if (dstoffs >= leng) return _Result;
                header <<= 1;
            }
        }
    }

    bool is_big_endian(void)
    {
        union {
            uint32_t i;
            char c[4];
        } bint = { 0x01020304 };

        return bint.c[0] == 1;
    }

    vector<u8> YAZ0::Compress(vector<u8> &Data, int level, int reserved1, int reserved2)
    {
        int maxBackLevel = (int)(0x10e0 * (level / 9.0) - 0x0e0);

        u8* dataptr = Data.data();

        vector<u8> result(Data.size() + Data.size() / 8 + 0x10);
        u8* resultptr = result.data();
        *resultptr++ = (u8)'Y';
        *resultptr++ = (u8)'a';
        *resultptr++ = (u8)'z';
        *resultptr++ = (u8)'0';
        *resultptr++ = (u8)((Data.size() >> 24) & 0xFF);
        *resultptr++ = (u8)((Data.size() >> 16) & 0xFF);
        *resultptr++ = (u8)((Data.size() >> 8) & 0xFF);
        *resultptr++ = (u8)((Data.size() >> 0) & 0xFF);
        {
            union { int val; u8 u8s[4]; } res1 = { reserved1 };
            union { int val; u8 u8s[4]; } res2 = { reserved2 };
            if (is_big_endian())
            {
                *resultptr++ = (u8)res1.u8s[0];
                *resultptr++ = (u8)res1.u8s[1];
                *resultptr++ = (u8)res1.u8s[2];
                *resultptr++ = (u8)res1.u8s[3];
                *resultptr++ = (u8)res2.u8s[0];
                *resultptr++ = (u8)res2.u8s[1];
                *resultptr++ = (u8)res2.u8s[2];
                *resultptr++ = (u8)res2.u8s[3];
            }
            else
            {
                *resultptr++ = (u8)res1.u8s[3];
                *resultptr++ = (u8)res1.u8s[2];
                *resultptr++ = (u8)res1.u8s[1];
                *resultptr++ = (u8)res1.u8s[0];
                *resultptr++ = (u8)res2.u8s[3];
                *resultptr++ = (u8)res2.u8s[2];
                *resultptr++ = (u8)res2.u8s[1];
                *resultptr++ = (u8)res2.u8s[0];
            }
        }
        int length = Data.size();
        int dstoffs = 16;
        int Offs = 0;
        while (true)
        {
            int headeroffs = dstoffs++;
            resultptr++;
            u8 header = 0;
            for (int i = 0; i < 8; i++)
 
 
            {
                int comp = 0;
                int back = 1;
                int nr = 2;
                {
                    u8* ptr = dataptr - 1;
                    int maxnum = 0x111;
                    if (length - Offs < maxnum) maxnum = length - Offs;
                    //Use a smaller amount of u8s back to decrease time
                    int maxback = maxBackLevel;//0x1000;
                    if (Offs < maxback) maxback = Offs;
                    maxback = (int)dataptr - maxback;
                    int tmpnr;
                    while (maxback <= (int)ptr)
                    {
                        if (*(u16*)ptr == *(u16*)dataptr && ptr[2] == dataptr[2])
                        {
                            tmpnr = 3;
                            while (tmpnr < maxnum && ptr[tmpnr] == dataptr[tmpnr]) tmpnr++;
                            if (tmpnr > nr)
                            {
                                if (Offs + tmpnr > length)
                                {
                                    nr = length - Offs;
                                    back = (int)(dataptr - ptr);
                                    break;
                                }
                                nr = tmpnr;
                                back = (int)(dataptr - ptr);
                                if (nr == maxnum) break;
                            }
                        }
                        --ptr;
                    }
                }
                if (nr > 2)
                {
                    Offs += nr;
                    dataptr += nr;
                    if (nr >= 0x12)
                    {
                        *resultptr++ = (u8)(((back - 1) >> 8) & 0xF);
                        *resultptr++ = (u8)((back - 1) & 0xFF);
                        *resultptr++ = (u8)((nr - 0x12) & 0xFF);
                        dstoffs += 3;
                    }
                    else
                    {
                        *resultptr++ = (u8)((((back - 1) >> 8) & 0xF) | (((nr - 2) & 0xF) << 4));
                        *resultptr++ = (u8)((back - 1) & 0xFF);
                        dstoffs += 2;
                    }
                    comp = 1;
                }
                else
                {
                    *resultptr++ = *dataptr++;
                    dstoffs++;
                    Offs++;
                }
                header = (u8)((header << 1) | ((comp == 1) ? 0 : 1));
                if (Offs >= length)
                {
                    header = (u8)(header << (7 - i));
                    break;
                }
            }
            result[headeroffs] = header;
            if (Offs >= length) break;
        }
        while ((dstoffs % 4) != 0) dstoffs++;
        result.resize(dstoffs);
        return result;
    }
}