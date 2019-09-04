# Changelog

- Remote PC

  - Unicode is now supported. No more complaints with special characters in drive labels or folders!

  - Read size was dropped again to 8MB (from previous 16MB), to make USB more stable (16MB size might cause unstability)

  - Added option to select a file directly on the remote PC tool (basically same system as on <=0.5 Goldleaf versions, for those who like the classic system!)

  - Goldtree's messages were slightly changed to be more accurate. Now it shows its version to avoid confusion between different executables.

- NSP installs

  - The NSP's name is shown above (instead of the directory).

  - ETA (estimated time for the install to finish) is now shown too.

  - Now Goldleaf always obtains the NSP's key generation (previously only when it had a ticket) and compares it to a computed system one, in order to block installs with higher key generations (mostly since the contents couldn't be decryptable, thus utterly useless)

  - Now install location is just chosen once, at the start of batch NSP installs (all NSPs within a directory)

  - Multi-content installations (just a few titles currently) work now (didn't seem to work on 0.6.x)

- Title dump / export

  - Titles are always dumped inside concatenation files (HOS's archive bit directories, used to support +4GB files), what means +4GB titles can safely be dumped on FAT32, and directly installed.

  - Fixed errors dumping updates and DLCs. They should work fine now.

- General

  - A big part of the code was reformatted: several small bugs fixed, others not that small mentioned below.

  - Added a warning and success / failure messages for directory deleting.

  - Goldleaf no longer relies on its custom libnx fork, since its only worthy difference was its (slightly) custom USB code, which has been merged in Goldleaf. This doesn't involve any performance changes, but less complexity in order to build the project.

  - Copying files from PC to SD which are over 4GB is now correctly handled, creating a concatenation file (see above) to correctly handle it.

  - The forwarder was updated with hbloader's latest version, thus now the forwarder's version is 0.2.

- User / accounts

  - Users have a more important role in Goldleaf. Now an account has to be selected in order to use several user-related features.

  - Added support for unlinking **(locally ONLY)** accounts, mainly for banned people who want to unlink their accounts in order to delete them.

- Content manager

  - Fixed a bug where the base title's size would be shown always, even when checking only updates or DLC.

  - Added play statistics support, both for user-only (user must be selected first, see above) and global, this includes last played time and total play time for both.

  - Fixed bugs where contents whose base title is on a different location than updates/DLCs would be shown incorrecly.

- UI

  - Unicode changes stated above

  - The system's shared font is no longer used, due to its lack of unicode characters. Instead, a new font is used from now on: Roboto (medium), aka Android's standard font.

  - Two new clickable icons have been added to the bottom right side of Goldleaf's top banner: one for user selection, while the other one for help and control information.

  - Some small corrections made: size adjustments, with no connection no IP or default text will be shown from now on.

  - Plutonium corrections with newlines and text sizes make dialog sizes more well measured now (previously, a single newline would make the dialog as wide as the entire screen)

  - Several icon changes (forwarder's icon and amiibo dump), thanks to **C4Phoenix** for all his help with graphic resources since 0.1!

  - Goldleaf now has been compiled with an experimental Plutonium branch which uses smart pointers instead of raw pointers aiming no memory leaks!

  - Now, Goldleaf also supports dutch language! Thanks a lot to **Mega** for helping with the translation!

## Known bugs

- Browsing (fast?) over certain (empty?) folders can sometimes have weird behavior, resulting into crashes.