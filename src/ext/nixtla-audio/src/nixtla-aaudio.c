//
//  nixtla-aaudio.c
//  NixtlaAudioLib
//
//  Created by Marcos Ortega on 12/07/25.
//  Copyright (c) 2025 Marcos Ortega. All rights reserved.
//
//  This entire notice must be retained in this source code.
//  This source code is under MIT Licence.
//
//  This software is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
//
//  Latest fixes enhancements and documentation at https://github.com/marcosjom/lib-nixtla-audio
//

//
//This file adds support for AAudio for playing and recording in Android 8+ (API 26+, Oreo)
//

#include "nixtla-audio-private.h"
#include "nixtla-audio.h"
#include "nixtla-aaudio.h"
#include <string.h> //for memset()

#ifdef __ANDROID__
#   include <aaudio/AAudio.h>
#endif

//------
//API Itf
//------

//AAudio interface (Android 8.0+, Oreo, API level 26+)

//Engine
STNixApiEngine  nixAAudioEngine_create(void);
void            nixAAudioEngine_destroy(STNixApiEngine obj);
void            nixAAudioEngine_printCaps(STNixApiEngine obj);
NixBOOL         nixAAudioEngine_ctxIsActive(STNixApiEngine obj);
NixBOOL         nixAAudioEngine_ctxActivate(STNixApiEngine obj);
NixBOOL         nixAAudioEngine_ctxDeactivate(STNixApiEngine obj);
void            nixAAudioEngine_tick(STNixApiEngine obj);
//PCMBuffer
STNixApiBuffer  nixAAudioPCMBuffer_create(const STNix_audioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes);
void            nixAAudioPCMBuffer_destroy(STNixApiBuffer obj);
NixBOOL         nixAAudioPCMBuffer_setData(STNixApiBuffer obj, const STNix_audioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes);
NixBOOL         nixAAudioPCMBuffer_fillWithZeroes(STNixApiBuffer obj);
//Source
STNixApiSource  nixAAudioSource_create(STNixApiEngine eng);
void            nixAAudioSource_destroy(STNixApiSource obj);
void            nixAAudioSource_setCallback(STNixApiSource obj, void (*callback)(void* pEng, const NixUI32 sourceIndex, const NixUI32 ammBuffs), void* callbackEng, NixUI32 callbackSourceIndex);
NixBOOL         nixAAudioSource_setVolume(STNixApiSource obj, const float vol);
NixBOOL         nixAAudioSource_setRepeat(STNixApiSource obj, const NixBOOL isRepeat);
void            nixAAudioSource_play(STNixApiSource obj);
void            nixAAudioSource_pause(STNixApiSource obj);
void            nixAAudioSource_stop(STNixApiSource obj);
NixBOOL         nixAAudioSource_isPlaying(STNixApiSource obj);
NixBOOL         nixAAudioSource_isPaused(STNixApiSource obj);
NixBOOL         nixAAudioSource_setBuffer(STNixApiSource obj, STNixApiBuffer buff);  //static-source
NixBOOL         nixAAudioSource_queueBuffer(STNixApiSource obj, STNixApiBuffer buff); //stream-source
//Recorder
STNixApiRecorder nixAAudioRecorder_create(STNixApiEngine eng, const STNix_audioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer);
void            nixAAudioRecorder_destroy(STNixApiRecorder obj);
NixBOOL         nixAAudioRecorder_setCallback(STNixApiRecorder obj, NixApiCaptureBufferFilledCallback callback, void* callbackData);
NixBOOL         nixAAudioRecorder_start(STNixApiRecorder obj);
NixBOOL         nixAAudioRecorder_stop(STNixApiRecorder obj);


NixBOOL nixAAudioEngine_getApiItf(STNixApiItf* dst){
    NixBOOL r = NIX_FALSE;
    if(dst != NULL){
        memset(dst, 0, sizeof(*dst));
        dst->engine.create      = nixAAudioEngine_create;
        dst->engine.destroy     = nixAAudioEngine_destroy;
        dst->engine.printCaps   = nixAAudioEngine_printCaps;
        dst->engine.ctxIsActive = nixAAudioEngine_ctxIsActive;
        dst->engine.ctxActivate = nixAAudioEngine_ctxActivate;
        dst->engine.ctxDeactivate = nixAAudioEngine_ctxDeactivate;
        dst->engine.tick        = nixAAudioEngine_tick;
        //PCMBuffer
        dst->buffer.create      = nixAAudioPCMBuffer_create;
        dst->buffer.destroy     = nixAAudioPCMBuffer_destroy;
        dst->buffer.setData     = nixAAudioPCMBuffer_setData;
        dst->buffer.fillWithZeroes = nixAAudioPCMBuffer_fillWithZeroes;
        //Source
        dst->source.create      = nixAAudioSource_create;
        dst->source.destroy     = nixAAudioSource_destroy;
        dst->source.setCallback = nixAAudioSource_setCallback;
        dst->source.setVolume   = nixAAudioSource_setVolume;
        dst->source.setRepeat   = nixAAudioSource_setRepeat;
        dst->source.play        = nixAAudioSource_play;
        dst->source.pause       = nixAAudioSource_pause;
        dst->source.stop        = nixAAudioSource_stop;
        dst->source.isPlaying   = nixAAudioSource_isPlaying;
        dst->source.isPaused    = nixAAudioSource_isPaused;
        dst->source.setBuffer   = nixAAudioSource_setBuffer;  //static-source
        dst->source.queueBuffer = nixAAudioSource_queueBuffer; //stream-source
        //Recorder
        dst->recorder.create    = nixAAudioRecorder_create;
        dst->recorder.destroy   = nixAAudioRecorder_destroy;
        dst->recorder.setCallback = nixAAudioRecorder_setCallback;
        dst->recorder.start     = nixAAudioRecorder_start;
        dst->recorder.stop      = nixAAudioRecorder_stop;
        //
        r = NIX_TRUE;
    }
    return r;
}


// Macros to allow compiling code outside of Android;
// just to use my prefered IDE before testing on Android.
#ifndef __ANDROID__
#   define aaudio_allowed_capture_policy_t              int
#   define aaudio_channel_mask_t                        unsigned int
#   define aaudio_content_type_t                        int
#   define aaudio_data_callback_result_t                int
#   define aaudio_direction_t                           int
#   define aaudio_format_t                              int
#   define aaudio_input_preset_t                        int
#   define aaudio_performance_mode_t                    int
#   define aaudio_result_t                              int
#   define aaudio_session_id_t                          int
#   define aaudio_sharing_mode_t                        int
#   define aaudio_spatialization_behavior_t             int
#   define aaudio_stream_state_t                        int
#   define aaudio_usage_t                               int
//
#   define AAudioStreamBuilder                          unsigned int
#   define AAudio_createStreamBuilder(PTR)              AAUDIO_OK; *(PTR) = (AAudioStreamBuilder*)malloc(sizeof(AAudioStreamBuilder))
#   define AAudioStreamBuilder_delete(B)                free(B)
#   define AAudioStreamBuilder_setDataCallback(B, C, D)
#   define AAudioStreamBuilder_setSampleRate(B, D)
#   define AAudioStreamBuilder_setDeviceId(B, D);
#   define AAudioStreamBuilder_setDirection(B, D);
#   define AAudioStreamBuilder_setSharingMode(B, D);
#   define AAudioStreamBuilder_setChannelCount(B, D);
#   define AAudioStreamBuilder_setFormat(B, D);
#   define AAudioStreamBuilder_setBufferCapacityInFrames(B, D);
#   define AAudioStreamBuilder_setDataCallback(B, C, D)
#   define AAudioStreamBuilder_setErrorCallback(B, C, D)
#   define AAudioStreamBuilder_openStream(B, S)         AAUDIO_OK; *(S) = (AAudioStream*)malloc(sizeof(AAudioStream))
//
#   define AAudioStream                                 unsigned int
#   define AAudioStream_getState(S)                     AAUDIO_STREAM_STATE_CLOSED
#   define AAudioStream_requestStart(S)                 AAUDIO_OK
#   define AAudioStream_requestPause(S)                 AAUDIO_OK
#   define AAudioStream_requestStop(S)                  AAUDIO_OK
#   define AAudioStream_requestFlush(S)                 AAUDIO_OK
#   define AAudioStream_close(S)                        AAUDIO_OK
#   define AAudioStream_getDirection(S)                 AAUDIO_DIRECTION_OUTPUT
#   define AAudioStream_getSharingMode(S)               AAUDIO_SHARING_MODE_SHARED
#   define AAudioStream_getSampleRate(S)                44100
#   define AAudioStream_getChannelCount(S)              2
#   define AAudioStream_getFormat(S)                    AAUDIO_FORMAT_PCM_I16
//
#   define AAudio_convertResultToText(C)                ""
//
    enum AAUDIO_DIRECTION {
     AAUDIO_DIRECTION_OUTPUT,
     AAUDIO_DIRECTION_INPUT
    };
    enum AAUDIO_SHARING_MODE {
      AAUDIO_SHARING_MODE_EXCLUSIVE,
      AAUDIO_SHARING_MODE_SHARED
    };
    enum AAUDIO_FORMAT {
      AAUDIO_FORMAT_INVALID = -1,
      AAUDIO_FORMAT_UNSPECIFIED = 0,
      AAUDIO_FORMAT_PCM_I16,
      AAUDIO_FORMAT_PCM_FLOAT,
      AAUDIO_FORMAT_PCM_I24_PACKED,
      AAUDIO_FORMAT_PCM_I32,
      AAUDIO_FORMAT_IEC61937
    };
    enum AAUDIO_CALLBACK_RESULT {
      AAUDIO_CALLBACK_RESULT_CONTINUE = 0,
      AAUDIO_CALLBACK_RESULT_STOP
    };
    enum AUDIO_RESULT {
      AAUDIO_OK,
      AAUDIO_ERROR_BASE = -900,
      AAUDIO_ERROR_DISCONNECTED,
      AAUDIO_ERROR_ILLEGAL_ARGUMENT,
      AAUDIO_ERROR_INTERNAL = AAUDIO_ERROR_ILLEGAL_ARGUMENT + 2,
      AAUDIO_ERROR_INVALID_STATE,
      AAUDIO_ERROR_INVALID_HANDLE = AAUDIO_ERROR_INVALID_STATE + 3,
      AAUDIO_ERROR_UNIMPLEMENTED = AAUDIO_ERROR_INVALID_HANDLE + 2,
      AAUDIO_ERROR_UNAVAILABLE,
      AAUDIO_ERROR_NO_FREE_HANDLES,
      AAUDIO_ERROR_NO_MEMORY,
      AAUDIO_ERROR_NULL,
      AAUDIO_ERROR_TIMEOUT,
      AAUDIO_ERROR_WOULD_BLOCK,
      AAUDIO_ERROR_INVALID_FORMAT,
      AAUDIO_ERROR_OUT_OF_RANGE,
      AAUDIO_ERROR_NO_SERVICE,
      AAUDIO_ERROR_INVALID_RATE
    };
    enum AAUDIO_STREAM_STATE {
      AAUDIO_STREAM_STATE_UNINITIALIZED = 0,
      AAUDIO_STREAM_STATE_UNKNOWN,
      AAUDIO_STREAM_STATE_OPEN,
      AAUDIO_STREAM_STATE_STARTING,
      AAUDIO_STREAM_STATE_STARTED,
      AAUDIO_STREAM_STATE_PAUSING,
      AAUDIO_STREAM_STATE_PAUSED,
      AAUDIO_STREAM_STATE_FLUSHING,
      AAUDIO_STREAM_STATE_FLUSHED,
      AAUDIO_STREAM_STATE_STOPPING,
      AAUDIO_STREAM_STATE_STOPPED,
      AAUDIO_STREAM_STATE_CLOSING,
      AAUDIO_STREAM_STATE_CLOSED,
      AAUDIO_STREAM_STATE_DISCONNECTED
    };
