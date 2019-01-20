# Goldleaf

![Logo](Goldleaf/RomFs/Logo.png)

> Easy-to-use title Nintendo Switch homebrew multitool, for several purposes and with several features

- **If you are looking for Tinfoil, this is Tinfoil's safer and way more extended evolution.**

## Brief description

Goldleaf is a multipurpose, easy-to-use (and mostly community-driven) tool, specialized on NSP installing, but with other utilities, such as NAND/SD browsing, title managing or ticket managing.

If you know what FBI is (related to 3DS homebrew), this is a similar project for Nintendo Switch homebrew.

## Disclaimer

### Installing NSPs

Installing NSPs can be dangerous. If you want to avoid ban risk as much as you can, always use airplane mode.

Despite having some ways to "prevent" some risks, don't install NSPs if you want to be safe.

Official NSPs won't be safer, any kind of NSP installed unofficially (by Goldleaf or other installers) has ban risks.

Despite being official ones, it's always recommended to install only stdcrypto NSPs, those NSPs which don't have nor need a ticket to work. Official titles would need to be reencrypted, but unofficial NSPs, such as homebrew ones, are already stdcrypto.

### NAND access

With Goldleaf you have almost full access to certain partitions of the console memory. This can be helpful for exporting certain files, but can be dangerous.

Deleting contents from the NAND, specially savedata or NCAs, is a highly dangerous action, which can lead to a semibrick, which could leave you without homebrew or even OFW unless you have a NAND backup.

Imagine deleting some EXE and/or DLL files from System32 folder in Windows. Well, the result would be quite similar.

### Ticket removing

Tickets are the system used by Nintendo to manage game purchases, but more technically, they provide the key to decrypt the title they belong to.

You can remove installed ones from Goldleaf, and import them too. Installing a NSP of a title containing a ticket will import the ticket too.

Despite not being a really big issue, removing a ticket of a title which does require it will leave the game unplayable, no matter if you have sigpatches or use a certain CFW. The console cannot find the key to decrypt the title's contents so cannot play it.

## Main features

