
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "core.h"
#include "core_instructions.h"
#include "cache_memory.h"
#include "common_structures.h"
#include "IO.h"


void initialize_core_statistics(core_statistics* core_statistic)
{
	core_statistic->number_of_cycles = 0;
	core_statistic->number_of_instructions = 0;
	core_statistic->number_of_read_hits = 0;
	core_statistic->number_of_write_hit = 0;
	core_statistic->number_of_read_miss = 0;
	core_statistic->number_of_write_miss = 0;
	core_statistic->number_of_decode_stalls = 0;
	core_statistic->number_of_mememory_stalls = 0;
}
void initialize_core_data(core_info_struct* core_data, int core_num, int argc, char** argv)
{
	for (int i = 0; i < NUMBER_OF_REGISTERS_IN_CORE; i++) {
		core_data->core_regs[i] = 0;
	}
	get_IMEM_for_core(core_num, core_data->IMEM, argc, argv);
	initialize_all_core_pipeline_registers(&core_data->pipeline_regs_old, &core_data->pipeline_regs_new, core_num);
	initialize_core_statistics(&core_data->core_statistic);
	initialize_cache(&core_data->cache_data);
}
void Init_all_cores_data(core_info_struct cores_data[], int argc, char** argv)
{
	for (int i = 0; i < NUMBER_OF_CORES; i++)
	{
		initialize_core_data(&cores_data[i], i, argc, argv);
	}
}

void decode_IR(unsigned int IR, decoded_instruction* instruction)
{
	instruction->op_code = (IR & OP_CODE_MASK) >> OP_CODE_INDEX;
	instruction->rd = (IR & RD_CODE_MASK) >> RD_CODE_INDEX;
	instruction->rs = (IR & RS_CODE_MASK) >> RS_CODE_INDEX;
	instruction->rt = (IR & RT_CODE_MASK) >> RT_CODE_INDEX;
	instruction->immediate = IR & FINAL_12_BITS;
}

void update_immidiate_register(int immediate, int* core_registers)
{
	core_registers[IMM] = immediate;
	if (SIGN_BITS_MASK & immediate) {
		core_registers[IMM] |= NEGATIVE_BITS_EXPENSION;
	}
}


bool are_registers_valid(decoded_instruction instruction)
{
	bool flag = true;
	if (instruction.rs >= NUMBER_OF_REGISTERS_IN_CORE || instruction.rs < 0)
		flag = false;
	if (instruction.rt >= NUMBER_OF_REGISTERS_IN_CORE || instruction.rt < 0)
		flag = false;
	if (instruction.rd >= NUMBER_OF_REGISTERS_IN_CORE || instruction.rd < 0)
		flag = false;
	return flag;
}

void fetch_regs_from_file_regsister(decoded_instruction instruction, ID_EX_registers* ID_EX_new, int* core_registers)
{
	if (are_registers_valid(instruction))
	{
		ID_EX_new->rs = instruction.rs;
		ID_EX_new->rs_data = core_registers[instruction.rs];
		ID_EX_new->rt = instruction.rt;
		ID_EX_new->rt_data = core_registers[instruction.rt];
		ID_EX_new->dst = instruction.rd;
		ID_EX_new->rd_data = core_registers[instruction.rd];
		ID_EX_new->op_code = instruction.op_code;
	}
	else {
		ID_EX_new->op_code = BUBBLE;	//TODO:Check again

	}
}

bool is_the_branch_taken(int rs_data, int rt_data, opcode branch_opcode)
{
	switch (branch_opcode)
	{
	case BEQ:
		return (rs_data == rt_data);
	case BNE:
		return (rs_data != rt_data);
	case BLT:
		return (rs_data < rt_data);
	case BGT:
		return (rs_data > rt_data);
	case BLE:
		return (rs_data <= rt_data);
	case BGE:
		return (rs_data >= rt_data);
	case JAL:
		return true;
	default:
		return false;
	}
}

void IF_stage(IF_ID_registers* p_IF_ID_old_regs
	, IF_ID_registers* p_IF_ID_new_regs, unsigned int* IMEM) {

	if (p_IF_ID_old_regs->fetch_enable_flag) {
		p_IF_ID_new_regs->IR = IMEM[p_IF_ID_old_regs->next_pc];
		p_IF_ID_new_regs->next_pc = (p_IF_ID_old_regs->next_pc + 1) % MAX_PC;
		p_IF_ID_new_regs->current_pc = p_IF_ID_old_regs->next_pc;
	}
	else {
		p_IF_ID_new_regs->IR = BUBBLE_INSTRUCTION;//TODO: insert bubble instruction 
	}

}

