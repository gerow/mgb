#ifndef GB_GB_H_
#define GB_GB_H_

#include <glib.h>

typedef struct GBRomBank {
  guchar data[0x4000];
} GBRomBank;


extern guchar gb_expected_nintendo_graphic[0x30];

GBRomBank *gb_load_bank_zero(char *filename);

#endif /* GB_GB_H_ */
