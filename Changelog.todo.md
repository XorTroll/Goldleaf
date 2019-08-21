# Changelog

- Remote PC

  - Unicode is now supported. No more complaints with special characters in drive labels or folders!

  - Read size was dropped again to 8MB (from previous 16MB), to make USB more stable (16MB size might cause unstability)

- NSP installs

  - The NSP's name is shown above (instead of the directory).

  - ETA (estimated time for the install to finish) is now shown too.

- Title dump / export

  - Titles are always dumped inside concatenation files (HOS's archive bit directories, used to support +4GB files), what means +4GB titles can safely be dumped on FAT32, and directly installed.

  - Fixed errors dumping updates and DLCs. They should work fine now.

- General

  - A big part of the code was reformatted: several small bugs fixed, others not that small mentioned below.

  - Added a warning and success / failure messages for directory deleting.

  - Goldleaf no longer relies on its custom libnx fork, since its only worthy difference was its (slightly) custom USB code, which has been merged in Goldleaf. This doesn't involve eny performance changes, but less complexity in order to build the project.

  - Copying files from PC to SD which are over 4GB is now correctly handled, creating a concatenation file (see above) to correctly handle it.

  - Goldleaf takes advantage now from hbloader v2.2.0's heap reservation, which reserved 128MB of heap for other applets. Goldleaf probably won't work without this trick, so its support is uncertain with old Atmosphere or other CFWs.

  - The forwarder was updated with hbloader's latest code.

- User / accounts

  - Users have a more important role in Goldleaf. Now an account has to be selected in order to use several user-related features.

  - Added support for unlinking **(locally ONLY)** accounts, mainly for banned people who want to unlink their accounts in order to delete them.

- Content manager

  - Fixed a bug where the base title's size would be shown always, even when checking only updates or DLC.

  - Added play statistics support, both for user-only (user must be selected first, see above) and global, this includes last played time and total play time for both.

- UI

  - Unicode changes stated above

  - The system's shared font is no longer used, due to its lack of unicode characters. Instead, a new font is used from now on: Roboto (medium), aka Android's standard font.

  - Two new clickable icons have been added to the bottom right side of Goldleaf's top banner: one for user selection, while the other one for help and control information.

  - Some small corrections made: size adjustments, with no connection no IP or default text will be shown from now on.

  - Plutonium corrections with newlines and text sizes make dialog sizes more well measured now (previously, a single newline would make the dialog as wide as the entire screen)

  - Several icon changes (forwarder's icon and amiibo dump), thanks to **C4Phoenix** for all his help with graphic resources since 0.1!

  - Now, Goldleaf also supports dutch! Thanks a lot to **Mega** for helping with the translation!