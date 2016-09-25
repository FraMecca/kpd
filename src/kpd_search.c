#include "kpd_search.h"
#include <string.h>

list_t
node()
{
	list_t x = malloc(sizeof(struct list));

	x->directory = NULL;
	x->fsName = NULL;
	x->album = NULL;
	x->title = NULL;
	x->artist = NULL;
	x->genre = NULL;
	x->date = NULL;
	x->netx = NULL;
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
create_list(const char* line)
{
	list_t list = NULL, node = NULL;
	char temp[50],type[50];
	directory_list_t dir = NULL;
	int l = 0;

	dir = new_directory_node();

	sscanf(line"%s:%s", type, temp);
	
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
			list = node();
			l = strlen(temp);
			list->fsName = malloc(l*sizeof(char));
			strcpy(list->fsName,temp);
			list->directory = get_current_directory(dir);
		}
		//insert in the list, the pointer point to the last song
		else
		{
			node = node();
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