#endif

struct STNix_AAudioEngine_;
struct STNix_AAudioSource_;
struct STNix_AAudioSourceCallback_;
struct STNix_AAudioQueue_;
struct STNix_AAudioQueuePair_;
struct STNix_AAudioSrcNotif_;
struct STNix_AAudioNotifQueue_;
struct STNix_AAudioRecorder_;

//------
//Engine
//------

typedef struct STNix_AAudioEngine_ {
    //srcs
    struct {
        NIX_MUTEX_T                 mutex;
        struct STNix_AAudioSource_** arr;
        NixUI32                     use;
        NixUI32                     sz;
        NixUI32 changingStateCountHint;
    } srcs;
    struct STNix_AAudioRecorder_* rec;
} STNix_AAudioEngine;

void Nix_AAudioEngine_init(STNix_AAudioEngine* obj);
void Nix_AAudioEngine_destroy(STNix_AAudioEngine* obj);
NixBOOL Nix_AAudioEngine_srcsAdd(STNix_AAudioEngine* obj, struct STNix_AAudioSource_* src);
void Nix_AAudioEngine_tick(STNix_AAudioEngine* obj, const NixBOOL isFinalCleanup);

//------
//Notif
//------

typedef struct STNix_AAudioSourceCallback_ {
    void            (*func)(void* pEng, const NixUI32 sourceIndex, const NixUI32 ammBuffs);
    void*           eng;
    NixUI32         sourceIndex;
} STNix_AAudioSourceCallback;

typedef struct STNix_AAudioSrcNotif_ {
    STNix_AAudioSourceCallback callback;
    NixUI32 ammBuffs;
} STNix_AAudioSrcNotif;

void Nix_AAudioSrcNotif_init(STNix_AAudioSrcNotif* obj);
void Nix_AAudioSrcNotif_destroy(STNix_AAudioSrcNotif* obj);

//------
//NotifQueue
//------

typedef struct STNix_AAudioNotifQueue_ {
    STNix_AAudioSrcNotif*  arr;
    NixUI32                use;
    NixUI32                sz;
    STNix_AAudioSrcNotif  arrEmbedded[32];
} STNix_AAudioNotifQueue;

void Nix_AAudioNotifQueue_init(STNix_AAudioNotifQueue* obj);
void Nix_AAudioNotifQueue_destroy(STNix_AAudioNotifQueue* obj);
//
NixBOOL Nix_AAudioNotifQueue_push(STNix_AAudioNotifQueue* obj, STNix_AAudioSrcNotif* pair);

//------
//QueuePair (Buffers)
//------

typedef struct STNix_AAudioQueuePair_ {
    STNixPCMBuffer*  org;    //original buffer (owned by the user)
    STNixPCMBuffer*  cnv;    //converted buffer (owned by the source)
} STNix_AAudioQueuePair;

void Nix_AAudioQueuePair_init(STNix_AAudioQueuePair* obj);
void Nix_AAudioQueuePair_destroy(STNix_AAudioQueuePair* obj);

//------
//Queue (Buffers)
//------

typedef struct STNix_AAudioQueue_ {
    STNix_AAudioQueuePair*  arr;
    NixUI32                 use;
    NixUI32                 sz;
} STNix_AAudioQueue;

void Nix_AAudioQueue_init(STNix_AAudioQueue* obj);
void Nix_AAudioQueue_destroy(STNix_AAudioQueue* obj);
//
NixBOOL Nix_AAudioQueue_flush(STNix_AAudioQueue* obj, const NixBOOL nullifyOrgs);
NixBOOL Nix_AAudioQueue_prepareForSz(STNix_AAudioQueue* obj, const NixUI32 minSz);
NixBOOL Nix_AAudioQueue_pushOwning(STNix_AAudioQueue* obj, STNix_AAudioQueuePair* pair);
NixBOOL Nix_AAudioQueue_popOrphaning(STNix_AAudioQueue* obj, STNix_AAudioQueuePair* dst);
NixBOOL Nix_AAudioQueue_popMovingTo(STNix_AAudioQueue* obj, STNix_AAudioQueue* other);

//------
//Source
//------

#define NIX_AAudioSource_BIT_isStatic   (0x1 << 0)  //source expects only one buffer, repeats or pauses after playing it
#define NIX_AAudioSource_BIT_isChanging (0x1 << 1)  //source is changing state after a call to request*()
#define NIX_AAudioSource_BIT_isRepeat   (0x1 << 2)
#define NIX_AAudioSource_BIT_isPlaying  (0x1 << 3)
#define NIX_AAudioSource_BIT_isPaused   (0x1 << 4)
#define NIX_AAudioSource_BIT_isClosing  (0x1 << 5)
#define NIX_AAudioSource_BIT_isOrphan   (0x1 << 6)  //source is waiting for close(), wait for the change of state and Nix_AAudioSource_release + NIX_FREE.

typedef struct STNix_AAudioSource_ {
    struct STNix_AAudioEngine_* eng;    //parent engine
    STNix_audioDesc         buffsFmt;   //first attached buffers' format (defines the converter config)
    STNix_audioDesc         srcFmt;
    AAudioStream*           src;
    //queues
    struct {
        NIX_MUTEX_T         mutex;
        void*               conv;   //nixFmtConverter
        STNix_AAudioSourceCallback callback;
        STNix_AAudioQueue   notify; //buffers (consumed, pending to notify)
        STNix_AAudioQueue   reuse;  //buffers (conversion buffers)
        STNix_AAudioQueue   pend;   //to be played/filled
        NixUI32             pendSampleIdx;  //current sample playing/filling
    } queues;
    //props
    float                   volume;
    NixUI8                  stateBits;  //packed bools to reduce padding, NIX_AAudioSource_BIT_
} STNix_AAudioSource;

void Nix_AAudioSource_init(STNix_AAudioSource* obj);
void Nix_AAudioSource_destroy(STNix_AAudioSource* obj);
NixBOOL Nix_AAudioSource_queueBufferForOutput(STNix_AAudioSource* obj, STNixPCMBuffer* buff);
NixUI32 Nix_AAudioSource_feedSamplesTo(STNix_AAudioSource* obj, void* dst, const NixUI32 samplesMax, NixBOOL* dstExplicitStop);
NixBOOL Nix_AAudioSource_pendPopOldestBuffLocked_(STNix_AAudioSource* obj);
NixBOOL Nix_AAudioSource_pendMoveAllBuffsToNotifyWithoutPoppingLocked_(STNix_AAudioSource* obj);

//
#define Nix_AAudioSource_isStatic(OBJ)          (((OBJ)->stateBits & NIX_AAudioSource_BIT_isStatic) != 0)
#define Nix_AAudioSource_isChanging(OBJ)        (((OBJ)->stateBits & NIX_AAudioSource_BIT_isChanging) != 0)
#define Nix_AAudioSource_isRepeat(OBJ)          (((OBJ)->stateBits & NIX_AAudioSource_BIT_isRepeat) != 0)
#define Nix_AAudioSource_isPlaying(OBJ)         (((OBJ)->stateBits & NIX_AAudioSource_BIT_isPlaying) != 0)
#define Nix_AAudioSource_isPaused(OBJ)          (((OBJ)->stateBits & NIX_AAudioSource_BIT_isPaused) != 0)
#define Nix_AAudioSource_isClosing(OBJ)         (((OBJ)->stateBits & NIX_AAudioSource_BIT_isClosing) != 0)
#define Nix_AAudioSource_isOrphan(OBJ)          (((OBJ)->stateBits & NIX_AAudioSource_BIT_isOrphan) != 0)
//
#define Nix_AAudioSource_setIsStatic(OBJ, V)    (OBJ)->stateBits = (V ? (OBJ)->stateBits | NIX_AAudioSource_BIT_isStatic : (OBJ)->stateBits & ~NIX_AAudioSource_BIT_isStatic)
#define Nix_AAudioSource_setIsChanging(OBJ, V)  (OBJ)->stateBits = (V ? (OBJ)->stateBits | NIX_AAudioSource_BIT_isChanging : (OBJ)->stateBits & ~NIX_AAudioSource_BIT_isChanging)
#define Nix_AAudioSource_setIsRepeat(OBJ, V)    (OBJ)->stateBits = (V ? (OBJ)->stateBits | NIX_AAudioSource_BIT_isRepeat : (OBJ)->stateBits & ~NIX_AAudioSource_BIT_isRepeat)
#define Nix_AAudioSource_setIsPlaying(OBJ, V)   (OBJ)->stateBits = (V ? (OBJ)->stateBits | NIX_AAudioSource_BIT_isPlaying : (OBJ)->stateBits & ~NIX_AAudioSource_BIT_isPlaying)
#define Nix_AAudioSource_setIsPaused(OBJ, V)    (OBJ)->stateBits = (V ? (OBJ)->stateBits | NIX_AAudioSource_BIT_isPaused : (OBJ)->stateBits & ~NIX_AAudioSource_BIT_isPaused)
#define Nix_AAudioSource_setIsClosing(OBJ)      (OBJ)->stateBits = ((OBJ)->stateBits | NIX_AAudioSource_BIT_isClosing)
#define Nix_AAudioSource_setIsOrphan(OBJ)       (OBJ)->stateBits = ((OBJ)->stateBits | NIX_AAudioSource_BIT_isOrphan)

//------
//Recorder
//------

typedef struct STNix_AAudioRecorder_ {
    NixBOOL                 engStarted;
    STNix_AAudioEngine*     engNx;
    AAudioStream*           rec;
    STNix_audioDesc         capFmt;
    //callback
    struct {
        NixApiCaptureBufferFilledCallback func;
        void*               data;
    } callback;
    //cfg
    struct {
        STNix_audioDesc     fmt;
        NixUI16             samplesPerBuffer;
        NixUI16             maxBuffers;
    } cfg;
    //queues
    struct {
        NIX_MUTEX_T         mutex;
        void*               conv;   //nixFmtConverter
        STNix_AAudioQueue   notify;
        STNix_AAudioQueue   reuse;
        //filling
        struct {
            NixSI32         iCurSample; //at first buffer in 'reuse'
        } filling;
    } queues;
} STNix_AAudioRecorder;

void Nix_AAudioRecorder_init(STNix_AAudioRecorder* obj);
void Nix_AAudioRecorder_destroy(STNix_AAudioRecorder* obj);
//
NixBOOL Nix_AAudioRecorder_prepare(STNix_AAudioRecorder* obj, const STNix_audioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer);
NixBOOL Nix_AAudioRecorder_setCallback(STNix_AAudioRecorder* obj, NixApiCaptureBufferFilledCallback callback, void* callbackData);
NixBOOL Nix_AAudioRecorder_start(STNix_AAudioRecorder* obj);
NixBOOL Nix_AAudioRecorder_stop(STNix_AAudioRecorder* obj);
NixBOOL Nix_AAudioRecorder_flush(STNix_AAudioRecorder* obj);
void Nix_AAudioRecorder_consumeInputBuffer(STNix_AAudioRecorder* obj, void* audioData, const NixSI32 numFrames);
void Nix_AAudioRecorder_notifyBuffers(STNix_AAudioRecorder* obj);

//------
//Engine
//------

void Nix_AAudioEngine_init(STNix_AAudioEngine* obj){
    memset(obj, 0, sizeof(STNix_AAudioEngine));
    //srcs
    {
        NIX_MUTEX_INIT(&obj->srcs.mutex);
    }
}

   
void Nix_AAudioEngine_destroy(STNix_AAudioEngine* obj){
    //srcs
    {
        //cleanup
        while(obj->srcs.arr != NULL && obj->srcs.use > 0){
            Nix_AAudioEngine_tick(obj, NIX_TRUE);
        }
        //
        if(obj->srcs.arr != NULL){
            NIX_FREE(obj->srcs.arr);
            obj->srcs.arr = NULL;
        }
        NIX_MUTEX_DESTROY(&obj->srcs.mutex);
    }
    //rec (recorder)
    if(obj->rec != NULL){
        obj->rec = NULL;
    }
}

