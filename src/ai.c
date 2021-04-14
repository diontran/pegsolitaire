#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "ai.h"
#include "utils.h"
#include "hashtable.h"
#include "stack.h"


void copy_state(state_t* dst, state_t* src){
	
	//Copy field
	memcpy( dst->field, src->field, SIZE*SIZE*sizeof(int8_t) );

	dst->cursor = src->cursor;
	dst->selected = src->selected;
}

/**
 * Saves the path up to the node as the best solution found so far
*/
void save_solution( node_t* solution_node ){
	node_t* n = solution_node;
	while( n->parent != NULL ){
		copy_state( &(solution[n->depth]), &(n->state) );
		solution_moves[n->depth-1] = n->move;

		n = n->parent;
	}
	solution_size = solution_node->depth;
}


node_t* create_init_node( state_t* init_state ){
	node_t * new_n = (node_t *) malloc(sizeof(node_t));
	new_n->parent = NULL;	
	new_n->depth = 0;
	copy_state(&(new_n->state), init_state);
	return new_n;
}

/**
 * Apply an action to node n and return a new node resulting from executing the action
*/
node_t* applyAction(node_t* n, position_s* selected_peg, move_t action ){
    node_t* new_node = (node_t *) malloc(sizeof(node_t));

	//FILL IN MISSING CODE
	if (can_apply(&(n->state), selected_peg, action)){
        //Copy parent state to new node state
        copy_state(&(new_node->state), &(n->state));
        //Execute move on new state
        execute_move_t(&(new_node->state), selected_peg, action);
        //Update new node with new state
        new_node = create_init_node(&(new_node->state));
        //Link new node to its parent and increase depth and update action
        new_node->parent = n;
        new_node->depth = n->depth+1;
        new_node->move = action;
    }
    return new_node;
}

/**
 * Find a solution path as per algorithm description in the handout
 */

void find_solution(state_t* init_state){

	HashTable table;

	// Choose initial capacity of PRIME NUMBER 
    #define PRIME NUMBER 16769023
    
	// Specify the size of the keys and values you want to store once 
	ht_setup( &table, sizeof(int8_t) * SIZE * SIZE, sizeof(int8_t) * SIZE * SIZE, 16769023);

	// Initialize Stack
	initialize_stack();

	//Add the initial node
	node_t* n = create_init_node(init_state);
	stack_push(n);
    
    //FILL IN THE GRAPH ALGORITHM
    int remainingPegs = 0;
    int i, j, k;
    
    position_s *currentPair = (position_s *)malloc(sizeof(position_s));
    remainingPegs = num_pegs(init_state);
    //printf("TEST 0\n");
    
    while(!is_stack_empty(stack)){
        node_t *newNode = (node_t *)malloc(sizeof(node_t));
        
        n = stack_top(); //current node be top of stack
        stack_pop(); //pop top of stack 
        expanded_nodes++;
        
        if (num_pegs(&(n->state)) < remainingPegs){ //Found a better solution
            save_solution(n);
            remainingPegs = num_pegs(&(n->state));
        }
        
        //printf("TEST 1\n");
        for(i = 0; i < SIZE; i++){
            currentPair->x = i;
            for(j = 0; j < SIZE; j++){
                currentPair->y = j;
                //printf("%d and %d \n", currentPair->x, currentPair->y);
                for(k = 0; k < 4; k++){
                    
                    if(can_apply(&(n->state), currentPair, k)){
                        //printf("TEST 3\n");
                        //printf("POSITIVE MOVE IS %d\n", k);
                        
                        newNode = applyAction(n, currentPair, k);
                        
                        //printf("NEWNODE DEPTH IS %d\n", newNode->depth);
                        generated_nodes++;
                        
                        if(won(&(newNode->state))){
                            //printf("WINNER WINNER CHICKEN DINNER\n");
                            save_solution(newNode);
                            remainingPegs = num_pegs(&(newNode->state));
                            return;
                        }
                        
                        if(!ht_contains(&table, &(newNode->state))){ // Avoid Duplicates // Check if visited
                            //printf("STATE NOT IN HASH TABLE\n");
                            ht_insert(&table, &(newNode->state), &(newNode));
                            //printf("NEW STATE IN HASH TABLE\n");
                            stack_push(newNode); // DFS strategy
                        }
                    }
                }
                if (expanded_nodes >= budget){
                    return;
                }
            }
        }
    }
    print_stack();
}