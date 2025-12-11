#include <stdio.h>
#include <stdlib.h>

// Δομή για την αναπαράσταση ενός Junction Box
typedef struct {
  int id;
  long long x, y, z;
} Point;

// Δομή για την αναπαράσταση μιας σύνδεσης (ακμή)
typedef struct {
  int u;             // Δείκτης του πρώτου κουτιού
  int v;             // Δείκτης του δεύτερου κουτιού
  long long dist_sq; // Τετράγωνο απόστασης
} Edge;

// Παγκόσμιες μεταβλητές για το DSU
int *parent;

// Εύρεση του αντιπροσώπου του συνόλου (με path compression)
int find_set(int i) {
  if (parent[i] == i)
    return i;
  return parent[i] = find_set(parent[i]);
}

// Ένωση δύο συνόλων. Επιστρέφει 1 αν έγινε ένωση (ήταν διαφορετικά σύνολα), 0
// αν ήταν ήδη ενωμένα.
int union_sets(int i, int j) {
  int root_i = find_set(i);
  int root_j = find_set(j);

  if (root_i != root_j) {
    parent[root_j] = root_i; // Απλή ένωση χωρίς heuristic για συντομία εδώ
    return 1;
  }
  return 0;
}

// Συνάρτηση σύγκρισης για την qsort (αύξουσα σειρά απόστασης)
int compare_edges(const void *a, const void *b) {
  Edge *e1 = (Edge *)a;
  Edge *e2 = (Edge *)b;
  if (e1->dist_sq < e2->dist_sq)
    return -1;
  if (e1->dist_sq > e2->dist_sq)
    return 1;
  return 0;
}

int main() {
  // 1. Ανάγνωση Δεδομένων
  int capacity = 1000;
  int n = 0;
  Point *points = malloc(capacity * sizeof(Point));

  long long x, y, z;
  while (scanf("%lld,%lld,%lld", &x, &y, &z) == 3) {
    if (n >= capacity) {
      capacity *= 2;
      points = realloc(points, capacity * sizeof(Point));
    }
    points[n].x = x;
    points[n].y = y;
    points[n].z = z;
    points[n].id = n;
    n++;
  }

  // 2. Δημιουργία όλων των ακμών
  long long max_edges = (long long)n * (n - 1) / 2;
  Edge *edges = malloc(max_edges * sizeof(Edge));
  long long edge_count = 0;

  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      long long dx = points[i].x - points[j].x;
      long long dy = points[i].y - points[j].y;
      long long dz = points[i].z - points[j].z;

      edges[edge_count].u = i;
      edges[edge_count].v = j;
      edges[edge_count].dist_sq = dx * dx + dy * dy + dz * dz;
      edge_count++;
    }
  }

  // 3. Ταξινόμηση ακμών (Shortest Path logic)
  qsort(edges, edge_count, sizeof(Edge), compare_edges);

  // 4. Αρχικοποίηση DSU
  parent = malloc(n * sizeof(int));
  for (int i = 0; i < n; i++) {
    parent[i] = i;
  }

  // Το πλήθος των ξεχωριστών κυκλωμάτων ξεκινάει ίσο με το πλήθος των κόμβων
  int components = n;

  // 5. Εκτέλεση αλγορίθμου Kruskal
  for (long long i = 0; i < edge_count; i++) {
    int u = edges[i].u;
    int v = edges[i].v;

    // Προσπαθούμε να ενώσουμε τα δύο κουτιά
    if (union_sets(u, v)) {
      // Αν η ένωση πέτυχε, μειώνουμε τον αριθμό των κυκλωμάτων
      components--;

      // Ελέγχουμε αν φτάσαμε σε 1 ενιαίο κύκλωμα
      if (components == 1) {
        long long result = points[u].x * points[v].x;
        printf("Last connection between boxes at X=%lld and X=%lld\n",
               points[u].x, points[v].x);
        printf("Result (Part 2): %lld\n", result);
        break; // Τελειώσαμε
      }
    }
  }

  // Απελευθέρωση μνήμης
  free(points);
  free(edges);
  free(parent);

  return 0;
}