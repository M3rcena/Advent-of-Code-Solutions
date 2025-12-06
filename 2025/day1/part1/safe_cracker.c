#include <stdio.h>

int main() {
  // The dial starts pointing at 50
  int current_pos = 50;
  int zero_hits = 0;

  char direction;
  int distance;

  // Read input line by line.
  while (scanf(" %c%d", &direction, &distance) == 2) {

    if (direction == 'R') {
      // Right goes toward higher numbers
      current_pos = (current_pos + distance) % 100;
    } else if (direction == 'L') {
      // Left goes toward lower numbers
      current_pos = (current_pos - distance) % 100;
      if (current_pos < 0) {
        current_pos += 100;
      }
    }

    // Check if the dial landed on 0
    if (current_pos == 0) {
      zero_hits++;
    }
  }

  printf("The password is: %d\n", zero_hits);

  return 0;
}