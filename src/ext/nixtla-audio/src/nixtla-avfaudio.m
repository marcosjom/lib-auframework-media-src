//
//  nixtla.m
//  NixtlaAudioLib
//
//  Created by Marcos Ortega on 06/07/25.
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
//This file adds support for AVFAudio for playing and recording in MacOS and iOS.
//

#include "nixtla-audio-private.h"
#include "nixtla-audio.h"
#include "nixtla-avfaudio.h"
//
#include <AVFAudio/AVAudioEngine.h>
#include <AVFAudio/AVAudioPlayerNode.h>
#include <AVFAudio/AVAudioMixerNode.h>
#include <AVFAudio/AVAudioFormat.h>
#include <AVFAudio/AVAudioChannelLayout.h>
#include <AVFAudio/AVAudioConverter.h>

//------
//API Itf
//------

//AVFAudio interface to objective-c (iOS/MacOS)

//Engine
STNixApiEngineRef  nixAVAudioEngine_alloc(STNixContextItf* ctx);
void            nixAVAudioEngine_free(STNixApiEngineRef ref);
void            nixAVAudioEngine_printCaps(STNixApiEngineRef ref);
NixBOOL         nixAVAudioEngine_ctxIsActive(STNixApiEngineRef ref);
NixBOOL         nixAVAudioEngine_ctxActivate(STNixApiEngineRef ref);
NixBOOL         nixAVAudioEngine_ctxDeactivate(STNixApiEngineRef ref);
void            nixAVAudioEngine_tick(STNixApiEngineRef ref);
//Source
STNixApiSourceRef  nixAVAudioSource_alloc(STNixApiEngineRef eng);
void            nixAVAudioSource_free(STNixApiSourceRef ref);
void            nixAVAudioSource_setCallback(STNixApiSourceRef ref, void (*callback)(void* eng, const NixUI32 sourceIndex, const NixUI32 ammBuffs), void* callbackEng, NixUI32 callbackSourceIndex);
NixBOOL         nixAVAudioSource_setVolume(STNixApiSourceRef ref, const float vol);
NixBOOL         nixAVAudioSource_setRepeat(STNixApiSourceRef ref, const NixBOOL isRepeat);
void            nixAVAudioSource_play(STNixApiSourceRef ref);
void            nixAVAudioSource_pause(STNixApiSourceRef ref);
void            nixAVAudioSource_stop(STNixApiSourceRef ref);
NixBOOL         nixAVAudioSource_isPlaying(STNixApiSourceRef ref);
NixBOOL         nixAVAudioSource_isPaused(STNixApiSourceRef ref);
NixBOOL         nixAVAudioSource_setBuffer(STNixApiSourceRef ref, STNixApiBufferRef buff);  //static-source
NixBOOL         nixAVAudioSource_queueBuffer(STNixApiSourceRef ref, STNixApiBufferRef buff); //stream-source
//Recorder
STNixApiRecorderRef nixAVAudioRecorder_alloc(STNixApiEngineRef eng, const STNixAudioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer);
void            nixAVAudioRecorder_free(STNixApiRecorderRef ref);
NixBOOL         nixAVAudioRecorder_setCallback(STNixApiRecorderRef ref, NixApiRecorderCallback callback, void* callbackData);
NixBOOL         nixAVAudioRecorder_start(STNixApiRecorderRef ref);
NixBOOL         nixAVAudioRecorder_stop(STNixApiRecorderRef ref);

NixBOOL nixAVAudioEngine_getApiItf(STNixApiItf* dst){
    NixBOOL r = NIX_FALSE;
    if(dst != NULL){
        memset(dst, 0, sizeof(*dst));
        dst->engine.alloc       = nixAVAudioEngine_alloc;
        dst->engine.free        = nixAVAudioEngine_free;
        dst->engine.printCaps   = nixAVAudioEngine_printCaps;
        dst->engine.ctxIsActive = nixAVAudioEngine_ctxIsActive;
        dst->engine.ctxActivate = nixAVAudioEngine_ctxActivate;
        dst->engine.ctxDeactivate = nixAVAudioEngine_ctxDeactivate;
        dst->engine.tick        = nixAVAudioEngine_tick;
        //PCMBuffer
        NixPCMBuffer_getApiItf(&dst->buffer);
        //Source
        dst->source.alloc       = nixAVAudioSource_alloc;
        dst->source.free        = nixAVAudioSource_free;
        dst->source.setCallback = nixAVAudioSource_setCallback;
        dst->source.setVolume   = nixAVAudioSource_setVolume;
        dst->source.setRepeat   = nixAVAudioSource_setRepeat;
        dst->source.play        = nixAVAudioSource_play;
        dst->source.pause       = nixAVAudioSource_pause;
        dst->source.stop        = nixAVAudioSource_stop;
        dst->source.isPlaying   = nixAVAudioSource_isPlaying;
        dst->source.isPaused    = nixAVAudioSource_isPaused;
        dst->source.setBuffer   = nixAVAudioSource_setBuffer;  //static-source
        dst->source.queueBuffer = nixAVAudioSource_queueBuffer; //stream-source
        //Recorder
        dst->recorder.alloc     = nixAVAudioRecorder_alloc;
        dst->recorder.free      = nixAVAudioRecorder_free;
        dst->recorder.setCallback = nixAVAudioRecorder_setCallback;
        dst->recorder.start     = nixAVAudioRecorder_start;
        dst->recorder.stop      = nixAVAudioRecorder_stop;
        //
        r = NIX_TRUE;
    }
    return r;
}

//

struct STNixAVAudioEngine_;
struct STNixAVAudioSource_;
struct STNixAVAudioSourceCallback_;
struct STNixAVAudioQueue_;
struct STNixAVAudioQueuePair_;
struct STNixAVAudioSrcNotif_;
struct STNixAVAudioNotifQueue_;
struct STNixAVAudioRecorder_;

//------
//Engine
//------

typedef struct STNixAVAudioEngine_ {
    STNixContextItf ctx;
    STNixApiItf     apiItf;
    //srcs
    struct {
        NIX_MUTEX_T                 mutex;
        struct STNixAVAudioSource_** arr;
        NixUI32                     use;
        NixUI32                     sz;
        NixUI32 changingStateCountHint;
    } srcs;
    //
    struct STNixAVAudioRecorder_* rec;
} STNixAVAudioEngine;

void NixAVAudioEngine_init(STNixAVAudioEngine* obj, STNixContextItf* ctx);
void NixAVAudioEngine_destroy(STNixAVAudioEngine* obj);
NixBOOL NixAVAudioEngine_srcsAdd(STNixAVAudioEngine* obj, struct STNixAVAudioSource_* src);
void NixAVAudioEngine_tick(STNixAVAudioEngine* obj, const NixBOOL isFinalCleanup);

//------
//Notif
//------

typedef struct STNixAVAudioSourceCallback_ {
    void            (*func)(void* pEng, const NixUI32 sourceIndex, const NixUI32 ammBuffs);
    void*           eng;
    NixUI32         sourceIndex;
} STNixAVAudioSourceCallback;

typedef struct STNixAVAudioSrcNotif_ {
    STNixAVAudioSourceCallback callback;
    NixUI32 ammBuffs;
} STNixAVAudioSrcNotif;

void NixAVAudioSrcNotif_init(STNixAVAudioSrcNotif* obj);
void NixAVAudioSrcNotif_destroy(STNixAVAudioSrcNotif* obj);

//------
//NotifQueue
//------

typedef struct STNixAVAudioNotifQueue_ {
    STNixAVAudioSrcNotif*  arr;
    NixUI32                use;
    NixUI32                sz;
    STNixAVAudioSrcNotif  arrEmbedded[32];
} STNixAVAudioNotifQueue;

void NixAVAudioNotifQueue_init(STNixAVAudioNotifQueue* obj);
void NixAVAudioNotifQueue_destroy(STNixAVAudioNotifQueue* obj);
//
NixBOOL NixAVAudioNotifQueue_push(STNixAVAudioNotifQueue* obj, STNixAVAudioSrcNotif* pair);

//------
//QueuePair (Buffers)
//------

typedef struct STNixAVAudioQueuePair_ {
    STNixApiBufferRef   org;    //original buffer (owned by the user)
    AVAudioPCMBuffer*   cnv;    //converted buffer (owned by the source)
} STNixAVAudioQueuePair;

void NixAVAudioQueuePair_init(STNixAVAudioQueuePair* obj);
void NixAVAudioQueuePair_destroy(STNixAVAudioQueuePair* obj);

//------
//Queue (Buffers)
//------

typedef struct STNixAVAudioQueue_ {
    STNixAVAudioQueuePair* arr;
    NixUI32                 use;
    NixUI32                 sz;
} STNixAVAudioQueue;

void NixAVAudioQueue_init(STNixAVAudioQueue* obj);
void NixAVAudioQueue_destroy(STNixAVAudioQueue* obj);
//
NixBOOL NixAVAudioQueue_flush(STNixAVAudioQueue* obj);
NixBOOL NixAVAudioQueue_prepareForSz(STNixAVAudioQueue* obj, const NixUI32 minSz);
NixBOOL NixAVAudioQueue_pushOwning(STNixAVAudioQueue* obj, STNixAVAudioQueuePair* pair);
NixBOOL NixAVAudioQueue_popOrphaning(STNixAVAudioQueue* obj, STNixAVAudioQueuePair* dst);
NixBOOL NixAVAudioQueue_popMovingTo(STNixAVAudioQueue* obj, STNixAVAudioQueue* other);

//------
//Source
//------

