#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct list_t {
	char* directory;
	char *fsName;
	char* album;
	char* title;
	char* artist;
	struct list_t *next;
	struct list_t *prev;
} list_t;

typedef struct directory_list_t {
	char *name;
	struct directory_list_t *next;
	struct directory_list_t *prev;
} directory_list_t;

list_t *get_list_head (list_t *listDB)
{
	if (listDB == NULL) {
		listDB = (list_t *) malloc (sizeof (list_t));
		listDB->prev = NULL;
		listDB->next = NULL;
	}
	return listDB;
}

list_t *return_to_head (list_t *listDB)
{
	while (listDB->prev != NULL) {
		listDB = listDB->prev;
	}
	printf (listDB->fsName);
	return listDB;
}


list_t *insert_tail (list_t *listDB)
{
	list_t *ptr = (list_t *) malloc (sizeof (list_t));
	listDB->next = ptr;
	ptr->prev = listDB;
	ptr->next = NULL;
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
	free (dirs);
	if (ptr != NULL) {
		ptr->next = NULL;
	}
	return ptr;
}

list_t* insert_line_in_memory (char *line, list_t *listDB, directory_list_t **dirsRef)
{
	list_t *ptr;
	char *partOneSt, *partTwoSt;
	char * stringa;
	
	directory_list_t *dirs = dirsRef[0];

	partOneSt = strtok (line, ": ");
	partTwoSt = strtok (NULL, ":");
	if (partTwoSt != NULL) {
		partTwoSt = partTwoSt + 1;
	}
	stringa = strdup (line);

	/*printf ("partone: %s-\npartTwo: %s\nstring == %s\n", partOneSt, partTwoSt, stringa);*/

	if (strcmp (partOneSt, "begin") == 0) {
		dirs = add_directory (partTwoSt, dirs); //directory are added to a list that are popped when end tag, added when begin tag;
		dirsRef[0] = dirs;
		return listDB;
	}
	
	ptr = get_list_head (listDB);
	
	if (strcmp (partOneSt, "song_begin") == 0) {
		ptr->fsName = strdup (partTwoSt);
		if (dirs == NULL) {
			ptr->directory = strdup ("\0");
		}
		else {
			ptr->directory = strdup (dirs->name);
		}
		return ptr;
	}
	if (strcmp (partOneSt, "end") == 0) {
		dirs = pop_directory (dirs);
		dirsRef[0] = dirs;
		return ptr;
	}
	if (strcmp (stringa, "song_end\n") == 0) {
		ptr = insert_tail (ptr);
		return ptr;
	}
	if (strcmp (partOneSt, "Artist") == 0) {
		ptr->artist = strdup (partTwoSt);
		return ptr;
	}
	if (strcmp (partOneSt, "Album") == 0) {
		ptr->album = strdup (partTwoSt);
		return ptr;
	}
	if (strcmp (partOneSt, "Title") == 0) {
		ptr->title = strdup (partTwoSt);
		return ptr;
	}

	/*printf ("ritorno %s\n", ptr->fsName);*/
	return ptr;
}

char *lower (char *st)
{
	int i;
	char *tmp;
	if (st == NULL) {
		return "A";
	}

	tmp = (char *) malloc (strlen (st) * sizeof (char));

	for (i = 0; st[i]; ++i) {
		tmp[i] = tolower (st[i]);
	}
	return tmp;
}

int is_contained (char *st, list_t *listDB)
{
	char *key, **array;
	int i;

	array = (char **) malloc (4 * sizeof (char *));

	key = lower (st);
	array[0] = strdup( lower (listDB->fsName));
	array[1]= strdup( lower (listDB->album));
	array[2]= strdup( lower (listDB->title));
	array[3] = strdup( lower (listDB->artist));

	for (i = 0; i < 4; ++i) {
		if (strstr (array[i], key) != NULL) {
			return 1;
		}
		/*printf ("confront %s vs %s\n", key, array[i]);*/
	}
	return 0;
}

void search (char *st, list_t *listDB)
{
	printf ("%s\n", st);
	while (listDB != NULL) {
		if (is_contained (st, listDB)) {
			printf ("%s\n", listDB->fsName);
		}
		listDB = listDB->next;
	}
}

	
int main (int argc, char *argv[])
{
	FILE *fp = fopen ("/home/user/.mpd/db_unzip", "r");
	list_t *listDB = NULL;
	directory_list_t *dirs = NULL;
	char buf[5000];
	
	while (fgets (buf, 5000, fp) != NULL) {
		listDB = insert_line_in_memory (buf, listDB, &dirs);
		if (dirs != NULL) {
		}
	}
	fclose (fp);

	listDB = return_to_head (listDB);
	search (argv[1], listDB);

	return 0;
}
