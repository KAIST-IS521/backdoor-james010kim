//-----------------------------------------------------------------------------
// minivm.c -- The basic definitions for the vm.
// Copyright (C) Philip Conrad 4/30/2013 @ 3:56 PM -- MIT License
//
//-----------------------------------------------------------------------------

#include "minivm.h"


//---------------------------------------------------------
// FUNCTION IMPLEMENTATIONS:
void vm_halt(struct VMContext* ctx, const uint32_t instr) {
	exit(0);
}

void vm_gets(struct VMContext* ctx, const uint32_t instr) {
	int p1 = EXTRACT_B1(instr); int p2 = EXTRACT_B2(instr); int p3 = EXTRACT_B3(instr);

	char* addr_r = (char*)(ctx->r[p1].value);
	if ( addr_r == 0 ) { printf ( " - invalid register address\n" ); exit(0); }
	gets( addr_r );
}

void vm_puts(struct VMContext* ctx, const uint32_t instr) {
	int p1 = EXTRACT_B1(instr); int p2 = EXTRACT_B2(instr); int p3 = EXTRACT_B3(instr);

	char* addr_r = (char*)(ctx->r[p1].value);
	if ( addr_r == 0 ) { printf ( " - invalid register address\n" ); exit(0); }
	printf ( "%s", addr_r );
}

void vm_move(struct VMContext* ctx, const uint32_t instr) {
	int p1 = EXTRACT_B1(instr); int p2 = EXTRACT_B2(instr); int p3 = EXTRACT_B3(instr); 
	ctx->r[p1] = ctx->r[p2];
}

void vm_puti(struct VMContext* ctx, const uint32_t instr) {
	int p1 = EXTRACT_B1(instr); int p2 = EXTRACT_B2(instr); int p3 = EXTRACT_B3(instr); 
	int i8v = EXTRACT_B0(p2);
	ctx->r[p1].type = DATA;
	ctx->r[p1].value = i8v;
}

void vm_eq(struct VMContext* ctx, const uint32_t instr) {
	int p1 = EXTRACT_B1(instr); int p2 = EXTRACT_B2(instr); int p3 = EXTRACT_B3(instr); 

	if ( ctx->r[p2].value == ctx->r[p3].value )
		ctx->r[p1].value = 1;
	else
		ctx->r[p1].value = 0;
}

void vm_ite(struct VMContext* ctx, const uint32_t instr) {
	int p1 = EXTRACT_B1(instr); int p2 = EXTRACT_B2(instr); int p3 = EXTRACT_B3(instr); 

	uint32_t** pc = ctx->r[210].value;
	uint32_t new_pc = 0;
	
	if ( ctx->r[p1].value > 0 )
		new_pc = ctx->r[199].value + p2 * 4 - 8;
	else if ( ctx->r[p1].value == 0 )
		new_pc = ctx->r[199].value + p3 * 4 - 8;
	else
		return;

	(*pc) = new_pc;
}

void vm_jump(struct VMContext* ctx, const uint32_t instr) {
	int p1 = EXTRACT_B1(instr); int p2 = EXTRACT_B2(instr); int p3 = EXTRACT_B3(instr); 

	uint32_t** pc = ctx->r[210].value;
	uint32_t new_pc = ctx->r[199].value + p1 * 4 - 8;
	(*pc) = new_pc;
}

void vm_add(struct VMContext* ctx, const uint32_t instr) {
	int p1 = EXTRACT_B1(instr); int p2 = EXTRACT_B2(instr); int p3 = EXTRACT_B3(instr); 

	ctx->r[p1].type = DATA;
	ctx->r[p1].value = ctx->r[p2].value + ctx->r[p3].value;
}

void vm_load(struct VMContext* ctx, const uint32_t instr) {
	int p1 = EXTRACT_B1(instr); int p2 = EXTRACT_B2(instr); int p3 = EXTRACT_B3(instr); 

	char* addr_r = (char*)(ctx->r[p2].value);
	ctx->r[p1].value = EXTRACT_B0(*addr_r);
}

void vm_store(struct VMContext* ctx, const uint32_t instr) {
	int p1 = EXTRACT_B1(instr); int p2 = EXTRACT_B2(instr); int p3 = EXTRACT_B3(instr); 

	int i8v = EXTRACT_B0(ctx->r[p2].value);
	char* addr_r = (char*)(ctx->r[p1].value);
	memcpy ( addr_r , &i8v, 1 );
}


// Defers decoding of register args to the called function.
// dispatch :: VMContext -> uint32_t -> Effect()
void dispatch(struct VMContext* ctx, const uint32_t instr) {
    const uint8_t i = EXTRACT_B0(instr);
    (*ctx->funtable[i])(ctx, instr);
}


// Initializes a VMContext in-place.
// initVMContext :: VMContext -> uint32_t -> uint32_t -> [Reg] -> [FunPtr] -> Effect()
void initVMContext(struct VMContext* ctx, const uint32_t numRegs, const uint32_t numFuns, Reg* registers, FunPtr* funtable) {
    ctx->numRegs    = numRegs;
    ctx->numFuns    = numFuns;
    ctx->r          = registers;
    ctx->funtable   = funtable;

    char* vm_memory = (char*)malloc(8192);
    printf ( " + init vm_mem : %x\n", vm_memory);
    ctx->r[200].type  = MEMSTART;
    ctx->r[200].value = vm_memory;
}


// Reads an instruction, executes it, then steps to the next instruction.
// stepVMContext :: VMContext -> uint32_t** -> Effect()
void stepVMContext(struct VMContext* ctx, uint32_t** pc) {
    // Read a 32-bit bytecode instruction.
    uint32_t instr = **pc;

    // set pc to instruction pointer.
    ctx->r[210].type = INSTRPTR;
    ctx->r[210].value = pc;
//    printf ( "\n --> IP[%d]: ", ((unsigned int)(*pc)-(unsigned int)ctx->r[199].value)/4+1 );

    // Dispatch to an opcode-handler.
    dispatch(ctx, instr);

    // Increment to next instruction.
    (*pc)++;
}

