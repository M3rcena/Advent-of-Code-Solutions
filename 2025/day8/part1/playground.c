#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Δομή για την αναπαράσταση ενός Junction Box (σημείο στο χώρο)
typedef struct {
    int id;
    long long x, y, z;
} Point;

// Δομή για την αναπαράσταση μιας σύνδεσης (ακμή) μεταξύ δύο κουτιών
typedef struct {
    int u; // Δείκτης του πρώτου κουτιού
    int v; // Δείκτης του δεύτερου κουτιού
    long long dist_sq; // Τετράγωνο της Ευκλείδειας απόστασης (για αποφυγή sqrt και float)
} Edge;

// Παγκόσμιες μεταβλητές για το DSU (Disjoint Set Union)
int *parent;
int *sz; // Μέγεθος του κάθε συνόλου (circuit)

// Συνάρτηση εύρεσης του αντιπροσώπου του συνόλου (με path compression)
int find_set(int i) {
    if (parent[i] == i)
        return i;
    return parent[i] = find_set(parent[i]);
}

// Συνάρτηση ένωσης δύο συνόλων
void union_sets(int i, int j) {
    int root_i = find_set(i);
    int root_j = find_set(j);
    
    if (root_i != root_j) {
        // Ένωση βάσει μεγέθους για βελτιστοποίηση
        if (sz[root_i] < sz[root_j]) {
            int temp = root_i;
            root_i = root_j;
            root_j = temp;
        }
        parent[root_j] = root_i;
        sz[root_i] += sz[root_j];
    }
}

// Συνάρτηση σύγκρισης για την qsort (ταξινόμηση ακμών αύξουσα)
int compare_edges(const void *a, const void *b) {
    Edge *e1 = (Edge *)a;
    Edge *e2 = (Edge *)b;
    if (e1->dist_sq < e2->dist_sq) return -1;
    if (e1->dist_sq > e2->dist_sq) return 1;
    return 0;
}

// Συνάρτηση σύγκρισης για την qsort (ταξινόμηση μεγεθών φθίνουσα)
int compare_sizes(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
}

int main() {
    // 1. Αρχικοποίηση και ανάγνωση δεδομένων
    int capacity = 1000;
    int n = 0;
    Point *points = malloc(capacity * sizeof(Point));
    
    long long x, y, z;
    // Η μορφή εισόδου είναι X,Y,Z
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

    // 2. Δημιουργία όλων των πιθανών ζευγών (ακμών)
    // Προσοχή: Για μεγάλο N, αυτό απαιτεί μνήμη. Για το puzzle αυτό είναι συνήθως ασφαλές.
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
            edges[edge_count].dist_sq = dx*dx + dy*dy + dz*dz;
            edge_count++;
        }
    }

    // 3. Ταξινόμηση των ακμών βάσει απόστασης
    qsort(edges, edge_count, sizeof(Edge), compare_edges);

    // 4. Αρχικοποίηση δομής DSU
    parent = malloc(n * sizeof(int));
    sz = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        parent[i] = i;
        sz[i] = 1; // Κάθε κόμβος ξεκινάει ως κύκλωμα μεγέθους 1
    }

    // 5. Επεξεργασία των 1000 κοντινότερων συνδέσεων
    int limit = 1000;
    if (edge_count < 1000) limit = edge_count;
    
    for (int i = 0; i < limit; i++) {
        union_sets(edges[i].u, edges[i].v);
    }

    // 6. Συλλογή των μεγεθών των κυκλωμάτων
    int *circuit_sizes = malloc(n * sizeof(int));
    int circuit_count = 0;
    for (int i = 0; i < n; i++) {
        // Αν ο κόμβος είναι "ρίζα", τότε αντιπροσωπεύει ένα κύκλωμα
        if (parent[i] == i) {
            circuit_sizes[circuit_count++] = sz[i];
        }
    }

    // 7. Ταξινόμηση μεγεθών και υπολογισμός αποτελέσματος
    qsort(circuit_sizes, circuit_count, sizeof(int), compare_sizes);

    long long result = 1;
    int count_to_mult = 3;
    if (circuit_count < 3) count_to_mult = circuit_count;
    
    // Πολλαπλασιασμός των 3 μεγαλύτερων κυκλωμάτων
    for (int i = 0; i < count_to_mult; i++) {
        result *= circuit_sizes[i];
    }

    printf("Result (Part 1): %lld\n", result);

    // Απελευθέρωση μνήμης
    free(points);
    free(edges);
    free(parent);
    free(sz);
    free(circuit_sizes);

    return 0;
}