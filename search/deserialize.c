#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tpl.h"

typedef struct list_t {
	char* directory;
	char *fsName;
	char* album;
	char* title;
	char* artist;
	struct list_t *next;
	struct list_t *prev;
} list_t;

list_t *initialize_list ()
{
	list_t *ptr;
	ptr = (list_t *) malloc (sizeof (list_t));
	memset (ptr, 0, sizeof (list_t));
	ptr->prev = NULL;
	ptr->next = NULL;
	return ptr;
}

list_t *insert_tail (list_t *listDB)
{
	list_t *ptr = initialize_list ();
	ptr->prev = listDB;
	listDB->next = ptr;
	return ptr;
}


list_t *deserialize ()
{
	tpl_node *tn;
	list_t TMP, *ptr = initialize_list ();
	int i = 0, c;

	tn = tpl_map ("A(S(ssss))", &TMP);
	tpl_load (tn, TPL_FILE, "list.serial.old");
	while (( c = tpl_unpack (tn, 1)) > 2) {
		TMP.next = NULL;
		TMP.prev = ptr->prev;
		*ptr = TMP;
		/*printf ("%s  %d-%d\n", new->fsName, i, c);*/
		ptr = insert_tail (ptr);
		memset (&TMP, 0, sizeof (list_t));
		/*++i;*/
	}
	tpl_free (tn);

	return ptr;

}

list_t *return_to_head (list_t *ptr)
{
	while (ptr->prev != NULL) {
		ptr = ptr->prev;
	}
	return ptr;
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

char *lower (char *st)
{
	int i;
	if (st == NULL) {
		return "NOTHING";
	}

	for (i = 0; st[i]; ++i) {
		st[i] = tolower (st[i]);
	}
	return st;
}

int is_contained (char *st, list_t *listDB)
{
	char *key, **array;
	int i;

	array = (char **) malloc (4 * sizeof (char *));
	memset (array, 0, 4 * sizeof (char));

	key = lower (st);
	array[0] = strdup( lower (listDB->fsName));
	array[1]= strdup( lower (listDB->album));
	array[2]= strdup( lower (listDB->title));
	array[3] = strdup( lower (listDB->artist));

	for (i = 0; i < 4; ++i) {
		if (strstr (array[i], key) != NULL) {
			for (i = 0; i < 4; ++i) free (array[i]);
			free (array);
			return 1;
		}
		/*printf ("confront %s vs %s\n", key, array[i]);*/
	}
	for (i = 0; i < 4; ++i) free (array[i]);
	free (array);
	return 0;
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

int main (int argc, char *argv[])
{
	list_t *ptr;
	int i;

	ptr = deserialize ();
	ptr = return_to_head (ptr);
	search (argv[1], ptr);

	destroy_list (ptr);
	return 0;
}
