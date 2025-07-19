//
//  main.c
//  nixtla-audio-demos
//
//  Created by Marcos Ortega on 24/02/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#include <stdio.h>		//printf
#include <stdlib.h>		//malloc, free
#include <string.h>     //memset
#include <time.h>       //time

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
#include "../utils/utilFilesList.h"
#include "../utils/utilLoadWav.h"

#define NIX_TEXT_LEAK_SECS_TO_EXIT   15

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

void loadAndPlayWav(STNix_Engine* nix, const char* strWavPath, NixUI16* iSourceWav, NixUI16* iBufferWav);
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
        //randomly select a wav from the list
        const char* strWavPath = _nixUtilFilesList[rand() % (sizeof(_nixUtilFilesList) / sizeof(_nixUtilFilesList[0]))];
		loadAndPlayWav(&nix, strWavPath, &iSourceWav, &iBufferWav);
		//Source for stream eco (play the captured audio)
		iSourceStrm = nixSourceAssignStream(&nix, 1, 0, NULL, NULL, 4, NULL, NULL);
		if(iSourceStrm != 0){
			nixSourcePlay(&nix, iSourceStrm);
			//Init the capture
			audioDesc.samplesFormat		= ENNixSampleFmt_Int;
			audioDesc.channels			= 1;
			audioDesc.bitsPerSample		= 16;
			audioDesc.samplerate		= 22050;
			audioDesc.blockAlign		= (audioDesc.bitsPerSample/8) * audioDesc.channels;
			if(nixCaptureInit(&nix, &audioDesc, 15, audioDesc.samplerate/10, &bufferCapturedCallback, &iSourceStrm)){
				if(nixCaptureStart(&nix)){
					const NixUI32 msPerTick = (1000 / 30);
                    NixUI32 secsAccum = 0, msAccum = 0;
					printf("Capturing and playing audio...\n");
					while(secsAccum < NIX_TEXT_LEAK_SECS_TO_EXIT){ //capture and play for 5 secodns
						nixTick(&nix);
						DEMO_SLEEP_MILLISEC(msPerTick); //30 ticks per second for this demo
                        msAccum += msPerTick;
                        if(msAccum >= 1000){
                            secsAccum += (msAccum / 1000);
                            printf("%d/%d secs running.\n", secsAccum, NIX_TEXT_LEAK_SECS_TO_EXIT);
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

void loadAndPlayWav(STNix_Engine* nix, const char* strWavPath, NixUI16* p_iSourceWav, NixUI16* p_iBufferWav){
	STNixAudioDesc audioDesc;
	NixUI16 iSourceWav = 0, iBufferWav = 0;
	NixUI8* audioData = NULL;
    NixUI32 audioDataBytes = 0;
	if(!loadDataFromWavFile(strWavPath, &audioDesc, &audioData, &audioDataBytes)){
		printf("ERROR, loading WAV file: '%s'.\n", strWavPath);
	} else {
		printf("WAV file loaded: '%s'.\n", strWavPath);
		iSourceWav = nixSourceAssignStatic(nix, 1, 0, NULL, NULL);
		if(iSourceWav == 0){
			printf("Source assign failed.\n");
		} else {
			printf("Source(%d) assigned and retained.\n", iSourceWav);
			iBufferWav = nixBufferWithData(nix, &audioDesc, audioData, audioDataBytes);
			if(iBufferWav == 0){
				printf("Buffer assign failed.\n");
			} else {
				printf("Buffer(%d) loaded with data and retained.\n", iBufferWav);
				if(!nixSourceSetBuffer(nix, iSourceWav, iBufferWav)){
					printf("Buffer-to-source linking failed.\n");
				} else {
					printf("Buffer(%d) linked with source(%d).\n", iBufferWav, iSourceWav);
					nixSourceSetRepeat(nix, iSourceWav, NIX_TRUE);
					nixSourceSetVolume(nix, iSourceWav, 1.0f);
					nixSourcePlay(nix, iSourceWav);
				}
			}
		}
	}
	if(audioData!=NULL) free(audioData); audioData = NULL;
	*p_iSourceWav = iSourceWav;
	*p_iBufferWav = iBufferWav;
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
