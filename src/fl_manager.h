#ifndef FL_MANAGER_H
#define FL_MANAGER_H

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <ncurses.h>
#include <curses.h>

#define TRUE 1
#define FALSE 0
#define K_UP 65
#define K_DOWN 66
#define K_RETURN 10
#define K_BACK 127

struct file_manager {
	char *path;
	DIR *dir_stream;
	int ent_count;
	int selected_ent;
	int with_hidden;
	char *dir_entries[1024];
};

struct file_manager *init_manger(const char *path, int hidden);
inline void free_manager(struct file_manager *mngr);
int o_dir(struct file_manager *mngr);
int read_dir(struct file_manager *mngr);
void sort(struct file_manager mngr*);
void print_dir(struct file_manager *mngr);
int mngr_loop(struct file_manager *mngr);

#endif