#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_ROWS 1000
#define MAX_COLS 4096

char grid[MAX_ROWS][MAX_COLS];
int num_rows = 0;
int max_width = 0;

// Check if a specific column is completely empty (separator)
bool is_col_empty(int c) {
  for (int r = 0; r < num_rows; r++) {
    char ch = grid[r][c];
    // Treat null terminator and newline as "empty" along with space
    if (ch != ' ' && ch != '\0' && ch != '\n') {
      return false;
    }
  }
  return true;
}

int main() {
  // 1. Initialize grid with spaces
  // This ensures that areas past the end of short lines are treated as empty
  for (int r = 0; r < MAX_ROWS; r++) {
    for (int c = 0; c < MAX_COLS; c++) {
      grid[r][c] = ' ';
    }
  }

  // 2. Read Input
  while (fgets(grid[num_rows], MAX_COLS, stdin)) {
    int len = strlen(grid[num_rows]);

    // Remove trailing newline for cleaner logic
    if (len > 0 && grid[num_rows][len - 1] == '\n') {
      grid[num_rows][len - 1] = ' ';
      len--;
    }

    if (len > max_width)
      max_width = len;
    num_rows++;
  }

  long long grand_total = 0;
  int c = 0;

  // 3. Scan columns to find problem blocks
  while (c < max_width) {

    // Skip empty separator columns
    if (is_col_empty(c)) {
      c++;
      continue;
    }

    // Identified a block start
    int start_col = c;
    int end_col = c;

    // Find the end of this block
    while (end_col < max_width && !is_col_empty(end_col)) {
      end_col++;
    }

    // 4. Process the Block (start_col to end_col)
    long long block_numbers[100]; // Assuming max 100 numbers per problem
    int count = 0;
    char op = '+'; // Default to + if not found (though input always has one)

    // Iterate through every column in this block
    for (int curr = start_col; curr < end_col; curr++) {
      long long current_num = 0;
      bool has_digit = false;

      // Scan Top-to-Bottom to build the number
      for (int r = 0; r < num_rows; r++) {
        char ch = grid[r][curr];

        if (isdigit(ch)) {
          current_num = current_num * 10 + (ch - '0');
          has_digit = true;
        } else if (ch == '+' || ch == '*') {
          op = ch; // Found the operator
        }
      }

      // If this column contained digits, add it to our list
      if (has_digit) {
        block_numbers[count++] = current_num;
      }
    }

    // 5. Calculate Result
    long long block_result = 0;

    if (count > 0) {
      if (op == '*') {
        block_result = 1;
        for (int i = 0; i < count; i++) {
          block_result *= block_numbers[i];
        }
      } else {
        // Addition
        block_result = 0;
        for (int i = 0; i < count; i++) {
          block_result += block_numbers[i];
        }
      }
    }

    grand_total += block_result;

    // Move cursor to the end of this block
    c = end_col;
  }

  printf("Grand Total (Part 2): %lld\n", grand_total);

  return 0;
}