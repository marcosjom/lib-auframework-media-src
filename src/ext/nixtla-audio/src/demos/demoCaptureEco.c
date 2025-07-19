//
//  main.c
//  NixtlaDemo
//
//  Created by Marcos Ortega on 11/02/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#include <stdio.h>	//printf
#include <string.h> //memset
#include <stdlib.h> //malloc

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

#define NIX_DEMO_ECO_SECS_TO_EXIT   60
#define NIX_DEMO_ECO_RECORD_SECS    5
#define NIX_DEMO_ECO_BUFFS_PER_SEC  10

// Custom memory allocation for this test,
// for detecting memory-leaks using a STNBMemMap.
STNBMemMap memmap;
void* NBMemMap_custom_malloc(const NixUI32 newSz, const char* dbgHintStr);
void* NBMemMap_custom_realloc(void* ptr, const NixUI32 newSz);
void NBMemMap_custom_free(void* ptr);

typedef struct STNixDemoEcoState_ {
    STNix_Engine    nix;
    NixUI16         iSourceStrm;
    //buffs
    struct {
        NixUI16     arr[NIX_DEMO_ECO_RECORD_SECS * NIX_DEMO_ECO_BUFFS_PER_SEC];
        NixUI32     use;
        NixUI32     iFilled;
        NixUI32     iPlayed;
        NixUI32     bytesPerBuffer;
        NixUI32     samplesPerBuffer;
    } buffs;
    //stats
    struct {
        //curSec
        struct {
            NixUI64     samplesRecordedCount;
            NixUI64     buffsPlayedCount;
            //
            NixUI64     samplesRecSum;
            NixUI64     samplesRecCount;
        } curSec;
    } stats;
} STNixDemoEcoState;

//recorder

void bufferCapturedCallback(STNix_Engine* eng, void* userdata, const STNixAudioDesc audioDesc, const NixUI8* audioData, const NixUI32 audioDataBytes, const NixUI32 audioDataSamples);

//player

void bufferUnqueuedCallback(STNix_Engine* engAbs, void* userdata, const NixUI32 sourceIndex, const NixUI16 buffersUnqueuedCount);

//main

int main(int argc, const char * argv[]){
    //
    nbMemmapInit(&memmap);
	//
    STNixDemoEcoState state;
    memset(&state, 0, sizeof(state));
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
    const NixUI16 ammPregeneratedSources = 0;
	if(nixInit(ctx, &state.nix, ammPregeneratedSources)){
		nixPrintCaps(&state.nix);
		//Source for stream eco (play the captured audio)
        const NixUI8 lookIntoReusable   = NIX_TRUE;
        const NixUI8 audioGroupIndex    = 0;
        PTRNIX_SourceReleaseCallback releaseCallBack = NULL;
        void* releaseCallBackUserData   = NULL;
        const NixUI16 buffsQueueSize    = 4;
        PTRNIX_StreamBufferUnqueuedCallback bufferUnqueueCallback = bufferUnqueuedCallback;
        void* bufferUnqueueCallbackData = &state;
        state.iSourceStrm = nixSourceAssignStream(&state.nix, lookIntoReusable, audioGroupIndex, releaseCallBack, releaseCallBackUserData, buffsQueueSize,bufferUnqueueCallback, bufferUnqueueCallbackData);
		if(state.iSourceStrm != 0){
            STNixAudioDesc audioDesc   = STNixAudioDesc_Zero;
            audioDesc.samplesFormat     = ENNixSampleFmt_Int;
            audioDesc.channels          = 1;
            audioDesc.bitsPerSample     = 16;
            audioDesc.samplerate        = 22050;
            audioDesc.blockAlign        = (audioDesc.bitsPerSample / 8) * audioDesc.channels;
            const NixUI16 buffersCount  = (sizeof(state.buffs.arr) / sizeof(state.buffs.arr[0]));
            const NixUI16 samplesPerBuffer = (audioDesc.samplerate / NIX_DEMO_ECO_BUFFS_PER_SEC);
            //allocate buffers
            while(state.buffs.use < (sizeof(state.buffs.arr) / sizeof(state.buffs.arr[0]))){
                NixUI16 iBuffer = nixBufferWithData(&state.nix, &audioDesc, NULL, samplesPerBuffer * audioDesc.blockAlign);
                if(iBuffer == 0){
                    printf("nixBufferWithData failed.\n");
                    break;
                } else {
                    state.buffs.arr[state.buffs.use++] = iBuffer;
                    iBuffer = 0;
                }
            }
            //
            if(state.buffs.use <= 0){
                printf("No stream-buffer could be created\n");
            } else {
                //init the capture
                state.buffs.iFilled = 0;
                state.buffs.iPlayed = state.buffs.use;
                state.buffs.bytesPerBuffer = samplesPerBuffer * audioDesc.blockAlign;
                state.buffs.samplesPerBuffer = samplesPerBuffer;
                //
                nixSourceSetVolume(&state.nix, state.iSourceStrm, 1.0f);
                nixSourcePlay(&state.nix, state.iSourceStrm);
                //
                if(!nixCaptureInit(&state.nix, &audioDesc, buffersCount, samplesPerBuffer, &bufferCapturedCallback, &state)){
                    printf("nixCaptureInit failed.\n");
                } else {
                    if(!nixCaptureStart(&state.nix)){
                        printf("nixCaptureStart failed.\n");
                    } else {
                        const NixUI32 msPerTick = 1000 / 30;
                        NixUI32 secsAccum = 0, msAccum = 0;
                        printf("Capturing and playing audio... switching every %d seconds.\n", NIX_DEMO_ECO_RECORD_SECS);
                        while(secsAccum < NIX_DEMO_ECO_SECS_TO_EXIT){ //Infinite loop, usually sync with your program main loop, or in a independent thread
                            nixTick(&state.nix);
                            DEMO_SLEEP_MILLISEC(msPerTick); //30 ticks per second for this demo
                            msAccum += msPerTick;
                            if(msAccum >= 1000){
                                secsAccum += (msAccum / 1000);
                                if(state.stats.curSec.samplesRecordedCount > 0){
                                    printf("%u/%u secs running, %llu samples-recorded/sec, %llu buffs-played/sec (recorded avg-value: %d)\n", secsAccum, NIX_DEMO_ECO_SECS_TO_EXIT, state.stats.curSec.samplesRecordedCount, state.stats.curSec.buffsPlayedCount, (NixSI32)(state.stats.curSec.samplesRecSum / state.stats.curSec.samplesRecordedCount));
                                } else {
                                    printf("%u/%u secs running, %llu samples-recorded/sec, %llu buffs-played/sec\n", secsAccum, NIX_DEMO_ECO_SECS_TO_EXIT, state.stats.curSec.samplesRecordedCount, state.stats.curSec.buffsPlayedCount);
                                }
                                state.stats.curSec.samplesRecordedCount = 0;
                                state.stats.curSec.buffsPlayedCount = 0;
                                msAccum %= 1000;
                            }
                        }
                        nixCaptureStop(&state.nix);
                    }
                    nixCaptureFinalize(&state.nix);
                }
            }
			nixSourceRelease(&state.nix, state.iSourceStrm);
		}
		nixFinalize(&state.nix);
	}
    //
    NixContext_release(&ctx);
    NixContext_null(&ctx);
    //Memory report
    nbMemmapPrintFinalReport(&memmap);
    nbMemmapFinalize(&memmap);
    //
    return 0;
}

