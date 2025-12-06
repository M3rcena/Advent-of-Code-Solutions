#include <stdio.h>

// Function to check if a number is composed of a sequence repeated twice
// Returns 1 if invalid (meaning it fits the puzzle description), 0 otherwise.
int is_invalid_id(long long n) {
  if (n <= 0)
    return 0;

  // 1. Count digits
  long long temp = n;
  int digits = 0;
  while (temp > 0) {
    temp /= 10;
    digits++;
  }

  // If odd number of digits, it cannot be split into two equal parts
  if (digits % 2 != 0)
    return 0;

  // 2. Calculate the divisor to split the number
  // For 4 digits, we need 10^2 = 100. For 6 digits, 10^3 = 1000.
  long long divisor = 1;
  for (int i = 0; i < digits / 2; i++) {
    divisor *= 10;
  }

  // 3. Split and compare
  long long upper_half = n / divisor;
  long long lower_half = n % divisor;

  return (upper_half == lower_half);
}

int main() {
  long long start, end;
  long long total_sum = 0;
  char separator;

  // Read loop:
  // scanf looks for "Long-Long".
  // We then look for the comma separator.
  while (scanf("%lld-%lld", &start, &end) == 2) {

    // Check every number in the range
    for (long long id = start; id <= end; id++) {
      if (is_invalid_id(id)) {
        total_sum += id;
      }
    }

    // Consume the next character (likely a comma, newline, or EOF)
    // If we hit EOF or a newline, we stop assuming the input line is done.
    separator = getchar();
    if (separator == '\n' || separator == EOF) {
      break;
    }
  }

  printf("Sum of invalid IDs: %lld\n", total_sum);

  return 0;
}