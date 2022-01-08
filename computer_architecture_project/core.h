#ifndef  CORE_H
#define  CORE_H

#include <stdbool.h>
#include "common_structures.h"
#include "cache_memory.h"
#include "pipeline.h"
#define MAX_PC 1024
#define NUMBER_OF_REGISTERS_IN_CORE 15

#define OP_CODE_MASK 0xFF000000
#define OP_CODE_INDEX 24
#define RD_CODE_MASK 0x00F00000
#define RD_CODE_INDEX 20
#define RS_CODE_MASK 0x000F0000
#define RS_CODE_INDEX 16
#define RT_CODE_MASK 0x0000F000
#define RT_CODE_INDEX 12
#define FINAL_12_BITS 0x00000FFF
#define SIGN_BITS_MASK 2048
#define NEGATIVE_BITS_EXPENSION 0xFFFFF000
#define JAL_REGISTER 15

#define BUBBLE_INSTRUCTION 0x15000000



typedef struct unit_to_be_watched {
	bool is_watched;
	int addr_to_watch;
}unit_to_be_watched;

typedef struct core_info_struct {
	unsigned int IMEM[MAX_PC];
	int core_regs[NUMBER_OF_REGISTERS_IN_CORE];
	cache_data cache_data;
	unit_to_be_watched watch_flag;
	core_pipeline_registers pipeline_regs_old;
	core_pipeline_registers pipeline_regs_new;
	core_statistics core_statistic;
}core_info_struct;

typedef struct global_memory {
	main_memory changed_main_memory;
	MESI_Bus old_MESI_bus;
	MESI_Bus new_MESI_bus;
}global_memory;

void check_for_watch_flags_update(core_info_struct* cores_data, MESI_Bus old_MESI_bus);
bool run_one_core_cycle(core_info_struct* core_data, global_memory* global_memory, int number_of_core, unsigned int clk, int argc, char** argv);
bool need_to_check_for_watch_update(core_info_struct* cores_data, MESI_Bus old_MESI_bus);
void initialize_core_data(core_info_struct* core_data, int core_num, int argc, char** argv);
void Init_all_cores_data(core_info_struct cores_data[], int argc, char** argv);
#endif // ! CORE_H
