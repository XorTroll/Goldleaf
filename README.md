<p align="center">
  <img alt="Goldleaf" src="Goldleaf/romfs/Logo.png">
</p

<p align="center">
  <b>💜 thanks for 2M downloads 💜</b>
</p

<p align="center">
  <a title="Discord" href="https://discord.gg/3KpFyaH">
    <img alt="Discord" src="https://img.shields.io/discord/789833418631675954?label=Discord&logo=Discord&logoColor=fff&style=for-the-badge">
  </a>
  <a title="Downloads" href="https://github.com/XorTroll/Goldleaf/releases/latest">
    <img alt="Downloads" src="https://img.shields.io/github/downloads/XorTroll/Goldleaf/total?longCache=true&style=for-the-badge&label=Downloads&logoColor=fff&logo=GitHub">
  </a>
  <a title="License" href="https://github.com/XorTroll/Goldleaf/blob/master/LICENSE">
    <img alt="License" src="https://img.shields.io/github/license/XorTroll/Goldleaf?style=for-the-badge">
  </a>
</p>

<p align="center">
  <a title="Patreon" href="https://www.patreon.com/xortroll">
    <img alt="Patreon" src="https://img.shields.io/endpoint.svg?url=https%3A%2F%2Fshieldsio-patreon.vercel.app%2Fapi%3Fusername%3DXorTroll%26type%3Dpatrons&style=for-the-badge"/>
  </a>
  <a href="https://github.com/sponsors/XorTroll">
    <img alt="GitHub sponsors" src="https://img.shields.io/github/sponsors/XorTroll?label=Sponsor&logo=GitHub&style=for-the-badge"/>
  </a>
  <a href="https://www.paypal.com/donate/?hosted_button_id=PHQKFTY9AHPUU">
    <img alt="PayPal" src="https://img.shields.io/badge/Donate-PayPal-green.svg?style=for-the-badge"/>
  </a>
</p>

<h3>
Table of contents
</h3>

