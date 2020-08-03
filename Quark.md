# Quark and USB setup

## Requisites

Quark needs Java 9 or greater to run. See below the recommended installation for Windows, Linux and Mac.

You also need to install libusbK drivers for USB to work fine.

## Setup

### Windows

The best way to install Java 9 in Windows (or a very simple one) is to install [AdoptOpenJDK 11 or higher](https://adoptopenjdk.net).

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

Now you can run Quark using ```java -jar Quark.jar```.

### Mac

Install OpenJDK 11 (or higher) in the terminal:

- Install brew ```/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"```

- Run ```brew tap AdoptOpenJDK/openjdk```

- Run ```brew cask install adoptopenjdk11```

- Finally, run ```java -version``` to check the JDK version

Now you can run Quark using ```java -jar Quark.jar```.

Having done all this, Quark <-> Goldleaf USB should work fine.
