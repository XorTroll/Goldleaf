
/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>
#include <vector>

namespace gleaf
{
    class ByteBuffer
    {
        public:
            ByteBuffer(size_t ReserveSize = 0);
            size_t GetSize();
            u8 *GetData();
            void Resize(size_t Size);
            template<typename T>
            T Read(u64 Offset)
            {
                if(Offset + sizeof(T) <= buffer.size()) return *((T*)&buffer.data()[Offset]);
                T def;
                memset(&def, 0, sizeof(T));
                return def;
            }
            template<typename T>
            void Write(T Data, u64 Offset)
            {
                size_t requiredSize = Offset + sizeof(T);
                if(requiredSize > buffer.size()) buffer.resize(requiredSize, 0);
                memcpy(buffer.data() + Offset, &Data, sizeof(T));
            }
            template<typename T>
            void Append(T Data)
            {
                this->Write(Data, this->GetSize());
            }
        private:
            std::vector<u8> buffer;
    };
}