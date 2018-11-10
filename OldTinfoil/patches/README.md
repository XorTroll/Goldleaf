Installation
============

You will need: 
 - **ChoiDujour 1.1.0+** from https://switchtools.sshnuke.net/
 - A dump of your own SYSTEM:/Contents/registered folder. This can be obtained using HacDiskMount with a NAND dump, or from your actual console by also using memloader.

Run the following command, replacing ``FIRMWARE_FOLDER`` with the path to your own firmware folder. If you are *not* using ExFAT, you will also need to add ``--noexfat``

```
ChoiDujour.exe --keyset=path/to/keys.txt --fspatches=nocmac,nosigchk FIRMWARE_FOLDER
```

Grab the file with the name ``FS<FIRMWARE_VERSION>-exfat_nocmac_nosigchk.kip1`` from the ``microSD`` folder inside ChoiDujour's output folder (should be named something like ``NX-<FIRMWARE_VERSION>[_exfat]``)

Place the newly generated file onto the root of your microSD, and add the following line to your hekate_ipl.ini launch configuration:
```
kip1=FS<FIRMWARE_VERSION>-exfat_nocmac_nosigchk.kip1
```
Patch Source
============

The source for all FS patches can be found at https://switchtools.sshnuke.net/firmware/fs_patches.json

Credits
=======
* **rajkosto** - For creating patches for all firmware versions and integrating them into Choi Dujour.
* **thomasnet** - For assisting with finding the original patch, and helping with other development.

Disclaimer
==========

I take zero responsibility for any bans, damage, nuclear explosions, or anything else these patches may cause. Proceed with caution!

Legacy 5.1.0 ExFAT Patches
==================

### NCA Fixed Key Sig Patches

| Name | Value |
| - | - |
| Offset | 7D860 |
| Original Value | 40 03 00 36 |
| Original Instruction | TBZ W0, #0, loc_710007D8C8 |
| New Value | 1F 20 03 D5 |
| New Instruction | NOP |

### Disable Program Verification

| Name | Value |
| - | - |
| Offset | 32820 |
| Original Value | A8 0E 00 90 00 81 64 39 |
| Original Instructions | ADRP X8, #byte_7100206920@PAGE |
| | LDRB W0, [X8,#byte_7100206920@PAGEOFF] |
| New Value | 1F 20 03 D5 E0 03 1F 2A |
| New Instructions | NOP 
| | MOV W0, WZR |

