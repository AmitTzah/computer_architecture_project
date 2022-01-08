

#include "core.h"


void insert_bubbles_to_pipeline_registers(core_pipeline_registers* pipeline_regs)
{
	pipeline_regs->IF_ID_regs.IR = 0x15000000;
	pipeline_regs->ID_EX_regs.op_code = BUBBLE;
	pipeline_regs->EX_MEM_regs.op_code = BUBBLE;
	pipeline_regs->MEM_WB_regs.op_code = BUBBLE;
}
void initialize_pipeline_registers(core_pipeline_registers* pipeline_regs, int number_of_the_core)
{
	pipeline_regs->number_of_the_core = number_of_the_core;
	insert_bubbles_to_pipeline_registers(pipeline_regs);
	pipeline_regs->IF_ID_regs.next_pc = 0;
	pipeline_regs->IF_ID_regs.fetch_enable_flag = true;
	pipeline_regs->ID_EX_regs.decode_stall_flag = false;
	pipeline_regs->MEM_WB_regs.MEM_stall_flag = false;
}
void initialize_all_core_pipeline_registers(core_pipeline_registers* pipeline_regs_old,
	core_pipeline_registers* pipeline_regs_new, int number_of_the_core)
{
	initialize_pipeline_registers(pipeline_regs_old, number_of_the_core);
	initialize_pipeline_registers(pipeline_regs_new, number_of_the_core);
}

void update_pipeline_registers(core_pipeline_registers* pipeline_regs_old, core_pipeline_registers* pipeline_regs_new)
{
	bool decode_stall = pipeline_regs_new->ID_EX_regs.decode_stall_flag
		, mem_stall = pipeline_regs_new->MEM_WB_regs.MEM_stall_flag;
	if (mem_stall) {
		pipeline_regs_new->MEM_WB_regs.op_code = BUBBLE;
	}
	else {
		memcpy(&pipeline_regs_old->EX_MEM_regs, &pipeline_regs_new->EX_MEM_regs, sizeof(EX_MEM_registers));
		if (decode_stall) {
			pipeline_regs_old->ID_EX_regs.op_code = BUBBLE;
		}
		else {
			memcpy(&pipeline_regs_old->IF_ID_regs, &pipeline_regs_new->IF_ID_regs, sizeof(IF_ID_registers));
			memcpy(&pipeline_regs_old->ID_EX_regs, &pipeline_regs_new->ID_EX_regs, sizeof(ID_EX_registers));
		}
	}
	memcpy(&pipeline_regs_old->MEM_WB_regs, &pipeline_regs_new->MEM_WB_regs, sizeof(MEM_WB_registers));
}