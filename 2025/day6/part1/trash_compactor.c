#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 1000
#define MAX_COLS 4096 // Handles very wide inputs

char grid[MAX_ROWS][MAX_COLS];
int num_rows = 0;
int max_width = 0;

// Helper to check if a specific vertical column is entirely empty (spaces)
bool is_col_empty(int col_index) {
  for (int r = 0; r < num_rows; r++) {
    // Check bounds (in case row is shorter than col_index)
    if (col_index < MAX_COLS && grid[r][col_index] != ' ' &&
        grid[r][col_index] != '\0' && grid[r][col_index] != '\n') {
      return false;
    }
  }
  return true;
}

int main() {
  // 1. Initialize grid with spaces
  for (int i = 0; i < MAX_ROWS; i++) {
    for (int j = 0; j < MAX_COLS; j++) {
      grid[i][j] = ' ';
    }
  }

  // 2. Read Input
  while (fgets(grid[num_rows], MAX_COLS, stdin)) {
    int len = strlen(grid[num_rows]);
    if (len > max_width)
      max_width = len;
    // Remove newline for cleaner processing
    if (grid[num_rows][len - 1] == '\n')
      grid[num_rows][len - 1] = ' ';
    num_rows++;
  }

  long long grand_total = 0;
  int c = 0;

  // 3. Scan columns to find problems
  while (c < max_width) {
    // If this column is empty, just move to the next
    if (is_col_empty(c)) {
      c++;
      continue;
    }

    int start_col = c;
    int end_col = c;

    // Find where this block ends (next empty column or end of line)
    while (end_col < max_width && !is_col_empty(end_col)) {
      end_col++;
    }

    // We need to extract numbers and the operator
    long long numbers[100];
    int num_count = 0;
    char operator = '?';

    for (int r = 0; r < num_rows; r++) {
      // Create a temporary string for just this slice of the row
      char buffer[256];
      int buf_idx = 0;
      bool has_content = false;

      for (int k = start_col; k < end_col; k++) {
        char ch = grid[r][k];
        if (ch != ' ') {
          if (isdigit(ch)) {
            buffer[buf_idx++] = ch;
            has_content = true;
          } else if (ch == '+' || ch == '*') {
            operator = ch;
          }
        }
      }
      buffer[buf_idx] = '\0';

      // If we found digits in this row slice, parse them
      if (has_content) {
        numbers[num_count++] = atoll(buffer);
      }
    }

    // 4. Perform the math
    long long block_result = 0;

    if (operator == '+') {
      block_result = 0;
      for (int i = 0; i < num_count; i++)
        block_result += numbers[i];
    } else if (operator == '*') {
      block_result = 1;
      for (int i = 0; i < num_count; i++)
        block_result *= numbers[i];
    }

    grand_total += block_result;

    // Move cursor past this block
    c = end_col;
  }

  printf("Grand Total: %lld\n", grand_total);

  return 0;
}