NixBOOL Nix_AAudioEngine_srcsAdd(STNix_AAudioEngine* obj, struct STNix_AAudioSource_* src){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL){
        NIX_MUTEX_LOCK(&obj->srcs.mutex);
        {
            //resize array (if necesary)
            if(obj->srcs.use >= obj->srcs.sz){
                const NixUI32 szN = obj->srcs.use + 4;
                STNix_AAudioSource** arrN = NULL;
                NIX_MALLOC(arrN, STNix_AAudioSource*, sizeof(STNix_AAudioSource*) * szN, "STNix_AAudioEngine::srcsN");
                if(arrN != NULL){
                    if(obj->srcs.arr != NULL){
                        if(obj->srcs.use > 0){
                            memcpy(arrN, obj->srcs.arr, sizeof(arrN[0]) * obj->srcs.use);
                        }
                        NIX_FREE(obj->srcs.arr);
                        obj->srcs.arr = NULL;
                    }
                    obj->srcs.arr = arrN;
                    obj->srcs.sz = szN;
                }
            }
            //add
            if(obj->srcs.use >= obj->srcs.sz){
                NIX_PRINTF_ERROR("nixAAudioSource_create::STNix_AAudioEngine::srcs failed (no allocated space).\n");
            } else {
                //become the owner of the pair
                obj->srcs.arr[obj->srcs.use++] = src;
                r = NIX_TRUE;
            }
        }
        NIX_MUTEX_UNLOCK(&obj->srcs.mutex);
    }
    return r;
}

void Nix_AAudioEngine_removeSrcRecordLocked_(STNix_AAudioEngine* obj, NixSI32* idx){
    STNix_AAudioSource* src = obj->srcs.arr[*idx];
    if(src != NULL){
        Nix_AAudioSource_destroy(src);
        NIX_FREE(src);
    }
    //fill gap
    --obj->srcs.use;
    {
        NixUI32 i2;
        for(i2 = *idx; i2 < obj->srcs.use; ++i2){
            obj->srcs.arr[i2] = obj->srcs.arr[i2 + 1];
        }
    }
    *idx = *idx - 1; //process record again
}

void Nix_AAudioEngine_tick_addQueueNotifSrcLocked_(STNix_AAudioNotifQueue* notifs, STNix_AAudioSource* srcLocked){
    if(srcLocked->queues.notify.use > 0){
        const NixBOOL nullifyOrgs = NIX_TRUE;
        STNix_AAudioSrcNotif n;
        Nix_AAudioSrcNotif_init(&n);
        n.callback = srcLocked->queues.callback;
        n.ammBuffs = srcLocked->queues.notify.use;
        if(!Nix_AAudioQueue_flush(&srcLocked->queues.notify, nullifyOrgs)){
            NIX_ASSERT(NIX_FALSE); //program logic error
        }
        if(!Nix_AAudioNotifQueue_push(notifs, &n)){
            NIX_ASSERT(NIX_FALSE); //program logic error
            Nix_AAudioSrcNotif_destroy(&n);
        }
    }
}
    
void Nix_AAudioEngine_tick(STNix_AAudioEngine* obj, const NixBOOL isFinalCleanup){
    if(obj != NULL){
        //srcs
        {
            STNix_AAudioNotifQueue notifs;
            Nix_AAudioNotifQueue_init(&notifs);
            NIX_MUTEX_LOCK(&obj->srcs.mutex);
            if(obj->srcs.arr != NULL && obj->srcs.use > 0){
                NixUI32 changingStateCount = 0;
                NixSI32 i; for(i = 0; i < (NixSI32)obj->srcs.use; ++i){
                    STNix_AAudioSource* src = obj->srcs.arr[i];
                    if(src->src == NULL && Nix_AAudioSource_isOrphan(src)){
                        //remove
                        Nix_AAudioEngine_removeSrcRecordLocked_(obj, &i);
                        src = NULL;
                    } else {
                        aaudio_stream_state_t state = AAUDIO_STREAM_STATE_UNKNOWN;
                        if(Nix_AAudioSource_isChanging(src) || Nix_AAudioSource_isOrphan(src) || isFinalCleanup){
                            aaudio_stream_state_t state = AAudioStream_getState(src->src);
                            switch (state) {
                                case AAUDIO_STREAM_STATE_STARTING:
                                case AAUDIO_STREAM_STATE_PAUSING:
                                case AAUDIO_STREAM_STATE_FLUSHING:
                                case AAUDIO_STREAM_STATE_STOPPING:
                                case AAUDIO_STREAM_STATE_CLOSING:
                                    ++changingStateCount;
                                    break;
                                case AAUDIO_STREAM_STATE_FLUSHED:
                                case AAUDIO_STREAM_STATE_STOPPED:
                                    Nix_AAudioSource_setIsPlaying(src, NIX_FALSE);
                                    Nix_AAudioSource_setIsPaused(src, NIX_FALSE);
                                    Nix_AAudioSource_setIsChanging(src, NIX_FALSE);
                                    //move all pending buffers to notify
                                    NIX_MUTEX_LOCK(&src->queues.mutex);
                                    {
                                        Nix_AAudioSource_pendMoveAllBuffsToNotifyWithoutPoppingLocked_(src);
                                    }
                                    NIX_MUTEX_UNLOCK(&src->queues.mutex);
                                    break;
                                case AAUDIO_STREAM_STATE_STARTED:
                                    Nix_AAudioSource_setIsPlaying(src, NIX_TRUE);
                                    Nix_AAudioSource_setIsPaused(src, NIX_FALSE);
                                    Nix_AAudioSource_setIsChanging(src, NIX_FALSE);
                                    break;
                                case AAUDIO_STREAM_STATE_PAUSED:
                                    Nix_AAudioSource_setIsPlaying(src, NIX_TRUE);
                                    Nix_AAudioSource_setIsPaused(src, NIX_TRUE);
                                    Nix_AAudioSource_setIsChanging(src, NIX_FALSE);
                                    break;
                                case AAUDIO_STREAM_STATE_CLOSED:
                                    Nix_AAudioSource_setIsPlaying(src, NIX_FALSE);
                                    Nix_AAudioSource_setIsPaused(src, NIX_FALSE);
                                    Nix_AAudioSource_setIsChanging(src, NIX_FALSE);
                                    //move all pending buffers to notify
                                    NIX_MUTEX_LOCK(&src->queues.mutex);
                                    {
                                        Nix_AAudioSource_pendMoveAllBuffsToNotifyWithoutPoppingLocked_(src);
                                        //add notif before removing
                                        Nix_AAudioEngine_tick_addQueueNotifSrcLocked_(&notifs, src);
                                    }
                                    NIX_MUTEX_UNLOCK(&src->queues.mutex);
                                    //release and remove
                                    Nix_AAudioEngine_removeSrcRecordLocked_(obj, &i);
                                    src = NULL;
                                    break;
                                default:
                                    //AAUDIO_STREAM_STATE_UNINITIALIZED
                                    //AAUDIO_STREAM_STATE_UNKNOWN
                                    //AAUDIO_STREAM_STATE_OPEN
                                    //
                                    //AAUDIO_STREAM_STATE_DISCONNECTED
                                    break;
                            }
                        }
                        //post-process
                        if(src != NULL){
                            //count as changing state
                            if(Nix_AAudioSource_isOrphan(src)){
                                ++changingStateCount;
                            }
                            //close orphaned sources (or final-cleanup)
                            if((isFinalCleanup || Nix_AAudioSource_isOrphan(src)) && state != AAUDIO_STREAM_STATE_CLOSING && state != AAUDIO_STREAM_STATE_CLOSED){
                                if(AAUDIO_OK != AAudioStream_close(src->src)){
                                    NIX_PRINTF_ERROR("Nix_AAudioEngine_tick::AAudioStream_close failed.\n");
                                } else {
                                    Nix_AAudioSource_setIsClosing(src);
                                    Nix_AAudioSource_setIsChanging(src, NIX_TRUE);
                                    ++changingStateCount;
                                }
                                Nix_AAudioSource_setIsPlaying(src, NIX_FALSE);
                                Nix_AAudioSource_setIsPaused(src, NIX_FALSE);
                            }
                            //add to notify queue
                            {
                                NIX_MUTEX_LOCK(&src->queues.mutex);
                                {
                                    Nix_AAudioEngine_tick_addQueueNotifSrcLocked_(&notifs, src);
                                }
                                NIX_MUTEX_UNLOCK(&src->queues.mutex);
                            }
                        }
                    }
                }
                obj->srcs.changingStateCountHint = changingStateCount;
            }
            NIX_MUTEX_UNLOCK(&obj->srcs.mutex);
            //notify (unloked)
            if(notifs.use > 0){
                NixUI32 i; for(i = 0; i < notifs.use; ++i){
                    STNix_AAudioSrcNotif* n = &notifs.arr[i];
                    if(n->callback.func != NULL){
                        (*n->callback.func)(n->callback.eng, n->callback.sourceIndex, n->ammBuffs);
                    }
                }
            }
            Nix_AAudioNotifQueue_destroy(&notifs);
        }
        //recorder
        if(obj->rec != NULL){
            Nix_AAudioRecorder_notifyBuffers(obj->rec);
        }
    }
}

//------
//Notif
//------

void Nix_AAudioSrcNotif_init(STNix_AAudioSrcNotif* obj){
    memset(obj, 0, sizeof(*obj));
}

void Nix_AAudioSrcNotif_destroy(STNix_AAudioSrcNotif* obj){
    //
}

//------
//NotifQueue
//------

void Nix_AAudioNotifQueue_init(STNix_AAudioNotifQueue* obj){
    memset(obj, 0, sizeof(*obj));
    obj->arr = obj->arrEmbedded;
    obj->sz = (sizeof(obj->arrEmbedded) / sizeof(obj->arrEmbedded[0]));
}

void Nix_AAudioNotifQueue_destroy(STNix_AAudioNotifQueue* obj){
    if(obj->arr != NULL){
        NixUI32 i; for(i = 0; i < obj->use; i++){
            STNix_AAudioSrcNotif* b = &obj->arr[i];
            Nix_AAudioSrcNotif_destroy(b);
        }
        if(obj->arr != obj->arrEmbedded){
            NIX_FREE(obj->arr);
        }
        obj->arr = NULL;
    }
    obj->use = obj->sz = 0;
}

NixBOOL Nix_AAudioNotifQueue_push(STNix_AAudioNotifQueue* obj, STNix_AAudioSrcNotif* pair){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL && pair != NULL){
        //resize array (if necesary)
        if(obj->use >= obj->sz){
            const NixUI32 szN = obj->use + 4;
            STNix_AAudioSrcNotif* arrN = NULL;
            NIX_MALLOC(arrN, STNix_AAudioSrcNotif, sizeof(STNix_AAudioSrcNotif) * szN, "Nix_AAudioNotifQueue_push::arrN");
            if(arrN != NULL){
                if(obj->arr != NULL){
                    if(obj->use > 0){
                        memcpy(arrN, obj->arr, sizeof(arrN[0]) * obj->use);
                    }
                    if(obj->arr != obj->arrEmbedded){
                        NIX_FREE(obj->arr);
                    }
                    obj->arr = NULL;
                }
                obj->arr = arrN;
                obj->sz = szN;
            }
        }
        //add
        if(obj->use >= obj->sz){
            NIX_PRINTF_ERROR("Nix_AAudioNotifQueue_push failed (no allocated space).\n");
        } else {
            //become the owner of the pair
            obj->arr[obj->use++] = *pair;
            r = NIX_TRUE;
        }
    }
    return r;
}

//------
//QueuePair (Buffers)
//------

void Nix_AAudioQueuePair_init(STNix_AAudioQueuePair* obj){
    memset(obj, 0, sizeof(*obj));
}

