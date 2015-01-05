#ifndef GB_ROM_INFO_H_
#define GB_ROM_INFO_H_

#include <glib.h>

#include "gb.h"

/* This struct comes from an offset of 0x104 into the rom */
typedef struct GBRomInfo {
  guchar nintendo_graphic[0x30];
  char title[0x10]; /* not necessarily null terminated. Padded with zeroes. */
  guchar color_flag; /* 0x80 for color gameboy, 0x00 otherwise */
  char new_licensee_code[2];
  guchar super_gameboy_flag; /* 0x03 for super gameboy, 0x00 otherwise */
  /*
   * 0x00 - ROM ONLY
   * 0x01 - ROM+MBC1
   * 0x02 - ROM+MBC1+RAM
   * 0x03 - ROM+MBC1+RAM+BATT
   * 0x04 - invalid
   * 0x05 - ROM+MBC2
   * 0x06 - ROM+MBC2+BATT
   */
  guchar cartridge_type;
  guchar rom_size;
  guchar ram_size;
  guchar destination_code; /* 0 for japanese, 1 for non-japanese */
  /* 0x33 check new_licensee_code, 0x79 accolate, 0xA4 Konami */
  guchar old_licensee_code;
  guchar mask_rom_version_number;
  guchar complement_check;
  /* the higher order byte is first here despite the fact that the gameboy
   * is little endian
   */
  guchar checksum[2];
} GBRomInfo;

void gb_rominfo_load(GBRomInfo *rom_info, GBRomBank *bank_zero);
char *gb_rominfo_human_readable(GBRomInfo *rom_info);

#endif /* GB_ROM_INFO_H_ */
