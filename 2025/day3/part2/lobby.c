#include <stdio.h>
#include <string.h>

int main() {
  char line[4096];
  long long total_output = 0;

  // Read input line by line
  while (scanf("%s", line) == 1) {
    int len = strlen(line);

    // Safety check: if line is too short, skip (shouldn't happen in valid
    // input)
    if (len < 12)
      continue;

    int last_picked_index = -1;
    long long current_bank_val = 0;

    // We need to pick exactly 12 digits
    for (int needed = 12; needed > 0; needed--) {
      int search_start = last_picked_index + 1;
      int search_end = len - needed;

      int best_digit = -1;
      int best_index = -1;

      // Find the largest digit in this window
      for (int i = search_start; i <= search_end; i++) {
        int val = line[i] - '0';

        // If we find a larger digit, pick it
        if (val > best_digit) {
          best_digit = val;
          best_index = i;

          if (val == 9)
            break;
        }
      }

      // Append the digit to our result
      current_bank_val = (current_bank_val * 10) + best_digit;

      last_picked_index = best_index;
    }

    total_output += current_bank_val;
  }

  printf("Total output joltage (Part 2): %lld\n", total_output);

  return 0;
}