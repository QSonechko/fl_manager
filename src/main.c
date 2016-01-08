#include "fl_manager.h"

int main(void)
{
	struct file_manager *mngr = init_manager("/", TRUE);
	int res;

	initscr();
	noecho();
	raw();

	start_color();

	res = mngr_loop(mngr);
	
	endwin();
	free_manager(mngr);

	return res;
}
