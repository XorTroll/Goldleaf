
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

    Credits to Stary for the base of USB-drive support!

*/

#pragma once
#include <cstring>

namespace gleaf::drive
{
	enum class SCSIDirection
	{
		None,
		In,
		Out
	};

	#define CBW_SIGNATURE 0x43425355

	#define COMMAND_PASSED 0
	#define COMMAND_FAILED 1
	#define PHASE_ERROR 2
	#define CSW_SIZE 13
	#define CSW_SIGNATURE 0x53425355

	class SCSIBuffer
	{
		public:
			int idx = 0;
			uint8_t storage[31];

			SCSIBuffer()
			{
				memset(storage, 0, sizeof(storage));
			}

			void write_u8(uint8_t e)
			{
				memcpy(&storage[idx], &e, sizeof(e));
				idx++;
			}

			void padding(int n)
			{
				idx += n;
			}

			void write_u16_be(uint16_t e)
			{
				e = __builtin_bswap16(e);
				memcpy(&storage[idx], &e, sizeof(e));
				idx += 2;
			}

			void write_u32(uint32_t e)
			{
				memcpy(&storage[idx], &e, sizeof(e));
				idx += 4;
			}

			void write_u32_be(uint32_t e)
			{
				e = __builtin_bswap32(e);
				memcpy(&storage[idx], &e, sizeof(e));
				idx += 4;
			}
	};

	class SCSICommand
	{ 
		public:
			uint32_t tag;
			uint32_t data_transfer_length;
			uint8_t flags;
			uint8_t lun;
			uint8_t cb_length;
			SCSIDirection direction;

			SCSICommand(uint32_t data_transfer_length_, SCSIDirection direction_, uint8_t lun_, uint8_t cb_length_)
			{
				tag = 0; // should this be default?

				data_transfer_length = data_transfer_length_;
				direction = direction_;
				lun = lun_;
				cb_length = cb_length_;

				if(direction == SCSIDirection::In)
				{
					flags = 0x80;
				}
				else
				{
					flags = 0;
				}
			}

			virtual void to_bytes(uint8_t *out) = 0;
			void write_header(SCSIBuffer &out)
			{
				out.write_u32(CBW_SIGNATURE);
				out.write_u32(tag);
				out.write_u32(data_transfer_length);
				out.write_u8(flags);
				out.write_u8(lun);
				out.write_u8(cb_length);
			};
	};

	class SCSIInquiryCommand : public SCSICommand
	{
		public:
			uint8_t allocation_length;
			uint8_t opcode;

			SCSIInquiryCommand(uint8_t allocation_length_) : SCSICommand(allocation_length_, SCSIDirection::In, 0, /* length */ 5)
			{
				opcode = 0x12;
				allocation_length = allocation_length_;
			}

			virtual void to_bytes(uint8_t *out)
			{
				SCSIBuffer b;
				write_header(b);
				b.write_u8(opcode);
				b.padding(3);
				b.write_u8(allocation_length);

				memcpy(out, b.storage, 31);
			}
	};


	class SCSITestUnitReadyCommand : public SCSICommand
	{
		public:
			uint8_t opcode;
			
			SCSITestUnitReadyCommand() : SCSICommand(0, SCSIDirection::None, 0, /* length */ 0x6)
			{
				opcode = 0;
			}

			virtual void to_bytes(uint8_t *out)
			{
				SCSIBuffer b;
				write_header(b);
				b.write_u8(opcode);

				memcpy(out, b.storage, 31);
			}
	};


	class SCSIReadCapacityCommand : public SCSICommand
	{
		public:
			uint8_t opcode;
			
			SCSIReadCapacityCommand() : SCSICommand(0x8, SCSIDirection::In, 0, /* length */ 0x10)
			{
				opcode = 0x25;
			}

			virtual void to_bytes(uint8_t *out)
			{
				SCSIBuffer b;
				write_header(b);
				b.write_u8(opcode);

				memcpy(out, b.storage, 31);
			}
	};


	class SCSIRead10Command : public SCSICommand
	{
		public:
			uint8_t opcode;
			uint32_t block_address;
			uint16_t transfer_blocks;
			
			SCSIRead10Command(uint32_t block_address_, uint32_t block_size, uint16_t transfer_blocks_) : SCSICommand(transfer_blocks_ * block_size, SCSIDirection::In, 0, /* length */ 10)
			{
				opcode = 0x28;
				block_address = block_address_;
				transfer_blocks = transfer_blocks_;
			}

			virtual void to_bytes(uint8_t *out)
			{
				SCSIBuffer b;
				write_header(b);
				b.write_u8(opcode);
				b.padding(1);
				b.write_u32_be(block_address);
				b.padding(1);
				b.write_u16_be(transfer_blocks);
				b.padding(1);

				memcpy(out, b.storage, 31);
			}
	};

	class SCSIWrite10Command : public SCSICommand
	{
		public:
			uint8_t opcode;
			uint32_t block_address;
			uint16_t transfer_blocks;

			SCSIWrite10Command(uint32_t block_address_, uint32_t block_size, uint16_t transfer_blocks_) : SCSICommand(transfer_blocks_ * block_size, SCSIDirection::Out, 0, /* length */ 10)
			{
				opcode = 0x2a;
				block_address = block_address_;
				transfer_blocks = transfer_blocks_;
			}

			virtual void to_bytes(uint8_t *out)
			{
				SCSIBuffer b;
				write_header(b);
				b.write_u8(opcode);
				b.padding(1);
				b.write_u32_be(block_address);
				b.padding(1);
				b.write_u16_be(transfer_blocks);
				b.padding(1);

				memcpy(out, b.storage, 31);
			}
	};


	class SCSICommandStatus
	{
		public:
			uint32_t signature;
			uint32_t tag;
			uint32_t data_residue;
			uint8_t status;

			SCSICommandStatus(){}
			SCSICommandStatus(uint8_t csw[13])
			{
				memcpy(&signature, &csw[0], 4);
				memcpy(&tag, &csw[4], 4);
				memcpy(&data_residue, &csw[8], 4);
				memcpy(&status, &csw[12], 1);
			}
	};
}