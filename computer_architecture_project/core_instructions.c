#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "core.h"

#define ZERO 0

int ADD_instruction(int rs_data, int rt_data)
{
	return rs_data + rt_data;
}
int SUB_instruction(int rs_data, int rt_data)
{
	return rs_data - rt_data;
}
int AND_instruction(int rs_data, int rt_data)
{
	return rs_data & rt_data;
}
int OR_instruction(int rs_data, int rt_data)
{
	return rs_data | rt_data;
}
int XOR_instruction(int rs_data, int rt_data)
{
	return rs_data ^ rt_data;
}
int MUL_instruction(int rs_data, int rt_data)
{
	return rs_data * rt_data;
}
int SLL_instruction(int rs_data, int rt_data)
{
	return rs_data << rt_data;
}
int SRA_instruction(int rs_data, int rt_data)
{
	return rs_data >> rt_data;
}
int SRL_instruction(int rs_data, int rt_data)
{
	return (int)((unsigned int)rs_data >> rt_data);
}


int ALU_instruction(int rs_data, int rt_data, opcode opcode)
{
	switch (opcode)
	{
	case ADD:
		return ADD_instruction(rs_data, rt_data);
	case SUB:
		return SUB_instruction(rs_data, rt_data);
	case AND:
		return AND_instruction(rs_data, rt_data);
	case OR:
		return OR_instruction(rs_data, rt_data);
	case XOR:
		return XOR_instruction(rs_data, rt_data);
	case MUL:
		return MUL_instruction(rs_data, rt_data);
	case SLL:
		return SLL_instruction(rs_data, rt_data);
	case SRA:
		return SRA_instruction(rs_data, rt_data);
	case SRL:
		return SRL_instruction(rs_data, rt_data);
	default:
		return ZERO;
	}
}