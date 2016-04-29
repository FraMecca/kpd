#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list_kpd.h"

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
