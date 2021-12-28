

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "IO.h"
#include "common_structures.h"
#include "core.h"
#include "main_memory.h"


void close_all_open_file_pointers_in_array(int size_of_array, FILE** fp_array) {
	for (int i = 0; i < size_of_array; i++) {
		fclose(fp_array[i]);
	}
}
FILE** initialize_file_pointer_array(int size_of_array) {
	FILE** fp_array = NULL;
	if (NULL == (fp_array = (FILE**)calloc(size_of_array, sizeof(FILE*)))) {
		printf("Couldnt allocate space for file pointer array!\n");
		return NULL;
	}
	for (int i = 0; i < 4; i++) {
		if (NULL == ((fp_array[i] = (FILE*)malloc(sizeof(FILE))))) {
			printf("Couldnt allocate space for file pointer array!\n");
			return NULL;
		}
	}
	return fp_array;
}
void get_data_from_input_files(int argc, char** argv, FILE** fp_array) {
	if (0 == argc) {
		//TODO: Support only one argument!!!
		printf("No files have been read\n");
		return;
	}
	for (int i = 0; i < 4; i++) {
		fopen_s(&fp_array[i], argv[i + 1], "r");
	}
}


void write_data_to_file(FILE* dst_fp, FILE* src_fp, const char* file_name) {
	char* read_write_buffer = { 0 };
	if (NULL == (read_write_buffer = (char*)malloc(10 * sizeof(char)))) {
		printf("read_write buffer allocation failed\n");
		return;
	}
	fopen_s(&dst_fp, file_name, "w");
	while (!feof(src_fp)) {
		if (NULL != (read_write_buffer) && (dst_fp)) {
			fgets(read_write_buffer, 10, src_fp);
			fprintf(dst_fp, read_write_buffer);
		}
	}


}


////////////////////////////////////////////////////////////////////////////////////////////////////////



void initialize_trace_files(int argc, char** argv)
{
	FILE* fp, * fp2;
	if (argc == NUM_OF_INPUT_FILES) {
		fopen_s(&fp, argv[BUS_TRACE], "w");
		fopen_s(&fp2, argv[MEMOUT], "w");
	}
	else {
		fopen_s(&fp, "bustrace.txt", "w");
		fopen_s(&fp2, "memout.txt", "w");
	}
	if (fp)
		fclose(fp);
	if (fp2)
		fclose(fp2);
	for (int i = 0; i < NUMBER_OF_CORES; i++)
	{
		if (argc == NUM_OF_INPUT_FILES)
			fopen_s(&fp, argv[CORE0TRACE + i], "w");
		else
		{
			char* trace_files[NUMBER_OF_CORES] = TRACE_FILES_NAMES;
			fopen_s(&fp, trace_files[i], "w");
		}
		if (fp)
			fclose(fp);
	}
}

void get_IMEM_for_core(unsigned int core_number, unsigned int* IMEM, int argc, char** argv)
{
	FILE* imem_fp;
	if (argc == NUM_OF_INPUT_FILES)
		fopen_s(&imem_fp, argv[IMEM0 + core_number], "r");
	else
	{
		char* imem_files[NUMBER_OF_CORES] = IMEM_FILES_NAMES;
		fopen_s(&imem_fp, imem_files[core_number], "r");
	}

	if (imem_fp == NULL)
	{
		printf("Couldn't open IMEM file for core %d\n", core_number);
		exit((int)IO_IMEM);
	}
	char new_line[LENGTH_OF_AN_INSTRUCTION] = { '\0' };
	int imem_index = 0;
	while (fgets(new_line, LENGTH_OF_AN_INSTRUCTION, imem_fp))
	{
		IMEM[imem_index] = (unsigned int)strtoll(new_line, NULL, 16);
		imem_index++;
	}
	for (imem_index; imem_index < MAX_NUMBER_OF_INSTRUCTIONS; imem_index++)
		IMEM[imem_index] = 0;
	fclose(imem_fp);
}

