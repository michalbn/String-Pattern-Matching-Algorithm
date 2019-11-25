#include"slist.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define FATAL_ERROR -1

//private function that create new node- one cell
slist_node_t * GetNewNode(void *my_data)
 { 
	slist_node_t* new_Node;
	new_Node=(slist_node_t*)malloc(sizeof(slist_node_t));
	if(new_Node==NULL)
		return NULL;//create node was unsuccessful
	slist_data(new_Node)=my_data;
	return new_Node; 
}

//initialize my list
void slist_init(slist_t *list)
{
	if(list==NULL)
		return;
	slist_head(list)=NULL;
	slist_tail(list)=NULL;
	slist_size(list)=0;
}

//delete list after malloc
void slist_destroy(slist_t * list ,slist_destroy_t destList)
{
		if(list==NULL||slist_head(list)==NULL)//list not exist
		return;
	
		slist_node_t* tempHead= slist_head(list);
		while(tempHead!=NULL)
		{
			if(destList==SLIST_FREE_DATA)
				free(slist_data(tempHead));
			
			tempHead=slist_next(slist_head(list));
			free(slist_head(list));
			slist_head(list)=tempHead;
			slist_size(list)=slist_size(list)-1;
		}
		
	slist_init(list);//empty list again
	
}

//pop data from list and delete the cell
void *slist_pop_first(slist_t *list)
{
	if(list==NULL)
		return NULL;
	if(slist_head(list)==NULL)//nothing to pop
		return NULL;
	else
	{
		
		slist_node_t *tempHead=slist_head(list);
		void *data =slist_data(tempHead);
		slist_node_t *next=slist_next(slist_head(list));
		free(tempHead);
		slist_head(list)=next;
		slist_size(list)=slist_size(list)-1;
		return data;
	}
}

//add node to tail
int slist_append(slist_t *list ,void *my_data)
{
	if(list==NULL)
		return FATAL_ERROR;

	slist_node_t *ptr1;
	ptr1=GetNewNode(my_data);
	if(ptr1==NULL)
		return FATAL_ERROR;	
	if (slist_head(list) == NULL)//if there is nothing in my list
	{
		slist_head(list)= ptr1;
		slist_tail(list)= ptr1;
		slist_next(slist_tail(list))=NULL;
		slist_size(list)=slist_size(list)+1;
		
	}
	else
	{
		slist_next(slist_tail(list))=ptr1;
		slist_tail(list)=ptr1;
		slist_next(ptr1)=NULL;
		slist_size(list)=slist_size(list)+1;
	}
	return 0;
}

//add node to head
int slist_prepend(slist_t *list ,void *my_data)
{
	if(list==NULL)
		return FATAL_ERROR;
	slist_node_t * ptr;
	ptr=GetNewNode(my_data);
	if(ptr==NULL)//create node was unsuccessful
		return FATAL_ERROR;	
	if (slist_head(list) == NULL)//if there is nothing in my list
	{
		slist_head(list)= ptr;
		slist_next(slist_head(list))=NULL;
		slist_size(list)=slist_size(list)+1;
	}
	else
	{
		slist_next(ptr)=slist_head(list);
		slist_head(list)=ptr;
		slist_size(list)=slist_size(list)+1;
	}
	return 0;
}


int slist_append_list(slist_t* list1, slist_t* list2)
{
	if((list1==NULL)||(list2==NULL))
		return FATAL_ERROR;
	int i;
	int size=slist_size(list2);
	slist_node_t *ptr=slist_head(list2);
	for(i=0;i<size;i++)
	{
		if(slist_append(list1,slist_data(ptr))==-1)//if addition was unsuccessful
			return FATAL_ERROR;
		ptr=slist_next(ptr);	
	}
	return 0;
}



