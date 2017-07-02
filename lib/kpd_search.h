#ifndef LIST_KPD
#define LIST_KPD

typedef struct list_t {
	char* directory;
	char *fsName;
	char* album;
	char* title;
	char* artist;
	char* genre;
	char* date;
	struct list_t *next;
	struct list_t *prev;
} list_t;


typedef struct directory_list_t {
	char *name;
	struct directory_list_t *next;
	struct directory_list_t *prev;
} directory_list_t;

/*
 * search_handler manages search for kpd.
 * it wants a single key and a size pointer to store the size of the results;
 * it uses also a system of filters.
 */
char ** search_handler (int *size, char *DBlocation, char *filterSt);
	/*	 search_handler does:
 	 *	 1. loads the db in memory
 	 *	 2. searches the key in the list that contains the db
 	 *	 3. filters the results for filterSt and revFiltersSt
 	 *	 4. prints the result --> it doesn't anymore	
 	 *	 5. frees the datastructures --> must be called from main at end of execution
 	 *	
 	 */
void destroy_results (char **results, int size);

#endif
