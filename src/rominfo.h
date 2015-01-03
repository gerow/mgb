#ifndef GB_ROM_INFO_H_
#define GB_ROM_INFO_H_

/* This struct comes from an offset of 0x104 into the rom */
typedef struct GBRomInfo {
  gbyte nintendo_graphic[0x30];
  char title[0x10]; /* not necessarily null terminated. Padded with zeroes. */
  gbyte color_flag; /* 0x80 for color gameboy, 0x00 otherwise */
  char new_licensee_code[2];
  gbyte super_gameboy_flag; /* 0x03 for super gameboy, 0x00 otherwise */
  /*
   * 0x00 - ROM ONLY
   * 0x01 - ROM+MBC1
   * 0x02 - ROM+MBC1+RAM
   * 0x03 - ROM+MBC1+RAM+BATT
   * 0x04 - invalid
   * 0x05 - ROM+MBC2
   * 0x06 - ROM+MBC2+BATT
   */
  gbyte cartridge_type;
  gbyte rom_size;
  gbyte ram_size;
  gbyte destination_code; /* 0 for japanese, 1 for non-japanese */
  /* 0x33 check new_licensee_code, 0x79 accolate, 0xA4 Konami */
  gbyte old_licensee_code;
  gbyte mask_rom_version_number;
  gbyte complement_check;
  /* the higher order byte is first here despite the fact that the gameboy
   * is little endian
   */
  gbyte checksum[2];
} GBRomInfo;

void gb_rominfo_load(GBRomInfo *rom_info, GBRomBank *bank_zero);
char *gb_rominfo_human_readable(GBRomInfo *rom_info);

#endif /* GB_ROM_INFO_H_ */
