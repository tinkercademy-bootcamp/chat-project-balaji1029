#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#define LEFT_WIDTH 20
#define INPUT_HEIGHT 3
#define MAX_LINES 1000

typedef enum { NAV_MODE, INPUT_MODE } Mode;

int main() {
	initscr();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0);

	int height, width;
	getmaxyx(stdscr, height, width);

	int right_width = width - LEFT_WIDTH;
	int right_height = height - INPUT_HEIGHT;

	WINDOW *left_win  = newwin(height, LEFT_WIDTH, 0, 0);
	WINDOW *right_win = newwin(right_height, right_width, 0, LEFT_WIDTH);
	WINDOW *input_win = newwin(INPUT_HEIGHT, right_width, right_height, LEFT_WIDTH);

	Mode mode = NAV_MODE;
	int focus = 0; // 0 = left, 1 = right
	char input[256] = "";
	int input_pos = 0;

	// For scrolling
	char *lines[MAX_LINES] = {0};
	int num_lines = 0;
	int scroll_offset = 0;

	int ch;
	do {
		// std::cout << "Heyyy";
		getmaxyx(stdscr, height, width);
		right_width = width - LEFT_WIDTH;
		right_height = height - INPUT_HEIGHT;

		wresize(left_win, height, LEFT_WIDTH);
		wresize(right_win, right_height, right_width);
		wresize(input_win, INPUT_HEIGHT, right_width);
		mvwin(input_win, right_height, LEFT_WIDTH);

		// Handle input
		if (mode == NAV_MODE) {
			if (ch == '\t') {
					focus = 1 - focus;
			} else if (ch == 'i' && focus == 1) {
					mode = INPUT_MODE;
					curs_set(1);
			} else if (focus == 1) {
					// Scroll logic
					if ((ch == KEY_UP || ch == 'k') && scroll_offset < num_lines - 1) {
							scroll_offset++;
					} else if ((ch == KEY_DOWN || ch == 'j') && scroll_offset > 0) {
							scroll_offset--;
					}
			}
		} else if (mode == INPUT_MODE) {
				if (ch == 27) { // ESC
						mode = NAV_MODE;
						curs_set(0);
				} else if ((ch == KEY_BACKSPACE || ch == 127 || ch == '\b') && input_pos > 0) {
						input[--input_pos] = '\0';
				} else if (ch == '\n') {
						if (num_lines < MAX_LINES) {
								lines[num_lines] = strdup(input);
								num_lines++;
						}
						input[0] = '\0';
						input_pos = 0;
						scroll_offset = 0; // reset scroll to show latest
				} else if (input_pos < sizeof(input) - 1 && ch >= 32 && ch <= 126) {
						input[input_pos++] = ch;
						input[input_pos] = '\0';
				}
			}

		// Draw left
		werase(left_win);
		box(left_win, 0, 0);
		mvwprintw(left_win, 0, 2, (focus == 0 && mode == NAV_MODE) ? " Left [F] " : " Left ");

		// Draw right
		werase(right_win);
		box(right_win, 0, 0);
		mvwprintw(right_win, 0, 2, (focus == 1 && mode == NAV_MODE) ? " Right [F] " : " Right ");

		int visible_lines = right_height - 2;
		int start = (num_lines > visible_lines + scroll_offset)
									? num_lines - visible_lines - scroll_offset
									: 0;

		for (int i = 0; i < visible_lines; i++) {
			int line_idx = start + i;
			if (line_idx < num_lines) {
					mvwprintw(right_win, i + 1, 1, "%s", lines[line_idx]);
			}
		}

		// Draw input
		werase(input_win);
		box(input_win, 0, 0);
		mvwprintw(input_win, 0, 2, " Input (%s Mode) ", mode == INPUT_MODE ? "Insert" : "Nav");
		mvwprintw(input_win, 1, 1, "%s", input);
		if (mode == INPUT_MODE) {
			wmove(input_win, 1, 1 + input_pos);
		}

		// Refresh windows
		wrefresh(left_win);
		wrefresh(right_win);
		wrefresh(input_win);
	} while ((ch = getch()) != 'q');

	// Cleanup
	for (int i = 0; i < num_lines; i++) free(lines[i]);
	delwin(left_win);
	delwin(right_win);
	delwin(input_win);
	endwin();
	return 0;
}
