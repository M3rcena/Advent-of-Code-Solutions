#include <stdio.h>
#include <string.h>

#define MAX_SIZE 2048 // Maximum grid size assumption

char grid[MAX_SIZE][MAX_SIZE];
int rows = 0;
int cols = 0;

int main() {
  // 1. Read the input into the grid
  while (scanf("%s", grid[rows]) == 1) {
    rows++;
  }

  // Determine the width based on the first row
  if (rows > 0) {
    cols = strlen(grid[0]);
  }

  int accessible_count = 0;

  // 2. Iterate through every cell
  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {

      if (grid[r][c] == '@') {
        int neighbors = 0;

        // 3. Check all 8 surrounding cells
        for (int dr = -1; dr <= 1; dr++) {
          for (int dc = -1; dc <= 1; dc++) {

            // Skip the center cell
            if (dr == 0 && dc == 0)
              continue;

            int nr = r + dr;
            int nc = c + dc;

            // Check boundaries to avoid accessing outside array
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
              if (grid[nr][nc] == '@') {
                neighbors++;
              }
            }
          }
        }

        // 4. Check the "fewer than 4" condition
        if (neighbors < 4) {
          accessible_count++;
        }
      }
    }
  }

  printf("Accessible paper rolls: %d\n", accessible_count);

  return 0;
}