bool is_the_instruction_for_ALU(opcode opcode)
{
	if (opcode >= ADD && opcode <= SRL)
		return true;
	return false;
}


void set_watch_flag(core_info_struct* core_data, int addr_to_watch)
{
	core_data->watch_flag.is_watched = true;
	core_data->watch_flag.addr_to_watch = addr_to_watch;
}

bool is_register_in_stage(int reg, opcode opcode, int dst)
{
	if (is_the_instruction_for_ALU(opcode) || opcode == LW || opcode == JAL) {  //opcodes that changing rd
		return (reg == dst);
	}
	return false;
}
bool is_register_in_pipeline(int reg, ID_EX_registers* ID_EX_old,
	EX_MEM_registers* EX_MEM_old, MEM_WB_registers* MEM_WB_old)
{
	bool res = false;
	res |= is_register_in_stage(reg, ID_EX_old->op_code, ID_EX_old->dst);
	res |= is_register_in_stage(reg, EX_MEM_old->op_code, EX_MEM_old->dst);
	res |= is_register_in_stage(reg, MEM_WB_old->op_code, MEM_WB_old->dst);
	return res;
}
bool There_is_a_data_hazard(ID_EX_registers* ID_EX_new, ID_EX_registers* ID_EX_old,
	EX_MEM_registers* EX_MEM_old, MEM_WB_registers* MEM_WB_old)
{
	bool res = false;
	if (ID_EX_new->op_code == JAL) {
		res = is_register_in_pipeline(ID_EX_new->dst, ID_EX_old, EX_MEM_old, MEM_WB_old);
	}
	else {
		res |= is_register_in_pipeline(ID_EX_new->rs, ID_EX_old, EX_MEM_old, MEM_WB_old);
		res |= is_register_in_pipeline(ID_EX_new->rt, ID_EX_old, EX_MEM_old, MEM_WB_old);
	}
	if (ID_EX_new->op_code >= BEQ && ID_EX_new->op_code <= BGE || ID_EX_new->op_code == SW) {
		res |= is_register_in_pipeline(ID_EX_new->dst, ID_EX_old, EX_MEM_old, MEM_WB_old);
	}
	return res;
}
bool is_need_to_stall_in_decode_stage(core_pipeline_registers* pipeline_regs_old, ID_EX_registers* ID_EX_new)
{
	return There_is_a_data_hazard(ID_EX_new, &pipeline_regs_old->ID_EX_regs,
		&pipeline_regs_old->EX_MEM_regs, &pipeline_regs_old->MEM_WB_regs);
}
void ID_stage(core_info_struct* core_data) {

	IF_ID_registers* IF_ID_old = &core_data->pipeline_regs_old.IF_ID_regs, * IF_ID_new = &core_data->pipeline_regs_new.IF_ID_regs;
	ID_EX_registers* ID_EX_new = &core_data->pipeline_regs_new.ID_EX_regs;
	decoded_instruction instruction;
	decode_IR(IF_ID_old->IR, &instruction);
	update_immidiate_register(instruction.immediate, core_data->core_regs);
	fetch_regs_from_file_regsister(instruction, ID_EX_new, core_data->core_regs);
	if (is_the_branch_taken(ID_EX_new->rs_data, ID_EX_new->rt_data, ID_EX_new->op_code)) {
		IF_ID_new->next_pc = ID_EX_new->rd_data % MAX_PC;
		if (instruction.op_code == JAL) {
			ID_EX_new->dst = JAL_REGISTER;
			ID_EX_new->rd_data = (IF_ID_old->current_pc + 1) % MAX_PC;   //R[15] = next PC. Will be write back in WB stage.
		}
	}
	ID_EX_new->current_pc = IF_ID_old->current_pc;
	if (instruction.op_code == HALT) {  //Decode HALT
		IF_ID_new->fetch_enable_flag = false;
		IF_ID_new->IR = BUBBLE_INSTRUCTION;
	}
	//TODO
	ID_EX_new->decode_stall_flag = is_need_to_stall_in_decode_stage(&core_data->pipeline_regs_old, ID_EX_new);
}

