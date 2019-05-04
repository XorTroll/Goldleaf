#include <string.h>
#include <switch.h>
#include <gleaf/drive/SCSIContext.hpp>

namespace gleaf::drive
{
    SCSIBlock::SCSIBlock(SCSIDevice *device_)
    {
        device = device_;
        SCSIInquiryCommand inquiry(36);
        SCSITestUnitReadyCommand test_unit_ready;
        SCSIReadCapacityCommand read_capacity;
        uint8_t inquiry_response[36];
        SCSICommandStatus status = device->transfer_cmd(&inquiry, inquiry_response, 36);
        status = device->transfer_cmd(&test_unit_ready, NULL, 0);
        uint8_t read_capacity_response[8];
        uint32_t size_lba;
        uint32_t lba_bytes;
        status = device->transfer_cmd(&read_capacity, read_capacity_response, 8);
        memcpy(&size_lba, &read_capacity_response[0], 4);
        size_lba = __builtin_bswap32(size_lba);
        memcpy(&lba_bytes, &read_capacity_response[4], 4);
        lba_bytes = __builtin_bswap32(lba_bytes);
        capacity = size_lba * lba_bytes;
        block_size = lba_bytes;
        uint8_t mbr[0x200];
        read_sectors(mbr, 0, 1);
        partition_infos[0] = MBRPartition::from_bytes(&mbr[0x1be]);
        partition_infos[1] = MBRPartition::from_bytes(&mbr[0x1ce]);
        partition_infos[2] = MBRPartition::from_bytes(&mbr[0x1de]);
        partition_infos[3] = MBRPartition::from_bytes(&mbr[0x1ee]);
        for(int i = 0; i < 4; i++)
        {
            MBRPartition p = partition_infos[i];
            if(p.partition_type != 0)
            {
                partitions[i] = SCSIBlockPartition(this, p);
            }
        }
    }

    int SCSIBlock::read_sectors(uint8_t *buffer, uint32_t sector_offset, uint32_t num_sectors)
    {
        SCSIRead10Command read_ten(sector_offset, block_size, num_sectors);
        SCSICommandStatus status = device->transfer_cmd(&read_ten, buffer, num_sectors * block_size);
        return num_sectors;
    }

    int SCSIBlock::write_sectors(const uint8_t *buffer, uint32_t sector_offset, uint32_t num_sectors)
    {
        SCSIWrite10Command write_ten(sector_offset, block_size, num_sectors);
        SCSICommandStatus status = device->transfer_cmd(&write_ten, (uint8_t*)buffer, num_sectors * block_size);
        return num_sectors;
    }

    SCSIBlockPartition::SCSIBlockPartition(SCSIBlock *block_, MBRPartition partition_info)
    {
        block = block_;
        start_lba_offset = partition_info.start_lba;
        lba_size = partition_info.num_sectors;	
    }

    int SCSIBlockPartition::read_sectors(uint8_t *buffer, uint32_t sector_offset, uint32_t num_sectors)
    {
        // TODO: assert we don't read outside the partition
        return block->read_sectors(buffer, sector_offset + start_lba_offset, num_sectors);
    }

    int SCSIBlockPartition::write_sectors(const uint8_t *buffer, uint32_t sector_offset, uint32_t num_sectors)
    {
        // TODO: assert we don't write (!!) outside the partition
        return block->write_sectors(buffer, sector_offset + start_lba_offset, num_sectors);
    }

}