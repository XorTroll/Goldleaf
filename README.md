# Goldleaf

![Logo](Logo.png)

> Easy-to-use title installer & manager for Nintendo Switch

## Brief description

Goldleaf is a multipurpose tool, specialized on title installing from NSP packages, but with other utilities, such as NAND/SD browsing, 

You can easily manage title-related stuff, like install titles via NSP packages or uninstall already installed titles.

If you know what FBI is (related to 3DS homebrew), this is a similar project for Nintendo Switch.

## Goldleaf's features

- Browse and explore SD card's and NAND partitions' (SAFE, USER and SYSTEM) files and directories. Goldleaf provides the functionality of a basic file explorer, with options to browse, delete and copy files or directories between NAND partitions and SD. (writing or deleting content in NAND will always warn the user before doing so).

- The previously mentioned SD/NAND browser also supports NSP installing and NRO launching.

- Manage currently installed titles, being able to see where are they installed, or uninstalling them.

- Manage currently installed tickets, being able to see the application they belong to (their application ID and the key generation), or removing them. (removing tickets can be dangerous anyway)

- Show SD card's and NAND partitions' space, and the firmware version (number and display name) of the current console.

- As some other miscellaneous options, you can easily reboot or shut down your console from Goldleaf.

## Installation

You have two options to use Goldleaf: load it as regular homebrew via hbmenu as a NRO binary, or install it as a regular title. Ironically, you would need to install Goldleaf's NSP via Goldleaf as a NRO (or older installers like original Tinfoil)

For both options, you will have to get the latest release of the NRO/NSP from [here](https://github.com/XorTroll/Goldleaf/releases).

Nothing else is required. No external files, or extra configuration are required for Goldleaf but the NRO/NSP.

- **NRO** binary

  Simply place the NRO anywhere in the SD card (people use to place NROs in `switch` folder) and launch it!

- **NSP** (installable title)

  Goldleaf's NSP title has application ID / title ID `050032A5CF12E000`. (as an extra piece of information)

  You need a homebrew to install the NSP. The best solution would be to download both the NRO and the NSP, and install the NSP via the NRO. (ironically)

  Having it installed, you should be able to launch Goldleaf as a normal title.

## Special features

Goldleaf differs on some features depending on whether it is loaded as a NRO or as an installed title:

- Goldleaf can be exited via Plus (+) or Minus (-) buttons if it's loaded as a NRO, but as regular titles have to be exited from the Home Menu, this feature is not available as a title.

- Goldleaf disables Home button pressing while installing a NSP if it's loaded as a title, but this feature isn't available as a NRO binary for technical reasons related to applets.

- Goldleaf cannot launch NRO binaries if it's loaded as a title due to technical reasons. They can only be launched from another NRO binary.

## Issues and support

In case you find a bug or you need help with Goldleaf, you have several places to ask.

Many errors are very common and can be misunderstood, and you should document a bit for some errors instead of directly calling them issues:

- It's a common issue for some NSPs, although they are completely valid ones, being detected as wrong NSPs. Although they can be really wrong NSPs, it is usually caused by firmware mismatch. For instance, in case you are trying to install a title which requires at least 5.1.0 version (which uses key generation 4) on a lower firmware version, it won't be recognised as a valid NSP for cryptographical reasons. (the console cannot decrypt the NSP because it is encrypted with unknown keys which are within 5.1.0 update)

## Possible future features

- Savedata mounting and browsing (and maybe exporting)

- Dark themeing?

## Credits

- Adubbz and all the (old) Tinfoil contributors, for their huge work with title installing.

- C4Phoenix, for his awesome work doing this project's logo.

- All the icons except Goldleaf's one (see credit above) were grabbed from [Icons8](https://icons8.com).

- The-4n for [hacBrewPack](https://github.com/The-4n/hacBrewPack), to make completely legal NSPs

- All the testers, for reporting bugs and helping with the project's development.

## Patreon

If you like my work, you should take a look at my [Patreon](https://patreon.com/xortroll)!