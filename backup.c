void find_solution( state_t* init_state ){

	HashTable table;

	// Choose initial capacity of PRIME NUMBER 
	// Specify the size of the keys and values you want to store once 
	ht_setup(&table, sizeof(int8_t)*SIZE*SIZE, sizeof(int8_t)*SIZE*SIZE, 16769023);

	// Initialize Stack
	initialize_stack();

	//Add the initial node
	node_t* n = create_init_node(init_state);
	node_t* new_node = (node_t *) malloc(sizeof(node_t));
	//FILL IN THE GRAPH ALGORITHM

	FILE *write = fopen("TESTOUT.csv","w");

	int i, j;
	move_t move;

	stack_push(n);
	int remaingPegs = num_pegs(&(n->state));
	while (is_stack_empty() == 0) {
		n = stack_top();
		stack_pop();
		expanded_nodes++;
		if (num_pegs(&(n->state)) < remaingPegs) {
			save_solution(n);
			remaingPegs = num_pegs(&(n->state));
			fprintf(write,"TEST2\n");
		}
		for (i = 0; i < SIZE; i++) {
			for (j = 0; j < SIZE; j++) {
				n->state.cursor.x = i;
				n->state.cursor.y = j;
				if (select_peg(&(n->state)) == true) {
					for (move = left; move <= down; move++) {
						if (can_apply(&(n->state), &(n->state.cursor), move) == true) {
							new_node = applyAction(n, &(n->state.cursor), move);
							generated_nodes++;
							if (won(&(new_node->state)) == 1) {
								fprintf(write,"TEST8\n");
								save_solution(new_node);
								remaingPegs = num_pegs(&(new_node->state));
								free(n);
								free(new_node);
								return;
							}
		
							if (ht_contains(&table, &(new_node->state)) == HT_NOT_FOUND) {
								ht_insert(&table, &(new_node->state), &generated_nodes);
								stack_push(new_node);
							}
						}
					}
				}
			}
		}
		
		if (expanded_nodes >= budget) {
			free(n);
			free(new_node);
			return;
		}
	}
}