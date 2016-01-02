#include "fl_manager.h"

struct file_manager *init_manager(const char *path, int hidden)
{
	struct file_manager *mngr = malloc(sizeof(struct file_manager));
	mngr->path = malloc(sizeof(path) + 1);
	strcpy(mngr->path, path);
	mngr->with_hidden = hidden;

	return mngr;
}

int o_dir(struct file_manager *mngr)
{
	if (mngr->dir_stream != NULL)
		closedir(mngr->dir_stream);

	mngr->dir_stream = opendir(mngr->path);
	if (mngr->dir_stream == NULL) {
		perror("open");
		return -1;
	}

	return 0;
}

int read_dir(struct file_manager *mngr)
{
	int err = errno;
	int ec = 1;
	struct dirent *readres;

	while ((readres = readdir(mngr->dir_stream)) != NULL ) {
		if (!mngr->with_hidden && readres->d_name[0] == '.')
			continue;
		if (mngr->dir_entries[ec] == NULL)
			mngr->dir_entries[ec] = malloc(sizeof(readres->d_name) + 1);
		else
			mngr->dir_entries[ec] = (char*)realloc(mngr->dir_entries[ec], 
												   sizeof(readres->d_name) + 1);
		strcpy(mngr->dir_entries[ec++], readres->d_name);
	}

	if (err != errno) {
		perror("readdir");
		return -1;
	}
	mngr->ent_count = ec;
	mngr->selected_ent = 0;

	return 0;
}

void sort(struct file_manager *mngr)
{
	int i, j;
	char tmp[100];
	for (i = 1; i < mngr->ent_count - 1; i++) {
		for (j = 1; j < mngr->ent_count - 1; j++) {
			if (mngr->dir_entries[j][0] > mngr->dir_entries[j + 1][0]) {
				strcpy(tmp, mngr->dir_entries[j]);
				strcpy(mngr->dir_entries[j], mngr->dir_entries[j + 1]);
				strcpy(mngr->dir_entries[j + 1], tmp);
			}
		}
	}
}

void print_dir(struct file_manager *mngr)
{
	int i;
	clear();
	for (i = 0; i < mngr->ent_count; i++) {
		if (i == mngr->selected_ent)
			printw("*");
		printw("\t%s\n", mngr->dir_entries[i]);
	}
	printw("-------------------------\n");
	printw("Currently in %s", mngr->path);
	printw("Entries in directory: %d\n", mngr->ent_count);
	refresh();
}

int mngr_loop(struct file_manager *mngr)
{
	int c;
	int se;

	initscr();
	noecho();
	raw();

	if (o_dir(mngr) == -1)
		return -1;
	if (read_dir(mngr) == -1)
		return -2;
	print_dir(mngr);

	while ((c = getch()) != (int)'q') {
		switch(c) {
		case K_UP:
			if (mngr->selected_ent > 0)
				mngr->selected_ent--;
			break;
		case K_DOWN:
			if (mngr->selected_ent < mngr->ent_count)
				mngr->selected_ent++;
			break;
		case K_RETURN:
			se = mngr->selected_ent;
			mngr->dir_entries[0] = (char*)realloc(mngr->dir_entries[0], 
												  sizeof(mngr->path + 1));
			strcpy(mngr->dir_entries[0], mngr->path);
			mngr->path = (char*)realloc(mngr->path, 
								 sizeof(mngr->dir_entries[se] + 1));
			strcpy(mngr->path, mngr->dir_entries[se]);
			if (o_dir(mngr) == -1)
				return -1;

			if (read_dir(mngr) == -1)
				return -2;
			break;
		case K_BACK:
	
			break;
		}
		print_dir(mngr);
	}

	endwin();

	return 0;
}

void free_manager(struct file_manager *mngr)
{
	if (mngr != NULL) {
		int i = 0;
		while (mngr->dir_entries[i] != NULL)
			free(mngr->dir_entries[i++]);
		free(mngr->path);
		closedir(mngr->dir_stream);
		free(mngr);
	}
}
