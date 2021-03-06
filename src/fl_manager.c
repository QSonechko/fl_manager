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
	int ec = 0;
	char *tmp;
	struct dirent *readres;

	while ((readres = readdir(mngr->dir_stream)) != NULL ) {
		if (!mngr->with_hidden && readres->d_name[0] == '.')
			continue;
		if (mngr->dir_entries[ec] == NULL) {
			mngr->dir_entries[ec] = malloc(sizeof(readres->d_name) + 1);
		} else {
			tmp = (char *)realloc(mngr->dir_entries[ec],
								sizeof(readres->d_name) + 1);
			if (tmp == NULL) {
				perror("realloc");
				return -1;
			} else {
				mngr->dir_entries[ec] = tmp;
			}
		}
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
	char *tmp;
	for (i = 0; i < mngr->ent_count - 1; i++) {
		for (j = 0; j < mngr->ent_count - 1; j++) {
			if (mngr->dir_entries[j][0] > mngr->dir_entries[j + 1][0])
				SWAP(tmp, mngr->dir_entries[j], mngr->dir_entries[j + 1]);

			if (mngr->dir_entries[j][0] == '.' &&
					mngr->dir_entries[j][1] == '\0' && j != 0)
				SWAP(tmp, mngr->dir_entries[j], mngr->dir_entries[0]);

			if (mngr->dir_entries[j][0] == '.' &&
					mngr->dir_entries[j][1] == '.' && j != 1)
				SWAP(tmp, mngr->dir_entries[j], mngr->dir_entries[1]);
		}
	}
}

void print_dir(struct file_manager *mngr)
{
	int i, j;
	int cols = cols_to_print(mngr->ent_count);
	clear();

	init_pair(1, COLOR_BLACK, COLOR_BLUE);

	for (i = 0; i < mngr->ent_count; i += cols) {
		for (j = 0; j < cols; j++) {
			if (i + j == mngr->selected_ent)
				attron(COLOR_PAIR(1));
			printw("%s\t", mngr->dir_entries[i + j]);
			attroff(COLOR_PAIR(1));
		}
		printw("\n");
	}
	printw("-------------------------\n");
	printw("Currently in %s\n", mngr->path);
	printw("Entries in directory: %d\n", mngr->ent_count);
	refresh();
}

int mngr_loop(struct file_manager *mngr)
{
	int c;
	int se;
	char *tmp;
	int len;

	if (o_dir(mngr) == -1)
		return -1;
	if (read_dir(mngr) == -1)
		return -2;
	sort(mngr);
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
		case K_BACK:
			mngr->selected_ent = 1;
		case K_RETURN:
			se = mngr->selected_ent;

			/*mngr->path*/
			len = strlen(mngr->dir_entries[se]);
			len += strlen(mngr->path);
			tmp = (char *)realloc(mngr->path, len * sizeof(char) + 1);
			if (tmp == NULL) {
				perror("realloc");
				return -1;
			} else {
				mngr->path = tmp;
			}

			strcat(mngr->path, mngr->dir_entries[se]);
			strcat(mngr->path, "/");

			if (o_dir(mngr) == -1)
				return -1;

			if (read_dir(mngr) == -1)
				return -2;
			break;
		case (int)'-':
			c = getch();
			if (!ISDIGIT(c))
				break;
			mngr->selected_ent -= 2 * (c - '0');
		default:
			if (ISDIGIT(c))
				mngr->selected_ent += c - '0';
			if (mngr->selected_ent > mngr->ent_count)
				mngr->selected_ent = mngr->ent_count - 1;
			break;
		}
		sort(mngr);
		print_dir(mngr);
	}

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

int cols_to_print(int ents)
{
	int ret;
	if (ents % 4 == 0) {
		if (ents >= 10)
			ret = 4;
		else
			ret = 2;
	} else if (ents % 3 == 0) {
		ret = 3;
	} else {
		ret = 2;
	}
	//ret = 0;
	return ret;
}

file_type get_type(char *fname)
{
	file_type ret = TXT;
	const char *txts = ".txt.c.cpp.h.php.py";
	const char *vids = ".mp4.webm.avi.mkv";
	const char *muss = ".mp3";

	char *tmp = fname + (strlen(fname) - 3);

	if (strstr(txts, tmp))
		ret = TXT;
	else if (strstr(vids, tmp))
		ret = VID;
	else if (strstr(muss, tmp))
		ret = MUS;
	else
		ret = DIRY;

	return ret;
}

void run(char *name, file_type type)
{
	if (type == TXT)
		execl("/usr/bin/gedit", name, NULL);
	else if (type == VID)
		execl("/usr/bin/totem", name, NULL);
}
