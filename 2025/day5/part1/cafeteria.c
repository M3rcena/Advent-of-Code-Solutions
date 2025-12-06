#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define MAX_RANGES 100000 // Sufficient space for many ranges

typedef struct {
  long long start;
  long long end;
} Range;

int main() {
  // Array to store the "Fresh" ranges
  Range ranges[MAX_RANGES];
  int range_count = 0;

  char line[256];
  long long fresh_count = 0;

  // Read the input line by line
  while (fgets(line, sizeof(line), stdin)) {

    // 1. Check if line is empty or just whitespace
    int has_digit = 0;
    for (int i = 0; line[i]; i++) {
      if (isdigit(line[i])) {
        has_digit = 1;
        break;
      }
    }
    if (!has_digit)
      continue; // Skip blank lines

    // 2. Identify line type
    // If it contains a hyphen '-', it is a Range (e.g., "3-5")
    if (strchr(line, '-') != NULL) {
      long long s, e;
      if (sscanf(line, "%lld-%lld", &s, &e) == 2) {
        if (range_count < MAX_RANGES) {
          ranges[range_count].start = s;
          ranges[range_count].end = e;
          range_count++;
        }
      }
    }
    // Otherwise, it is an ID to check (e.g., "5")
    else {
      long long id;
      if (sscanf(line, "%lld", &id) == 1) {

        // Check this ID against all stored ranges
        int is_fresh = 0;
        for (int i = 0; i < range_count; i++) {
          if (id >= ranges[i].start && id <= ranges[i].end) {
            is_fresh = 1;
            break;
          }
        }

        if (is_fresh) {
          fresh_count++;
        }
      }
    }
  }

  printf("Fresh ingredients: %lld\n", fresh_count);

  return 0;
}