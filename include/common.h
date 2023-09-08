#ifndef COMMON_H
#define COMMON_H

/* Define structs and routine could be used by every source files */

#include <stdint.h>

#ifndef OSCFG_H
#include "os-cfg.h"
#endif

#ifndef OSMM_H
#include "os-mm.h"
#endif

#define ADDRESS_SIZE	20
#define OFFSET_LEN		10
#define FIRST_LV_LEN	5
#define SECOND_LV_LEN	5
#define SEGMENT_LEN     FIRST_LV_LEN
#define PAGE_LEN        SECOND_LV_LEN

#define NUM_PAGES	(1 << (ADDRESS_SIZE - OFFSET_LEN))
#define PAGE_SIZE	(1 << OFFSET_LEN)

enum ins_opcode_t {
	CALC,	// Just perform calculation, only use CPU
	ALLOC,	// Allocate memory
	FREE,	// Deallocated a memory block
	READ,	// Write data to a byte on memory
	WRITE	// Read data from a byte on memory
};

/* instructions executed by the CPU */
struct inst_t {
	enum ins_opcode_t opcode;
	uint32_t arg_0; // Argument lists for instructions
	uint32_t arg_1;
	uint32_t arg_2;
};

struct code_seg_t {
	struct inst_t * text;
	uint32_t size;
};

struct trans_table_t {
	/* A row in the page table of the second layer */
	struct  {
		addr_t v_index; // The index of virtual address
		addr_t p_index; // The index of physical address
	} table[1 << SECOND_LV_LEN];
	int size;
};

/* Mapping virtual addresses and physical ones */
struct page_table_t {
	/* Translation table for the first layer */
	struct {
		addr_t v_index;	// Virtual index
		struct trans_table_t * next_lv;
	} table[1 << FIRST_LV_LEN];
	int size;	// Number of row in the first layer
};

/* PCB, describe information about a process */
struct pcb_t {
	uint32_t pid;	// PID
	uint32_t priority; // Default priority, this legacy (FIXED) value depend on process itself
	struct code_seg_t * code;	// Code segment
	addr_t regs[10]; // Registers, store address of allocated regions
	uint32_t pc; // Program pointer, point to the next instruction
#ifdef MLQ_SCHED
	// Priority on execution (if supported), on-fly aka. changeable
	// and this vale overwrites the default priority when it existed
	uint32_t prio;     
#endif
#ifdef MM_PAGING
	struct mm_struct *mm;
	struct memphy_struct *mram;
	struct memphy_struct **mswp;
	struct memphy_struct *active_mswp;
#endif
	struct page_table_t * page_table; // Page table
	uint32_t bp;	// Break pointer

};

#endif

