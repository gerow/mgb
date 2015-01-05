#include <glib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "gb.h"

GBRomBank *gb_load_bank_zero(char *filename) {
  int f = open(filename, O_RDONLY);
  if (f == -1) {
    // fail
  }
  GBRomBank *rombuf = g_new0(GBRomBank, 1);
  ssize_t bytes_read = read(f, &rombuf->data, sizeof(rombuf->data));
  if (bytes_read != sizeof(rombuf->data)) {
    g_free(rombuf);
    return NULL;
  }
  if (close(f)) {
    g_free(rombuf);
    return NULL;
  }
  return rombuf;
}