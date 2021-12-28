#ifndef IO_H
#define IO_H

#define IMEM_FILES_NAMES {"imem0.txt", "imem1.txt", "imem2.txt", "imem3.txt"}
#define REGOUT_FILES_NAMES {"regout0.txt", "regout1.txt", "regout2.txt", "regout3.txt"}
#define TRACE_FILES_NAMES {"core0trace.txt", "core1trace.txt", "core2trace.txt", "core3trace.txt"}
#define DSRAM_FILES_NAMES {"dsram0.txt", "dsram1.txt", "dsram2.txt", "dsram3.txt"}
#define TSRAM_FILES_NAMES {"tsram0.txt", "tsram1.txt", "tsram2.txt", "tsram3.txt"}
#define STATS_FILES_NAMES {"stats0.txt", "stats1.txt", "stats2.txt", "stats3.txt"}
#define NUM_OF_INPUT_FILES 28
#define NUMBER_OF_CORES 4
#define LENGTH_OF_AN_INSTRUCTION 10
#define MAX_NUMBER_OF_INSTRUCTIONS 1024

#define READ_LENGTH 10
typedef enum position_in_argv {
	IMEM0 = 1,
	MEMIN = 5,
	MEMOUT = 6,
	REGOUT0 = 7,
	CORE0TRACE = 11,
	BUS_TRACE = 15,
	DSARM0 = 16,
	TSRAM0 = 20,
	STATS0 = 24
}position_in_argv;

void close_all_open_file_pointers_in_array(int size_of_array, FILE** fp_array);
FILE** initialize_file_pointer_array(int size_of_array);
void get_data_from_input_files(int argc, char** argv, FILE** fp_array);
void write_data_to_file(FILE* dst_fp, FILE* src_fp, const char* file_name);





#endif // !IO_H
