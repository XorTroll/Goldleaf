# Master

- Added option to show USB speed

# `v1.2.0`

- Updated with latest libnx, supporting (at least) up to firmware 21.1.0

- Fixed an issue where Goldleaf would crash after selecting (or having the system in) Russian and Taiwanese languages

- Fixed an issue where Goldleaf would inmediately crash if the user SAFE partition was not mountable (now such partitions are not mounted right on startup)

- Installation procedure was internally changed, so that contents are written before any records are modified (so that, if any problems happens, only already-written contents need to be removed and no records got to change)

- Fixed minor mistake in key generation detection code

- The "custom language" dialog now shows actual language names, not just language codes (which were less readable)

- Fixed an issue where Goldleaf would incorrectly access PC paths (accessing the one listed below instead of the actual selected one)

- Added a config option to change the speed of navigating menus with the joysticks

- Fixed an old dump replace-all bug where Korean strings would say "g_Clipboard" instead of "clipboard"

- Added an option to automatically skip already installed NSPs from bulk NSP installations
  
- Added the option to dump amiibos even if they haven't been initialized yet (by the user providing an amiibo name and Mii)

- Added support for Latin American Spanish language

- Updated Korean and Spanish translations

# `v1.1.1`

- Changed the JSON libraries used internally (should slightly improve performance)

  - The `export` settings field was renamed to `exports` for convenience

- Reverted default config changes to use `8MB` for copy and install buffers (the previous `16MB` default config would make USB freeze)

- The explore-content menu was slightly redesigned by merging USB drives in the main explorer menu: new (removed) inserted USB drives will automatically (dis)appear

- Animations were made slightly faster

- Fixed a bug where the firmware key generation would be not read properly on Mariko models

- Fixed UI bugs where several menu info/icons were not properly hid when moving to other menus

- Fixed a minor bug when parsing pending update versions

- Additionally mounted filesystems are properly removed from the menu now

- Fixed a bug where deleting an entire program would not be reflected in the menu (it would still show up)

- Fixed a major bug where (sometimes?) some contents would not be deleted properly (I specially apologize for this one, further testing of the last version would have shown this behavior quickly)

- Updated Korean and Spanish translations

- Added an option to clear Goldleaf logs from the app itself (mostly for debugging/testing things, maybe someone else other than me will find this useful)

## Quark

- Improved support for Home directory in Linux, and also added it for any supported operating systems

- Improved support for drives in Linux

- Fixed minor bug where the drives would be re-scanned multiple times

# `v1.1.0`

## General

- Updated for latest Plutonium (UI library):

  - When cancelling a dialog, the "cancel" option is focused before closing (to also indicate visually it was cancelled)

  - Default resolution is always 1080p internally (see consequences below)

## Goldleaf

- Fixed an issue where custom background colors were not properly parsed by Goldleaf

- Goldleaf is 1080p now! Various relevant UI changes come in this release:

  - The color scheme is no longer randomly generated at startup: there is now a default blue light/dark color scheme (although you can still customize it yourself) along with the project's gold color

  - New icons are used everywhere (since I had to find new ones that had good resolutions)

- Goldleaf's settings have (slightly) changed: setting names are (slightly) different now, and the color customization is done in a more organized way (check the [README](README.md) for all details)

- The memory/space menu has been reorganized, taking up space in a more visually appealing way

- Too-long texts are clamped now, so that they don't protrude outside their expected bounds

- Now directory sizes are only computed if a setting (disabled by default) is enabled, since it lead to slowdowns when selecting large directories for other purposes.

