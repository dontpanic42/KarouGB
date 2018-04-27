### KarouGB

A simple project to help me understand hardware emulation and c++.

### What works

Some games with an MBC-1/MBC-3 Chip or no MBC chip, like Super Mario Land, Legend of Zelda, Donkey Kong Land, Pokémon Red.

Some Gameboy Color games might work. Gameboy Color support is considered experimental and unstable  at this time. The original Gameboy Color Bootrom is currently not supported. To still run Games, you have to patch the Classic Gameboy Rom (change Byte 254 (first Byte is counted as "1") from 0x01 to 0x11).

### What doesn't work

Lots of stuff. Only pure ROM and MBC-1/MBC-3 cartridges are supported at the time.

No sound support at all.

### Screenshots
![Settings Window](https://raw.githubusercontent.com/dontpanic42/KarouGB/master/docs/screenshots/pokemon.png)

![Settings Window](https://raw.githubusercontent.com/dontpanic42/KarouGB/master/docs/screenshots/zelda1.png)

![Settings Window](https://raw.githubusercontent.com/dontpanic42/KarouGB/master/docs/screenshots/zelda2.png)

![Settings Window](https://raw.githubusercontent.com/dontpanic42/KarouGB/master/docs/screenshots/donkeykong.png)
### Resources

http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-Memory-Banking
https://github.com/drhelius/Gearboy
https://github.com/sinamas/gambatte

### Building

Depends on Qt5 for UI and GTest for Testing. GTest is downloaded/built automatically as part of the build process.

Requires you to create an "rom_bios.h" file, which contains the bootrom (readily available via google search) as an array (u08i rom_bios[]).


