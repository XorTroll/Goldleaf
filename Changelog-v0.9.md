# Changelog

## General

- Simplified directory deleting logic for both Goldleaf and Quark (thanks @pheki)

## Goldleaf

- Introducing savedata browsing! select a game from the game manager menu, and mount it's savedata. Note that, if you forget to unmount them manually, they will be automatically unmounted when Goldleaf is exited

- Simplified and cleaned a lot of internal code, hopefully making everything more stable

- Updated libnx and Plutonium - this implies support for previously unsupported special text (Japanese and Chinese characters, etc.) with last Plutonium

- Non-lowercase file extensions (.TXT, .BIN, .Nsp, etc.) are now properly supported

- Fixed resetting a title's launch version, which wasn't properly implemented on previous releases

- Random color schemes/pallets are the default in Goldleaf now # TODO: finish this, and decide whether to support them by default

## Quark

- Several improvements related to various small errors

- From now on, two different releases will be made: one for Java 8 and one for Java 9 or higher (to avoid version issues)