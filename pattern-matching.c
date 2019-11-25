#include "pattern-matching.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FATAL_ERROR -1

int pm_init(pm_t * root)//initialize my DFA
{
	if(root==NULL)
		return FATAL_ERROR;
	root->newstate=1;
	root->zerostate=(pm_state_t*)malloc(sizeof(pm_state_t));
	if(root->zerostate==NULL)
		return FATAL_ERROR;//create node was unsuccessful
	root->zerostate->id=0;
	root->zerostate->depth=0;
	root->zerostate->output=(slist_t*)malloc(sizeof(slist_t));
	if(root->zerostate->output==NULL)
		return FATAL_ERROR;//create node was unsuccessful
	slist_init(root->zerostate->output);
	root->zerostate->fail=NULL;
	root->zerostate->_transitions=(slist_t*)malloc(sizeof(slist_t));
	if(root->zerostate->_transitions==NULL)
		return -1;//create node was unsuccessful
	slist_init(root->zerostate->_transitions);
	return 0;
}


int pm_addstring(pm_t *root,unsigned char * stateLabel, size_t n)//add tree to my DFA
{
	int i;
	if((root==NULL) ||(stateLabel==NULL))
		return FATAL_ERROR;
	pm_state_t *from_state=root->zerostate;
	pm_state_t *to_state;

	for(i=0;i<n;i++)
	{
	to_state=pm_goto_get(from_state,stateLabel[i]);//check if stateLabel[i] already exist
		if(to_state==NULL)//not exist
		{
			to_state =(pm_state_t*)malloc(sizeof(pm_state_t));//create value to my new state
			if(to_state==NULL)
				return FATAL_ERROR;//create node was unsuccessful
			to_state->id=root->newstate ;
			root->newstate+=1;
			to_state->depth=i+1;
			to_state->output=(slist_t*)malloc(sizeof(slist_t));
			if(to_state->output==NULL)
			return -1;//create node was unsuccessful
			slist_init(to_state->output);
			to_state->fail=NULL ;
			to_state->_transitions=(slist_t*)malloc(sizeof(slist_t));
			if(to_state->_transitions==NULL)
			return -1;//create node was unsuccessful
			slist_init(to_state->_transitions);
			if(pm_goto_set(from_state,stateLabel[i],to_state)==-1)//create new state
				return FATAL_ERROR;							
		}
			from_state=to_state;
	}
	//enter output
	if(slist_append(from_state->output ,stateLabel)==-1)//leaf-"finish mode"
			return FATAL_ERROR;
	return 0;
}

int pm_makeFSM(pm_t *root)//make fail state to DFA
{ 
	if(root==NULL)
		return FATAL_ERROR;	
	int i;
	slist_t *list=root->zerostate->_transitions;
	slist_node_t *tempHead=slist_head(list);
	slist_t *queue=(slist_t*)malloc(sizeof(slist_t));//create priority queue- "bfs"
	if(queue==NULL)
			return FATAL_ERROR;//create node was unsuccessful
	slist_init(queue);

	for(i=0;i<slist_size(root->zerostate->_transitions) ;i++)//first level- fail go to root
	{	
		
		pm_labeled_edge_t* edge=(pm_labeled_edge_t*)slist_data(tempHead);
		if(slist_append(queue ,edge->state)==-1)
			return FATAL_ERROR;
		edge->state->fail=root->zerostate;
		printf("Setting f(%u) = %u\n",edge->state->id,edge->state->fail->id);
		tempHead=slist_next(tempHead);
	}

	while(slist_head(queue)!=NULL)//other level
	{
	
		pm_state_t *state =(pm_state_t*)slist_pop_first(queue);//take the first state in queue
		if(state==NULL)
			return FATAL_ERROR;//pop was unsuccessful
		tempHead=slist_head(state->_transitions);
		for(i=0;i<slist_size(state->_transitions); i++)
		{
			pm_labeled_edge_t* edge=(pm_labeled_edge_t*)slist_data(tempHead);
			pm_state_t* fail_state=state->fail;//fail state-go to "father"
			pm_state_t* goTo_state= pm_goto_get(fail_state,edge->label);//next state after fail state
			while(goTo_state==NULL)
			{
				if(fail_state->depth==0)//if i in the root
				{
					goTo_state=root->zerostate;
					break;
				}
				fail_state=fail_state->fail;
				goTo_state= pm_goto_get(fail_state,edge->label);
			}
			edge->state->fail=goTo_state;//connect my state to fail state
			slist_append_list(edge->state->output, goTo_state->output);//add to my output the output of the fail state
			slist_append(queue ,edge->state);//add state "son" to the queue
			printf("Setting f(%u) = %u\n",edge->state->id,edge->state->fail->id);
			tempHead=slist_next(tempHead);
		}
	
	}
	free(queue);
	return 0;
}


