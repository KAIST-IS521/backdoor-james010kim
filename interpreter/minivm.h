//-----------------------------------------------------------------------------
// minivm.h -- Prototypes and definitions for the vm.
// Copyright (C) Philip Conrad 4/30/2013 @ 3:56 PM -- MIT License
//
//-----------------------------------------------------------------------------

#include <stdint.h>

#ifndef MINIVM_H
#define MINIVM_H


//---------------------------------------------------------
// MACRO DEFINITIONS:


// Size of the global function pointer table
#define MVM_NUM_FUNS 256

#define MVM_NUM_REGISTERS 16 // Default


//---------------------------------------------------------
// DATA STRUCTURES & TYPEDEFS:

struct VMContext;

typedef void (*FunPtr)(struct VMContext* ctx, const uint32_t);

//CHANGE THE INTERNALS OF THIS FOR YOUR OWN VM!
typedef struct Reg {
    uint32_t type;
    uint32_t value;
} Reg;

typedef struct VMContext {
    uint32_t numRegs;
    uint32_t numFuns;
    Reg* r;           // Ptr to register array.
    FunPtr* funtable; // Ptr to a funptr table.
} VMContext;

// Actually, flags does not required. Just register is meanful.
#define		CODESTART	0x00	// r199
#define		MEMSTART	0x01	// r200
#define		DATA		0x02
#define		INSTRPTR	0x03	// r210
#define		RETURNADDR	0x04	// r220 , posA : 1 , posB : 0
#define		STRCMPP1	0x05	// r231
#define		STRCMPP2	0x06	// r232
#define		STRCMPRET	0x07	// r0

//---------------------------------------------------------
// ESOTERIC ITEMS:


#ifdef MVM_GLOBAL_FUNTABLE
// The global function pointer table.
static FunPtr mvm_function_table[MVM_NUM_FUNS];
#endif

// Byte extraction macros.
#define EXTRACT_B0(i) (i & 0xFF)
#define EXTRACT_B1(i) ((i >> 8) & 0xFF)
#define EXTRACT_B2(i) ((i >> 16) & 0xFF)
#define EXTRACT_B3(i) ((i >> 24) & 0xFF)


//---------------------------------------------------------
// FUNCTIONS:

void vm_halt(struct VMContext* ctx, const uint32_t instr);
void vm_load(struct VMContext* ctx, const uint32_t instr);
void vm_gets(struct VMContext* ctx, const uint32_t instr);
void vm_puts(struct VMContext* ctx, const uint32_t instr);
void vm_move(struct VMContext* ctx, const uint32_t instr);
void vm_puti(struct VMContext* ctx, const uint32_t instr);
void vm_ite(struct VMContext* ctx, const uint32_t instr);
void vm_eq(struct VMContext* ctx, const uint32_t instr);
void vm_jump(struct VMContext* ctx, const uint32_t instr);
void vm_add(struct VMContext* ctx, const uint32_t instr);
void vm_store(struct VMContext* ctx, const uint32_t instr);

// Selects and executes an opcode function from the function pointer table.
// Passes the entire bytecode instruction as the argument.
// dispatch :: VMContext -> uint32_t -> Effect()
void dispatch(struct VMContext* ctx, const uint32_t instr);

// Initializes a VMContext in-place.
// initVMContext :: VMContext -> uint32_t -> uint32_t -> [Reg] -> [FunPtr] -> Effect()
void initVMContext(struct VMContext* ctx,
                      const uint32_t numRegs,
                      const uint32_t numFuns,
                                Reg* registers,
                             FunPtr* funtable);

// Reads an instruction, executes it, then steps to the next instruction.
// stepVMContext :: VMContext -> uint32_t** -> Effect()
void stepVMContext(struct VMContext* ctx, uint32_t** pc);


//---------------------------------------------------------
#endif //ifndef MINIVM_H
