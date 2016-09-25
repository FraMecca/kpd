#include <zlib.h> // gzgets gzopen


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
	memset (filter.type, sizeof (char *), 0);
	memset (filter.key, sizeof (char *), 0);


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
	list_t *kpdDB = initialize_list ();
	directory_list_t *dirs = NULL;
	char buf[5000], **results;
	int i, filterFlag, revFilterFlag;
	Filter_struct filter, revFilter;

	while (gzgets (fp, buf, 5000) != NULL) {
		listDB = insert_line_in_memory (buf, listDB, &dirs);
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
