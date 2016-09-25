#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcpy strcat


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


directory_list_t *
new_directory(directory_list_t *dirlist, char *dirname){
	directory_list_t *node;

	node = new_directory_node();
	
	// linking the nodes
	strcpy(dirname, node.name);
	dirlist->next = node;
	node->prev = dirlist;

	return	node;

}

directory_list_t *
destroy_directory(directory_list_t *dirlist){
	directory_list_t *node;
	
	// node returned as current	
	node = dirlist->prev;
	
	free(dirlist);

	return node;
}

char *
get_current_directory(directory_list_t *dirlist){
	int n=0;	
	directory_list_t *node;
	char *directory;

	for(node=dirlist; node->prev != NULL; node=node->prev){
		n += strlen(node.name) + 1;
	}

	directory = malloc(n*sizeof(char));	

	for( ; node->next != NULL; node = node->next){
		strcat(directory, node->name);
		strcat(directory, "/");
	}

	return directory;
}
