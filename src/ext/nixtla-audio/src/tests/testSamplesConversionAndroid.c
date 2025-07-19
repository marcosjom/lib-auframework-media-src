//
//  main.c
//  nixtla-audio-demos
//
//  Created by Marcos Ortega on 10/07/25.
//  Copyright (c) 2014 Marcos Ortega. All rights reserved.
//

#include <stdio.h>  //printf
#include <stdlib.h> //malloc, free, rand
#include <time.h>   //time
#include <string.h> //memset

#include <jni.h>
#include <android/log.h>    //for __android_log_print()

#include "nixtla-audio.h"
#include "testMemMap.h"
#include "../utils/utilFilesList.h"
#include "../utils/utilLoadWav.h"

// Custom memory allocation for this test,
// for detecting memory-leaks using a STNBMemMap.
STNBMemMap memmap;
void* NBMemMap_custom_malloc(const NixUI32 newSz, const char* dbgHintStr);
void* NBMemMap_custom_realloc(void* ptr, const NixUI32 newSz);
void NBMemMap_custom_free(void* ptr);

#ifndef PRINTF_INFO
#   define PRINTF_INFO(STR_FMT, ...)    __android_log_print(ANDROID_LOG_INFO, "Nixtla", "INFO, "STR_FMT, ##__VA_ARGS__)
#endif
#ifndef PRINTF_ERROR
#   define PRINTF_ERROR(STR_FMT, ...)   __android_log_print(ANDROID_LOG_ERROR, "Nixtla", "ERROR, "STR_FMT, ##__VA_ARGS__)
#endif
#ifndef PRINTF_WARNING
#   define PRINTF_WARNING(STR_FMT, ...) __android_log_print(ANDROID_LOG_WARN, "Nixtla", "WARNING, "STR_FMT, ##__VA_ARGS__)
#endif

static STNix_Engine nix;
static NixUI32 secsAccum = 0;
static NixUI32 msAccum = 0;
//original source
static NixUI16 iSourceOrg = 0, iSourceOrgPlayCount = 0;
static NixUI8* audioData = NULL;
static NixUI32 audioDataBytes = 0;
static STNixAudioDesc audioDesc;
//converted source
static NixUI16 iSourceConv = 0, iSourceConvPlayCount = 0;
static NixUI8* audioDataConv = NULL;
static NixUI32 audioDataBytesConv = 0;
static STNixAudioDesc audioDescConv;

JNIEXPORT jboolean JNICALL Java_com_mortegam_nixtla_1audio_MainActivity_testStart(JNIEnv* env, jobject obj, jobject assetManager){
    jboolean r = 0;
    PRINTF_INFO("native-testStart\n");
    //
    nbMemmapInit(&memmap);
    //
    srand((unsigned int)time(NULL));
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
    if(nixInit(ctx, &nix, 8)){
        NixEngine_printCaps(nix);
        //randomly select a wav from the list
        const char* strWavPath = _nixUtilFilesList[rand() % (sizeof(_nixUtilFilesList) / sizeof(_nixUtilFilesList[0]))];
        if(!loadDataFromWavFile(env, assetManager, strWavPath, &audioDesc, &audioData, &audioDataBytes)){
            PRINTF_ERROR("ERROR, loading WAV file: '%s'.\n", strWavPath);
        } else {
            PRINTF_INFO("WAV file loaded: '%s'.\n", strWavPath);
            NixUI16 iSrcTmp = nixSourceAssignStatic(&nix, NIX_TRUE, 0, NULL, NULL);
            if(iSrcTmp == 0){
                PRINTF_ERROR("Source assign failed.\n");
            } else {
                NixUI16 iBufferWav = nixBufferWithData(&nix, &audioDesc, audioData, audioDataBytes);
                if(iBufferWav == 0){
                    PRINTF_ERROR("Buffer assign failed.\n");
                } else {
                    if(!nixSourceSetBuffer(&nix, iSrcTmp, iBufferWav)){
                        PRINTF_ERROR("Buffer-to-source linking failed.\n");
                    } else {
                        PRINTF_INFO("origin playing %u time(s).\n", iSourceOrgPlayCount + 1);
                        //retain source
                        iSourceOrg = iSrcTmp;
                        nixSourceRetain(&nix, iSrcTmp);
                        //start
                        nixSourceSetVolume(&nix, iSrcTmp, 1.0f);
                        nixSourcePlay(&nix, iSrcTmp);
                        //
                        r = 1;
                    }
                    //release buffer (already retained by source if success)
                    nixBufferRelease(&nix, iBufferWav);
                    iBufferWav = 0;
                }
                //release source
                nixSourceRelease(&nix, iSrcTmp);
                iSrcTmp = 0;
            }
        }
    }
    //
    NixContext_release(&ctx);
    NixContext_null(&ctx);
    return r;
}

