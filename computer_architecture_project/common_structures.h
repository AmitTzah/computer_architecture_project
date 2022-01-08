#ifndef COMMON_STRUCTURES
#define COMMON_STRUCTURES
#define NUMBER_OF_REGISTERS_IN_CORE 15

enum  return_exit_errors {
	process_complete, IO_IMEM, IO_dtrace, IO_ttrace,
	IO_memin, IO_memout, IO_regout, IO_bustrace
} return_exit_errors;


enum registers_of_cpu
{
	ZERO = 0, IMM, R1, R2, R3, R4, R5, R6
	, R7, R8, R9, R10, R11, R12, R13, R14, R15
}typedef registers_of_cpu;

typedef enum opcode {
	ADD = 0, SUB, AND, OR, XOR, MUL, SLL, SRA,
	SRL, BEQ, BNE, BLT, BGT, BLE, BGE, JAL, LW,
	SW, HALT = 20, BUBBLE
}opcode;
typedef struct core_statistics {

	int number_of_cycles;
	int number_of_instructions;
	int number_of_read_hits;
	int number_of_write_hit;
	int number_of_read_miss;
	int number_of_write_miss;
	int number_of_decode_stalls;
	int number_of_mememory_stalls;
}core_statistics;

#endif // !COMMON_STRUCTURES
