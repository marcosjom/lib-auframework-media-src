//
//  nbMemmap.h
//
//  Created by Marcos Ortega on 24/02/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#ifndef nibsa_memmap
#define nibsa_memmap

//---------------------------------------------
//-- Usefull implementation for memory leaking
//-- detection and tracking.
//---------------------------------------------

//STNB_MemBlock
typedef struct {
	char			regUsed;
	unsigned long	iStrHint;
	void*			pointer;
	unsigned long	bytes;
} STNB_MemBlock;

//STNBMemMap
typedef struct {
	//
	unsigned long	currCountAllocationsActive;
	unsigned long	currBytesAllocationsActive;
	unsigned long	totalCountAllocations;
	unsigned long	totalBytesAllocations;
	unsigned long	maxCountAllocationsActive;
	unsigned long	maxBytesAllocationsActive;
	//Strings (Hints)
	char*			strArr;
	unsigned long	strArrSize;
	unsigned long	strArrUse;
	unsigned long*	strIndexsArr;
	unsigned long	strIndexsArrSize;
	unsigned long	strIndexsArrUse;
	//Allocated memory blocks
	STNB_MemBlock*	blocksArr;
	unsigned long	blocksArrUse;
	unsigned long	blocksArrSize;
} STNBMemMap;

void nbMemmapInit(STNBMemMap* map);
void nbMemmapFinalize(STNBMemMap* map);
void nbMemmapRegister(STNBMemMap* map, void* pointer, unsigned long bytes, const char* strHint);
void nbMemmapUnregister(STNBMemMap* map, void* pointer);

void nbMemmapPrintActive(STNBMemMap* map);
void nbMemmapPrintFinalReport(STNBMemMap* map);

#endif