//recorder

void bufferCapturedCallback(STNix_Engine* eng, void* userdata, const STNixAudioDesc audioDesc, const NixUI8* audioData, const NixUI32 audioDataBytes, const NixUI32 audioDataSamples){
    STNixDemoEcoState* state = (STNixDemoEcoState*)userdata;
    state->stats.curSec.samplesRecordedCount += audioDataSamples;
    //calculate avg (for dbg and stats)
    if(audioDesc.bitsPerSample == 16 && audioDesc.samplesFormat == ENNixSampleFmt_Int){
        const NixSI16* s16 = (const NixSI16*)audioData;
        const NixSI16* s16AfterEnd = s16 + (audioDataBytes / 2);
        state->stats.curSec.samplesRecordedCount += (s16AfterEnd - s16);
        while(s16 < s16AfterEnd){
            state->stats.curSec.samplesRecSum += *s16;
            ++s16;
        }
    }
    //fill buffer
    if(state->buffs.iFilled >= state->buffs.use){
        //ignore samples, probably is the flush after stopping the recording
    } else {
        //fill samples into a buffer
        NixUI16 iBuffer = state->buffs.arr[state->buffs.iFilled];
        if(!nixBufferSetData(eng, iBuffer, &audioDesc, audioData, audioDataBytes)){
            printf("bufferCapturedCallback, nixBufferSetData failed for iSource(%d)\n", state->iSourceStrm);
        } else {
            if(audioDataBytes < state->buffs.bytesPerBuffer){
                //fill remainig space with zeroes
                if(!nixBufferFillZeroes(eng, iBuffer)){
                    printf("bufferCapturedCallback, nixBufferFillZeroes failed for iSource(%d)\n", state->iSourceStrm);
                }
            }
            ++state->buffs.iFilled;
        }
        //start playback
        if(state->buffs.iFilled >= state->buffs.use){
            printf("bufferCapturedCallback, stopping capture, feeding stream\n");
            nixCaptureStop(&state->nix);
            //
            state->buffs.iPlayed = 0;
            //queue all buffers
            {
                NixUI32 iBuff; for(iBuff = 0; iBuff < state->buffs.use; ++iBuff){
                    NixUI16 iBuffer = state->buffs.arr[iBuff];
                    if(!nixSourceStreamAppendBuffer(eng, state->iSourceStrm, iBuffer)){
                        printf("bufferCapturedCallback, ERROR attaching new buffer buffer(%d) to source(%d)\n", iBuffer, state->iSourceStrm);
                    }
                }
            }
        }
    }
}

//player

void bufferUnqueuedCallback(STNix_Engine* engAbs, void* userdata, const NixUI32 sourceIndex, const NixUI16 buffersUnqueuedCount){
    STNixDemoEcoState* state = (STNixDemoEcoState*)userdata;
    state->stats.curSec.buffsPlayedCount += buffersUnqueuedCount;
    //
    state->buffs.iPlayed += buffersUnqueuedCount;
    if(state->buffs.iPlayed >= state->buffs.use){
        printf("bufferCapturedCallback, stream played (hungry), starting capture\n");
        //
        state->buffs.iFilled = 0;
        if(!nixCaptureStart(&state->nix)){
            printf("nixCaptureStart failed.\n");
        }
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