- **SD card and console memory file browsers**

  Simple and easy-to-use file browsers. In my opinion, one of the most useful features in Goldleaf. Just a file browser for the SD card or for certain console memory partitions, providing special support for certain file formats:

  - **NSP (Nintendo Submission Package)**
  
    It's the official format used by Nintendo to provide installable content from their CDN servers.

    Technically, it's a PFS (a simple file format containing several files) with NCA contents, sometimes XML and/or icon metadata (exported by official packaging tools), and a ticket and a certificate in case its contents are signed with a titlekey (official games are, system contents or homebrew NSPs' contents aren't).

    Goldleaf can install NSPs same way official apps would, like DevMenu.

    As a warning, I do not recommend installing NSPs in case you plan to go online. Installing content from unofficial tools (such as Goldleaf or the old Tinfoil) can lead to permanent bans from online services, and in severe cases, from every Nintendo service, even from updating the console or games. Install NSPs at your own risk.

  - **NRO (Nintendo Relocatable Object)**

    Officially is treated as a dynamic library object, similar to DLLs on Windows. These are loaded dynamically by applications at runtime.

    Homebrew uses them as executable binaries because as they can contain multiple data sections, we can use them to load extra info such as NACP (title, author and version), an icon, or even RomFs data to access at runtime.

    Goldleaf has the ability of launching NROs located at the SD card. Keep in mind that this feature could cause undefined behavior depending on the NRO being launched, so expect errors with some NROs.

  - **NCA (Nintendo Content Archive)**

    This format is the base format used by Horizon OS to store content.

    While the previously mentioned formats don't require any kind of cryptography to be extracted or used, NCAs have to be decrypted, so the user is required to provide the required keys to extract the content.

    NCAs have different sections, as well as other information. The two main sections are the ExeFs and RomFs: the ExeFs filesystem contains the code NSO binaries and a NPDM metadata file, while the RomFs contains different files to be accessed from the title.

    Some NCAs could also contain a special section, which is the section 2. This section contains the PNG and GIF logos displayed at the top-left and botton-right of the console when launching a title.

    NCAs don't have neccessarily those two sections. The program NCA, the base of any application or applet, will contain an ExeFs, and usually a RomFs in case it's a game or a system title with resources like settings or Home Menu.

    Appart from program NCAs there are some other NCA types: control NCA (NACP and icons), legalinfo NCA (HTML manuals and information), offline NCA (HTML documents in case the title wants to load them) content meta CNMT NCA (with title specific information)...

    Goldleaf has embedded hactool in his code, so it can extract those partitions from NCAs, if the neccessary keys are provided.

  - **NXTheme (custom Home Menu theme)**

    This format is developed by Qcean team, a format to handle Home Menu layout modding.

    Goldleaf can install them, but it needs to have Home Menu's RomFs at `sd:/goldleaf/qlaunch`. If the keys used with other features are provided, Goldleaf itself will locate the console's qlaunch (Home Menu title) contents and extract them to that directory.

    You will have to reboot with CFW after installing a theme to see any changes.

  - **NACP (Nintendo Application Control Property)**

    This is the format used by Nintendo to store several properties of installed titles, like the title's name, version, author name and other information, like if the title supports screnshots or video captures.

    Goldleaf can parse a NACP file and display some of it's information.

  - **Tickets and certificates**

    Tickets (.tik) and certificates (.cert) are the format used by Nintendo to provide encryption data for titlekey-crypto titles. (official games)
    
    NSPs might contain them, and Goldleaf is able to install them if they are located in the SD card.

    To install a ticket you will have to provide a certificate, both with the same name and in the same directory, with their extension. (example: `sd:/game.tik` and `sd:/game.cert`)

  The NAND browser has almost the same support, but it's reccomended to use it only to export files via copying them to the SD card, to avoid any trouble. Goldleaf should warn when doing dangerous operations anyway.

  - **(A)** Select a file for several file options or browse a directory

  - **(Y)** Select a directory for several directory options

  - **(X)** Paste the path of the clipboard, if there's somethind copied there.

  - **(L)** Create a new (empty) file

  - **(R)** Create a new directory

- **USB installer and Goldtree**

  You can install NSPs via USB using Goldtree, a C# PC client to handle this installations.

  Keep in mind that USB installations are unstable, and might bug sometimes.

- **Title manager**

  Goldleaf's title manager will display all the titles installed in the system, both SD, NAND and the one in the gamecart if inserted.

  You can view these titles' information, icon, install location and Application ID.

  Titles can also be completely uninstalled here. Keep in mind that deleting a title won't delete its savedata, that should stay on the system.

  Titles (not gamecart one) can also be dumped from the SD/NAND as a NSP. Keep in mind that this feature is still experimental, and that +4GB titles SHOULDN'T be dumped with a FAT32-formatted SD card.

- **Ticket manager**

  Goldleaf can get all the tickets installed on the console. While some of them will contain the name of the title they belong to, some could belong to DLC or other types of content despite not belonging directly to a title.

  You can view the Application ID and the key generation of a ticket by selecting it.

  Tickets can also be removed from the console. Removing tickets can be a dangerous option, as without them, titlekey-crypto won't be playable.

- **Internet browser**´

  Goldleaf uses the console's internal browser title (the online web-applet) to let the user browse web pages.

  For special reasons, the browser can only be used from the installable version:

  - The web-applet attempts to find a file called `accessible-urls.txt` in the Offline Html NCA contents of the title calling it and won't work unless it finds this file, so applets cannot launch this applet properly.

- **CFW and custom theme management**

  This is mostly a useful feature for checking which CFWs are on the SD card and which of them have any Home Menu modifications, meaning that a theme is probably installed there.

  You can delete any Home Menu modifications of a certain CFW after selecting a CFW.

