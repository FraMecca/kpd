#include "list_kpd.h"
char **search_c_main (char *key, int *size, char *DBlocation, char *filterSt, char *revFilterSt);
void destroy_results (char **results, int size);
void print_results (char **results, int size);
int is_contained (char *st, list_t *ptr);
