#ifndef FILTER_KPD
#define FILTER_KPD

typedef struct Filter_struct {
	char **type;
	char **key;
	int size;
} Filter_struct;

void destroy_filter_struct (Filter_struct filter);
int filter_check (int flag, Filter_struct filter, list_t *listDB);
int rev_filter_check (int flag, Filter_struct filter, list_t *listDB);
int filter_items (Filter_struct f, list_t *ptr, int i);

#endif