- **Console information**

  On this option, you can check the used size of the SD card, the entire NAND, and on the different NAND partitions.

  It also displays the current firmware version of the console.

- **About**

  Displays Goldleaf's logo and whether Goldleaf is running as a NRO from hbmenu or as a normal title.

### Common controls

- **(L-stick / D-pad)** Move through the menu

- **(R-stick)** Move (very) fast through the menu

- **(ZL / ZR)** Show a dialog to shut down or reboot the console.

- **(+ / -)** [NRO only] exit the application in order to return to hbmenu.

### Providing keys

If you provide a file with several Switch keys (commonly named as `prod.keys` or `keys.dat`) you will be able to do some extra stuff with Goldleaf: (remember that you have to place it as `sd:/switch/keys.dat`, `prod.keys`, `keys.txt` or `keys.ini`)

- Unless you already have dumped qlaunch's (aka Home Menu) RomFs (or at least the files within `lyt` folder) in `sd:/goldleaf/qlaunch` directory, you will need to provide the previously mentioned key file with several keys, so that Goldleaf can extract directly the RomFs of the console's qlaunch contents to use it.

- You have the option to extract NCA contents (ExeFs, RomFs or section 2), but you will need the previously mentioned keys too.

## Installation

You have two options to use Goldleaf: load it as **regular homebrew via hbmenu** as a **NRO** binary, or install the **NSP** as a **regular title**. Ironically, you would need to install Goldleaf's NSP via Goldleaf as a NRO (or older installers like the original Tinfoil)

