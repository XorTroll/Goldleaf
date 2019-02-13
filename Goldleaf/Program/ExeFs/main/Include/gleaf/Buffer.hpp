
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

    Code ported from exelix's source

*/

#pragma once
#include <vector>  // buffers
#include <sstream> // strings, byteStr()
#include <gleaf/Types.hpp>

namespace gleaf
{
    enum class Endianness : unsigned char
    {
        LittleEndian,
        BigEndian
    };

    class Buffer {
    public:
        enum class BinaryString : unsigned char 
        {
            NoPrefixOrTermination,
            NullTerminated,
            WordLengthPrefix
        };

        Endianness ByteOrder = Endianness::LittleEndian;

        Buffer() ;
        Buffer(const std::vector<unsigned char>&) ;

        void setBuffer(std::vector<unsigned char>&) ;
        const std::vector<unsigned char> &getBuffer() const ;
        void clear() ;

        std::string byteStr(bool LE = true) const ;

        unsigned long long Length();
        unsigned long long Position = 0;
        /************************** Writing ***************************/

        template <class T> inline void writeBytes(const T &val, bool LE = true);

        void WriteAlign(int val);

        void Write(const std::string&, BinaryString s = BinaryString::NoPrefixOrTermination);
        void Write(char);
        void Write(unsigned char);
        void Write(short);
        void Write(unsigned short);
        void Write(int);
        void Write(unsigned int);
        void Write(long long);
        void Write(unsigned long long);
        void Write(float);
        void Write(double);
        void Write(const std::vector<unsigned char>&);

        void WriteU32Array(const std::vector<unsigned int>& arr);

        void writeInt16_LE(short) ;
        void writeInt16_BE(short) ;
        void writeUInt16_LE(unsigned short) ;
        void writeUInt16_BE(unsigned short) ;

        void writeInt32_LE(int) ;
        void writeInt32_BE(int) ;
        void writeUInt32_LE(unsigned int) ;
        void writeUInt32_BE(unsigned int) ;

        void writeInt64_LE(long long) ;
        void writeInt64_BE(long long) ;
        void writeUInt64_LE(unsigned long long) ;
        void writeUInt64_BE(unsigned long long) ;

        void writeFloat_LE(float) ;
        void writeFloat_BE(float) ;
        void writeDouble_LE(double) ;
        void writeDouble_BE(double) ;

        /************************** Reading ***************************/

        template <class T> inline T readBytes(bool LE = true);

        bool               readBool() ;
        std::string        readStr(unsigned long long len) ;
        std::string        readStr_NullTerm(int maxLen = INT32_MAX);
        std::string        readStr_U16Prefix();

        char               readInt8() ;
        unsigned char      readUInt8() ;

        short              readInt16() ;
        short              readInt16_LE() ;
        short              readInt16_BE() ;
        unsigned short     readUInt16() ;
        unsigned short     readUInt16_LE() ;
        unsigned short     readUInt16_BE() ;

        int                readInt32() ;
        int                readInt32_LE() ;
        int                readInt32_BE() ;
        unsigned int       readUInt32() ;
        unsigned int       readUInt32_LE() ;
        unsigned int       readUInt32_BE() ;

        long long          readInt64() ;
        long long          readInt64_LE() ;
        long long          readInt64_BE() ;
        unsigned long long readUInt64() ;
        unsigned long long readUInt64_LE() ;
        unsigned long long readUInt64_BE() ;

        float              readFloat() ;
        float              readFloat_LE() ;
        float              readFloat_BE() ;
        double             readDouble() ;
        double             readDouble_LE() ;
        double             readDouble_BE() ;

        std::vector<unsigned int> ReadU32Array(int count);
        std::vector<int> ReadS32Array(int count);

        std::vector<unsigned char> readBytes(unsigned int count);

        ~Buffer();
    private:
        std::vector<unsigned char> buffer;
        void putByte(unsigned char b);
    };
}