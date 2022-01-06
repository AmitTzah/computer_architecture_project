#ifndef CASHE_MEMORY_H
#define CASHE_MEMORY_H

#include "main_memory.h"
#define CACHE_SIZE 256
#define NUM_OF_PROCESSORS 4
#define INITIALIZE_VALUE -1
typedef struct TS_ram {
	int tag;
	MESI_State MESI_state;
}TS_ram;

typedef struct cache_mem {
	int   DS_Ram[CACHE_SIZE];
	TS_ram TS_Ram[CACHE_SIZE];
}cache_mem;

typedef enum processor_to_order {
	NoOrder,
	Read,
	Write
}processor_to_order;

typedef struct processor_to_memory {
	processor_to_order order;
	int addr;
	int data;
}processor_to_memory;

typedef struct cache_data {
	cache_mem cache_mem;
	processor_to_memory processor_to_memory;
	int returned_data;
	bool is_data_returned;
	bool waiting_for_read_data_flag;
	bool waiting_for_write_data_flag;
}cache_data;

void ask_from_memory(processor_to_order order, int addr, int data, cache_data* cache_data);
MESI_State get_MESI_state(int cache_index, int cache_tag, cache_data* cache_data);
void initialize_cache(cache_data* cache_data);
void send_modified_data_to_bus(cache_data* cache_data, int number_of_core, MESI_Bus* new_MESI_bus, int cache_index, bool IsBusConfiged);
void excute_bus_order_for_cache_memory(cache_data* cache_data, int number_of_core, MESI_Bus* old_MESI_bus, MESI_Bus* new_MESI_bus);
void excute_bus_order(cache_data* cache_data, int number_of_core, MESI_Bus* old_MESI_bus, MESI_Bus* new_MESI_bus,
	main_memory* changed_main_memory, int argc, char** argv);
void execute_core_orders(cache_data* cache_data, int number_of_core, MESI_Bus* old_MESI_bus,
	MESI_Bus* new_MESI_bus, main_memory* changed_main_memory);
bool read_from_cache(int addr, int data_in, int* data_out, int number_of_coreber, cache_data* cache_data,
	MESI_Bus* old_MESI_bus, MESI_Bus* new_MESI_bus, main_memory* changed_main_memory);
bool write_to_cache(int addr, int data_in, int number_of_coreber, cache_data* cache_data,
	MESI_Bus* old_MESI_bus, MESI_Bus* new_MESI_bus, main_memory* changed_main_memory);
#endif // !CASHE_MEMORY_H
