#include <stdlib.h>
#include <zlib.h> // gzgets gzopen
#include <stdio.h> //sscanf
#include <stdbool.h> // true false
#include "kpd_search.h"
#define _GNU_SOURCE
#include <string.h> // strcpy strcat

static bool is_contained (char *st, list_t *ptr);

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
	strcpy(dirname, node->name);
	dirlist->next = node;
	node->prev = dirlist;

	return	node;

}

static directory_list_t *
destroy_directory(directory_list_t *dirlist){
	directory_list_t *node;
	
	// node returned as current	
	node = dirlist->prev;
	
	free(dirlist);

	return node;
}

static char *
get_current_directory(directory_list_t *dirlist){
	int n=0;	
	directory_list_t *node;
	char *directory;

	for(node=dirlist; node->prev != NULL; node=node->prev){
		n += strlen(node->name) + 1;
	}

	directory = malloc(n*sizeof(char));	

	for( ; node->next != NULL; node = node->next){
		strcat(directory, node->name);
		strcat(directory, "/");
	}

	return directory;
}

/************************************* directory_struct functions *************************************/ 

list_t *
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

list_t*
add_node(list_t* list, list_t* node)
{
	list->next = node;
	node->prev = list;
	list = node;
	return list;
}

list_t*
create_list(const char* line, list_t *list, directory_list_t *dir)
{
	// insert line in memory
	list_t  *node = NULL;
	char temp[5000],type[50];
	int l = 0;


	sscanf(line, "%s:%s", type, temp);
	
	//add new directory
	if(strcmp(type,"directory")==0)
	{
		//if exit from the main directory
		if(dir==NULL)
	   	{
			dir = new_directory_node();
		}
		dir = new_directory(dir,temp);			
	}
	
	//begin new song
	if(strcmp(type,"song_begin")==0)
	{
		//if the head is empty
		if(list==NULL)
		{
			list = new_node();
			l = strlen(temp);
			list->fsName = malloc(l*sizeof(char));
			strcpy(list->fsName,temp);
			list->directory = get_current_directory(dir);
		}
		//insert in the list, the pointer point to the last song
		else
		{
			node = new_node();
			l = strlen(temp);
			node->fsName = malloc(l*sizeof(char));
			strcpy(node->fsName,temp);
			node->directory = get_current_directory(dir);

			list = add_node(list, node);
		}
	}
	
	//if the song end
	if(strcmp(type,"end")==0)
	{
		dir = destroy_directory(dir);
	}
	
	if(strcmp(type,"genre")==0)
	{
		l = strlen(temp);
		list->genre = malloc(l*sizeof(char));
		strcpy(list->genre,temp);
	}
	
	if(strcmp(type,"artist")==0)
	{
		l = strlen(temp);
		list->artist = malloc(l*sizeof(char));
		strcpy(list->artist,temp);
	}
	
	if(strcmp(type,"date")==0)
	{
		l = strlen(temp);
		list->date = malloc(l*sizeof(char));
		strcpy(list->date,temp);
	}

	if(strcmp(type,"album")==0)
	{
		l = strlen(temp);
		list->album = malloc(l*sizeof(char));
		strcpy(list->album,temp);
	}

	if(strcmp(type,"title")==0)
	{
		l = strlen(temp);
		list->title = malloc(l*sizeof(char));
		strcpy(list->title,temp);
	}

	return list;
}



/************************************ Begin of filter functions ************************************/
typedef struct Filter_struct {
	char **type;
	char **key;
	int size;
} Filter_struct;

bool
filter_items (Filter_struct f, list_t *ptr, int i)
{
	if (strcasecmp (f.type[i], "any") == 0) {
		return is_contained (f.key[i], ptr);
	} else {
		if (strcasecmp ("artist", f.type[i]) == 0 && 
				ptr->artist != NULL && strcasestr (ptr->artist, f.key[i]) != NULL) {
			return 1;
		} else {
			if (strcasecmp ("album", f.type[i]) == 0 && 
					ptr->album != NULL && strcasestr (ptr->album, f.key[i]) != NULL) {
				return 1;
			} else {
				if (strcasecmp ("title", f.type[i]) == 0 && 
						ptr->title != NULL && strcasestr (ptr->title, f.key[i]) != NULL) {
					return 1;
				} else {
					if (strcasecmp ("directory", f.type[i]) == 0 &&
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
parse_filter_struct (char *filterSt)
{
	// this function create a filter struct to be used 
	Filter_struct filter;
	char *tmp;

	filter.type = (char **) malloc (sizeof (char *));
	filter.key = (char **) malloc (sizeof (char *));
	/*memset (filter.type, sizeof (char *), 0);*/
	/*memset (filter.key, sizeof (char *), 0);*/


	filter.size = 0;
	tmp = strtok (filterSt, " ");
	// a filter can have a key such as artist or album
	// tokenize the string and check if there is a key or no tipekey was issued
	while (tmp != NULL) {
		if (strcasecmp ("artist", tmp) == 0 || strcasecmp ("album", tmp) == 0 || 
				strcasecmp ("title", tmp) == 0 || strcasecmp ("directory", tmp) == 0) {
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
	char *st = malloc ( (1 + strlen (ptr->directory) + strlen (ptr->fsName)) * sizeof (char));

	strcpy (st, ptr->directory);
	st[strlen (st) - 1] = '/';
	strcat (st, ptr->fsName);
	st[strlen (st) - 1] = '\0';
	return (st);
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

	struct resList *resultsList = NULL, *head = NULL;
	char *res, **results;
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
	for (i = 0; i < size; ++i) {
		results[i] = strdup (resultsList->name);
		resultsList = resultsList->next;
	}
	
	*cnt = size;
	return results;
}

char **
search_handler (char *key, int *size, char *DBlocation, char *filterSt, char *revFilterSt)
{
	// This function is the main handler for the search
	
	gzFile fp = gzopen (DBlocation, "r");
	list_t *kpdDB = NULL; // will be initialized in create_list
	directory_list_t *dir = new_directory_node();

	char buf[5000], **results;
	int i, filterFlag, revFilterFlag;
	Filter_struct filter, revFilter;

	while (gzgets (fp, buf, 5000) != NULL) {
		kpdDB = create_list (buf, kpdDB, dir);
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
	
	// now the search can start 
	results = search (key, kpdDB, size, filter, filterFlag, revFilter, revFilterFlag);

	// destroy filter_struct
	// destroy search_struct
}
