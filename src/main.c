#include "fl_manager.h"

int main(void)
{
	struct fl_manager *mngr = init_manager("/home/pasha/", FALSE);
	int res = mngr_loop(mngr);

	return res;
}
