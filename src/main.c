#include "fl_manager.h"

int main(void)
{
	struct file_manager *mngr = init_manager("/home/pasha/", FALSE);
	int res = mngr_loop(mngr);
	free_manager(mngr);

	return res;
}
