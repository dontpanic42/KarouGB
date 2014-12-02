### KarouGB

A simple project to help me understand hardware emulation and c++.

### What works

Some games with an MBC-1 Chip or no MBC chip, like Super Mario Land, Legend of Zelda, Donkey Kong Land.

### What doesn't work

Lots of stuff. Only pure ROM and MBC-1 cartridges are supported at the time (that means no Pokemon).

No sound support at all, although i'm working on that :-)

### Resources

http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-Memory-Banking
https://github.com/drhelius/Gearboy
https://github.com/sinamas/gambatte

### Building

Depends on the SDL2.framework.

Requires you to create an "rom_bios.h" file, which contains the bootrom (readily available via google search) as an array (u08i rom_bios[]). The bios might be copyrighted (?).


