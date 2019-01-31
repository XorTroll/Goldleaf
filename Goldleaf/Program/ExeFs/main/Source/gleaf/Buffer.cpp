#include <gleaf/Buffer.hpp>
#include <stdexcept>
#include <iomanip>

namespace gleaf
{
    /************************* WRITING *************************/
    Buffer::Buffer()  {}
    Buffer::Buffer(const std::vector<unsigned char> &_buffer) :
        buffer(_buffer) {}

    long long unsigned int Buffer::Length() { return buffer.size(); }

    void Buffer::setBuffer(std::vector<unsigned char> &_buffer)  {
        buffer = _buffer;
    }
    const std::vector<unsigned char> &Buffer::getBuffer() const  {
        return buffer;
    }
    void Buffer::clear()  {
        buffer.clear();
        Position = 0;
    }

    std::string Buffer::byteStr(bool LE) const  {
        std::stringstream byteStr;
        byteStr << std::hex << std::setfill('0');

        if (LE == true) {
            for (unsigned long long i = 0; i < buffer.size(); ++i)
                byteStr << std::setw(2) << (unsigned short)buffer[i] << " ";
        } else {
            unsigned long long size = buffer.size();
            for (unsigned long long i = 0; i < size; ++i)
                byteStr << std::setw(2) << (unsigned short)buffer[size - i - 1] << " ";
        }

        return byteStr.str();
    }

    void Buffer::putByte(unsigned char b)
    {
        if (Position < buffer.size())
            buffer[Position] = b;
        else 
        {
            buffer.push_back(b);
        }
        Position++;
    }

    //I think this works correctly only on little endian cpus 
    template <class T> inline void Buffer::writeBytes(const T &val, bool LE) {
        unsigned int size = sizeof(T);

        if (LE == true) {
            for (unsigned int i = 0, mask = 0; i < size; ++i, mask += 8)
                putByte(val >> mask);
        } else {
            unsigned const char *array = reinterpret_cast<unsigned const char*>(&val);
            for (unsigned int i = 0; i < size; ++i)
                putByte(array[size - i - 1]);
        }
    }

    void Buffer::Write(char val)  {
        putByte(*(unsigned char*)&val);
    }
    void Buffer::Write(unsigned char val)  {
        putByte(val);
    }

    void Buffer::WriteAlign(int val) 
    {
        while (Position % val != 0)
            Write((unsigned char)0);
    }

    void Buffer::WriteU32Array(const std::vector<unsigned int>& arr)
    {
        for (int i = 0; i < arr.size(); i++)
            Write(arr[i]);
    }

    void Buffer::Write(const std::string &str, BinaryString type) {
        for (const unsigned char &s : str) Write(s);
        if (type == BinaryString::NullTerminated)
            Write((unsigned char)0);
    }
    void Buffer::Write(short val)  
    {
        writeBytes<short>(val, ByteOrder == Endianness::LittleEndian);
    }
    void Buffer::writeInt16_LE(short val)  {
        writeBytes<short>(val);
    }
    void Buffer::writeInt16_BE(short val)  {
        writeBytes<short>(val, false);
    }

    void Buffer::Write(unsigned short val)  {
        writeBytes<unsigned short>(val, ByteOrder == Endianness::LittleEndian);
    }
    void Buffer::writeUInt16_LE(unsigned short val)  {
        writeBytes<unsigned short>(val);
    }
    void Buffer::writeUInt16_BE(unsigned short val)  {
        writeBytes<unsigned short>(val, false);
    }

    void Buffer::Write(int val)  {
        writeBytes<int>(val, ByteOrder == Endianness::LittleEndian);
    }
    void Buffer::writeInt32_LE(int val)  {
        writeBytes<int>(val);
    }
    void Buffer::writeInt32_BE(int val)  {
        writeBytes<int>(val, false);
    }

    void Buffer::Write(unsigned int val)  {
        writeBytes<unsigned int>(val, ByteOrder == Endianness::LittleEndian);
    }
    void Buffer::writeUInt32_LE(unsigned int val)  {
        writeBytes<unsigned int>(val);
    }
    void Buffer::writeUInt32_BE(unsigned int val)  {
        writeBytes<unsigned int>(val, false);
    }

    void Buffer::Write(long long val)  {
        writeBytes<long long>(val, ByteOrder == Endianness::LittleEndian);
    }
    void Buffer::writeInt64_LE(long long val)  {
        writeBytes<long long>(val);
    }
    void Buffer::writeInt64_BE(long long val)  {
        writeBytes<long long>(val, false);
    }

    void Buffer::Write(unsigned long long val)  {
        writeBytes<unsigned long long>(val, ByteOrder == Endianness::LittleEndian);
    }
    void Buffer::writeUInt64_LE(unsigned long long val)  {
        writeBytes<unsigned long long>(val);
    }
    void Buffer::writeUInt64_BE(unsigned long long val)  {
        writeBytes<unsigned long long>(val, false);
    }

    void Buffer::Write(float val)  {
        union { float fnum; unsigned inum; } u;
        u.fnum = val;
        Write(u.inum);
    }
    void Buffer::writeFloat_LE(float val)  {
        union { float fnum; unsigned inum; } u;
        u.fnum = val;
        writeUInt32_LE(u.inum);
    }
    void Buffer::writeFloat_BE(float val)  {
        union { float fnum; unsigned inum; } u;
        u.fnum = val;
        writeUInt32_BE(u.inum);
    }

