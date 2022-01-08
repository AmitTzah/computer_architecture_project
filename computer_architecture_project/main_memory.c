
#include "main_memory.h"
#include "IO.h"

void clean_bus(MESI_Bus* mesi_bus) {
	mesi_bus->bus_cmd = NoCommand;
	mesi_bus->is_the_bus_configed = false;
	mesi_bus->bus_address = ZERO_NUMBER;
	mesi_bus->bus_data = ZERO_NUMBER;
	mesi_bus->bus_origid = ZERO_NUMBER;
	mesi_bus->bus_shared = false;
}


void add_data_to_memory(int Data, int Address, main_memory* changed_memory, int argc, char** argv) {
	if (Address == 534) {
		Address = 534;
	}
	if (Address > changed_memory->size) {
		for (int i = (changed_memory->size + 1); i < Address; i++) {
			print_a_line_to_the_main_memory(i, 0, argc, argv);
		}
		changed_memory->size = Address;
	}
	print_a_line_to_the_main_memory(Address, Data, argc, argv);

}

void initialize_memout(int argc, char** argv, main_memory* changed_memory) {
	int max_addr = number_of_lines_in_MEMIN(argc, argv);
	changed_memory->size = max_addr;
	for (int WriteAddress = 0; WriteAddress <= max_addr; WriteAddress++) {
		unsigned int DataFromMemin = read_a_line_from_main_memory(WriteAddress, argc, argv, true);
		print_a_line_to_the_main_memory(WriteAddress, DataFromMemin, argc, argv);
	}
}

void initialize_main_memory(main_memory* changed_memory) {
	changed_memory->addr_to_send = NOT_DEFINED;
	changed_memory->size = ZERO_NUMBER;
	changed_memory->time_counter = ZERO_NUMBER;
	changed_memory->bus_read_origid = NOT_DEFINED;
}

void initialize_memory(MESI_Bus* old_MESI_bus, MESI_Bus* new_MESI_bus, main_memory* changed_memory)
{
	initialize_main_memory(changed_memory);
	clean_bus(old_MESI_bus);
	clean_bus(new_MESI_bus);
}

void update_bus(MESI_Bus* old_MESI_bus, MESI_Bus* new_MESI_bus) {
	memcpy(old_MESI_bus, new_MESI_bus, sizeof(MESI_Bus));
	clean_bus(new_MESI_bus);
}

bool configure_bus(int bus_origid, bus_cmd bus_cmd, int bus_address, int bus_data, MESI_Bus* new_MESI_bus) {
	if (new_MESI_bus->is_the_bus_configed == true) {
		return false;
	}
	new_MESI_bus->bus_origid = bus_origid;
	new_MESI_bus->bus_cmd = bus_cmd;
	new_MESI_bus->bus_address = bus_address;
	new_MESI_bus->bus_data = bus_data;
	new_MESI_bus->is_the_bus_configed = true;
	return true;
}


void execute_bus_orders_for_main_memory(int ProcessorNum, MESI_Bus* old_MESI_bus, MESI_Bus* new_MESI_bus,
	main_memory* changed_memory, int argc, char** argv) {
	if (old_MESI_bus->bus_cmd == Flush && old_MESI_bus->bus_origid != MAIN_MEMORY_ORIGID) {
		changed_memory->time_counter = 0;
		add_data_to_memory(old_MESI_bus->bus_data, old_MESI_bus->bus_address, changed_memory, argc, argv);
	}
	else if (old_MESI_bus->bus_cmd == BusRd || old_MESI_bus->bus_cmd == BusRdX) {
		changed_memory->time_counter = WAIT_TIME;
		changed_memory->addr_to_send = old_MESI_bus->bus_address;
		changed_memory->bus_read_origid = old_MESI_bus->bus_origid;
	}
	else if (old_MESI_bus->bus_cmd == NoCommand && changed_memory->time_counter > ZERO_NUMBER
		&& ProcessorNum == changed_memory->bus_read_origid) {
		changed_memory->time_counter--;
		if (changed_memory->time_counter == 0) {
			int sent_data = read_a_line_from_main_memory(changed_memory->addr_to_send, argc, argv, false);
			bool is_confirmed = configure_bus(MAIN_MEMORY_ORIGID, Flush, changed_memory->addr_to_send, sent_data, new_MESI_bus);
		}
	}
}