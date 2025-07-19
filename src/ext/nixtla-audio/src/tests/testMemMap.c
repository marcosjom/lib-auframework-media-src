//
//  nbMemmap.c
//
//  Created by Marcos Ortega on 24/02/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#include <stdio.h>		//NULL
#include <stdlib.h>		//malloc, free
#include <string.h>		//memcpy, memset
#include <assert.h>		//assert

#include "testMemMap.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__ANDROID__) //Android
#   define NIX_PRINTF_INFO(STR_FMT, ...)   __android_log_print(ANDROID_LOG_INFO, "Nixtla", STR_FMT, ##__VA_ARGS__)
#   define NIX_PRINTF_ERROR(STR_FMT, ...)  __android_log_print(ANDROID_LOG_ERROR, "Nixtla", "ERROR, "STR_FMT, ##__VA_ARGS__)
#   define NIX_PRINTF_WARNING(STR_FMT, ...) __android_log_print(ANDROID_LOG_WARN, "Nixtla", "WARNING, "STR_FMT, ##__VA_ARGS__)
#elif defined(__QNX__) //BB10
#   define NIX_PRINTF_INFO(STR_FMT, ...)   fprintf(stdout, "Nix, " STR_FMT, ##__VA_ARGS__); fflush(stdout)
#   define NIX_PRINTF_ERROR(STR_FMT, ...)  fprintf(stderr, "Nix ERROR, " STR_FMT, ##__VA_ARGS__); fflush(stderr)
#   define NIX_PRINTF_WARNING(STR_FMT, ...) fprintf(stdout, "Nix WARNING, " STR_FMT, ##__VA_ARGS__); fflush(stdout)
#else
#   define NIX_PRINTF_INFO(STR_FMT, ...)   printf("Nix, " STR_FMT, ##__VA_ARGS__)
#   define NIX_PRINTF_ERROR(STR_FMT, ...)   printf("Nix ERROR, " STR_FMT, ##__VA_ARGS__)
#   define NIX_PRINTF_WARNING(STR_FMT, ...) printf("Nix WARNING, " STR_FMT, ##__VA_ARGS__)
#endif
    
//---------------------------------------------
//-- Usefull implementation for memory leaking
//-- detection and tracking.
//---------------------------------------------

void nbMemmapInit(STNBMemMap* map){
    memset(map, 0, sizeof(*map));
	//Strings (Hints)
	map->strArr				= (char*)malloc(sizeof(char) * 2048);
	map->strArr[0]			= '\0'; //The first string is always <empty string>
	map->strArrUse			= 1;
	map->strArrSize			= 2048;
}

void nbMemmapFinalize(STNBMemMap* map){
	if(map->strArr!=NULL) free(map->strArr); map->strArr = NULL;
	if(map->strIndexsArr!=NULL) free(map->strIndexsArr); map->strIndexsArr = NULL;
	if(map->blocksArr!=NULL) free(map->blocksArr); map->blocksArr = NULL;
}

