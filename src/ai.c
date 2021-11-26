/***************************************************************************
 *
 *   File        : ai.c
 *   Student Id  : 1001969
 *   Name        : Goh Xing Yang
 *
 ***************************************************************************/

#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <assert.h>

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
	node_t* new_n = (node_t *) malloc(sizeof(node_t));
	new_n->parent = NULL;	
	new_n->depth = 0;
	copy_state(&(new_n->state), init_state);
	return new_n;
}

/**
 * Apply an action to node n and return a new node resulting from executing the action
*/
node_t* applyAction(node_t* n, position_s* selected_peg, move_t action){

	// Create a new_node and a parent_node
    node_t* new_node = (node_t *)malloc(sizeof(node_t));
    assert(new_node != NULL);
    node_t* parent_node = (node_t *)malloc(sizeof(node_t));
    assert(parent_node);

    // Copy states from the original node n
    copy_state(&(parent_node->state), &(n->state));
    copy_state(&(new_node->state), &(n->state));

    // Initialise parent node information
    parent_node->parent = n->parent;
    parent_node->depth = n->depth;
    parent_node->move = n->move;
   	
   	// Link new node to parent node and initialise node information
    new_node->parent = parent_node;
    new_node->depth = parent_node->depth+1;
    new_node->move = action;

    // Initialise new_node cursor 
    new_node->state.cursor = *selected_peg;

    // Update new_node cursor after applying an action
    execute_move_t(&(new_node->state), selected_peg, action);
	
	return new_node;
}

/**
 * Find a solution path as per algorithm description in the handout
 */
void find_solution( state_t* init_state ) {

	// Initialise hash table and buffer to defined INITIALBUFFER
	HashTable table;
	int buffer = INITIALBUFFER;

	// Choose initial capacity of PRIME NUMBER 
	// Specify the size of the keys and values you want to store once 
	ht_setup(&table, sizeof(int8_t)*SIZE*SIZE, sizeof(int8_t)*SIZE*SIZE, 16769023);

	// Initialize Stack
	initialize_stack();

	//Add the initial node
	node_t* n = create_init_node(init_state);
	node_t* new_node;

	// Create a dynamic array to store all generated nodes
	node_t** all_nodes = (node_t **)malloc(sizeof(node_t *)*buffer);
	assert(all_nodes != NULL);
	all_nodes[generated_nodes] = n;
	position_s currentCursor;

	// Push initial node to stack and initialise the number of remaining pegs
	int i, j, k;
	move_t move;
	stack_push(n);
	int remaingPegs = num_pegs(&(n->state));

	// If stack is non-empty, pop the top node of the stack and continue DFS
	while (is_stack_empty() == 0) {
		n = stack_top();
		stack_pop();
		expanded_nodes++;

		// Found a better solution, update best solution matrix and update best number
		// of remaining pegs
		if (num_pegs(&(n->state)) < remaingPegs) {
			save_solution(n);
			remaingPegs = num_pegs(&(n->state));
		}

		// All possible jumps for current node state are considered
		for (i = 0; i < SIZE; i++) {
			for (j = 0; j < SIZE; j++) {
				for (move = left; move <= down; move++) {
					currentCursor.x = i;
					currentCursor.y = j;

					// A jump is possible for the cursor at position (i,j) in node n state
					if (can_apply(&(n->state), &(currentCursor), move) == true) {

						// Generate a new node where the jump is executed and store this state
						new_node = applyAction(n, &(currentCursor), move);
						generated_nodes++;

						// Buffer for all_nodes has been reached, double buffer size and 
						// reallocate memory to store the pointer of the newly generated node
						if (generated_nodes >= buffer) {
							buffer*=2;
							all_nodes = (node_t **)realloc(all_nodes, sizeof(node_t *)*buffer);
						}
						all_nodes[generated_nodes] = new_node;

						// First solution has been found as there is only 1 peg remaining
						if (won(&(new_node->state)) == 1) {

							// Update best solution matrix and best number of remaining pegs
							save_solution(new_node);
							remaingPegs = num_pegs(&(new_node->state));

							// Free all the nodes and hash table
							for (k = 0; k<=generated_nodes; k++) {

								// Since a parent node is generated for each new node, free
								// the generated parent nodes as well
								if (all_nodes[k]->parent != NULL) {
									free(all_nodes[k]->parent);
									free(all_nodes[k]);
								}
								else {
									free(all_nodes[k]);
								}
							}		
							free(all_nodes);
							ht_destroy(&table);
							return;
						}
						
						// Check if the new node board state reached has been reached previously in
						// the hash table, add the new node state to the hash table if it is unique
						if (ht_contains(&table, &(new_node->state)) == HT_NOT_FOUND) {
							ht_insert(&table, &(new_node->state), &generated_nodes);

							// Also push unique board state into the stack
							stack_push(new_node);
						}
					}
				}
			}
		}
		
		// Winning solution not found within designated budget, end DFS with best solution so far
		if (expanded_nodes >= budget) {

			// Free all the nodes and hash table
			for (k = 0; k<=generated_nodes; k++) {
				if (all_nodes[k]->parent != NULL) {

					// Since a parent node is generated for each new node, free the generated
					// parent nodes as well
					free(all_nodes[k]->parent);
					free(all_nodes[k]);
				}
				else {
					free(all_nodes[k]);
				}
			}		
			free(all_nodes);
			ht_destroy(&table);
			return;
		}
	}
}