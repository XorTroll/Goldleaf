Installation
============

You will need: 
 - **kip1decomp** from https://files.sshnuke.net/kip1decomp.exe. Alternatively compile yourself from https://github.com/rajkosto/memloader (x86 only)
 * **xdelta3** which is included with https://sourceforge.net/projects/xdelta3-gui/ (and possibly other places too)

Use hactool to get your own FS.kip1 from the INI1 file located in package2. It is important this matches your console, it may or may not use the ExFat variant of FS.kip1.

Download the appropriate patch file from this folder, applicable to your firmware version and whether you are using the ExFAT FS.kip1.

Next run the following commands, replacing FS_PATCH_FILENAME.vcdiff with the actual filename for your version

   ```
   kip1decomp d FS.kip1 FS.decomp.kip1
   xdelta3 -d -f -s FS.decomp.kip1 FS_PATCH_FILENAME.vcdiff FS.patched.kip1 <- Errors here means the patch doesn't match your FS.kip1
   kip1decomp c FS.patched.kip1 FS.kip1
   ```

Place the new FS.kip1 onto the root of your microSD, as well as adding the following line to your hekate_ipl.ini launch configuration:
```
kip1=FS.kip1
```

**DISCLAIMER:**

I take zero responsibility for any bans, damage, nuclear explosions, or anything else these patches may cause. Proceed with caution!

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

Disable Program Verification
============================

## 5.1.0 Patches

### ExFAT Variant
| Name | Value |
| - | - |
| Offset | 32820 |
| Original Value | A8 0E 00 90 00 81 64 39 |
| Original Instructions | ADRP X8, #byte_7100206920@PAGE |
| | LDRB W0, [X8,#byte_7100206920@PAGEOFF] |
| New Value | 1F 20 03 D5 E0 03 1F 2A |
| New Instructions | NOP 
| | MOV W0, WZR |

Credits
=======
* **thomasnet** - For assisting with finding the original patch, and helping with other development.
* **rajkosto** - For letting me reuse his patching guide