unsigned int read_a_line_from_main_memory(long address, int argc, char** argv, bool is_memin)
{
	FILE* memin_fp;
	if (is_memin == true) {
		if (argc == NUM_OF_INPUT_FILES)
			fopen_s(&memin_fp, argv[MEMIN], "r");
		else
			fopen_s(&memin_fp, "memin.txt", "r");
		if (memin_fp == NULL)
		{
			printf("Couldn't open MEM_IN.txt\n");
			exit((int)IO_memin);
		}
	}
	else {
		if (argc == NUM_OF_INPUT_FILES)
			fopen_s(&memin_fp, argv[MEMOUT], "r");
		else
			fopen_s(&memin_fp, "memout.txt", "r");
		if (memin_fp == NULL)
		{
			printf("Couldn't open MEM_OUT.txt\n");
			exit((int)IO_memout);
		}
	}

	fseek(memin_fp, address * LENGTH_OF_AN_INSTRUCTION, SEEK_SET);

	char new_line[LENGTH_OF_AN_INSTRUCTION];
	unsigned int data;
	if (fgets(new_line, LENGTH_OF_AN_INSTRUCTION, memin_fp))
		data = (unsigned int)strtoll(new_line, NULL, 16);
	else
		data = 0;

	fclose(memin_fp);
	return data;
}

void print_a_line_to_the_main_memory(long address, unsigned int data, int argc, char** argv)
{
	FILE* memout_fp;
	if (argc == NUM_OF_INPUT_FILES)
		fopen_s(&memout_fp, argv[MEMOUT], "r+");
	else
		fopen_s(&memout_fp, "memout.txt", "r+");
	if (memout_fp == NULL)
	{
		printf("Couldn't open memout.txt\n");
		exit((int)IO_memout);
	}

	fseek(memout_fp, address * LENGTH_OF_AN_INSTRUCTION, SEEK_SET);
	fprintf(memout_fp, "%08X\n", data);
	fclose(memout_fp);
}


int number_of_lines_in_MEMIN(int argc, char** argv) {
	FILE* memin_fp;
	if (argc == NUM_OF_INPUT_FILES)
		fopen_s(&memin_fp, argv[MEMIN], "r");
	else
		fopen_s(&memin_fp, "memin.txt", "r");
	if (memin_fp == NULL)
	{
		printf("Couldn't open memin.txt\n");
		exit((int)IO_memin);
	}
	int counter = 0;
	char Line[READ_LENGTH] = { 0 };
	while (fgets(Line, READ_LENGTH, memin_fp) != NULL) {
		counter++;
	}
	fclose(memin_fp);
	return counter - 1;
}

void print_REGOUT_for_single_core(int core_number, int* core_registers, int argc, char** argv)
{
	FILE* regout_fp;
	if (argc == NUM_OF_INPUT_FILES)
		fopen_s(&regout_fp, argv[REGOUT0 + core_number], "w");
	else
	{
		char* regout_files[NUMBER_OF_CORES] = REGOUT_FILES_NAMES;
		fopen_s(&regout_fp, regout_files[core_number], "w");
	}
	if (regout_fp == NULL)
	{
		printf("Couldn't open REGOUT%d file\n", core_number);
		exit((int)IO_regout);
	}
	for (int i = 2; i < NUMBER_OF_REGISTERS_IN_CORE; i++)
		fprintf(regout_fp, "%08X\n", core_registers[i]);
	fclose(regout_fp);
}

void print_trace_of_a_single_core(core_pipeline_registers* pipeline_regs, int clk,
	int* core_registers, int argc, char** argv)
{
	FILE* trace_fp;
	if (argc == NUM_OF_INPUT_FILES)
		fopen_s(&trace_fp, argv[CORE0TRACE + pipeline_regs->number_of_the_core], "a");
	else
	{
		char* trace_files[NUMBER_OF_CORES] = TRACE_FILES_NAMES;
		fopen_s(&trace_fp, trace_files[pipeline_regs->number_of_the_core], "a");
	}
	if (trace_fp == NULL)
	{
		printf("Couldn't open core%dtrace file\n", pipeline_regs->number_of_the_core);
		exit((int)IO_ttrace);
	}
	fprintf(trace_fp, "%d", clk);
	if (pipeline_regs->IF_ID_regs.fetch_enable_flag)
		fprintf(trace_fp, " %03X", pipeline_regs->IF_ID_regs.next_pc);
	else
		fprintf(trace_fp, " ---");
	if (pipeline_regs->IF_ID_regs.IR == BUBBLE_INSTRUCTION)
		fprintf(trace_fp, " ---");
	else
		fprintf(trace_fp, " %03X", pipeline_regs->IF_ID_regs.current_pc);
	if (pipeline_regs->ID_EX_regs.op_code == BUBBLE)
		fprintf(trace_fp, " ---");
	else
		fprintf(trace_fp, " %03X", pipeline_regs->ID_EX_regs.current_pc);
	if (pipeline_regs->EX_MEM_regs.op_code == BUBBLE)
		fprintf(trace_fp, " ---");
	else
		fprintf(trace_fp, " %03X", pipeline_regs->EX_MEM_regs.current_pc);
	if (pipeline_regs->MEM_WB_regs.op_code == BUBBLE)
		fprintf(trace_fp, " --- ");
	else
		fprintf(trace_fp, " %03X ", pipeline_regs->MEM_WB_regs.current_pc);
	for (int i = 2; i < NUMBER_OF_REGISTERS_IN_CORE; i++)
		fprintf(trace_fp, "%08X ", core_registers[i]);
	fprintf(trace_fp, "\n");
	fclose(trace_fp);
}

