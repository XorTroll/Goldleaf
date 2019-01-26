#pragma once
#include <gleaf/Types.hpp>
#include <Commands.hpp>

namespace gleaf::usb
{
    class UsbReader
    {
        public:
            UsbReader();
            void Initialize();

            std::unique_ptr<std::string[]> ReadFiles(size_t* count);

            std::vector<NSPContentData> ReadData(std::string file);
            std::unique_ptr<u8[]> ReadContent(std::string file, u32 index, size_t* size);
            std::unique_ptr<u8[]> ReadTicket(std::string file, size_t* size);

            u32 TestEcho(const void* data, size_t size);
        private:
            u32 ReadUInt32();
            u64 ReadUInt64();
            void* Read(size_t size);
            std::string ReadString();


            void WriteUInt32(u32 value);
            void WriteCommandId(CommandId id);
            void WriteString(std::string value);
            void Write(const void* data, size_t size);
            
    };
}