void EX_stage(ID_EX_registers* ID_EX_old, EX_MEM_registers* EX_MEM_new) {


	if (is_the_instruction_for_ALU(ID_EX_old->op_code)) {
		EX_MEM_new->ALU_result = ALU_instruction(ID_EX_old->rs_data, ID_EX_old->rt_data, ID_EX_old->op_code);
	}
	//TODO
	else if (ID_EX_old->op_code >= LW && ID_EX_old->op_code <= SW) {  //MEM instructions
		EX_MEM_new->ALU_result = ADD_instruction(ID_EX_old->rs_data, ID_EX_old->rt_data);
	}
	EX_MEM_new->data_in = ID_EX_old->rd_data;
	EX_MEM_new->dst = ID_EX_old->dst;
	EX_MEM_new->op_code = ID_EX_old->op_code;
	EX_MEM_new->current_pc = ID_EX_old->current_pc;

}

void MEM_stage(core_info_struct* core_data, cache_data* cache_data,
	MESI_Bus* OldMsiBus, MESI_Bus* NewMsiBus, main_memory* changed_main_memory) {

	EX_MEM_registers* EX_MEM_old = &core_data->pipeline_regs_old.EX_MEM_regs;
	MEM_WB_registers* MEM_WB_new = &core_data->pipeline_regs_new.MEM_WB_regs;
	int ProcessorNumber = core_data->pipeline_regs_old.number_of_the_core;
	bool memory_hit = true;
	if (EX_MEM_old->op_code == LW) {//TODO
		memory_hit = read_from_cache(EX_MEM_old->ALU_result, EX_MEM_old->data_in, &MEM_WB_new->MEM_result,
			ProcessorNumber, cache_data, OldMsiBus, NewMsiBus, changed_main_memory);
		/*if (memory_hit && EX_MEM_old->op_code == LL) {
			set_watch_flag(core_data, EX_MEM_old->ALU_result);
		}*/
	}
	else if (EX_MEM_old->op_code == SW) {
		memory_hit = write_to_cache(EX_MEM_old->ALU_result, EX_MEM_old->data_in,
			ProcessorNumber, cache_data, OldMsiBus, NewMsiBus, changed_main_memory);
	}

	MEM_WB_new->ALU_result = EX_MEM_old->ALU_result;
	MEM_WB_new->dst = EX_MEM_old->dst;
	MEM_WB_new->op_code = EX_MEM_old->op_code;
	MEM_WB_new->current_pc = EX_MEM_old->current_pc;
	MEM_WB_new->MEM_stall_flag = (memory_hit == false);
	if (EX_MEM_old->op_code == JAL)   //moving next PC to WB stage
		MEM_WB_new->MEM_result = EX_MEM_old->data_in;

}

void WB_stage(MEM_WB_registers* MEM_WB_old, int* core_regs) {

	if (MEM_WB_old->dst == ZERO || MEM_WB_old->dst == IMM) {  //RO registers
		return;
	}
	if (is_the_instruction_for_ALU(MEM_WB_old->op_code)) {
		core_regs[MEM_WB_old->dst] = MEM_WB_old->ALU_result;
	}
	else if (MEM_WB_old->op_code == LW || MEM_WB_old->op_code == JAL) {
		core_regs[MEM_WB_old->dst] = MEM_WB_old->MEM_result;
	}
}


void check_for_misses_and_hits(EX_MEM_registers* EX_MEM_old, MEM_WB_registers* MEM_WB_old,
	MEM_WB_registers* MEM_WB_new, core_statistics* core_statistics)
{
	bool hit = (MEM_WB_old->MEM_stall_flag == false && MEM_WB_new->MEM_stall_flag == false),
		miss = (MEM_WB_old->MEM_stall_flag == false && MEM_WB_new->MEM_stall_flag == true);
	if (EX_MEM_old->op_code == SW) {
		core_statistics->number_of_write_hit += hit ? 1 : 0;
		core_statistics->number_of_write_miss += miss ? 1 : 0;
	}
	if (EX_MEM_old->op_code == LW) {
		core_statistics->number_of_read_hits += hit ? 1 : 0;
		core_statistics->number_of_read_miss += miss ? 1 : 0;
	}
}

