#include "core.h"
#include "core_instructions.h"
#include "cache_memory.h"
#include "main_memory.h"

void ask_from_memory(processor_to_order order, int addr, int data, cache_data* cache_data) {
	cache_data->processor_to_memory.order = order;
	cache_data->processor_to_memory.addr = addr;
	cache_data->processor_to_memory.data = data;
}


MESI_State get_MESI_state(int cache_index, int cache_tag, cache_data* cache_data) {
	if (cache_data->cache_mem.TS_Ram[cache_index].tag == cache_tag) {
		return cache_data->cache_mem.TS_Ram[cache_index].MESI_state;
	}
	else {
		return Invalid;
	}
}

void initialize_cache(cache_data* cache_data) {
	for (int i = 0; i < CACHE_SIZE; i++) {
		cache_data->cache_mem.TS_Ram[i].MESI_state = Invalid;
		cache_data->cache_mem.TS_Ram[i].tag = 0;
		cache_data->cache_mem.DS_Ram[i] = 0;
	}
	cache_data->is_data_returned = false;
	cache_data->waiting_for_read_data_flag = false;
	cache_data->waiting_for_write_data_flag = false;
	cache_data->processor_to_memory.order = NoCommand;
	cache_data->returned_data = INITIALIZE_VALUE;
}


void send_modified_data_to_bus(cache_data* cache_data, int number_of_core, MESI_Bus* new_MESI_bus, int cache_index, bool IsBusConfiged) {
	int Modifieddataaddr = cache_data->cache_mem.TS_Ram[cache_index].tag * CACHE_SIZE + cache_index;
	IsBusConfiged = configure_bus(number_of_core, Flush, Modifieddataaddr, cache_data->cache_mem.DS_Ram[cache_index], new_MESI_bus);
	if (IsBusConfiged == true) {
		cache_data->cache_mem.TS_Ram[cache_index].MESI_state = Invalid;
	}
}


void excute_bus_order_for_cache_memory(cache_data* cache_data, int number_of_core, MESI_Bus* old_MESI_bus, MESI_Bus* new_MESI_bus) {
	int bus_origid = old_MESI_bus->bus_origid, bus_addr = old_MESI_bus->bus_address, bus_data = old_MESI_bus->bus_data;
	bus_cmd bus_cmd = old_MESI_bus->bus_cmd;
	if (bus_cmd == NoCommand) {
		return;
	}
	int cache_index = bus_addr % CACHE_SIZE, CacheTag = bus_addr / CACHE_SIZE;
	MESI_State CurrentMsiState = get_MESI_state(cache_index, CacheTag, cache_data);
	if (bus_cmd == BusRd && number_of_core != old_MESI_bus->bus_origid) {
		if (CurrentMsiState == Modified) {
			cache_data->cache_mem.TS_Ram[cache_index].MESI_state = Shared;
			bool IsBusConfiged = configure_bus(number_of_core, Flush, bus_addr, cache_data->cache_mem.DS_Ram[cache_index], new_MESI_bus);
		}
	}
	else if (bus_cmd == BusRdX && number_of_core != old_MESI_bus->bus_origid) {
		if (CurrentMsiState != Invalid) {
			if (CurrentMsiState == Modified) {
				bool IsBusConfiged = configure_bus(number_of_core, Flush, bus_addr, cache_data->cache_mem.DS_Ram[cache_index], new_MESI_bus);
			}
			cache_data->cache_mem.TS_Ram[cache_index].MESI_state = Invalid;
		}
	}
	else if (bus_cmd == Flush) {
		if (cache_data->waiting_for_read_data_flag == true) {
			cache_data->cache_mem.DS_Ram[cache_index] = bus_data;
			cache_data->cache_mem.TS_Ram[cache_index].MESI_state = (CurrentMsiState == Modified) ? Modified : Shared;
			cache_data->cache_mem.TS_Ram[cache_index].tag = CacheTag;
			cache_data->waiting_for_read_data_flag = false;
			cache_data->is_data_returned = true;
			cache_data->returned_data = bus_data;
		}
		else if (cache_data->waiting_for_write_data_flag == true) {
			cache_data->cache_mem.DS_Ram[cache_index] = cache_data->processor_to_memory.data;
			cache_data->cache_mem.TS_Ram[cache_index].MESI_state = Modified;
			cache_data->cache_mem.TS_Ram[cache_index].tag = CacheTag;
			cache_data->waiting_for_write_data_flag = false;
			cache_data->is_data_returned = true;
		}
		else if (CurrentMsiState == Shared) {
			cache_data->cache_mem.DS_Ram[cache_index] = bus_data;
		}
	}
}

