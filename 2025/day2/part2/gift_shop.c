#include <stdio.h>
#include <string.h>

// Function to check if a number is composed of any sequence repeated at least
// twice
int is_recursive_pattern(long long n) {
  char s[32]; // Buffer to hold the number string (long long max is ~19 digits)
  sprintf(s, "%lld", n);
  int len = strlen(s);

  // Try every possible substring length from 1 up to half the total length
  for (int sub_len = 1; sub_len <= len / 2; sub_len++) {

    // The total length must be perfectly divisible by the pattern length
    if (len % sub_len == 0) {
      int pattern_matches = 1;

      for (int i = sub_len; i < len; i++) {
        if (s[i] != s[i - sub_len]) {
          pattern_matches = 0;
          break;
        }
      }

      if (pattern_matches) {
        return 1;
      }
    }
  }
  return 0;
}

int main() {
  long long start, end;
  long long total_sum = 0;
  char separator;

  // Read loop: Look for "Number-Number"
  while (scanf("%lld-%lld", &start, &end) == 2) {

    for (long long id = start; id <= end; id++) {
      if (is_recursive_pattern(id)) {
        total_sum += id;
      }
    }

    // Consume the separator (comma or newline)
    separator = getchar();
    if (separator == '\n' || separator == EOF) {
      break;
    }
  }

  printf("Sum of invalid IDs (Part 2): %lld\n", total_sum);

  return 0;
}