typedef struct STNixAVAudioSource_ {
    STNixContextItf*        ctx;
    struct STNixAVAudioEngine_* engp; //parent engine
    STNixAudioDesc         buffsFmt; //first attached buffers' format (defines the converter config)
    AVAudioPlayerNode*      src;    //AVAudioPlayerNode
    AVAudioEngine*          eng;    //AVAudioEngine
    //queues
    struct {
        NIX_MUTEX_T         mutex;
        void*               conv;   //nixFmtConverter
        STNixAVAudioSourceCallback callback;
        STNixAVAudioQueue  notify; //buffers (consumed, pending to notify)
        STNixAVAudioQueue  reuse;  //buffers (conversion buffers)
        STNixAVAudioQueue  pend;   //to be played/filled
        NixUI32             pendSampleIdx;  //current sample playing/filling
        NixUI32             pendScheduledCount;
    } queues;
    //packed bools to reduce padding
    NixBOOL                 engStarted;
    NixUI8                  stateBits;  //packed bools to reduce padding, NIX_AVAudioSource_BIT_
} STNixAVAudioSource;

void NixAVAudioSource_init(STNixAVAudioSource* obj);
void NixAVAudioSource_destroy(STNixAVAudioSource* obj);
void NixAVAudioSource_scheduleEnqueuedBuffers(STNixAVAudioSource* obj);
NixBOOL NixAVAudioSource_queueBufferForOutput(STNixAVAudioSource* obj, STNixApiBufferRef buff);
NixBOOL NixAVAudioSource_pendPopOldestBuffLocked_(STNixAVAudioSource* obj);
NixBOOL NixAVAudioSource_pendPopAllBuffsLocked_(STNixAVAudioSource* obj);

#define NIX_AVAudioSource_BIT_isStatic   (0x1 << 0)  //source expects only one buffer, repeats or pauses after playing it
#define NIX_AVAudioSource_BIT_isChanging (0x1 << 1)  //source is changing state after a call to request*()
#define NIX_AVAudioSource_BIT_isRepeat   (0x1 << 2)
#define NIX_AVAudioSource_BIT_isPlaying  (0x1 << 3)
#define NIX_AVAudioSource_BIT_isPaused   (0x1 << 4)
#define NIX_AVAudioSource_BIT_isClosing  (0x1 << 5)
#define NIX_AVAudioSource_BIT_isOrphan   (0x1 << 6)  //source is waiting for close(), wait for the change of state and NixAVAudioSource_release + NIX_FREE.
//
#define NixAVAudioSource_isStatic(OBJ)          (((OBJ)->stateBits & NIX_AVAudioSource_BIT_isStatic) != 0)
#define NixAVAudioSource_isChanging(OBJ)        (((OBJ)->stateBits & NIX_AVAudioSource_BIT_isChanging) != 0)
#define NixAVAudioSource_isRepeat(OBJ)          (((OBJ)->stateBits & NIX_AVAudioSource_BIT_isRepeat) != 0)
#define NixAVAudioSource_isPlaying(OBJ)         (((OBJ)->stateBits & NIX_AVAudioSource_BIT_isPlaying) != 0)
#define NixAVAudioSource_isPaused(OBJ)          (((OBJ)->stateBits & NIX_AVAudioSource_BIT_isPaused) != 0)
#define NixAVAudioSource_isClosing(OBJ)         (((OBJ)->stateBits & NIX_AVAudioSource_BIT_isClosing) != 0)
#define NixAVAudioSource_isOrphan(OBJ)          (((OBJ)->stateBits & NIX_AVAudioSource_BIT_isOrphan) != 0)
//
#define NixAVAudioSource_setIsStatic(OBJ, V)    (OBJ)->stateBits = (V ? (OBJ)->stateBits | NIX_AVAudioSource_BIT_isStatic : (OBJ)->stateBits & ~NIX_AVAudioSource_BIT_isStatic)
#define NixAVAudioSource_setIsChanging(OBJ, V)  (OBJ)->stateBits = (V ? (OBJ)->stateBits | NIX_AVAudioSource_BIT_isChanging : (OBJ)->stateBits & ~NIX_AVAudioSource_BIT_isChanging)
#define NixAVAudioSource_setIsRepeat(OBJ, V)    (OBJ)->stateBits = (V ? (OBJ)->stateBits | NIX_AVAudioSource_BIT_isRepeat : (OBJ)->stateBits & ~NIX_AVAudioSource_BIT_isRepeat)
#define NixAVAudioSource_setIsPlaying(OBJ, V)   (OBJ)->stateBits = (V ? (OBJ)->stateBits | NIX_AVAudioSource_BIT_isPlaying : (OBJ)->stateBits & ~NIX_AVAudioSource_BIT_isPlaying)
#define NixAVAudioSource_setIsPaused(OBJ, V)    (OBJ)->stateBits = (V ? (OBJ)->stateBits | NIX_AVAudioSource_BIT_isPaused : (OBJ)->stateBits & ~NIX_AVAudioSource_BIT_isPaused)
#define NixAVAudioSource_setIsClosing(OBJ)      (OBJ)->stateBits = ((OBJ)->stateBits | NIX_AVAudioSource_BIT_isClosing)
#define NixAVAudioSource_setIsOrphan(OBJ)       (OBJ)->stateBits = ((OBJ)->stateBits | NIX_AVAudioSource_BIT_isOrphan)

//------
//Recorder
//------

typedef struct STNixAVAudioRecorder_ {
    NixBOOL                 engStarted;
    STNixApiEngineRef       engRef;
    STNixApiRecorderRef     selfRef;
    AVAudioEngine*          eng;    //AVAudioEngine
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
        NIX_MUTEX_T         mutex;
        void*               conv;   //nixFmtConverter
        STNixAVAudioQueue  notify;
        STNixAVAudioQueue  reuse;
        //filling
        struct {
            NixSI32         iCurSample; //at first buffer in 'reuse'
        } filling;
    } queues;
} STNixAVAudioRecorder;

void NixAVAudioRecorder_init(STNixAVAudioRecorder* obj);
void NixAVAudioRecorder_destroy(STNixAVAudioRecorder* obj);
//
NixBOOL NixAVAudioRecorder_prepare(STNixAVAudioRecorder* obj, STNixAVAudioEngine* eng, const STNixAudioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer);
NixBOOL NixAVAudioRecorder_setCallback(STNixAVAudioRecorder* obj, NixApiRecorderCallback callback, void* callbackData);
NixBOOL NixAVAudioRecorder_start(STNixAVAudioRecorder* obj);
NixBOOL NixAVAudioRecorder_stop(STNixAVAudioRecorder* obj);
NixBOOL NixAVAudioRecorder_flush(STNixAVAudioRecorder* obj);
void NixAVAudioRecorder_notifyBuffers(STNixAVAudioRecorder* obj);

//------
//nixFmtConverter
//------

void nixFmtConverter_buffFmtToAudioDesc(AVAudioFormat* buffFmt, STNixAudioDesc* dst);

//------
//Engine
//------

void NixAVAudioEngine_init(STNixAVAudioEngine* obj, STNixContextItf* ctx){
    memset(obj, 0, sizeof(STNixAVAudioEngine));
    //
    obj->ctx = *ctx;
    nixAVAudioEngine_getApiItf(&obj->apiItf);
    //srcs
    {
        NIX_MUTEX_INIT(&obj->srcs.mutex);
    }
}

