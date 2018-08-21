# Tinfoil
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
https://github.com/Adubbz/Tinfoil/releases/latest

## Usage
1. Place NSPs in ``/tinfoil/nsp`` or extracted NSPs in ``/tinfoil/extracted``.
2. Launch Tinfoil via the homebrew menu and install your NSPs.

## Network Install Instructions
1. Download and install Python 3.6.6 from https://www.python.org/downloads/
2. Download remote_install_pc.py from https://github.com/Adubbz/Tinfoil/blob/master/tools/remote_install_pc.py by right clicking on Raw and clicking "Save link as..."
3. Open the Network Install menu in Tinfoil
4. Run ``remote_install_pc.py`` on your PC on the same network as your Switch with python ``remote_install_pc.py <switch ip> <nsp directory>``, or alternatively with no arguments for interactive mode.
5. ???
6. Profit

## Donate
If you enjoy my work any and all donations are much appreciated :)

[![Donate](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=TBB9Q9YL9AB74&lc=AU&item_name=Adubbz%27s%20Tools%20%26%20Game%20Mods&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted)
