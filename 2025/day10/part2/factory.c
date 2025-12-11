/**
 * Advent of Code 2025 - Day 10, Part 2: Factory Joltages
 * * Problem Description:
 * We need to configure machines to reach specific target integer values
 * (joltages). Each button press adds +1 to specific counters. We must find the
 * non-negative integer number of presses for each button such that all counters
 * match their targets exactly, minimizing the total number of presses.
 * * Mathematical Model:
 * This is a system of Linear Diophantine Equations: Ax = b
 * subject to x_i >= 0 (non-negative) and x_i in Z (integers).
 * - A is the binary matrix of button effects.
 * - b is the target vector.
 * - x is the vector of press counts.
 * * Algorithm:
 * 1. Gaussian Elimination: Converts the matrix A to Reduced Row Echelon Form
 * (RREF).
 * 2. Variable Classification: Identifies 'Pivot' variables (dependent) and
 * 'Free' variables (independent).
 * 3. Recursive Search: Since the system is underdetermined, we iterate through
 * possible values for the 'Free' variables. For each guess, we calculate the
 * required values of the 'Pivot' variables.
 * 4. Validation: A solution is valid only if all calculated Pivot variables are
 * non-negative integers.
 */

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VARS 100
#define EPSILON 1e-9

/**
 * System Structure
 * Represents the linear equation system Ax = b.
 */
typedef struct {
  int rows;                     // Number of equations (counters)
  int cols;                     // Number of variables (buttons)
  double A[MAX_VARS][MAX_VARS]; // Coefficient Matrix
  double b[MAX_VARS];           // Target Vector
  int pivot_col[MAX_VARS];      // Index of the pivot column for each row (-1 if
                                // none)
  int free_vars[MAX_VARS];      // List of column indices corresponding to free
                                // variables
  int num_free;                 // Count of free variables
} System;

// Tracks the global minimum sum of presses found across all valid solutions for
// a machine
long long global_min_presses = -1;

/**
 * Helper: Checks if a floating point value is effectively an integer.
 * Used because Gaussian elimination introduces floating point division.
 */
bool is_int(double v) { return fabs(v - round(v)) < EPSILON; }

/**
 * Recursive Search for Free Variables
 * * @param sys The system struct containing RREF matrix.
 * @param free_idx The index in the free_vars array we are currently guessing.
 * @param solution The array storing current values for all variables (x).
 */
void search(System *sys, int free_idx, double *solution) {

  // Base Case: All free variables have been assigned a value.
  // Now we compute the dependent (Pivot) variables to see if this configuration
  // works.
  if (free_idx == sys->num_free) {
    double current_sol[MAX_VARS];
    // Copy current guesses for free variables
    for (int i = 0; i < sys->cols; i++)
      current_sol[i] = solution[i];

    long long current_sum = 0;
    bool valid = true;

    // Calculate Pivot Variables using back-substitution logic.
    // In RREF, the equation for row r is: x_{pivot} + sum(A[r][f] * x_f) = b[r]
    // Therefore: x_{pivot} = b[r] - sum(A[r][f] * x_f)
    for (int r = 0; r < sys->rows; r++) {
      int p = sys->pivot_col[r];

      // If this row has no pivot (all zeros), check consistency (0 = b[r])
      if (p == -1) {
        if (fabs(sys->b[r]) > EPSILON) {
          valid = false;
          break;
        }
        continue;
      }

      double val = sys->b[r];

      // Subtract contributions from free variables
      for (int j = 0; j < sys->num_free; j++) {
        int f = sys->free_vars[j];
        if (fabs(sys->A[r][f]) > EPSILON) {
          val -= sys->A[r][f] * solution[f];
        }
      }

      // Constraint Check: Pivot must be a non-negative Integer
      if (val < -EPSILON || !is_int(val)) {
        valid = false;
        break;
      }
      current_sol[p] = round(val);
    }

    // Final Summation and Update Global Min
    if (valid) {
      for (int i = 0; i < sys->cols; i++) {
        if (current_sol[i] < 0) {
          valid = false;
          break;
        } // Safety check
        current_sum += (long long)current_sol[i];
      }
    }

    if (valid) {
      if (global_min_presses == -1 || current_sum < global_min_presses) {
        global_min_presses = current_sum;
      }
    }
    return;
  }

  // Recursive Step: Iterate possible values for the current free variable.
  int f_var = sys->free_vars[free_idx];

  // Search Range: 0 to 200.
  // Free variables represent degrees of freedom in the system.
  // In these puzzles, the required adjustment via free variables is typically
  // small.
  for (int val = 0; val <= 200; val++) {
    solution[f_var] = (double)val;

    // Optimization: We could add pruning here if the partial sum already
    // exceeds global_min_presses, but given the small depth (num_free is
    // usually low), raw recursion is fast enough.

    search(sys, free_idx + 1, solution);
  }
  solution[f_var] = 0; // Reset for cleanliness
}

/**
 * Solves the linear system using Gaussian Elimination + Search.
 */
