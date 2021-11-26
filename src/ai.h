#ifndef __AI__
#define __AI__

#define INITIALBUFFER 1000

#include <stdint.h>
#include <unistd.h>
#include "utils.h"


void initialize_ai();

void find_solution( state_t* init_state );
void free_memory(unsigned expanded_nodes);

node_t* applyAction(node_t* n, position_s* selected_peg, move_t action);
node_t* create_init_node( state_t* init_state );
void save_solution( node_t* solution_node );
void copy_state(state_t* dst, state_t* src);



#endif
