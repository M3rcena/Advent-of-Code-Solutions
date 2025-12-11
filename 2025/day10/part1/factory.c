/**
 * Advent of Code 2025 - Day 10: Factory
 * * Problem: Minimize the number of button presses to achieve a specific light
 * configuration.
 * * Mathematical Model:
 * This problem describes a system of linear equations over the Finite Field
 * GF(2).
 * - Each light represents an equation.
 * - Each button represents a variable (x_i) which can be 0 (unpressed) or 1
 * (pressed).
 * - The operation is XOR (addition modulo 2).
 * * The system is Ax = b, where:
 * - A is an (n x m) matrix where A[i][j] = 1 if button j toggles light i.
 * - b is a vector of length n representing the target state of the lights.
 * - x is the solution vector we are solving for.
 * * Algorithm: Gaussian Elimination optimized for minimal Hamming weight.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LIGHTS 100
#define MAX_BUTTONS 100

/**
 * Structure representing the Linear System Ax = b
 */
typedef struct {
  int n;                          // Number of equations (lights)
  int m;                          // Number of variables (buttons)
  int A[MAX_LIGHTS][MAX_BUTTONS]; // The coefficient matrix (mod 2)
  int b[MAX_LIGHTS];              // The constant vector (target states)
} System;

/**
 * Parses a single line of input to populate the linear system.
 * Format: "[.##.] (1,2) (3) ..."
 * @param line The raw input string.
 * @param sys Pointer to the System struct to populate.
 */
void read_line(char *line, System *sys) {
  sys->n = 0;
  sys->m = 0;
  memset(sys->A, 0, sizeof(sys->A));
  memset(sys->b, 0, sizeof(sys->b));

  // 1. Parse Indicator Lights (The target vector b)
  // '#' denotes On (1), '.' denotes Off (0)
  char *p = strchr(line, '[');
  if (!p)
    return;
  p++;
  int idx = 0;
  while (*p && *p != ']') {
    if (*p == '#')
      sys->b[idx] = 1;
    else if (*p == '.')
      sys->b[idx] = 0;
    else {
      p++;
      continue;
    } // Skip extraneous chars
    idx++;
    p++;
  }
  sys->n = idx;

  // 2. Parse Buttons (The columns of matrix A)
  // Format: (0,2,3) means this button affects lights 0, 2, and 3.
  p = strchr(p, '(');
  while (p && *p) {
    if (*p == '(') {
      p++;
      char num[10];
      int num_idx = 0;
      // Parse comma-separated numbers within parentheses
      while (*p && *p != ')') {
        if (*p >= '0' && *p <= '9') {
          num[num_idx++] = *p;
        } else if (*p == ',' || *p == ')') {
          if (num_idx > 0) {
            num[num_idx] = '\0';
            int light = atoi(num);
            if (light < sys->n) {
              sys->A[light][sys->m] = 1;
            }
            num_idx = 0;
          }
        }
        p++;
      }
      // Catch the last number before the closing parenthesis
      if (num_idx > 0) {
        num[num_idx] = '\0';
        int light = atoi(num);
        if (light < sys->n) {
          sys->A[light][sys->m] = 1;
        }
      }
      sys->m++; // Move to next button (variable)
    }
    p = strchr(p, '(');
  }
}

/**
 * Performs Gaussian Elimination over GF(2) to solve Ax = b.
 * * Since the system might be underdetermined (multiple solutions), this
 * function employs a brute-force search over the "free variables" to find the
 * solution with the minimum Hamming weight (fewest button presses).
 * * @param sys The linear system.
 * @param sol Output array to store the best solution found (0s and 1s).
 * @return The rank of the matrix, or -1 if the system is inconsistent (no
 * solution).
 */
