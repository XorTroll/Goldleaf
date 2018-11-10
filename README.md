<<<<<<< HEAD
# Goldleaf - Nintendo Switch title manager homebrew (open-source)

### This is the project Tinfoil is going to become

![Goldleaf](Icon.jpg)

## Current state (this will be removed when it gets released)

- Goldleaf is still NOT released. It can be built and tested, but it's kinda complex.

- It is NOT recommended to try to build this project. It's easy to get messed up building it.

## What is Goldleaf?

- If you know what FBI is (related to 3DS homebrew), this is a similar project for the Switch.

- You can easily manage title-related stuff, like install titles via NSP packages or uninstall them.

## What can Goldleaf do?

- Install NSP packages, browsing the SD card for the package (needs to be a *.nsp file).

- Force uninstall titles (this won't delete the save data of the titles).

- Browse currently installed tickets, and the option to delete them

- Browse NAND partitions, being able to copy files and paste them to the SD card (no delete/paste permissions on NAND)

## Credits

- Adubbz and all the (old) Tinfoil contributors, for their huge work with title installing

  (more people needs to be added here) 
=======
# Tinfoil

### [XorTroll] This project is about to become Goldleaf! This will be updated soon!!

A homebrew application for the Nintendo Switch used to manage titles.

DISCLAIMER: I take zero responsibility for any bans, damage, nuclear explosions, or anything else the use of Tinfoil may cause. Proceed with caution!

## Installation
1. Install FS sig patches, there are two ways of doing this:
    - Using the latest [hekate](https://github.com/CTCaer/hekate) with the option ``kip1patch=nosigchk`` in ``hekate_ipl.ini``.
    - Using [ReiNX](https://github.com/Reisyukaku/ReiNX)
2. Place the tinfoil nro in the "switch" folder on your sd card, and run using the homebrew menu.

## Download
Courtesy of [LavaTech](https://discord.gg/VjyDSuu), builds are made automatically for each commit at:
https://bsnx.lavatech.top/tinfoil/

When ready, full releases will be located at:
https://github.com/XorTroll/Tinfoil/releases/latest

## Usage
1. Place NSPs in ``/tinfoil/nsp`` or extracted NSPs in ``/tinfoil/extracted``.
2. Launch Tinfoil via the homebrew menu and install your NSPs.

## Network Install Instructions
1. Download and install Python 3.6.6 from https://www.python.org/downloads/
2. Download remote_install_pc.py from https://github.com/XorTroll/Tinfoil/blob/master/tools/remote_install_pc.py by right clicking on Raw and clicking "Save link as..."
3. Open the Network Install menu in Tinfoil
4. Run ``remote_install_pc.py`` on your PC on the same network as your Switch with python ``remote_install_pc.py <switch ip> <nsp directory>``, or alternatively with no arguments for interactive mode.
5. ???
6. Profit

## Usb Install Instructions
1. Download and install Python 3.6.6 from https://www.python.org/downloads/
2. Download usb_install_pc.py from https://github.com/XorTroll/Tinfoil/blob/master/tools/usb_install_pc.py by right clicking on Raw and clicking "Save link as..."
3. Open the Usb Install menu in Tinfoil
4. Run ``usb_install_pc.py`` on your PC on the same network as your Switch with python ``usb_install_pc.py <nsp directory>``.
5. ???
6. Profit

## Donate
If you enjoy my work any and all donations are much appreciated :)

[![Donate](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=TBB9Q9YL9AB74&lc=AU&item_name=Adubbz%27s%20Tools%20%26%20Game%20Mods&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted)
>>>>>>> 550df97279543b82143cbec4675242d58a6a893e
