#ifndef LIST_KPD
#define LIST_KPD

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

#endif
