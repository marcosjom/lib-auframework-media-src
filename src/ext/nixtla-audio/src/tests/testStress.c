//
//  main.c
//  nixtla-audio-demos
//
//  Created by Marcos Ortega on 24/02/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#include <stdio.h>		//printf
#include <stdlib.h>		//malloc, free

#if defined(_WIN32) || defined(WIN32)
	#include <windows.h> //Sleep
	#define DEMO_SLEEP_MILLISEC(MS) Sleep(MS)
	#pragma comment(lib, "OpenAL32.lib") //link to openAL's lib
#else
	#include <unistd.h> //sleep, usleep
	#define DEMO_SLEEP_MILLISEC(MS) usleep((MS) * 1000);
#endif

#include "nixtla-audio.h"
#include "testMemMap.h"

#define NIX_TEXT_STRESS_SECS_TO_EXIT    60

// Custom memory allocation for this test,
// for detecting memory-leaks using a STNBMemMap.
STNBMemMap memmap;
void* NBMemMap_custom_malloc(const NixUI32 newSz, const char* dbgHintStr);
void* NBMemMap_custom_realloc(void* ptr, const NixUI32 newSz);
void NBMemMap_custom_free(void* ptr);

#define NIX_MALLOC(POINTER_DEST, POINTER_TYPE, SIZE_BYTES, STR_HINT) \
	{ \
		POINTER_DEST = (POINTER_TYPE*)malloc(SIZE_BYTES); \
		nbMemmapRegister(&memmap, POINTER_DEST, SIZE_BYTES, STR_HINT); \
	}
#define NIX_FREE(POINTER)	\
	{ \
		free(POINTER); \
		nbMemmapUnregister(&memmap, POINTER); \
	}

//In this test, we include the "source" file to implement customized memory allocation.
//The "nixtla-audio.c" file is not part of the project tree.

typedef struct STTestSound {
	STNixAudioDesc audioDesc;
	NixBOOL			isStatic;	//NIX_FALSE for static sound
	NixBOOL			isPlaying;
	NixFLOAT		volume;
};

void bufferCapturedCallback(STNix_Engine* nix, void* userdata, const STNixAudioDesc audioDesc, const NixUI8* audioData, const NixUI32 audioDataBytes, const NixUI32 audioDataSamples);

int main(int argc, const char * argv[]) {
    nbMemmapInit(&memmap);
    //
    srand((unsigned int)time(NULL));
    //
    //Test Itf filling missing members (missing code?)
    {
        STNixApiItf itf;
        memset(&itf, 0, sizeof(itf));
        NixApiItf_fillMissingMembers(&itf); //it ASSERTS internally
        //validate missing implementations
        {
            void** ptr = (void**)&itf;
            void** afterEnd = ptr + (sizeof(itf) / sizeof(void*));
            while(ptr < afterEnd){
                if(*ptr == NULL){ //function pointer should be defined
                    printf("ERROR, missing function pointer after 'NixApiItf_fillMissingMembers()'!.\n");
                    exit(-1);
                }
                ptr++;
            }
        }
    }
    //
    STNixContextItf ctxItf;
    memset(&ctxItf, 0, sizeof(ctxItf));
    //custom memory allocation (for memory leaks dtection)
    ctxItf.mem.malloc = NBMemMap_custom_malloc;
    ctxItf.mem.realloc = NBMemMap_custom_realloc;
    ctxItf.mem.free = NBMemMap_custom_free;
    //use default for others
    NixContextItf_fillMissingMembers(&ctxItf);
    STNixContextRef ctx = NixContext_alloc(&ctxItf);
    //
	STNix_Engine nix;
	if(nixInit(ctx, &nix, 8)){
		STNixAudioDesc audioDesc; NixUI16 iSourceStrm;
		nixPrintCaps(&nix);
		//Load and play wav file
		NixUI16 iSourceWav = 0; NixUI16 iBufferWav = 0;
		loadAndPlayWav(&nix, "./res/audioTest.wav", &iSourceWav, &iBufferWav);
		//Source for stream eco (play the captured audio)
		iSourceStrm = nixSourceAssignStream(&nix, 1, 0, NULL, NULL, 4, NULL, NULL);
		if(iSourceStrm != 0){
			nixSourcePlay(&nix, iSourceStrm);
			//Init the capture
			audioDesc._canales			= 1;
			audioDesc._bitsPorMuestra	= 16;
			audioDesc._frecuencia		= 22050;
			audioDesc._blockAlign		= (audioDesc._bitsPorMuestra/8) * audioDesc._canales;
			if(nixCaptureInit(&nix, &audioDesc, 15, audioDesc._frecuencia/10, &bufferCapturedCallback, &iSourceStrm)){
				if(nixCaptureStart(&nix)){
                    const NixUI32 msPerTick = 1000 / 30;
                    NixUI32 secsAccum = 0, msAccum = 0;
					printf("Capturing and playing audio...\n");
					while(secsAccum < NIX_TEXT_STRESS_SECS_TO_EXIT){ //capture and play for 5 secodns
						nixTick(&nix);
						DEMO_SLEEP_MILLISEC(msPerTick); //30 ticks per second for this demo
                        msAccum += msPerTick;
                        if(msAccum >= 1000){
                            secsAccum += (msAccum / 1000);
                            printf("%u/%u secs testing\n", secsAccum, NIX_TEXT_CONVERSION_SECS_TO_EXIT);
                            msAccum %= 1000;
                        }
					}
					nixCaptureStop(&nix);
				}
				nixCaptureFinalize(&nix);
			}
			nixSourceRelease(&nix, iSourceStrm);
		}
		nixFinalize(&nix);
	}
    //
    NixContext_release(&ctx);
    NixContext_null(&ctx);
	//Memory report
    nbMemmapPrintFinalReport(&memmap);
	nbMemmapFinalize(&memmap);
	//
	{
		char c;
		printf("Press ENTER to exit.");
		scanf("%c", &c);
	}
    return 0;
}

void bufferCapturedCallback(STNix_Engine* nix, void* userdata, const STNixAudioDesc audioDesc, const NixUI8* audioData, const NixUI32 audioDataBytes, const NixUI32 audioDataSamples){
	const NixUI16 iSource = *((NixUI16*)userdata);
	const NixUI16 iBuffer = nixBufferWithData(nix, &audioDesc, audioData, audioDataBytes);
	if(iBuffer == 0){
		printf("bufferCapturedCallback, nixBufferWithData failed for iSource(%d)\n", iSource);
	} else {
		if(nixSourceStreamAppendBuffer(nix, iSource, iBuffer)){
			//PRINTF_INFO("bufferCapturedCallback, source(%d) new buffer(%d) attached\n", iSource, iBuffer);
		} else {
			printf("bufferCapturedCallback, ERROR attaching new buffer buffer(%d) to source(%d)\n", iBuffer, iSource);
		}
		nixBufferRelease(nix, iBuffer);
	}
}

//custom memory allocation (for memory leaks detection)

void* NBMemMap_custom_malloc(const NixUI32 newSz, const char* dbgHintStr){
    void* r = malloc(newSz);
    nbMemmapRegister(&memmap, r, newSz, dbgHintStr);
    return r;
}

void* NBMemMap_custom_realloc(void* ptr, const NixUI32 newSz){
    void* r = realloc(ptr, newSz);
    nbMemmapUnregister(&memmap, ptr);
    nbMemmapRegister(&memmap, r, newSz, "NBMemMap_custom_realloc");
    return r;
}

void NBMemMap_custom_free(void* ptr){
    nbMemmapUnregister(&memmap, ptr);
    free(ptr);
}