void update_core_statistic(core_info_struct* core_data)
{
	core_statistics* core_statistics = &core_data->core_statistic;
	MEM_WB_registers* MEM_WB_old = &core_data->pipeline_regs_old.MEM_WB_regs, * MEM_WB_new = NULL;
	EX_MEM_registers* EX_MEM_old = &core_data->pipeline_regs_old.EX_MEM_regs;
	bool is_mem_stall = core_data->pipeline_regs_new.MEM_WB_regs.MEM_stall_flag,
		is_decode_stall = core_data->pipeline_regs_new.ID_EX_regs.decode_stall_flag;
	core_statistics->number_of_cycles++;
	if (MEM_WB_old->op_code != BUBBLE) {  //Valid instruction finished WB in this clk
		core_statistics->number_of_instructions++;
	}
	if (MEM_WB_old->op_code == HALT) {
		return;
	}
	if (is_decode_stall && is_mem_stall == false) {
		core_statistics->number_of_decode_stalls++;
	}
	if (is_mem_stall) {
		core_statistics->number_of_mememory_stalls++;
	}
	if ((LW <= EX_MEM_old->op_code)) {
		if (SW >= EX_MEM_old->op_code) {
			MEM_WB_new = &core_data->pipeline_regs_new.MEM_WB_regs;
			check_for_misses_and_hits(EX_MEM_old, MEM_WB_old, MEM_WB_new, core_statistics);
		}
	}
}

void run_one_pipeline_cycle(core_info_struct* core_data, global_memory* global_memory, int ProcessorNumber)
{
	core_pipeline_registers* pipeline_regs_old = &core_data->pipeline_regs_old;
	core_pipeline_registers* pipeline_regs_new = &core_data->pipeline_regs_new;
	IF_stage(&pipeline_regs_old->IF_ID_regs, &pipeline_regs_new->IF_ID_regs, core_data->IMEM);
	ID_stage(core_data);
	EX_stage(&pipeline_regs_old->ID_EX_regs, &pipeline_regs_new->EX_MEM_regs);
	MEM_stage(core_data, &core_data->cache_data, &global_memory->old_MESI_bus,
		&global_memory->new_MESI_bus, &global_memory->changed_main_memory);
	WB_stage(&pipeline_regs_old->MEM_WB_regs, core_data->core_regs);
	update_core_statistic(core_data);
	update_pipeline_registers(pipeline_regs_old, pipeline_regs_new);
}

void last_core_cycle(core_info_struct* core_data, int number_of_core, int argc, char** argv)
{
	//TODO:Check if core number needed
	update_core_statistic(core_data);
	print_REGOUT_for_single_core(core_data->pipeline_regs_old.number_of_the_core, core_data->core_regs, argc, argv);
	print_statistics_of_the_core(core_data->pipeline_regs_old.number_of_the_core, core_data->core_statistic, argc, argv);
}

bool run_one_core_cycle(core_info_struct* core_data, global_memory* global_memory, int number_of_core, unsigned int clk, int argc, char** argv)
{
	print_trace_of_a_single_core(&core_data->pipeline_regs_old, clk, core_data->core_regs, argc, argv);
	if (core_data->pipeline_regs_old.MEM_WB_regs.op_code == HALT) {
		last_core_cycle(core_data, number_of_core, argc, argv);
		return false;
	}
	run_one_pipeline_cycle(core_data, global_memory, number_of_core);
	return true;
}

bool there_is_watch_flag_active(core_info_struct* core_data, int bus_origid)
{
	bool result = false;
	for (int i = 0; i < NUMBER_OF_CORES; i++) {
		if (i == bus_origid)
			continue;
		result |= (core_data + i)->watch_flag.is_watched;
	}
	return result;
}
bool need_to_check_for_watch_update(core_info_struct* cores_data, MESI_Bus old_MESI_bus)
{
	if (old_MESI_bus.bus_origid == MAIN_MEMORY_ORIGID)
		return false;
	/*if ((cores_data + old_MESI_bus.bus_origid)->pipeline_regs_old.EX_MEM_regs.op_code != SC)  //Only sc cmd from another core reset watch.
		return false;*/
	return there_is_watch_flag_active(cores_data, old_MESI_bus.bus_origid);
}

void check_for_watch_flags_update(core_info_struct* cores_data, MESI_Bus old_MESI_bus)
{
	for (int i = 0; i < NUMBER_OF_CORES; i++) {
		if (i == old_MESI_bus.bus_origid)
			continue;
		if (old_MESI_bus.bus_address == (cores_data + i)->watch_flag.addr_to_watch)
			(cores_data + i)->watch_flag.is_watched = false;
	}
}