#include <gleaf/usb/UsbReader.hpp>


namespace gleaf::usb
{
    UsbReader::UsbReader() 
    {

    }

    void UsbReader::Initialize()
    {

    }
    
    std::unique_ptr<std::string[]> UsbReader::ReadFiles(size_t* count)
    {
        this->WriteCommandId(CommandId::ListNSPs);
        this->WriteUInt32(0);

        *count = this->ReadUInt32();
        std::unique_ptr<std::string[]> result = std::make_unique<std::string[]>(*count);
        
        for (u32 i = 0; i < *count; i++) {
            result[i] = this->ReadString();
        }

        return result;
    }
    
    std::vector<NSPContentData> UsbReader::ReadData(std::string file)
    {
        this->WriteCommandId(CommandId::GetNSPInfo);
        this->WriteString(file);

        std::vector<NSPContentData> result;

        u32 filecount = this->ReadUInt32();
        for(u32 i = 0; i < filecount; i++)
        {
            std::string name = this->ReadString();
            u64 offset = this->ReadUInt64();
            u64 size = this->ReadUInt64();
            result.push_back({ i, name, offset, size });
        }
    
        return result;
    }

    std::unique_ptr<u8[]> UsbReader::ReadContent(std::string file, u32 index, size_t* size)
    {
        this->WriteCommandId(CommandId::GetNSPContent);
        this->WriteUInt32(file.length() + 2 * sizeof(u32)); // Command length
        this->WriteString(file);
        this->WriteUInt32(index);

        *size = this->ReadUInt32();
        std::unique_ptr<u8[]> result(static_cast<u8*>(this->Read(*size)));
        return result;
    }
    
    std::unique_ptr<u8[]> UsbReader::ReadTicket(std::string file, size_t* size)
    {
        this->WriteCommandId(CommandId::GetNSPTicket);
        this->WriteString(file);

        *size = this->ReadUInt32();
        std::unique_ptr<u8[]> result(static_cast<u8*>(this->Read(*size)));
        return result;
    }

    std::unique_ptr<u8[]> UsbReader::ReadCertificate(std::string file, size_t* size)
    {
        this->WriteCommandId(CommandId::GetNSPCertificate);
        this->WriteString(file);

        *size = this->ReadUInt32();
        std::unique_ptr<u8[]> result(static_cast<u8*>(this->Read(*size)));
        return result;
    }

    u32 UsbReader::TestEcho(const void* data, size_t size)
    {
        this->WriteCommandId(CommandId::Echo);
        this->WriteUInt32(size);
        this->Write(data, size);

        if (size != this->ReadUInt32()) {
            return -1;
        }

        const void* result = this->Read(size);

        for (u32 i = 0; i < size; i++) {
            if (static_cast<const u8*>(data)[i] != static_cast<const u8*>(result)[i]) {
                return i;
            }
        }

        return 0;
    }


    u32 UsbReader::ReadUInt32()
    {
        void* value = this->Read(sizeof(u32));
        u32 result = *((u32*)value);
        free(value);
        return result;
    }
    
    u64 UsbReader::ReadUInt64()
    {
        void* value = this->Read(sizeof(u64));
        u64 result = *((u64*)value);
        free(value);
        return result;
    }

    void* UsbReader::Read(size_t size) 
    {
        u8* result = static_cast<u8*>(malloc(size));
        size_t read = 0;
        while (read < size) 
        {
            read += usbCommsRead(result + read, size);

            if (read == 0) {
                break; // If the size of read is zero we assume the read size is invalid and assume that everything has been read
            }
        }

        return result;
    }

    std::string UsbReader::ReadString()
    {
        size_t size = this->ReadUInt32();
        void* data = this->Read(size);
        return std::string(static_cast<const char*>(data));
    }
    
    void UsbReader::WriteUInt32(u32 value)
    {
        this->Write(&value, sizeof(u32));
    }


    void UsbReader::WriteCommandId(CommandId id)
    {
        this->WriteUInt32(static_cast<u32>(id));        
    }
    
    void UsbReader::WriteString(std::string value)
    {
        this->WriteUInt32(value.length());
        this->Write(value.c_str(), sizeof(char) * value.length());
    }

    void UsbReader::Write(const void* data, size_t size)
    {
        const u8* pointer = static_cast<const u8*>(data);
        while (size > 0)
        {
            size_t written = usbCommsWrite(pointer, size);
            if (written == 0) {
                return;  // If the size of written is zero we assume the written size is invalid and assume that everything has been written
            }

            size -= written;
            pointer += written;
        }
    }
} // gleaf.usb
