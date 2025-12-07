#include <stdio.h>
#include <string.h>

#define MAX_COLS 2048
#define MAX_ROWS 2048

char grid[MAX_ROWS][MAX_COLS];

unsigned long long current_counts[MAX_COLS];
unsigned long long next_counts[MAX_COLS];

int main() {
  int rows = 0;
  int cols = 0;

  // 1. Read input
  while (scanf("%s", grid[rows]) == 1) {
    rows++;
  }
  if (rows > 0)
    cols = strlen(grid[0]);

  // 2. Initialize: Find 'S' and start with 1 timeline there
  memset(current_counts, 0, sizeof(current_counts));
  for (int c = 0; c < cols; c++) {
    if (grid[0][c] == 'S') {
      current_counts[c] = 1;
    }
  }

  unsigned long long total_finished_timelines = 0;

  // 3. Process row by row
  for (int r = 0; r < rows; r++) {
    // Reset the counts for the next row
    memset(next_counts, 0, sizeof(next_counts));

    for (int c = 0; c < cols; c++) {
      // Only process if there are active timelines in this column
      if (current_counts[c] > 0) {

        char cell = grid[r][c];

        if (cell == '^') {
          // Splitter: Timelines split Left and Right

          // Left child
          if (c - 1 >= 0) {
            next_counts[c - 1] += current_counts[c];
          } else {
            // Exited off the left edge
            total_finished_timelines += current_counts[c];
          }

          // Right child
          if (c + 1 < cols) {
            next_counts[c + 1] += current_counts[c];
          } else {
            // Exited off the right edge
            total_finished_timelines += current_counts[c];
          }
        } else {
          // Empty space (or Start 'S' if checking row 0 logic strictness)
          // Timelines continue straight down
          next_counts[c] += current_counts[c];
        }
      }
    }

    // Advance to the next row state
    // (For the very last row, next_counts contains those that fell off the
    // bottom)
    memcpy(current_counts, next_counts, sizeof(current_counts));
  }

  // 4. Sum up all timelines that made it past the bottom row
  for (int c = 0; c < cols; c++) {
    total_finished_timelines += current_counts[c];
  }

  printf("Total distinct timelines: %llu\n", total_finished_timelines);

  return 0;
}