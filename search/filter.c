#define _GNU_SOURCE
#include "list_kpd.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "C_search.h"
#include "filter.h"



int filter_items (Filter_struct f, list_t *ptr, int i)
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

int filter_check (int flag, Filter_struct filter, list_t *listDB)
{
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

int rev_filter_check (int flag, Filter_struct filter, list_t *listDB)
{
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

void destroy_filter_struct (Filter_struct filter)
{
	int i;
	for (i = 0; i < filter.size; ++i) {
		free (filter.type[i]);
		free (filter.key[i]);
	}
	free (filter.type);
	free (filter.key);
}
