//
//  main.c
//  NixtlaDemo
//
//  Created by Marcos Ortega on 11/02/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#include <stdio.h>	//printf
#include <stdlib.h>	//malloc, free, rand
#include <time.h>   //time
#include <string.h> //memset

#if defined(_WIN32) || defined(WIN32)
	#include <windows.h> //Sleep
	#pragma comment(lib, "OpenAL32.lib") //link to openAL's lib
	#define DEMO_SLEEP_MILLISEC(MS) Sleep(MS)
#else
	#include <unistd.h> //sleep, usleep
	#define DEMO_SLEEP_MILLISEC(MS) usleep((MS) * 1000);
#endif

#include "nixtla-audio.h"
#include "testMemMap.h"
#include "../utils/utilFilesList.h"
#include "../utils/utilLoadWav.h"

#define NIX_DEMO_PLAY_SECS_TO_EXIT    10    //stop the demo after this time

// Custom memory allocation for this test,
// for detecting memory-leaks using a STNBMemMap.
STNBMemMap memmap;
//
void* NBMemMap_custom_malloc(const NixUI32 newSz, const char* dbgHintStr);
void* NBMemMap_custom_realloc(void* ptr, const NixUI32 newSz);
void NBMemMap_custom_free(void* ptr);

int main(int argc, const char * argv[]){
    nbMemmapInit(&memmap);
    //
    srand((unsigned int)time(NULL));
    //
    STNixApiEngineRef nix = STNixApiEngineRef_Zero;
    //init engine
    {
        STNixContextItf ctxItf;
        memset(&ctxItf, 0, sizeof(ctxItf));
        //define context interface
        {
            //custom memory allocation (for memory leaks dtection)
            {
                ctxItf.mem.malloc = NBMemMap_custom_malloc;
                ctxItf.mem.realloc = NBMemMap_custom_realloc;
                ctxItf.mem.free = NBMemMap_custom_free;
            }
            //use default for others
            NixContextItf_fillMissingMembers(&ctxItf);
        }
        //allocate a context
        STNixContextRef ctx = NixContext_alloc(&ctxItf);
        {
            //get the API interface
            STNixApiItf apiItf;
            if(!NixApiItf_getDefaultForCurrentOS(&apiItf)){
                printf("ERROR, NixApiItf_getDefaultForCurrentOS failed.\n");
            } else {
                //create engine
                nix = NixApiEngine_alloc(ctx, &apiItf);
                if(nix.ptr == NULL){
                    printf("ERROR, api.engine.alloc failed.\n");
                } else {
                    
                }
            }
        }
        //context is retained by the engine
        NixContext_release(&ctx);
        NixContext_null(&ctx);
    }
    //execute
    if(nix.ptr != NULL){
        STNixApiSourceRef src = STNixApiSourceRef_Zero;
        const char* strWavPath = _nixUtilFilesList[rand() % (sizeof(_nixUtilFilesList) / sizeof(_nixUtilFilesList[0]))];
        NixUI8* audioData = NULL;
        NixUI32 audioDataBytes = 0;
        STNixAudioDesc audioDesc;
        if(!loadDataFromWavFile(strWavPath, &audioDesc, &audioData, &audioDataBytes)){
            printf("ERROR, loading WAV file: '%s'.\n", strWavPath);
        } else {
            printf("WAV file loaded: '%s'.\n", strWavPath);
            src = NixApiEngine_sourceAlloc(nix);
            if(src.ptr == NULL){
                printf("ERROR, api.source.alloc failed.\n");
            } else {
                STNixApiBufferRef buff = NixApiEngine_bufferAlloc(nix, &audioDesc, audioData, audioDataBytes);
                if(buff.ptr == NULL){
                    printf("ERROR, api.buffer.alloc failed.\n");
                } else {
                    if(!NixApiSource_setBuffer(src, buff)){
                        printf("ERROR, api.source.setBuffer failed.\n");
                    } else {
                        NixApiSource_setRepeat(src, NIX_TRUE);
                        NixApiSource_setVolume(src, 1.0f);
                        NixApiSource_play(src);
                    }
                    //Buffer is already retained by the source
                    NixApiBuffer_release(&buff);
                }
            }
        }
        //wav samples already loaded into buffer
        if(audioData != NULL){
            free(audioData);
            audioData = NULL;
        }
        //
        //Infinite loop, usually sync with your program main loop, or in a independent thread
        //
        if(src.ptr != NULL){
            const NixUI32 msPerTick = 1000 / 30;
            NixUI32 secsAccum = 0;
            NixUI32 msAccum = 0;
            while(secsAccum < NIX_DEMO_PLAY_SECS_TO_EXIT){
                NixApiEngine_tick(nix);
                DEMO_SLEEP_MILLISEC(msPerTick); //30 ticks per second for this demo
                msAccum += msPerTick;
                if(msAccum >= 1000){
                    secsAccum += (msAccum / 1000);
                    printf("%u/%u secs playing\n", secsAccum, NIX_DEMO_PLAY_SECS_TO_EXIT);
                    msAccum %= 1000;
                }
            }
        }
        //
        NixApiSource_release(&src);
        NixApiEngine_release(&nix);
    }
    //Memory report
    nbMemmapPrintFinalReport(&memmap);
    nbMemmapFinalize(&memmap);
    //
    return 0;
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