int pm_goto_set(pm_state_t *from_state,unsigned char symbol, pm_state_t *to_state)//put label between 2 state (edge)
{
	if(from_state==NULL||to_state==NULL)
		return FATAL_ERROR;
	pm_labeled_edge_t* newEdge=(pm_labeled_edge_t*)malloc(sizeof(pm_labeled_edge_t));	
	if(newEdge==NULL)
		return FATAL_ERROR;//create node was unsuccessful
	newEdge->label=symbol;
	newEdge->state=to_state;
	if(slist_append(from_state->_transitions,newEdge)==-1)//add connection between 2 state 
		return FATAL_ERROR ;
	else
	{
		printf("Allocating state %u \n",newEdge->state->id);
		printf("%u -> %c-> %u\n",from_state->id,newEdge->label,to_state->id);
		return 0;
	}

}

pm_state_t* pm_goto_get(pm_state_t *state,unsigned char symbol)//get the next state-get the "son"
{
	if(state==NULL)
		return NULL;
	slist_node_t *ptr=slist_head(state->_transitions);
	pm_labeled_edge_t* edgeNode;
	while(ptr!=NULL)//passing all the son
	{
		edgeNode=(pm_labeled_edge_t*)slist_data(ptr);
		if(edgeNode->label==symbol)//if the son exist
			{
				return edgeNode->state;
			}
		ptr=slist_next(ptr);
	}

	return NULL;
}


slist_t* pm_fsm_search(pm_state_t * state,unsigned char * userString ,size_t n)//serch match from user string to my DFA
{
	
	if(state==NULL||userString==NULL)//state = zerostate at first
		return NULL;
	int i,j;
	pm_state_t *to_state;
	slist_t* matchList=(slist_t*)malloc(sizeof(slist_t));//create match list
	if(matchList==NULL)
		return NULL;//create node was unsuccessful
	slist_init(matchList);
	for(i=0;i<n;i++)
	{
		to_state=pm_goto_get(state,userString[i]);
		pm_state_t* fail_state=state;
		
		while(to_state==NULL)//if this latter from userString not exsit
			{
				if(fail_state->depth==0)
				{
					
					to_state=fail_state;
					break;
				}
				fail_state=fail_state->fail;
				if(fail_state==NULL)
				{
					return NULL;
					slist_destroy(matchList,SLIST_FREE_DATA);

				}
				to_state= pm_goto_get(fail_state,userString[i]);//go to fail state
			}
		if(slist_head(to_state->output)!=NULL)
		{
			slist_node_t *ptr=slist_head(to_state->output);
			for(j=0;j<slist_size(to_state->output); j++)
			{
				
				pm_match_t* newMatch=(pm_match_t*)malloc(sizeof(pm_match_t));//create struct of match
				if(newMatch==NULL)
					return NULL;//create node was unsuccessful
				newMatch->pattern=slist_data(ptr);
				newMatch->start_pos=i-strlen(newMatch->pattern)+1;
				newMatch->end_pos=i;
				newMatch->fstate=to_state;
				slist_append(matchList,newMatch);//connect the struct to my list
				printf("pattern: %s ,start at: %d ,ends at: %d ,last state: %d\n",newMatch->pattern,newMatch->start_pos,newMatch->end_pos,to_state->id);
				ptr=slist_next(ptr);
			}
		}

		state=to_state;
	}
	return matchList;
}


void destroy(pm_state_t* state)
{	
	int i;
	slist_node_t* node=slist_head(state->_transitions);
	
	for(i=0; i<slist_size(state->_transitions);i++)
	{	

		destroy(((pm_labeled_edge_t*)slist_data(node))->state);
		node=slist_next(node);
	}
		slist_destroy(state->output,SLIST_LEAVE_DATA);
		free(state->output);
		slist_destroy(state->_transitions,SLIST_FREE_DATA);
		free(state->_transitions);
		
		free(state);//delete struct
}

void pm_destroy(pm_t *root)//destroy DFA
{
	if(root==NULL)
		return;
	destroy(root->zerostate);//call in recursion

}




