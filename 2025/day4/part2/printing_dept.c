#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_SIZE 2048 // Maximum expected grid size

char grid[MAX_SIZE][MAX_SIZE];
bool to_remove[MAX_SIZE][MAX_SIZE]; // Map to store which cells change in a step
int rows = 0;
int cols = 0;

int main() {
  // 1. Read the input
  while (scanf("%s", grid[rows]) == 1) {
    rows++;
  }
  if (rows > 0)
    cols = strlen(grid[0]);

  long long total_removed_count = 0;

  while (true) {
    bool changed = false;
    int removed_this_step = 0;

    // Reset the removal map for this new step
    memset(to_remove, 0, sizeof(to_remove));

    // 2. Identify all rolls to be removed (based on CURRENT grid state)
    for (int r = 0; r < rows; r++) {
      for (int c = 0; c < cols; c++) {

        if (grid[r][c] == '@') {
          int neighbors = 0;

          // Check all 8 neighbors
          for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
              if (dr == 0 && dc == 0)
                continue;

              int nr = r + dr;
              int nc = c + dc;

              // Bounds check
              if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                // Only current paper rolls count as blocking neighbors
                if (grid[nr][nc] == '@') {
                  neighbors++;
                }
              }
            }
          }

          // Condition: Fewer than 4 neighbors
          if (neighbors < 4) {
            to_remove[r][c] = true;
            changed = true;
            removed_this_step++;
          }
        }
      }
    }

    // 3. Update Phase: Remove them all at once
    if (!changed) {
      break; // Stop if no changes occurred
    }

    for (int r = 0; r < rows; r++) {
      for (int c = 0; c < cols; c++) {
        if (to_remove[r][c]) {
          // Turn it into empty space so it doesn't count as a neighbor in the
          // next step
          grid[r][c] = '.';
        }
      }
    }

    total_removed_count += removed_this_step;
  }

  printf("Total rolls removed: %lld\n", total_removed_count);

  return 0;
}