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
//This file adds support for OpenAL for playing and recording.
//

#include "nixtla-audio-private.h"
#include "nixtla-audio.h"
#include "nixtla-openal.h"
#include <string.h> //for memset()

#ifdef __MAC_OS_X_VERSION_MAX_ALLOWED
#   include <OpenAL/al.h>
#   include <OpenAL/alc.h>
#else
#   include <AL/al.h>
#   include <AL/alc.h>
#endif

#define NIX_OPENAL_NULL     AL_NONE

#ifdef NIX_ASSERTS_ACTIVATED
#   define NIX_OPENAL_ERR_VERIFY(nomFunc)   { ALenum idErrorAL=alGetError(); if(idErrorAL != AL_NO_ERROR){ NIX_PRINTF_ERROR("'%s' (#%d) en %s\n", alGetString(idErrorAL), idErrorAL, nomFunc);} NIX_ASSERT(idErrorAL == AL_NO_ERROR);}
#else
#   define NIX_OPENAL_ERR_VERIFY(nomFunc)   { }
#endif

//------
//API Itf
//------

//OpenAL interface

//Engine
STNixApiEngineRef  nixOpenALEngine_alloc(STNixContextItf* ctx);
void            nixOpenALEngine_free(STNixApiEngineRef ref);
void            nixOpenALEngine_printCaps(STNixApiEngineRef ref);
NixBOOL         nixOpenALEngine_ctxIsActive(STNixApiEngineRef ref);
NixBOOL         nixOpenALEngine_ctxActivate(STNixApiEngineRef ref);
NixBOOL         nixOpenALEngine_ctxDeactivate(STNixApiEngineRef ref);
void            nixOpenALEngine_tick(STNixApiEngineRef ref);
//Source
STNixApiSourceRef  nixOpenALSource_alloc(STNixApiEngineRef eng);
void            nixOpenALSource_free(STNixApiSourceRef ref);
void            nixOpenALSource_setCallback(STNixApiSourceRef ref, void (*callback)(void* pEng, const NixUI32 sourceIndex, const NixUI32 ammBuffs), void* callbackEng, NixUI32 callbackSourceIndex);
NixBOOL         nixOpenALSource_setVolume(STNixApiSourceRef ref, const float vol);
NixBOOL         nixOpenALSource_setRepeat(STNixApiSourceRef ref, const NixBOOL isRepeat);
void            nixOpenALSource_play(STNixApiSourceRef ref);
void            nixOpenALSource_pause(STNixApiSourceRef ref);
void            nixOpenALSource_stop(STNixApiSourceRef ref);
NixBOOL         nixOpenALSource_isPlaying(STNixApiSourceRef ref);
NixBOOL         nixOpenALSource_isPaused(STNixApiSourceRef ref);
NixBOOL         nixOpenALSource_setBuffer(STNixApiSourceRef ref, STNixApiBufferRef buff);  //static-source
NixBOOL         nixOpenALSource_queueBuffer(STNixApiSourceRef ref, STNixApiBufferRef buff); //stream-source
//Recorder
STNixApiRecorderRef nixOpenALRecorder_alloc(STNixApiEngineRef eng, const STNixAudioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer);
void            nixOpenALRecorder_free(STNixApiRecorderRef ref);
NixBOOL         nixOpenALRecorder_setCallback(STNixApiRecorderRef ref, NixApiRecorderCallback callback, void* callbackData);
NixBOOL         nixOpenALRecorder_start(STNixApiRecorderRef ref);
NixBOOL         nixOpenALRecorder_stop(STNixApiRecorderRef ref);

NixBOOL nixOpenALEngine_getApiItf(STNixApiItf* dst){
    NixBOOL r = NIX_FALSE;
    if(dst != NULL){
        memset(dst, 0, sizeof(*dst));
        dst->engine.alloc       = nixOpenALEngine_alloc;
        dst->engine.free        = nixOpenALEngine_free;
        dst->engine.printCaps   = nixOpenALEngine_printCaps;
        dst->engine.ctxIsActive = nixOpenALEngine_ctxIsActive;
        dst->engine.ctxActivate = nixOpenALEngine_ctxActivate;
        dst->engine.ctxDeactivate = nixOpenALEngine_ctxDeactivate;
        dst->engine.tick        = nixOpenALEngine_tick;
        //PCMBuffer
        NixPCMBuffer_getApiItf(&dst->buffer);
        //Source
        dst->source.alloc       = nixOpenALSource_alloc;
        dst->source.free        = nixOpenALSource_free;
        dst->source.setCallback = nixOpenALSource_setCallback;
        dst->source.setVolume   = nixOpenALSource_setVolume;
        dst->source.setRepeat   = nixOpenALSource_setRepeat;
        dst->source.play        = nixOpenALSource_play;
        dst->source.pause       = nixOpenALSource_pause;
        dst->source.stop        = nixOpenALSource_stop;
        dst->source.isPlaying   = nixOpenALSource_isPlaying;
        dst->source.isPaused    = nixOpenALSource_isPaused;
        dst->source.setBuffer   = nixOpenALSource_setBuffer;  //static-source
        dst->source.queueBuffer = nixOpenALSource_queueBuffer; //stream-source
        //Recorder
        dst->recorder.alloc     = nixOpenALRecorder_alloc;
        dst->recorder.free      = nixOpenALRecorder_free;
        dst->recorder.setCallback   = nixOpenALRecorder_setCallback;
        dst->recorder.start     = nixOpenALRecorder_start;
        dst->recorder.stop      = nixOpenALRecorder_stop;
        //
        r = NIX_TRUE;
    }
    return r;
}

struct STNixOpenALEngine_;
struct STNixOpenALSource_;
struct STNixOpenALSourceCallback_;
struct STNixOpenALQueue_;
struct STNixOpenALQueuePair_;
struct STNixOpenALSrcNotif_;
struct STNixOpenALNotifQueue_;
struct STNixOpenALRecorder_;

//------
//Engine
//------

typedef struct STNixOpenALEngine_ {
    STNixContextItf ctx;
    STNixApiItf     apiItf;
    NixUI32         maskCapabilities;
    NixBOOL         contextALIsCurrent;
    ALCcontext*     contextAL;
    ALCdevice*      deviceAL;
    ALCdevice*      idCaptureAL;                //OpenAL specific
    NixUI32         captureMainBufferBytesCount;    //OpenAL specific
    //srcs
    struct {
        STNixMutex                  mutex;
        struct STNixOpenALSource_** arr;
        NixUI32                     use;
        NixUI32                     sz;
    } srcs;
    struct STNixOpenALRecorder_* rec;
} STNixOpenALEngine;

void NixOpenALEngine_init(STNixContextItf* ctx, STNixOpenALEngine* obj);
void NixOpenALEngine_destroy(STNixOpenALEngine* obj);
NixBOOL NixOpenALEngine_srcsAdd(STNixOpenALEngine* obj, struct STNixOpenALSource_* src);
void NixOpenALEngine_tick(STNixOpenALEngine* obj, const NixBOOL isFinalCleanup);

//------
//Notif
//------

typedef struct STNixOpenALSourceCallback_ {
    void            (*func)(void* pEng, const NixUI32 sourceIndex, NixUI32 ammBuffs);
    void*           eng;
    NixUI32         sourceIndex;
} STNixOpenALSourceCallback;

typedef struct STNixOpenALSrcNotif_ {
    STNixOpenALSourceCallback callback;
    NixUI32 ammBuffs;
} STNixOpenALSrcNotif;

void NixOpenALSrcNotif_init(STNixOpenALSrcNotif* obj);
void NixOpenALSrcNotif_destroy(STNixOpenALSrcNotif* obj);

//------
//NotifQueue
//------

typedef struct STNixOpenALNotifQueue_ {
    STNixContextItf*        ctx;
    STNixOpenALSrcNotif*    arr;
    NixUI32                 use;
    NixUI32                 sz;
    STNixOpenALSrcNotif     arrEmbedded[32];
} STNixOpenALNotifQueue;

void NixOpenALNotifQueue_init(STNixContextItf* ctx, STNixOpenALNotifQueue* obj);
void NixOpenALNotifQueue_destroy(STNixOpenALNotifQueue* obj);
//
NixBOOL NixOpenALNotifQueue_push(STNixOpenALNotifQueue* obj, STNixOpenALSrcNotif* pair);

//------
//QueuePair (Buffers)
//------

typedef struct STNixOpenALQueuePair_ {
    STNixApiBufferRef   org;    //original buffer (owned by the user)
    ALuint              idBufferAL; //converted buffer (owned by the source)
} STNixOpenALQueuePair;

void NixOpenALQueuePair_init(STNixOpenALQueuePair* obj);
void NixOpenALQueuePair_destroy(STNixOpenALQueuePair* obj);

//------
//Queue (Buffers)
//------

typedef struct STNixOpenALQueue_ {
    STNixContextItf*        ctx;
    STNixOpenALQueuePair*   arr;
    NixUI32                 use;
    NixUI32                 sz;
} STNixOpenALQueue;

void NixOpenALQueue_init(STNixContextItf* ctx, STNixOpenALQueue* obj);
void NixOpenALQueue_destroy(STNixOpenALQueue* obj);
//
NixBOOL NixOpenALQueue_flush(STNixOpenALQueue* obj);
NixBOOL NixOpenALQueue_prepareForSz(STNixOpenALQueue* obj, const NixUI32 minSz);
NixBOOL NixOpenALQueue_pushOwning(STNixOpenALQueue* obj, STNixOpenALQueuePair* pair);
NixBOOL NixOpenALQueue_popOrphaning(STNixOpenALQueue* obj, STNixOpenALQueuePair* dst);
NixBOOL NixOpenALQueue_popMovingTo(STNixOpenALQueue* obj, STNixOpenALQueue* other);

//------
//Source
//------

typedef struct STNixOpenALSource_ {
    STNixContextItf*        ctx;
    struct STNixOpenALEngine_* eng;    //parent engine
    STNixAudioDesc          buffsFmt;   //first attached buffers' format (defines the converter config)
    STNixAudioDesc          srcFmt;
    ALenum                  srcFmtAL;
    ALuint                  idSourceAL;
    //queues
    struct {
        STNixMutex          mutex;
        //conv
        struct {
            void*           obj;   //nixFmtConverter
            //buff
            struct {
                void*       ptr;
                NixUI32     sz;
            } buff;
        } conv;
        STNixOpenALSourceCallback callback;
        STNixOpenALQueue   notify; //buffers (consumed, pending to notify)
        STNixOpenALQueue   reuse;  //buffers (conversion buffers)
        STNixOpenALQueue   pend;   //to be played/filled
        NixUI32             pendSampleIdx;  //current sample playing/filling
    } queues;
    //props
    float                   volume;
    NixUI8                  stateBits;  //packed bools to reduce padding, NIX_OpenALSource_BIT_
} STNixOpenALSource;

void NixOpenALSource_init(STNixContextItf* ctx, STNixOpenALSource* obj);
void NixOpenALSource_destroy(STNixOpenALSource* obj);
NixBOOL NixOpenALSource_queueBufferForOutput(STNixOpenALSource* obj, STNixApiBufferRef buff, const NixBOOL isStream);
NixBOOL NixOpenALSource_pendPopOldestBuffLocked_(STNixOpenALSource* obj);
NixBOOL NixOpenALSource_pendMoveAllBuffsToNotifyWithoutPoppingLocked_(STNixOpenALSource* obj);

