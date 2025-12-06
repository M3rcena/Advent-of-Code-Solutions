#include <stdio.h>
#include <string.h>

int main() {
  char line[4096]; // Buffer to hold one line of input
  long long total_output = 0;

  // Read input string by string (assumes one bank per line)
  while (scanf("%s", line) == 1) {
    int len = strlen(line);
    int max_bank_joltage = 0;

    // Iterate through every possible pair of batteries
    for (int i = 0; i < len - 1; i++) {
      for (int j = i + 1; j < len; j++) {

        // Convert characters to integer digits
        int tens = line[i] - '0';
        int ones = line[j] - '0';

        int current_joltage = (tens * 10) + ones;

        if (current_joltage > max_bank_joltage) {
          max_bank_joltage = current_joltage;
        }
      }
    }

    total_output += max_bank_joltage;
  }

  printf("Total output joltage: %lld\n", total_output);

  return 0;
}