void solve_system(System *sys) {
  // --- Phase 1: Gaussian Elimination (Forward) ---
  int pivot_row = 0;
  for (int col = 0; col < sys->cols && pivot_row < sys->rows; col++) {
    // Find a pivot row for this column
    int sel = -1;
    for (int i = pivot_row; i < sys->rows; i++) {
      if (fabs(sys->A[i][col]) > EPSILON) {
        sel = i;
        break;
      }
    }
    if (sel == -1)
      continue; // Column is independent (Free Variable)

    // Swap pivot row to current position
    for (int j = col; j < sys->cols; j++) {
      double tmp = sys->A[pivot_row][j];
      sys->A[pivot_row][j] = sys->A[sel][j];
      sys->A[sel][j] = tmp;
    }
    double tmp_b = sys->b[pivot_row];
    sys->b[pivot_row] = sys->b[sel];
    sys->b[sel] = tmp_b;

    // Normalize row (make pivot element 1.0)
    double div = sys->A[pivot_row][col];
    for (int j = col; j < sys->cols; j++)
      sys->A[pivot_row][j] /= div;
    sys->b[pivot_row] /= div;

    // Eliminate column entries in other rows
    for (int i = 0; i < sys->rows; i++) {
      if (i != pivot_row && fabs(sys->A[i][col]) > EPSILON) {
        double factor = sys->A[i][col];
        for (int j = col; j < sys->cols; j++)
          sys->A[i][j] -= factor * sys->A[pivot_row][j];
        sys->b[i] -= factor * sys->b[pivot_row];
      }
    }
    sys->pivot_col[pivot_row] = col;
    pivot_row++;
  }

  // --- Phase 2: Identify Free Variables ---
  sys->num_free = 0;
  int is_pivot[MAX_VARS] = {0};

  // Map rows to their pivot columns
  for (int i = 0; i < sys->rows; i++) {
    if (fabs(sys->b[i]) < EPSILON)
      sys->b[i] = 0.0; // Clean small errors

    int p = -1;
    for (int j = 0; j < sys->cols; j++) {
      if (fabs(sys->A[i][j]) > EPSILON) { // First non-zero is pivot
        p = j;
        is_pivot[j] = 1;
        break;
      }
    }
    sys->pivot_col[i] = p;
  }

  // Any column that isn't a pivot is a Free Variable
  for (int j = 0; j < sys->cols; j++) {
    if (!is_pivot[j]) {
      sys->free_vars[sys->num_free++] = j;
    }
  }

  // --- Phase 3: Search for Minimal Integer Solution ---
  double solution[MAX_VARS] = {0};
  global_min_presses = -1;
  search(sys, 0, solution);
}

/**
 * Parses input string into System struct.
 * Expected format includes (...) for buttons and {...} for targets.
 */
void parse_line(char *line, System *sys) {
  sys->rows = 0;
  sys->cols = 0;
  memset(sys->A, 0, sizeof(sys->A));
  memset(sys->b, 0, sizeof(sys->b));

  // Parse Buttons: "(1,2) (3)"
  char *p = line;
  char *brace = strchr(line, '{');

  while ((p = strchr(p, '(')) != NULL) {
    if (brace && p > brace)
      break; // Don't parse inside {}

    char *end = strchr(p, ')');
    if (!end)
      break;

    char buf[256];
    int l = end - p - 1;
    if (l > 255)
      l = 255;
    strncpy(buf, p + 1, l);
    buf[l] = 0;

    char *tok = strtok(buf, ",");
    while (tok) {
      int r = atoi(tok);
      // Expand rows if a button affects a higher index counter
      if (r >= sys->rows)
        sys->rows = r + 1;
      sys->A[r][sys->cols] = 1.0;
      tok = strtok(NULL, ",");
    }
    sys->cols++; // Move to next button (variable)
    p = end + 1;
  }

  // Parse Targets: "{3, 5, ...}"
  if (brace) {
    char *end = strchr(brace, '}');
    if (end) {
      char buf[256];
      int l = end - brace - 1;
      if (l > 255)
        l = 255;
      strncpy(buf, brace + 1, l);
      buf[l] = 0;

      int idx = 0;
      char *tok = strtok(buf, ",");
      while (tok) {
        sys->b[idx] = atof(tok);
        if (idx >= sys->rows)
          sys->rows = idx + 1;
        idx++;
        tok = strtok(NULL, ",");
      }
    }
  }
}

int main() {
  char line[4096];
  long long grand_total = 0;

  // Read Input
  while (fgets(line, sizeof(line), stdin)) {
    if (strlen(line) < 3)
      continue;
    // Basic validation that line contains machine data
    if (!strchr(line, '(') || !strchr(line, '{'))
      continue;

    System sys;
    parse_line(line, &sys);

    solve_system(&sys);

    if (global_min_presses != -1) {
      grand_total += global_min_presses;
    }
  }

  printf("Total presses: %lld\n", grand_total);
  return 0;
}