
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

    Credits to Stary for the base of USB-drive support!

*/

#pragma once
#include <gleaf/drive/SCSICommand.hpp>
#include <malloc.h>

namespace gleaf::drive
{
	class SCSIDevice
	{
		public:
			uint8_t *usb_bounce_buffer_a;
			uint8_t *usb_bounce_buffer_b;
			uint8_t *usb_bounce_buffer_c;
			UsbHsClientIfSession *client;
			UsbHsClientEpSession *in_endpoint;
			UsbHsClientEpSession *out_endpoint;

			SCSIDevice(){}
			SCSIDevice(UsbHsClientIfSession *client_, UsbHsClientEpSession *in_endpoint_, UsbHsClientEpSession *out_endpoint_)
			{
				usb_bounce_buffer_a = (uint8_t*) memalign(0x1000, 0x1000);
				usb_bounce_buffer_b = (uint8_t*) memalign(0x1000, 0x1000);
				usb_bounce_buffer_c = (uint8_t*) memalign(0x1000, 0x1000);
				client = client_;
				in_endpoint = in_endpoint_;
				out_endpoint = out_endpoint_;
			}

			SCSICommandStatus read_csw();
			void push_cmd(SCSICommand *c);
			SCSICommandStatus transfer_cmd(SCSICommand *c, uint8_t *buffer, size_t buffer_size);
	};


	class MBRPartition
	{
		public:
			uint8_t active;
			uint8_t partition_type;
			uint32_t start_lba;
			uint32_t num_sectors;

			static MBRPartition from_bytes(uint8_t *entry)
			{
				MBRPartition p;
				p.active = entry[0];
				// 1 - 3 are chs start, skip them
				p.partition_type = entry[4];
				// 5 - 7 are chs end, skip them
				memcpy(&p.start_lba, &entry[8], 4);
				memcpy(&p.num_sectors, &entry[12], 4);
				return p;
			}
	};

	class SCSIBlock;

	class SCSIBlockPartition
	{
		public:
			SCSIBlock *block;
			uint32_t start_lba_offset;
			uint32_t lba_size;
			SCSIBlockPartition() {}
			SCSIBlockPartition(SCSIBlock *block_, MBRPartition partition_info);
			int read_sectors(uint8_t *buffer, uint32_t sector_offset, uint32_t num_sectors);
			int write_sectors(const uint8_t *buffer, uint32_t sector_offset, uint32_t num_sectors);
	};

	class SCSIBlock
	{
		public:
			uint64_t capacity;
			uint32_t block_size;

			SCSIBlockPartition partitions[4];
			MBRPartition partition_infos[4];

			SCSIDevice *device;

			SCSIBlock(){}
			SCSIBlock(SCSIDevice *device_);
			int read_sectors(uint8_t *buffer, uint32_t sector_offset, uint32_t num_sectors);
			int write_sectors(const uint8_t *buffer, uint32_t sector_offset, uint32_t num_sectors);
	};
}