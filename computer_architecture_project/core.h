#ifndef  CORE_H
#define  CORE_H
#include <stdbool.h>


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
#define SIGN_BITS_MASK 0x800
#define NEGATIVE_BITS_EXPENSION 0xFFFFF000
#define JAL_REGISTER 15

#define BUBBLE_INSTRUCTION 0x15000000




enum registers_of_cpu
{
	ZERO = 0, IMM, R1, R2, R3, R4, R5, R6
	, R7, R8, R9, R10, R11, R12, R13, R14, R15
}typedef registers_of_cpu;

typedef enum opcode {
	ADD = 0, SUB, AND, OR, XOR, MUL, SLL, SRA,
	SRL, BEQ, BNE, BLT, BGT, BLE, BGE, JAL, LW,
	SW, LL, SC, HALT, BUBBLE
}opcode;


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

typedef struct unit_to_be_watched {
	bool is_watched;
	int addr_to_watch;
}unit_to_be_watched;
typedef struct core_info_struct {
	unsigned int IMEM[MAX_PC];
	int core_regs[NUMBER_OF_REGISTERS_IN_CORE];
	//CacheData Cache;
	unit_to_be_watched watch_flag;
	core_pipeline_registers pipeline_regs_old;
	core_pipeline_registers pipeline_regs_new;
	//Stats core_statistic;
}core_info_struct;

#endif // ! CORE_H
