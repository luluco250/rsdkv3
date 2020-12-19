#include "RetroEngine.hpp"

// Palettes (as RGB888 Colours)
PaletteEntry fullPalette32[PALETTE_COUNT][PALETTE_SIZE];
PaletteEntry *activePalette32 = fullPalette32[0];

// Palettes (as RGB565 Colours)
ushort fullPalette[PALETTE_COUNT][PALETTE_SIZE];
ushort *activePalette = fullPalette[0]; // Ptr to the 256 colour set thats active

byte gfxLineBuffer[SCREEN_YSIZE]; // Pointers to active palette

int fadeMode = 0;
byte fadeA   = 0;
byte fadeR   = 0;
byte fadeG   = 0;
byte fadeB   = 0;

int paletteMode = 0;

void LoadPalette(const char *filePath, int paletteID, int startPaletteIndex, int startIndex, int endIndex)
{
    FileInfo info;
    char fullPath[0x80];

    StrCopy(fullPath, "Data/Palettes/");
    StrAdd(fullPath, filePath);

    if (LoadFile(fullPath, &info)) {
        SetFilePosition(3 * startIndex);
        if (paletteID >= PALETTE_COUNT || paletteID < 0)
            paletteID = 0;

        byte colour[3];
        if (paletteID) {
            for (int i = startIndex; i < endIndex; ++i) {
                FileRead(&colour, 3);
                SetPaletteEntry(paletteID, startPaletteIndex++, colour[0], colour[1], colour[2]);
            }
        }
        else {
            for (int i = startIndex; i < endIndex; ++i) {
                FileRead(&colour, 3);
                SetPaletteEntry(-1, startPaletteIndex++, colour[0], colour[1], colour[2]);
            }
        }
        CloseFile();
    }
}

void SetLimitedFade(byte paletteID, byte R, byte G, byte B, ushort Alpha, int startIndex, int endIndex)
{
    if (paletteID >= PALETTE_COUNT)
        return;
    paletteMode     = 1;
    activePalette   = fullPalette[paletteID];
    activePalette32 = fullPalette32[paletteID];
    if (Alpha > 0xFF)
        Alpha = 0xFF;

    if (endIndex < PALETTE_SIZE)
        ++endIndex;
    for (int i = startIndex; i < endIndex; ++i) {
        activePalette[i] = ((int)(byte)((ushort)(B * Alpha + (0xFF - Alpha) * activePalette32[i].r) >> 8) >> 3)
                           | 32 * ((int)(byte)((ushort)(G * Alpha + (0xFF - Alpha) * activePalette32[i].g) >> 8) >> 2)
                           | ((ushort)((int)(byte)((ushort)(R * Alpha + (0xFF - Alpha) * activePalette32[i].b) >> 8) >> 3) << 11);
    }
}