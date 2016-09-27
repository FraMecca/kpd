#define _GNU_SOURCE
#include <string.h> // strcpy strcat
#include <zlib.h> // gzgets gzopen
#include <stdlib.h>
#include <stdio.h> //sscanf
#include <stdbool.h> // true false
#include "kpd_search.h"

static bool is_contained (char *st, list_t *ptr);

static list_t *return_to_head (list_t *ptr)
{
	while (ptr->prev != NULL) {
		ptr = ptr->prev;
	}
	return ptr;
}

/************************************* directory_struct functions *************************************/ 

// creation of a new node of the directory
static directory_list_t *
new_directory_node(){
	directory_list_t *node;

	node = malloc(sizeof(directory_list_t));
	
	node->next = NULL;
	node->prev = NULL;
	node->name = NULL;

	return node;
}


static directory_list_t *
new_directory(directory_list_t *dirlist, char *dirname){
	directory_list_t *node;

	node = new_directory_node();
	
	// linking the nodes
	node->name = strdup (dirname);
	if (dirlist != NULL) {
		dirlist->next = node;
	}
	node->prev = dirlist;

	return	node;

}

static directory_list_t *
destroy_directory(directory_list_t *dirlist){
	directory_list_t *node;
	
	// node returned as current	
	node = dirlist->prev;
	if (node != NULL) {
		node->next = NULL;
	}
	free (dirlist->name);	
	free(dirlist);

	return node;
}

static char *
get_current_directory(directory_list_t *dirlist){
	int n=0;	
	directory_list_t *node;
	char *directory;

	if (dirlist == NULL) {
		return strdup ("");
	}

	for(node=dirlist; node->prev != NULL; node=node->prev){
		n += strlen(node->name) + 1;
	}
	n+= strlen (node->name) + 1; // the last one should be added as well

	directory = malloc((n + 1)*sizeof(char));	
	memset (directory, 0, (n+1)*sizeof (char));

	for( ; node != NULL; node = node->next){
		strcat(directory, node->name);
		strcat(directory, "/");
	}

	return directory;
}

/************************************* directory_struct functions *************************************/ 

static list_t *
new_node()
{
	list_t *x = malloc(sizeof(list_t));

	x->directory = NULL;
	x->fsName = NULL;
	x->album = NULL;
	x->title = NULL;
	x->artist = NULL;
	x->genre = NULL;
	x->date = NULL;
	x->next = NULL;
	x->prev = NULL;
	
	return x;
}

static void 
destroy_nodes (list_t *ptr)
{
	list_t *tmp;
	while (ptr != NULL) {
		if (ptr->directory != NULL) {
			free (ptr->directory);
			/*printf ("%s\n", ptr->directory);*/
		}
		if (ptr->fsName != NULL) {
			free (ptr->fsName);
		}
		if (ptr->album != NULL) {
			free (ptr->album);
		}
		if (ptr->title != NULL) {
			free (ptr->title);
		}
		if (ptr->artist != NULL) {
			free (ptr->artist);
		}
		if (ptr->genre != NULL) {
			free (ptr->genre);
		}
		if (ptr->date != NULL) {
			free (ptr->date);
		}
		tmp = ptr;
		ptr = ptr->next; 
		free (tmp);
	}
}

static list_t*
add_node(list_t* list, list_t* node)
{
	list->next = node;
	node->prev = list;
	list = node;
	return list;
}

static list_t*
create_list(const char* line, list_t *list, directory_list_t **dir)
{
	// insert line in memory
	list_t  *node = NULL;
	char temp[5000],type[20];

	sscanf(line, "%s %[^\n]", type, temp);
	type[strlen(type)-1] = '\0';
	
	//add new directory
	if(strcmp(type,"directory")==0)
	{
		*dir = new_directory(*dir,temp);			
		return list;
	}
	
	//begin new song
	if(strcmp(type,"song_begin")==0)
	{
		//if the head is empty
		if(list==NULL)
		{
			list = new_node();
			list->fsName = strdup (temp);
			list->directory = get_current_directory(*dir);
			return list;
		}
		//insert in the list, the pointer point to the last song
		else
		{
			node = new_node();
			node->fsName = strdup (temp);
			node->directory = get_current_directory(*dir);

			list = add_node(list, node);
			return list;
		}
	}
	
	//if the song end
	if(strcmp(type,"end")==0)
	{
		*dir = destroy_directory(*dir);
		return list;
	}
	
	if(strcmp(type,"genre")==0)
	{
		list->genre = strdup (temp);
		return list;
	}
	
	if(strcmp(type,"artist")==0)
	{
		list->artist = strdup (temp);
		return list;
	}
	
	if(strcmp(type,"date")==0)
	{
		list->date = strdup (temp);
		return list;
	}

	if(strcmp(type,"album")==0)
	{
		list->album = strdup (temp);
		return list;
	}

	if(strcmp(type,"title")==0)
	{
		list->title = strdup (temp);
		return list;
	}

	return list; // does not happen, but issues a warning because no if-else structure
}

