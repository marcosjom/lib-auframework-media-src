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
    STNixEngineRef  nix;
    STNixSourceRef  source;
    STNixRecorderRef rec;
    //buffs
    struct {
        STNixBufferRef arr[NIX_DEMO_ECO_RECORD_SECS * NIX_DEMO_ECO_BUFFS_PER_SEC];
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

void bufferCapturedCallback(STNixEngineRef* eng, STNixRecorderRef* rec, const STNixAudioDesc audioDesc, const NixUI8* audioData, const NixUI32 audioDataBytes, const NixUI32 audioDataSamples, void* userdata);

//player

void bufferUnqueuedCallback(STNixSourceRef* src, STNixBufferRef* buffs, const NixUI32 buffsSz, void* userdata);

//main

int main(int argc, const char * argv[]){
    //
    nbMemmapInit(&memmap);
	//
    STNixDemoEcoState state;
    memset(&state, 0, sizeof(state));
    //
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
    //
    if(!NixEngine_isNull(state.nix)){
        NixEngine_printCaps(state.nix);
		//Source for stream eco (play the captured audio)
        state.source = NixEngine_sourceAlloc(state.nix);
		if(!NixSource_isNull(state.source)){
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
                STNixBufferRef buff = NixEngine_bufferAlloc(state.nix, &audioDesc, NULL, samplesPerBuffer * audioDesc.blockAlign);
                if(NixBuffer_isNull(buff)){
                    printf("nixBufferWithData failed.\n");
                    break;
                } else {
                    state.buffs.arr[state.buffs.use++] = buff;
                    NixBuffer_null(&buff);
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
                NixSource_setCallback(state.source, bufferUnqueuedCallback, &state);
                NixSource_setVolume(state.source, 1.0f);
                NixSource_play(state.source);
                //
                state.rec = NixEngine_recorderAlloc(state.nix, &audioDesc, buffersCount, samplesPerBuffer);
                if(NixRecorder_isNull(state.rec)){
                    printf("NixEngine_recorderAlloc failed.\n");
                } else {
                    NixRecorder_setCallback(state.rec, bufferCapturedCallback, &state);
                    if(!NixRecorder_start(state.rec)){
                        printf("nixCaptureStart failed.\n");
                    } else {
                        const NixUI32 msPerTick = 1000 / 30;
                        NixUI32 secsAccum = 0, msAccum = 0;
                        printf("Capturing and playing audio... switching every %d seconds.\n", NIX_DEMO_ECO_RECORD_SECS);
                        while(secsAccum < NIX_DEMO_ECO_SECS_TO_EXIT){ //Infinite loop, usually sync with your program main loop, or in a independent thread
                            NixEngine_tick(state.nix);
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
                        NixRecorder_stop(state.rec);
                    }
                    NixRecorder_release(&state.rec);
                    NixRecorder_null(&state.rec);
                }
            }
            NixSource_release(&state.source);
            NixSource_null(&state.source);
		}
        NixEngine_release(&state.nix);
        NixEngine_null(&state.nix);
	}
    //Memory report
    nbMemmapPrintFinalReport(&memmap);
    nbMemmapFinalize(&memmap);
    //
    return 0;
}

//recorder

void bufferCapturedCallback(STNixEngineRef* eng, STNixRecorderRef* rec, const STNixAudioDesc audioDesc, const NixUI8* audioData, const NixUI32 audioDataBytes, const NixUI32 audioDataSamples, void* userdata){
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
        STNixBufferRef buff = state->buffs.arr[state->buffs.iFilled];
        if(!NixBuffer_setData(buff, &audioDesc, audioData, audioDataBytes)){
            printf("bufferCapturedCallback, NixBuffer_setData failed\n");
        } else {
            if(audioDataBytes < state->buffs.bytesPerBuffer){
                //fill remainig space with zeroes
                if(!NixBuffer_fillWithZeroes(buff)){
                    printf("bufferCapturedCallback, NixBuffer_fillWithZeroes failed\n");
                }
            }
            ++state->buffs.iFilled;
        }
        //start playback
        if(state->buffs.iFilled >= state->buffs.use){
            printf("bufferCapturedCallback, stopping capture, feeding stream\n");
            NixRecorder_stop(state->rec);
            //
            state->buffs.iPlayed = 0;
            //queue all buffers
            {
                NixUI32 iBuff; for(iBuff = 0; iBuff < state->buffs.use; ++iBuff){
                    STNixBufferRef buff = state->buffs.arr[iBuff];
                    if(!NixSource_queueBuffer(state->source, buff)){
                        printf("bufferCapturedCallback, NixSource_queueBuffer failed.\n");
                    }
                }
            }
        }
    }
}

//player

void bufferUnqueuedCallback(STNixSourceRef* src, STNixBufferRef* buffs, const NixUI32 buffsSz, void* userdata){
    STNixDemoEcoState* state = (STNixDemoEcoState*)userdata;
    state->stats.curSec.buffsPlayedCount += buffsSz;
    //
    state->buffs.iPlayed += buffsSz;
    if(state->buffs.iPlayed >= state->buffs.use){
        printf("bufferCapturedCallback, stream played (hungry), starting capture\n");
        //
        state->buffs.iFilled = 0;
        if(!NixRecorder_start(state->rec)){
            printf("NixRecorder_start failed.\n");
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
