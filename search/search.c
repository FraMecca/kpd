#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tpl.h"
#include <zlib.h>

typedef struct list_t {
	char* directory;
	char *fsName;
	char* album;
	char* title;
	char* artist;
	struct list_t *next;
	struct list_t *prev;
} list_t;

void serialize (list_t *listDB);

typedef struct directory_list_t {
	char *name;
	struct directory_list_t *next;
	struct directory_list_t *prev;
} directory_list_t;

list_t *initialize_list ()
{
	list_t *ptr;
	ptr = (list_t *) malloc (sizeof (list_t));
	memset (ptr, 0, sizeof (list_t));
	ptr->prev = NULL;
	ptr->next = NULL;
	return ptr;
}

list_t *return_to_head (list_t *listDB)
{
	while (listDB->prev != NULL) {
		listDB = listDB->prev;
	}
	return listDB;
}

list_t *insert_tail (list_t *listDB)
{
	list_t *ptr = initialize_list ();
	ptr->prev = listDB;
	listDB->next = ptr;
	return ptr;
}

directory_list_t *add_directory (char *st, directory_list_t *dirs)
{
	directory_list_t *ptr = (directory_list_t *) malloc (sizeof (directory_list_t));
	
	ptr->name = strdup (st);
	ptr->next = NULL;
	ptr->prev = NULL;
	if (dirs == NULL) {
		return ptr;
	} else {
		dirs->next = ptr;
		ptr->prev = dirs;
		return ptr;
	}
}

directory_list_t *pop_directory (directory_list_t *dirs)
{
	directory_list_t *ptr;

	if (dirs == NULL) {
		return dirs;
	}
	ptr = dirs->prev;
	free (dirs->name);
	free (dirs);
	if (ptr != NULL) {
		ptr->next = NULL;
	}
	return ptr;
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

void destroy_list (list_t *ptr)
{
	list_t *tmp;

	if (ptr == NULL) {
		return;
	}
	if (ptr != NULL) {
		tmp = ptr->next;
		if (ptr->album != NULL) {
			free (ptr->album);
		}
		if (ptr->directory != NULL) {
			free (ptr->directory);
		}
		if (ptr->fsName != NULL) {
			free (ptr->fsName);
		}
		if (ptr->artist != NULL) {
			free (ptr->artist);
		}
		if (ptr->title != NULL) {
			free (ptr->title);
		}
		free (ptr);
		destroy_list (tmp);
	}
}


/*int main (int argc, char *argv[])*/
/*{*/
	/*gzFile fp = gzopen ("./db.zip", "r");*/
	/*list_t *listDB = initialize_list (), *ptr;*/
	/*directory_list_t *dirs = NULL;*/
	/*char buf[5000];*/
	
	/*while (gzgets (fp, buf, 5000) != NULL) {*/
		/*listDB = insert_line_in_memory (buf, listDB, &dirs);*/
		/*printf ("%s\n", buf);*/
	/*}*/
	/*gzclose (fp);*/

	/*listDB = return_to_head (listDB);*/
	/*serialize (listDB);*/
	/*destroy_list (listDB);*/

	/*return 0;*/
/*}*/

void serialize (list_t *listDB)
{
	tpl_node *tn;
	int i = 0;
	list_t TMP;
	
	tn = tpl_map ("A(S(ssss))", &TMP);
	/*tpl_pack (tn, 0);*/
	while (listDB != NULL) {
		TMP = *listDB;
		tpl_pack (tn, 1);
		listDB = listDB->next;
	}
	tpl_dump (tn, TPL_FILE, "list.serial");
	tpl_free (tn);
}





int main (int argc, char *argv[])
{
	FILE *fp = fopen ("/home/user/.mpd/db_2", "r");
	list_t *listDB = initialize_list (), *ptr;
	directory_list_t *dirs = NULL;
	char buf[5000];
	
	while (fgets (buf, 5000, fp) != NULL) {
		listDB = insert_line_in_memory (buf, listDB, &dirs);
		printf ("%s\n", buf);
	}
	fclose (fp);

	listDB = return_to_head (listDB);
	serialize (listDB);
	destroy_list (listDB);

	return 0;
}
