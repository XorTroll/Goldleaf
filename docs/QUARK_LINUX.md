# Using Quark in Linux

In order to use Quark in Linux, you need openJDK or Java SE 11, then you just launch Quark.jar. You may want to make sure that you have a udev rule set so you don't have to run java as root!

## Setting udev rule

Open your editor as root.

    sudoedit /etc/udev/rules.d/99-NS.rules

Paste this inside.    
    
    SUBSYSTEM=="usb", ATTRS{idProduct}=="3000", ATTRS{idVendor}=="057e", MODE="0666"

Close editor and reload udev rules.

    sudo udevadm control --reload-rules && sudo udevadm trigger    

## Building and running

Set udev rule or run java as root. Ensure maven and JDK11 are installed and set as default or manually called. To ensure the result can be used on Java 8+ ensure you compile with Java SE 8.

    git clone https://github.com/droidman/goldleaf.git
    cd Goldleaf/Quark
    mvn -B -DskipTests clean package
    java -jar target/Quark.jar

## Important notes

With Java 12 choosing directly a file (Select file from Goldleaf) doesn't work yet.