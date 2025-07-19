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

#define NIX_DEMO_STREAM_SECS_TO_EXIT    10    //stop the demo after this time

// Custom memory allocation for this test,
// for detecting memory-leaks using a STNBMemMap.
STNBMemMap memmap;
void* NBMemMap_custom_malloc(const NixUI32 newSz, const char* dbgHintStr);
void* NBMemMap_custom_realloc(void* ptr, const NixUI32 newSz);
void NBMemMap_custom_free(void* ptr);

typedef struct STDemoStreamState_ {
    STNixEngineRef  nix;
    STNixSourceRef  source;
    STNixBufferRef  buffsWav[10];
    NixUI32         buffsUse;
    NixUI32         iOldestBuffQueued;
    //stats
    struct {
        NixUI32     buffQueued;
        NixUI32     buffUnqueued;
    } stats;
} STDemoStreamState;

//callback

void demoStreamBufferUnqueuedCallback_(STNixSourceRef* src, STNixBufferRef* buffs, const NixUI32 buffsSz, void* userData);

int main(int argc, const char * argv[]){
    //
    nbMemmapInit(&memmap);
    //
    STDemoStreamState state;
    memset(&state, 0, sizeof(state));
    //
    srand((unsigned int)time(NULL));
    //
    {
        //init engine
        {
            STNixContextItf ctxItf;
            memset(&ctxItf, 0, sizeof(ctxItf));
            //define context interface
            {
                //custom memory allocation (for memory leaks dtection)
                {
                    ctxItf.mem.malloc   = NBMemMap_custom_malloc;
                    ctxItf.mem.realloc  = NBMemMap_custom_realloc;
                    ctxItf.mem.free     = NBMemMap_custom_free;
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
                    state.nix = NixEngine_alloc(ctx, &apiItf);
                    if(NixEngine_isNull(state.nix)){
                        printf("ERROR, NixEngine_alloc failed.\n");
                    }
                }
            }
            //context is retained by the engine
            NixContext_release(&ctx);
            NixContext_null(&ctx);
        }
    }
    //
	if(!NixEngine_isNull(state.nix)){
        NixEngine_printCaps(state.nix);
        const NixUI32 ammBuffs = (sizeof(state.buffsWav) / sizeof(state.buffsWav[0])); //ammount of buffers for stream
        //randomly select a wav from the list
		const char* strWavPath = _nixUtilFilesList[rand() % (sizeof(_nixUtilFilesList) / sizeof(_nixUtilFilesList[0]))];
        NixUI8* audioData = NULL;
        NixUI32 audioDataBytes = 0;
        STNixAudioDesc audioDesc;
		if(!loadDataFromWavFile(strWavPath, &audioDesc, &audioData, &audioDataBytes)){
			printf("ERROR, loading WAV file: '%s'.\n", strWavPath);
		} else {
			printf("WAV file loaded: '%s'.\n", strWavPath);
            state.source = NixEngine_sourceAlloc(state.nix);
			if(NixSource_isNull(state.source)){
				printf("NixEngine_sourceAlloc failed.\n");
			} else {
                printf("NixEngine_sourceAlloc ok.\n");
                NixSource_setCallback(state.source, demoStreamBufferUnqueuedCallback_, &state);
                const NixUI32 samplesWav = (audioDataBytes / audioDesc.blockAlign); //ammount of samples available
                const NixUI32 samplesPerBuff = (samplesWav / (ammBuffs - 1)); //-1, last buffer could be ignored or partially populated
                NixUI32 iSample = 0;
                while(iSample < samplesWav && state.buffsUse < ammBuffs){
                    void* samplesPtr = &audioData[iSample * audioDesc.blockAlign];
                    NixUI32 samplesToUse = (samplesWav - iSample);
                    if(samplesToUse > samplesPerBuff){
                        samplesToUse = samplesPerBuff;
                    }
                    if(samplesToUse <= 0){
                        //no more samples to add
                        break;
                    } else {
                        state.buffsWav[state.buffsUse] = NixEngine_bufferAlloc(state.nix, &audioDesc, samplesPtr, samplesToUse * audioDesc.blockAlign);
                        if(NixBuffer_isNull(state.buffsWav[state.buffsUse])){
                            printf("Buffer assign failed.\n");
                            break;
                        } else {
                            printf("Buffer loaded with data and retained.\n");
                            if(!NixSource_queueBuffer(state.source, state.buffsWav[state.buffsUse])){
                                printf("Buffer-to-source queueing failed.\n");
                                break;
                            } else {
                                state.stats.buffQueued++;
                                iSample += samplesToUse;
                                state.buffsUse++;
                            }
                        }
                    }
                }
                //start stream
                NixSource_setVolume(state.source, 1.0f);
                NixSource_play(state.source);
			}
		}
        //wav samples already loaded into buffers
        if(audioData != NULL){
            free(audioData);
            audioData = NULL;
        }
		//
		//Infinite loop, usually sync with your program main loop, or in a independent thread
		//
        if(!NixSource_isNull(state.source)){
            const NixUI32 msPerTick = 1000 / 30;
            NixUI32 secsAccum = 0;
            NixUI32 msAccum = 0;
            while(secsAccum < NIX_DEMO_STREAM_SECS_TO_EXIT){
                NixEngine_tick(state.nix);
                DEMO_SLEEP_MILLISEC(msPerTick); //30 ticks per second for this demo
                msAccum += msPerTick;
                if(msAccum >= 1000){
                    secsAccum += (msAccum / 1000);
                    printf("%u/%u secs playing (%d buffs enqueued, %d dequeued)\n", secsAccum, NIX_DEMO_STREAM_SECS_TO_EXIT, state.stats.buffQueued, state.stats.buffUnqueued);
                    state.stats.buffQueued = 0;
                    state.stats.buffUnqueued = 0;
                    msAccum %= 1000;
                }
            }
        }
		//
        NixSource_release(&state.source);
        NixSource_null(&state.source);
        NixEngine_release(&state.nix);
        NixEngine_null(&state.nix);
	}
    //Memory report
    nbMemmapPrintFinalReport(&memmap);
    nbMemmapFinalize(&memmap);
    //
    return 0;
}

//callback

void demoStreamBufferUnqueuedCallback_(STNixSourceRef* src, STNixBufferRef* buffs, const NixUI32 buffsSz, void* userData){
    STDemoStreamState* state = (STDemoStreamState*)userData;
    //re-enqueue buffers
    NixUI16 buffToReEnq = buffsSz;
    while(buffToReEnq > 0){
        STNixBufferRef buff = state->buffsWav[state->iOldestBuffQueued % state->buffsUse];
        state->iOldestBuffQueued = (state->iOldestBuffQueued + 1) % state->buffsUse;
        if(!NixSource_queueBuffer(state->source, buff)){
            printf("NixSource_queueBuffer failed.\n");
        } else {
            state->stats.buffQueued++;
        }
        state->stats.buffUnqueued++;
        --buffToReEnq;
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