- [Features](#features)
  - [Controls](#controls)
- [Disclaimer](#disclaimer)
  - [NSP installing](#nsp-installing)
  - [Tickets](#tickets)
- [Quark and remote browsing](#quark-and-remote-browsing)
  - [Windows](#windows)
  - [Linux](#linux)
  - [Mac](#mac)
- [Settings](#settings)
  - [Sample](#sample)
  - [Notes](#notes)
- [Known bugs](#known-bugs)
- [Building](#building)
- [Contributing](#contributing)
  - [Translations](#translations)
- [Credits](#credits)

## Features

Goldleaf's concept is very similar to [FBI](https://github.com/Steveice10/FBI)'s, a well-known Nintendo 3DS homebrew tool.

- **Content explorer**: An easy-to-use but fully featured filesystem browser.

  - Supported locations/devices:

    - *SD card*

    - *Console memory (SAVE, SYSTEM, USER and PRODINFOF partitions)*

    - *Remote systems (via remote tools like Quark)*

    - *USB drives (thanks to libusbhsfs)*

    - *Game savedata (read-only, check below how to mount it)*

    - *NRO homebrew RomFs (read-only)*

    > Note: all will be automatically unmounted when Goldleaf is exited

  - Features:

    - Basic file operations: create, delete, copy, rename, view as hexadecimal/text, get size, etc.

    - Basic directory operations: create, delete, copy, rename, (un)set archive bit, get full size, etc.

    - Install NSP packages of applications/games, updates, add-on-content/DLC, etc. (**Note: use this carefully, and make sure you know what you're doing!**)

    - Launch other NRO homebrews and mount their RomFs (option mentioned above)

    - Execute .bin files as RCM payloads (reboot with them)

    - Set JPG/JPEG files as user icons (**Note: use this carefully, and make sure you know what you're doing!**)

    - // TODO: more

- **Content manager**: an easy-to-use but fully featured content manager:

  - Basic content (program, update, DLC, etc.) operations:

    - View content information (ID, size, version, play stats...)

    - Uninstall/completely remove the content (won't remove savedata)

    - Export existing contents to a NSP package

    - Mount savedata to navigate it (check the **Content explorer** above)

    - Remove ticket if present (**Note: use this carefully, and make sure you know what you're doing!**)

    - Reset launch version (useful to be able to play a base game after an update has been removed)

    > Note: some of this operations are not available with system or gamecard contents.

  - Check unused tickets, and remove them (**Note: use this carefully, and make sure you know what you're doing!**)

- **Web browser**: makes use of the console's web applet:

  - Browse user-submitted URLs

  - Create and save bookmarks

  > Note: the web browser is only guaranteed to work properly when launched as application.

- **Account manager**: select a user (or just use the pre-selected one if present) and manage it:

  - Change nickname

  - View its icon

  - Delete user (of course, only if there is more than one in the system!)

  > Note: to change the user's icon, check the **Content explorer** above.

- **Amiibo dump**: create a virtual amiibo (for emuiibo) from a real amiibo figurine.

- **Settings**: check both system and Goldleaf settings:

  - Firmware and updates:

    - Check current firmware information (version, hash, key generation)

    - Check for pending updates, and (if desired) remove them if present

    - Export current firmware or pending update data

  - Memory: check free and total space on different locations

  - Goldleaf settings: check settings present in `settings.json`

- **Updates**: check for latest Goldleaf updates, and (if desired) update to a new release if it exists

- **About**: check Goldleaf's version and how it was launched (as an applet or an application)

- Goldleaf has been translated to **11** different languages so far: **English**, **Spanish**, **German**, **French**, **Italian**, **Dutch**, **Japanese**, **Portuguese**, **Korean**, **traditional Chinese** and **simplified Chinese**.

> *Thanks to all the various contributors to the different translations! 💜*

### Controls

- Common:

  - **(LR-stick / R-stick / D-pad)**: Move through the current menu

  - **(ZL / ZR)**: Show a dialog to shut down or reboot the console.

  - **(Minus / -)**: Show a dialog with this control information.

  - **(Plus / +)**: Exit the application in order to return to hbmenu.

- Content explorer:

  - **(X)**: Unmount the selected location (only when a manually-mounted location is selected)

  - **(Y)**: Select folder (might take a bit of time with folders with many sub-items)

## Disclaimer

### NSP installing

Always keep in mind that installing NSP packages can be a very easy way to get banned. If you use airplane mode and special DNSs like 90DNS, there is a smaller chance to get banned.

**NEVER** install untrusted NSPs. Goldleaf won't do any special verification, so please be very careful about installing untrusted packages.

### Tickets

Tickets represent a game purchase - technically speaking, you can't launch a title if the ticket isn't present and the title requires it (official titles require them). NSP packages which don't have/require a ticket are standard-crypto.

Goldleaf will display whether a ticket is present or not, and if so, it's titlekey. Titlekeys are required to decrypt and access a title's content. Otherwise, they are undecryptable, hence unlaunchable by the system.

## Quark and remote browsing

Quark Goldleaf's desktop Java tool, working as the desktop client for the remote PC option. It supports Windows, Linux and Mac.

Quark needs Java 9 or greater to run. See below the recommended installation for each supported system.

You also need to install libusbK drivers for USB to work fine.

### Windows

The best way to install Java 9 in Windows (or a very simple one) is to install [AdoptOpenJDK 11 or higher](https://adoptopenjdk.net).

> Note: make sure that the JDK/JRE you choose contains JavaFX! You can always install it manually otherwise

After installing it, double-clicking the JAR should be enough to start it.

Otherwise, run ```java -jar Quark.jar``` in the command prompt.

For the USB to get recognized, follow the following steps:

- Download [Zadig](https://zadig.akeo.ie)

- Boot your console with CFW, connect it to the PC via USB

- Open Goldleaf

- With Zadig, select the device named "Goldleaf" (if it doesn't appear, ensure Goldleaf has a USB icon on the top of the screen, and select "List all devices" under "Options" in Zadig)

- Install **libusbK** to that device (any other driver won't work fine)

### Linux

Install OpenJDK 11 (or higher) in the terminal:

- Run ```sudo add-apt-repository ppa:openjdk-r/ppa```

- Run ```sudo apt-get update```

- Finally, run ```sudo apt-get install openjdk-11-jdk``` (if you just want the JRE, install `openjdk-11-jre` instead)

- Create the file ```/etc/udev/rules.d/99-switch.rules``` with the following contents: ```SUBSYSTEM=="usb", ATTRS{idVendor}=="057e", ATTRS{idProduct}=="3000", GROUP="plugdev"```

- Reload udev rules with: ```sudo udevadm control --reload-rules```

Now you can run Quark using ```java -jar Quark.jar```.

### Mac

Install OpenJDK 11 (or higher) in the terminal:

- Install brew ```/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"```

- Run ```brew tap AdoptOpenJDK/openjdk```

- Run ```brew install adoptopenjdk11 --cask```

- Finally, run ```java -version``` to check the JDK version

Now you can run Quark using ```java -jar Quark.jar```.

Having done all this, the USB connection should work fine.

## Settings

Goldleaf supports the customization of colors, assets and other options via a JSON file located at Goldleaf's folder: `sd:/switch/Goldleaf/settings.json`.

### Sample

This is an example JSON for Goldleaf settings:

```json
{
    "general": {
        "customLanguage": "es",
        "externalRomFs": "/switch/Goldleaf/theme/rom",
        "use12hTime": true
    },
    "ui": {
        "background": "#aabbccdd",
        "base": "#aabbccdd",
        "baseFocus": "#aabbccdd",
        "text": "#aabbccdd",
        "menuItemSize": 80
    },
    "installs": {
        "ignoreRequiredFwVersion": false,
        "copyBufferMaxSize": 10485760
    },
    "export": {
        "decryptBufferMaxSize": 10485760
    },
    "web": {
        "bookmarks": [
            {
                "name": "Google",
                "url": "https://www.google.com"
            },
            {
                "name": "GitHub",
                "url": "https://www.github.com"
            }
        ]
    }
}
```

If a certain attribute isn't present Goldleaf will use default values. For colors, Goldleaf will initially load default light/dark themes according to the console's theme color, and later, colors present in the JSON will be used instead of default theme ones.

### Notes

Via RomFs replacement, when Goldleaf tries to locate, for instance, `romfs:/FileSystem/FileDataFont.ttf` resource font, if `romfsReplace` is true and `romfsReplacePath` is, for instance, `/switch/Goldleaf/testromfs`, Goldleaf will look if `sd:/switch/Goldleaf/testromfs/FileSystem/FileDataFont.ttf` exists and use it if so, otherwise will use RomFs's one.

So, via this configurations, UI's images, resources, element sizes and even translations (using custom JSON translations) can be used, plus some more assets that might be added in future updates.

## Known bugs

- Exiting Goldleaf via HOME menu (as a NRO) seems to crash the system on 7.x firmwares due to a weird USB bug present on those specific versions. Any non-7.x firmware doesn't seem to have this issue.

## Building

In order to build Goldleaf, you will need the following:

- [devkitA64](https://devkitpro.org)

- JDK 9 or higher (needed for Quark)

- Maven (needed for Quark)

You will also need to install the following packages with devkitPro's pacman:

- `switch-sdl2 switch-freetype switch-glad switch-libdrm_nouveau switch-sdl2_gfx switch-sdl2_image switch-sdl2_ttf switch-sdl2_mixer`

Remember to clone this repository recursively (`git clone <this-repo-url> --recurse-submodules`) since Goldleaf makes use of submodules.

In order to build Goldleaf, run `make setup` (for the first time, just `make` for the following builds) and wait for it to finish building.

In order to build Quark, just execute the `build.sh` script in its directory.

## Contributing

If you would like to contribute with new features, you are free to fork Goldleaf and open pull requests showcasing your additions.

If you just would like to suggest new ideas, but without actual code implementations, you're free to open an issue. Please try not to duplicate those, if the idea or problem is already reported in another issue.

You can always contact me on my Discord server (invite link below) as an easier way to suggest ideas or directly report issues.

### Translations

Goldleaf's aim is to, mainly, support languages supported by the console itself, so those not yet supported by Goldleaf and which aren't supported by consoles should have less priority and won't be probably accepted.

Note that some languages may contain untranslated (English) content. I prioritize relevant releases/updates over correct and fully up-to-date translations, thus releases may not contain complete translations.

Anyone with enough knowledge of a certain language is absolutely welcome to make a PR with translation suggestions and/or fixes.

## Credits

The main concepts of Goldleaf are and have been developed by me (XorTroll), but without the effort and support of many others, this project wouldn't have been a thing:

- Adubbz and all the (old) [Tinfoil](https://github.com/Adubbz/Tinfoil) contributors, for their huge work with title installing.

- C4Phoenix, for his awesome work doing this project's logo, and the GIF displayed when launching the installed forwarder.

- All the graphics except Goldleaf's logo (see credit above) were grabbed from [Icons8](https://icons8.com).

- 2767mr, for all the support given in Goldtree (former Quark tool).

- developer.su for [NS-USBloader](https://github.com/developersu/ns-usbloader), which helped a lot on the development of Quark, mainly on the usage of usb4java and the base of a multi-platform JavaFX Java project.

- Simon for his libusbK implementation for C#, which made the former Goldtree client possible.

- shchmue for the system to get tickets from system save data without breaking anything, found in [Lockpick](https://github.com/shchmue/Lockpick).

- WerWolv for the help with custom exception handling, avoiding those frustrating fatals.

- Translators: [unbranched](https://github.com/unbranched) and [exelix](https://github.com/exelix11) for Italian, [tiliarou](https://github.com/tiliarou), [FuryBaguette](https://github.com/FuryBaguette) and GOffrier#9688 for French, and [LoOkYe](https://github.com/lookye) and C4Phoenix for German.

- All the testers, for reporting bugs and helping a lot with the project's development <3