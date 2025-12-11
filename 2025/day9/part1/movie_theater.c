#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Δομή για την αποθήκευση συντεταγμένων
typedef struct {
  int x;
  int y;
} Point;

int main() {
  int capacity = 1000;
  int count = 0;
  Point *tiles = malloc(capacity * sizeof(Point));

  // 1. Ανάγνωση εισόδου (μορφή X,Y)
  // Διαβάζουμε μέχρι να τελειώσει το αρχείο
  while (scanf("%d,%d", &tiles[count].x, &tiles[count].y) == 2) {
    count++;
    // Αν γεμίσει ο πίνακας, διπλασιασμός χωρητικότητας
    if (count >= capacity) {
      capacity *= 2;
      tiles = realloc(tiles, capacity * sizeof(Point));
    }
  }

  long long max_area = 0;

  // 2. Έλεγχος όλων των πιθανών ζευγών (Brute Force)
  for (int i = 0; i < count; i++) {
    for (int j = i + 1; j < count; j++) {

      // Υπολογισμός διαφοράς συντεταγμένων
      long long dx = abs(tiles[i].x - tiles[j].x);
      long long dy = abs(tiles[i].y - tiles[j].y);

      // Υπολογισμός εμβαδού (Inclusive: +1 σε κάθε διάσταση)
      // Στο παράδειγμα: |9-2|=7 -> πλάτος 8, |7-5|=2 -> ύψος 3. 8*3=24.
      long long area = (dx + 1) * (dy + 1);

      if (area > max_area) {
        max_area = area;
      }
    }
  }

  printf("Largest area: %lld\n", max_area);

  free(tiles);
  return 0;
}