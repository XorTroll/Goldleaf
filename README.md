![Logo](Goldleaf/RomFs/Logo.png)

![License](https://img.shields.io/badge/License-GPLv3-blue.svg) [![Releases](https://img.shields.io/github/downloads/xortroll/goldleaf/total.svg)]() [![LatestVer](https://img.shields.io/github/release-pre/xortroll/goldleaf.svg)]()

> Goldleaf is multipurpose homebrew tool for Nintendo Switch.

# Table of contents

1. [Features](#features)
2. [Disclaimer](#disclaimer)
3. [Forwarding](#forwarding)
4. [Configuration](#configuration)
5. [Known bugs](#known-bugs)
6. [Credits and support](#credits-and-support)

## **[Download it from here!](https://github.com/XorTroll/Goldleaf/releases)**

<p>
<a href="https://www.patreon.com/xortroll"><img src="https://c5.patreon.com/external/logo/become_a_patron_button@2x.png" height="50" width="220"></a>
<br>
<b>If you like my work, consider becoming a patron and supporting it!</b>
</p>

## Features

Goldleaf has a very similar concept to [FBI](https://github.com/Steveice10/FBI), which is a similar tool for Nintendo 3DS homebrew.

These are its main features:

- **SD card browser**:  A simple but fully equiped file browser for the SD card, supporting several file formats (such as NSP, NRO, NACP, NXTheme, JPEG, tickets...), and with support for copying, pasting, renaming, deleting and creating files and directories.

- **Console memory browser**: Same browser but to be used with NAND partitions, which also warns when writing or deleting content as it can be dangerous.

- **USB installer**: Via USB connection, you can send NSPs from **Goldtree** or other PC clients and install them into your console. Keep in mind that this feature is unstable, and might face bugs and issues.

- **Content manager**: Browse any kind of content in your SD card, console memory or game cartridge. You can browse all the NCAs, and as title options, you can remove the content, or export it as a NSP. NAND SYSTEM contents (system titles or contents) aren't allowed to be deleted. Invidual contents such as updates or DLC can be removed individually.

- **Ticket manager**: Browse all tickets, both common and personalized. You can remove them, but an extra warning will be displayed when attempting to remove tickets being used by some content.

- **Internet via web applets**: Using the console's web applets, Goldleaf allows to search and navigate through web pages. If Goldleaf is launched through hbmenu as a NRO, WifiWebAuth applet will be used, which tends to fail sometimes, along with fewer features, such as lack of video support.

- **User account manager**: Select a user account, and easily change things as the user's nickname or even delete it. Via the file browsers you can replace a user's icon with a JPEG image, but it can be a risky action. The icon needs to be a valid JPEG, with 256x256 dimensions. Goldleaf checks if the icon is valid to replace the other one.

- **Console information**: Simple menu to see current firmware string, and the total free and occupied size on SD card and console memory.

Among all this features, Goldleaf is a very intuitive tool, with a fancy UI, and fully translated into 5 different languages: **English**, **Spanish**, **German**, **French** and **Italian**.

### Common controls

- **(L-stick / D-pad)** Move through the menu

- **(R-stick)** Move (very) fast through the menu

- **(ZL / ZR)** Show a dialog to shut down or reboot the console.

- **(+ / -)** [NRO only] exit the application in order to return to hbmenu.

## Disclaimer

### NSP installs

Always keep in mind that installing NSPs can be a very easy way to get banned. If you use airplane mode and special DNSs like 90DNS, there is a smaller chance to get banned.

NEVER install untrusted NSPs. Goldleaf can identify (according to the NSP's application ID) whether the NSP is a homebrew NSP (IDs starting with "05", usually for forwarders), or a regular title (official ID range just allows IDs starting with "01")

### Tickets

Tickets represent a game purchase, but technically speaking, you can't boot a title if the ticket isn't present (in case the title requires the ticket).

NSPs which don't have (nor require) a ticket are "standard crypto" NSPs. Standard-crypto titles aren't offical (usually reconverted content to avoid importing tickets).

Goldleaf will display whether a ticket is present or not, and if so, it's titlekey. Titlekeys are required to decrypt and access titles' content which require a ticket. Otherwise, they are undecryptable, hence unlaunchable by the system.

## Forwarding

> **IMPORTANT!** *This is going to be part of 0.6 and following releases. This information doesn't apply for 0.5 or pre0.5 versions of Goldleaf.*

Goldleaf itself is a regular homebrew project, thus a simple NRO. However, via the forwarding projects' system, you can launch Goldleaf as an installed title (installing a forwarder NSP), or even replace HOME menu!

### Forwarder projects

The forwarding process is common and simple: locate Goldleaf's NRO, launch it via argc/argv. These are the forwarders available:

- **Normal forwarder**: Locates and launches normally Goldleaf NRO. Consists on an installable NSP, whose ApplicationId (aka TitleID) is `050032A5CF12E000`.

- **hbmenu(ish) forwarder**: Locates and launches Goldleaf NRO, but in hbmenu mode, which will make Goldleaf display a different initial menu, UI... made for homebrew NRO browsing and launching. This one consists on an installable NSP, whose ApplicationId (aka TitleID) is `0500E83A507FE000`, and on a single NRO which can be replaced with *hbmenu.nro* to use Goldleaf as hbmenu instead of the common one.

- **HOME menu forwarder (LFS replacement)**: This one is a simple ExeFs replacement, to be applied to the CFW being used, which will locate Goldleaf's NRO and start it. For the moment Goldleaf will display the same UI, but soon a special UI will be made for this mode, like hbmenu forwarder's one.

## Configuration

Goldleaf supports the customization of colors, assets and other options via a INI file located at Goldleaf's folder: `sd:/goldleaf/goldleaf.ini`.

### Options

These are the options supported on the INI file (not found boolean options are defaulted to false):

| Section | Option               | Description                                                                          |
|---------|----------------------|--------------------------------------------------------------------------------------|
| General | useCustomLanguage    | { true, false }  If not true,'customLanguage' will be ignored.                       |
| General | customLanguage       | { en, es, de, fr, it } Ignores the console's language and use this one.              |
| General | keysPath             | { (path starting with "/") } Key file path, defaults to "/switch/prod.keys".         |
| UI      | romfsReplace         | { true, false } If not true, 'romfsReplacePath' will be ignored.                     |
| UI      | romfsReplacePath     | { (path starting with "/") } Path to search RomFs resources, if it's a valid one.    |
| UI      | useCustomColors      | { true, false } If not true, colors' options will be ignored.                        |
| UI      | colorBackground      | { (color in 4 bytes, example: "55,125,255,255") } Background color.                  |
| UI      | colorBase            | { (color in 4 bytes, example: "55,125,255,255") } Base display color.                |
| UI      | colorBaseFocus       | { (color in 4 bytes, example: "55,125,255,255") } Base display  color, when focused. |
| UI      | colorText            | { (color in 4 bytes, example: "55,125,255,255") } Text color.                        |
| UI      | useCustomSizes       | { true, false } If not true, sizes' options will be ignored.                         |
| UI      | fileBrowserItemsSize | { (number, divisible by 5) } Size of the items on file browsers, 50 by default.      |

### Notes

Via RomFs replacement, when Goldleaf tries to locate, for instance, `romfs:/FileSystem/FileDataFont.ttf` resource font, if `romfsReplace` is true and `romfsReplacePath` is, for instance, `/goldleaf/testromfs`, Goldleaf will look if `sd:/goldleaf/testromfs/FileSystem/FileDataFont.ttf` exists and use it if so, otherwise will use RomFs's one.

So, via this configurations, UI's images, resources, element sizes and even translations (using custom JSON translations) can be used, plus some more assets which will be added in future updates.

## Known bugs

- On Atmosphère and SX OS, exiting Goldleaf via HOME menu (as a NRO) seems to crash the system on 7.x firmwares.

- USB installation in general seems to have several bugs, most of them related to the UI system.

## Credits and support

### Credits

The main concepts of Goldleaf are and have been developed by XorTroll, but without the effort and support of many others, this project wouldn't be what it is now:

(from all the people listed below, special thanks to Adubbz, exelix, C4Phoenix, The-4n and SciresM, for their huge support on their respective areas of homebrew)

- Adubbz and all the (old) [Tinfoil](https://github.com/Adubbz/Tinfoil) contributors, for their huge work with title installing.

- exelix and Qcean team, for all their huge support with Home Menu themes. Goldleaf uses (adapted) [SwitchThemesCommon](https://github.com/exelix11/SwitchThemeInjector) libraries to handle theme installs.

- C4Phoenix, for his awesome work doing this project's logo, and the GIF displayed when launching the installed version.

- All the icons except Goldleaf's one (see credit above) were grabbed from [Icons8](https://icons8.com).

- 2767mr, for all the support for making USB installs more stable, and the work-in-progress .NET core version of Goldtree.

- The-4n, for [hacBrewPack](https://github.com/The-4n/hacBrewPack), to make completely legal NSPs.

- SciresM for [hactool](https://github.com/SciresM/hactool), which was ported as a library to make NCA extraction a thing in Goldleaf.

- Thealexbarney, for his C# libraries for various Nintendo Switch formats: LibHac, used by Goldtree.

- Simon for his libusbK implementation for C#, which has made Goldtree client possible.

- shchmue for FATFS libs and the system to get titlekeys without breaking processes, found in [Lockpick](https://github.com/shchmue/Lockpick).

- Translators: [unbranched](https://github.com/unbranched) and [exelix](https://github.com/exelix11) for Italian, [tiliarou](https://github.com/tiliarou) and [FuryBaguette](https://github.com/FuryBaguette) for French, and [LoOkYe](https://github.com/lookye) for German. (Both English and Spanish were made by me)

- All the testers, for reporting bugs and helping a lot with the project's development.

### Discord server

If you would like to be more informed about my projects' status and support, you should check [H&H](https://discord.gg/3KpFyaH), my Discord server. It's a simple server for Homebrew and Hacking, focused on my projects. If you would like to be a beta-tester, you might be interested on the nightly building system we have there for testers.