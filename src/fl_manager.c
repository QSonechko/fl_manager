#include "fl_manager.h"
// TODO: write loop function and try compiling this shit already ffs

struct file_manager *init_manager(const char *path, int hidden)
{
	struct file_manager *mngr = malloc(sizeof(struct file_manager));
	mngr->path = malloc(sizeof(path) + 1);
	strcpy(mngr->path, path);
	mngr->with_hidden = hidden;
}

int o_dir(struct file_manager *mngr)
{
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
			realloc(mngr->dir_entries[ec], sizeof(readres->d_name) + 1);
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
	char tmp[];
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

void free_manager(struct file_manager *mngr)
{
	if (mngr != NULL) {
		int i = 0;
		while (mngr->dir_entries[i] != NULL)
			free(mngr->dir_entries[i++]);
		free(mngr->path);
		dirclose(mngr->dir_stream);
		free(mngr);
	}
}
