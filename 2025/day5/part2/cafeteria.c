#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RANGES 10000 // Adjust if input is very large

typedef struct {
  long long start;
  long long end;
} Range;

// Comparator function for qsort
int compare_ranges(const void *a, const void *b) {
  Range *r1 = (Range *)a;
  Range *r2 = (Range *)b;
  if (r1->start < r2->start)
    return -1;
  if (r1->start > r2->start)
    return 1;
  return 0;
}

long long max(long long a, long long b) { return (a > b) ? a : b; }

int main() {
  Range ranges[MAX_RANGES];
  int count = 0;
  char line[256];

  // 1. Read and Parse Input
  while (fgets(line, sizeof(line), stdin)) {
    if (strchr(line, '-') != NULL) {
      long long s, e;
      if (sscanf(line, "%lld-%lld", &s, &e) == 2) {
        if (count < MAX_RANGES) {
          ranges[count].start = s;
          ranges[count].end = e;
          count++;
        }
      }
    }
  }

  if (count == 0) {
    printf("No ranges found.\n");
    return 0;
  }

  // 2. Sort ranges by start value
  qsort(ranges, count, sizeof(Range), compare_ranges);

  long long total_fresh = 0;

  // Initialize the "current merged range" with the first one
  long long current_start = ranges[0].start;
  long long current_end = ranges[0].end;

  // 3. Iterate and Merge
  for (int i = 1; i < count; i++) {
    if (ranges[i].start <= current_end + 1) {
      // Extend the current end if the new range goes further
      current_end = max(current_end, ranges[i].end);
    } else {
      // Add the length of the completed block to the total
      total_fresh += (current_end - current_start + 1);

      // Start a new block
      current_start = ranges[i].start;
      current_end = ranges[i].end;
    }
  }

  total_fresh += (current_end - current_start + 1);

  printf("Total fresh IDs covered: %lld\n", total_fresh);

  return 0;
}