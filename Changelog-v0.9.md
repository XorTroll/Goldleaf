# Changelog

## General

- Simplified directory deleting logic for both Goldleaf and Quark, making everything less slower (thanks @pheki)

## Goldleaf

- Introducing savedata browsing! select a game from the game manager menu, and mount it's savedata. Note that, if you forget to unmount them manually, they will be automatically unmounted when Goldleaf is exited

- Support for firmware installing, via Atmosphere's ams:su service extension!

- Now fatal crashes are avoided, and Goldleaf will just display an error message via the console's error applet, and save its own report with information about the crash/error.

- Simplified and cleaned a lot of internal code, hopefully making everything more stable

- Updated libnx and Plutonium - this implies support for previously unsupported special text (Japanese and Chinese characters, etc.) with last Plutonium

- Removed emuiibo manager - emuiibo's overlay is now the optimal and recommended manager for emuiibo

- Amiibo dumping support was updated to match emuiibo v0.5.x's format

- Non-lowercase file extensions (.TXT, .biN, .Nsp, etc.) are now properly supported

- Fixed resetting a title's launch version, which wasn't properly implemented on previous releases

- Now dumped firmware will be compatible with Atmosphere's Daybreak firmware tool, deprecating Choi support (as Daybreak is a better and safer alternative)

- Removed support for dumping firmware as NSP packages, as no relevent firmware tool makes use of that format

- Reboot-to-payload functionality (which can be used with .bin files) has been internally improved with Atmosphere's bpc:ams extensions

- 🌈 Random color schemes/pallets are used by default now, instead of default light/dark blue schemes

- Fixed a small bug where no personalized tickets would be detected if no common tickets were present in the console

- Introducing USB drive support, thanks to me and DarkMatterCore's new project: [libusbhsfs](https://github.com/DarkMatterCore/libusbhsfs), a library to include USB drive support in your homebrew projects!

  - Both FAT (including exFAT) and NTFS filesystems are supported ;)

- Other minor code improvements or small bug fixes

## Quark

- Several improvements related to various small errors

- From now on, two different builds will be compiled/released: one for Java 8 and one for Java 9 or higher, in order to avoid version issues