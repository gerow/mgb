#include "rominfo.h"

#include <string.h>

void gb_rominfo_load(GBRomInfo *rom_info, GBRomBank *bank_zero) {
  guchar *ptr = bank_zero->data;
  memcpy(rom_info, ptr + 0x104, sizeof(*rom_info));
}