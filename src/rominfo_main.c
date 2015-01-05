#include <stdio.h>

#include "gb.h"
#include "rominfo.h"

int main(int argc, char **argv) {
  g_assert(argc == 2);
  char *filename = argv[1];
  GBRomBank *bank_zero = gb_load_bank_zero(filename);
  GBRomInfo rom_info;
  gb_rominfo_load(&rom_info, bank_zero);
  g_free(bank_zero);

  printf("title %s\n", rom_info.title);
}