/************************************ Begin of filter functions ************************************/
typedef struct Filter_struct {
	char **type;
	char **key;
	int size;
} Filter_struct;

static bool
filter_items (Filter_struct f, list_t *ptr, int i)
{
	if (strcmp (f.type[i], "Any") == 0) {
		return is_contained (f.key[i], ptr);
	} else {
		if (strcmp ("Artist", f.type[i]) == 0 && 
				ptr->artist != NULL && strcasestr (ptr->artist, f.key[i]) != NULL) {
			return 1;
		} else {
			if (strcmp ("Album", f.type[i]) == 0 && 
					ptr->album != NULL && strcasestr (ptr->album, f.key[i]) != NULL) {
				return 1;
			} else {
				if (strcmp ("Title", f.type[i]) == 0 && 
						ptr->title != NULL && strcasestr (ptr->title, f.key[i]) != NULL) {
					return 1;
				} else {
					if (strcmp ("Directory", f.type[i]) == 0 &&
							ptr->directory != NULL && strcasestr (ptr->directory, f.key[i]) != NULL) {
							return 1;
					}
				}
			}
		}
	}
	
	return 0;
}

static bool
filter_check (int flag, Filter_struct filter, list_t *listDB)
{
	// check if filter was issued
	int i;
	if (flag) {
		for (i = 0; i < filter.size; ++i) {
			if (!filter_items (filter, listDB, i)) {
				return 0;
			}
		}
		return 1;
	} else {
		return 1;
	}
}

static bool
rev_filter_check (int flag, Filter_struct filter, list_t *listDB)
{
	// check if rev_filter is issued
	int i;
	if (flag) {
		for (i = 0; i < filter.size; ++i) {
			if (!filter_items (filter, listDB, i)) {
				return 1;
			}
		}
		return 0;
	} else {
		return 1;
	}
}

static void 
destroy_filter_struct (Filter_struct filter)
{
	int i;
	for (i = 0; i < filter.size; ++i) {
		free (filter.type[i]);
		free (filter.key[i]);
	}
	free (filter.type);
	free (filter.key);
}

static Filter_struct 
parse_filter_struct (const char *filterSt)
{
	// this function create a filter struct to be used 
	Filter_struct filter;
	char *tmp = NULL, *tmpfSt = strdup (filterSt);

	filter.type = (char **) malloc (sizeof (char *));
	filter.key = (char **) malloc (sizeof (char *));
	/*memset (filter.type, sizeof (char *), 0);*/
	/*memset (filter.key, sizeof (char *), 0);*/


	filter.size = 0;
	tmp = strtok (tmpfSt, " ");
	/* ARtist banco Album tracce libro */
	// a filter can have a key such as artist or album
	// tokenize the string and check if there is a key or no tipekey was issued
	while (tmp != NULL) {
		if (strcmp ("Artist", tmp) == 0 || strcmp ("Album", tmp) == 0 || 
				strcmp ("Title", tmp) == 0 || strcmp ("Directory", tmp) == 0) {
			filter.type[filter.size] = strdup (tmp);
			tmp = strtok (NULL, " ");
		} else {
			filter.type[filter.size] = strdup ("any");
		}
		// tmp holds (now after the while or before the while if there is no typekey)
		filter.key[filter.size] = strdup (tmp);
		filter.size++;
		tmp = strtok (NULL, " "); // now continue

		filter.type = (char **) realloc (filter.type, (filter.size + 1) * sizeof (char *));
		filter.key = (char **) realloc (filter.key, (filter.size + 1) * sizeof (char *));
	}
	free (tmpfSt);

	return filter;
}