void excute_bus_order(cache_data* cache_data, int number_of_core, MESI_Bus* old_MESI_bus, MESI_Bus* new_MESI_bus,
	main_memory* changed_main_memory, int argc, char** argv) {
	execute_bus_orders_for_main_memory(number_of_core, old_MESI_bus, new_MESI_bus, changed_main_memory, argc, argv);
	excute_bus_order_for_cache_memory(cache_data, number_of_core, old_MESI_bus, new_MESI_bus);
}


void execute_core_orders(cache_data* cache_data, int number_of_core, MESI_Bus* old_MESI_bus,
	MESI_Bus* new_MESI_bus, main_memory* changed_main_memory) {
	processor_to_order Order;
	int addr, data;
	Order = cache_data->processor_to_memory.order;
	addr = cache_data->processor_to_memory.addr;
	data = cache_data->processor_to_memory.data;
	int cache_index = addr % CACHE_SIZE, CacheTag = addr / CACHE_SIZE;
	MESI_State CurrentMsiState = get_MESI_state(cache_index, CacheTag, cache_data);
	if (Order == Read) {
		if (CurrentMsiState != Invalid) {
			cache_data->returned_data = cache_data->cache_mem.DS_Ram[cache_index];
			cache_data->is_data_returned = true;
		}
		else {//Invalid
			cache_data->is_data_returned = false;
			if (cache_data->waiting_for_read_data_flag == true) {
				return;
			}
			bool IsBusConfiged = false;
			if (cache_data->cache_mem.TS_Ram[cache_index].MESI_state == Modified) {//previous data need to be written to main memory
				send_modified_data_to_bus(cache_data, number_of_core, new_MESI_bus, cache_index, IsBusConfiged);
			}
			else {
				if (changed_main_memory->time_counter == 0) {
					IsBusConfiged = configure_bus(number_of_core, BusRd, addr, 0, new_MESI_bus);
					if (IsBusConfiged == true) {
						cache_data->waiting_for_read_data_flag = true;
					}
				}
			}
		}
	}
	if (Order == Write) {
		if (CurrentMsiState == Modified) {
			cache_data->cache_mem.DS_Ram[cache_index] = data;
			cache_data->is_data_returned = true;
		}
		else {
			cache_data->is_data_returned = false;
			if (cache_data->waiting_for_write_data_flag == true) {
				return;
			}
			bool IsBusConfiged = false;
			if (cache_data->cache_mem.TS_Ram[cache_index].MESI_state == Modified) {//previous data need to be written to main memory
				send_modified_data_to_bus(cache_data, number_of_core, new_MESI_bus, cache_index, IsBusConfiged);
			}
			else {
				if (changed_main_memory->time_counter == 0) {
					IsBusConfiged = configure_bus(number_of_core, BusRdX, addr, 0, new_MESI_bus);
					if (IsBusConfiged == true) {
						cache_data->waiting_for_write_data_flag = true;
					}
				}
			}
		}
	}
}


bool read_from_cache(int addr, int data_in, int* data_out, int number_of_coreber, cache_data* cache_data,
	MESI_Bus* old_MESI_bus, MESI_Bus* new_MESI_bus, main_memory* changed_main_memory)
{
	ask_from_memory(Read, addr, data_in, cache_data);
	execute_core_orders(cache_data, number_of_coreber, old_MESI_bus, new_MESI_bus, changed_main_memory);
	*data_out = cache_data->returned_data;
	return cache_data->is_data_returned;
}


bool write_to_cache(int addr, int data_in, int number_of_coreber, cache_data* cache_data,
	MESI_Bus* old_MESI_bus, MESI_Bus* new_MESI_bus, main_memory* changed_main_memory)
{
	ask_from_memory(Write, addr, data_in, cache_data);
	execute_core_orders(cache_data, number_of_coreber, old_MESI_bus, new_MESI_bus, changed_main_memory);
	return cache_data->is_data_returned;
}
