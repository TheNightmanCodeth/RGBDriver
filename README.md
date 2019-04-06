#  RGBDriver

A VirtualSMC plugin for controlling RGB LEDs on some motherboards.

`Check COMPAT.md for compatibility`

## Features
* Control lights from userland config application (coming soon)
* Extensible (Add your MOBO)

## Building
You'll need to have the DEBUG versions of Lilu and VirtualSMC installed.
Point XCode to the Resources folder inside of the kexts and run a build.

## Installing
Move kext to /EFI/CLOVER/kexts/other