void print_trace_of_bus(int clk, MESI_Bus MESI_Bus, int argc, char** argv)
{
	if (MESI_Bus.bus_cmd == 0)
		return;
	FILE* bustrace_fp;
	if (argc == NUM_OF_INPUT_FILES)
		fopen_s(&bustrace_fp, argv[BUS_TRACE], "a");
	else
		fopen_s(&bustrace_fp, "bustrace.txt", "a");
	if (bustrace_fp == NULL)
	{
		printf("Couldn't open bustrace file\n");
		exit((int)IO_bustrace);
	}
	fprintf(bustrace_fp, "%d %01X %01X %05X %08X\n", clk, MESI_Bus.bus_origid, MESI_Bus.bus_cmd, MESI_Bus.bus_address, MESI_Bus.bus_data);
	fclose(bustrace_fp);
}

/*void print_cache_memory_for_a_single_core(int core_num, CacheMemory cache_memory, int argc, char** argv)
{
	FILE* dsram_fp, * tsram_fp;
	char* dsram_files[NUM_OF_CORES] = DSRAM_FILES;
	char* tsram_files[NUM_OF_CORES] = TSRAM_FILES;
	if (argc == NUM_OF_INPUT_FILES)
	{
		fopen_s(&dsram_fp, argv[DSARM0 + core_num], "w");
		fopen_s(&tsram_fp, argv[TSRAM0 + core_num], "w");
	}
	else
	{
		fopen_s(&dsram_fp, dsram_files[core_num], "w");
		fopen_s(&tsram_fp, tsram_files[core_num], "w");
	}
	if (dsram_fp == NULL || tsram_fp == NULL)
	{
		printf("Couldn't open dsram/tsram files\n");
		exit(3);
	}
	int tsram_line;
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		fprintf(dsram_fp, "%08X\n", cache_memory.DSRam[i]);
		tsram_line = (cache_memory.TSRam[i].MsiState << 12) + cache_memory.TSRam[i].Tag;
		fprintf(tsram_fp, "%08X\n", tsram_line);
	}
	fclose(dsram_fp);
	fclose(tsram_fp);
}*/

/*void PrintStatsForCore(int core_num, Stats stats, int argc, char** argv)
{
	FILE* stats_fp;
	if (argc == NUM_OF_INPUT_FILES)
		fopen_s(&stats_fp, argv[STATS0 + core_num], "w");
	else
	{
		char* stats_files[NUM_OF_CORES] = STATS_FILES;
		fopen_s(&stats_fp, stats_files[core_num], "w");
	}
	if (stats_fp == NULL)
	{
		printf("Couldn't open stats%d file\n", core_num);
		exit(3);
	}
	fprintf(stats_fp, "cycles %d\n", stats.cycles);
	fprintf(stats_fp, "instructions %d\n", stats.instructions);
	fprintf(stats_fp, "read_hit %d\n", stats.read_hit);
	fprintf(stats_fp, "write_hit %d\n", stats.write_hit);
	fprintf(stats_fp, "read_miss %d\n", stats.read_miss);
	fprintf(stats_fp, "write_miss %d\n", stats.write_miss);
	fprintf(stats_fp, "decode stall %d\n", stats.decode_stall);
	fprintf(stats_fp, "mem_stall %d\n", stats.mem_stall);
	fclose(stats_fp);
}*/