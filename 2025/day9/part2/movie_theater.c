#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

// Δομή για την αποθήκευση σημείων (X, Y)
typedef struct {
    long long x;
    long long y;
} Point;

// Δομή για την αναπαράσταση ορθογωνίου
typedef struct {
    long long min_x, max_x;
    long long min_y, max_y;
} Rect;

// Βοηθητική συνάρτηση: min/max
long long min_val(long long a, long long b) { return (a < b) ? a : b; }
long long max_val(long long a, long long b) { return (a > b) ? a : b; }

// Έλεγχος αν το κέντρο του ορθογωνίου είναι μέσα στο πολύγωνο (Ray Casting)
// Το πολύγωνο ορίζεται από τον πίνακα tiles (μέγεθος n)
bool is_center_inside(Rect r, Point *tiles, int n) {
    // Το κέντρο του ορθογωνίου (σε double για ακρίβεια)
    double cx = (r.min_x + r.max_x) / 2.0;
    double cy = (r.min_y + r.max_y) / 2.0;
    
    int intersections = 0;

    for (int i = 0; i < n; i++) {
        Point p1 = tiles[i];
        Point p2 = tiles[(i + 1) % n]; // Κυκλική σύνδεση (το τελευταίο με το πρώτο)

        // Έλεγχος αν η ακμή τέμνει την οριζόντια ακτίνα που ξεκινάει από το (cx, cy)
        // Ray Casting: Κοιτάμε αν το cy είναι ανάμεσα στα Y των άκρων της ακμής
        if ((p1.y > cy) != (p2.y > cy)) {
            // Υπολογισμός του X στο σημείο τομής
            double intersectX = (p2.x - p1.x) * (cy - p1.y) / (double)(p2.y - p1.y) + p1.x;
            
            // Μετράμε μόνο αν η τομή είναι δεξιά από το σημείο μας
            if (cx < intersectX) {
                intersections++;
            }
        }
    }

    // Αν οι τομές είναι περιττός αριθμός, είμαστε μέσα.
    return (intersections % 2 != 0);
}

// Έλεγχος αν κάποια πλευρά του πολυγώνου "κόβει" το ορθογώνιο στη μέση
bool do_edges_intersect(Rect r, Point *tiles, int n) {
    for (int i = 0; i < n; i++) {
        Point p1 = tiles[i];
        Point p2 = tiles[(i + 1) % n];

        // Έλεγχος αν η ακμή είναι κάθετη
        if (p1.x == p2.x) {
            long long edge_x = p1.x;
            long long y_start = min_val(p1.y, p2.y);
            long long y_end = max_val(p1.y, p2.y);

            // Για να "κόψει" το ορθογώνιο, το X της ακμής πρέπει να είναι ΑΥΣΤΗΡΑ ανάμεσα στα X του ορθογωνίου
            // Και τα Y να αλληλεπικαλύπτονται
            if (edge_x > r.min_x && edge_x < r.max_x) {
                // Έλεγχος επικάλυψης στον άξονα Y
                // Αν το διάστημα [y_start, y_end] έχει κοινά σημεία με το [r.min_y, r.max_y]
                if (max_val(y_start, r.min_y) < min_val(y_end, r.max_y)) {
                    return true; // Υπάρχει τομή
                }
            }
        }
        // Έλεγχος αν η ακμή είναι οριζόντια
        else if (p1.y == p2.y) {
            long long edge_y = p1.y;
            long long x_start = min_val(p1.x, p2.x);
            long long x_end = max_val(p1.x, p2.x);

            // Αντίστοιχος έλεγχος για οριζόντια τομή
            if (edge_y > r.min_y && edge_y < r.max_y) {
                if (max_val(x_start, r.min_x) < min_val(x_end, r.max_x)) {
                    return true; // Υπάρχει τομή
                }
            }
        }
    }
    return false;
}

int main() {
    int capacity = 1000;
    int n = 0;
    Point *tiles = malloc(capacity * sizeof(Point));
    
    // 1. Ανάγνωση εισόδου
    while (scanf("%lld,%lld", &tiles[n].x, &tiles[n].y) == 2) {
        n++;
        if (n >= capacity) {
            capacity *= 2;
            tiles = realloc(tiles, capacity * sizeof(Point));
        }
    }

    long long max_area = 0;

    // 2. Εξέταση κάθε ζεύγους σημείων
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            
            // Δημιουργία του υποψήφιου ορθογωνίου
            Rect r;
            r.min_x = min_val(tiles[i].x, tiles[j].x);
            r.max_x = max_val(tiles[i].x, tiles[j].x);
            r.min_y = min_val(tiles[i].y, tiles[j].y);
            r.max_y = max_val(tiles[i].y, tiles[j].y);

            // 3. Υπολογισμός Εμβαδού (Πρώτα, για να αποφύγουμε βαρύ έλεγχο αν είναι μικρότερο)
            long long area = (r.max_x - r.min_x + 1) * (r.max_y - r.min_y + 1);

            if (area <= max_area) continue;

            // 4. Έλεγχοι Εγκυρότητας
            // Α. Είναι το κέντρο μέσα στο πολύγωνο;
            if (!is_center_inside(r, tiles, n)) continue;

            // Β. Μήπως κάποιος τοίχος κόβει το ορθογώνιο;
            if (do_edges_intersect(r, tiles, n)) continue;

            // Αν περάσει τους ελέγχους, είναι έγκυρο
            max_area = area;
        }
    }

    printf("Largest valid area (Part 2): %lld\n", max_area);

    free(tiles);
    return 0;
}