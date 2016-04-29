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

directory_list_t *pop_directory (directory_list_t *dirs);
directory_list_t *add_directory (char *st, directory_list_t *dirs);
void destroy_list (list_t *ptr);
list_t *return_to_head (list_t *ptr);
list_t *insert_tail (list_t *listDB);
list_t *initialize_list ();

#endif
