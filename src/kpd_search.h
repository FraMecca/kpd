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
char ** search_handler (char *key, int *size, char *DBlocation, char *filterSt, char *revFilterSt);

#endif