/************************************ End of filter functions ************************************/

static bool 
is_contained (char *st, list_t *ptr)
{
	/* This function 
	 * checks if st is contained in any of the entry of the ptr
	 */
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

static char *
get_complete_name (list_t *ptr)
{
	// this function returns the complete name AKA the uri for mpd
	char *st;

	st = calloc ((strlen (ptr->directory) + strlen (ptr->fsName) + 1), sizeof (char));
	strncpy (st, ptr->directory, strlen (ptr->directory));
	strncat (st, ptr->fsName, strlen (ptr->fsName));
	
	return (st);
}

void 
destroy_results (char **res, int size)
{
	int j; 
	if (res == NULL) {
		return;
	} else {
		for (j = 0; j < size; ++j) {
			free (res[j]);
		}
	}
	free (res);
}


static char **
search (char *st, list_t *listDB, int *cnt, Filter_struct filter, int filterFlag, Filter_struct revFilter, int revFilterFlag)
{
	struct resList {
		// will use a list to hold the result
		// will be transformed into a char array
		char *name;
		struct resList *next;
	};

	struct resList *resultsList = NULL, *head = NULL, *tmp = NULL;
	char **results;
	int size = 0, i; // size holds the number of entries

	resultsList = (struct resList *) malloc (sizeof (struct resList));
	resultsList->next = NULL;
	head = resultsList; // head stores the root of the list
	
	while (listDB != NULL) {
		if (is_contained (st, listDB) && 
				filter_check (filterFlag, filter, listDB) && 
				rev_filter_check (revFilterFlag, revFilter, listDB)) {
			// the if checks if the word is contained and is ok with filters
			resultsList->name = get_complete_name (listDB);
			// now allocate another node
			resultsList->next = (struct resList *) malloc (sizeof (struct resList));
			resultsList = resultsList->next;
			resultsList->next = NULL;

			size++;
		}
		listDB = listDB->next;
	}

	// now convert resultsList into a char **
	results = (char **) malloc (size * sizeof (char *));
	resultsList = head;
	/*head = NULL;*/
	for (i = 0; i < size; ++i) {
		results[i] = strdup (resultsList->name);
		tmp = resultsList; // for next free
		resultsList = resultsList->next;
			free (tmp->name);
		free (tmp);
	}
	free (resultsList); // no name field in last
	
	*cnt = size;
	return results;
}

char **
search_handler (char *key, int *size, char *DBlocation, char *filterSt, char *revFilterSt)
{
	// This function is the main handler for the search
	
	gzFile fp = gzopen (DBlocation, "r");
	list_t *kpdDB = NULL; // will be initialized in create_list
	directory_list_t *dir = NULL;

	char buf[5000], **results;
	int filterFlag, revFilterFlag;
	Filter_struct filter, revFilter;

	while (gzgets (fp, buf, 5000) != NULL) {
		kpdDB = create_list (buf, kpdDB, &dir);
	}
	gzclose (fp);

	// now consider the filters
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
	
	kpdDB = return_to_head (kpdDB);
	// now the search can start 
	results = search (key, kpdDB, size, filter, filterFlag, revFilter, revFilterFlag);

	/*// print results*/
	/*for (i = 0; i < size; ++i) {*/
		/*printf ("%s\n", results[i]);*/
	/*}*/

	destroy_nodes (kpdDB); // free the struct used to allocate the whole mpd db
	if (filterFlag != false) {
		destroy_filter_struct (filter);
	}
	if (revFilterFlag != false) {
		destroy_filter_struct (revFilter);
	}

	/*destroy_results (results, size);*/
	return results;
}

/*int main (int argc, char **argv)*/
/*{*/
	/*int i;*/
	/*char **results;*/
	/*results = search_handler (argv[1], &i, "/home/user/.mpd/database", NULL, NULL);*/
/*int j;*/
	/*for (j = 0; j < i; ++j) {*/
		/*[>printf ("%s\n", results[j]);<]*/
		/*free (results[j]);*/
	/*}*/
	/*free (results);*/


	/*return 0;*/
/*}*/
