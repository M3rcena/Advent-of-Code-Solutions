#include <stdio.h>

int main() {
  int current_pos = 50;    // Dial starts at 50
  long long zero_hits = 0; // Use long long just in case the number gets large

  char direction;
  int distance;

  while (scanf(" %c%d", &direction, &distance) == 2) {

    // 1. Calculate Full Loops
    int full_loops = distance / 100;
    zero_hits += full_loops;

    // 2. Handle the remaining distance
    int remainder = distance % 100;

    if (direction == 'R') {
      // Check if we cross/hit 100 (which is 0)
      if (current_pos + remainder >= 100) {
        zero_hits++;
      }
      // Update position
      current_pos = (current_pos + remainder) % 100;
    } else if (direction == 'L') {
      if (current_pos > 0 && current_pos - remainder <= 0) {
        zero_hits++;
      }

      // Update position
      current_pos = (current_pos - remainder) % 100;
      if (current_pos < 0) {
        current_pos += 100;
      }
    }
  }

  printf("The password is: %lld\n", zero_hits);

  return 0;
}