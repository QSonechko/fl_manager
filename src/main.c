#include "fl_manager.h"

int main(void)
{
	struct file_manager *mngr = init_manager("/home/pasha/", FALSE);
	int res;

	initscr();
	noecho();
	raw();

	res = mngr_loop(mngr);
	
	endwin();
	free_manager(mngr);

	return res;
}