JNIEXPORT void JNICALL Java_com_mortegam_nixtla_1audio_MainActivity_testEnd(JNIEnv* env, jobject obj){
    PRINTF_INFO("native-testEnd\n");
    if(audioData!=NULL){
        free(audioData);
        audioData = NULL;
    }
    if(audioDataConv != NULL){
        free(audioDataConv);
        audioDataConv = NULL;
    }
    if(iSourceOrg != 0){
        nixSourceRelease(&nix, iSourceOrg);
        iSourceOrg = 0;
    }
    if(iSourceConv != 0){
        nixSourceRelease(&nix, iSourceConv);
        iSourceConv = 0;
    }
    nixFinalize(&nix);
    //Memory report
    nbMemmapPrintFinalReport(&memmap);
    nbMemmapFinalize(&memmap);
}

JNIEXPORT void JNICALL Java_com_mortegam_nixtla_1audio_MainActivity_testTick(JNIEnv* env, jobject obj){
    if(iSourceOrg > 0){
        const NixUI32 msPerTick = 1000 / 30;
        nixTick(&nix);
        msAccum += msPerTick;
        if(msAccum >= 1000){
            secsAccum += (msAccum / 1000);
            //PRINTF_INFO("%u secs playing\n", secsAccum);
            msAccum %= 1000;
        }
        //analyze converted playback
        if(iSourceConv != 0){
            if(!nixSourceIsPlaying(&nix, iSourceConv)){
                iSourceConvPlayCount++;
                if(iSourceConvPlayCount < 2){
                    //play again
                    PRINTF_INFO("converted played %u time(s).\n", iSourceConvPlayCount + 1);
                    nixSourcePlay(&nix, iSourceConv);
                } else {
                    iSourceConvPlayCount = 0;
                    //release
                    nixSourceRelease(&nix, iSourceConv);
                    iSourceConv = 0;
                    //play original
                    iSourceOrgPlayCount = 0;
                    PRINTF_INFO("origin playing %u time(s).\n", iSourceOrgPlayCount + 1);
                    nixSourcePlay(&nix, iSourceOrg);
                }
            }
        } else {
            //analyze original playback
            if(!nixSourceIsPlaying(&nix, iSourceOrg)){
                iSourceOrgPlayCount++;
                if(iSourceOrgPlayCount < 2){
                    //play again
                    PRINTF_INFO("origin playing %u time(s).\n", iSourceOrgPlayCount + 1);
                    nixSourcePlay(&nix, iSourceOrg);
                } else {
                    iSourceOrgPlayCount = 0;
                    //create random conversion
                    if(iSourceConv != 0){
                        nixSourceRelease(&nix, iSourceConv);
                        iSourceConv = 0;
                    }
                    switch(rand() % 4){
                        case 0:
                            audioDescConv.samplesFormat = ENNixSampleFmt_Int;
                            audioDescConv.bitsPerSample = 8;
                            break;
                        case 1:
                            audioDescConv.samplesFormat = ENNixSampleFmt_Int;
                            audioDescConv.bitsPerSample = 16;
                            break;
                        case 2:
                            audioDescConv.samplesFormat = ENNixSampleFmt_Int;
                            audioDescConv.bitsPerSample = 32;
                            break;
                        default:
                            audioDescConv.bitsPerSample = 32;
                            audioDescConv.samplesFormat = ENNixSampleFmt_Float;
                            break;
                    }
                    audioDescConv.channels = 1 + (rand() % 2);
                    audioDescConv.samplerate = (rand() % 10) == 0 ? audioDesc.samplerate : 800 + (rand() % 92000);
                    audioDescConv.blockAlign = (audioDescConv.bitsPerSample / 8) * audioDescConv.channels;
                    const NixUI32 samplesReq = NixFmtConverter_samplesForNewFrequency(audioDataBytes / audioDesc.blockAlign, audioDesc.samplerate, audioDescConv.samplerate);
                    const NixUI32 bytesReq = samplesReq * audioDescConv.blockAlign;
                    if(audioDataBytesConv < bytesReq){
                        if(audioDataConv != NULL){
                            free(audioDataConv);
                            audioDataConv = NULL;
                        }
                        audioDataBytesConv = 0;
                        audioDataConv = (NixUI8*)malloc(bytesReq);
                        if(audioDataConv != NULL){
                            audioDataBytesConv = bytesReq;
                            PRINTF_INFO("Conversion buffer resized to %u bytes.\n", audioDataBytesConv);
                        }
                    }
                    if(audioDataConv != NULL){
                        void* conv = NixFmtConverter_alloc();
                        if(conv != NULL){
                            const NixUI32 srcBlocks = (audioDataBytes / audioDesc.blockAlign);
                            const NixUI32 dstBlocks = samplesReq;
                            NixUI32 ammBlocksRead = 0;
                            NixUI32 ammBlocksWritten = 0;
                            if(!NixFmtConverter_prepare(conv, &audioDesc, &audioDescConv)){
                                PRINTF_ERROR("NixFmtConverter_prepare failed.\n");
                            } else if(!NixFmtConverter_setPtrAtSrcInterlaced(conv, &audioDesc, audioData, 0)){
                                PRINTF_ERROR("NixFmtConverter_setPtrAtSrcInterlaced failed.\n");
                            } else if(!NixFmtConverter_setPtrAtDstInterlaced(conv, &audioDescConv, audioDataConv, 0)){
                                PRINTF_ERROR("NixFmtConverter_setPtrAtDstInterlaced failed.\n");
                            } else if(!NixFmtConverter_convert(conv, srcBlocks, dstBlocks, &ammBlocksRead, &ammBlocksWritten)){
                                PRINTF_ERROR("NixFmtConverter_convert failed.\n");
                            } else {
                                PRINTF_INFO("NixFmtConverter_convert transformed %u of %u samples (%u%%) (%u hz, %d bits, %d channels) to %u of %u samples(%u%%) (%u hz, %d bits, %d channels, %s).\n", ammBlocksRead, srcBlocks, ammBlocksRead * 100 / srcBlocks, audioDesc.samplerate, audioDesc.bitsPerSample, audioDesc.channels, ammBlocksWritten, dstBlocks, ammBlocksWritten * 100 / dstBlocks, audioDescConv.samplerate, audioDescConv.bitsPerSample, audioDescConv.channels, audioDescConv.samplesFormat == ENNixSampleFmt_Float ? "float" : "int");
                                //iSourceConv
                                NixUI16 iSrcConv = nixSourceAssignStatic(&nix, NIX_TRUE, 0, NULL, NULL);
                                if(iSrcConv == 0){
                                    PRINTF_ERROR("Source assign failed.\n");
                                } else {
                                    NixUI16 iBufferWav = nixBufferWithData(&nix, &audioDescConv, audioDataConv, ammBlocksWritten * audioDescConv.blockAlign);
                                    if(iBufferWav == 0){
                                        PRINTF_ERROR("Buffer assign failed.\n");
                                    } else {
                                        if(!nixSourceSetBuffer(&nix, iSrcConv, iBufferWav)){
                                            PRINTF_ERROR("Buffer-to-source linking failed.\n");
                                        } else {
                                            iSourceConvPlayCount = 0;
                                            //assign as new source
                                            {
                                                //retain new source
                                                nixSourceRetain(&nix, iSrcConv);
                                                //release previous source
                                                if(iSourceConv != 0){
                                                    nixSourceRelease(&nix, iSourceConv);
                                                    iSourceConv = 0;
                                                }
                                                //assign
                                                iSourceConv = iSrcConv;
                                            }
                                            //
                                            PRINTF_INFO("converted played %u time(s).\n", iSourceConvPlayCount + 1);
                                            nixSourceSetVolume(&nix, iSrcConv, 1.0f);
                                            nixSourcePlay(&nix, iSrcConv);
                                        }
                                        //release buffer (already retained by source if success)
                                        nixBufferRelease(&nix, iBufferWav);
                                        iBufferWav = 0;
                                    }
                                    nixSourceRelease(&nix, iSrcConv);
                                    iSrcConv = 0;
                                }
                            }
                            NixFmtConverter_free(conv);
                            conv = NULL;
                        }
                    }
                }
            }
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