#define NIX_OpenALSource_BIT_isStatic   (0x1 << 0)  //source expects only one buffer, repeats or pauses after playing it
#define NIX_OpenALSource_BIT_isChanging (0x1 << 1)  //source is changing state after a call to request*()
#define NIX_OpenALSource_BIT_isRepeat   (0x1 << 2)
#define NIX_OpenALSource_BIT_isPlaying  (0x1 << 3)
#define NIX_OpenALSource_BIT_isPaused   (0x1 << 4)
#define NIX_OpenALSource_BIT_isClosing  (0x1 << 5)
#define NIX_OpenALSource_BIT_isOrphan   (0x1 << 6)  //source is waiting for close(), wait for the change of state and NixOpenALSource_release + free.
//
#define NixOpenALSource_isStatic(OBJ)          (((OBJ)->stateBits & NIX_OpenALSource_BIT_isStatic) != 0)
#define NixOpenALSource_isChanging(OBJ)        (((OBJ)->stateBits & NIX_OpenALSource_BIT_isChanging) != 0)
#define NixOpenALSource_isRepeat(OBJ)          (((OBJ)->stateBits & NIX_OpenALSource_BIT_isRepeat) != 0)
#define NixOpenALSource_isPlaying(OBJ)         (((OBJ)->stateBits & NIX_OpenALSource_BIT_isPlaying) != 0)
#define NixOpenALSource_isPaused(OBJ)          (((OBJ)->stateBits & NIX_OpenALSource_BIT_isPaused) != 0)
#define NixOpenALSource_isClosing(OBJ)         (((OBJ)->stateBits & NIX_OpenALSource_BIT_isClosing) != 0)
#define NixOpenALSource_isOrphan(OBJ)          (((OBJ)->stateBits & NIX_OpenALSource_BIT_isOrphan) != 0)
//
#define NixOpenALSource_setIsStatic(OBJ, V)    (OBJ)->stateBits = (V ? (OBJ)->stateBits | NIX_OpenALSource_BIT_isStatic : (OBJ)->stateBits & ~NIX_OpenALSource_BIT_isStatic)
#define NixOpenALSource_setIsChanging(OBJ, V)  (OBJ)->stateBits = (V ? (OBJ)->stateBits | NIX_OpenALSource_BIT_isChanging : (OBJ)->stateBits & ~NIX_OpenALSource_BIT_isChanging)
#define NixOpenALSource_setIsRepeat(OBJ, V)    (OBJ)->stateBits = (V ? (OBJ)->stateBits | NIX_OpenALSource_BIT_isRepeat : (OBJ)->stateBits & ~NIX_OpenALSource_BIT_isRepeat)
#define NixOpenALSource_setIsPlaying(OBJ, V)   (OBJ)->stateBits = (V ? (OBJ)->stateBits | NIX_OpenALSource_BIT_isPlaying : (OBJ)->stateBits & ~NIX_OpenALSource_BIT_isPlaying)
#define NixOpenALSource_setIsPaused(OBJ, V)    (OBJ)->stateBits = (V ? (OBJ)->stateBits | NIX_OpenALSource_BIT_isPaused : (OBJ)->stateBits & ~NIX_OpenALSource_BIT_isPaused)
#define NixOpenALSource_setIsClosing(OBJ)      (OBJ)->stateBits = ((OBJ)->stateBits | NIX_OpenALSource_BIT_isClosing)
#define NixOpenALSource_setIsOrphan(OBJ)       (OBJ)->stateBits = ((OBJ)->stateBits | NIX_OpenALSource_BIT_isOrphan)

//------
//Recorder
//------

typedef struct STNixOpenALRecorder_ {
    STNixContextItf*        ctx;
    NixBOOL                 engStarted;
    STNixApiEngineRef       engRef;
    STNixApiRecorderRef     selfRef;
    ALuint                  idCaptureAL;
    STNixAudioDesc         capFmt;
    //callback
    struct {
        NixApiRecorderCallback func;
        void*               data;
    } callback;
    //cfg
    struct {
        STNixAudioDesc     fmt;
        NixUI16             samplesPerBuffer;
        NixUI16             maxBuffers;
    } cfg;
    //queues
    struct {
        STNixMutex          mutex;
        void*               conv;   //nixFmtConverter
        STNixOpenALQueue    notify;
        STNixOpenALQueue    reuse;
        //filling
        struct {
            NixUI8*         tmp;
            NixUI32         tmpSz;
            NixSI32         iCurSample; //at first buffer in 'reuse'
        } filling;
    } queues;
} STNixOpenALRecorder;

void NixOpenALRecorder_init(STNixContextItf* ctx, STNixOpenALRecorder* obj);
void NixOpenALRecorder_destroy(STNixOpenALRecorder* obj);
//
NixBOOL NixOpenALRecorder_prepare(STNixOpenALRecorder* obj, STNixOpenALEngine* eng, const STNixAudioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer);
NixBOOL NixOpenALRecorder_setCallback(STNixOpenALRecorder* obj, NixApiRecorderCallback callback, void* callbackData);
NixBOOL NixOpenALRecorder_start(STNixOpenALRecorder* obj);
NixBOOL NixOpenALRecorder_stop(STNixOpenALRecorder* obj);
NixBOOL NixOpenALRecorder_flush(STNixOpenALRecorder* obj);
void NixOpenALRecorder_consumeInputBuffer(STNixOpenALRecorder* obj);
void NixOpenALRecorder_notifyBuffers(STNixOpenALRecorder* obj);

//------
//Engine
//------

void NixOpenALEngine_init(STNixContextItf* ctx, STNixOpenALEngine* obj){
    memset(obj, 0, sizeof(STNixOpenALEngine));
    //
    obj->ctx = *ctx;
    nixOpenALEngine_getApiItf(&obj->apiItf);
    //
    obj->deviceAL = NIX_OPENAL_NULL;
    obj->contextAL = NIX_OPENAL_NULL;
    //srcs
    {
        obj->srcs.mutex = (ctx->mutex.alloc)(ctx);
    }
}
  
void NixOpenALEngine_destroy(STNixOpenALEngine* obj){
    STNixContextItf* ctx = &obj->ctx;
    //srcs
    {
        //cleanup
        while(obj->srcs.arr != NULL && obj->srcs.use > 0){
            NixOpenALEngine_tick(obj, NIX_TRUE);
        }
        //
        if(obj->srcs.arr != NULL){
            (*ctx->mem.free)(obj->srcs.arr);
            obj->srcs.arr = NULL;
        }
        (*obj->ctx.mutex.free)(&obj->srcs.mutex);
    }
    //api
    if(alcMakeContextCurrent(NULL) == AL_FALSE){
        NIX_PRINTF_ERROR("alcMakeContextCurrent(NULL) failed\n");
    } else {
        obj->contextALIsCurrent = NIX_FALSE;
        alcDestroyContext(obj->contextAL); NIX_OPENAL_ERR_VERIFY("alcDestroyContext");
        if(!alcCloseDevice(obj->deviceAL)){
            NIX_PRINTF_ERROR("alcCloseDevice failed\n");
        } NIX_OPENAL_ERR_VERIFY("alcCloseDevice");
        obj->deviceAL = NIX_OPENAL_NULL;
        obj->contextAL = NIX_OPENAL_NULL;
    }
    //rec (recorder)
    if(obj->rec != NULL){
        obj->rec = NULL;
    }
}

NixBOOL NixOpenALEngine_srcsAdd(STNixOpenALEngine* obj, struct STNixOpenALSource_* src){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL){
        STNixContextItf* ctx = &obj->ctx;
        (*ctx->mutex.lock)(&obj->srcs.mutex);
        {
            //resize array (if necesary)
            if(obj->srcs.use >= obj->srcs.sz){
                const NixUI32 szN = obj->srcs.use + 4;
                STNixOpenALSource** arrN = (STNixOpenALSource**)(*ctx->mem.malloc)(sizeof(STNixOpenALSource*) * szN, "STNixOpenALEngine::srcsN");
                if(arrN != NULL){
                    if(obj->srcs.arr != NULL){
                        if(obj->srcs.use > 0){
                            memcpy(arrN, obj->srcs.arr, sizeof(arrN[0]) * obj->srcs.use);
                        }
                        (*ctx->mem.free)(obj->srcs.arr);
                        obj->srcs.arr = NULL;
                    }
                    obj->srcs.arr = arrN;
                    obj->srcs.sz = szN;
                }
            }
            //add
            if(obj->srcs.use >= obj->srcs.sz){
                NIX_PRINTF_ERROR("nixOpenALSource_create::STNixOpenALEngine::srcs failed (no allocated space).\n");
            } else {
                //become the owner of the pair
                obj->srcs.arr[obj->srcs.use++] = src;
                r = NIX_TRUE;
            }
        }
        (*ctx->mutex.unlock)(&obj->srcs.mutex);
    }
    return r;
}

