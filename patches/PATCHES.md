Installation
============

You will need to dump the FS.kip1 file specific to your firmware version and whether or not you are using ExFAT.

After you have done this, you must choose the IPS patch file for your specific system version and ExFAT install status. You must then use a tool such as [Lunar IPS](https://www.romhacking.net/utilities/240/) to apply the patch to your FS.kip1.

NOTE: Not all patches are available at this time.

Finally, you need to place the file on your Switch SD card, and add it to the hekate_ipl.ini file along with all of the other kip1s.

NCA Fixed Key Sig Patches
=========================

## 5.1.0 Patches

### ExFAT Variant
| Name | Value |
| - | - |
| Offset | 7D860 |
| Original Value | 40 03 00 36 |
| Original Instruction | TBZ W0, #0, loc_710007D8C8 |
| New Value | 1F 20 03 D5 |
| New Instruction | NOP |

Credits
=======
* **thomasnet** - For assisting with finding the original patch, and helping with other development.