    void Buffer::Write(double val)  {
        union { double fnum; unsigned long long inum; } u;
        u.fnum = val;
        Write(u.inum);
    }
    void Buffer::writeDouble_LE(double val)  {
        union { double fnum; unsigned long long inum; } u;
        u.fnum = val;
        writeUInt64_LE(u.inum);
    }
    void Buffer::writeDouble_BE(double val)  {
        union { double fnum; unsigned long long inum; } u;
        u.fnum = val;
        writeUInt64_BE(u.inum);
    }

    void Buffer::Write(const std::vector<unsigned char>& vec)
    {
        int size = vec.size();
        for (int i = 0; i < size; i++)
            Write(vec[i]);
    }

    /************************* READING *************************/

    template <class T> inline T Buffer::readBytes(bool LE) {
        T result = 0;
        unsigned int size = sizeof(T);

        // Do not overflow
        if (Position + size > buffer.size())
            throw std::out_of_range("Position + size > buffer.size()");

        char *dst = (char*)&result;
        char *src = (char*)&buffer[Position];

        if (LE == true) {
            for (unsigned int i = 0; i < size; ++i)
                dst[i] = src[i];
        } else {
            for (unsigned int i = 0; i < size; ++i)
                dst[i] = src[size - i - 1];
        }
        Position += size;
        return result;
    }

    std::vector<unsigned int> Buffer::ReadU32Array(int count) {
        std::vector<unsigned int> out(count);
        for (int i = 0; i < count; i++)
            out[i] = readUInt32();
        return out;
    }

    std::vector<int> Buffer::ReadS32Array(int count)
    {
        std::vector<int> out(count);
        for (int i = 0; i < count; i++)
            out[i] = readInt32();
        return out;
    }

    bool Buffer::readBool()  {
        return readBytes<bool>();
    }

    std::string Buffer::readStr_U16Prefix() 
    {
        unsigned short len = readUInt16();
        return readStr(len);
    }

    std::string Buffer::readStr(unsigned long long len)  {
        if (Position + len > buffer.size())
            throw std::out_of_range("Buffer out of range (provided length greater than buffer size)");
        std::string result(buffer.begin() + Position, buffer.begin() + Position + len);
        Position += len;
        return result;
    }
    std::string Buffer::readStr_NullTerm(int maxLen) 
    {
        std::vector<char> buf;
        char c = readInt8();
        int charCount = 0;
        while (c != 0 && charCount < maxLen)
        {
            buf.push_back(c);
            charCount++;
            c = readInt8();
        }
        return std::string(buf.begin(), buf.end());
    }

    char Buffer::readInt8()  {
        return readBytes<char>();
    }
    unsigned char Buffer::readUInt8()  {
        return readBytes<unsigned char>();
    }

    short Buffer::readInt16()  {
        return readBytes<short>(ByteOrder == Endianness::LittleEndian);
    }
    short Buffer::readInt16_LE()  {
        return readBytes<short>();
    }
    short Buffer::readInt16_BE()  {
        return readBytes<short>(false);
    }

    unsigned short Buffer::readUInt16()  {
        return readBytes<unsigned short>(ByteOrder == Endianness::LittleEndian);
    }
    unsigned short Buffer::readUInt16_LE()  {
        return readBytes<unsigned short>();
    }
    unsigned short Buffer::readUInt16_BE()  {
        return readBytes<unsigned short>(false);
    }

    int Buffer::readInt32()  {
        return readBytes<int>(ByteOrder == Endianness::LittleEndian);
    }
    int Buffer::readInt32_LE()  {
        return readBytes<int>();
    }
    int Buffer::readInt32_BE()  {
        return readBytes<int>(false);
    }

    unsigned int Buffer::readUInt32()  {
        return readBytes<unsigned int>(ByteOrder == Endianness::LittleEndian);
    }
    unsigned int Buffer::readUInt32_LE()  {
        return readBytes<unsigned int>();
    }
    unsigned int Buffer::readUInt32_BE()  {
        return readBytes<unsigned int>(false);
    }

    long long Buffer::readInt64()  {
        return readBytes<long long>(ByteOrder == Endianness::LittleEndian);
    }
    long long Buffer::readInt64_LE()  {
        return readBytes<long long>();
    }
    long long Buffer::readInt64_BE()  {
        return readBytes<long long>(false);
    }

    unsigned long long Buffer::readUInt64()  {
        return readBytes<unsigned long long>(ByteOrder == Endianness::LittleEndian);
    }
    unsigned long long Buffer::readUInt64_LE()  {
        return readBytes<unsigned long long>();
    }
    unsigned long long Buffer::readUInt64_BE()  {
        return readBytes<unsigned long long>(false);
    }

    float Buffer::readFloat()  {
        return readBytes<float>(ByteOrder == Endianness::LittleEndian);
    }
    float Buffer::readFloat_LE()  {
        return readBytes<float>();
    }
    float Buffer::readFloat_BE()  {
        return readBytes<float>(false);
    }

    double Buffer::readDouble()  {
        return readBytes<double>(ByteOrder == Endianness::LittleEndian);
    }
    double Buffer::readDouble_LE()  {
        return readBytes<double>();
    }
    double Buffer::readDouble_BE()  {
        return readBytes<double>(false);
    }

    std::vector<unsigned char> Buffer::readBytes(unsigned int count) 
    {
        if (Position + count > buffer.size())
            throw std::out_of_range("Buffer out of range (provided length greater than buffer size)");
        std::vector<unsigned char> res(count);
        for (unsigned i = 0; i < count; i++)
            res[i] = buffer[Position++];
        return res;
    }

    Buffer::~Buffer() {
        clear();
    }
}