int gauss(System *sys, int *sol) {
  int n = sys->n, m = sys->m;

  // 'where' array maps columns (variables) to the row of their pivot.
  // where[j] = i means the pivot for variable x_j is at row i.
  int *where = malloc(m * sizeof(int));
  for (int i = 0; i < m; i++)
    where[i] = -1;

  int row = 0;

  // --- Phase 1: Forward Elimination (Row Echelon Form) ---
  for (int col = 0; col < m && row < n; col++) {
    // Find a pivot row for the current column
    int sel = -1;
    for (int i = row; i < n; i++) {
      if (sys->A[i][col]) {
        sel = i;
        break;
      }
    }
    if (sel == -1)
      continue; // No pivot in this column, it's a free variable

    // Swap the pivot row with the current row
    for (int j = col; j < m; j++) {
      int tmp = sys->A[row][j];
      sys->A[row][j] = sys->A[sel][j];
      sys->A[sel][j] = tmp;
    }
    int tmp = sys->b[row];
    sys->b[row] = sys->b[sel];
    sys->b[sel] = tmp;

    where[col] = row;

    // Eliminate this column from all OTHER rows (making it a unit vector
    // column) This effectively performs Gauss-Jordan to simplify
    // back-substitution later.
    for (int i = 0; i < n; i++) {
      if (i != row && sys->A[i][col]) {
        for (int j = col; j < m; j++) {
          sys->A[i][j] ^=
              sys->A[row][j]; // XOR is subtraction/addition in GF(2)
        }
        sys->b[i] ^= sys->b[row];
      }
    }
    row++;
  }

  // --- Phase 2: Check Consistency ---
  // If a row is all zeros (A[i] == 0) but the target is 1 (b[i] == 1),
  // we have 0 = 1, which is impossible.
  for (int i = row; i < n; i++) {
    if (sys->b[i]) {
      free(where);
      return -1; // No solution
    }
  }

  // --- Phase 3: Construct Minimal Solution ---

  // Initial solution: set all variables to 0, then solve for pivots.
  for (int i = 0; i < m; i++)
    sol[i] = 0;
  for (int i = 0; i < m; i++) {
    if (where[i] != -1) {
      sol[i] = sys->b[where[i]];
    }
  }

  // Identify Free Variables (columns without pivots).
  // These variables can be arbitrarily set to 0 or 1.
  // Changing a free variable requires adjusting dependent variables (pivots) to
  // keep equations valid.
  int free_vars = 0;
  int free_idx[MAX_BUTTONS];
  for (int i = 0; i < m; i++) {
    if (where[i] == -1) {
      free_idx[free_vars++] = i;
    }
  }

  // Calculate weight of the base solution
  int min_weight = 0;
  for (int i = 0; i < m; i++)
    min_weight += sol[i];

  int best_sol[MAX_BUTTONS];
  memcpy(best_sol, sol, m * sizeof(int));

  // Optimization: Brute force the Free Variables to minimize total presses.
  // If there are k free variables, there are 2^k valid solutions (the Null
  // Space). We expect 'free_vars' to be small (<= 15) for this optimization to
  // run quickly.
  if (free_vars <= 15) {
    for (int mask = 0; mask < (1 << free_vars); mask++) {
      int temp_sol[MAX_BUTTONS];
      memcpy(temp_sol, sol, m * sizeof(int));

      // 1. Set free variables based on the current mask
      for (int f = 0; f < free_vars; f++) {
        if (mask & (1 << f)) {
          temp_sol[free_idx[f]] = 1;
        } else {
          temp_sol[free_idx[f]] = 0;
        }
      }

      // 2. Re-calculate dependent variables (pivots) based on free variable
      // choices
      for (int i = 0; i < m; i++) {
        if (where[i] != -1) { // If 'i' is a pivot variable
          int val = sys->b[where[i]];
          // The value of pivot i depends on the target b and the values of
          // other cols in this row
          for (int j = 0; j < m; j++) {
            if (j != i && sys->A[where[i]][j]) {
              val ^= temp_sol[j];
            }
          }
          temp_sol[i] = val;
        }
      }

      // 3. Check if this combination yields a lower total weight
      int weight = 0;
      for (int i = 0; i < m; i++)
        weight += temp_sol[i];

      if (weight < min_weight) {
        min_weight = weight;
        memcpy(best_sol, temp_sol, m * sizeof(int));
      }
    }
  }

  memcpy(sol, best_sol, m * sizeof(int));
  free(where);
  return row; // Returns rank (number of independent equations)
}

/**
 * Wrapper to solve the system and sum the result.
 */
int solve_system(System *sys) {
  int sol[MAX_BUTTONS];
  int rank = gauss(sys, sol);

  if (rank == -1)
    return -1; // No solution found

  int presses = 0;
  for (int i = 0; i < sys->m; i++) {
    presses += sol[i];
  }
  return presses;
}

int main() {
  char line[1000];
  int total_presses = 0;

  // Read input line by line until EOF
  while (fgets(line, sizeof(line), stdin)) {
    // Skip short or empty lines
    if (strlen(line) < 3)
      continue;

    System sys;
    read_line(line, &sys);

    // Solve for the current machine
    int presses = solve_system(&sys);

    // Only add to total if a solution exists (presses >= 0)
    if (presses >= 0) {
      total_presses += presses;
    }
  }

  printf("Total Minimum Presses: %d\n", total_presses);
  return 0;
}