void nbMemmapRegister(STNBMemMap* map, void* pointer, unsigned long bytes, const char* strHint){
	unsigned long i, iStrHint = 0; const unsigned long strIndexsArrUse = map->strIndexsArrUse; const unsigned long blocksArrUse = map->blocksArrUse;
	//Is this pointer already registered?
	for(i=0; i<blocksArrUse; i++){
		if(map->blocksArr[i].regUsed){
			assert(map->blocksArr[i].pointer != pointer);
		}
	}
	//Find or register hint string
	if(strHint!=NULL){
		if(strHint[0]!='\0'){
			//Look for string
			char strEquals; const char* cadena1; const char* cadena2;
			for(i=0; i<strIndexsArrUse; i++){
				assert(map->strIndexsArr[i] < map->strArrUse);
				cadena1		= strHint;
				cadena2		= &map->strArr[map->strIndexsArr[i]];
				strEquals	= 1;
				//comparas caracteres
				while((*cadena1) != 0 && (*cadena2) != 0){
					assert(cadena2 < &map->strArr[map->strArrUse]);
					if((*cadena1)!=(*cadena2)){
						strEquals = 0;
						break;
					}
					cadena1++;	//mover puntero al siguiente caracter
					cadena2++;	//mover puntero al siguiente caracter
				}
				//validar si no se realizo la compracion de todos los caracteres (una cadena es mas corta que la otra)
				if(strEquals){
					if((*cadena1)==(*cadena2)){ //la condicional es falso si uno es '\0' y el otro es diferente
						iStrHint = map->strIndexsArr[i]; assert(iStrHint != 0);
						break;
					}
				}
			}
			//Register new string
			if(iStrHint == 0){
				unsigned long strHintLen = 0;
				cadena1		= strHint;
				while((*cadena1) != 0) cadena1++;
				strHintLen	= (cadena1 - strHint);
				//
				if((map->strArrUse + strHintLen) >= map->strArrSize){
					char* newArr;
					map->strArrSize += 2048 + ((map->strArrUse + strHintLen) - map->strArrSize);
					newArr			= (char*)malloc(sizeof(char) * map->strArrSize);
					if(map->strArr!=NULL){
						if(map->strArrUse != 0) memcpy(newArr, map->strArr, sizeof(char) * map->strArrUse);
						free(map->strArr);
					}
					map->strArr = newArr;
				}
				//
				if(map->strIndexsArrUse == map->strIndexsArrSize){
					unsigned long* newArr;
					map->strIndexsArrSize	+= 128;
					newArr					= (unsigned long*)malloc(sizeof(unsigned long) * map->strIndexsArrSize);
					if(map->strIndexsArr!=NULL){
						if(map->strIndexsArrUse != 0) memcpy(newArr, map->strIndexsArr, sizeof(unsigned long) * map->strIndexsArrUse);
						free(map->strIndexsArr);
					}
					map->strIndexsArr = newArr;
				}
				//Add index and string
				iStrHint = map->strArrUse;
				map->strIndexsArr[map->strIndexsArrUse++] = map->strArrUse;
				memcpy(&map->strArr[map->strArrUse], strHint, strHintLen + 1);
				map->strArrUse += (strHintLen + 1);
			}
		}
	}
	//assert(iStrHint != 0); //Temporal
	//Register pointer
	{
		STNB_MemBlock newBlock;
		//
		map->currCountAllocationsActive	+= 1;
		map->currBytesAllocationsActive	+= bytes;
		map->totalCountAllocations		+= 1;
		map->totalBytesAllocations		+= bytes;
		if(map->maxCountAllocationsActive < map->currCountAllocationsActive) map->maxCountAllocationsActive = map->currCountAllocationsActive;
		if(map->maxBytesAllocationsActive < map->currBytesAllocationsActive) map->maxBytesAllocationsActive = map->currBytesAllocationsActive;
		//Reusing register
		newBlock.regUsed	= 1;
		newBlock.iStrHint	= iStrHint;
		newBlock.pointer	= pointer;
		newBlock.bytes		= bytes;
		for(i=0; i<blocksArrUse; i++){
			if(!map->blocksArr[i].regUsed){
				map->blocksArr[i] = newBlock;
				return;
			}
		}
		//Adding register
		if(map->blocksArrUse == map->blocksArrSize){
			STNB_MemBlock* newArr;
			map->blocksArrSize	+= 1024;
			newArr				= (STNB_MemBlock*)malloc(sizeof(STNB_MemBlock) * map->blocksArrSize);
			if(map->blocksArr!=NULL){
				if(map->blocksArrUse != 0) memcpy(newArr, map->blocksArr, sizeof(STNB_MemBlock) * map->blocksArrUse);
				free(map->blocksArr);
			}
			map->blocksArr = newArr;
		}
		map->blocksArr[map->blocksArrUse++] = newBlock;
	}
}

void nbMemmapUnregister(STNBMemMap* map, void* pointer){
	unsigned long i; const unsigned long blocksArrUse = map->blocksArrUse;
	for(i=0; i<blocksArrUse; i++){
		if(map->blocksArr[i].regUsed){
			if(map->blocksArr[i].pointer == pointer){
				map->blocksArr[i].regUsed = 0;
				map->currCountAllocationsActive -= 1;
				map->currBytesAllocationsActive	-= map->blocksArr[i].bytes;
				return;
			}
		}
	}
	assert(0); //Pointer was not found
}

void nbMemmapPrintActive(STNBMemMap* map){
	unsigned long i, countUsed = 0; const unsigned long blocksArrUse = map->blocksArrUse;
	for(i=0; i<blocksArrUse; i++){
		if(map->blocksArr[i].regUsed){
			countUsed++;
            NIX_PRINTF_INFO("#%lu) %lu, %lu bytes, '%s'\n", countUsed, (unsigned long) map->blocksArr[i].pointer, map->blocksArr[i].bytes, &map->strArr[map->blocksArr[i].iStrHint]);
		}
	}
    NIX_PRINTF_INFO("\n");
    NIX_PRINTF_INFO("CURRENTLY USED   : %lu blocks (%lu bytes)\n", map->currCountAllocationsActive, map->currBytesAllocationsActive);
    NIX_PRINTF_INFO("MAX USED         : %lu blocks (%lu bytes)\n", map->maxCountAllocationsActive, map->maxBytesAllocationsActive);
    NIX_PRINTF_INFO("TOTAL ALLOCATIONS: %lu blocks (%lu bytes)\n", map->totalCountAllocations, map->totalBytesAllocations);
	assert(map->currCountAllocationsActive==countUsed);
}

void nbMemmapPrintFinalReport(STNBMemMap* map){
    NIX_PRINTF_INFO("-------------- MEM REPORT -----------\n");
    if(map->currCountAllocationsActive == 0){
        NIX_PRINTF_INFO("Nixtla: no memory leaking detected :)\n");
    } else {
        NIX_PRINTF_WARNING("WARNING, NIXTLA MEMORY-LEAK DETECTED! :(\n");
    }
    nbMemmapPrintActive(map);
    NIX_PRINTF_INFO("-------------------------------------\n");
}

#ifdef __cplusplus
} //extern "C" {
#endif
