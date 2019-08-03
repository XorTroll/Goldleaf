![Logo](Goldleaf/RomFs/Logo.png)

![License](https://img.shields.io/badge/License-GPLv3-blue.svg) [![Releases](https://img.shields.io/github/downloads/xortroll/goldleaf/total.svg)]() [![LatestVer](https://img.shields.io/github/release-pre/xortroll/goldleaf.svg)]()

> Goldleaf is multipurpose homebrew tool for Nintendo Switch.

# Table of contents

1. [Features](#features)
2. [Disclaimer](#disclaimer)
3. [Forwarding](#forwarding)
4. [Settings](#settings)
5. [Known bugs](#known-bugs)
6. [Contributing](#contributing)
7. [Credits and support](#credits-and-support)

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

- **Remote PC browser**: Via this feature, one can browse a PC's drives like any other filesystem, connected by a USB-C cable.

- **Content manager**: Browse any kind of content in your SD card, console memory or game cartridge. You can browse all the NCAs, and as title options, you can remove the content, or export it as a NSP. NAND SYSTEM contents (system titles or contents) aren't allowed to be deleted. Invidual contents such as updates or DLC can be removed individually.

- **Ticket manager**: Browse all tickets, both common and personalized. You can remove them, but an extra warning will be displayed when attempting to remove tickets being used by some content.

- **Web browser (web applets)**: Using the console's web applets, Goldleaf allows to search and navigate through web pages. If Goldleaf is launched through hbmenu as a NRO, WifiWebAuth applet will be used, which tends to fail sometimes, along with fewer features, such as lack of video support.

- **User account manager**: Select a user account, and easily change things as the user's nickname or even delete it. Via the file browsers you can replace a user's icon with a JPEG image, but it can be a risky action. The icon needs to be a valid JPEG, with 256x256 dimensions. Goldleaf checks if the icon is valid to replace the other one.

- **Console information**: Simple menu to see current firmware string, and the total free and occupied size on SD card and console memory.

Among all this features, Goldleaf is a very intuitive tool, with a fancy UI, and fully translated into 5 different languages: **English**, **Spanish**, **German**, **French** and **Italian**.

### Common controls

- **(L-stick / D-pad)** Move through the current menu

- **(R-stick)** Move slightly faster through the current menu

- **(ZL / ZR)** Show a dialog to shut down or reboot the console.

#### NRO version

- **(+ / -)** Exit the application in order to return to hbmenu.

## Disclaimer

### NSP installs

Always keep in mind that installing NSPs can be a very easy way to get banned. If you use airplane mode and special DNSs like 90DNS, there is a smaller chance to get banned.

**NEVER** install untrusted NSPs. Goldleaf can identify (according to the NSP's application ID) whether the NSP is a homebrew NSP (IDs starting with "05", usually for forwarders), or a regular title (official ID range just allows IDs starting with "01")

### Tickets

Tickets represent a game purchase, but technically speaking, you can't boot a title if the ticket isn't present (in case the title requires the ticket).

NSPs which don't have (nor require) a ticket are "standard crypto" NSPs. Standard-crypto titles aren't offical (usually reconverted content to avoid importing tickets).

Goldleaf will display whether a ticket is present or not, and if so, it's titlekey. Titlekeys are required to decrypt and access titles' content which require a ticket. Otherwise, they are undecryptable, hence unlaunchable by the system.

**TL;DR:** **Always** try to avoid untrusted NSPs, and if you go ahead and install CDN NSPs with tickets, avoid going online, or use special tools such as 90DNS, or you'll be banned for sure.

## Forwarding

Goldleaf is released along with a NSP, in order to launch Goldleaf as an installed title, and take advantage of extra RAM and features such as web browsing.

The NSP is a forwarder since **0.6**, what means that, instead of having Goldleaf's source inside the NSP, it simply loads the NRO from the SD card, meaning that only the NRO should be updated instead of having to reinstall the NSP.

Goldleaf's forwarding code tries to locate Goldleaf (`Goldleaf.nro`) in `sd:/switch/` and `sd:/switch/Goldleaf/`. If it isn't found, an error message is displayed.

## Settings

Goldleaf supports the customization of colors, assets and other options via a JSON file located at Goldleaf's folder: `sd:/switch/Goldleaf/settings.json`.

### Sample

This is an example JSON for Goldleaf settings:

```json
{
    "general": {
        "customLanguage": "es",
        "keysPath": "/switch/prod.keys",
        "externalRomfs": "/switch/Goldleaf/theme/rom"
    },
    "ui": {
        "background": "#aabbccdd",
        "base": "#aabbccdd",
        "baseFocus": "#aabbccdd",
        "text": "#aabbccdd",
        "menuItemSize": 80
    },
    "installs": {
        "ignoreRequiredFwVersion": true
    }
}
```

If a certain attribute isn't present Goldleaf will use default values. For colors, Goldleaf will initially load default light/dark themes according to the console's theme color, and later, colors present in the JSON will be used instead of default theme ones.

### Notes

Via RomFs replacement, when Goldleaf tries to locate, for instance, `romfs:/FileSystem/FileDataFont.ttf` resource font, if `romfsReplace` is true and `romfsReplacePath` is, for instance, `/switch/Goldleaf/testromfs`, Goldleaf will look if `sd:/switch/Goldleaf/testromfs/FileSystem/FileDataFont.ttf` exists and use it if so, otherwise will use RomFs's one.

So, via this configurations, UI's images, resources, element sizes and even translations (using custom JSON translations) can be used, plus some more assets which will be added in future updates.

## Known bugs

- On Atmosphère and SX OS, exiting Goldleaf via HOME menu (as a NRO) seems to crash the system on 7.x firmwares.

- Remote PC browsing in general seems to have several bugs, most of them related to the UI system.

## Contributing

If you would like to contribute with new features, you are free to fork Goldleaf and open pull requests showcasing your additions.

If you just would like to suggest new ideas, but without actual code implementations, you're free to open an issue. Please try not to duplicate those, if the idea or problem is already reported in another issue.

### Translations

Read this before making any new translations:

- Goldleaf's aim is to, mainly, support languages supported by the console itself, so those not yet supported by Goldleaf and which aren't supported by consoles should have less priority and won't be probably accepted.

- UI (Plutonium) lacks special UTF16 characters, so some languages like Russian or Korean will not work, at least on the UI's current state.

## Credits and support

### Credits

The main concepts of Goldleaf are and have been developed by me (XorTroll), but without the effort and support of many others, this project wouldn't have been a thing:

- Adubbz and all the (old) [Tinfoil](https://github.com/Adubbz/Tinfoil) contributors, for their huge work with title installing.

- C4Phoenix, for his awesome work doing this project's logo, and the GIF displayed when launching the installed forwarder.

- All the graphics except Goldleaf's logo (see credit above) were grabbed from [Icons8](https://icons8.com).

- 2767mr, for all the support given in Goldtree.

- Simon for his libusbK implementation for C#, which has made Goldtree client possible.

- shchmue and blawar for the system to get titlekeys without breaking processes, found in [Lockpick](https://github.com/shchmue/Lockpick).

- Translators: [unbranched](https://github.com/unbranched) and [exelix](https://github.com/exelix11) for Italian, [tiliarou](https://github.com/tiliarou) and [FuryBaguette](https://github.com/FuryBaguette) for French, and [LoOkYe](https://github.com/lookye) and C4Phoenix for German.

- All the testers, for reporting bugs and helping a lot with the project's development.

### Discord server and beta testing

If you would like to be more informed about my projects' status and support, you should check [my Discord server](https://discord.gg/3KpFyaH). It's a simple server for Nintendo hacking and homebrew, mainly focused on my projects. If you would like to be a beta-tester, then this is what you're looking for.
