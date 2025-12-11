/**
 * Advent of Code 2025 - Day 11, Part 2: Reactor
 * * Problem Description:
 * We need to calculate the number of distinct paths in a directed graph from a
 * source node 'svr' to a target node 'out'. Constraint: All valid paths must
 * visit two specific intermediate nodes: 'dac' and 'fft'.
 * * Mathematical Approach (Combinatorics on DAGs):
 * Since data flow is directional (Directed Acyclic Graph - DAG), we cannot have
 * cycles. To visit both 'dac' and 'fft', the path must follow one of two
 * mutually exclusive sequences:
 * 1. Sequence A: svr -> ... -> dac -> ... -> fft -> ... -> out
 * 2. Sequence B: svr -> ... -> fft -> ... -> dac -> ... -> out
 * * We calculate the total paths by segmenting the journey:
 * Total(A) = Paths(svr->dac) * Paths(dac->fft) * Paths(fft->out)
 * Total(B) = Paths(svr->fft) * Paths(fft->dac) * Paths(dac->out)
 * Grand Total = Total(A) + Total(B)
 * * Algorithm:
 * Depth First Search (DFS) with Memoization is used to efficiently count paths
 * between any two given nodes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Configuration Constants
// Increased limits to handle dense inputs often found in Advent of Code
#define MAX_DEVICES 5000  // Maximum number of unique devices (nodes)
#define MAX_NAME_LEN 16   // Maximum length of a device name string
#define MAX_NEIGHBORS 100 // Maximum outgoing connections per device

// Structure to map string names to integer IDs
typedef struct {
  char name[MAX_NAME_LEN];
  int id;
} DeviceMap;

// Global Graph Storage
// Using Adjacency List representation for memory efficiency on sparse graphs
int adj[MAX_DEVICES][MAX_NEIGHBORS]; // Lists of neighbors for each node
int adj_count[MAX_DEVICES];          // Number of neighbors for each node
int num_devices = 0;                 // Current count of discovered devices
DeviceMap map[MAX_DEVICES];          // Lookup table for name-to-ID mapping

// Memoization Table
// Stores the result of count_paths(u, target) to avoid re-calculating
// subproblems. Using 'unsigned long long' to prevent overflow as path counts
// can be massive.
unsigned long long memo[MAX_DEVICES];

/**
 * Gets the integer ID for a device name.
 * If the device is new, assigns a new ID and initializes it.
 * * @param name The string name of the device (e.g., "aaa").
 * @return The integer ID associated with the name.
 */
int get_id(char *name) {
  // Linear search for existing ID (sufficient for N=5000)
  for (int i = 0; i < num_devices; i++) {
    if (strcmp(map[i].name, name) == 0) {
      return i;
    }
  }

  // Safety check for array bounds
  if (num_devices >= MAX_DEVICES) {
    fprintf(stderr,
            "Error: MAX_DEVICES limit reached! Increase the constant.\n");
    exit(1);
  }

  // Register new device
  strcpy(map[num_devices].name, name);
  map[num_devices].id = num_devices;
  adj_count[num_devices] = 0;
  return num_devices++;
}

/**
 * Resets the memoization table.
 * Must be called before starting a new path counting segment (e.g., changing
 * the target). Sets all entries to -1 (represented as max unsigned value).
 */
void reset_memo() {
  // 0xFF represents all bits set to 1. In two's complement, this is -1.
  // For unsigned types, this is the maximum possible value, acting as a
  // sentinel.
  memset(memo, 0xFF, sizeof(memo));
}

/**
 * Recursive Depth First Search (DFS) with Memoization.
 * Counts all distinct paths from node u to target_id.
 * * @param u The current node ID.
 * @param target_id The destination node ID.
 * @return The number of valid paths.
 */
unsigned long long count_paths(int u, int target_id) {
  // 1. Check Memoization Table
  // If value is not the sentinel (max ULL), return the cached result.
  if (memo[u] != 0xFFFFFFFFFFFFFFFFULL) {
    return memo[u];
  }

  // 2. Base Case
  // We reached the target node. This counts as 1 valid path.
  if (u == target_id) {
    return 1;
  }

  // 3. Recursive Step
  // Sum the paths from all neighbors to the target.
  unsigned long long total = 0;
  for (int i = 0; i < adj_count[u]; i++) {
    int v = adj[u][i];
    total += count_paths(v, target_id);
  }

  // 4. Store Result
  memo[u] = total;
  return total;
}

int main() {
  char line[4096];
  num_devices = 0;

  // --- 1. Parse Input Graph ---
  // Reads line by line, format: "source: dest1 dest2 ..."
  while (fgets(line, sizeof(line), stdin)) {
    // Pre-cleaning: replace ':' with space for easier tokenization
    char *colon = strchr(line, ':');
    if (!colon)
      continue;
    *colon = ' ';

    char *token = strtok(line, " \t\n");
    if (!token)
      continue;

    int u = get_id(token); // Parse Source Node

    // Parse Destination Nodes
    while ((token = strtok(NULL, " \t\n")) != NULL) {
      int v = get_id(token);

      if (adj_count[u] >= MAX_NEIGHBORS) {
        fprintf(stderr,
                "Error: MAX_NEIGHBORS limit reached for device '%s'! Increase "
                "the constant.\n",
                map[u].name);
        exit(1);
      }
      adj[u][adj_count[u]++] = v; // Add Directed Edge u -> v
    }
  }

  // --- 2. Identify Critical Nodes ---
  // We need the IDs for the start, end, and the two required intermediate
  // nodes.
  int id_svr = -1, id_out = -1, id_dac = -1, id_fft = -1;

  for (int i = 0; i < num_devices; i++) {
    if (strcmp(map[i].name, "svr") == 0)
      id_svr = i;
    if (strcmp(map[i].name, "out") == 0)
      id_out = i;
    if (strcmp(map[i].name, "dac") == 0)
      id_dac = i;
    if (strcmp(map[i].name, "fft") == 0)
      id_fft = i;
  }

  if (id_svr == -1 || id_out == -1 || id_dac == -1 || id_fft == -1) {
    printf(
        "Error: Missing one of the required devices (svr, out, dac, fft).\n");
    return 1;
  }

  // --- 3. Calculate Sequence A: svr -> dac -> fft -> out ---
  // Since we reuse the DFS function, we must reset memoization between segments
  // because the 'target_id' changes.

  reset_memo();
  unsigned long long p1 = count_paths(id_svr, id_dac); // Segment 1

  reset_memo();
  unsigned long long p2 = count_paths(id_dac, id_fft); // Segment 2

  reset_memo();
  unsigned long long p3 = count_paths(id_fft, id_out); // Segment 3

  // Paths combine multiplicatively in series
  unsigned long long count_sequence_A = p1 * p2 * p3;

  // --- 4. Calculate Sequence B: svr -> fft -> dac -> out ---

  reset_memo();
  unsigned long long q1 = count_paths(id_svr, id_fft); // Segment 1

  reset_memo();
  unsigned long long q2 = count_paths(id_fft, id_dac); // Segment 2

  reset_memo();
  unsigned long long q3 = count_paths(id_dac, id_out); // Segment 3

  unsigned long long count_sequence_B = q1 * q2 * q3;

  // --- 5. Total ---
  // Sequences A and B represent disjoint sets of paths in a DAG
  unsigned long long total = count_sequence_A + count_sequence_B;

  printf("Paths (svr->dac->fft->out): %llu\n", count_sequence_A);
  printf("Paths (svr->fft->dac->out): %llu\n", count_sequence_B);
  printf("Total valid paths: %llu\n", total);

  return 0;
}