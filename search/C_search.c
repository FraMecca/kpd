#define _GNU_SOURCE
#include "list_kpd.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <zlib.h>
#include <string.h>

list_t* insert_line_in_memory (char *line, list_t *listDB, directory_list_t **dirsRef);

int is_contained (char *st, list_t *ptr)
{
	if (ptr->fsName != NULL && strcasestr (ptr->fsName, st) != NULL) {
		return 1;
	} else {
		if (ptr->title != NULL && strcasestr (ptr->title , st) != NULL) {
			return 1;
		} else {
			if (ptr->album != NULL && strcasestr (ptr->album, st) != NULL) {
				return 1;
			} else {
				if (ptr->artist != NULL && strcasestr (ptr->artist, st) != NULL) {
					return 1;
				} else {
					return 0;
				}
			}
		}
	}
}

char *get_complete_name (list_t *ptr)
{
	char *st = malloc ( (1 + strlen (ptr->directory) + strlen (ptr->fsName)) * sizeof (char));

	strcpy (st, ptr->directory);
	st[strlen (st) - 1] = '/';
	strcat (st, ptr->fsName);
	st[strlen (st) - 1] = '\0';
	return (st);
}

void search (char *st, list_t *listDB)
{
	char *res;

	while (listDB != NULL) {
		if (is_contained (st, listDB)) {
			res = (get_complete_name (listDB));
			printf ("%s\n", res);
			free (res);
		}
		listDB = listDB->next;
	}
}

list_t* insert_line_in_memory (char *line, list_t *listDB, directory_list_t **dirsRef)
{
	char *partOneSt, *partTwoSt;
	
	directory_list_t *dirs = dirsRef[0];

	partOneSt = strtok (line, ": ");
	partTwoSt = strtok (NULL, ":");
	if (partTwoSt != NULL) {
		partTwoSt = partTwoSt + 1;
	}

	/*printf ("partone: %s-\npartTwo: %s\nstring == %s\n", partOneSt, partTwoSt, stringa);*/

	if (strcmp (partOneSt, "begin") == 0) {
		dirs = add_directory (partTwoSt, dirs); //directory are added to a list that are popped when end tag, added when begin tag;
		dirsRef[0] = dirs;
		return listDB;
	}
	
	if (strcmp (partOneSt, "song_begin") == 0) {
		listDB->fsName = strdup (partTwoSt);
		if (dirs == NULL) {
			listDB->directory = strdup ("\0");
		}
		else {
			listDB->directory = strdup (dirs->name);
		}
		return listDB;
	}
	if (strcmp (partOneSt, "end") == 0) {
		dirs = pop_directory (dirs);
		dirsRef[0] = dirs;
		return listDB;
	}
	if (strcmp (line, "song_end\n") == 0) {
		listDB = insert_tail (listDB);
		return listDB;
	}
	if (strcmp (partOneSt, "Artist") == 0) {
		listDB->artist = strdup (partTwoSt);
		return listDB;
	}
	if (strcmp (partOneSt, "Album") == 0) {
		listDB->album = strdup (partTwoSt);
		return listDB;
	}
	if (strcmp (partOneSt, "Title") == 0) {
		listDB->title = strdup (partTwoSt);
		return listDB;
	}

	return listDB;
}

int main (int argc, char *argv[])
{
	gzFile fp = gzopen ("/home/user/.mpd/database", "r");
	list_t *listDB = initialize_list ();
	directory_list_t *dirs = NULL;
	char buf[5000];
	
	while (gzgets (fp, buf, 5000) != NULL) {
		listDB = insert_line_in_memory (buf, listDB, &dirs);
	}
	gzclose (fp);

	/*listDB = deserialize ();*/
	listDB = return_to_head (listDB);
	search (argv[1], listDB);

	destroy_list (listDB);
	return 0;
}

void search_c_main (char *key)
{
	gzFile fp = gzopen ("/home/user/.mpd/database", "r");
	list_t *listDB = initialize_list ();
	directory_list_t *dirs = NULL;
	char buf[5000];
	
	while (gzgets (fp, buf, 5000) != NULL) {
		listDB = insert_line_in_memory (buf, listDB, &dirs);
	}
	gzclose (fp);

	listDB = return_to_head (listDB);
	search (key, listDB);

	destroy_list (listDB);
}