- When exporting/dumping contents, names containing invalid FS characters (":", "/" or "\") are now replaced to avoid issues when creating the exported NSP.

- Removed the old crash handling system, which worked half the time and made things annoying: crashes are handled like regular homebrew (with the usual system crash-screen), and detailed information about what caused the crash is logged in Goldleaf's log file

- Icons are now internally shared resources (previously, for each menu item/place it was used, a new icon copy was loaded in memory), which should reduce the memory footprint and somewhat improve performance

- Added confirmation dialogs to the power-off/reboot dialog, to avoid unwanted ZL/ZR presses causing unwanted shutdowns/reboots

- Using the web browser is no longer allowed unless Goldleaf is launched as an application (since otherwise it won't work and just cause a crash)

- Folders have now two different icons, visually distinguishing between empty and non-empty directories

- Fixed a bug where the message after exporting ticket/cert was not shown at all (an empty, long box was only displayed)

- The "image" menu icon is now shown also for PNG/WEBP images (even though they cannot be set as user icons)

- Updated key generation recognition up to the latest `20.0.0`/`20.0.1` firmwares

- For used tickets, the game title is displayed instead of the application ID on the ticket menu items (the ID can still be viewed by selecting a given ticket)

- Added special handling when removing an account that is also linked (removing requires it to be unlinked first, this is now done automatically)

- Added special result description messages for dedicated result codes (those with no previous description, or those where the base description was not really informative to users)

- Fixed ETA calculation when copying directories (which would display nonsense when copying multiple small files) so that it displays somewhat coherent values

- The user icon is exported again after changing it

- A bug was fixed where logging wouldn't happen correctly in certain menus

- Mounted filesystems are now properly disposed when exiting Goldleaf

- Pressing Y when a file is focused in the file browser will also display its options (like pressing A, so Y results in the same behaviour for files and directories)

- The previous content system has been completely replaced by a brand-new game list menu:

  - This no longer considers/lists NAND-System contents, as that was a mostly useless (and slow) feature

  - Slowness when loading contents has been reduced by using lazy-loading threaded code and caching many things

  - Games and their contents are directly browsed now, instead of having to choose storages

  - Useful information for debugging/dev purposes is displayed now as well (last record event and view flags)

  - Size computing is more simple and straightforward now

  - Save data mounting was fixed for games which use device savedata instead of per-user savedata (like Animal Crossing: New Horizons)

  - If no user was yet selected, trying to mount the save data will directly prompt to select a user, and mount it if succeeded

- The tickets menu was moved to the main menu

- The update menu was merged with the about menu: you can now check updates by pressing A on the about page

- Fixed a dump replace-all bug where english strings would say "g_Clipboard" instead of "clipboard"

- Ticket information now also shows the key generation version range (not only on NSP installation info)

- Empty *.bin files can no longer be launched (or rather, Goldleaf attempting to launch them)

- Made transitions and movement in menus slightly faster (for convenience) and with more icons/dedicated messages for each menu

- The text viewer was slightly improved, allowing to also displace horizontally to read long lines, and only rendering the visible part of each line

# `v1.0.0`

## General

- Some minor rewrites (in a probably unnoticeable way) improved the internals of the project

- As always, all libraries were updated to their latest versions

## Goldleaf

- NSP installs have been (finally) made multithreaded - I've personally noticed up to 25% faster install times, but this speedups might be even greater ;)

- Proper support for multi-program titles was added (for installing, browsing and exporting them correctly)

- Switched to an alternative result system via *arc*, so that now more system results have proper names/descriptions for more readability

- Improved various internal parts via caching or general optimizations

- Added support to check titles with no data (archived content, gamecard titles without the gamecard inserted, actually corrupted content, etc.)

- Extended keyboard support, now supporting other kinds of characters (particularly fixed support for account renaming)

- Fixed an annoying ticket/cert exporting bug which caused invalid NSP exports being generated

- Now one can mount and explore homebrew NROs' RomFs

- Updated some language translations (thanks a lot to all the people who frequently help with that <3)

- Added support for 12h/24h time display and configuration

- Support for NXThemes was removed

- Added a setting to allow custom NSP installation copy buffer sizes

- Slightly improved random UI color generation

- Fixed menu scrollbars behaving incorrectly in menus with too many items

- Fixed a bug where one could exit Goldleaf in the middle of installing NSPs, copying files, etc.

- Improved the file/directory copy menu and content export menu UIs

- Added a setting to disable showing hidden files/directories (those starting by '.' in their name)

- Now showing a deletion dialog after a NSP install is disabled by default, and a setting was added to enable/disable it

## Quark

- Fixed a bug where, when successfully reconnecting to Goldleaf after a connection loss, Quark would regardless close