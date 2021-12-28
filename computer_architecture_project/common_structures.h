#ifndef COMMON_STRUCTURES
#define COMMON_STRUCTURES
#define NUMBER_OF_REGISTERS_IN_CORE 15

enum  return_exit_errors {
	process_complete, IO_IMEM, IO_dtrace, IO_ttrace,
	IO_memin, IO_memout, IO_regout, IO_bustrace
} return_exit_errors;




#endif // !COMMON_STRUCTURES
