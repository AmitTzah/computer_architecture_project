
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "core.h"
#include "core_instructions.h"

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

void IF_stage(IF_ID_registers* p_IF_ID_new_regs
	, IF_ID_registers* p_IF_ID_old_regs, unsigned int* IMEM) {

	if (p_IF_ID_old_regs->fetch_enable_flag) {
		p_IF_ID_new_regs->IR = IMEM[p_IF_ID_old_regs->next_pc];
		p_IF_ID_new_regs->next_pc = (p_IF_ID_old_regs->next_pc + 1) % MAX_PC;
		p_IF_ID_new_regs->current_pc = p_IF_ID_old_regs->next_pc;
	}
	else {
		p_IF_ID_new_regs->IR = BUBBLE;//TODO: insert bubble instruction 
	}

}

bool is_the_instruction_for_ALU(opcode opcode)
{
	if (opcode >= ADD && opcode <= SRL)
		return true;
	return false;
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
	//ID_EX_new->is_decode_stall = is_need_to_stall_in_decode_stage(&core_data->pipeline_regs_old, ID_EX_new);
}

void EX_stage(ID_EX_registers* ID_EX_old, EX_MEM_registers* EX_MEM_new) {


	if (is_the_instruction_for_ALU(ID_EX_old->op_code)) {
		EX_MEM_new->ALU_result = ALU_instruction(ID_EX_old->rs_data, ID_EX_old->rt_data, ID_EX_old->op_code);
	}
	else if (ID_EX_old->op_code >= LW && ID_EX_old->op_code <= SC) {  //MEM instructions
		EX_MEM_new->ALU_result = ADD_instruction(ID_EX_old->rs_data, ID_EX_old->rt_data);
	}
	EX_MEM_new->data_in = ID_EX_old->rd_data;
	EX_MEM_new->dst = ID_EX_old->dst;
	EX_MEM_new->op_code = ID_EX_old->op_code;
	EX_MEM_new->current_pc = ID_EX_old->current_pc;

}

void MEM_stage() {


}

void WB_stage() {


}





