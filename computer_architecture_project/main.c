#include <stdio.h>
#include <stdlib.h>
#include "IO.h"
#include "core.h"




void run_test(global_memory* Global_memory, core_info_struct* cores_data, int argc, char** argv)
{
	bool Proccesor_is_active = true, Cores_active[NUM_OF_PROCESSORS] = { true, true , true, true };
	unsigned int clk = 0;
	while (Proccesor_is_active)
	{
		Proccesor_is_active = false;
		update_bus(&Global_memory->old_MESI_bus, &Global_memory->new_MESI_bus);
		if (Global_memory->old_MESI_bus.bus_cmd != Invalid) {
			print_trace_of_bus(clk, Global_memory->old_MESI_bus, argc, argv);
		}
		for (int ProcessorNumber = 0; ProcessorNumber < NUM_OF_PROCESSORS; ProcessorNumber++)
		{
			excute_bus_order(&cores_data[ProcessorNumber].cache_data, ProcessorNumber, &Global_memory->old_MESI_bus,
				&Global_memory->new_MESI_bus, &Global_memory->changed_main_memory, argc, argv);
			if (Cores_active[ProcessorNumber]) {
				Cores_active[ProcessorNumber] = run_one_core_cycle(&cores_data[ProcessorNumber], Global_memory,
					ProcessorNumber, clk, argc, argv);
				Proccesor_is_active |= Cores_active[ProcessorNumber];
			}
			if (need_to_check_for_watch_update(cores_data, Global_memory->old_MESI_bus)) {
				check_for_watch_flags_update(cores_data, Global_memory->old_MESI_bus);
			}
		}
		if (!Proccesor_is_active) {
			for (int ProcessorNumber = 0; ProcessorNumber < NUM_OF_PROCESSORS; ProcessorNumber++) {
				print_cache_memory_for_a_single_core(ProcessorNumber, cores_data[ProcessorNumber].cache_data.cache_mem, argc, argv);
			}
		}
		clk++;
	}
}
void initialize_the_entire_process(int argc, char** argv, core_info_struct* cores_data,
	global_memory global_memory)
{
	initialize_trace_files(argc, argv);
	Init_all_cores_data(cores_data, argc, argv);
	initialize_memory(&global_memory.old_MESI_bus, &global_memory.new_MESI_bus, &global_memory.changed_main_memory);
	initialize_memout(argc, argv, &global_memory.changed_main_memory);
}
void main(int argc, char** argv) {
	/*FILE** fp_array_imem_read = initialize_file_pointer_array(4);
	FILE** fp_array_imem_write = initialize_file_pointer_array(4);
	get_data_from_input_files(argc, argv, fp_array_imem_read);
	write_data_to_file(fp_array_imem_write[0], fp_array_imem_read[0], "testing");*/

	global_memory global_memory;
	core_info_struct* cores_data = calloc(NUMBER_OF_CORES, sizeof(core_info_struct));
	if (cores_data == NULL) {
		printf("Not enough space to sim to run\n");
		return;
	};
	initialize_trace_files(argc, argv);
	Init_all_cores_data(cores_data, argc, argv);
	initialize_memory(&global_memory.old_MESI_bus, &global_memory.new_MESI_bus, &global_memory.changed_main_memory);
	initialize_memout(argc, argv, &global_memory.changed_main_memory);

	//initialize_the_entire_process(argc, argv, cores_data, global_memory);
	run_test(&global_memory, cores_data, argc, argv);
	free(cores_data);


}