void NixAVAudioEngine_destroy(STNixAVAudioEngine* obj){
    //srcs
    {
        //cleanup
        while(obj->srcs.arr != NULL && obj->srcs.use > 0){
            NixAVAudioEngine_tick(obj, NIX_TRUE);
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

NixBOOL NixAVAudioEngine_srcsAdd(STNixAVAudioEngine* obj, struct STNixAVAudioSource_* src){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL){
        NIX_MUTEX_LOCK(&obj->srcs.mutex);
        {
            //resize array (if necesary)
            if(obj->srcs.use >= obj->srcs.sz){
                const NixUI32 szN = obj->srcs.use + 4;
                STNixAVAudioSource** arrN = NULL;
                NIX_MALLOC(arrN, STNixAVAudioSource*, sizeof(STNixAVAudioSource*) * szN, "STNixAVAudioEngine::srcsN");
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
                NIX_PRINTF_ERROR("nixAAudioSource_create::STNixAVAudioEngine::srcs failed (no allocated space).\n");
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

void NixAVAudioEngine_removeSrcRecordLocked_(STNixAVAudioEngine* obj, NixSI32* idx){
    STNixAVAudioSource* src = obj->srcs.arr[*idx];
    if(src != NULL){
        NixAVAudioSource_destroy(src);
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

void NixAVAudioEngine_tick_addQueueNotifSrcLocked_(STNixAVAudioNotifQueue* notifs, STNixAVAudioSource* srcLocked){
    if(srcLocked->queues.notify.use > 0){
        STNixAVAudioSrcNotif n;
        NixAVAudioSrcNotif_init(&n);
        n.callback = srcLocked->queues.callback;
        n.ammBuffs = srcLocked->queues.notify.use;
        if(!NixAVAudioQueue_flush(&srcLocked->queues.notify)){
            NIX_ASSERT(NIX_FALSE); //program logic error
        }
        if(!NixAVAudioNotifQueue_push(notifs, &n)){
            NIX_ASSERT(NIX_FALSE); //program logic error
            NixAVAudioSrcNotif_destroy(&n);
        }
    }
}
    
void NixAVAudioEngine_tick(STNixAVAudioEngine* obj, const NixBOOL isFinalCleanup){
    if(obj != NULL){
        //srcs
        {
            STNixAVAudioNotifQueue notifs;
            NixAVAudioNotifQueue_init(&notifs);
            NIX_MUTEX_LOCK(&obj->srcs.mutex);
            if(obj->srcs.arr != NULL && obj->srcs.use > 0){
                NixUI32 changingStateCount = 0;
                //NIX_PRINTF_INFO("NixAVAudioEngine_tick::%d sources.\n", obj->srcs.use);
                NixSI32 i; for(i = 0; i < (NixSI32)obj->srcs.use; ++i){
                    STNixAVAudioSource* src = obj->srcs.arr[i];
                    //NIX_PRINTF_INFO("NixAVAudioEngine_tick::source(#%d/%d).\n", i + 1, obj->srcs.use);
                    //release orphan
                    if(NixAVAudioSource_isOrphan(src) || isFinalCleanup){
                        if(src->src != nil){
                            if([src->src isPlaying]){
                                [src->src stop];
                            } else {
                                [src->src release];
                                src->src = nil;
                            }
                        }
                    }
                    //process
                    if(src->src == nil){
                        //remove
                        //NIX_PRINTF_INFO("NixAVAudioEngine_tick::source(#%d/%d); remove-NULL.\n", i + 1, obj->srcs.use);
                        NixAVAudioEngine_removeSrcRecordLocked_(obj, &i);
                        src = NULL;
                    } else {
                        //post-process
                        if(src != NULL){
                            //add to notify queue
                            {
                                NIX_MUTEX_LOCK(&src->queues.mutex);
                                {
                                    NixAVAudioEngine_tick_addQueueNotifSrcLocked_(&notifs, src);
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
                //NIX_PRINTF_INFO("NixAVAudioEngine_tick::notify %d.\n", notifs.use);
                NixUI32 i;
                for(i = 0; i < notifs.use; ++i){
                    STNixAVAudioSrcNotif* n = &notifs.arr[i];
                    //NIX_PRINTF_INFO("NixAVAudioEngine_tick::notify(#%d/%d).\n", i + 1, notifs.use);
                    if(n->callback.func != NULL){
                        (*n->callback.func)(n->callback.eng, n->callback.sourceIndex, n->ammBuffs);
                    }
                }
            }
            //NIX_PRINTF_INFO("NixAVAudioEngine_tick::NixAVAudioNotifQueue_destroy.\n");
            NixAVAudioNotifQueue_destroy(&notifs);
        }
        //recorder
        if(obj->rec != NULL){
            NixAVAudioRecorder_notifyBuffers(obj->rec);
        }
    }
}

//------
//Notif
//------

void NixAVAudioSrcNotif_init(STNixAVAudioSrcNotif* obj){
    memset(obj, 0, sizeof(*obj));
}

void NixAVAudioSrcNotif_destroy(STNixAVAudioSrcNotif* obj){
    //
}

//------
//NotifQueue
//------

void NixAVAudioNotifQueue_init(STNixAVAudioNotifQueue* obj){
    memset(obj, 0, sizeof(*obj));
    obj->arr = obj->arrEmbedded;
    obj->sz = (sizeof(obj->arrEmbedded) / sizeof(obj->arrEmbedded[0]));
}

void NixAVAudioNotifQueue_destroy(STNixAVAudioNotifQueue* obj){
    if(obj->arr != NULL){
        NixUI32 i; for(i = 0; i < obj->use; i++){
            STNixAVAudioSrcNotif* b = &obj->arr[i];
            NixAVAudioSrcNotif_destroy(b);
        }
        if(obj->arr != obj->arrEmbedded){
            NIX_FREE(obj->arr);
        }
        obj->arr = NULL;
    }
    obj->use = obj->sz = 0;
}

NixBOOL NixAVAudioNotifQueue_push(STNixAVAudioNotifQueue* obj, STNixAVAudioSrcNotif* pair){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL && pair != NULL){
        //resize array (if necesary)
        if(obj->use >= obj->sz){
            const NixUI32 szN = obj->use + 4;
            STNixAVAudioSrcNotif* arrN = NULL;
            NIX_MALLOC(arrN, STNixAVAudioSrcNotif, sizeof(STNixAVAudioSrcNotif) * szN, "NixAVAudioNotifQueue_push::arrN");
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
            NIX_PRINTF_ERROR("NixAVAudioNotifQueue_push failed (no allocated space).\n");
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

void NixAVAudioQueuePair_init(STNixAVAudioQueuePair* obj){
    memset(obj, 0, sizeof(*obj));
}

void NixAVAudioQueuePair_destroy(STNixAVAudioQueuePair* obj){
    if(obj->org.ptr != NULL){
        NixApiBufferRef_release(&obj->org);
        obj->org.ptr = NULL;
    }
    if(obj->cnv != nil){
        [obj->cnv release];
        obj->cnv = nil;
    }
}

//------
//Queue (Buffers)
//------

void NixAVAudioQueue_init(STNixAVAudioQueue* obj){
    memset(obj, 0, sizeof(*obj));
}

void NixAVAudioQueue_destroy(STNixAVAudioQueue* obj){
    if(obj->arr != NULL){
        NixUI32 i; for(i = 0; i < obj->use; i++){
            STNixAVAudioQueuePair* b = &obj->arr[i];
            NixAVAudioQueuePair_destroy(b);
        }
        NIX_FREE(obj->arr);
        obj->arr = NULL;
    }
    obj->use = obj->sz = 0;
}

NixBOOL NixAVAudioQueue_flush(STNixAVAudioQueue* obj){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL){
        if(obj->arr != NULL){
            NixUI32 i; for(i = 0; i < obj->use; i++){
                STNixAVAudioQueuePair* b = &obj->arr[i];
                NixAVAudioQueuePair_destroy(b);
            }
            obj->use = 0;
        }
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL NixAVAudioQueue_prepareForSz(STNixAVAudioQueue* obj, const NixUI32 minSz){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL){
        //resize array (if necesary)
        if(minSz > obj->sz){
            const NixUI32 szN = minSz;
            STNixAVAudioQueuePair* arrN = NULL;
            NIX_MALLOC(arrN, STNixAVAudioQueuePair, sizeof(STNixAVAudioQueuePair) * szN, "NixAVAudioQueue_prepareForSz::arrN");
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
            NIX_PRINTF_ERROR("NixAVAudioQueue_prepareForSz failed (no allocated space).\n");
        } else {
            r = NIX_TRUE;
        }
    }
    return r;
}

NixBOOL NixAVAudioQueue_pushOwning(STNixAVAudioQueue* obj, STNixAVAudioQueuePair* pair){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL && pair != NULL){
        //resize array (if necesary)
        if(obj->use >= obj->sz){
            const NixUI32 szN = obj->use + 4;
            STNixAVAudioQueuePair* arrN = NULL;
            NIX_MALLOC(arrN, STNixAVAudioQueuePair, sizeof(STNixAVAudioQueuePair) * szN, "NixAVAudioQueue_pushOwning::arrN");
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
            NIX_PRINTF_ERROR("NixAVAudioQueue_pushOwning failed (no allocated space).\n");
        } else {
            //become the owner of the pair
            obj->arr[obj->use++] = *pair;
            r = NIX_TRUE;
        }
    }
    return r;
}

NixBOOL NixAVAudioQueue_popOrphaning(STNixAVAudioQueue* obj, STNixAVAudioQueuePair* dst){
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

NixBOOL NixAVAudioQueue_popMovingTo(STNixAVAudioQueue* obj, STNixAVAudioQueue* other){
    NixBOOL r = NIX_FALSE;
    STNixAVAudioQueuePair pair;
    if(!NixAVAudioQueue_popOrphaning(obj, &pair)){
        //
    } else {
        if(!NixAVAudioQueue_pushOwning(other, &pair)){
            //program logic error
            NIX_ASSERT(NIX_FALSE);
            NixAVAudioQueuePair_destroy(&pair);
        } else {
            r = NIX_TRUE;
        }
    }
    return r;
}

//------
//Source
//------

void NixAVAudioSource_init(STNixAVAudioSource* obj){
    memset(obj, 0, sizeof(STNixAVAudioSource));
    //queues
    {
        NIX_MUTEX_INIT(&obj->queues.mutex);
        NixAVAudioQueue_init(&obj->queues.notify);
        NixAVAudioQueue_init(&obj->queues.pend);
        NixAVAudioQueue_init(&obj->queues.reuse);
    }
}
   
void NixAVAudioSource_destroy(STNixAVAudioSource* obj){
    //src
    if(obj->src != nil){ [obj->src release]; obj->src = nil; }
    if(obj->eng != nil){ [obj->eng release]; obj->eng = nil; }
    //queues
    {
        if(obj->queues.conv != NULL){
            nixFmtConverter_free(obj->queues.conv);
            obj->queues.conv = NULL;
        }
        NixAVAudioQueue_destroy(&obj->queues.pend);
        NixAVAudioQueue_destroy(&obj->queues.reuse);
        NixAVAudioQueue_destroy(&obj->queues.notify);
        NIX_MUTEX_DESTROY(&obj->queues.mutex);
    }
}

void NixAVAudioSource_queueBufferScheduleCallback_(STNixAVAudioSource* obj, AVAudioPCMBuffer* cnvBuff){
    NIX_MUTEX_LOCK(&obj->queues.mutex);
    {
        NIX_ASSERT(obj->queues.pendScheduledCount > 0)
        if(obj->queues.pendScheduledCount > 0){
            //Note: AVFAudio calls callbacks not in the same order of scheduling.
/*#         ifdef NIX_ASSERTS_ACTIVATED
            if(obj->queues.pend.arr[0].cnv != cnvBuff){
                NIX_PRINTF_WARNING("Unqueued buffer does not match oldest buffer.\n");
                NixUI32 i; for(i = 0; i < obj->queues.pend.use; i++){
                    const STNixAVAudioQueuePair* pair = &obj->queues.pend.arr[i];
                    NIX_PRINTF_WARNING("Buffer #%d/%d: %lld vs %lld%s.\n", (i + 1), obj->queues.pend.use, (long long)pair->cnv, (long long)cnvBuff, pair->cnv == cnvBuff ? " MATCH": "");
                }
            }
            NIX_ASSERT(obj->queues.pend.arr[0].cnv == cnvBuff)
#           endif*/
            --obj->queues.pendScheduledCount;
        }
        if(NixAVAudioSource_isStatic(obj)){
            //schedule again
            if(NixAVAudioSource_isPlaying(obj) && !NixAVAudioSource_isPaused(obj) && NixAVAudioSource_isRepeat(obj) && obj->queues.pend.use == 1 && obj->queues.pendScheduledCount == 0){
                ++obj->queues.pendScheduledCount;
                NIX_MUTEX_UNLOCK(&obj->queues.mutex);
                {
                    [obj->src scheduleBuffer:cnvBuff completionCallbackType:AVAudioPlayerNodeCompletionDataRendered completionHandler:^(AVAudioPlayerNodeCompletionCallbackType cbType) {
                        //printf("completionCallbackType: %s.\n", cbType == AVAudioPlayerNodeCompletionDataConsumed ? "DataConsumed" : cbType == AVAudioPlayerNodeCompletionDataRendered? "DataRendered" : cbType == AVAudioPlayerNodeCompletionDataPlayedBack ? "DataPlayedBack" : "unknown");
                        if(cbType == AVAudioPlayerNodeCompletionDataRendered){
                            NixAVAudioSource_queueBufferScheduleCallback_(obj, cnvBuff);
                        }
                    }];
                }
                NIX_MUTEX_LOCK(&obj->queues.mutex);
            } else {
                //pause
                NixAVAudioSource_setIsPaused(obj, NIX_TRUE);
            }
        } else {
            if(!NixAVAudioSource_pendPopOldestBuffLocked_(obj)){
                //remove from queue, to pend
            } else {
                //buff moved from pend to reuse and notif
            }
        }
    }
    NIX_MUTEX_UNLOCK(&obj->queues.mutex);
}

void NixAVAudioSource_scheduleEnqueuedBuffers(STNixAVAudioSource* obj){
    NIX_MUTEX_LOCK(&obj->queues.mutex);
    while(obj->queues.pendScheduledCount < obj->queues.pend.use){
        STNixAVAudioQueuePair* pair = &obj->queues.pend.arr[obj->queues.pendScheduledCount];
        NIX_ASSERT(pair->cnv != nil)
        if(pair->cnv == nil){
            //program logic error
            break;
        } else {
            AVAudioPCMBuffer* cnv = pair->cnv;
            ++obj->queues.pendScheduledCount;
            NIX_MUTEX_UNLOCK(&obj->queues.mutex);
            {
                [obj->src scheduleBuffer:cnv completionCallbackType:AVAudioPlayerNodeCompletionDataRendered completionHandler:^(AVAudioPlayerNodeCompletionCallbackType cbType) {
                    //printf("completionCallbackType: %s.\n", cbType == AVAudioPlayerNodeCompletionDataConsumed ? "DataConsumed" : cbType == AVAudioPlayerNodeCompletionDataRendered? "DataRendered" : cbType == AVAudioPlayerNodeCompletionDataPlayedBack ? "DataPlayedBack" : "unknown");
                    if(cbType == AVAudioPlayerNodeCompletionDataRendered){
                        NixAVAudioSource_queueBufferScheduleCallback_(obj, cnv);
                    }
                }];
            }
            NIX_MUTEX_LOCK(&obj->queues.mutex);
        }
    }
    NIX_MUTEX_UNLOCK(&obj->queues.mutex);
}

NixBOOL NixAVAudioSource_queueBufferForOutput(STNixAVAudioSource* obj, STNixApiBufferRef pBuff){
    NixBOOL r = NIX_FALSE;
    STNixPCMBuffer* buff = (STNixPCMBuffer*)NixSharedPtr_getOpq(pBuff.ptr);
    if(!STNixAudioDesc_IsEqual(&obj->buffsFmt, &buff->desc)){
        //error
    } else if(buff->desc.blockAlign <= 0){
        //error
    } else {
        STNixAVAudioQueuePair pair;
        NixAVAudioQueuePair_init(&pair);
        //prepare conversion buffer (if necesary)
        if(obj->eng != NULL && obj->queues.conv != NULL){
            AVAudioFormat* outFmt = [[obj->eng outputNode] outputFormatForBus:0];
            //create copy buffer
            const NixUI32 buffSamplesMax    = (buff->sz / buff->desc.blockAlign);
            const NixUI32 samplesReq        = nixFmtConverter_samplesForNewFrequency(buffSamplesMax, obj->buffsFmt.samplerate, [outFmt sampleRate]);
            //try to reuse buffer
            {
                STNixAVAudioQueuePair reuse;
                if(NixAVAudioQueue_popOrphaning(&obj->queues.reuse, &reuse)){
                    //reusable buffer
                    NIX_ASSERT(reuse.org.ptr == NULL) //program logic error
                    NIX_ASSERT(reuse.cnv != NULL) //program logic error
                    if([reuse.cnv frameCapacity] < samplesReq){
                        [reuse.cnv release];
                        reuse.cnv = nil;
                    } else {
                        pair.cnv = reuse.cnv;
                        reuse.cnv = nil;
                    }
                    NixAVAudioQueuePair_destroy(&reuse);
                }
            }
            //create new buffer
            if(pair.cnv == nil){
                pair.cnv = [[AVAudioPCMBuffer alloc] initWithPCMFormat:outFmt frameCapacity:samplesReq];
                if(pair.cnv == NULL){
                    NIX_PRINTF_ERROR("NixAVAudioSource_queueBufferForOutput::pair.cnv could be allocated.\n");
                }
            }
            //convert
            if(pair.cnv == NULL){
                r = NIX_FALSE;
            } else if(!nixFmtConverter_setPtrAtSrcInterlaced(obj->queues.conv, &buff->desc, buff->ptr, 0)){
                NIX_PRINTF_ERROR("NixAVAudioSource_queueBufferForOutput::nixFmtConverter_setPtrAtSrcInterlaced failed.\n");
                r = NIX_FALSE;
            } else {
                r = NIX_TRUE;
                STNixAudioDesc srcFmt;
                srcFmt.channels = [outFmt channelCount];
                srcFmt.samplerate = [outFmt sampleRate];
                NixBOOL isInterleavedDst = [outFmt isInterleaved];
                NixUI32 iCh, maxChannels = nixFmtConverter_maxChannels();
                switch([outFmt commonFormat]) {
                    case AVAudioPCMFormatFloat32:
                        srcFmt.samplesFormat = ENNixSampleFmt_Float;
                        srcFmt.bitsPerSample = 32;
                        srcFmt.blockAlign = (srcFmt.bitsPerSample / 8) * (isInterleavedDst ? srcFmt.channels : 1);
                        for(iCh = 0; iCh < srcFmt.channels && iCh < maxChannels; ++iCh){
                            if(!nixFmtConverter_setPtrAtDst(obj->queues.conv, iCh, pair.cnv.floatChannelData[iCh], srcFmt.blockAlign)){
                                NIX_PRINTF_ERROR("NixAVAudioSource_queueBufferForOutput::nixFmtConverter_setPtrAtDst failed.\n");
                                r = NIX_FALSE;
                            }
                        }
                        break;
                    case AVAudioPCMFormatInt16:
                        srcFmt.samplesFormat = ENNixSampleFmt_Int;
                        srcFmt.bitsPerSample = 16;
                        srcFmt.blockAlign = (srcFmt.bitsPerSample / 8) * (isInterleavedDst ? srcFmt.channels : 1);
                        for(iCh = 0; iCh < srcFmt.channels && iCh < maxChannels; ++iCh){
                            if(!nixFmtConverter_setPtrAtDst(obj->queues.conv, iCh, pair.cnv.int16ChannelData[iCh], srcFmt.blockAlign)){
                                NIX_PRINTF_ERROR("NixAVAudioSource_queueBufferForOutput::nixFmtConverter_setPtrAtDst failed.\n");
                                r = NIX_FALSE;
                            }
                        }
                        break;
                    case AVAudioPCMFormatInt32:
                        srcFmt.samplesFormat = ENNixSampleFmt_Int;
                        srcFmt.bitsPerSample = 32;
                        srcFmt.blockAlign = (srcFmt.bitsPerSample / 8) * (isInterleavedDst ? srcFmt.channels : 1);
                        for(iCh = 0; iCh < srcFmt.channels && iCh < maxChannels; ++iCh){
                            if(!nixFmtConverter_setPtrAtDst(obj->queues.conv, iCh, pair.cnv.int32ChannelData[iCh], srcFmt.blockAlign)){
                                NIX_PRINTF_ERROR("NixAVAudioSource_queueBufferForOutput::nixFmtConverter_setPtrAtDst failed.\n");
                                r = NIX_FALSE;
                            }
                        }
                        break;
                    default:
                        NIX_PRINTF_ERROR("NixAVAudioSource_queueBufferForOutput::unexpected 'commonFormat'.\n");
                        r = NIX_FALSE;
                        break;
                }
                //conver
                if(r){
                    const NixUI32 srcBlocks = (buff->use / buff->desc.blockAlign);
                    const NixUI32 dstBlocks = [pair.cnv frameCapacity];
                    NixUI32 ammBlocksRead = 0;
                    NixUI32 ammBlocksWritten = 0;
                    if(!nixFmtConverter_convert(obj->queues.conv, srcBlocks, dstBlocks, &ammBlocksRead, &ammBlocksWritten)){
                        NIX_PRINTF_ERROR("NixAVAudioSource_queueBufferForOutput::nixFmtConverter_convert failed from(%uhz, %uch, %dbit-%s) to(%uhz, %uch, %dbit-%s).\n"
                                         , obj->buffsFmt.samplerate
                                         , obj->buffsFmt.channels
                                         , obj->buffsFmt.bitsPerSample
                                         , obj->buffsFmt.samplesFormat == ENNixSampleFmt_Int ? "int" : obj->buffsFmt.samplesFormat == ENNixSampleFmt_Float ? "float" : "unknown"
                                         , srcFmt.samplerate
                                         , srcFmt.channels
                                         , srcFmt.bitsPerSample
                                         , srcFmt.samplesFormat == ENNixSampleFmt_Int ? "int" : srcFmt.samplesFormat == ENNixSampleFmt_Float ? "float" : "unknown"
                                         );
                        r = NIX_FALSE;
                    } else {
                        /*
#                       ifdef NIX_ASSERTS_ACTIVATED
                        if((ammBlocksRead * 100 / srcBlocks) < 90 || (ammBlocksWritten * 100 / dstBlocks) < 90){
                            NIX_PRINTF_ERROR("NixAVAudioSource_queueBufferForOutput::nixFmtConverter_convert under-consumption from(%uhz, %uch, %dbit-%s) to(%uhz, %uch, %dbit-%s) = %d%% of %u src-frames consumed, %d%% of %u dst-frames populated.\n"
                                             , obj->buffsFmt.samplerate
                                             , obj->buffsFmt.channels
                                             , obj->buffsFmt.bitsPerSample
                                             , obj->buffsFmt.samplesFormat == ENNixSampleFmt_Int ? "int" : obj->buffsFmt.samplesFormat == ENNixSampleFmt_Float ? "float" : "unknown"
                                             , srcFmt.samplerate
                                             , srcFmt.channels
                                             , srcFmt.bitsPerSample
                                             , srcFmt.samplesFormat == ENNixSampleFmt_Int ? "int" : srcFmt.samplesFormat == ENNixSampleFmt_Float ? "float" : "unknown"
                                             //
                                             , ammBlocksRead * 100 / srcBlocks, srcBlocks, ammBlocksWritten * 100 / dstBlocks, dstBlocks
                                             );
                            NIX_ASSERT(FALSE) //program logic error
                        }
#                       endif
                        */
                        [pair.cnv setFrameLength:ammBlocksWritten];
                    }
                }
            }
        }
        //add to queue
        if(r){
            pair.org = pBuff;
            NixApiBufferRef_retain(&pBuff);
            NIX_MUTEX_LOCK(&obj->queues.mutex);
            {
                if(!NixAVAudioQueue_pushOwning(&obj->queues.pend, &pair)){
                    NIX_PRINTF_ERROR("NixAVAudioSource_queueBufferForOutput::NixAVAudioQueue_pushOwning failed.\n");
                    r = NIX_FALSE;
                } else {
                    //added to queue
                    NixAVAudioQueue_prepareForSz(&obj->queues.reuse, obj->queues.pend.use); //this ensures malloc wont be calle inside a callback
                    NixAVAudioQueue_prepareForSz(&obj->queues.notify, obj->queues.pend.use); //this ensures malloc wont be calle inside a callback
                    //this is the first buffer i the queue
                    if(obj->queues.pend.use == 1){
                        obj->queues.pendSampleIdx = 0;
                    }
                }
            }
            NIX_MUTEX_UNLOCK(&obj->queues.mutex);
        }
        if(!r){
            NixAVAudioQueuePair_destroy(&pair);
        }
    }
    return r;
}

NixBOOL NixAVAudioSource_pendPopOldestBuffLocked_(STNixAVAudioSource* obj){
    NixBOOL r = NIX_FALSE;
    STNixAVAudioQueuePair pair;
    if(!NixAVAudioQueue_popOrphaning(&obj->queues.pend, &pair)){
        NIX_ASSERT(NIX_FALSE); //program logic error
    } else {
        //move "cnv" to reusable queue
        if(pair.cnv != NULL){
            STNixAVAudioQueuePair reuse;
            NixAVAudioQueuePair_init(&reuse);
            reuse.cnv = pair.cnv;
            if(!NixAVAudioQueue_pushOwning(&obj->queues.reuse, &reuse)){
                NIX_PRINTF_ERROR("NixAVAudioSource_pendPopOldestBuffLocked_::NixAVAudioQueue_pushOwning(reuse) failed.\n");
                reuse.cnv = NULL;
                NixAVAudioQueuePair_destroy(&reuse);
            } else {
                //now owned by reuse
                pair.cnv = NULL;
            }
        }
        //move "org" to notify queue
        if(pair.org.ptr != NULL){
            STNixAVAudioQueuePair notif;
            NixAVAudioQueuePair_init(&notif);
            notif.org = pair.org;
            if(!NixAVAudioQueue_pushOwning(&obj->queues.notify, &notif)){
                NIX_PRINTF_ERROR("NixAVAudioSource_pendPopOldestBuffLocked_::NixAVAudioQueue_pushOwning(notify) failed.\n");
                NixAVAudioQueuePair_destroy(&notif);
            } else {
                //now owned by reuse
                pair.org.ptr = NULL;
            }
        }
        NIX_ASSERT(pair.org.ptr == NULL); //program logic error
        NIX_ASSERT(pair.cnv == NULL); //program logic error
        NixAVAudioQueuePair_destroy(&pair);
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL NixAVAudioSource_pendPopAllBuffsLocked_(STNixAVAudioSource* obj){
    NixBOOL r = NIX_TRUE;
    while(obj->queues.pend.use > 0){
        if(!NixAVAudioSource_pendPopOldestBuffLocked_(obj)){
            r = NIX_FALSE;
            break;
        }
    }
    return r;
}

//------
//Recorder
//------

void NixAVAudioRecorder_init(STNixAVAudioRecorder* obj){
    memset(obj, 0, sizeof(*obj));
    //cfg
    {
        //
    }
    //queues
    {
        NIX_MUTEX_INIT(&obj->queues.mutex);
        NixAVAudioQueue_init(&obj->queues.notify);
        NixAVAudioQueue_init(&obj->queues.reuse);
    }
}

void NixAVAudioRecorder_destroy(STNixAVAudioRecorder* obj){
    //queues
    {
        NIX_MUTEX_LOCK(&obj->queues.mutex);
        {
            NixAVAudioQueue_destroy(&obj->queues.notify);
            NixAVAudioQueue_destroy(&obj->queues.reuse);
            if(obj->queues.conv != NULL){
                nixFmtConverter_free(obj->queues.conv);
                obj->queues.conv = NULL;
            }
        }
        NIX_MUTEX_UNLOCK(&obj->queues.mutex);
        NIX_MUTEX_DESTROY(&obj->queues.mutex);
    }
    //
    if(obj->eng != nil){
        AVAudioInputNode* input = [obj->eng inputNode];
        [input removeTapOnBus:0];
        [obj->eng stop];
        [obj->eng release];
        obj->eng = nil;
    }
    //
    if(obj->engRef.ptr != NULL){
        STNixAVAudioEngine* eng = (STNixAVAudioEngine*)NixSharedPtr_getOpq(obj->engRef.ptr);
        if(eng != NULL && eng->rec == obj){
            eng->rec = NULL;
        }
        NixApiEngineRef_release(&obj->engRef);
        obj->engRef.ptr = NULL;
    }
}

void NixAVAudioRecorder_consumeInputBuffer_(STNixAVAudioRecorder* obj, AVAudioPCMBuffer* buff){
    if(obj->queues.conv != NULL){
        NIX_MUTEX_LOCK(&obj->queues.mutex);
        {
            NixUI32 inIdx = 0;
            const NixUI32 inSz = [buff frameLength];
            while(inIdx < inSz){
                if(obj->queues.reuse.use <= 0){
                    //move oldest-notify buffer to reuse
                    if(!NixAVAudioQueue_popMovingTo(&obj->queues.notify, &obj->queues.reuse)){
                        //program logic error
                        NIX_ASSERT(NIX_FALSE);
                        break;
                    }
                } else {
                    STNixAVAudioQueuePair* pair = &obj->queues.reuse.arr[0];
                    STNixPCMBuffer* org = (STNixPCMBuffer*)NixSharedPtr_getOpq(pair->org.ptr);
                    if(org == NULL || org->desc.blockAlign <= 0){
                        //just remove
                        STNixAVAudioQueuePair pair;
                        if(!NixAVAudioQueue_popOrphaning(&obj->queues.reuse, &pair)){
                            NIX_ASSERT(NIX_FALSE);
                            //program logic error
                            break;
                        }
                        NixAVAudioQueuePair_destroy(&pair);
                    } else {
                        const NixUI32 outSz = (org->sz / org->desc.blockAlign);
                        const NixUI32 outAvail = (obj->queues.filling.iCurSample >= outSz ? 0 : outSz - obj->queues.filling.iCurSample);
                        const NixUI32 inAvail = inSz - inIdx;
                        NixUI32 ammBlocksRead = 0, ammBlocksWritten = 0;
                        if(outAvail > 0 && inAvail > 0){
                            //convert
                            NixUI32 iCh;
                            const NixUI32 maxChannels = nixFmtConverter_maxChannels();
                            //dst
                            nixFmtConverter_setPtrAtDstInterlaced(obj->queues.conv, &org->desc, org->ptr, obj->queues.filling.iCurSample);
                            //src
                            AVAudioFormat* orgFmt = [ buff format];
                            NixBOOL isInterleavedSrc = [orgFmt isInterleaved];
                            NixUI32 chCountSrc = [orgFmt channelCount];
                            switch([orgFmt commonFormat]) {
                                case AVAudioPCMFormatFloat32:
                                    for(iCh = 0; iCh < chCountSrc && iCh < maxChannels; ++iCh){
                                        const NixUI32 bitsPerSample = 32;
                                        const NixUI32 blockAlign = (bitsPerSample / 8) * (isInterleavedSrc ? chCountSrc : 1);
                                        nixFmtConverter_setPtrAtSrc(obj->queues.conv, iCh,  buff.floatChannelData[iCh] + inIdx, blockAlign);
                                    }
                                    break;
                                case AVAudioPCMFormatInt16:
                                    for(iCh = 0; iCh < chCountSrc && iCh < maxChannels; ++iCh){
                                        const NixUI32 bitsPerSample = 16;
                                        const NixUI32 blockAlign = (bitsPerSample / 8) * (isInterleavedSrc ? chCountSrc : 1);
                                        nixFmtConverter_setPtrAtSrc(obj->queues.conv, iCh,  buff.int16ChannelData[iCh] + inIdx, blockAlign);
                                    }
                                    break;
                                case AVAudioPCMFormatInt32:
                                    for(iCh = 0; iCh < chCountSrc && iCh < maxChannels; ++iCh){
                                        const NixUI32 bitsPerSample = 32;
                                        const NixUI32 blockAlign = (bitsPerSample / 8) * (isInterleavedSrc ? chCountSrc : 1);
                                        nixFmtConverter_setPtrAtSrc(obj->queues.conv, iCh,  buff.int32ChannelData[iCh] + inIdx, blockAlign);
                                    }
                                    break;
                                default:
                                    break;
                            }
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
                            if(!NixAVAudioQueue_popMovingTo(&obj->queues.reuse, &obj->queues.notify)){
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


NixBOOL NixAVAudioRecorder_prepare(STNixAVAudioRecorder* obj, STNixAVAudioEngine* eng, const STNixAudioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer){
    NixBOOL r = NIX_FALSE;
    NIX_MUTEX_LOCK(&obj->queues.mutex);
    if(obj->queues.conv == NULL && audioDesc->blockAlign > 0){
        obj->eng = [[AVAudioEngine alloc] init];
        {
            void* conv = nixFmtConverter_alloc();
            AVAudioInputNode* input = [obj->eng inputNode];
            AVAudioFormat* inFmt = [input inputFormatForBus:0];
            const NixUI32 inSampleRate = [inFmt sampleRate];
            STNixAudioDesc inDesc;
            nixFmtConverter_buffFmtToAudioDesc(inFmt, &inDesc);
            if(!nixFmtConverter_prepare(conv, &inDesc, audioDesc)){
                NIX_PRINTF_ERROR("NixAVAudioRecorder_prepare::nixFmtConverter_prepare failed.\n");
                nixFmtConverter_free(conv);
                conv = NULL;
            } else {
                //allocate reusable buffers
                while(obj->queues.reuse.use < buffersCount){
                    STNixAVAudioQueuePair pair;
                    NixAVAudioQueuePair_init(&pair);
                    pair.org = (*eng->apiItf.buffer.alloc)(&eng->ctx, audioDesc, NULL, audioDesc->blockAlign * samplesPerBuffer);
                    if(pair.org.ptr == NULL){
                        NIX_PRINTF_ERROR("NixAVAudioRecorder_prepare::pair.org allocation failed.\n");
                        break;
                    } else {
                        NixAVAudioQueue_pushOwning(&obj->queues.reuse, &pair);
                    }
                }
                //
                if(obj->queues.reuse.use <= 0){
                    NIX_PRINTF_ERROR("NixAVAudioRecorder_prepare::no reusable buffer could be allocated.\n");
                } else {
                    r = NIX_TRUE;
                    //prepared
                    obj->queues.filling.iCurSample = 0;
                    obj->queues.conv = conv; conv = NULL; //consume
                    //cfg
                    obj->cfg.fmt = *audioDesc;
                    obj->cfg.maxBuffers = buffersCount;
                    obj->cfg.samplesPerBuffer = samplesPerBuffer;
                    //
                    NIX_MUTEX_UNLOCK(&obj->queues.mutex);
                    //install tap (unlocked)
                    {
                        [input installTapOnBus:0 bufferSize:(inSampleRate / 30) format:inFmt block:^(AVAudioPCMBuffer * _Nonnull buffer, AVAudioTime * _Nonnull when) {
                            NixAVAudioRecorder_consumeInputBuffer_(obj, buffer);
                            //printf("AVFAudio recorder buffer with %d samples (%d samples in memory).\n", [buffer frameLength], obj->in.samples.cur);
                        }];
                    }
                    NIX_MUTEX_LOCK(&obj->queues.mutex);
                    [obj->eng prepare];
                }
            }
            //release (if not consumed)
            if(conv != NULL){
                nixFmtConverter_free(conv);
                conv = NULL;
            }
        }
    }
    NIX_MUTEX_UNLOCK(&obj->queues.mutex);
    return r;
}

NixBOOL NixAVAudioRecorder_setCallback(STNixAVAudioRecorder* obj, NixApiRecorderCallback callback, void* callbackData){
    NixBOOL r = NIX_FALSE;
    {
        obj->callback.func = callback;
        obj->callback.data = callbackData;
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL NixAVAudioRecorder_start(STNixAVAudioRecorder* obj){
    NixBOOL r = NIX_TRUE;
    if(!obj->engStarted){
        NSError* err = nil;
        if(![obj->eng startAndReturnError:&err]){
            NIX_PRINTF_ERROR("nixAVAudioRecorder_create, AVAudioEngine::startAndReturnError failed: '%s'.\n", err == nil ? "unknown" : [[err description] UTF8String]);
            r = NIX_FALSE;
        } else {
            obj->engStarted = NIX_TRUE;
        }
    }
    return r;
}

NixBOOL NixAVAudioRecorder_stop(STNixAVAudioRecorder* obj){
    NixBOOL r = NIX_TRUE;
    if(obj->eng != nil){
        [obj->eng stop];
        obj->engStarted = NIX_FALSE;
    }
    NixAVAudioRecorder_flush(obj);
    return r;
}

NixBOOL NixAVAudioRecorder_flush(STNixAVAudioRecorder* obj){
    NixBOOL r = NIX_TRUE;
    //move filling buffer to notify (if data is available)
    NIX_MUTEX_LOCK(&obj->queues.mutex);
    if(obj->queues.reuse.use > 0){
        STNixAVAudioQueuePair* pair = &obj->queues.reuse.arr[0];
        if(pair->org.ptr != NULL && ((STNixPCMBuffer*)NixSharedPtr_getOpq(pair->org.ptr))->use > 0){
            obj->queues.filling.iCurSample = 0;
            if(!NixAVAudioQueue_popMovingTo(&obj->queues.reuse, &obj->queues.notify)){
                //program logic error
                r = NIX_FALSE;
            }
        }
    }
    NIX_MUTEX_UNLOCK(&obj->queues.mutex);
    return r;
}

void NixAVAudioRecorder_notifyBuffers(STNixAVAudioRecorder* obj){
    NIX_MUTEX_LOCK(&obj->queues.mutex);
    {
        const NixUI32 maxProcess = obj->queues.notify.use;
        NixUI32 ammProcessed = 0;
        while(ammProcessed < maxProcess && obj->queues.notify.use > 0){
            STNixAVAudioQueuePair pair;
            if(!NixAVAudioQueue_popOrphaning(&obj->queues.notify, &pair)){
                NIX_ASSERT(NIX_FALSE);
                //program logic error
                break;
            } else {
                //notify (unlocked)
                if(pair.org.ptr != NULL && ((STNixPCMBuffer*)NixSharedPtr_getOpq(pair.org.ptr))->desc.blockAlign > 0 && obj->callback.func != NULL){
                    STNixPCMBuffer* org = (STNixPCMBuffer*)NixSharedPtr_getOpq(pair.org.ptr);
                    NIX_MUTEX_UNLOCK(&obj->queues.mutex);
                    {
                        (*obj->callback.func)(obj->engRef, obj->selfRef, org->desc, org->ptr, org->use, (org->use / org->desc.blockAlign), obj->callback.data);
                    }
                    NIX_MUTEX_LOCK(&obj->queues.mutex);
                }
                //move to reuse
                if(!NixAVAudioQueue_pushOwning(&obj->queues.reuse, &pair)){
                    //program logic error
                    NIX_ASSERT(NIX_FALSE);
                    NixAVAudioQueuePair_destroy(&pair);
                }
            }
            //processed
            ++ammProcessed;
        }
    }
    NIX_MUTEX_UNLOCK(&obj->queues.mutex);
}

//------
//Engine API
//------

STNixApiEngineRef nixAVAudioEngine_alloc(STNixContextItf* ctx){
    STNixApiEngineRef r = STNixApiEngineRef_Zero;
    STNixAVAudioEngine* obj = NULL;
    NIX_MALLOC(obj, STNixAVAudioEngine, sizeof(STNixAVAudioEngine), "STNixAVAudioEngine");
    if(obj != NULL){
        NixAVAudioEngine_init(obj, ctx);
        if(NULL == (r.ptr = NixSharedPtr_create(obj))){
            NIX_PRINTF_ERROR("nixAVAudioEngine_create::NixSharedPtr_create failed.\n");
        } else {
            r.itf = &obj->apiItf.engine;
            obj = NULL; //consume
        }
    }
    //release (if not consumed)
    if(obj != NULL){
        NixAVAudioEngine_destroy(obj);
        NIX_FREE(obj);
        obj = NULL;
    }
    return r;
}

void nixAVAudioEngine_free(STNixApiEngineRef pObj){
    if(pObj.ptr != NULL){
        STNixAVAudioEngine* obj = (STNixAVAudioEngine*)NixSharedPtr_getOpq(pObj.ptr);
        if(obj != NULL){
            NixAVAudioEngine_destroy(obj);
            NIX_FREE(obj);
            obj = NULL;
        }
        NixSharedPtr_destroy(pObj.ptr);
    }
    //itf belongs to Engine
}

void nixAVAudioEngine_printCaps(STNixApiEngineRef pObj){
    //
}

NixBOOL nixAVAudioEngine_ctxIsActive(STNixApiEngineRef pObj){
    NixBOOL r = NIX_FALSE;
    STNixAVAudioEngine* obj = (STNixAVAudioEngine*)NixSharedPtr_getOpq(pObj.ptr);
    if(obj != NULL){
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL nixAVAudioEngine_ctxActivate(STNixApiEngineRef pObj){
    NixBOOL r = NIX_FALSE;
    STNixAVAudioEngine* obj = (STNixAVAudioEngine*)NixSharedPtr_getOpq(pObj.ptr);
    if(obj != NULL){
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL nixAVAudioEngine_ctxDeactivate(STNixApiEngineRef pObj){
    NixBOOL r = NIX_FALSE;
    STNixAVAudioEngine* obj = (STNixAVAudioEngine*)NixSharedPtr_getOpq(pObj.ptr);
    if(obj != NULL){
        r = NIX_TRUE;
    }
    return r;
}

void nixAVAudioEngine_tick(STNixApiEngineRef pObj){
    STNixAVAudioEngine* obj = (STNixAVAudioEngine*)NixSharedPtr_getOpq(pObj.ptr);
    if(obj != NULL){
        NixAVAudioEngine_tick(obj, NIX_FALSE);
    }
}

//------
//Source API
//------
  
STNixApiSourceRef nixAVAudioSource_alloc(STNixApiEngineRef pEng){
    STNixApiSourceRef r = STNixApiSourceRef_Zero;
    STNixAVAudioEngine* eng = (STNixAVAudioEngine*)NixSharedPtr_getOpq(pEng.ptr);
    if(eng != NULL){
        STNixContextItf* ctx = &eng->ctx;
        STNixAVAudioSource* obj = NULL;
        NIX_MALLOC(obj, STNixAVAudioSource, sizeof(STNixAVAudioSource), "STNixAVAudioSource");
        if(obj != NULL){
            NixAVAudioSource_init(obj);
            //
            obj->ctx = ctx;
            obj->engp = eng;
            //
            obj->eng = [[AVAudioEngine alloc] init];
            obj->src = [[AVAudioPlayerNode alloc] init];
            {
                //attach
                [obj->eng attachNode:obj->src];
                //connect
                [obj->eng connect:obj->src to:[obj->eng outputNode] format:nil];
                //
                if(!obj->engStarted){
                    NSError* err = nil;
                    [obj->eng prepare];
                    if(![obj->eng startAndReturnError:&err]){
                        NIX_PRINTF_ERROR("nixAVAudioSource_create, AVAudioEngine::startAndReturnError failed: '%s'.\n", err == nil ? "unknown" : [[err description] UTF8String]);
                        [obj->src release]; obj->src = nil;
                        [obj->eng release]; obj->eng = nil;
                        NIX_FREE(obj);
                        obj = NULL;
                    } else {
                        obj->engStarted = NIX_TRUE;
                    }
                }
            }
        }
        //add to engine
        if(!NixAVAudioEngine_srcsAdd(eng, obj)){
            NIX_PRINTF_ERROR("nixAVAudioSource_create::NixAVAudioEngine_srcsAdd failed.\n");
        } else if(NULL == (r.ptr = NixSharedPtr_create(obj))){
            NIX_PRINTF_ERROR("nixAAudioEngine_create::NixSharedPtr_create failed.\n");
        } else {
            r.itf = &eng->apiItf.source;
            obj = NULL; //consume
        }
        //release (if not consumed)
        if(obj != NULL){
            NixAVAudioSource_destroy(obj);
            NIX_FREE(obj);
            obj = NULL;
        }
    }
    return r;
}

void nixAVAudioSource_removeAllBuffersAndNotify_(STNixAVAudioSource* obj){
    STNixAVAudioNotifQueue notifs;
    NixAVAudioNotifQueue_init(&notifs);
    //move all pending buffers to notify
    NIX_MUTEX_LOCK(&obj->queues.mutex);
    {
        NixAVAudioSource_pendPopAllBuffsLocked_(obj);
        NixAVAudioEngine_tick_addQueueNotifSrcLocked_(&notifs, obj);
    }
    NIX_MUTEX_UNLOCK(&obj->queues.mutex);
    //notify
    {
        NixUI32 i;
        for(i = 0; i < notifs.use; ++i){
            STNixAVAudioSrcNotif* n = &notifs.arr[i];
            NIX_PRINTF_INFO("nixAVAudioSource_removeAllBuffersAndNotify_::notify(#%d/%d).\n", i + 1, notifs.use);
            if(n->callback.func != NULL){
                (*n->callback.func)(n->callback.eng, n->callback.sourceIndex, n->ammBuffs);
            }
        }
    }
}

void nixAVAudioSource_free(STNixApiSourceRef pObj){
    if(pObj.ptr != NULL){
        STNixAVAudioSource* obj = (STNixAVAudioSource*)NixSharedPtr_getOpq(pObj.ptr);
        if(obj != NULL){
            if(obj->src != NULL){
                [obj->src stop];
            }
            //flush all pending buffers
            nixAVAudioSource_removeAllBuffersAndNotify_(obj);
            //Flag as orphan, for cleanup inside 'tick'
            NixAVAudioSource_setIsOrphan(obj);
        }
        NixSharedPtr_destroy(pObj.ptr);
    }
    //itf belongs to Engine
}

void nixAVAudioSource_setCallback(STNixApiSourceRef pObj, void (*callback)(void* pEng, const NixUI32 sourceIndex, const NixUI32 ammBuffs), void* callbackEng, NixUI32 callbackSourceIndex){
    if(pObj.ptr != NULL){
        STNixAVAudioSource* obj = (STNixAVAudioSource*)NixSharedPtr_getOpq(pObj.ptr);
        obj->queues.callback.func  = callback;
        obj->queues.callback.eng   = callbackEng;
        obj->queues.callback.sourceIndex = callbackSourceIndex;
    }
}

NixBOOL nixAVAudioSource_setVolume(STNixApiSourceRef pObj, const float vol){
    NixBOOL r = NIX_FALSE;
    if(pObj.ptr != NULL){
        STNixAVAudioSource* obj = (STNixAVAudioSource*)NixSharedPtr_getOpq(pObj.ptr);
        [obj->src setVolume:(vol < 0.f ? 0.f : vol > 1.f ? 1.f : vol)];
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL nixAVAudioSource_setRepeat(STNixApiSourceRef pObj, const NixBOOL isRepeat){
    NixBOOL r = NIX_FALSE;
    if(pObj.ptr != NULL){
        STNixAVAudioSource* obj = (STNixAVAudioSource*)NixSharedPtr_getOpq(pObj.ptr);
        NixAVAudioSource_setIsRepeat(obj, isRepeat);
        r = NIX_TRUE;
    }
    return r;
}

   
void nixAVAudioSource_play(STNixApiSourceRef pObj){
    if(pObj.ptr != NULL){
        STNixAVAudioSource* obj = (STNixAVAudioSource*)NixSharedPtr_getOpq(pObj.ptr);
        //
        NixAVAudioSource_setIsPlaying(obj, NIX_TRUE);
        NixAVAudioSource_setIsPaused(obj, NIX_FALSE);
        NixAVAudioSource_scheduleEnqueuedBuffers(obj);
        [obj->src play];
    }
}

void nixAVAudioSource_pause(STNixApiSourceRef pObj){
    if(pObj.ptr != NULL){
        STNixAVAudioSource* obj = (STNixAVAudioSource*)NixSharedPtr_getOpq(pObj.ptr);
        [obj->src pause];
        NixAVAudioSource_setIsPaused(obj, NIX_TRUE);
    }
}

void nixAVAudioSource_stop(STNixApiSourceRef pObj){
    if(pObj.ptr != NULL){
        STNixAVAudioSource* obj = (STNixAVAudioSource*)NixSharedPtr_getOpq(pObj.ptr);
        NixAVAudioSource_setIsPlaying(obj, NIX_FALSE);
        NixAVAudioSource_setIsPaused(obj, NIX_FALSE);
        [obj->src stop];
        //flush all pending buffers
        nixAVAudioSource_removeAllBuffersAndNotify_(obj);
    }
}

NixBOOL nixAVAudioSource_isPlaying(STNixApiSourceRef pObj){
    NixBOOL r = NIX_FALSE;
    if(pObj.ptr != NULL){
        STNixAVAudioSource* obj = (STNixAVAudioSource*)NixSharedPtr_getOpq(pObj.ptr);
        r = (obj->src != nil && [obj->src isPlaying] && obj->queues.pendScheduledCount > 0) ? NIX_TRUE : NIX_FALSE;
    }
    return r;
}

NixBOOL nixAVAudioSource_isPaused(STNixApiSourceRef pObj){
    NixBOOL r = NIX_FALSE;
    if(pObj.ptr != NULL){
        STNixAVAudioSource* obj = (STNixAVAudioSource*)NixSharedPtr_getOpq(pObj.ptr);
        r = NixAVAudioSource_isPlaying(obj) && NixAVAudioSource_isPaused(obj) ? NIX_TRUE : NIX_FALSE;
    }
    return r;
}

void* nixAVAudioSource_createConverter(const STNixAudioDesc* srcFmt, AVAudioFormat* outFmt){
    void* r = NULL;
    STNixAudioDesc outDesc;
    memset(&outDesc, 0, sizeof(outDesc));
    outDesc.samplerate  = [outFmt sampleRate];
    outDesc.channels    = [outFmt channelCount];
    switch([outFmt commonFormat]) {
        case AVAudioPCMFormatFloat32:
            outDesc.bitsPerSample   = 32;
            outDesc.samplesFormat   = ENNixSampleFmt_Float;
            outDesc.blockAlign      = (outDesc.bitsPerSample / 8) * ([outFmt isInterleaved] ? outDesc.channels : 1);
            break;
        case AVAudioPCMFormatInt16:
            outDesc.bitsPerSample   = 16;
            outDesc.samplesFormat   = ENNixSampleFmt_Int;
            outDesc.blockAlign      = (outDesc.bitsPerSample / 8) * ([outFmt isInterleaved] ? outDesc.channels : 1);
            break;
        case AVAudioPCMFormatInt32:
            outDesc.bitsPerSample   = 32;
            outDesc.samplesFormat   = ENNixSampleFmt_Int;
            outDesc.blockAlign      = (outDesc.bitsPerSample / 8) * ([outFmt isInterleaved] ? outDesc.channels : 1);
            break;
        default:
            break;
    }
    if(outDesc.bitsPerSample > 0){
        r = nixFmtConverter_alloc();
        if(!nixFmtConverter_prepare(r, srcFmt, &outDesc)){
            nixFmtConverter_free(r);
            r = NULL;
        }
    }
    return r;
}

NixBOOL nixAVAudioSource_setBuffer(STNixApiSourceRef pObj, STNixApiBufferRef pBuff){  //static-source
    NixBOOL r = NIX_FALSE;
    if(pObj.ptr != NULL && pBuff.ptr != NULL){
        STNixAVAudioSource* obj    = (STNixAVAudioSource*)NixSharedPtr_getOpq(pObj.ptr);
        STNixPCMBuffer* buff = (STNixPCMBuffer*)NixSharedPtr_getOpq(pBuff.ptr);
        if(obj->queues.conv != NULL || obj->buffsFmt.blockAlign > 0){
            //error, buffer already set
        } else {
            AVAudioOutputNode* outNode  = [obj->eng outputNode];
            AVAudioFormat* outFmt       = [outNode outputFormatForBus:0];
            obj->queues.conv = nixAVAudioSource_createConverter(&buff->desc, outFmt);
            if(obj->queues.conv == NULL){
                NIX_PRINTF_ERROR("nixAVAudioSource_queueBuffer, nixAVAudioSource_createConverter failed.\n");
            } else {
                //set format
                obj->buffsFmt = buff->desc;
                NixAVAudioSource_setIsStatic(obj, NIX_TRUE);
                if(!NixAVAudioSource_queueBufferForOutput(obj, pBuff)){
                    NIX_PRINTF_ERROR("nixAVAudioSource_queueBuffer, NixAVAudioSource_queueBufferForOutput failed.\n");
                } else {
                    //enqueue
                    if(NixAVAudioSource_isPlaying(obj) && !NixAVAudioSource_isPaused(obj)){
                        NixAVAudioSource_scheduleEnqueuedBuffers(obj);
                    }
                    r = NIX_TRUE;
                }
            }
        }
    }
    return r;
}

//2025-07-08
//Note: AVAudioMixerNode and all attempts to play 22050Hz 16-bits mono audio produces Operating-System assertions.
//Implementing manual-samples conversion as result.
//
NixBOOL nixAVAudioSource_queueBuffer(STNixApiSourceRef pObj, STNixApiBufferRef pBuff) {
    NixBOOL r = NIX_FALSE;
    if(pObj.ptr != NULL && pBuff.ptr != NULL){
        STNixAVAudioSource* obj    = (STNixAVAudioSource*)NixSharedPtr_getOpq(pObj.ptr);
        STNixPCMBuffer* buff = (STNixPCMBuffer*)NixSharedPtr_getOpq(pBuff.ptr);
        //define format
        if(obj->queues.conv == NULL && obj->buffsFmt.blockAlign <= 0){
            //first buffer, define as format
            AVAudioOutputNode* outNode  = [obj->eng outputNode];
            AVAudioFormat* outFmt       = [outNode outputFormatForBus:0];
            obj->queues.conv = nixAVAudioSource_createConverter(&buff->desc, outFmt);
            if(obj->queues.conv == NULL){
                //error, converter creation failed
            } else {
                //set format
                obj->buffsFmt = buff->desc;
            }
        }
        //queue buffer
        if(!STNixAudioDesc_IsEqual(&obj->buffsFmt, &buff->desc)){
            NIX_PRINTF_ERROR("nixAVAudioSource_queueBuffer, new buffer doesnt match first buffer's format.\n");
        } else if(!NixAVAudioSource_queueBufferForOutput(obj, pBuff)){
            NIX_PRINTF_ERROR("nixAVAudioSource_queueBuffer, NixAVAudioSource_queueBufferForOutput failed.\n");
        } else {
            //enqueue
            if(NixAVAudioSource_isPlaying(obj) && !NixAVAudioSource_isPaused(obj)){
                NixAVAudioSource_scheduleEnqueuedBuffers(obj);
            }
            r = NIX_TRUE;
        }
    }
    return r;
}

//------
//nixFmtConverter API
//------

void nixFmtConverter_buffFmtToAudioDesc(AVAudioFormat* buffFmt, STNixAudioDesc* dst){
    if(buffFmt != nil && dst != NULL){
        memset(dst, 0, sizeof(*dst));
        dst->samplerate  = [buffFmt sampleRate];
        dst->channels    = [buffFmt channelCount];
        switch([buffFmt commonFormat]) {
            case AVAudioPCMFormatFloat32:
                dst->bitsPerSample   = 32;
                dst->samplesFormat   = ENNixSampleFmt_Float;
                dst->blockAlign      = (dst->bitsPerSample / 8) * ([buffFmt isInterleaved] ? dst->channels : 1);
                break;
            case AVAudioPCMFormatInt16:
                dst->bitsPerSample   = 16;
                dst->samplesFormat   = ENNixSampleFmt_Int;
                dst->blockAlign      = (dst->bitsPerSample / 8) * ([buffFmt isInterleaved] ? dst->channels : 1);
                break;
            case AVAudioPCMFormatInt32:
                dst->bitsPerSample   = 32;
                dst->samplesFormat   = ENNixSampleFmt_Int;
                dst->blockAlign      = (dst->bitsPerSample / 8) * ([buffFmt isInterleaved] ? dst->channels : 1);
                break;
            default:
                break;
        }
    }
}



//------
//Recorder API
//------

STNixApiRecorderRef nixAVAudioRecorder_alloc(STNixApiEngineRef pEng, const STNixAudioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer){
    STNixApiRecorderRef r = STNixApiRecorderRef_Zero;
    STNixAVAudioEngine* eng = (STNixAVAudioEngine*)NixSharedPtr_getOpq(pEng.ptr);
    if(eng != NULL && audioDesc != NULL && audioDesc->samplerate > 0 && audioDesc->blockAlign > 0 && eng->rec == NULL){
        STNixAVAudioRecorder* obj = NULL;
        NIX_MALLOC(obj, STNixAVAudioRecorder, sizeof(STNixAVAudioRecorder), "STNixAVAudioRecorder");
        if(obj != NULL){
            NixAVAudioRecorder_init(obj);
            if(!NixAVAudioRecorder_prepare(obj, eng, audioDesc, buffersCount, samplesPerBuffer)){
                NIX_PRINTF_ERROR("nixAVAudioRecorder_create, NixAVAudioRecorder_prepare failed.\n");
            } else if(NULL == (r.ptr = NixSharedPtr_create(obj))){
                NIX_PRINTF_ERROR("nixAVAudioRecorder_create::NixSharedPtr_create failed.\n");
            } else {
                r.itf = &eng->apiItf.recorder;
                obj->engRef     = pEng; NixApiEngineRef_retain(&pEng);
                obj->selfRef    = r;
                eng->rec        = obj; obj = NULL; //consume
            }
        }
        //release (if not consumed)
        if(obj != NULL){
            NixAVAudioRecorder_destroy(obj);
            NIX_FREE(obj);
            obj = NULL;
        }
    }
    return r;
}

void nixAVAudioRecorder_free(STNixApiRecorderRef pObj){
    if(pObj.ptr != NULL){
        STNixAVAudioRecorder* obj = (STNixAVAudioRecorder*)NixSharedPtr_getOpq(pObj.ptr);
        if(obj != NULL){
            NixAVAudioRecorder_destroy(obj);
            NIX_FREE(obj);
            obj = NULL;
        }
        NixSharedPtr_destroy(pObj.ptr);
    }
    //itf belongs to Engine
}

NixBOOL nixAVAudioRecorder_setCallback(STNixApiRecorderRef pObj, NixApiRecorderCallback callback, void* callbackData){
    NixBOOL r = NIX_FALSE;
    STNixAVAudioRecorder* obj = (STNixAVAudioRecorder*)NixSharedPtr_getOpq(pObj.ptr);
    if(obj != NULL){
        r = NixAVAudioRecorder_setCallback(obj, callback, callbackData);
    }
    return r;
}

NixBOOL nixAVAudioRecorder_start(STNixApiRecorderRef pObj){
    NixBOOL r = NIX_FALSE;
    STNixAVAudioRecorder* obj = (STNixAVAudioRecorder*)NixSharedPtr_getOpq(pObj.ptr);
    if(obj != NULL){
        r = NixAVAudioRecorder_start(obj);
    }
    return r;
}

NixBOOL nixAVAudioRecorder_stop(STNixApiRecorderRef pObj){
    NixBOOL r = NIX_FALSE;
    STNixAVAudioRecorder* obj = (STNixAVAudioRecorder*)NixSharedPtr_getOpq(pObj.ptr);
    if(obj != NULL){
        r = NixAVAudioRecorder_stop(obj);
    }
    return r;
}