void NixOpenALEngine_removeSrcRecordLocked_(STNixOpenALEngine* obj, NixSI32* idx){
    STNixContextItf* ctx = &obj->ctx;
    STNixOpenALSource* src = obj->srcs.arr[*idx];
    if(src != NULL){
        NixOpenALSource_destroy(src);
        (*ctx->mem.free)(src);
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

void NixOpenALEngine_tick_addQueueNotifSrcLocked_(STNixOpenALNotifQueue* notifs, STNixOpenALSource* srcLocked){
    if(srcLocked->queues.notify.use > 0){
        STNixOpenALSrcNotif n;
        NixOpenALSrcNotif_init(&n);
        n.callback = srcLocked->queues.callback;
        n.ammBuffs = srcLocked->queues.notify.use;
        if(!NixOpenALQueue_flush(&srcLocked->queues.notify)){
            NIX_ASSERT(NIX_FALSE); //program logic error
        }
        if(!NixOpenALNotifQueue_push(notifs, &n)){
            NIX_ASSERT(NIX_FALSE); //program logic error
            NixOpenALSrcNotif_destroy(&n);
        }
    }
}

void NixOpenALEngine_tick(STNixOpenALEngine* obj, const NixBOOL isFinalCleanup){
    if(obj != NULL){
        STNixContextItf* ctx = &obj->ctx;
        //srcs
        {
            STNixOpenALNotifQueue notifs;
            NixOpenALNotifQueue_init(ctx, &notifs);
            (*ctx->mutex.lock)(&obj->srcs.mutex);
            if(obj->srcs.arr != NULL && obj->srcs.use > 0){
                //NIX_PRINTF_INFO("NixOpenALEngine_tick::%d sources.\n", obj->srcs.use);
                NixSI32 i; for(i = 0; i < (NixSI32)obj->srcs.use; ++i){
                    STNixOpenALSource* src = obj->srcs.arr[i];
                    //NIX_PRINTF_INFO("NixOpenALEngine_tick::source(#%d/%d).\n", i + 1, obj->srcs.use);
                    if(src->idSourceAL == NIX_OPENAL_NULL){
                        //remove
                        //NIX_PRINTF_INFO("NixOpenALEngine_tick::source(#%d/%d); remove-NULL.\n", i + 1, obj->srcs.use);
                        NixOpenALEngine_removeSrcRecordLocked_(obj, &i);
                        src = NULL;
                    } else {
                        //remove processed buffers
                        if(src != NULL && !NixOpenALSource_isStatic(src)){
                            ALint csmdAmm = 0;
                            alGetSourceiv(src->idSourceAL, AL_BUFFERS_PROCESSED, &csmdAmm); NIX_OPENAL_ERR_VERIFY("alGetSourceiv(AL_BUFFERS_PROCESSED)");
                            if(csmdAmm > 0){
                                (*ctx->mutex.lock)(&src->queues.mutex);
                                {
                                    NIX_ASSERT(csmdAmm <= src->queues.pend.use) //Just checking, this should be always be true
                                    while(csmdAmm > 0){
                                        if(!NixOpenALSource_pendPopOldestBuffLocked_(src)){
                                            NIX_ASSERT(NIX_FALSE) //program logic error
                                            break;
                                        }
                                        --csmdAmm;
                                    }
                                }
                                (*ctx->mutex.unlock)(&src->queues.mutex);
                            }
                        }
                        //post-process
                        if(src != NULL){
                            //add to notify queue
                            {
                                (*ctx->mutex.lock)(&src->queues.mutex);
                                {
                                    NixOpenALEngine_tick_addQueueNotifSrcLocked_(&notifs, src);
                                }
                                (*ctx->mutex.unlock)(&src->queues.mutex);
                            }
                        }
                    }
                }
            }
            (*ctx->mutex.unlock)(&obj->srcs.mutex);
            //notify (unloked)
            if(notifs.use > 0){
                //NIX_PRINTF_INFO("NixOpenALEngine_tick::notify %d.\n", notifs.use);
                NixUI32 i;
                for(i = 0; i < notifs.use; ++i){
                    STNixOpenALSrcNotif* n = &notifs.arr[i];
                    //NIX_PRINTF_INFO("NixOpenALEngine_tick::notify(#%d/%d).\n", i + 1, notifs.use);
                    if(n->callback.func != NULL){
                        (*n->callback.func)(n->callback.eng, n->callback.sourceIndex, n->ammBuffs);
                    }
                }
            }
            //NIX_PRINTF_INFO("NixOpenALEngine_tick::NixOpenALNotifQueue_destroy.\n");
            NixOpenALNotifQueue_destroy(&notifs);
        }
        //recorder
        if(obj->rec != NULL){
            //Note: when the capture is stopped, ALC_CAPTURE_SAMPLES returns the maximun size instead of the captured-only size.
            if(obj->rec->engStarted){
                NixOpenALRecorder_consumeInputBuffer(obj->rec);
            }
            NixOpenALRecorder_notifyBuffers(obj->rec);
        }
    }
}

//------
//Notif
//------

void NixOpenALSrcNotif_init(STNixOpenALSrcNotif* obj){
    memset(obj, 0, sizeof(*obj));
}

void NixOpenALSrcNotif_destroy(STNixOpenALSrcNotif* obj){
    //
}

//------
//NotifQueue
//------

void NixOpenALNotifQueue_init(STNixContextItf* ctx, STNixOpenALNotifQueue* obj){
    memset(obj, 0, sizeof(*obj));
    obj->ctx = ctx;
    obj->arr = obj->arrEmbedded;
    obj->sz = (sizeof(obj->arrEmbedded) / sizeof(obj->arrEmbedded[0]));
}

void NixOpenALNotifQueue_destroy(STNixOpenALNotifQueue* obj){
    if(obj->arr != NULL){
        NixUI32 i; for(i = 0; i < obj->use; i++){
            STNixOpenALSrcNotif* b = &obj->arr[i];
            NixOpenALSrcNotif_destroy(b);
        }
        if(obj->arr != obj->arrEmbedded){
            (*obj->ctx->mem.free)(obj->arr);
        }
        obj->arr = NULL;
    }
    obj->use = obj->sz = 0;
}

NixBOOL NixOpenALNotifQueue_push(STNixOpenALNotifQueue* obj, STNixOpenALSrcNotif* pair){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL && pair != NULL){
        //resize array (if necesary)
        if(obj->use >= obj->sz){
            const NixUI32 szN = obj->use + 4;
            STNixOpenALSrcNotif* arrN = (STNixOpenALSrcNotif*)(*obj->ctx->mem.malloc)(sizeof(STNixOpenALSrcNotif) * szN, "NixOpenALNotifQueue_push::arrN");
            if(arrN != NULL){
                if(obj->arr != NULL){
                    if(obj->use > 0){
                        memcpy(arrN, obj->arr, sizeof(arrN[0]) * obj->use);
                    }
                    if(obj->arr != obj->arrEmbedded){
                        (*obj->ctx->mem.free)(obj->arr);
                    }
                    obj->arr = NULL;
                }
                obj->arr = arrN;
                obj->sz = szN;
            }
        }
        //add
        if(obj->use >= obj->sz){
            NIX_PRINTF_ERROR("NixOpenALNotifQueue_push failed (no allocated space).\n");
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

void NixOpenALQueuePair_init(STNixOpenALQueuePair* obj){
    memset(obj, 0, sizeof(*obj));
    obj->idBufferAL = NIX_OPENAL_NULL;
}

void NixOpenALQueuePair_destroy(STNixOpenALQueuePair* obj){
    {
        NixApiBufferRef_release(&obj->org);
        obj->org.ptr = NULL;
    }
    if(obj->idBufferAL != NIX_OPENAL_NULL){
        alDeleteBuffers(1, &obj->idBufferAL); NIX_OPENAL_ERR_VERIFY("alDeleteBuffers");
        obj->idBufferAL = NIX_OPENAL_NULL;
    }
}

//------
//Queue (Buffers)
//------

void NixOpenALQueue_init(STNixContextItf* ctx, STNixOpenALQueue* obj){
    memset(obj, 0, sizeof(*obj));
    obj->ctx = ctx;
}

void NixOpenALQueue_destroy(STNixOpenALQueue* obj){
    if(obj->arr != NULL){
        NixUI32 i; for(i = 0; i < obj->use; i++){
            STNixOpenALQueuePair* b = &obj->arr[i];
            NixOpenALQueuePair_destroy(b);
        }
        (*obj->ctx->mem.free)(obj->arr);
        obj->arr = NULL;
    }
    obj->use = obj->sz = 0;
}

NixBOOL NixOpenALQueue_flush(STNixOpenALQueue* obj){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL){
        if(obj->arr != NULL){
            NixUI32 i; for(i = 0; i < obj->use; i++){
                STNixOpenALQueuePair* b = &obj->arr[i];
                NixOpenALQueuePair_destroy(b);
            }
            obj->use = 0;
        }
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL NixOpenALQueue_prepareForSz(STNixOpenALQueue* obj, const NixUI32 minSz){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL){
        //resize array (if necesary)
        if(minSz > obj->sz){
            const NixUI32 szN = minSz;
            STNixOpenALQueuePair* arrN = (STNixOpenALQueuePair*)(*obj->ctx->mem.malloc)(sizeof(STNixOpenALQueuePair) * szN, "NixOpenALQueue_prepareForSz::arrN");
            if(arrN != NULL){
                if(obj->arr != NULL){
                    if(obj->use > 0){
                        memcpy(arrN, obj->arr, sizeof(arrN[0]) * obj->use);
                    }
                    (*obj->ctx->mem.free)(obj->arr);
                    obj->arr = NULL;
                }
                obj->arr = arrN;
                obj->sz = szN;
            }
        }
        //analyze
        if(minSz > obj->sz){
            NIX_PRINTF_ERROR("NixOpenALQueue_prepareForSz failed (no allocated space).\n");
        } else {
            r = NIX_TRUE;
        }
    }
    return r;
}

NixBOOL NixOpenALQueue_pushOwning(STNixOpenALQueue* obj, STNixOpenALQueuePair* pair){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL && pair != NULL){
        //resize array (if necesary)
        if(obj->use >= obj->sz){
            const NixUI32 szN = obj->use + 4;
            STNixOpenALQueuePair* arrN = (STNixOpenALQueuePair*)(*obj->ctx->mem.malloc)(sizeof(STNixOpenALQueuePair) * szN, "NixOpenALQueue_pushOwning::arrN");
            if(arrN != NULL){
                if(obj->arr != NULL){
                    if(obj->use > 0){
                        memcpy(arrN, obj->arr, sizeof(arrN[0]) * obj->use);
                    }
                    (*obj->ctx->mem.free)(obj->arr);
                    obj->arr = NULL;
                }
                obj->arr = arrN;
                obj->sz = szN;
            }
        }
        //add
        if(obj->use >= obj->sz){
            NIX_PRINTF_ERROR("NixOpenALQueue_pushOwning failed (no allocated space).\n");
        } else {
            //become the owner of the pair
            obj->arr[obj->use++] = *pair;
            r = NIX_TRUE;
        }
    }
    return r;
}

NixBOOL NixOpenALQueue_popOrphaning(STNixOpenALQueue* obj, STNixOpenALQueuePair* dst){
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

NixBOOL NixOpenALQueue_popMovingTo(STNixOpenALQueue* obj, STNixOpenALQueue* other){
    NixBOOL r = NIX_FALSE;
    STNixOpenALQueuePair pair;
    if(!NixOpenALQueue_popOrphaning(obj, &pair)){
        //
    } else {
        if(!NixOpenALQueue_pushOwning(other, &pair)){
            //program logic error
            NIX_ASSERT(NIX_FALSE);
            NixOpenALQueuePair_destroy(&pair);
        } else {
            r = NIX_TRUE;
        }
    }
    return r;
}

//------
//Source
//------

void NixOpenALSource_init(STNixContextItf* ctx, STNixOpenALSource* obj){
    memset(obj, 0, sizeof(STNixOpenALSource));
    obj->ctx = ctx;
    obj->idSourceAL = NIX_OPENAL_NULL;
    //queues
    {
        obj->queues.mutex = (ctx->mutex.alloc)(ctx);
        NixOpenALQueue_init(ctx, &obj->queues.notify);
        NixOpenALQueue_init(ctx, &obj->queues.pend);
        NixOpenALQueue_init(ctx, &obj->queues.reuse);
    }
}

void NixOpenALSource_destroy(STNixOpenALSource* obj){
    //src
    if(obj->idSourceAL != NIX_OPENAL_NULL){
        alSourceStop(obj->idSourceAL); NIX_OPENAL_ERR_VERIFY("alSourceStop");
        alDeleteSources(1, &obj->idSourceAL); NIX_OPENAL_ERR_VERIFY("alDeleteSources");
        obj->idSourceAL = NIX_OPENAL_NULL;
    }
    //queues
    {
        {
            if(obj->queues.conv.buff.ptr != NULL){
                (*obj->ctx->mem.free)(obj->queues.conv.buff.ptr);
                obj->queues.conv.buff.ptr = NULL;
            }
            obj->queues.conv.buff.sz = 0;
        }
        if(obj->queues.conv.obj != NULL){
            nixFmtConverter_free(obj->queues.conv.obj);
            obj->queues.conv.obj = NULL;
        }
        NixOpenALQueue_destroy(&obj->queues.pend);
        NixOpenALQueue_destroy(&obj->queues.reuse);
        NixOpenALQueue_destroy(&obj->queues.notify);
        (*obj->ctx->mutex.free)(&obj->queues.mutex);
    }
}

NixBOOL NixOpenALSource_queueBufferForOutput(STNixOpenALSource* obj, STNixApiBufferRef pBuff, const NixBOOL isStream){
    NixBOOL r = NIX_FALSE;
    if(pBuff.ptr != NULL){
        STNixPCMBuffer* buff = (STNixPCMBuffer*)NixSharedPtr_getOpq(pBuff.ptr);
        if(!STNixAudioDesc_IsEqual(&obj->buffsFmt, &buff->desc)){
            //error
        } else {
            STNixAudioDesc dataFmt;
            void* data = NULL;
            NixUI32 dataSz = 0;
            if(obj->queues.conv.obj == NULL){
                data = buff->ptr;
                dataSz = buff->use;
                dataFmt = buff->desc;
            } else {
                //populate with converted data
                const NixUI32 buffSamples = (buff->use / buff->desc.blockAlign);
                const NixUI32 buffConvSz = (buffSamples * obj->srcFmt.blockAlign);
                //resize cnv buffer (if necesary)
                if(obj->queues.conv.buff.sz < buffConvSz){
                    void* cnvBuffN = (void*)(*obj->ctx->mem.malloc)(buffConvSz, "cnvBuffSzN");
                    if(cnvBuffN != NULL){
                        if(obj->queues.conv.buff.ptr != NULL){
                            (*obj->ctx->mem.free)(obj->queues.conv.buff.ptr);
                            obj->queues.conv.buff.ptr = NULL;
                        }
                        obj->queues.conv.buff.ptr = cnvBuffN;
                        obj->queues.conv.buff.sz = buffConvSz;
                    }
                }
                //convert
                if(buffConvSz > obj->queues.conv.buff.sz){
                    NIX_PRINTF_ERROR("NixOpenALSource_queueBufferForOutput, could not allocate conversion buffer.\n");
                } else if(!nixFmtConverter_setPtrAtSrcInterlaced(obj->queues.conv.obj, &buff->desc, buff->ptr, 0)){
                    NIX_PRINTF_ERROR("nixFmtConverter_setPtrAtSrcInterlaced, failed.\n");
                } else if(!nixFmtConverter_setPtrAtDstInterlaced(obj->queues.conv.obj, &obj->srcFmt, obj->queues.conv.buff.ptr, 0)){
                    NIX_PRINTF_ERROR("nixFmtConverter_setPtrAtDstInterlaced, failed.\n");
                } else {
                    const NixUI32 srcBlocks = (buff->use / buff->desc.blockAlign);
                    const NixUI32 dstBlocks = (obj->queues.conv.buff.sz / obj->srcFmt.blockAlign);
                    NixUI32 ammBlocksRead = 0;
                    NixUI32 ammBlocksWritten = 0;
                    if(!nixFmtConverter_convert(obj->queues.conv.obj, srcBlocks, dstBlocks, &ammBlocksRead, &ammBlocksWritten)){
                        NIX_PRINTF_ERROR("NixOpenALSource_queueBufferForOutput::nixFmtConverter_convert failed from(%uhz, %uch, %dbit-%s) to(%uhz, %uch, %dbit-%s).\n"
                                         , obj->buffsFmt.samplerate
                                         , obj->buffsFmt.channels
                                         , obj->buffsFmt.bitsPerSample
                                         , obj->buffsFmt.samplesFormat == ENNixSampleFmt_Int ? "int" : obj->buffsFmt.samplesFormat == ENNixSampleFmt_Float ? "float" : "unknown"
                                         , obj->srcFmt.samplerate
                                         , obj->srcFmt.channels
                                         , obj->srcFmt.bitsPerSample
                                         , obj->srcFmt.samplesFormat == ENNixSampleFmt_Int ? "int" : obj->srcFmt.samplesFormat == ENNixSampleFmt_Float ? "float" : "unknown"
                                         );
                    } else {
                        data = obj->queues.conv.buff.ptr;
                        dataSz = ammBlocksWritten * obj->srcFmt.blockAlign;
                        dataFmt = obj->srcFmt;
                    }
                }
            }
            //
            if(data != NULL && dataSz > 0){
                STNixOpenALQueuePair pair;
                NixOpenALQueuePair_init(&pair);
                //reuse or create bufferAL
                {
                    STNixOpenALQueuePair reuse;
                    if(!NixOpenALQueue_popOrphaning(&obj->queues.reuse, &reuse)){
                        //no reusable buffer available, create new
                        ALenum errorAL;
                        alGenBuffers(1, &pair.idBufferAL);
                        if(AL_NONE != (errorAL = alGetError())){
                            NIX_PRINTF_ERROR("alGenBuffers failed: #%d '%s' idBufferAL(%d)\n", errorAL, alGetString(errorAL), pair.idBufferAL);
                            pair.idBufferAL = NIX_OPENAL_NULL;
                        }
                    } else {
                        //reuse buffer
                        NIX_ASSERT(reuse.org.ptr == NULL) //program logic error
                        NIX_ASSERT(reuse.idBufferAL != NIX_OPENAL_NULL) //program logic error
                        if(reuse.idBufferAL == NIX_OPENAL_NULL){
                            NIX_PRINTF_ERROR("NixOpenALSource_queueBufferForOutput::reuse.cnv should not be NULL.\n");
                        } else {
                            pair.idBufferAL = reuse.idBufferAL; reuse.idBufferAL = NIX_OPENAL_NULL; //consume
                        }
                        NixOpenALQueuePair_destroy(&reuse);
                    }
                }
                //populate bufferAL
                if(pair.idBufferAL != NIX_OPENAL_NULL){
                    ALenum errorAL;
                    alBufferData(pair.idBufferAL, obj->srcFmtAL, data, dataSz, dataFmt.samplerate);
                    if(AL_NONE != (errorAL = alGetError())){
                        NIX_PRINTF_ERROR("alBufferData failed: #%d '%s' idBufferAL(%d)\n", errorAL, alGetString(errorAL), pair.idBufferAL);
                        alDeleteBuffers(1, &pair.idBufferAL); NIX_OPENAL_ERR_VERIFY("alDeleteBuffers");
                        pair.idBufferAL = NIX_OPENAL_NULL;
                    }
                }
                //
                if(pair.idBufferAL != NIX_OPENAL_NULL){
                    r = NIX_TRUE;
                    //link buffer to source
                    {
                        if(isStream){
                            //queue buffer
                            ALenum errorAL;
                            alSourceQueueBuffers(obj->idSourceAL, 1, &pair.idBufferAL);
                            if(AL_NONE != (errorAL = alGetError())){
                                r = NIX_FALSE;
                            } else if(NixOpenALSource_isPlaying(obj) && !NixOpenALSource_isPaused(obj)){
                                //start playing if necesary
                                ALint sourceState;
                                alGetSourcei(obj->idSourceAL, AL_SOURCE_STATE, &sourceState);    NIX_OPENAL_ERR_VERIFY("alGetSourcei(AL_SOURCE_STATE)");
                                if(sourceState != AL_PLAYING){
                                    alSourcePlay(obj->idSourceAL);
                                }
                            }
                        } else {
                            //set buffer
                            ALenum errorAL;
                            alSourcei(obj->idSourceAL, AL_BUFFER, pair.idBufferAL);
                            if(AL_NONE != (errorAL = alGetError())){
                                r = NIX_FALSE;
                            }
                        }
                    }
                    //
                    if(r){
                        pair.org = pBuff;
                        NixApiBufferRef_retain(&pair.org);
                        (*obj->ctx->mutex.lock)(&obj->queues.mutex);
                        {
                            if(!NixOpenALQueue_pushOwning(&obj->queues.pend, &pair)){
                                NIX_PRINTF_ERROR("NixOpenALSource_queueBufferForOutput::NixOpenALQueue_pushOwning failed.\n");
                                NixApiBufferRef_release(&pair.org);
                                pair.org.ptr = NULL;
                                r = NIX_FALSE;
                            } else {
                                //added to queue
                                NixOpenALQueue_prepareForSz(&obj->queues.reuse, obj->queues.pend.use); //this ensures malloc wont be calle inside a callback
                                NixOpenALQueue_prepareForSz(&obj->queues.notify, obj->queues.pend.use); //this ensures malloc wont be calle inside a callback
                                //this is the first buffer i the queue
                                if(obj->queues.pend.use == 1){
                                    obj->queues.pendSampleIdx = 0;
                                }
                                r = NIX_TRUE;
                            }
                        }
                        (*obj->ctx->mutex.unlock)(&obj->queues.mutex);
                    }
                }
                if(!r){
                    NixOpenALQueuePair_destroy(&pair);
                }
            }
        }
    }
    return r;
}

NixBOOL NixOpenALSource_pendPopOldestBuffLocked_(STNixOpenALSource* obj){
    NixBOOL r = NIX_FALSE;
    STNixOpenALQueuePair pair;
    if(!NixOpenALQueue_popOrphaning(&obj->queues.pend, &pair)){
        NIX_ASSERT(NIX_FALSE); //program logic error
    } else {
        //move "cnv" to reusable queue
        if(pair.idBufferAL != NIX_OPENAL_NULL){
            ALenum errorAL;
            ALuint idBufferAL = pair.idBufferAL;
            alSourceUnqueueBuffers(obj->idSourceAL, 1, &idBufferAL);
            if(AL_NO_ERROR != (errorAL = alGetError())){
                NIX_PRINTF_ERROR("alSourceUnqueueBuffers failed: #%d '%s' idBufferAL(%d)\n", errorAL, alGetString(errorAL), pair.idBufferAL);
            } else {
                STNixOpenALQueuePair reuse;
                NixOpenALQueuePair_init(&reuse);
                reuse.idBufferAL = pair.idBufferAL;
                if(!NixOpenALQueue_pushOwning(&obj->queues.reuse, &reuse)){
                    NIX_PRINTF_ERROR("NixOpenALSource_pendPopOldestBuffLocked_::NixOpenALQueue_pushOwning(reuse) failed.\n");
                    reuse.idBufferAL = NIX_OPENAL_NULL;
                    NixOpenALQueuePair_destroy(&reuse);
                } else {
                    //now owned by reuse
                    pair.idBufferAL = NIX_OPENAL_NULL;
                }
            }
        }
        //move "org" to notify queue
        if(pair.org.ptr != NULL){
            STNixOpenALQueuePair notif;
            NixOpenALQueuePair_init(&notif);
            notif.org = pair.org;
            if(!NixOpenALQueue_pushOwning(&obj->queues.notify, &notif)){
                NIX_PRINTF_ERROR("NixOpenALSource_pendPopOldestBuffLocked_::NixOpenALQueue_pushOwning(notify) failed.\n");
                NixOpenALQueuePair_destroy(&notif);
            } else {
                //now owned by reuse
                pair.org.ptr = NULL;
            }
        }
        NIX_ASSERT(pair.org.ptr == NULL); //program logic error
        NIX_ASSERT(pair.idBufferAL == NIX_OPENAL_NULL); //program logic error
        NixOpenALQueuePair_destroy(&pair);
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL NixOpenALSource_pendMoveAllBuffsToNotifyWithoutPoppingLocked_(STNixOpenALSource* obj){
    NixBOOL r = NIX_TRUE;
    NixUI32 i; for(i = 0; i < obj->queues.pend.use; i++){
        STNixOpenALQueuePair* pair = &obj->queues.pend.arr[i];
        //move "org" to notify queue
        if(pair->org.ptr != NULL){
            STNixOpenALQueuePair notif;
            NixOpenALQueuePair_init(&notif);
            notif.org = pair->org;
            if(!NixOpenALQueue_pushOwning(&obj->queues.notify, &notif)){
                NIX_PRINTF_ERROR("NixOpenALSource_pendPopOldestBuffLocked_::NixOpenALQueue_pushOwning(notify) failed.\n");
                NixOpenALQueuePair_destroy(&notif);
            } else {
                //now owned by reuse
                pair->org.ptr = NULL;
            }
        }
        NIX_ASSERT(pair->org.ptr == NULL); //program logic error
    }
    return r;
}

//------
//Recorder
//------

void NixOpenALRecorder_init(STNixContextItf* ctx, STNixOpenALRecorder* obj){
    memset(obj, 0, sizeof(*obj));
    obj->ctx = ctx;
    obj->idCaptureAL = NIX_OPENAL_NULL;
    //cfg
    {
        //
    }
    //queues
    {
        obj->queues.mutex = (ctx->mutex.alloc)(ctx);
        NixOpenALQueue_init(ctx, &obj->queues.notify);
        NixOpenALQueue_init(ctx, &obj->queues.reuse);
    }
}

void NixOpenALRecorder_destroy(STNixOpenALRecorder* obj){
    //queues
    {
        (*obj->ctx->mutex.lock)(&obj->queues.mutex);
        {
            NixOpenALQueue_destroy(&obj->queues.notify);
            NixOpenALQueue_destroy(&obj->queues.reuse);
            if(obj->queues.conv != NULL){
                nixFmtConverter_free(obj->queues.conv);
                obj->queues.conv = NULL;
            }
            //tmp
            {
                if(obj->queues.filling.tmp != NULL){
                    (*obj->ctx->mem.free)(obj->queues.filling.tmp);
                    obj->queues.filling.tmp = NULL;
                }
                obj->queues.filling.tmpSz = 0;
            }
        }
        (*obj->ctx->mutex.unlock)(&obj->queues.mutex);
        (*obj->ctx->mutex.free)(&obj->queues.mutex);
    }
    //
    if(obj->idCaptureAL != NIX_OPENAL_NULL){
        ALenum errorAL;
        alcCaptureStop(obj->idCaptureAL);
        if(AL_NO_ERROR != (errorAL = alGetError())){
            NIX_PRINTF_ERROR("alcCaptureStop failed: #%d '%s'.\n", errorAL, alGetString(errorAL));
        }
        if(!alcCaptureCloseDevice(obj->idCaptureAL)){
            NIX_PRINTF_ERROR("alcCaptureCloseDevice failed\n");
        }
        obj->idCaptureAL = NIX_OPENAL_NULL;
    }
    if(obj->engRef.ptr != NULL){
        STNixOpenALEngine* eng = (STNixOpenALEngine*)NixSharedPtr_getOpq(obj->engRef.ptr);
        if(eng != NULL && eng->rec == obj){
            eng->rec = NULL;
        }
        NixApiEngineRef_release(&obj->engRef);
    }
}

NixBOOL NixOpenALRecorder_prepare(STNixOpenALRecorder* obj, STNixOpenALEngine* eng, const STNixAudioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer){
    NixBOOL r = NIX_FALSE;
    STNixContextItf* ctx = obj->ctx;
    (*obj->ctx->mutex.lock)(&obj->queues.mutex);
    if(obj->queues.conv == NULL && audioDesc->blockAlign > 0){
        STNixAudioDesc inDesc = STNixAudioDesc_Zero;
        ALenum apiFmt = AL_UNDETERMINED;
        switch(audioDesc->channels){
            case 1:
                switch(audioDesc->bitsPerSample){
                    case 8:
                        apiFmt = AL_FORMAT_MONO8;
                        inDesc.samplesFormat   = ENNixSampleFmt_Int;
                        inDesc.bitsPerSample   = 8;
                        inDesc.channels        = audioDesc->channels;
                        inDesc.samplerate      = audioDesc->samplerate;
                        break;
                    default:
                        apiFmt = AL_FORMAT_MONO16;
                        inDesc.samplesFormat   = ENNixSampleFmt_Int;
                        inDesc.bitsPerSample   = 16;
                        inDesc.channels        = audioDesc->channels;
                        inDesc.samplerate      = audioDesc->samplerate;
                        break;
                }
                break;
            case 2:
                switch(audioDesc->bitsPerSample){
                    case 8:
                        apiFmt = AL_FORMAT_STEREO8;
                        inDesc.samplesFormat   = ENNixSampleFmt_Int;
                        inDesc.bitsPerSample   = 8;
                        inDesc.channels        = audioDesc->channels;
                        inDesc.samplerate      = audioDesc->samplerate;
                        break;
                    default:
                        apiFmt = AL_FORMAT_STEREO16;
                        inDesc.samplesFormat   = ENNixSampleFmt_Int;
                        inDesc.bitsPerSample   = 16;
                        inDesc.channels        = audioDesc->channels;
                        inDesc.samplerate      = audioDesc->samplerate;
                        break;
                }
                break;
                break;
            default:
                break;
        }
        inDesc.blockAlign = (inDesc.bitsPerSample / 8) * inDesc.channels;
        //
        if(apiFmt == AL_UNDETERMINED){
            NIX_PRINTF_ERROR("Unsupported format for OpenAL (2 channels max)\n");
        } else {
            const NixUI32 capPerBuffSz = inDesc.blockAlign * samplesPerBuffer;
            const NixUI32 capMainBuffSz = capPerBuffSz * 2;
            ALenum errAL;
            ALuint capDev = alcCaptureOpenDevice(NULL/*devName*/, inDesc.samplerate, apiFmt, capMainBuffSz);
            if (AL_NO_ERROR != (errAL = alGetError())) {
                NIX_PRINTF_ERROR("alcCaptureOpenDevice failed\n");
            } else {
                //tmp
                {
                    if(obj->queues.filling.tmp != NULL){
                        (*ctx->mem.free)(obj->queues.filling.tmp);
                        obj->queues.filling.tmp = NULL;
                    }
                    obj->queues.filling.tmpSz = 0;
                }
                NixUI8* tmpBuff = (NixUI8*)(*ctx->mem.malloc)(capMainBuffSz, "tmpBuff");
                if(tmpBuff == NULL){
                    NIX_PRINTF_ERROR("NixOpenALRecorder_prepare::allocation of temporary buffer failed.\n");
                } else {
                    void* conv = nixFmtConverter_alloc(ctx);
                    if(!nixFmtConverter_prepare(conv, &inDesc, audioDesc)){
                        NIX_PRINTF_ERROR("NixOpenALRecorder_prepare::nixFmtConverter_prepare failed.\n");
                        nixFmtConverter_free(conv);
                        conv = NULL;
                    } else {
                        //allocate reusable buffers
                        while(obj->queues.reuse.use < buffersCount){
                            STNixOpenALQueuePair pair;
                            NixOpenALQueuePair_init(&pair);
                            pair.org = (eng->apiItf.buffer.alloc)(&eng->ctx, audioDesc, NULL, audioDesc->blockAlign * samplesPerBuffer);
                            if(pair.org.ptr == NULL){
                                NIX_PRINTF_ERROR("NixOpenALRecorder_prepare::pair.org allocation failed.\n");
                                NixOpenALQueuePair_destroy(&pair);
                                break;
                            } else if(!NixOpenALQueue_pushOwning(&obj->queues.reuse, &pair)){
                                NixOpenALQueuePair_destroy(&pair);
                                break;
                            }
                        }
                        //
                        if(obj->queues.reuse.use <= 0){
                            NIX_PRINTF_ERROR("NixOpenALRecorder_prepare::no reusable buffer could be allocated.\n");
                        } else {
                            //prepared
                            obj->queues.filling.iCurSample = 0;
                            obj->queues.conv = conv; conv = NULL; //consume
                            obj->queues.filling.tmp = tmpBuff; tmpBuff = NULL; //consume
                            obj->queues.filling.tmpSz = capMainBuffSz;
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
                        nixFmtConverter_free(conv);
                        conv = NULL;
                    }
                }
                //release (if not consumed)
                if(tmpBuff != NULL){
                    (*ctx->mem.free)(tmpBuff);
                    tmpBuff = NULL;
                }
                obj->idCaptureAL = capDev;
            }
        }
    }
    (*obj->ctx->mutex.unlock)(&obj->queues.mutex);
    return r;
}

NixBOOL NixOpenALRecorder_setCallback(STNixOpenALRecorder* obj, NixApiRecorderCallback callback, void* callbackData){
    NixBOOL r = NIX_FALSE;
    {
        obj->callback.func = callback;
        obj->callback.data = callbackData;
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL NixOpenALRecorder_start(STNixOpenALRecorder* obj){
    NixBOOL r = NIX_TRUE;
    if(!obj->engStarted){
        if(obj->idCaptureAL != NIX_OPENAL_NULL){
            ALenum errorAL;
            alcCaptureStart(obj->idCaptureAL);
            if(AL_NO_ERROR != (errorAL = alGetError())){
                NIX_PRINTF_ERROR("alcCaptureStart failed: #%d '%s'.\n", errorAL, alGetString(errorAL));
                r = NIX_FALSE;
            } else {
                obj->engStarted = NIX_TRUE;
            }
        }
    }
    return r;
}

NixBOOL NixOpenALRecorder_stop(STNixOpenALRecorder* obj){
    NixBOOL r = NIX_TRUE;
    if(obj->idCaptureAL != NIX_OPENAL_NULL){
        ALenum errorAL;
        alcCaptureStop(obj->idCaptureAL);
        if(AL_NO_ERROR != (errorAL = alGetError())){
            NIX_PRINTF_ERROR("alcCaptureStop failed: #%d '%s'.\n", errorAL, alGetString(errorAL));
        }
        obj->engStarted = NIX_FALSE;
    }
    NixOpenALRecorder_flush(obj);
    return r;
}

NixBOOL NixOpenALRecorder_flush(STNixOpenALRecorder* obj){
    NixBOOL r = NIX_TRUE;
    //move filling buffer to notify (if data is available)
    (*obj->ctx->mutex.lock)(&obj->queues.mutex);
    if(obj->queues.reuse.use > 0){
        STNixOpenALQueuePair* pair = &obj->queues.reuse.arr[0];
        if(pair->org.ptr != NULL && ((STNixPCMBuffer*)NixSharedPtr_getOpq(pair->org.ptr))->use > 0){
            obj->queues.filling.iCurSample = 0;
            if(!NixOpenALQueue_popMovingTo(&obj->queues.reuse, &obj->queues.notify)){
                //program logic error
                r = NIX_FALSE;
            }
        }
    }
    (*obj->ctx->mutex.unlock)(&obj->queues.mutex);
    return r;
}

void NixOpenALRecorder_consumeInputBuffer(STNixOpenALRecorder* obj){
    if(obj->queues.conv != NULL && obj->idCaptureAL != NIX_OPENAL_NULL){
        NixUI32 inIdx = 0;
        ALCint inSz = 0;
        //get samples
        {
            ALenum errorAL;
            alcGetIntegerv(obj->idCaptureAL, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(inSz), &inSz);
            if(AL_NO_ERROR != (errorAL = alGetError())){
                NIX_PRINTF_ERROR("alcGetIntegerv(ALC_CAPTURE_SAMPLES) failed with error #%d\n", (NixSI32)errorAL, alGetString(errorAL));
                inSz = 0;
            } else {
                NIX_ASSERT(inSz >= 0)
                const NixUI32 tmpSzInSamples = (obj->queues.filling.tmpSz / obj->capFmt.blockAlign);
                if(inSz > tmpSzInSamples){
                    inSz = tmpSzInSamples;
                }
                if(inSz > 0){
                    alcCaptureSamples(obj->idCaptureAL, (ALCvoid *)obj->queues.filling.tmp, inSz);
                    if(AL_NO_ERROR != (errorAL = alGetError())){
                        NIX_PRINTF_ERROR("alcCaptureSamples failed with error #%d\n", (NixSI32)errorAL, alGetString(errorAL));
                        inSz = 0;
                    } else {
                        //
                    }
                }
            }
        }
        (*obj->ctx->mutex.lock)(&obj->queues.mutex);
        {
            //process
            while(inIdx < inSz){
                if(obj->queues.reuse.use <= 0){
                    //move oldest-notify buffer to reuse
                    if(!NixOpenALQueue_popMovingTo(&obj->queues.notify, &obj->queues.reuse)){
                        //program logic error
                        NIX_ASSERT(NIX_FALSE);
                        break;
                    }
                } else {
                    STNixOpenALQueuePair* pair = &obj->queues.reuse.arr[0];
                    if(pair->org.ptr == NULL || ((STNixPCMBuffer*)NixSharedPtr_getOpq(pair->org.ptr))->desc.blockAlign <= 0){
                        //just remove
                        STNixOpenALQueuePair pair;
                        if(!NixOpenALQueue_popOrphaning(&obj->queues.reuse, &pair)){
                            NIX_ASSERT(NIX_FALSE);
                            //program logic error
                            break;
                        }
                        NixOpenALQueuePair_destroy(&pair);
                    } else {
                        STNixPCMBuffer* org = (STNixPCMBuffer*)NixSharedPtr_getOpq(pair->org.ptr);
                        const NixUI32 outSz = (org->sz / org->desc.blockAlign);
                        const NixUI32 outAvail = (obj->queues.filling.iCurSample >= outSz ? 0 : outSz - obj->queues.filling.iCurSample);
                        const NixUI32 inAvail = inSz - inIdx;
                        NixUI32 ammBlocksRead = 0, ammBlocksWritten = 0;
                        if(outAvail > 0 && inAvail > 0){
                            //dst
                            nixFmtConverter_setPtrAtDstInterlaced(obj->queues.conv, &org->desc, org->ptr, obj->queues.filling.iCurSample);
                            //src
                            nixFmtConverter_setPtrAtSrcInterlaced(obj->queues.conv, &obj->capFmt, obj->queues.filling.tmp, inIdx);
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
                                org->use = (obj->queues.filling.iCurSample * org->desc.blockAlign); NIX_ASSERT(org->use <= org->sz)
                            }
                        }
                        //move reusable buffer to notify
                        if(ammBlocksWritten == outAvail){
                            obj->queues.filling.iCurSample = 0;
                            if(!NixOpenALQueue_popMovingTo(&obj->queues.reuse, &obj->queues.notify)){
                                //program logic error
                                NIX_ASSERT(NIX_FALSE);
                                break;
                            }
                        }
                    }
                }
            }
        }
        (*obj->ctx->mutex.unlock)(&obj->queues.mutex);
    }
}

void NixOpenALRecorder_notifyBuffers(STNixOpenALRecorder* obj){
    (*obj->ctx->mutex.lock)(&obj->queues.mutex);
    {
        const NixUI32 maxProcess = obj->queues.notify.use;
        NixUI32 ammProcessed = 0;
        while(ammProcessed < maxProcess && obj->queues.notify.use > 0){
            STNixOpenALQueuePair pair;
            if(!NixOpenALQueue_popOrphaning(&obj->queues.notify, &pair)){
                NIX_ASSERT(NIX_FALSE);
                //program logic error
                break;
            } else {
                //notify (unlocked)
                if(pair.org.ptr != NULL && ((STNixPCMBuffer*)NixSharedPtr_getOpq(pair.org.ptr))->desc.blockAlign > 0 && obj->callback.func != NULL){
                    STNixPCMBuffer* org = (STNixPCMBuffer*)NixSharedPtr_getOpq(pair.org.ptr);
                    (*obj->ctx->mutex.unlock)(&obj->queues.mutex);
                    {
                        (*obj->callback.func)(obj->engRef, obj->selfRef, org->desc, org->ptr, org->use, (org->use / org->desc.blockAlign), obj->callback.data);
                    }
                    (*obj->ctx->mutex.lock)(&obj->queues.mutex);
                }
                //move to reuse
                if(!NixOpenALQueue_pushOwning(&obj->queues.reuse, &pair)){
                    //program logic error
                    NIX_ASSERT(NIX_FALSE);
                    NixOpenALQueuePair_destroy(&pair);
                }
            }
            //processed
            ++ammProcessed;
        }
    }
    (*obj->ctx->mutex.unlock)(&obj->queues.mutex);
}

//------
//Engine API
//------

STNixApiEngineRef nixOpenALEngine_alloc(STNixContextItf* ctx){
    STNixApiEngineRef r = STNixApiEngineRef_Zero;
    STNixOpenALEngine* obj = (STNixOpenALEngine*)(*ctx->mem.malloc)(sizeof(STNixOpenALEngine), "STNixOpenALEngine");
    if(obj != NULL){
        NixOpenALEngine_init(ctx, obj);
        obj->deviceAL = alcOpenDevice(NULL);
        if(obj->deviceAL == NULL){
            NIX_PRINTF_ERROR("OpenAL::alcOpenDevice failed.\n");
            obj->deviceAL = NIX_OPENAL_NULL;
        } else {
            obj->contextAL = alcCreateContext(obj->deviceAL, NULL);
            if(obj->contextAL == NULL){
                NIX_PRINTF_ERROR("OpenAL::alcCreateContext failed\n");
                obj->contextAL = NIX_OPENAL_NULL;
            } else {
                if(alcMakeContextCurrent(obj->contextAL) == AL_FALSE){
                    NIX_PRINTF_ERROR("OpenAL::alcMakeContextCurrent failed\n");
                } else if(NULL == (r.ptr = NixSharedPtr_alloc(ctx, obj))){
                    NIX_PRINTF_ERROR("nixAAudioEngine_create::NixSharedPtr_alloc failed.\n");
                } else {
                    obj->contextALIsCurrent = NIX_TRUE;
                    //Masc of capabilities
                    obj->maskCapabilities   |= (alcIsExtensionPresent(obj->deviceAL, "ALC_EXT_CAPTURE") != ALC_FALSE || alcIsExtensionPresent(obj->deviceAL, "ALC_EXT_capture") != ALC_FALSE) ? NIX_CAP_AUDIO_CAPTURE : 0;
                    obj->maskCapabilities   |= (alIsExtensionPresent("AL_EXT_STATIC_BUFFER") != AL_FALSE) ? NIX_CAP_AUDIO_STATIC_BUFFERS : 0;
                    obj->maskCapabilities   |= (alIsExtensionPresent("AL_EXT_OFFSET") != AL_FALSE) ? NIX_CAP_AUDIO_SOURCE_OFFSETS : 0;
                    //
                    r.itf = &obj->apiItf.engine;
                    obj = NULL; //consume
                }
            }
        }
        //release (if not consumed)
        if(obj != NULL){
            NixOpenALEngine_destroy(obj);
            (*ctx->mem.free)(obj);
            obj = NULL;
        }
    }
    return r;
}

void nixOpenALEngine_free(STNixApiEngineRef pObj){
    if(pObj.ptr != NULL){
        STNixOpenALEngine* obj = (STNixOpenALEngine*)NixSharedPtr_getOpq(pObj.ptr);
        if(obj != NULL){
            NixOpenALEngine_destroy(obj);
            (*obj->ctx.mem.free)(obj);
            obj = NULL;
        }
        NixSharedPtr_destroy(pObj.ptr);
    }
    //itf belongs to Engine
}

void nixOpenALEngine_printCaps(STNixApiEngineRef pObj){
    STNixOpenALEngine* obj = (STNixOpenALEngine*)NixSharedPtr_getOpq(pObj.ptr);
    if(obj != NULL && obj->deviceAL != NIX_OPENAL_NULL){
        ALCint versionMayorALC, versionMenorALC;
        const char* strAlVersion;
        const char* strAlRenderer;
        const char* strAlVendor;
        const char* strAlExtensions;
        const char* strAlcExtensions;
        const char* defDeviceName;
        strAlVersion        = alGetString(AL_VERSION);    NIX_OPENAL_ERR_VERIFY("alGetString(AL_VERSION)");
        strAlRenderer       = alGetString(AL_RENDERER);    NIX_OPENAL_ERR_VERIFY("alGetString(AL_RENDERER)");
        strAlVendor         = alGetString(AL_VENDOR);    NIX_OPENAL_ERR_VERIFY("alGetString(AL_VENDOR)");
        strAlExtensions     = alGetString(AL_EXTENSIONS);    NIX_OPENAL_ERR_VERIFY("alGetString(AL_EXTENSIONS)");
        strAlcExtensions    = alcGetString(obj->deviceAL, ALC_EXTENSIONS); NIX_OPENAL_ERR_VERIFY("alcGetString(ALC_EXTENSIONS)");
        alcGetIntegerv(obj->deviceAL, ALC_MAJOR_VERSION, (ALCsizei)sizeof(versionMayorALC), &versionMayorALC);
        alcGetIntegerv(obj->deviceAL, ALC_MINOR_VERSION, (ALCsizei)sizeof(versionMenorALC), &versionMenorALC);
        //
        printf("----------- OPENAL -------------\n");
        printf("Version:          AL('%s') ALC(%d.%d):\n", strAlVersion, versionMayorALC, versionMenorALC);
        printf("Renderizador:     '%s'\n", strAlRenderer);
        printf("Vendedor:         '%s'\n", strAlVendor);
        printf("EXTCaptura:       %s\n", (obj->maskCapabilities & NIX_CAP_AUDIO_CAPTURE)?"supported":"unsupported");
        printf("EXTBuffEstaticos: %s\n", (obj->maskCapabilities & NIX_CAP_AUDIO_STATIC_BUFFERS)?"supported":"unsupported");
        printf("EXTOffsets:       %s\n", (obj->maskCapabilities & NIX_CAP_AUDIO_SOURCE_OFFSETS)?"supported":"unsupported");
        printf("Extensions AL:    '%s'\n", strAlExtensions);
        printf("Extensions ALC:   '%s'\n", strAlcExtensions);
        //List sound devices
        defDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER); NIX_OPENAL_ERR_VERIFY("alcGetString(ALC_DEFAULT_DEVICE_SPECIFIER)")
        printf("DefautlDevice:    '%s'\n", defDeviceName);
        {
            NixSI32 pos = 0, deviceCount = 0;
            const ALCchar* deviceList = alcGetString(NULL, ALC_DEVICE_SPECIFIER); NIX_OPENAL_ERR_VERIFY("alcGetString(ALC_DEVICE_SPECIFIER)")
            while(deviceList[pos]!='\0'){
                const char* strDevice = &(deviceList[pos]); NixUI32 strSize = 0;
                while(strDevice[strSize]!='\0') strSize++;
                pos += strSize + 1; deviceCount++;
                printf("Device #%d:        '%s'\n", deviceCount, strDevice);
            }
        }
        //List capture devices
        if(obj->maskCapabilities & NIX_CAP_AUDIO_CAPTURE){
            const char* defCaptureDeviceName = alcGetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER); NIX_OPENAL_ERR_VERIFY("alcGetString(ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER)")
            {
                NixSI32 pos = 0, deviceCount = 0;
                const ALCchar* deviceList = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER); NIX_OPENAL_ERR_VERIFY("alcGetString(ALC_CAPTURE_DEVICE_SPECIFIER)")
                while(deviceList[pos]!='\0'){
                    const char* strDevice = &(deviceList[pos]); NixUI32 strSize = 0;
                    while(strDevice[strSize]!='\0') strSize++;
                    pos += strSize + 1; deviceCount++;
                    printf("Capture #%d:       '%s'\n", deviceCount, strDevice);
                }
            }
            printf("DefautlCapture:   '%s'\n", defCaptureDeviceName);
        }
        
    }
}

NixBOOL nixOpenALEngine_ctxIsActive(STNixApiEngineRef pObj){
    NixBOOL r = NIX_FALSE;
    STNixOpenALEngine* obj = (STNixOpenALEngine*)NixSharedPtr_getOpq(pObj.ptr);
    if(obj != NULL){
        r = obj->contextALIsCurrent;
    }
    return r;
}

NixBOOL nixOpenALEngine_ctxActivate(STNixApiEngineRef pObj){
    NixBOOL r = NIX_FALSE;
    STNixOpenALEngine* obj = (STNixOpenALEngine*)NixSharedPtr_getOpq(pObj.ptr);
    if(obj != NULL){
        if(alcMakeContextCurrent(obj->contextAL) == AL_FALSE){
            NIX_PRINTF_ERROR("OpenAL::alcMakeContextCurrent(context) failed\n");
        } else {
            obj->contextALIsCurrent = NIX_TRUE;
            r = NIX_TRUE;
        }
    }
    return r;
}

NixBOOL nixOpenALEngine_ctxDeactivate(STNixApiEngineRef pObj){
    NixBOOL r = NIX_FALSE;
    STNixOpenALEngine* obj = (STNixOpenALEngine*)NixSharedPtr_getOpq(pObj.ptr);
    if(obj != NULL){
        if(alcMakeContextCurrent(NULL) == AL_FALSE){
            NIX_PRINTF_ERROR("OpenAL::alcMakeContextCurrent(NULL) failed\n");
        } else {
            obj->contextALIsCurrent = NIX_FALSE;
            r = NIX_TRUE;
        }
    }
    return r;
}

void nixOpenALEngine_tick(STNixApiEngineRef pObj){
    STNixOpenALEngine* obj = (STNixOpenALEngine*)NixSharedPtr_getOpq(pObj.ptr);
    if(obj != NULL){
        NixOpenALEngine_tick(obj, NIX_FALSE);
    }
}

//------
//Source API
//------

STNixApiSourceRef nixOpenALSource_alloc(STNixApiEngineRef pEng){
    STNixApiSourceRef r = STNixApiSourceRef_Zero;
    STNixOpenALEngine* eng = (STNixOpenALEngine*)NixSharedPtr_getOpq(pEng.ptr);
    if(eng != NULL){
        STNixContextItf* ctx = &eng->ctx;
        STNixOpenALSource* obj = (STNixOpenALSource*)(*ctx->mem.malloc)(sizeof(STNixOpenALSource), "STNixOpenALSource");
        if(obj != NULL){
            NixOpenALSource_init(ctx, obj);
            obj->ctx = ctx;
            obj->eng = eng;
            //
            ALenum errorAL;
            alGenSources(1, &obj->idSourceAL);
            if(AL_NO_ERROR != (errorAL = alGetError())){
                NIX_PRINTF_ERROR("alGenSources failed with error #%d\n", (NixSI32)errorAL);
                obj->idSourceAL = NIX_OPENAL_NULL;
            } else {
                obj->idSourceAL = obj->idSourceAL;
                //add to engine
                if(!NixOpenALEngine_srcsAdd(eng, obj)){
                    NIX_PRINTF_ERROR("nixOpenALSource_create::NixOpenALEngine_srcsAdd failed.\n");
                } else if(NULL == (r.ptr = NixSharedPtr_alloc(ctx, obj))){
                    NIX_PRINTF_ERROR("nixAAudioEngine_create::NixSharedPtr_alloc failed.\n");
                } else {
                    r.itf = &eng->apiItf.source;
                    obj = NULL; //consume
                }
            }
        }
        //release (if not consumed)
        if(obj != NULL){
            NixOpenALSource_destroy(obj);
            (*ctx->mem.free)(obj);
            obj = NULL;
        }
    }
    return r;
}

void nixOpenALSource_removeAllBuffersAndNotify_(STNixOpenALSource* obj){
    STNixContextItf* ctx = obj->ctx;
    STNixOpenALNotifQueue notifs;
    NixOpenALNotifQueue_init(ctx, &notifs);
    //move all pending buffers to notify
    (*obj->ctx->mutex.lock)(&obj->queues.mutex);
    {
        NixOpenALSource_pendMoveAllBuffsToNotifyWithoutPoppingLocked_(obj);
        NixOpenALEngine_tick_addQueueNotifSrcLocked_(&notifs, obj);
    }
    (*obj->ctx->mutex.unlock)(&obj->queues.mutex);
    //notify
    {
        NixUI32 i;
        for(i = 0; i < notifs.use; ++i){
            STNixOpenALSrcNotif* n = &notifs.arr[i];
            NIX_PRINTF_INFO("nixOpenALSource_removeAllBuffersAndNotify_::notify(#%d/%d).\n", i + 1, notifs.use);
            if(n->callback.func != NULL){
                (*n->callback.func)(n->callback.eng, n->callback.sourceIndex, n->ammBuffs);
            }
        }
    }
}

void nixOpenALSource_free(STNixApiSourceRef pObj){ //orphans the source, will automatically be destroyed after internal cleanup
    if(pObj.ptr != NULL){
        STNixOpenALSource* obj = (STNixOpenALSource*)NixSharedPtr_getOpq(pObj.ptr);
        if(obj != NULL){
            //close
            if(obj->idSourceAL != NIX_OPENAL_NULL){
                alSourceStop(obj->idSourceAL); NIX_OPENAL_ERR_VERIFY("alSourceStop");
            }
            NixOpenALSource_setIsOrphan(obj); //source is waiting for close(), wait for the change of state and NixOpenALSource_release + free.
            NixOpenALSource_setIsPlaying(obj, NIX_FALSE);
            NixOpenALSource_setIsPaused(obj, NIX_FALSE);
            //flush all pending buffers
            nixOpenALSource_removeAllBuffersAndNotify_(obj);
            //source orphaned, will be removed inside the "tick" method
        }
        NixSharedPtr_destroy(pObj.ptr);
    }
    //itf belongs to Engine
}

void nixOpenALSource_setCallback(STNixApiSourceRef pObj, void (*callback)(void* eng, const NixUI32 sourceIndex, const NixUI32 ammBuffs), void* callbackEng, NixUI32 callbackSourceIndex){
    if(pObj.ptr != NULL){
        STNixOpenALSource* obj = (STNixOpenALSource*)NixSharedPtr_getOpq(pObj.ptr);
        obj->queues.callback.func = callback;
        obj->queues.callback.eng = callbackEng;
        obj->queues.callback.sourceIndex = callbackSourceIndex;
    }
}

NixBOOL nixOpenALSource_setVolume(STNixApiSourceRef pObj, const float vol){
    NixBOOL r = NIX_FALSE;
    if(pObj.ptr != NULL){
        STNixOpenALSource* obj = (STNixOpenALSource*)NixSharedPtr_getOpq(pObj.ptr);
        obj->volume = vol;
        if(obj->idSourceAL != NIX_OPENAL_NULL){
            alSourcef(obj->idSourceAL, AL_GAIN, vol);
            NIX_OPENAL_ERR_VERIFY("alSourcef(AL_GAIN)");
        }
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL nixOpenALSource_setRepeat(STNixApiSourceRef pObj, const NixBOOL isRepeat){
    NixBOOL r = NIX_FALSE;
    if(pObj.ptr != NULL){
        STNixOpenALSource* obj = (STNixOpenALSource*)NixSharedPtr_getOpq(pObj.ptr);
        NixOpenALSource_setIsRepeat(obj, isRepeat);
        if(obj->idSourceAL != NIX_OPENAL_NULL){
            alSourcei(obj->idSourceAL, AL_LOOPING, isRepeat ? AL_TRUE : AL_FALSE); NIX_OPENAL_ERR_VERIFY("alSourcei(AL_LOOPING)");
        }
        r = NIX_TRUE;
    }
    return r;
}

void nixOpenALSource_play(STNixApiSourceRef pObj){
    if(pObj.ptr != NULL){
        STNixOpenALSource* obj = (STNixOpenALSource*)NixSharedPtr_getOpq(pObj.ptr);
        if(obj->idSourceAL != NIX_OPENAL_NULL){
            alSourcePlay(obj->idSourceAL);    NIX_OPENAL_ERR_VERIFY("alSourcePlay");
        }
        NixOpenALSource_setIsPlaying(obj, NIX_TRUE);
        NixOpenALSource_setIsPaused(obj, NIX_FALSE);
    }
}

void nixOpenALSource_pause(STNixApiSourceRef pObj){
    if(pObj.ptr != NULL){
        STNixOpenALSource* obj = (STNixOpenALSource*)NixSharedPtr_getOpq(pObj.ptr);
        if(obj->idSourceAL != NULL){
            alSourcePause(obj->idSourceAL);    NIX_OPENAL_ERR_VERIFY("alSourcePause");
        }
        NixOpenALSource_setIsPaused(obj, NIX_TRUE);
    }
}

void nixOpenALSource_stop(STNixApiSourceRef pObj){
    if(pObj.ptr != NULL){
        STNixOpenALSource* obj = (STNixOpenALSource*)NixSharedPtr_getOpq(pObj.ptr);
        if(obj->idSourceAL != NIX_OPENAL_NULL){
            alSourceStop(obj->idSourceAL); NIX_OPENAL_ERR_VERIFY("alSourceStop");
        }
        NixOpenALSource_setIsPlaying(obj, NIX_FALSE);
        NixOpenALSource_setIsPaused(obj, NIX_FALSE);
        //flush all pending buffers
        nixOpenALSource_removeAllBuffersAndNotify_(obj);
    }
}

NixBOOL nixOpenALSource_isPlaying(STNixApiSourceRef pObj){
    NixBOOL r = NIX_FALSE;
    if(pObj.ptr != NULL){
        STNixOpenALSource* obj = (STNixOpenALSource*)NixSharedPtr_getOpq(pObj.ptr);
        if(obj->idSourceAL != NIX_OPENAL_NULL){
            ALint sourceState;
            alGetSourcei(obj->idSourceAL, AL_SOURCE_STATE, &sourceState);    NIX_OPENAL_ERR_VERIFY("alGetSourcei(AL_SOURCE_STATE)");
            r = sourceState == AL_PLAYING ? NIX_TRUE : NIX_FALSE;
        }
    }
    return r;
}

NixBOOL nixOpenALSource_isPaused(STNixApiSourceRef pObj){
    NixBOOL r = NIX_FALSE;
    if(pObj.ptr != NULL){
        STNixOpenALSource* obj = (STNixOpenALSource*)NixSharedPtr_getOpq(pObj.ptr);
        r = NixOpenALSource_isPlaying(obj) && NixOpenALSource_isPaused(obj) ? NIX_TRUE : NIX_FALSE;
    }
    return r;
}

ALenum nixOpenALSource_alFormat(const STNixAudioDesc* fmt){
    ALenum dataFormat = AL_UNDETERMINED;
    switch(fmt->bitsPerSample){
        case 8:
            switch(fmt->channels){
                case 1:
                    dataFormat = AL_FORMAT_MONO8;
                    break;
                case 2:
                    dataFormat = AL_FORMAT_STEREO8;
                    break;
            }
            break;
        case 16:
            switch(fmt->channels){
                case 1:
                    dataFormat = AL_FORMAT_MONO16;
                    break;
                case 2:
                    dataFormat = AL_FORMAT_STEREO16;
                    break;
            }
            break;
        default:
            break;
    }
    return dataFormat;
}

NixBOOL nixOpenALSource_prepareSourceForFmtAndSz_(STNixOpenALSource* obj, const STNixAudioDesc* fmt, const NixUI32 buffSz){
    NixBOOL r = NIX_FALSE;
    STNixContextItf* ctx = obj->ctx;
    if(fmt != NULL && fmt->blockAlign > 0 && fmt->bitsPerSample > 0 && fmt->channels > 0 && fmt->samplerate > 0 && fmt->samplesFormat > ENNixSampleFmt_Unknown && fmt->samplesFormat <= ENNixSampleFmt_Count){
        ALenum fmtAL = nixOpenALSource_alFormat(fmt);
        if(fmtAL != AL_UNDETERMINED){
            //buffer format cmpatible qith OpenAL
            obj->buffsFmt   = *fmt;
            obj->srcFmtAL   = fmtAL;
            obj->srcFmt     = *fmt;
            r = NIX_TRUE;
        } else {
            //prepare converter
            void* conv = nixFmtConverter_alloc(ctx);
            STNixAudioDesc convFmt = STNixAudioDesc_Zero;
            convFmt.channels        = fmt->channels;
            convFmt.bitsPerSample   = 16;
            convFmt.samplerate      = fmt->samplerate;
            convFmt.samplesFormat   = ENNixSampleFmt_Int;
            convFmt.blockAlign      = (convFmt.bitsPerSample / 8) * convFmt.channels;
            if(!nixFmtConverter_prepare(conv, fmt, &convFmt)){
                NIX_PRINTF_ERROR("nixOpenALSource_prepareSourceForFmtAndSz_, nixFmtConverter_prepare failed.\n");
            } else {
                fmtAL = nixOpenALSource_alFormat(&convFmt);
                if(fmtAL == AL_UNDETERMINED){
                    NIX_PRINTF_ERROR("nixOpenALSource_prepareSourceForFmtAndSz_, nixFmtConverter_prepare sucess but OpenAL unsupported format.\n");
                } else {
                    //allocate conversion buffer
                    const NixUI32 srcSamples = buffSz / fmt->blockAlign * fmt->blockAlign;
                    const NixUI32 cnvBytes = srcSamples * convFmt.blockAlign;
                    NIX_ASSERT(obj->queues.conv.buff.ptr == NULL && obj->queues.conv.buff.sz == 0)
                    obj->queues.conv.buff.ptr = (void*)(*ctx->mem.malloc)(cnvBytes, "obj->queues.conv.buff.ptr");
                    if(obj->queues.conv.buff.ptr != NULL){
                        obj->queues.conv.buff.sz = cnvBytes;
                        obj->queues.conv.obj = conv; conv = NULL;
                        obj->buffsFmt   = *fmt;
                        obj->srcFmtAL   = fmtAL;
                        obj->srcFmt     = convFmt;
                        r = NIX_TRUE;
                    }
                }
            }
            //release (if not consumed)
            if(conv != NULL){
                nixFmtConverter_free(conv);
                conv = NULL;
            }
        }
    }
    return r;
}
    
NixBOOL nixOpenALSource_setBuffer(STNixApiSourceRef pObj, STNixApiBufferRef pBuff){  //static-source
    NixBOOL r = NIX_FALSE;
    if(pObj.ptr != NULL && pBuff.ptr != NULL){
        STNixOpenALSource* obj    = (STNixOpenALSource*)NixSharedPtr_getOpq(pObj.ptr);
        STNixPCMBuffer* buff = (STNixPCMBuffer*)NixSharedPtr_getOpq(pBuff.ptr);
        if(obj->buffsFmt.blockAlign == 0){
            if(!nixOpenALSource_prepareSourceForFmtAndSz_(obj, &buff->desc, buff->sz)){
                NIX_PRINTF_ERROR("nixOpenALSource_setBuffer::nixOpenALSource_prepareSourceForFmtAndSz_ failed.\n");
            }
        }
        //apply
        if(obj->idSourceAL == NIX_OPENAL_NULL){
            NIX_PRINTF_ERROR("nixOpenALSource_setBuffer, no source available.\n");
        } else if(obj->queues.pend.use != 0){
            NIX_PRINTF_ERROR("nixOpenALSource_setBuffer, source already has buffer.\n");
        } else if(NixOpenALSource_isStatic(obj)){
            NIX_PRINTF_ERROR("nixOpenALSource_setBuffer, source is already static.\n");
        } else if(!STNixAudioDesc_IsEqual(&obj->buffsFmt, &buff->desc)){
            NIX_PRINTF_ERROR("nixOpenALSource_setBuffer, new buffer doesnt match first buffer's format.\n");
        } else {
            NixOpenALSource_setIsStatic(obj, NIX_TRUE);
            //schedule
            NixBOOL isStream = NIX_FALSE;
            if(!NixOpenALSource_queueBufferForOutput(obj, pBuff, isStream)){
                NIX_PRINTF_ERROR("nixOpenALSource_setBuffer, NixOpenALSource_queueBufferForOutput failed.\n");
            } else {
                r = NIX_TRUE;
            }
        }
    }
    return r;
}

NixBOOL nixOpenALSource_queueBuffer(STNixApiSourceRef pObj, STNixApiBufferRef pBuff){
    NixBOOL r = NIX_FALSE;
    if(pObj.ptr != NULL && pBuff.ptr != NULL){
        STNixOpenALSource* obj    = (STNixOpenALSource*)NixSharedPtr_getOpq(pObj.ptr);
        STNixPCMBuffer* buff = (STNixPCMBuffer*)NixSharedPtr_getOpq(pBuff.ptr);
        if(obj->buffsFmt.blockAlign == 0){
            if(!nixOpenALSource_prepareSourceForFmtAndSz_(obj, &buff->desc, buff->sz)){
                //error
            }
        }
        //
        if(obj->idSourceAL == NIX_OPENAL_NULL){
            NIX_PRINTF_ERROR("nixOpenALSource_queueBuffer, no source available.\n");
        } else if(NixOpenALSource_isStatic(obj)){
            NIX_PRINTF_ERROR("nixOpenALSource_queueBuffer, source is static.\n");
        } else if(!STNixAudioDesc_IsEqual(&obj->buffsFmt, &buff->desc)){
            NIX_PRINTF_ERROR("nixOpenALSource_queueBuffer, new buffer doesnt match first buffer's format.\n");
        } else {
            //schedule
            NixBOOL isStream = NIX_TRUE;
            if(!NixOpenALSource_queueBufferForOutput(obj, pBuff, isStream)){
                NIX_PRINTF_ERROR("nixOpenALSource_queueBuffer, NixOpenALSource_queueBufferForOutput failed.\n");
            } else {
                r = NIX_TRUE;
            }
        }
    }
    return r;
}

//------
//Recorder API
//------

STNixApiRecorderRef nixOpenALRecorder_alloc(STNixApiEngineRef pEng, const STNixAudioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer){
    STNixApiRecorderRef r = STNixApiRecorderRef_Zero;
    STNixOpenALEngine* eng = (STNixOpenALEngine*)NixSharedPtr_getOpq(pEng.ptr);
    if(eng != NULL && audioDesc != NULL && audioDesc->samplerate > 0 && audioDesc->blockAlign > 0 && eng->rec == NULL){
        STNixContextItf* ctx = &eng->ctx;
        STNixOpenALRecorder* obj = (STNixOpenALRecorder*)(*ctx->mem.malloc)(sizeof(STNixOpenALRecorder), "STNixOpenALRecorder");
        if(obj != NULL){
            NixOpenALRecorder_init(ctx, obj);
            if(!NixOpenALRecorder_prepare(obj, eng, audioDesc, buffersCount, samplesPerBuffer)){
                NIX_PRINTF_ERROR("NixOpenALRecorder_create, NixOpenALRecorder_prepare failed.\n");
            } else if(NULL == (r.ptr = NixSharedPtr_alloc(ctx, obj))){
                NIX_PRINTF_ERROR("nixAAudioEngine_create::NixSharedPtr_alloc failed.\n");
            } else {
                r.itf = &eng->apiItf.recorder;
                obj->engRef     = pEng; NixApiEngineRef_retain(&pEng);
                obj->selfRef    = r;
                eng->rec        = obj; obj = NULL; //consume
            }
        }
        //release (if not consumed)
        if(obj != NULL){
            NixOpenALRecorder_destroy(obj);
            (*ctx->mem.free)(obj);
            obj = NULL;
        }
    }
    return r;
}

void nixOpenALRecorder_free(STNixApiRecorderRef pObj){
    if(pObj.ptr != NULL){
        STNixOpenALRecorder* obj = (STNixOpenALRecorder*)NixSharedPtr_getOpq(pObj.ptr);
        if(obj != NULL){
            NixOpenALRecorder_destroy(obj);
            (*obj->ctx->mem.free)(obj);
            obj = NULL;
        }
        NixSharedPtr_destroy(pObj.ptr);
    }
    //itf belongs to Engine
}

NixBOOL nixOpenALRecorder_setCallback(STNixApiRecorderRef pObj, NixApiRecorderCallback callback, void* callbackData){
    NixBOOL r = NIX_FALSE;
    STNixOpenALRecorder* obj = (STNixOpenALRecorder*)NixSharedPtr_getOpq(pObj.ptr);
    if(obj != NULL){
        r = NixOpenALRecorder_setCallback(obj, callback, callbackData);
    }
    return r;
}

NixBOOL nixOpenALRecorder_start(STNixApiRecorderRef pObj){
    NixBOOL r = NIX_FALSE;
    STNixOpenALRecorder* obj = (STNixOpenALRecorder*)NixSharedPtr_getOpq(pObj.ptr);
    if(obj != NULL){
        r = NixOpenALRecorder_start(obj);
    }
    return r;
}

NixBOOL nixOpenALRecorder_stop(STNixApiRecorderRef pObj){
    NixBOOL r = NIX_FALSE;
    STNixOpenALRecorder* obj = (STNixOpenALRecorder*)NixSharedPtr_getOpq(pObj.ptr);
    if(obj != NULL){
        r = NixOpenALRecorder_stop(obj);
    }
    return r;
}
