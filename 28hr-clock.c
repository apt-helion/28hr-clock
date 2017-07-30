#include <time.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>

typedef struct {
	char* day;
	int hour;
	int min;
	int sec;
} ti;

// seven-segment display
//   |a|b|c|d|e|f|g|
// 0 |1|1|1|1|1|1|0|
// 1 |0|1|1|0|0|0|0|
// 2 |1|1|0|1|1|0|1|
// 3 |1|1|1|1|0|0|1|
// 4 |0|1|1|0|0|1|1|
// 5 |1|0|1|1|0|1|1|
// 6 |1|0|1|1|1|1|1|
// 7 |1|1|1|0|0|0|0|
// 8 |1|1|1|1|1|1|1|
// 9 |1|1|1|1|0|1|1|

int LED_DIGITS[10] = {
	0b1111110,
	0b0110000,
	0b1101101,
	0b1111001,
	0b0110011,
	0b1011011,
	0b1011111,
	0b1110000,
	0b1111111,
	0b1111011
};

void print_led_digit(int dig, int y, int x)
{
	if ((dig >> 6) & 1) // a
	    mvprintw(y, x + 1, "__");
	if ((dig >> 5) & 1) // b
	    mvprintw(y + 1, x + 3, "|");
	if ((dig >> 4) & 1) // c
	    mvprintw(y + 2, x + 3, "|");
	if ((dig >> 3) & 1) // d
	    mvprintw(y + 2, x + 1, "__");
	if ((dig >> 2) & 1) // e
	    mvprintw(y + 2, x, "|");
	if ((dig >> 1) & 1) // f
	    mvprintw(y + 1, x, "|");
	if ((dig >> 0) & 1) // g
	    mvprintw(y + 1, x + 1, "__");
}


void convert_time(int time_day, int time_hour, ti *new_time)
{
	// finds current hour of the week
	int current_hour = (time_day * 24) + time_hour;

	int day = current_hour / 28;
	switch (day) {
		case (0) :
			new_time->day = "Alphaday";
			new_time->hour = current_hour;
		break;
		case (1) :
			new_time->day = "Betaday";
			new_time->hour = current_hour-28;
		break;
		case (2) :
			new_time->day = "Gammaday";
			new_time->hour = current_hour-56;
		break;
		case (3) :
			new_time->day = "Deltaday";
			new_time->hour = current_hour-86;
		break;
		case (4) :
			new_time->day = "Epsilonday";
			new_time->hour = current_hour-112;
		break;
		case (5) :
			new_time->day = "Zetaday";
			new_time->hour = current_hour-140;
		break;
		default : break;
	}
}

void display_clock()
{
	int row, col;
	int dig1, dig2;

	while (1) {
		clear();
		getmaxyx(stdscr, row, col);

		// get time
		time_t rawtime;
		struct tm * timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);

		ti time; // new time structure in 28 hr format

		int row_off = (row / 2) - 1;
		int col_off = (col / 2) - (33 / 2);

		// get min and secs
		time.min = timeinfo->tm_min;
		time.sec = timeinfo->tm_sec;

		// number of days since Sunday, range 0 to 6
		int time_day = timeinfo->tm_wday;
		// number of hours past midnight, range 0 to 23
		int time_hour = timeinfo->tm_hour;

		convert_time(time_day, time_hour, &time);

		int hr = time.hour;
		dig1 = (hr / 10) % 10;
		dig2 = hr % 10;
		print_led_digit(LED_DIGITS[dig1], row_off, col_off);
		print_led_digit(LED_DIGITS[dig2], row_off, col_off + 4);
		mvaddch(row_off + 2, col_off + 8, ':');

		int min = time.min;
		dig1 = (min / 10) % 10;
		dig2 = min % 10;
		print_led_digit(LED_DIGITS[dig1], row_off, col_off + 9);
		print_led_digit(LED_DIGITS[dig2], row_off, col_off + 13);
		mvaddch(row_off + 2, col_off + 17, ':');

		int sec = time.sec;
		dig1 = (sec / 10) % 10;
		dig2 = sec % 10;
		print_led_digit(LED_DIGITS[dig1], row_off, col_off + 18);
		print_led_digit(LED_DIGITS[dig2], row_off, col_off + 22);

		mvprintw(row_off-1, (col-strlen(time.day))/2-2, time.day);

		refresh();
		sleep(1);
	}

}

void main()
{
	initscr(); // initialize ncurses
	noecho();
	cbreak();
	curs_set(0);
	keypad(stdscr, TRUE);

	display_clock();

	endwin(); // restore terminal settings
}