void Nix_AAudioQueuePair_destroy(STNix_AAudioQueuePair* obj){
    NIX_ASSERT(obj->org == NULL) //program-logic error; should be always NULLyfied before the pair si destroyed
    if(obj->org != NULL){
        //Note: org is owned by the user, do not destroy
        obj->org = NULL;
    }
    if(obj->cnv != NULL){
        NixPCMBuffer_destroy(obj->cnv);
        NIX_FREE(obj->cnv);
        obj->cnv = NULL;
    }
}

//------
//Queue (Buffers)
//------

void Nix_AAudioQueue_init(STNix_AAudioQueue* obj){
    memset(obj, 0, sizeof(*obj));
}

void Nix_AAudioQueue_destroy(STNix_AAudioQueue* obj){
    if(obj->arr != NULL){
        NixUI32 i; for(i = 0; i < obj->use; i++){
            STNix_AAudioQueuePair* b = &obj->arr[i];
            Nix_AAudioQueuePair_destroy(b);
        }
        NIX_FREE(obj->arr);
        obj->arr = NULL;
    }
    obj->use = obj->sz = 0;
}

NixBOOL Nix_AAudioQueue_flush(STNix_AAudioQueue* obj, const NixBOOL nullifyOrgs){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL){
        if(obj->arr != NULL){
            NixUI32 i; for(i = 0; i < obj->use; i++){
                STNix_AAudioQueuePair* b = &obj->arr[i];
                if(nullifyOrgs){
                    b->org = NULL;
                }
                Nix_AAudioQueuePair_destroy(b);
            }
            obj->use = 0;
        }
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL Nix_AAudioQueue_prepareForSz(STNix_AAudioQueue* obj, const NixUI32 minSz){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL){
        //resize array (if necesary)
        if(minSz > obj->sz){
            const NixUI32 szN = minSz;
            STNix_AAudioQueuePair* arrN = NULL;
            NIX_MALLOC(arrN, STNix_AAudioQueuePair, sizeof(STNix_AAudioQueuePair) * szN, "Nix_AAudioQueue_prepareForSz::arrN");
            if(arrN != NULL){
                if(obj->arr != NULL){
                    if(obj->use > 0){
                        memcpy(arrN, obj->arr, sizeof(arrN[0]) * obj->use);
                    }
                    NIX_FREE(obj->arr);
                    obj->arr = NULL;
                }
                obj->arr = arrN;
                obj->sz = szN;
            }
        }
        //analyze
        if(minSz > obj->sz){
            NIX_PRINTF_ERROR("Nix_AAudioQueue_prepareForSz failed (no allocated space).\n");
        } else {
            r = NIX_TRUE;
        }
    }
    return r;
}

NixBOOL Nix_AAudioQueue_pushOwning(STNix_AAudioQueue* obj, STNix_AAudioQueuePair* pair){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL && pair != NULL){
        //resize array (if necesary)
        if(obj->use >= obj->sz){
            const NixUI32 szN = obj->use + 4;
            STNix_AAudioQueuePair* arrN = NULL;
            NIX_MALLOC(arrN, STNix_AAudioQueuePair, sizeof(STNix_AAudioQueuePair) * szN, "Nix_AAudioQueue_pushOwning::arrN");
            if(arrN != NULL){
                if(obj->arr != NULL){
                    if(obj->use > 0){
                        memcpy(arrN, obj->arr, sizeof(arrN[0]) * obj->use);
                    }
                    NIX_FREE(obj->arr);
                    obj->arr = NULL;
                }
                obj->arr = arrN;
                obj->sz = szN;
            }
        }
        //add
        if(obj->use >= obj->sz){
            NIX_PRINTF_ERROR("Nix_AAudioQueue_pushOwning failed (no allocated space).\n");
        } else {
            //become the owner of the pair
            obj->arr[obj->use++] = *pair;
            r = NIX_TRUE;
        }
    }
    return r;
}

