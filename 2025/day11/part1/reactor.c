/**
 * Advent of Code 2025 - Day 11: Reactor
 * Problem: Count all paths from node "you" to node "out".
 * Method: Depth First Search (DFS) with Memoization.
 */

#include <stdio.h>
#include <string.h>

#define MAX_DEVICES 2000
#define MAX_NAME_LEN 16
#define MAX_NEIGHBORS 20

typedef struct {
  char name[MAX_NAME_LEN];
  int id;
} DeviceMap;

// Graph structure
int adj[MAX_DEVICES][MAX_NEIGHBORS]; // Adjacency list
int adj_count[MAX_DEVICES];          // Number of neighbors for each node
int num_devices = 0;

DeviceMap map[MAX_DEVICES];

// Memoization array: stores path counts for each node. -1 means not calculated
// yet.
long long memo[MAX_DEVICES];

// --- Helper: Get or Create ID for a name ---
int get_id(char *name) {
  for (int i = 0; i < num_devices; i++) {
    if (strcmp(map[i].name, name) == 0) {
      return i;
    }
  }
  // New device found
  strcpy(map[num_devices].name, name);
  map[num_devices].id = num_devices;
  adj_count[num_devices] = 0;
  memo[num_devices] = -1; // Initialize memo
  return num_devices++;
}

// --- Recursive DFS with Memoization ---
long long count_paths(int u, int target_id) {
  // 1. Check Memoization table
  if (memo[u] != -1) {
    return memo[u];
  }

  // 2. Base Case: Reached the target
  if (u == target_id) {
    return 1;
  }

  // 3. Recursive Step
  long long total = 0;
  for (int i = 0; i < adj_count[u]; i++) {
    int v = adj[u][i];
    total += count_paths(v, target_id);
  }

  // 4. Store and return
  memo[u] = total;
  return total;
}

int main() {
  char line[1024];

  // Reset graph
  num_devices = 0;

  // 1. Parse Input
  while (fgets(line, sizeof(line), stdin)) {
    // Line format: "name: dest1 dest2 ..."
    // Replace ':' with space to simplify tokenization
    char *colon = strchr(line, ':');
    if (!colon)
      continue;
    *colon = ' ';

    // Tokenize
    char *token = strtok(line, " \t\n");
    if (!token)
      continue;

    // First token is the Source Node
    int u = get_id(token);

    // Subsequent tokens are Destination Nodes
    while ((token = strtok(NULL, " \t\n")) != NULL) {
      int v = get_id(token);

      // Add edge u -> v
      if (adj_count[u] < MAX_NEIGHBORS) {
        adj[u][adj_count[u]++] = v;
      }
    }
  }

  // 2. Identify Start and End IDs
  int start_id = -1, end_id = -1;
  for (int i = 0; i < num_devices; i++) {
    if (strcmp(map[i].name, "you") == 0)
      start_id = i;
    if (strcmp(map[i].name, "out") == 0)
      end_id = i;
  }

  if (start_id == -1 || end_id == -1) {
    printf("Error: Could not find 'you' or 'out' in the input.\n");
    return 1;
  }

  // 3. Calculate Paths
  long long total_paths = count_paths(start_id, end_id);

  printf("Total paths from 'you' to 'out': %lld\n", total_paths);

  return 0;
}