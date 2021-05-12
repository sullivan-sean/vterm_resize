#include <vterm.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  VTerm* vt;
  char **rows;
  int verbose;
} ScreenData;

static int damage_cb(VTermRect rect, void *user) {
  // Practical damage callback that maintains an updated copy of row text
  // as the screen is damaged.
  ScreenData *screen_data = user;
  int nrow, ncol;
  vterm_get_size(screen_data->vt, &nrow, &ncol);

  if (screen_data->verbose)
    printf("Damage rect (%d, %d, %d, %d). Screen size = %dx%d\n", rect.start_row, rect.end_row, rect.start_col, rect.end_col, nrow, ncol);

  VTermScreen *vts = vterm_obtain_screen(screen_data->vt);
  int end_row = rect.end_row;
  for (int i = rect.start_row; i < end_row; i++) {
    rect.start_row = i;
    rect.end_row = i + 1;

    char* row_i = screen_data->rows[i] + rect.start_col;
    vterm_screen_get_text(vts, row_i, rect.end_col - rect.start_col, rect);
  }
  return 0;
}

void test_resize(int start_nrow, int start_ncol, int delta_nrow, int delta_ncol, int verbose) {
  // Allocate enough to hold largest possible screen.
  int end_nrow = start_nrow + delta_nrow;
  int end_ncol = start_ncol + delta_ncol;

  int max_nrow = start_nrow > end_nrow ? start_nrow : end_nrow;
  int max_ncol = start_ncol > end_ncol ? start_ncol : end_ncol;

  char** rows = malloc(sizeof(char *) * max_nrow);
  for (int i = 0; i < max_nrow; i++) {
    rows[i] = malloc(sizeof(char) * max_ncol);;
  }

  // Initialize vterm (start_nrow x start_ncol) and set damage callback.
  VTerm *vt = vterm_new(start_nrow, start_ncol);
  ScreenData screen_data = { .vt = vt, .rows = rows, .verbose = verbose };
  VTermScreen *vts = vterm_obtain_screen(vt);

  VTermScreenCallbacks screen_callbacks = { .damage = damage_cb };
  vterm_screen_set_callbacks(vts, &screen_callbacks, &screen_data);
  vterm_screen_set_damage_merge(vts, VTERM_DAMAGE_ROW);
  vterm_screen_reset(vts, 1);

  // Resize vterm to (end_nrow x end_ncol).
  vterm_set_size(vt, end_nrow, end_ncol);

  // Clean up.
  for (int i = 0; i < max_nrow; i++) free(rows[i]);
  free(rows);
  vterm_free(vt);
}

int main(int argc, char *argv[]) {
  for (int i = -10; i <= 10; i += 10) {
    for (int j = -10; j <= 10; j += 10) {
      // Skip failing examples, and avoid being verbose when success is
      // known.
      if (i < 0 && j > 0) continue;
      int verbose = 0;

      printf("Resizing from 20x20 with delta (%d, %d)\n", i, j);
      test_resize(20, 20, i, j, verbose);
    }
  }

  // Failing example, decrease rows and increase cols.
  int verbose = 1;
  test_resize(20, 20, -10, 10, verbose);
}