NixBOOL Nix_AAudioQueue_popOrphaning(STNix_AAudioQueue* obj, STNix_AAudioQueuePair* dst){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL && obj->use > 0 && dst != NULL){
        *dst = obj->arr[0];
        --obj->use;
        //fill the gap
        {
            NixUI32 i; for(i = 0; i < obj->use; ++i){
                obj->arr[i] = obj->arr[i + 1];
            }
        }
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL Nix_AAudioQueue_popMovingTo(STNix_AAudioQueue* obj, STNix_AAudioQueue* other){
    NixBOOL r = NIX_FALSE;
    STNix_AAudioQueuePair pair;
    if(!Nix_AAudioQueue_popOrphaning(obj, &pair)){
        //
    } else {
        if(!Nix_AAudioQueue_pushOwning(other, &pair)){
            //program logic error
            NIX_ASSERT(NIX_FALSE);
            if(pair.org != NULL){
                NixPCMBuffer_destroy(pair.org);
                NIX_FREE(pair.org);
                pair.org = NULL;
            }
            Nix_AAudioQueuePair_destroy(&pair);
        } else {
            r = NIX_TRUE;
        }
    }
    return r;
}

//------
//Source
//------

void Nix_AAudioSource_init(STNix_AAudioSource* obj){
    memset(obj, 0, sizeof(STNix_AAudioSource));
    //queues
    {
        NIX_MUTEX_INIT(&obj->queues.mutex);
        Nix_AAudioQueue_init(&obj->queues.notify);
        Nix_AAudioQueue_init(&obj->queues.pend);
        Nix_AAudioQueue_init(&obj->queues.reuse);
    }
}

void Nix_AAudioSource_destroy(STNix_AAudioSource* obj){
    //src
    if(obj->src != NULL){
#       ifdef NIX_ASSERTS_ACTIVATED
        aaudio_stream_state_t state = AAudioStream_getState(obj->src);
        NIX_ASSERT(state == AAUDIO_STREAM_STATE_CLOSED || state == AAUDIO_STREAM_STATE_UNINITIALIZED)
#       endif
        obj->src = NULL;
    }
    //queues
    {
        if(obj->queues.conv != NULL){
            nixFmtConverter_destroy(obj->queues.conv);
            obj->queues.conv = NULL;
        }
        Nix_AAudioQueue_destroy(&obj->queues.pend);
        Nix_AAudioQueue_destroy(&obj->queues.reuse);
        Nix_AAudioQueue_destroy(&obj->queues.notify);
        NIX_MUTEX_DESTROY(&obj->queues.mutex);
    }
}

NixBOOL Nix_AAudioSource_queueBufferForOutput(STNix_AAudioSource* obj, STNixPCMBuffer* buff){
    NixBOOL r = NIX_FALSE;
    if(!STNix_audioDesc_IsEqual(&obj->buffsFmt, &buff->desc)){
        //error
    } else {
        STNix_AAudioQueuePair pair;
        Nix_AAudioQueuePair_init(&pair);
        r = NIX_TRUE;
        //prepare conversion buffer (if necesary)
        if(obj->queues.conv != NULL){
            //create copy buffer
            const NixUI32 buffSamplesMax    = (buff->sz / buff->desc.blockAlign);
            const NixUI32 samplesReq        = nixFmtConverter_samplesForNewFrequency(buffSamplesMax, obj->buffsFmt.samplerate, obj->srcFmt.samplerate);
            STNix_AAudioQueuePair reuse;
            if(!Nix_AAudioQueue_popOrphaning(&obj->queues.reuse, &reuse)){
                //no reusable buffer available, create new
                NIX_MALLOC(pair.cnv, STNixPCMBuffer, sizeof(STNixPCMBuffer), "Nix_AAudioSource_queueBufferForOutput::pair.cnv");
                if(pair.cnv == NULL){
                    NIX_PRINTF_ERROR("Nix_AAudioSource_queueBufferForOutput::pair.cnv could be allocated.\n");
                    r = NIX_FALSE;
                } else {
                    NixPCMBuffer_init(pair.cnv);
                }
            } else {
                //reuse buffer
                NIX_ASSERT(reuse.org == NULL) //program logic error
                NIX_ASSERT(reuse.cnv != NULL) //program logic error
                if(reuse.cnv == NULL){
                    NIX_PRINTF_ERROR("Nix_AAudioSource_queueBufferForOutput::reuse.cnv should not be NULL.\n");
                    r = NIX_FALSE;
                } else {
                    pair.cnv = reuse.cnv; reuse.cnv = NULL; //consume
                }
                Nix_AAudioQueuePair_destroy(&reuse);
            }
            //convert
            if(pair.cnv == NULL){
                r = NIX_FALSE;
            } else if(!NixPCMBuffer_setData(pair.cnv, &obj->srcFmt, NULL, samplesReq * obj->srcFmt.blockAlign)){
                NIX_PRINTF_ERROR("Nix_AAudioSource_queueBufferForOutput::NixPCMBuffer_setData failed.\n");
                r = NIX_FALSE;
            } else if(!nixFmtConverter_setPtrAtSrcInterlaced(obj->queues.conv, &buff->desc, buff->ptr, 0)){
                NIX_PRINTF_ERROR("Nix_AAudioSource_queueBufferForOutput::nixFmtConverter_setPtrAtSrcInterlaced failed.\n");
                r = NIX_FALSE;
            } else if(!nixFmtConverter_setPtrAtDstInterlaced(obj->queues.conv, &pair.cnv->desc, pair.cnv->ptr, 0)){
                NIX_PRINTF_ERROR("Nix_AAudioSource_queueBufferForOutput::nixFmtConverter_setPtrAtDstInterlaced failed.\n");
                r = NIX_FALSE;
            } else {
                const NixUI32 srcBlocks = (buff->use / buff->desc.blockAlign);
                const NixUI32 dstBlocks = (pair.cnv->sz / pair.cnv->desc.blockAlign);
                NixUI32 ammBlocksRead = 0;
                NixUI32 ammBlocksWritten = 0;
                if(!nixFmtConverter_convert(obj->queues.conv, srcBlocks, dstBlocks, &ammBlocksRead, &ammBlocksWritten)){
                    NIX_PRINTF_ERROR("Nix_AAudioSource_queueBufferForOutput::nixFmtConverter_convert failed from(%uhz, %uch, %dbit-%s) to(%uhz, %uch, %dbit-%s).\n"
                                     , obj->buffsFmt.samplerate
                                     , obj->buffsFmt.channels
                                     , obj->buffsFmt.bitsPerSample
                                     , obj->buffsFmt.samplesFormat == ENNix_sampleFormat_int ? "int" : obj->buffsFmt.samplesFormat == ENNix_sampleFormat_float ? "float" : "unknown"
                                     , obj->srcFmt.samplerate
                                     , obj->srcFmt.channels
                                     , obj->srcFmt.bitsPerSample
                                     , obj->srcFmt.samplesFormat == ENNix_sampleFormat_int ? "int" : obj->srcFmt.samplesFormat == ENNix_sampleFormat_float ? "float" : "unknown"
                                     );
                    r = NIX_FALSE;
                }
            }
        }
        //add to queue
        if(r){
            pair.org = buff;
            NIX_MUTEX_LOCK(&obj->queues.mutex);
            {
                if(!Nix_AAudioQueue_pushOwning(&obj->queues.pend, &pair)){
                    NIX_PRINTF_ERROR("Nix_AAudioSource_queueBufferForOutput::Nix_AAudioQueue_pushOwning failed.\n");
                    pair.org = NULL;
                    r = NIX_FALSE;
                } else {
                    //added to queue
                    Nix_AAudioQueue_prepareForSz(&obj->queues.reuse, obj->queues.pend.use); //this ensures malloc wont be calle inside a callback
                    Nix_AAudioQueue_prepareForSz(&obj->queues.notify, obj->queues.pend.use); //this ensures malloc wont be calle inside a callback
                    //this is the first buffer i the queue
                    if(obj->queues.pend.use == 1){
                        obj->queues.pendSampleIdx = 0;
                    }
                    //start/resume stream?
                }
            }
            NIX_MUTEX_UNLOCK(&obj->queues.mutex);
        }
        if(!r){
            Nix_AAudioQueuePair_destroy(&pair);
        }
    }
    return r;
}

NixBOOL Nix_AAudioSource_pendPopOldestBuffLocked_(STNix_AAudioSource* obj){
    NixBOOL r = NIX_FALSE;
    STNix_AAudioQueuePair pair;
    if(!Nix_AAudioQueue_popOrphaning(&obj->queues.pend, &pair)){
        NIX_ASSERT(NIX_FALSE); //program logic error
    } else {
        //move "cnv" to reusable queue
        if(pair.cnv != NULL){
            STNix_AAudioQueuePair reuse;
            Nix_AAudioQueuePair_init(&reuse);
            reuse.cnv = pair.cnv;
            if(!Nix_AAudioQueue_pushOwning(&obj->queues.reuse, &reuse)){
                NIX_PRINTF_ERROR("Nix_AAudioSource_pendPopOldestBuffLocked_::Nix_AAudioQueue_pushOwning(reuse) failed.\n");
                reuse.cnv = NULL;
                Nix_AAudioQueuePair_destroy(&reuse);
            } else {
                //now owned by reuse
                pair.cnv = NULL;
            }
        }
        //move "org" to notify queue
        if(pair.org != NULL){
            STNix_AAudioQueuePair notif;
            Nix_AAudioQueuePair_init(&notif);
            notif.org = pair.org;
            if(!Nix_AAudioQueue_pushOwning(&obj->queues.notify, &notif)){
                NIX_PRINTF_ERROR("Nix_AAudioSource_pendPopOldestBuffLocked_::Nix_AAudioQueue_pushOwning(notify) failed.\n");
                notif.org = NULL;
                Nix_AAudioQueuePair_destroy(&notif);
            } else {
                //now owned by reuse
                pair.org = NULL;
            }
        }
        NIX_ASSERT(pair.org == NULL); //program logic error
        NIX_ASSERT(pair.cnv == NULL); //program logic error
        Nix_AAudioQueuePair_destroy(&pair);
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL Nix_AAudioSource_pendMoveAllBuffsToNotifyWithoutPoppingLocked_(STNix_AAudioSource* obj){
    NixBOOL r = NIX_TRUE;
    NixUI32 i; for(i = 0; i < obj->queues.pend.use; i++){
        STNix_AAudioQueuePair* pair = &obj->queues.pend.arr[i];
        //move "org" to notify queue
        if(pair->org != NULL){
            STNix_AAudioQueuePair notif;
            Nix_AAudioQueuePair_init(&notif);
            notif.org = pair->org;
            if(!Nix_AAudioQueue_pushOwning(&obj->queues.notify, &notif)){
                NIX_PRINTF_ERROR("Nix_AAudioSource_pendMoveAllBuffsToNotifyWithoutPoppingLocked_::Nix_AAudioQueue_pushOwning(notify) failed.\n");
                notif.org = NULL;
                Nix_AAudioQueuePair_destroy(&notif);
            } else {
                //now owned by reuse
                pair->org = NULL;
            }
        }
        NIX_ASSERT(pair->org == NULL); //program logic error
    }
    return r;
}
    
NixUI32 Nix_AAudioSource_feedSamplesTo(STNix_AAudioSource* obj, void* pDst, const NixUI32 samplesMax, NixBOOL* dstExplicitStop){
    NixUI32 r = 0;
    NIX_MUTEX_LOCK(&obj->queues.mutex);
    {
        while(r < samplesMax && obj->queues.pend.use > 0){
            NixBOOL remove = NIX_FALSE, isFullyConsumed = NIX_FALSE;
            STNix_AAudioQueuePair* pair = &obj->queues.pend.arr[0];
            STNixPCMBuffer* buff = (pair->cnv != NULL ? pair->cnv : pair->org);
            NIX_ASSERT(buff != NULL) //program logic error
            if(buff == NULL || buff->ptr == NULL || buff->desc.blockAlign <= 0 || obj->srcFmt.blockAlign <= 0 || buff->desc.blockAlign != obj->srcFmt.blockAlign){
                //just remove
                remove = NIX_TRUE;
            } else {
                const NixUI32 samples = ( buff->use / buff->desc.blockAlign );
                if(samples <= obj->queues.pendSampleIdx){
                    //just remove
                    remove = isFullyConsumed = NIX_TRUE;
                } else {
                    //fill samples
                    const NixUI32 samplesAvailRead = (samples - obj->queues.pendSampleIdx);
                    const NixUI32 samplesAvailWrite = (samplesMax - r);
                    const NixUI32 samplesDo = (samplesAvailRead < samplesAvailWrite ? samplesAvailRead : samplesAvailWrite);
                    if(samplesDo > 0){
                        NixBYTE* dst = &((NixBYTE*)pDst)[r * obj->srcFmt.blockAlign];
                        NixBYTE* src = &((NixBYTE*)buff->ptr)[obj->queues.pendSampleIdx * buff->desc.blockAlign];
                        //ToDo: copy applying volume
                        memcpy(dst, src, samplesDo * obj->srcFmt.blockAlign);
                        obj->queues.pendSampleIdx += samplesDo;
                        r += samplesDo;
                    }
                    if(samplesAvailRead == samplesDo){
                        remove = isFullyConsumed = NIX_TRUE;
                    }
                }
            }
            //
            if(remove){
                if(Nix_AAudioSource_isStatic(obj) && isFullyConsumed && obj->queues.pend.use == 1){
                    if(Nix_AAudioSource_isRepeat(obj)){
                        //consume again
                        obj->queues.pendSampleIdx = 0;
                    } else {
                        //pause while referencing the buffer as fully processed
                        {
                            Nix_AAudioSource_setIsPaused(obj, NIX_TRUE);
                        }
                        //stop consuming
                        {
                            if(dstExplicitStop != NULL){
                                *dstExplicitStop = NIX_TRUE;
                            }
                        }
                        break;
                    }
                } else {
                    //remove
                    Nix_AAudioSource_pendPopOldestBuffLocked_(obj);
                    //prepare for next buffer
                    obj->queues.pendSampleIdx = 0;
                }
            }
        }
    }
    NIX_MUTEX_UNLOCK(&obj->queues.mutex);
    return r;
}

//------
//Recorder
//------

void Nix_AAudioRecorder_init(STNix_AAudioRecorder* obj){
    memset(obj, 0, sizeof(*obj));
    //cfg
    {
        //
    }
    //queues
    {
        NIX_MUTEX_INIT(&obj->queues.mutex);
        Nix_AAudioQueue_init(&obj->queues.notify);
        Nix_AAudioQueue_init(&obj->queues.reuse);
    }
}

void Nix_AAudioRecorder_destroy(STNix_AAudioRecorder* obj){
    //queues
    {
        NIX_MUTEX_LOCK(&obj->queues.mutex);
        {
            //ToDo: remove all 'org' buffers manually
            Nix_AAudioQueue_destroy(&obj->queues.notify);
            Nix_AAudioQueue_destroy(&obj->queues.reuse);
        }
        NIX_MUTEX_UNLOCK(&obj->queues.mutex);
        NIX_MUTEX_DESTROY(&obj->queues.mutex);
    }
    //
    if(obj->rec != NULL){
        AAudioStream_close(obj->rec);
        obj->rec = NULL;
    }
}

void nixAAudioRecorder_errorCallback_(AAudioStream *_Nonnull stream, void *_Nullable userData, aaudio_result_t error){
    //STNix_AAudioRecorder* obj = (STNix_AAudioRecorder*)userData;
    NIX_PRINTF_ERROR("nixAAudioRecorder_errorCallback_::error %d = '%s'.\n", error, AAudio_convertResultToText(error));
}

aaudio_data_callback_result_t nixAAudioRecorder_dataCallback_(AAudioStream *_Nonnull stream, void *_Nullable userData, void *_Nonnull audioData, int32_t numFrames){
    STNix_AAudioRecorder* obj = (STNix_AAudioRecorder*)userData;
    {
        Nix_AAudioRecorder_consumeInputBuffer(obj, audioData, numFrames);
    }
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

NixBOOL Nix_AAudioRecorder_prepare(STNix_AAudioRecorder* obj, const STNix_audioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer){
    NixBOOL r = NIX_FALSE;
    NIX_MUTEX_LOCK(&obj->queues.mutex);
    if(obj->queues.conv == NULL && audioDesc->blockAlign > 0){
        AAudioStreamBuilder *bldr;
        aaudio_result_t rr = AAudio_createStreamBuilder(&bldr);
        if(rr != AAUDIO_OK || bldr == NULL){
            NIX_PRINTF_ERROR("Nix_AAudioRecorder_prepare::AAudio_createStreamBuilder failed.\n");
        } else {
            aaudio_result_t rr = 0;
            AAudioStream *stream = NULL;
            AAudioStreamBuilder_setDirection(bldr, AAUDIO_DIRECTION_INPUT);
            //AAudioStreamBuilder_setSharingMode(bldr, AAUDIO_SHARING_MODE_SHARED);
            AAudioStreamBuilder_setSampleRate(bldr, audioDesc->samplerate);
            AAudioStreamBuilder_setChannelCount(bldr, audioDesc->channels);
            AAudioStreamBuilder_setDataCallback(bldr, nixAAudioRecorder_dataCallback_, obj);
            AAudioStreamBuilder_setErrorCallback(bldr, nixAAudioRecorder_errorCallback_, obj);
            if(audioDesc->samplesFormat == ENNix_sampleFormat_int){
                if(audioDesc->bitsPerSample == 16){
                    AAudioStreamBuilder_setFormat(bldr, AAUDIO_FORMAT_PCM_I16);
                } else if(audioDesc->bitsPerSample == 32){
                    AAudioStreamBuilder_setFormat(bldr, AAUDIO_FORMAT_PCM_I32);
                }
            } else if(audioDesc->samplesFormat == ENNix_sampleFormat_float){
                if(audioDesc->bitsPerSample == 32){
                    AAudioStreamBuilder_setFormat(bldr, AAUDIO_FORMAT_PCM_FLOAT);
                }
            }
            rr = AAudioStreamBuilder_openStream(bldr, &stream);
            if(AAUDIO_OK != rr){
                NIX_PRINTF_ERROR("Nix_AAudioRecorder_prepare::AAudioStreamBuilder_openStream failed.\n");
            } else {
                STNix_audioDesc inDesc;
                memset(&inDesc, 0, sizeof(inDesc));
                //read properties
                switch(AAudioStream_getFormat(stream)){
                    case AAUDIO_FORMAT_PCM_I16:
                        inDesc.bitsPerSample = 16;
                        inDesc.samplesFormat = ENNix_sampleFormat_int;
                        break;
                    case AAUDIO_FORMAT_PCM_I32:
                        inDesc.bitsPerSample = 32;
                        inDesc.samplesFormat = ENNix_sampleFormat_int;
                        break;
                    case AAUDIO_FORMAT_PCM_FLOAT:
                        inDesc.bitsPerSample = 32;
                        inDesc.samplesFormat = ENNix_sampleFormat_float;
                        break;
                    default:
                        inDesc.bitsPerSample = 0;
                        inDesc.samplesFormat = ENNix_sampleFormat_none;
                        break;
                }
                inDesc.channels   = AAudioStream_getChannelCount(stream);
                inDesc.samplerate = AAudioStream_getSampleRate(stream);
                if(inDesc.bitsPerSample <= 0 || inDesc.channels <= 0 || inDesc.samplerate <= 0){
                    NIX_PRINTF_ERROR("Nix_AAudioRecorder_prepare, unknown stream sample format.\n");
                } else {
                    inDesc.blockAlign = (inDesc.bitsPerSample / 8) * inDesc.channels;
                    void* conv = nixFmtConverter_create();
                    if(!nixFmtConverter_prepare(conv, &inDesc, audioDesc)){
                        NIX_PRINTF_ERROR("Nix_AAudioRecorder_prepare::nixFmtConverter_prepare failed.\n");
                        nixFmtConverter_destroy(conv);
                        conv = NULL;
                    } else {
                        //allocate reusable buffers
                        while(obj->queues.reuse.use < buffersCount){
                            STNix_AAudioQueuePair pair;
                            Nix_AAudioQueuePair_init(&pair);
                            NIX_MALLOC(pair.org, STNixPCMBuffer, sizeof(STNixPCMBuffer), "Nix_AAudioRecorder_prepare.pair.org");
                            if(pair.org == NULL){
                                NIX_PRINTF_ERROR("Nix_AAudioRecorder_prepare::pair.org allocation failed.\n");
                                break;
                            } else {
                                NixPCMBuffer_init(pair.org);
                                if(!NixPCMBuffer_setData(pair.org, audioDesc, NULL, audioDesc->blockAlign * samplesPerBuffer)){
                                    NIX_PRINTF_ERROR("Nix_AAudioRecorder_prepare::NixPCMBuffer_setData failed.\n");
                                    NixPCMBuffer_destroy(pair.org);
                                    NIX_FREE(pair.org);
                                    pair.org = NULL;
                                    break;
                                } else {
                                    Nix_AAudioQueue_pushOwning(&obj->queues.reuse, &pair);
                                }
                            }
                        }
                        //
                        if(obj->queues.reuse.use <= 0){
                            NIX_PRINTF_ERROR("Nix_AAudioRecorder_prepare::no reusable buffer could be allocated.\n");
                        } else {
                            //prepared
                            obj->queues.filling.iCurSample = 0;
                            obj->queues.conv = conv; conv = NULL; //consume
                            obj->rec = stream; stream = NULL; //consume
                            //cfg
                            obj->cfg.fmt = *audioDesc;
                            obj->cfg.maxBuffers = buffersCount;
                            obj->cfg.samplesPerBuffer = samplesPerBuffer;
                            //
                            obj->capFmt = inDesc;
                            r = NIX_TRUE;
                        }
                    }
                    //release (if not consumed)
                    if(conv != NULL){
                        nixFmtConverter_destroy(conv);
                        conv = NULL;
                    }
                    
                }
                //release (if not consumed)
                if(stream != NULL){
                    AAudioStream_close(stream);
#                   ifdef NIX_ASSERTS_ACTIVATED
                    {
                        aaudio_stream_state_t state = AAudioStream_getState(stream);
                        NIX_ASSERT(state == AAUDIO_STREAM_STATE_CLOSED || state == AAUDIO_STREAM_STATE_UNINITIALIZED)
                    }
#                   endif
                    stream = NULL;
                }
            }
            AAudioStreamBuilder_delete(bldr);
        }
    }
    NIX_MUTEX_UNLOCK(&obj->queues.mutex);
    return r;
}

NixBOOL Nix_AAudioRecorder_setCallback(STNix_AAudioRecorder* obj, NixApiCaptureBufferFilledCallback callback, void* callbackData){
    NixBOOL r = NIX_FALSE;
    {
        obj->callback.func = callback;
        obj->callback.data = callbackData;
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL Nix_AAudioRecorder_start(STNix_AAudioRecorder* obj){
    NixBOOL r = NIX_FALSE;
    if(obj->rec != NULL){
        r = NIX_TRUE;
        if(!obj->engStarted){
            if(AAUDIO_OK != AAudioStream_requestStart(obj->rec)){
                NIX_PRINTF_ERROR("Nix_AAudioRecorder_start::AAudioStream_requestStart failed.\n");
                r = NIX_FALSE;
            } else {
                obj->engStarted = NIX_TRUE;
            }
        }
    }
    return r;
}

NixBOOL Nix_AAudioRecorder_stop(STNix_AAudioRecorder* obj){
    NixBOOL r = NIX_TRUE;
    if(obj->rec != NULL){
        if(AAUDIO_OK != AAudioStream_requestStop(obj->rec)){
            NIX_PRINTF_ERROR("Nix_AAudioRecorder_stop::AAudioStream_requestStop failed.\n");
            r = NIX_FALSE;
        }
        obj->engStarted = NIX_FALSE;
    }
    Nix_AAudioRecorder_flush(obj);
    return r;
}

NixBOOL Nix_AAudioRecorder_flush(STNix_AAudioRecorder* obj){
    NixBOOL r = NIX_TRUE;
    //move filling buffer to notify (if data is available)
    NIX_MUTEX_LOCK(&obj->queues.mutex);
    if(obj->queues.reuse.use > 0){
        STNix_AAudioQueuePair* pair = &obj->queues.reuse.arr[0];
        if(pair->org != NULL && pair->org->use > 0){
            obj->queues.filling.iCurSample = 0;
            if(!Nix_AAudioQueue_popMovingTo(&obj->queues.reuse, &obj->queues.notify)){
                //program logic error
                r = NIX_FALSE;
            }
        }
    }
    NIX_MUTEX_UNLOCK(&obj->queues.mutex);
    return r;
}

void Nix_AAudioRecorder_consumeInputBuffer(STNix_AAudioRecorder* obj, void* audioData, const NixSI32 numFrames){
    if(obj->queues.conv != NULL && obj->rec != NULL && audioData != NULL && numFrames > 0){
        NIX_MUTEX_LOCK(&obj->queues.mutex);
        {
            NixUI32 inIdx = 0;
            NixSI32 inSz = numFrames;
            //process
            while(inIdx < inSz){
                if(obj->queues.reuse.use <= 0){
                    //move oldest-notify buffer to reuse
                    if(!Nix_AAudioQueue_popMovingTo(&obj->queues.notify, &obj->queues.reuse)){
                        //program logic error
                        NIX_ASSERT(NIX_FALSE);
                        break;
                    }
                } else {
                    STNix_AAudioQueuePair* pair = &obj->queues.reuse.arr[0];
                    if(pair->org == NULL || pair->org->desc.blockAlign <= 0){
                        //just remove
                        STNix_AAudioQueuePair pair;
                        if(!Nix_AAudioQueue_popOrphaning(&obj->queues.reuse, &pair)){
                            NIX_ASSERT(NIX_FALSE);
                            //program logic error
                            break;
                        }
                        if(pair.org != NULL){
                            NixPCMBuffer_destroy(pair.org);
                            NIX_FREE(pair.org);
                            pair.org = NULL;
                        }
                        Nix_AAudioQueuePair_destroy(&pair);
                    } else {
                        const NixUI32 outSz = (pair->org->sz / pair->org->desc.blockAlign);
                        const NixUI32 outAvail = (obj->queues.filling.iCurSample >= outSz ? 0 : outSz - obj->queues.filling.iCurSample);
                        const NixUI32 inAvail = inSz - inIdx;
                        NixUI32 ammBlocksRead = 0, ammBlocksWritten = 0;
                        if(outAvail > 0 && inAvail > 0){
                            //dst
                            nixFmtConverter_setPtrAtDstInterlaced(obj->queues.conv, &pair->org->desc, pair->org->ptr, obj->queues.filling.iCurSample);
                            //src
                            nixFmtConverter_setPtrAtSrcInterlaced(obj->queues.conv, &obj->capFmt, audioData, inIdx);
                            //convert
                            if(!nixFmtConverter_convert(obj->queues.conv, inAvail, outAvail, &ammBlocksRead, &ammBlocksWritten)){
                                //error
                                break;
                            } else if(ammBlocksRead == 0 && ammBlocksWritten == 0){
                                //converter did nothing, avoid infinite cycle
                                break;
                            } else {
                                inIdx += ammBlocksRead;
                                obj->queues.filling.iCurSample += ammBlocksWritten;
                                pair->org->use = (obj->queues.filling.iCurSample * pair->org->desc.blockAlign); NIX_ASSERT(pair->org->use <= pair->org->sz)
                            }
                        }
                        //move reusable buffer to notify
                        if(ammBlocksWritten == outAvail){
                            obj->queues.filling.iCurSample = 0;
                            if(!Nix_AAudioQueue_popMovingTo(&obj->queues.reuse, &obj->queues.notify)){
                                //program logic error
                                NIX_ASSERT(NIX_FALSE);
                                break;
                            }
                        }
                    }
                }
            }
        }
        NIX_MUTEX_UNLOCK(&obj->queues.mutex);
    }
}

void Nix_AAudioRecorder_notifyBuffers(STNix_AAudioRecorder* obj){
    NIX_MUTEX_LOCK(&obj->queues.mutex);
    {
        const NixUI32 maxProcess = obj->queues.notify.use;
        NixUI32 ammProcessed = 0;
        while(ammProcessed < maxProcess && obj->queues.notify.use > 0){
            STNix_AAudioQueuePair pair;
            if(!Nix_AAudioQueue_popOrphaning(&obj->queues.notify, &pair)){
                NIX_ASSERT(NIX_FALSE);
                //program logic error
                break;
            } else {
                //notify (unlocked)
                if(pair.org != NULL && pair.org->desc.blockAlign > 0 && obj->callback.func != NULL){
                    NIX_MUTEX_UNLOCK(&obj->queues.mutex);
                    {
                        (*obj->callback.func)((STNixApiEngine){ obj->engNx }, (STNixApiRecorder){ obj }, pair.org->desc, pair.org->ptr, pair.org->use, (pair.org->use / pair.org->desc.blockAlign), obj->callback.data);
                    }
                    NIX_MUTEX_LOCK(&obj->queues.mutex);
                }
                //move to reuse
                if(!Nix_AAudioQueue_pushOwning(&obj->queues.reuse, &pair)){
                    //program logic error
                    NIX_ASSERT(NIX_FALSE);
                    if(pair.org != NULL){
                        NixPCMBuffer_destroy(pair.org);
                        NIX_FREE(pair.org);
                        pair.org = NULL;
                    }
                    Nix_AAudioQueuePair_destroy(&pair);
                }
            }
            //processed
            ++ammProcessed;
        }
    }
    NIX_MUTEX_UNLOCK(&obj->queues.mutex);
}

//------
//Engine (API)
//------

STNixApiEngine nixAAudioEngine_create(void){
    STNixApiEngine r = STNixApiEngine_Zero;
    STNix_AAudioEngine* obj = NULL;
    NIX_MALLOC(obj, STNix_AAudioEngine, sizeof(STNix_AAudioEngine), "STNix_AAudioEngine");
    if(obj != NULL){
        Nix_AAudioEngine_init(obj);
        r.opq = obj;
    }
    return r;
}

void nixAAudioEngine_destroy(STNixApiEngine pObj){
    STNix_AAudioEngine* obj = (STNix_AAudioEngine*)pObj.opq;
    if(obj != NULL){
        Nix_AAudioEngine_destroy(obj);
        obj = NULL;
    }
}

void nixAAudioEngine_printCaps(STNixApiEngine pObj){
    //
}

NixBOOL nixAAudioEngine_ctxIsActive(STNixApiEngine pObj){
    NixBOOL r = NIX_FALSE;
    STNix_AAudioEngine* obj = (STNix_AAudioEngine*)pObj.opq;
    if(obj != NULL){
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL nixAAudioEngine_ctxActivate(STNixApiEngine pObj){
    NixBOOL r = NIX_FALSE;
    STNix_AAudioEngine* obj = (STNix_AAudioEngine*)pObj.opq;
    if(obj != NULL){
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL nixAAudioEngine_ctxDeactivate(STNixApiEngine pObj){
    NixBOOL r = NIX_FALSE;
    STNix_AAudioEngine* obj = (STNix_AAudioEngine*)pObj.opq;
    if(obj != NULL){
        r = NIX_TRUE;
    }
    return r;
}

void nixAAudioEngine_tick(STNixApiEngine pObj){
    STNix_AAudioEngine* obj = (STNix_AAudioEngine*)pObj.opq;
    if(obj != NULL){
        Nix_AAudioEngine_tick(obj, NIX_FALSE);
    }
}

//------
//PCMBuffer (API)
//------

STNixApiBuffer nixAAudioPCMBuffer_create(const STNix_audioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes){
    STNixApiBuffer r = STNixApiBuffer_Zero;
    if(audioDesc != NULL && audioDesc->blockAlign > 0){
        STNixPCMBuffer* obj = NULL;
        NIX_MALLOC(obj, STNixPCMBuffer, sizeof(STNixPCMBuffer), "STNixPCMBuffer");
        if(obj != NULL){
            NixPCMBuffer_init(obj);
            if(!NixPCMBuffer_setData(obj, audioDesc, audioDataPCM, audioDataPCMBytes)){
                NIX_PRINTF_ERROR("nixAAudioPCMBuffer_create::NixPCMBuffer_setData failed.\n");
                NixPCMBuffer_destroy(obj);
                NIX_FREE(obj);
                obj = NULL;
            }
            r.opq = obj;
        }
    }
    return r;
}

void nixAAudioPCMBuffer_destroy(STNixApiBuffer pObj){
    if(pObj.opq != NULL){
        STNixPCMBuffer* obj = (STNixPCMBuffer*)pObj.opq;
        NixPCMBuffer_destroy(obj);
        NIX_FREE(obj);
        obj = NULL;
    }
}
   
NixBOOL nixAAudioPCMBuffer_setData(STNixApiBuffer pObj, const STNix_audioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes){
    NixBOOL r = NIX_FALSE;
    if(pObj.opq != NULL && audioDesc != NULL && audioDesc->blockAlign > 0){
        STNixPCMBuffer* obj = (STNixPCMBuffer*)pObj.opq;
        r = NixPCMBuffer_setData(obj, audioDesc, audioDataPCM, audioDataPCMBytes);
    }
    return r;
}

NixBOOL nixAAudioPCMBuffer_fillWithZeroes(STNixApiBuffer pObj){
    NixBOOL r = NIX_FALSE;
    if(pObj.opq != NULL){
        STNixPCMBuffer* obj = (STNixPCMBuffer*)pObj.opq;
        r = NixPCMBuffer_fillWithZeroes(obj);
    }
    return r;
}

//------
//Source (API)
//------

STNixApiSource nixAAudioSource_create(STNixApiEngine pEng){
    STNixApiSource r = STNixApiSource_Zero;
    STNix_AAudioEngine* eng = (STNix_AAudioEngine*)pEng.opq;
    if(eng != NULL){
        STNix_AAudioSource* obj = NULL;
        NIX_MALLOC(obj, STNix_AAudioSource, sizeof(STNix_AAudioSource), "STNix_AAudioSource");
        if(obj != NULL){
            Nix_AAudioSource_init(obj);
            obj->eng = eng;
            //add to engine
            if(!Nix_AAudioEngine_srcsAdd(eng, obj)){
                NIX_PRINTF_ERROR("nixAAudioSource_create::Nix_AAudioEngine_srcsAdd failed.\n");
                Nix_AAudioSource_destroy(obj);
                NIX_FREE(obj);
                obj = NULL;
            } else {
                //
            }
        }
        r.opq = obj;
    }
    return r;
}

void nixAAudioSource_removeAllBuffersAndNotify_(STNix_AAudioSource* obj){
    STNix_AAudioNotifQueue notifs;
    Nix_AAudioNotifQueue_init(&notifs);
    //move all pending buffers to notify
    NIX_MUTEX_LOCK(&obj->queues.mutex);
    {
        Nix_AAudioSource_pendMoveAllBuffsToNotifyWithoutPoppingLocked_(obj);
        Nix_AAudioEngine_tick_addQueueNotifSrcLocked_(&notifs, obj);
    }
    NIX_MUTEX_UNLOCK(&obj->queues.mutex);
    //notify
    {
        NixUI32 i; for(i = 0; i < notifs.use; ++i){
            STNix_AAudioSrcNotif* n = &notifs.arr[i];
            if(n->callback.func != NULL){
                (*n->callback.func)(n->callback.eng, n->callback.sourceIndex, n->ammBuffs);
            }
        }
    }
}

void nixAAudioSource_destroy(STNixApiSource pObj){ //orphans the source, will automatically be destroyed after internal cleanup
    if(pObj.opq != NULL){
        STNix_AAudioSource* obj = (STNix_AAudioSource*)pObj.opq;
        //close
        if(obj->src != NULL){
            if(AAUDIO_OK != AAudioStream_close(obj->src)){
                NIX_PRINTF_ERROR("nixAAudioSource_destroy::AAudioStream_close failed.\n");
            } else {
                Nix_AAudioSource_setIsClosing(obj);
                Nix_AAudioSource_setIsChanging(obj, NIX_TRUE);
            }
        }
        Nix_AAudioSource_setIsOrphan(obj); //source is waiting for close(), wait for the change of state and Nix_AAudioSource_release + NIX_FREE.
        Nix_AAudioSource_setIsPlaying(obj, NIX_FALSE);
        Nix_AAudioSource_setIsPaused(obj, NIX_FALSE);
        ++obj->eng->srcs.changingStateCountHint;
        //flush all pending buffers
        nixAAudioSource_removeAllBuffersAndNotify_(obj);
    }
}

void nixAAudioSource_setCallback(STNixApiSource pObj, void (*callback)(void* pEng, const NixUI32 sourceIndex, const NixUI32 ammBuffs), void* callbackEng, NixUI32 callbackSourceIndex){
    if(pObj.opq != NULL){
        STNix_AAudioSource* obj = (STNix_AAudioSource*)pObj.opq;
        obj->queues.callback.func = callback;
        obj->queues.callback.eng = callbackEng;
        obj->queues.callback.sourceIndex = callbackSourceIndex;
    }
}

NixBOOL nixAAudioSource_setVolume(STNixApiSource pObj, const float vol){
    NixBOOL r = NIX_FALSE;
    if(pObj.opq != NULL){
        STNix_AAudioSource* obj = (STNix_AAudioSource*)pObj.opq;
        obj->volume = vol;
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL nixAAudioSource_setRepeat(STNixApiSource pObj, const NixBOOL isRepeat){
    NixBOOL r = NIX_FALSE;
    if(pObj.opq != NULL){
        STNix_AAudioSource* obj = (STNix_AAudioSource*)pObj.opq;
        Nix_AAudioSource_setIsRepeat(obj, isRepeat);
        r = NIX_TRUE;
    }
    return r;
}

void nixAAudioSource_play(STNixApiSource pObj){
    if(pObj.opq != NULL){
        STNix_AAudioSource* obj = (STNix_AAudioSource*)pObj.opq;
        if(obj->src != NULL && (!Nix_AAudioSource_isPlaying(obj) || Nix_AAudioSource_isPaused(obj))){
            //restart static buffer
            if(Nix_AAudioSource_isStatic(obj)){
                NIX_MUTEX_LOCK(&obj->queues.mutex);
                {
                    if(obj->queues.pend.use == 1){
                        //last buffer to play
                        STNix_AAudioQueuePair* pair = &obj->queues.pend.arr[0];
                        STNixPCMBuffer* buff = (pair->cnv != NULL ? pair->cnv : pair->org);
                        if(buff != NULL && buff->desc.blockAlign > 0){
                            const NixUI32 samples = buff->use / buff->desc.blockAlign;
                            if(obj->queues.pendSampleIdx >= samples){
                                //restart this buffer
                                obj->queues.pendSampleIdx = 0;
                            }
                        }
                    }
                }
                NIX_MUTEX_UNLOCK(&obj->queues.mutex);
            }
            if(AAUDIO_OK != AAudioStream_requestStart(obj->src)){
                NIX_PRINTF_ERROR("nixAAudioSource_play::AAudioStream_requestStart failed.\n");
            } else {
                Nix_AAudioSource_setIsChanging(obj, NIX_TRUE);
                ++obj->eng->srcs.changingStateCountHint;
            }
        }
        Nix_AAudioSource_setIsPlaying(obj, NIX_TRUE);
        Nix_AAudioSource_setIsPaused(obj, NIX_FALSE);
    }
}

void nixAAudioSource_pause(STNixApiSource pObj){
    if(pObj.opq != NULL){
        STNix_AAudioSource* obj = (STNix_AAudioSource*)pObj.opq;
        if(obj->src != NULL && Nix_AAudioSource_isPlaying(obj) && !Nix_AAudioSource_isPaused(obj)){
            if(AAUDIO_OK != AAudioStream_requestPause(obj->src)){
                NIX_PRINTF_ERROR("nixAAudioSource_pause::AAudioStream_requestPause failed.\n");
            } else {
                Nix_AAudioSource_setIsChanging(obj, NIX_TRUE);
                ++obj->eng->srcs.changingStateCountHint;
            }
        }
        Nix_AAudioSource_setIsPaused(obj, NIX_TRUE);
    }
}

void nixAAudioSource_stop(STNixApiSource pObj){
    if(pObj.opq != NULL){
        STNix_AAudioSource* obj = (STNix_AAudioSource*)pObj.opq;
        if(obj->src != NULL && Nix_AAudioSource_isPlaying(obj)){
            if(AAUDIO_OK != AAudioStream_requestStop(obj->src)){
                NIX_PRINTF_ERROR("nixAAudioSource_stop::AAudioStream_requestStop failed.\n");
            } else {
                Nix_AAudioSource_setIsChanging(obj, NIX_TRUE);
                ++obj->eng->srcs.changingStateCountHint;
            }
        }
        Nix_AAudioSource_setIsPlaying(obj, NIX_FALSE);
        Nix_AAudioSource_setIsPaused(obj, NIX_FALSE);
        //flush all pending buffers
        nixAAudioSource_removeAllBuffersAndNotify_(obj);
    }
}

NixBOOL nixAAudioSource_isPlaying(STNixApiSource pObj){
    NixBOOL r = NIX_FALSE;
    if(pObj.opq != NULL){
        STNix_AAudioSource* obj = (STNix_AAudioSource*)pObj.opq;
        if(Nix_AAudioSource_isPlaying(obj) && !Nix_AAudioSource_isPaused(obj)){
            r = NIX_TRUE;
        }
    }
    return r;
}

NixBOOL nixAAudioSource_isPaused(STNixApiSource pObj){
    NixBOOL r = NIX_FALSE;
    if(pObj.opq != NULL){
        STNix_AAudioSource* obj = (STNix_AAudioSource*)pObj.opq;
        r = Nix_AAudioSource_isPlaying(obj) && Nix_AAudioSource_isPaused(obj) ? NIX_TRUE : NIX_FALSE;
    }
    return r;
}

void nixAAudioSource_errorCallback_(AAudioStream *_Nonnull stream, void *_Nullable userData, aaudio_result_t error){
    //STNix_AAudioSource* obj = (STNix_AAudioSource*)userData;
    NIX_PRINTF_ERROR("nixAAudioSource_errorCallback_::error %d = '%s'.\n", error, AAudio_convertResultToText(error));
}

aaudio_data_callback_result_t nixAAudioSource_dataCallback_(AAudioStream *_Nonnull stream, void *_Nullable userData, void *_Nonnull audioData, int32_t numFrames){
    STNix_AAudioSource* obj = (STNix_AAudioSource*)userData;
    NixBOOL dstExplicitStop = NIX_FALSE;
    const NixUI32 numFed = Nix_AAudioSource_feedSamplesTo(obj, audioData, numFrames, &dstExplicitStop);
    //fille with zeroes the unpopulated area
    if(numFed < numFrames && obj->srcFmt.blockAlign > 0){
        void* data = &(((NixUI8*)audioData)[numFed * obj->srcFmt.blockAlign]);
        const NixUI32 dataSz = (numFrames - numFed) * obj->srcFmt.blockAlign;
        memset(data, 0, dataSz);
    }
    return (numFed < numFrames || dstExplicitStop ? AAUDIO_CALLBACK_RESULT_STOP : AAUDIO_CALLBACK_RESULT_CONTINUE);
}

NixBOOL nixAAudioSource_prepareSourceForFmt_(STNix_AAudioSource* obj, const STNix_audioDesc* fmt){
    NixBOOL r = NIX_FALSE;
    AAudioStreamBuilder *bldr;
    aaudio_result_t rr = AAudio_createStreamBuilder(&bldr);
    if(rr != AAUDIO_OK || bldr == NULL){
        NIX_PRINTF_ERROR("nixAAudioSource_prepareSourceForFmt_::AAudio_createStreamBuilder failed.\n");
    } else {
        aaudio_result_t rr = 0;
        AAudioStream *stream = NULL;
        AAudioStreamBuilder_setDirection(bldr, AAUDIO_DIRECTION_OUTPUT);
        AAudioStreamBuilder_setSharingMode(bldr, AAUDIO_SHARING_MODE_SHARED);
        AAudioStreamBuilder_setSampleRate(bldr, fmt->samplerate);
        AAudioStreamBuilder_setChannelCount(bldr, fmt->channels);
        AAudioStreamBuilder_setDataCallback(bldr, nixAAudioSource_dataCallback_, obj);
        AAudioStreamBuilder_setErrorCallback(bldr, nixAAudioSource_errorCallback_, obj);
        if(fmt->samplesFormat == ENNix_sampleFormat_int){
            if(fmt->bitsPerSample == 16){
                AAudioStreamBuilder_setFormat(bldr, AAUDIO_FORMAT_PCM_I16);
            } else if(fmt->bitsPerSample == 32){
                AAudioStreamBuilder_setFormat(bldr, AAUDIO_FORMAT_PCM_I32);
            }
        } else if(fmt->samplesFormat == ENNix_sampleFormat_float){
            if(fmt->bitsPerSample == 32){
                AAudioStreamBuilder_setFormat(bldr, AAUDIO_FORMAT_PCM_FLOAT);
            }
        }
        rr = AAudioStreamBuilder_openStream(bldr, &stream);
        if(AAUDIO_OK != rr){
            NIX_PRINTF_ERROR("nixAAudioSource_prepareSourceForFmt_::AAudioStreamBuilder_openStream failed.\n");
        } else {
            STNix_audioDesc desc;
            memset(&desc, 0, sizeof(desc));
            //read properties
            switch(AAudioStream_getFormat(stream)){
                case AAUDIO_FORMAT_PCM_I16:
                    desc.bitsPerSample = 16;
                    desc.samplesFormat = ENNix_sampleFormat_int;
                    break;
                case AAUDIO_FORMAT_PCM_I32:
                    desc.bitsPerSample = 32;
                    desc.samplesFormat = ENNix_sampleFormat_int;
                    break;
                case AAUDIO_FORMAT_PCM_FLOAT:
                    desc.bitsPerSample = 32;
                    desc.samplesFormat = ENNix_sampleFormat_float;
                    break;
                default:
                    desc.bitsPerSample = 0;
                    desc.samplesFormat = ENNix_sampleFormat_none;
                    break;
            }
            desc.channels   = AAudioStream_getChannelCount(stream);
            desc.samplerate = AAudioStream_getSampleRate(stream);
            if(desc.bitsPerSample <= 0 || desc.channels <= 0 || desc.samplerate <= 0){
                NIX_PRINTF_ERROR("nixAAudioSource_prepareSourceForFmt_, unknown stream sample format.\n");
            } else {
                desc.blockAlign = (desc.bitsPerSample / 8) * desc.channels;
                r = NIX_TRUE;
                //converter
                if(!STNix_audioDesc_IsEqual(fmt, &desc)){
                    void* conv = nixFmtConverter_create();
                    if(!nixFmtConverter_prepare(conv, fmt, &desc)){
                        NIX_PRINTF_ERROR("nixAAudioSource_prepareSourceForFmt_, nixFmtConverter_prepare failed.\n");
                        nixFmtConverter_destroy(conv);
                        r = NIX_FALSE;
                    } else {
                        obj->queues.conv = conv;
                    }
                }
                //set
                if(r){
                    obj->src = stream; stream = NULL; //consume
                    obj->srcFmt = desc;
                    obj->buffsFmt = *fmt;
                }
                
            }
            //release (if not consumed)
            if(stream != NULL){
                AAudioStream_close(stream);
#               ifdef NIX_ASSERTS_ACTIVATED
                {
                    aaudio_stream_state_t state = AAudioStream_getState(stream);
                    NIX_ASSERT(state == AAUDIO_STREAM_STATE_CLOSED || state == AAUDIO_STREAM_STATE_UNINITIALIZED)
                }
#               endif
                stream = NULL;
            }
        }
        AAudioStreamBuilder_delete(bldr);
    }
    return r;
}
    
NixBOOL nixAAudioSource_setBuffer(STNixApiSource pObj, STNixApiBuffer pBuff){  //static-source
    NixBOOL r = NIX_FALSE;
    if(pObj.opq != NULL && pBuff.opq != NULL){
        STNix_AAudioSource* obj    = (STNix_AAudioSource*)pObj.opq;
        STNixPCMBuffer* buff = (STNixPCMBuffer*)pBuff.opq;
        if(obj->src == NULL || obj->srcFmt.blockAlign <= 0){
            if(!nixAAudioSource_prepareSourceForFmt_(obj, &buff->desc)){
                NIX_PRINTF_ERROR("nixAAudioSource_setBuffer::nixAAudioSource_prepareSourceForFmt_ failed.\n");
            } else {
                NIX_ASSERT(obj->src != NULL) //program logic error
            }
        }
        //apply
        if(obj->src == NULL){
            NIX_PRINTF_ERROR("nixAAudioSource_setBuffer, no source available.\n");
        } else if(obj->queues.pend.use != 0){
            NIX_PRINTF_ERROR("nixAAudioSource_setBuffer, source already has buffer.\n");
        } else if(Nix_AAudioSource_isStatic(obj)){
            NIX_PRINTF_ERROR("nixAAudioSource_setBuffer, source is already static.\n");
        } else if(!STNix_audioDesc_IsEqual(&obj->buffsFmt, &buff->desc)){
            NIX_PRINTF_ERROR("nixAAudioSource_setBuffer, new buffer doesnt match first buffer's format.\n");
        } else {
            Nix_AAudioSource_setIsStatic(obj, NIX_TRUE);
            //schedule
            if(!Nix_AAudioSource_queueBufferForOutput(obj, buff)){
                NIX_PRINTF_ERROR("nixAAudioSource_setBuffer, Nix_AAudioSource_queueBufferForOutput failed.\n");
            } else {
                r = NIX_TRUE;
            }
        }
    }
    return r;
}

NixBOOL nixAAudioSource_queueBuffer(STNixApiSource pObj, STNixApiBuffer pBuff){
    NixBOOL r = NIX_FALSE;
    if(pObj.opq != NULL && pBuff.opq != NULL){
        STNix_AAudioSource* obj    = (STNix_AAudioSource*)pObj.opq;
        STNixPCMBuffer* buff = (STNixPCMBuffer*)pBuff.opq;
        if(obj->src == NULL || obj->srcFmt.blockAlign <= 0){
            if(!nixAAudioSource_prepareSourceForFmt_(obj, &buff->desc)){
                NIX_PRINTF_ERROR("nixAAudioSource_queueBuffer, nixAAudioSource_prepareSourceForFmt_ failed.\n");
            } else {
                NIX_ASSERT(obj->src != NULL) //program logic error
            }
        }
        //
        if(obj->src == NULL){
            NIX_PRINTF_ERROR("nixAAudioSource_queueBuffer, no source available.\n");
        } else if(Nix_AAudioSource_isStatic(obj)){
            NIX_PRINTF_ERROR("nixAAudioSource_queueBuffer, source is static.\n");
        } else if(!STNix_audioDesc_IsEqual(&obj->buffsFmt, &buff->desc)){
            NIX_PRINTF_ERROR("nixAAudioSource_queueBuffer, new buffer doesnt match first buffer's format.\n");
        } else {
            //schedule
            if(!Nix_AAudioSource_queueBufferForOutput(obj, buff)){
                NIX_PRINTF_ERROR("nixAAudioSource_queueBuffer, Nix_AAudioSource_queueBufferForOutput failed.\n");
            } else {
                //restart stream if necesary
                if(obj->src != NULL && Nix_AAudioSource_isPlaying(obj) && !Nix_AAudioSource_isPaused(obj)){
                    aaudio_stream_state_t state = AAudioStream_getState(obj->src);
                    if(state != AAUDIO_STREAM_STATE_STARTING && state != AAUDIO_STREAM_STATE_STARTED){
                        if(AAUDIO_OK != AAudioStream_requestStart(obj->src)){
                            NIX_PRINTF_WARNING("nixAAudioSource_queueBuffer::AAudioStream_requestStart failed.\n");
                        } else {
                            NIX_PRINTF_INFO("nixAAudioSource_queueBuffer::AAudioStream_requestStart.\n");
                        }
                    }
                }
                r = NIX_TRUE;
            }
        }
    }
    return r;
}

//------
//Recorder (API)
//------

STNixApiRecorder nixAAudioRecorder_create(STNixApiEngine pEng, const STNix_audioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer){
    STNixApiRecorder r = STNixApiRecorder_Zero;
    STNix_AAudioEngine* eng = (STNix_AAudioEngine*)pEng.opq;
    if(eng != NULL && audioDesc != NULL && audioDesc->samplerate > 0 && audioDesc->blockAlign > 0 && eng->rec == NULL){
        STNix_AAudioRecorder* obj = NULL;
        NIX_MALLOC(obj, STNix_AAudioRecorder, sizeof(STNix_AAudioRecorder), "STNix_AAudioRecorder");
        if(obj != NULL){
            Nix_AAudioRecorder_init(obj);
            if(!Nix_AAudioRecorder_prepare(obj, audioDesc, buffersCount, samplesPerBuffer)){
                NIX_PRINTF_ERROR("Nix_AAudioRecorder_create, Nix_AAudioRecorder_prepare failed.\n");
                Nix_AAudioRecorder_destroy(obj);
                NIX_FREE(obj);
                obj = NULL;
            } else {
                obj->engNx = eng;
                eng->rec = obj;
            }
        }
        r.opq = obj;
    }
    return r;
}

void nixAAudioRecorder_destroy(STNixApiRecorder pObj){
    STNix_AAudioRecorder* obj = (STNix_AAudioRecorder*)pObj.opq;
    if(obj != NULL){
        if(obj->engNx != NULL && obj->engNx->rec == obj){
            obj->engNx->rec = NULL;
        }
        Nix_AAudioRecorder_destroy(obj);
        NIX_FREE(obj);
        obj = NULL;
    }
}

NixBOOL nixAAudioRecorder_setCallback(STNixApiRecorder pObj, NixApiCaptureBufferFilledCallback callback, void* callbackData){
    NixBOOL r = NIX_FALSE;
    STNix_AAudioRecorder* obj = (STNix_AAudioRecorder*)pObj.opq;
    if(obj != NULL){
        r = Nix_AAudioRecorder_setCallback(obj, callback, callbackData);
    }
    return r;
}

NixBOOL nixAAudioRecorder_start(STNixApiRecorder pObj){
    NixBOOL r = NIX_FALSE;
    STNix_AAudioRecorder* obj = (STNix_AAudioRecorder*)pObj.opq;
    if(obj != NULL){
        r = Nix_AAudioRecorder_start(obj);
    }
    return r;
}

NixBOOL nixAAudioRecorder_stop(STNixApiRecorder pObj){
    NixBOOL r = NIX_FALSE;
    STNix_AAudioRecorder* obj = (STNix_AAudioRecorder*)pObj.opq;
    if(obj != NULL){
        r = Nix_AAudioRecorder_stop(obj);
    }
    return r;
}
