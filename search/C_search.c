#define _GNU_SOURCE
#include "list_kpd.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <zlib.h>
#include <string.h>
#include "C_search.h"
#include "filter.h"

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
					if (ptr->directory != NULL && strcasestr (ptr->directory, st) != NULL) {
						return 1;
					} else {
						return 0;
					}
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


char **search (char *st, list_t *listDB, int *cnt, Filter_struct filter, int filterFlag, Filter_struct revFilter, int revFilterFlag)
{
	char *res, **results;
	int size = 0;

	results = malloc (0 * sizeof (char*));


	while (listDB != NULL) {
		if (is_contained (st, listDB) && 
				filter_check (filterFlag, filter, listDB) && 
				rev_filter_check (revFilterFlag, revFilter, listDB)) {
			
			res = (get_complete_name (listDB));
			size++;
			results = (char **) realloc (results, size * sizeof (char *));
			results[size - 1] = strdup (res);
			free (res);
		}
		listDB = listDB->next;
	}
	
	*cnt = size;
	return results;
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
	/*gzFile fp = gzopen ("/home/user/.mpd/database", "r");*/
	/*list_t *listDB = initialize_list ();*/
	/*directory_list_t *dirs = NULL;*/
	/*char buf[5000];*/
	int i = 0;
	
	/*while (gzgets (fp, buf, 5000) != NULL) {*/
		/*listDB = insert_line_in_memory (buf, listDB, &dirs);*/
	/*}*/
	/*gzclose (fp);*/

	/*[>listDB = deserialize ();<]*/
	/*listDB = return_to_head (listDB);*/
	/*search (argv[1], listDB, &i);*/

	/*destroy_list (listDB);*/

	search_c_main (argv[1], &i, "/home/user/.mpd/database", argv[2], argv[3]);

	return 0;
}

Filter_struct parse_filter_struct (char *filterSt)
{
	Filter_struct filter;
	char *tmp;

	filter.type = (char **) malloc (sizeof (char *));
	filter.key = (char **) malloc (sizeof (char *));
	memset (filter.type, sizeof (char *), 0);
	memset (filter.key, sizeof (char *), 0);


	filter.size = 0;
	tmp = strtok (filterSt, " ");
	while (tmp != NULL) {
		if (strcasecmp ("artist", tmp) == 0 || strcasecmp ("album", tmp) == 0 || 
				strcasecmp ("title", tmp) == 0 || strcasecmp ("directory", tmp) == 0) {
			filter.type[filter.size] = strdup (tmp);
			tmp = strtok (NULL, " ");
		} else {
			filter.type[filter.size] = strdup ("any");
		}
		filter.key[filter.size] = strdup (tmp);
		filter.size++;
		tmp = strtok (NULL, " ");

		filter.type = (char **) realloc (filter.type, (filter.size + 1) * sizeof (char *));
		filter.key = (char **) realloc (filter.key, (filter.size + 1) * sizeof (char *));
	}

	return filter;
}

char **search_c_main (char *key, int *size, char *DBlocation, char *filterSt, char *revFilterSt)
{
	gzFile fp = gzopen (DBlocation, "r");
	list_t *listDB = initialize_list ();
	directory_list_t *dirs = NULL;
	char buf[5000], **results;
	int i, filterFlag, revFilterFlag;
	Filter_struct filter, revFilter;

	while (gzgets (fp, buf, 5000) != NULL) {
		listDB = insert_line_in_memory (buf, listDB, &dirs);
	}
	gzclose (fp);

	if (filterSt != NULL) {
		filterFlag = 1;
		filter = parse_filter_struct (filterSt);
	} else {
		filterFlag = 0;
	}
	if (revFilterSt != NULL) {
		revFilterFlag = 1;
		revFilter = parse_filter_struct (revFilterSt);
	} else {
		revFilterFlag = 0;
	}
	listDB = return_to_head (listDB);
	results = search (key, listDB, size, filter, filterFlag, revFilter, revFilterFlag);
	destroy_list (listDB);
	if (filterSt != NULL) {
		destroy_filter_struct (filter);
	}
	return results;
}

void destroy_results (char **results, int size)
{
	int i;
	for (i = 0; i < size; ++i) {
		free (results[i]);
	}
	free (results);
}

void print_results (char **results, int size)
{
	int i;
	for (i = 0; i < size; ++i) {
		printf ("%s\n", results[i]);
	}
}