For both options, you will have to get the latest release of the **NRO**/**NSP** from [here](https://github.com/XorTroll/Goldleaf/releases).

Nothing else is required. No external files, or extra configuration are required for Goldleaf but the NRO/NSP.

- **NRO** binary

  Simply place the NRO anywhere in the SD card (people use to place NROs in `switch` folder) and launch it!

- **NSP** (installable title)

  Goldleaf's NSP title has application ID / title ID `050032A5CF12E000`. (as an extra piece of information)

  You need a homebrew to install the NSP. The best solution would be to download both the NRO and the NSP, and install the NSP via the NRO. (ironically)

  Having it installed, you should be able to launch Goldleaf as a normal title.

### Getting ready for USB installations

USB installations require a few extra things to be available:

- Download Zadig tool from [here](https://zadig.akeo.ie/)

- Open Goldleaf and select the USB install option, with the Switch connected to the PC via a USB-C cable.

- Open Zadig, and select the device of your Nintendo Switch, and install libusbK there.

## Goldtree and USB installs

USB communication is slightly different from Tinfoil's one, so Tinfoil's old Python script, AluminumFoil nor other tools won't work properly.

Goldtree will ask you to choose a NSP after establishing connection with Goldleaf, and it will be received and installed by Goldleaf.

Keep in mind that USB support is a bit unstable, and sometimes it might fail. Anyway, it should work most of the times.

## Special features

Goldleaf differs on some features depending on whether it is loaded as a NRO or as an installed title:

- Goldleaf can be exited via **Plus (+)** or **Minus (-)** buttons if it's loaded as a NRO, but as regular titles have to be exited from the Home Menu, this feature is not available as a title.

- Goldleaf disables **Home button pressing** while installing a NSP if it's loaded as a title, but this feature isn't available as a NRO binary for technical reasons related to applets.

- Goldleaf cannot launch NRO binaries if it's loaded as a title due to technical reasons. They can only be launched from another NRO binary.

## Issues and support

In case you find a bug or you need help with Goldleaf, you have several places to ask.

Many errors are very common and can be misunderstood, and you should document a bit for some errors instead of directly calling them issues:

- It's a common issue for some NSPs, although they are completely valid ones, being detected as wrong NSPs. Although they can be really wrong NSPs, it is usually caused by firmware mismatch. For instance, in case you are trying to install a title which requires at least 5.1.0 version (which uses key generation 4) on a lower firmware version, it won't be recognised as a valid NSP for cryptographical reasons. (the console cannot decrypt the NSP because it is encrypted with unknown keys which are within 5.1.0 update)

- It's common to get errors installing NSPs like error 0x234c02. This errors usually mean that the current CFW doesn't have the required signature patches (sigpatches) to be able to install contents.

- The USB installation can sometimes freeze in the middle of the install. If that happens, try closing Goldleaf and Goldtree, uninstalling the wrongly-installed title and retrying.

## Screenshots

- Goldleaf

![Screenshot](Screenshots/Goldleaf.menu.jpg)

![Screenshot](Screenshots/Goldleaf.install1.jpg)

![Screenshot](Screenshots/Goldleaf.install2.jpg)

![Screenshot](Screenshots/Goldleaf.install3.jpg)

![Screenshot](Screenshots/Goldleaf.nandSelect.jpg)

![Screenshot](Screenshots/Goldleaf.sysNAND.jpg)

![Screenshot](Screenshots/Goldleaf.titleBrowse.jpg)

![Screenshot](Screenshots/Goldleaf.ticketBrowse.jpg)

![Screenshot](Screenshots/Goldleaf.ncaExtract.jpg)

![Screenshot](Screenshots/Goldleaf.nxtheme.jpg)

![Screenshot](Screenshots/Goldleaf.themeInfo.jpg)

![Screenshot](Screenshots/Goldleaf.nacp.jpg)

![Screenshot](Screenshots/Goldleaf.sysInfo.jpg)

- Goldtree

![Screenshot](Screenshots/Goldtree.start.png)

![Screenshot](Screenshots/Goldtree.selectNSP.png)

![Screenshot](Screenshots/Goldtree.install.png)

## Possible future features

- Savedata mounting and browsing (and hopefully exporting)

- Support more extraction formats (PFS0 (aka NSP) or XCI), same way GodMode9 does with the 3DS system

- As GodMode9 does, implement a more simple way to mount contents instead of static menus

## Credits

The main conceps of Goldleaf are and have been developed by XorTroll, but without the effort and support of many others, this project wouldn't be what it is now:

(from all the people listed below, special thanks to Adubbz, exelix, C4Phoenix, The-4n and SciresM, for their huge support on their respective areas of homebrew)

- Adubbz and all the (old) [Tinfoil](https://github.com/Adubbz/Tinfoil) contributors, for their huge work with title installing.

- exelix and Qcean team, for all their huge support with Home Menu themes. Goldleaf uses (adapted) [SwitchThemesCommon](https://github.com/exelix11/SwitchThemeInjector) libraries to handle theme installs.

- C4Phoenix, for his awesome work doing this project's logo, and the GIF displayed when launching the installed version.

- All the icons except Goldleaf's one (see credit above) were grabbed from [Icons8](https://icons8.com).

- The-4n, for [hacBrewPack](https://github.com/The-4n/hacBrewPack), to make completely legal NSPs.

- SciresM for [hactool](https://github.com/SciresM/hactool), which was ported as a library to make NCA extraction a thing in Goldleaf.

- Thealexbarney, for his C# libraries for various Nintendo Switch formats: LibHac, used by Goldtree.

- Simon for his libusbK implementation for C#, which has made Goldtree client possible.

- All the testers, for reporting bugs and helping with the project's development.

## Support

If you would like to be more informed about my projects' status and support, you should check [H&H](https://discord.gg/3KpFyaH), my Discord server. It's a simple server for Homebrew and Hacking, focused on my projects. If you would like to be a beta-tester, you might be interested on the nightly building system we have there for testers.

If you like my work, you should take a look at my [Patreon](https://patreon.com/xortroll) page. For those who support me, you will be credited on my projects, and you'll gain some nice extras on H&H!
