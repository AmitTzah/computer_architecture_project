#ifndef PIPELINE_H
#define PIPELINE_H
#include "common_structures.h"
#include <stdbool.h>
#include "core.h"
#define BUBBLE_INSTRUCTION 0x15000000






typedef struct decoded_instruction {
	opcode op_code;
	int rd, rt, rs, immediate;
}decoded_instruction;


typedef struct IF_ID_registers {

	unsigned int current_pc;
	unsigned int next_pc;
	bool fetch_enable_flag;
	unsigned int IR;

}IF_ID_registers;

typedef struct ID_EX_registers {

	int rd, rd_data, rt, rt_data, rs, rs_data, dst;
	opcode op_code;
	unsigned current_pc;
	bool decode_stall_flag; //if TRUE -> the Decode is stalled

}ID_EX_registers;

typedef struct EX_MEM_registers {

	opcode op_code;
	unsigned int current_pc;
	int data_in, ALU_result, rt, dst;

}EX_MEM_registers;

typedef struct MEM_WB_registers {

	unsigned int current_pc;
	int ALU_result, MEM_result, dst;
	bool MEM_stall_flag;//if TRUE -> the MEM is stalled
	opcode op_code;

}MEM_WB_registers;

typedef struct core_pipeline_registers {

	int number_of_the_core;
	IF_ID_registers IF_ID_regs;
	ID_EX_registers ID_EX_regs;
	EX_MEM_registers EX_MEM_regs;
	MEM_WB_registers MEM_WB_regs;

}core_pipeline_registers;

void update_pipeline_registers(core_pipeline_registers* pipeline_regs_old, core_pipeline_registers* pipeline_regs_new);
void initialize_all_core_pipeline_registers(core_pipeline_registers* pipeline_regs_old, core_pipeline_registers* pipeline_regs_new, int number_of_the_core);
#endif // !PIPELINE_H