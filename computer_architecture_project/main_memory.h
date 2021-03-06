

#ifndef MAIN_MEMMORY
#define MAIN_MEMMORY


#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "common_structures.h"
#define NOT_IN_CHANGED_MAIN_MEMORY -1
#define WAIT_TIME 63
#define MAIN_MEMORY_ORIGID 4
#define MAIN_MEMORY_SIZE 1<<20
#define ZERO_NUMBER 0
#define NOT_DEFINED -1

typedef enum bus_cmd {
	NoCommand,
	BusRd,
	BusRdX,
	Flush
}bus_cmd;

typedef enum MESI_State {
	Invalid,
	Shared,
	/*Exclusive,*/
	Modified
}MESI_State;

typedef struct MESI_Bus {
	int bus_origid;
	bus_cmd bus_cmd;
	int bus_address;
	int bus_data;
	bool bus_shared;
	bool is_the_bus_configed;
}MESI_Bus;

typedef struct main_memory {
	int size;
	int time_counter;
	int addr_to_send;
	int bus_read_origid;
}main_memory;

bool configure_bus(int bus_origid, bus_cmd bus_cmd, int bus_address, int bus_data, MESI_Bus* new_MESI_bus);
void execute_bus_orders_for_main_memory(int ProcessorNum, MESI_Bus* old_MESI_bus, MESI_Bus* new_MESI_bus,
	main_memory* changed_memory, int argc, char** argv);
void update_bus(MESI_Bus* old_MESI_bus, MESI_Bus* new_MESI_bus);
void initialize_main_memory(main_memory* changed_memory);
void initialize_memout(int argc, char** argv, main_memory* changed_memory);
void initialize_memory(MESI_Bus* old_MESI_bus, MESI_Bus* new_MESI_bus, main_memory* changed_memory);
#endif // !MAIN_MEMMORY
