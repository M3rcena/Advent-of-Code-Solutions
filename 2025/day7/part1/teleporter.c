#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_COLS 1024
#define MAX_ROWS 1024

char grid[MAX_ROWS][MAX_COLS];
bool active_beams[MAX_COLS];
bool next_beams[MAX_COLS];

int main() {
  int rows = 0;
  int cols = 0;

  // Read input into grid
  while (scanf("%s", grid[rows]) == 1) {
    rows++;
  }
  if (rows > 0)
    cols = strlen(grid[0]);

  // Initial state: Find the starting beam 'S'
  memset(active_beams, 0, sizeof(active_beams));
  for (int c = 0; c < cols; c++) {
    if (grid[0][c] == 'S') {
      active_beams[c] = true;
      break;
    }
  }

  int split_count = 0;

  // Process row by row starting from the top
  for (int r = 1; r < rows; r++) {
    memset(next_beams, 0, sizeof(next_beams));

    for (int c = 0; c < cols; c++) {
      if (active_beams[c]) {
        // Encounter a splitter: stop current, emit left and right
        if (grid[r][c] == '^') {
          split_count++;
          if (c > 0)
            next_beams[c - 1] = true;
          if (c < cols - 1)
            next_beams[c + 1] = true;
        }
        // Encounter empty space: beam continues downward in current column
        else {
          next_beams[c] = true;
        }
      }
    }

    // Move state to the next row
    memcpy(active_beams, next_beams, sizeof(active_beams));
  }

  printf("Total beam splits: %d\n", split_count);

  return 0;
}