//
//  nixtla.c
//  NixtlaAudioLib
//
//  Created by Marcos Ortega on 11/02/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//
//  This entire notice must be retained in this source code.
//  This source code is under MIT Licence.
//
//  This software is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
//
//  Latest fixes enhancements and documentation at https://github.com/marcosjom/lib-nixtla-audio
//

#include "nixtla-audio-private.h"
#include "nixtla-audio.h"
//
#include "nixtla-openal.h"
#include "nixtla-aaudio.h"
#include "nixtla-avfaudio.h"
//
#include <stdio.h>  //NULL
#include <string.h> //memcpy, memset
#include <stdlib.h> //malloc

//-------------------------------
//-- IDENTIFY OS
//-------------------------------
#if defined(_WIN32) || defined(WIN32) //Windows
#   pragma message("NIXTLA-AUDIO, COMPILING FOR WINDOWS (OpenAL as default API)")
#   include <AL/al.h>
#   include <AL/alc.h>
#   define NIX_DEFAULT_API_OPENAL
#elif defined(__ANDROID__) //Android
//#   pragma message("NIXTLA-AUDIO, COMPILING FOR ANDROID (OpenSL)")
//#   include <SLES/OpenSLES.h>
//#   include <SLES/OpenSLES_Android.h>
//#   define NIX_OPENSL
#   pragma message("NIXTLA-AUDIO, COMPILING FOR ANDROID 8+ (API 26+, Oreo) (AAudio as default API)")
#   define NIX_DEFAULT_API_AAUDIO
#elif defined(__QNX__) //QNX 4, QNX Neutrino, or BlackBerry 10 OS
#   pragma message("NIXTLA-AUDIO, COMPILING FOR QNX / BB10 (OpenAL as default API)")
#   include <AL/al.h>
#   include <AL/alc.h>
#   define NIX_DEFAULT_API_OPENAL
#elif defined(__linux__) || defined(linux) //Linux
#   pragma message("NIXTLA-AUDIO, COMPILING FOR LINUX (OpenAL as default API)")
#   include <AL/al.h>	//remember to install "libopenal-dev" package
#   include <AL/alc.h> //remember to install "libopenal-dev" package
#   define NIX_DEFAULT_API_OPENAL
#elif defined(__MAC_OS_X_VERSION_MAX_ALLOWED) //OSX
#   pragma message("NIXTLA-AUDIO, COMPILING FOR MacOSX (AVFAudio as default API)")
#   define NIX_DEFAULT_API_AVFAUDIO
#else	//iOS?
#   pragma message("NIXTLA-AUDIO, COMPILING FOR iOS? (AVFAudio as default API)")
#   define NIX_DEFAULT_API_AVFAUDIO
#endif


//Default API

NixBOOL NixApiItf_getDefaultForCurrentOS(STNixApiItf* dst){
    NixBOOL r = NIX_FALSE;
    if(dst != NULL){
        //API
#       ifdef NIX_DEFAULT_API_AVFAUDIO
        r = nixAVAudioEngine_getApiItf(dst);
#       elif defined(NIX_DEFAULT_API_AAUDIO)
        r = nixAAudioEngine_getApiItf(dst);
#       elif defined(NIX_DEFAULT_API_OPENAL)
        r = nixOpenALEngine_getApiItf(dst);
#       endif
    }
    return r;
}

//Audio description
   
NixBOOL STNixAudioDesc_isEqual(const STNixAudioDesc* obj, const STNixAudioDesc* other){
    return (obj->samplesFormat == other->samplesFormat
            && obj->channels == other->channels
            && obj->bitsPerSample == other->bitsPerSample
            && obj->samplerate == other->samplerate
            && obj->blockAlign == other->blockAlign) ? NIX_TRUE : NIX_FALSE;
}

// STNixSharedPtr (provides retain/release model)

typedef struct STNixSharedPtr_ {
    void*           opq;    //opaque object, must be first member to allow toll-free casting
    STNixMutexRef   mutex;
    NixSI32         retainCount;
    STNixMemoryItf  memItf;
} STNixSharedPtr;

struct STNixSharedPtr_* NixSharedPtr_alloc(STNixContextItf* itf, void* opq){
    struct STNixSharedPtr_* obj = NULL;
    obj = (struct STNixSharedPtr_*)(*itf->mem.malloc)(sizeof(struct STNixSharedPtr_), "NixSharedPtr_alloc");
    if(obj != NULL){
        obj->mutex = (itf->mutex.alloc)(itf);
        obj->retainCount = 1; //retained by creator
        //
        obj->opq = opq;
        obj->memItf = itf->mem;
    }
    return obj;
}

void NixSharedPtr_free(struct STNixSharedPtr_* obj){
    NixMutex_free(&obj->mutex);
    (*obj->memItf.free)(obj);
}

void* NixSharedPtr_getOpq(struct STNixSharedPtr_* obj){
    //can also be obtained by casting 'obj' as '*(void**)obj'
    return (obj != NULL ? obj->opq : NULL);
}

void NixSharedPtr_retain(struct STNixSharedPtr_* obj){
    NixMutex_lock(obj->mutex);
    {
        ++obj->retainCount;
    }
    NixMutex_unlock(obj->mutex);
}

NixSI32 NixSharedPtr_release(struct STNixSharedPtr_* obj){
    NixSI32 r = 0;
    NixMutex_lock(obj->mutex);
    {
        r = --obj->retainCount;
    }
    NixMutex_unlock(obj->mutex);
    return r;
}

#define NIX_REF_METHOD_DEFINITION_VOID(TYPE, METHOD, PARAMS_DEFS, PARAMS_NAMES)   \
void TYPE ## _ ## METHOD PARAMS_DEFS { \
        if(ref.itf != NULL && ref.itf->METHOD != NULL){ \
            (*ref.itf->METHOD)PARAMS_NAMES; \
        } \
    }

#define NIX_REF_METHOD_DEFINITION_BOOL(TYPE, METHOD, PARAMS_DEFS, PARAMS_NAMES)   \
    NixBOOL TYPE ## _ ## METHOD PARAMS_DEFS{ \
        return (ref.itf != NULL && ref.itf->METHOD != NULL ? (*ref.itf->METHOD)PARAMS_NAMES : NIX_FALSE); \
    }


//STNixApiEngineRef (shared pointer)

#define STNixApiEngineRef_Zero     { NULL, NULL }

STNixApiEngineRef NixApiEngine_alloc(STNixContextRef ctx, struct STNixApiItf_* apiItf){
    return (ctx.ptr != NULL && apiItf != NULL && apiItf->engine.alloc != NULL ? (*apiItf->engine.alloc)(ctx) : (STNixApiEngineRef)STNixApiEngineRef_Zero);
}

void NixApiEngine_retain(STNixApiEngineRef ref){
    NixSharedPtr_retain(ref.ptr);
}

void NixApiEngine_release(STNixApiEngineRef* ref){
    if(ref != NULL && ref->ptr != NULL){
        if(0 == NixSharedPtr_release(ref->ptr)){
            if(ref->itf != NULL && ref->itf->free != NULL){
                (*ref->itf->free)(*ref);
            }
        }
    }
}

NIX_REF_METHOD_DEFINITION_VOID(NixApiEngine, printCaps, (STNixApiEngineRef ref), (ref))
NIX_REF_METHOD_DEFINITION_BOOL(NixApiEngine, ctxIsActive, (STNixApiEngineRef ref), (ref))
NIX_REF_METHOD_DEFINITION_BOOL(NixApiEngine, ctxActivate, (STNixApiEngineRef ref), (ref))
NIX_REF_METHOD_DEFINITION_BOOL(NixApiEngine, ctxDeactivate, (STNixApiEngineRef ref), (ref))
NIX_REF_METHOD_DEFINITION_VOID(NixApiEngine, tick, (STNixApiEngineRef ref), (ref))

//Factory

STNixApiSourceRef NixApiEngine_sourceAlloc(STNixApiEngineRef ref){
    return (ref.itf != NULL && ref.itf->sourceAlloc != NULL ? (*ref.itf->sourceAlloc)(ref) : (STNixApiSourceRef)STNixApiSourceRef_Zero);
}

STNixApiBufferRef NixApiEngine_bufferAlloc(STNixApiEngineRef ref, const STNixAudioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes){
    return (ref.itf != NULL && ref.itf->bufferAlloc != NULL ? (*ref.itf->bufferAlloc)(ref, audioDesc, audioDataPCM, audioDataPCMBytes) : (STNixApiBufferRef)STNixApiBufferRef_Zero);
}

STNixApiRecorderRef NixApiEngine_recorderAlloc(STNixApiEngineRef ref, const STNixAudioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer){
    return (ref.itf != NULL && ref.itf->recorderAlloc != NULL ? (*ref.itf->recorderAlloc)(ref, audioDesc, buffersCount, samplesPerBuffer) : (STNixApiRecorderRef)STNixApiRecorderRef_Zero);
}


//STNixApiBufferRef (shared pointer)

#define STNixApiBufferRef_Zero     { NULL, NULL }

void NixApiBuffer_retain(STNixApiBufferRef ref){
    NixSharedPtr_retain(ref.ptr);
}

void NixApiBuffer_release(STNixApiBufferRef* ref){
    if(ref != NULL && ref->ptr != NULL){
        if(0 == NixSharedPtr_release(ref->ptr)){
            if(ref->itf != NULL && ref->itf->free != NULL){
                (*ref->itf->free)(*ref);
            }
        }
    }
}

NIX_REF_METHOD_DEFINITION_BOOL(NixApiBuffer, setData, (STNixApiBufferRef ref, const STNixAudioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes), (ref, audioDesc, audioDataPCM, audioDataPCMBytes) )
NIX_REF_METHOD_DEFINITION_BOOL(NixApiBuffer, fillWithZeroes, (STNixApiBufferRef ref), (ref))


//STNixApiSourceRef (shared pointer)

#define STNixApiSourceRef_Zero     { NULL, NULL }

void NixApiSource_retain(STNixApiSourceRef obj){
    NixSharedPtr_retain(obj.ptr);
}

void NixApiSource_release(STNixApiSourceRef* obj){
    if(obj != NULL && obj->ptr != NULL){
        if(0 == NixSharedPtr_release(obj->ptr)){
            if(obj->itf != NULL && obj->itf->free != NULL){
                (*obj->itf->free)(*obj);
            }
        }
    }
}

NIX_REF_METHOD_DEFINITION_VOID(NixApiSource, setCallback, (STNixApiSourceRef ref, void (*callback)(void* pEng, const NixUI32 sourceIndex, const NixUI32 ammBuffs), void* callbackEng, NixUI32 callbackSourceIndex), (ref, callback, callbackEng, callbackSourceIndex))
NIX_REF_METHOD_DEFINITION_BOOL(NixApiSource, setVolume, (STNixApiSourceRef ref, const float vol), (ref, vol))
NIX_REF_METHOD_DEFINITION_BOOL(NixApiSource, setRepeat, (STNixApiSourceRef ref, const NixBOOL isRepeat), (ref, isRepeat))
NIX_REF_METHOD_DEFINITION_VOID(NixApiSource, play, (STNixApiSourceRef ref), (ref))
NIX_REF_METHOD_DEFINITION_VOID(NixApiSource, pause, (STNixApiSourceRef ref), (ref))
NIX_REF_METHOD_DEFINITION_VOID(NixApiSource, stop, (STNixApiSourceRef ref), (ref))
NIX_REF_METHOD_DEFINITION_BOOL(NixApiSource, isPlaying, (STNixApiSourceRef ref), (ref))
NIX_REF_METHOD_DEFINITION_BOOL(NixApiSource, isPaused, (STNixApiSourceRef ref), (ref))
NIX_REF_METHOD_DEFINITION_BOOL(NixApiSource, setBuffer, (STNixApiSourceRef ref, STNixApiBufferRef buff), (ref, buff))
NIX_REF_METHOD_DEFINITION_BOOL(NixApiSource, queueBuffer, (STNixApiSourceRef ref, STNixApiBufferRef buff), (ref, buff))

//STNixApiRecorderRef (shared pointer)

#define STNixApiRecorderRef_Zero     { NULL, NULL }

void NixApiRecorder_retain(STNixApiRecorderRef obj){
    NixSharedPtr_retain(obj.ptr);
}

void NixApiRecorder_release(STNixApiRecorderRef* obj){
    if(obj != NULL && obj->ptr != NULL){
        if(0 == NixSharedPtr_release(obj->ptr)){
            if(obj->itf != NULL && obj->itf->free != NULL){
                (*obj->itf->free)(*obj);
            }
        }
    }
}

NIX_REF_METHOD_DEFINITION_BOOL(NixApiRecorder, setCallback, (STNixApiRecorderRef ref, NixApiRecorderCallbackFnc callback, void* callbackData), (ref, callback, callbackData))
NIX_REF_METHOD_DEFINITION_BOOL(NixApiRecorder, start, (STNixApiRecorderRef ref), (ref))
NIX_REF_METHOD_DEFINITION_BOOL(NixApiRecorder, stop, (STNixApiRecorderRef ref), (ref))

// STNixMutexRef

void NixMutex_lock(STNixMutexRef ref){
    if(ref.opq != NULL && ref.itf != NULL && ref.itf->lock != NULL){
        (*ref.itf->lock)(ref);
    }
}

void NixMutex_unlock(STNixMutexRef ref){
    if(ref.opq != NULL && ref.itf != NULL && ref.itf->unlock != NULL){
        (*ref.itf->unlock)(ref);
    }
}

void NixMutex_free(STNixMutexRef* ref){
    if(ref->opq != NULL && ref->itf != NULL && ref->itf->free != NULL){
        (*ref->itf->free)(ref);
    }
}

//

#define NIX_ITF_SET_MISSING_METHOD_TO_NOP(ITF, ITF_NAME, M_NAME) \
    if(itf->M_NAME == NULL) itf->M_NAME = ITF_NAME ## _nop_ ## M_NAME

#define NIX_ITF_SET_MISSING_METHOD_TO_DEFAULT(ITF, ITF_NAME, M_NAME) \
    if(itf->M_NAME == NULL) itf->M_NAME = ITF_NAME ## _default_ ## M_NAME

//STNixMemoryItf (API)

void* NixMemoryItf_default_malloc(const NixUI32 newSz, const char* dbgHintStr){
    return malloc(newSz);
}

void* NixMemoryItf_default_realloc(void* ptr, const NixUI32 newSz){
    return realloc(ptr, newSz);
}

void NixMemoryItf_default_free(void* ptr){
    free(ptr);
}

//Links NULL methods to a DEFAULT implementation,
//this reduces the need to check for functions NULL pointers.
void NixMemoryItf_fillMissingMembers(STNixMemoryItf* itf){
    if(itf == NULL) return;
    NIX_ITF_SET_MISSING_METHOD_TO_DEFAULT(itf, NixMemoryItf, malloc);
    NIX_ITF_SET_MISSING_METHOD_TO_DEFAULT(itf, NixMemoryItf, realloc);
    NIX_ITF_SET_MISSING_METHOD_TO_DEFAULT(itf, NixMemoryItf, free);
    //validate missing implementations
#   ifdef NIX_ASSERTS_ACTIVATED
    {
        void** ptr = (void**)itf;
        void** afterEnd = ptr + (sizeof(*itf) / sizeof(void*));
        while(ptr < afterEnd){
            NIX_ASSERT(*ptr != NULL) //function pointer should be defined
            ptr++;
        }
    }
#   endif
}

//STNixMutexItf (API)

#if !defined(NIX_MUTEX_T) || !defined(NIX_MUTEX_INIT) || !defined(NIX_MUTEX_DESTROY) || !defined(NIX_MUTEX_LOCK) || !defined(NIX_MUTEX_UNLOCK)
#   ifdef _WIN32
//#     define WIN32_LEAN_AND_MEAN
#       include <windows.h>             //for CRITICAL_SECTION
#       define NIX_MUTEX_T              CRITICAL_SECTION
#       define NIX_MUTEX_INIT(PTR)      InitializeCriticalSection(PTR)
#       define NIX_MUTEX_DESTROY(PTR)   DeleteCriticalSection(PTR)
#       define NIX_MUTEX_LOCK(PTR)      EnterCriticalSection(PTR)
#       define NIX_MUTEX_UNLOCK(PTR)    LeaveCriticalSection(PTR)
#   else
#       include <pthread.h>             //for pthread_mutex_t
#       define NIX_MUTEX_T              pthread_mutex_t
#       define NIX_MUTEX_INIT(PTR)      pthread_mutex_init(PTR, NULL)
#       define NIX_MUTEX_DESTROY(PTR)   pthread_mutex_destroy(PTR)
#       define NIX_MUTEX_LOCK(PTR)      pthread_mutex_lock(PTR)
#       define NIX_MUTEX_UNLOCK(PTR)    pthread_mutex_unlock(PTR)
#   endif
#endif

typedef struct STNixMutexOpq_ {
    NIX_MUTEX_T             mutex;
    struct STNixContextItf_ ctx;
} STNixMutexOpq;

STNixMutexRef NixMutexItf_default_alloc(struct STNixContextItf_* ctx){
    STNixMutexRef r = STNixMutexRef_Zero;
    STNixMutexOpq* obj = (STNixMutexOpq*)(*ctx->mem.malloc)(sizeof(STNixMutexOpq), "NixMutexItf_default_alloc");
    if(obj != NULL){
        NIX_MUTEX_INIT(&obj->mutex);
        obj->ctx = *ctx;
        r.opq = obj;
        r.itf = &obj->ctx.mutex;
    }
    return r;
}

void NixMutexItf_default_free(STNixMutexRef* pObj){
    if(pObj != NULL){
        STNixMutexOpq* obj = (STNixMutexOpq*)pObj->opq;
        if(obj != NULL){
            NIX_MUTEX_DESTROY(&obj->mutex);
            (*obj->ctx.mem.free)(obj);
        }
        pObj->opq = NULL;
        pObj->itf = NULL;
    }
}

void NixMutexItf_default_lock(STNixMutexRef pObj){
    if(pObj.opq != NULL){
        STNixMutexOpq* obj = (STNixMutexOpq*)pObj.opq;
        NIX_MUTEX_LOCK(&obj->mutex);
    }
}

void NixMutexItf_default_unlock(STNixMutexRef pObj){
    if(pObj.opq != NULL){
        STNixMutexOpq* obj = (STNixMutexOpq*)pObj.opq;
        NIX_MUTEX_UNLOCK(&obj->mutex);
    }
}

//Links NULL methods to a DEFAULT implementation,
//this reduces the need to check for functions NULL pointers.
void NixMutexItf_fillMissingMembers(STNixMutexItf* itf){
    if(itf == NULL) return;
    NIX_ITF_SET_MISSING_METHOD_TO_DEFAULT(itf, NixMutexItf, alloc);
    NIX_ITF_SET_MISSING_METHOD_TO_DEFAULT(itf, NixMutexItf, free);
    NIX_ITF_SET_MISSING_METHOD_TO_DEFAULT(itf, NixMutexItf, lock);
    NIX_ITF_SET_MISSING_METHOD_TO_DEFAULT(itf, NixMutexItf, unlock);
    //validate missing implementations
#   ifdef NIX_ASSERTS_ACTIVATED
    {
        void** ptr = (void**)itf;
        void** afterEnd = ptr + (sizeof(*itf) / sizeof(void*));
        while(ptr < afterEnd){
            NIX_ASSERT(*ptr != NULL) //function pointer should be defined
            ptr++;
        }
    }
#   endif
}

//STNixContextRef

typedef struct STNixContextOpq_ {
    NixUI32 dummy;  //empty struct
} STNixContextOpq;

STNixContextRef NixContext_alloc(STNixContextItf* ctx){
    STNixContextRef r = STNixContextRef_Zero;
    if(ctx != NULL){
        STNixContextOpq* opq = (STNixContextOpq*)(ctx->mem.malloc)(sizeof(STNixContextOpq), "NixContext_alloc::opq");
        STNixContextItf* itf = (STNixContextItf*)(ctx->mem.malloc)(sizeof(STNixContextItf), "NixContext_alloc::itf");
        STNixSharedPtr* ptr  = NixSharedPtr_alloc(ctx, opq);
        if(opq != NULL && itf != NULL && ptr != NULL){
            memset(opq, 0, sizeof(*opq));
            memcpy(itf, ctx, sizeof(*itf));
            r.ptr = ptr; ptr = NULL; //consume
            r.itf = itf; itf = NULL; //consume
            opq = NULL; //consume
        }
        //release (if not consumed)
        if(opq != NULL){
            (ctx->mem.free)(opq);
            opq = NULL;
        }
        if(itf != NULL){
            (ctx->mem.free)(itf);
            itf = NULL;
        }
        if(ptr != NULL){
            NixSharedPtr_free(ptr);
            ptr = NULL;
        }
    }
    return r;
}

void NixContext_retain(STNixContextRef ref){
    NixSharedPtr_retain(ref.ptr);
}

void NixContext_release(STNixContextRef* ref){
    if(ref != NULL && 0 == NixSharedPtr_release(ref->ptr)){
        STNixContextOpq* opq = (STNixContextOpq*)NixSharedPtr_getOpq(ref->ptr);
        //free
        if(opq != NULL){
            (ref->itf->mem.free)(opq);
            opq = NULL;
        }
        if(ref->itf != NULL){
            (ref->itf->mem.free)(ref->itf);
            ref->itf = NULL;
        }
        if(ref->ptr != NULL){
            NixSharedPtr_free(ref->ptr);
            ref->ptr = NULL;
        }
        *ref = (STNixContextRef)STNixContextRef_Zero;
    }
}

void NixContext_set(STNixContextRef* ref, STNixContextRef other){
    //retain first
    if(other.ptr != NULL){
        NixContext_retain(other);
    }
    //release after
    if(ref->ptr != NULL){
        NixContext_release(ref);
    }
    //set
    *ref = other;
}

void NixContext_null(STNixContextRef* ref){
    *ref = (STNixContextRef)STNixContextRef_Zero;
}

//context (memory)

void* NixContext_malloc(STNixContextRef ref, const NixUI32 newSz, const char* dbgHintStr){
    return (ref.itf != NULL && ref.itf->mem.malloc != NULL ? (*ref.itf->mem.malloc)(newSz, dbgHintStr) : NULL);
}

void* NixContext_mrealloc(STNixContextRef ref, void* ptr, const NixUI32 newSz){
    return (ref.itf != NULL && ref.itf->mem.realloc != NULL ? (*ref.itf->mem.realloc)(ptr, newSz) : NULL);
}

void NixContext_mfree(STNixContextRef ref, void* ptr){
    if(ref.itf != NULL && ref.itf->mem.free != NULL){
        (*ref.itf->mem.free)(ptr);
    }
}

//context (mutex)

STNixMutexRef NixContext_mutex_alloc(STNixContextRef ref){
    return (ref.itf != NULL && ref.itf->mutex.alloc != NULL ? (*ref.itf->mutex.alloc)(ref.itf) : (STNixMutexRef)STNixMutexRef_Zero);
}


//STNixContextItf (API)

STNixContextItf NixContextItf_getDefault(void){
    STNixContextItf itf;
    memset(&itf, 0, sizeof(itf));
    NixContextItf_fillMissingMembers(&itf);
    return itf;
}

//Links NULL methods to a DEFAULT implementation,
//this reduces the need to check for functions NULL pointers.
void NixContextItf_fillMissingMembers(STNixContextItf* itf){
    if(itf == NULL) return;
    NixMemoryItf_fillMissingMembers(&itf->mem);
    NixMutexItf_fillMissingMembers(&itf->mutex);
    //validate missing implementations
#   ifdef NIX_ASSERTS_ACTIVATED
    {
        void** ptr = (void**)itf;
        void** afterEnd = ptr + (sizeof(*itf) / sizeof(void*));
        while(ptr < afterEnd){
            NIX_ASSERT(*ptr != NULL) //function pointer should be defined
            ptr++;
        }
    }
#   endif
}

//STNixApiEngineItf

STNixApiEngineRef  NixApiEngineItf_nop_alloc(STNixContextRef ctx) { return (STNixApiEngineRef)STNixApiEngineRef_Zero; }
void            NixApiEngineItf_nop_free(STNixApiEngineRef ref) { }
void            NixApiEngineItf_nop_printCaps(STNixApiEngineRef ref) { }
NixBOOL         NixApiEngineItf_nop_ctxIsActive(STNixApiEngineRef ref) { return NIX_FALSE; }
NixBOOL         NixApiEngineItf_nop_ctxActivate(STNixApiEngineRef ref) { return NIX_FALSE; }
NixBOOL         NixApiEngineItf_nop_ctxDeactivate(STNixApiEngineRef ref) { return NIX_FALSE; }
void            NixApiEngineItf_nop_tick(STNixApiEngineRef ref) { }
//Factory
STNixApiSourceRef NixApiEngineItf_nop_sourceAlloc(STNixApiEngineRef ref) { return (STNixApiSourceRef)STNixApiSourceRef_Zero; }
STNixApiBufferRef NixApiEngineItf_nop_bufferAlloc(STNixApiEngineRef ref, const STNixAudioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes) { return (STNixApiBufferRef)STNixApiBufferRef_Zero; }
STNixApiRecorderRef NixApiEngineItf_nop_recorderAlloc(STNixApiEngineRef ref, const STNixAudioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer) { return (STNixApiRecorderRef)STNixApiRecorderRef_Zero; }

//Links NULL methods to a NOP implementation,
//this reduces the need to check for functions NULL pointers.
void NixApiEngineItf_fillMissingMembers(STNixApiEngineItf* itf){
    if(itf == NULL) return;
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiEngineItf, alloc);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiEngineItf, free);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiEngineItf, printCaps);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiEngineItf, ctxIsActive);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiEngineItf, ctxActivate);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiEngineItf, ctxActivate);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiEngineItf, ctxDeactivate);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiEngineItf, tick);
    //Factory
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiEngineItf, sourceAlloc);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiEngineItf, bufferAlloc);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiEngineItf, recorderAlloc);
    //validate missing implementations
#   ifdef NIX_ASSERTS_ACTIVATED
    {
        void** ptr = (void**)itf;
        void** afterEnd = ptr + (sizeof(*itf) / sizeof(void*));
        while(ptr < afterEnd){
            NIX_ASSERT(*ptr != NULL) //function pointer should be defined
            ptr++;
        }
    }
#   endif
}

//STNixApiBufferItf

STNixApiBufferRef  NixApiBufferItf_nop_alloc(STNixContextRef ctx, const STNixAudioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes) { return (STNixApiBufferRef)STNixApiBufferRef_Zero; }
void            NixApiBufferItf_nop_free(STNixApiBufferRef ref) { }
NixBOOL         NixApiBufferItf_nop_setData(STNixApiBufferRef ref, const STNixAudioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes) { return NIX_FALSE; }
NixBOOL         NixApiBufferItf_nop_fillWithZeroes(STNixApiBufferRef ref) { return NIX_FALSE; }

//Links NULL methods to a NOP implementation,
//this reduces the need to check for functions NULL pointers.
void NixApiBufferItf_fillMissingMembers(STNixApiBufferItf* itf){
    if(itf == NULL) return;
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiBufferItf, alloc);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiBufferItf, free);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiBufferItf, setData);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiBufferItf, fillWithZeroes);
    //validate missing implementations
#   ifdef NIX_ASSERTS_ACTIVATED
    {
        void** ptr = (void**)itf;
        void** afterEnd = ptr + (sizeof(*itf) / sizeof(void*));
        while(ptr < afterEnd){
            NIX_ASSERT(*ptr != NULL) //function pointer should be defined
            ptr++;
        }
    }
#   endif
}

//STNixApiSourceItf

STNixApiSourceRef  NixApiSourceItf_nop_alloc(STNixApiEngineRef eng) { return (STNixApiSourceRef)STNixApiSourceRef_Zero; }
void            NixApiSourceItf_nop_free(STNixApiSourceRef ref) { }
void            NixApiSourceItf_nop_setCallback(STNixApiSourceRef ref, void (*callback)(void* pEng, const NixUI32 sourceIndex, const NixUI32 ammBuffs), void* callbackEng, NixUI32 callbackSourceIndex) { }
NixBOOL         NixApiSourceItf_nop_setVolume(STNixApiSourceRef ref, const float vol)  { return NIX_FALSE; }
NixBOOL         NixApiSourceItf_nop_setRepeat(STNixApiSourceRef ref, const NixBOOL isRepeat)  { return NIX_FALSE; }
void            NixApiSourceItf_nop_play(STNixApiSourceRef ref) { }
void            NixApiSourceItf_nop_pause(STNixApiSourceRef ref) { }
void            NixApiSourceItf_nop_stop(STNixApiSourceRef ref) { }
NixBOOL         NixApiSourceItf_nop_isPlaying(STNixApiSourceRef ref) { return NIX_FALSE; }
NixBOOL         NixApiSourceItf_nop_isPaused(STNixApiSourceRef ref) { return NIX_FALSE; }
NixBOOL         NixApiSourceItf_nop_setBuffer(STNixApiSourceRef ref, STNixApiBufferRef buff) { return NIX_FALSE; }
NixBOOL         NixApiSourceItf_nop_queueBuffer(STNixApiSourceRef ref, STNixApiBufferRef buff) { return NIX_FALSE; }

//Links NULL methods to a NOP implementation,
//this reduces the need to check for functions NULL pointers.
void NixApiSourceItf_fillMissingMembers(STNixApiSourceItf* itf){
    if(itf == NULL) return;
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiSourceItf, alloc);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiSourceItf, free);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiSourceItf, setCallback);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiSourceItf, setVolume);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiSourceItf, setRepeat);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiSourceItf, play);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiSourceItf, pause);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiSourceItf, stop);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiSourceItf, isPlaying);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiSourceItf, isPaused);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiSourceItf, setBuffer);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiSourceItf, queueBuffer);
    //validate missing implementations
#   ifdef NIX_ASSERTS_ACTIVATED
    {
        void** ptr = (void**)itf;
        void** afterEnd = ptr + (sizeof(*itf) / sizeof(void*));
        while(ptr < afterEnd){
            NIX_ASSERT(*ptr != NULL) //function pointer should be defined
            ptr++;
        }
    }
#   endif
}

//STNixApiRecorderItf

STNixApiRecorderRef NixApiRecorderItf_nop_alloc(STNixApiEngineRef eng, const STNixAudioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer) { return (STNixApiRecorderRef)STNixApiRecorderRef_Zero; }
void            NixApiRecorderItf_nop_free(STNixApiRecorderRef ref) { }
NixBOOL         NixApiRecorderItf_nop_setCallback(STNixApiRecorderRef ref, NixApiRecorderCallbackFnc callback, void* callbackData) { return NIX_FALSE; }
NixBOOL         NixApiRecorderItf_nop_start(STNixApiRecorderRef ref) { return NIX_FALSE; }
NixBOOL         NixApiRecorderItf_nop_stop(STNixApiRecorderRef ref) { return NIX_FALSE; }


//Links NULL methods to a NOP implementation,
//this reduces the need to check for functions NULL pointers.
void NixApiRecorderItf_fillMissingMembers(STNixApiRecorderItf* itf){
    if(itf == NULL) return;
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiRecorderItf, alloc);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiRecorderItf, free);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiRecorderItf, setCallback);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiRecorderItf, start);
    NIX_ITF_SET_MISSING_METHOD_TO_NOP(itf, NixApiRecorderItf, stop);
    //validate missing implementations
#   ifdef NIX_ASSERTS_ACTIVATED
    {
        void** ptr = (void**)itf;
        void** afterEnd = ptr + (sizeof(*itf) / sizeof(void*));
        while(ptr < afterEnd){
            NIX_ASSERT(*ptr != NULL) //function pointer should be defined
            ptr++;
        }
    }
#   endif
}

//STNixApiItf

//Links NULL methods to a NOP implementation,
//this reduces the need to check for functions NULL pointers.
void NixApiItf_fillMissingMembers(STNixApiItf* itf){
    if(itf == NULL) return;
    NixApiEngineItf_fillMissingMembers(&itf->engine);
    NixApiBufferItf_fillMissingMembers(&itf->buffer);
    NixApiSourceItf_fillMissingMembers(&itf->source);
    NixApiRecorderItf_fillMissingMembers(&itf->recorder);
    //validate missing implementations
#   ifdef NIX_ASSERTS_ACTIVATED
    {
        void** ptr = (void**)itf;
        void** afterEnd = ptr + (sizeof(*itf) / sizeof(void*));
        while(ptr < afterEnd){
            NIX_ASSERT(*ptr != NULL) //function pointer should be defined
            ptr++;
        }
    }
#   endif
}

//------
//PCMBuffer
//------

void NixPCMBuffer_init(STNixContextRef ctx, STNixPCMBuffer* obj){
    memset(obj, 0, sizeof(*obj));
    NixContext_set(&obj->ctx, ctx);
}

void NixPCMBuffer_destroy(STNixPCMBuffer* obj){
    if(obj->ptr != NULL){
        NixContext_mfree(obj->ctx, obj->ptr);
        obj->ptr = NULL;
    }
    obj->use = obj->sz = 0;
    NixContext_release(&obj->ctx);
    NixContext_null(&obj->ctx);
}

NixBOOL NixPCMBuffer_setData(STNixPCMBuffer* obj, const STNixAudioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes){
    NixBOOL r = NIX_FALSE;
    if(audioDesc != NULL && audioDesc->blockAlign > 0){
        const NixUI32 reqBytes = (audioDataPCMBytes / audioDesc->blockAlign * audioDesc->blockAlign);
        //destroy current buffer (if necesary)
        if(!STNixAudioDesc_isEqual(&obj->desc, audioDesc) || obj->sz < reqBytes){
            if(obj->ptr != NULL){
                NixContext_mfree(obj->ctx, obj->ptr);
                obj->ptr = NULL;
            }
            obj->use = obj->sz = 0;
        }
        //set fmt
        obj->desc = *audioDesc;
        obj->use = 0;
        //copy data
        if(reqBytes <= 0){
            r = NIX_TRUE;
        } else {
            //allocate buffer (if necesary)
            if(obj->ptr == NULL){
                obj->ptr = (NixUI8*)NixContext_malloc(obj->ctx, reqBytes, "NixPCMBuffer_setData::ptr");
                if(obj->ptr != NULL){
                    obj->sz = reqBytes;
                }
            }
            //results
            if(obj->ptr != NULL){
                //copy data
                if(audioDataPCM != NULL){
                    memcpy(obj->ptr, audioDataPCM, reqBytes);
                }
                obj->use = reqBytes;
                r = NIX_TRUE;
            }
        }
    }
    return r;
}

NixBOOL NixPCMBuffer_fillWithZeroes(STNixPCMBuffer* obj){
    NixBOOL r = NIX_FALSE;
    if(obj->ptr != NULL){
        if(obj->use < obj->sz){
            memset(&((NixBYTE*)obj->ptr)[obj->use], 0, obj->sz - obj->use);
            obj->use = obj->sz;
        }
        r = NIX_TRUE;
    }
    return r;
}

//------
//Notif
//------

void NixSourceNotif_init(STNixSourceNotif* obj){
    memset(obj, 0, sizeof(*obj));
}

void NixSourceNotif_destroy(STNixSourceNotif* obj){
    //
}

//------
//NotifQueue
//------

void NixNotifQueue_init(STNixContextRef ctx, STNixNotifQueue* obj){
    memset(obj, 0, sizeof(*obj));
    NixContext_set(&obj->ctx, ctx);
    obj->arr = obj->arrEmbedded;
    obj->sz = (sizeof(obj->arrEmbedded) / sizeof(obj->arrEmbedded[0]));
}

void NixNotifQueue_destroy(STNixNotifQueue* obj){
    if(obj->arr != NULL){
        NixUI32 i; for(i = 0; i < obj->use; i++){
            STNixSourceNotif* b = &obj->arr[i];
            NixSourceNotif_destroy(b);
        }
        if(obj->arr != obj->arrEmbedded){
            NixContext_mfree(obj->ctx, obj->arr);
        }
        obj->arr = NULL;
    }
    obj->use = obj->sz = 0;
    NixContext_release(&obj->ctx);
    NixContext_null(&obj->ctx);
}

NixBOOL NixNotifQueue_push(STNixNotifQueue* obj, STNixSourceNotif* pair){
    NixBOOL r = NIX_FALSE;
    if(obj != NULL && pair != NULL){
        //resize array (if necesary)
        if(obj->use >= obj->sz){
            const NixUI32 szN = obj->use + 4;
            STNixSourceNotif* arrN = (STNixSourceNotif*)NixContext_malloc(obj->ctx, sizeof(STNixSourceNotif) * szN, "NixNotifQueue_push::arrN");
            if(arrN != NULL){
                if(obj->arr != NULL){
                    if(obj->use > 0){
                        memcpy(arrN, obj->arr, sizeof(arrN[0]) * obj->use);
                    }
                    if(obj->arr != obj->arrEmbedded){
                        NixContext_mfree(obj->ctx, obj->arr);
                    }
                    obj->arr = NULL;
                }
                obj->arr = arrN;
                obj->sz = szN;
            }
        }
        //add
        if(obj->use >= obj->sz){
            NIX_PRINTF_ERROR("NixNotifQueue_push failed (no allocated space).\n");
        } else {
            //become the owner of the pair
            obj->arr[obj->use++] = *pair;
            r = NIX_TRUE;
        }
    }
    return r;
}

//STNixPCMBuffer (API, common)

STNixApiBufferRef nixPCMBuffer_alloc(STNixContextRef ctx, const STNixAudioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes);
void            nixPCMBuffer_free(STNixApiBufferRef ref);
NixBOOL         nixPCMBuffer_setData(STNixApiBufferRef ref, const STNixAudioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes);
NixBOOL         nixPCMBuffer_fillWithZeroes(STNixApiBufferRef ref);

NixBOOL NixPCMBuffer_getApiItf(STNixApiBufferItf* dst){
    NixBOOL r = NIX_FALSE;
    if(dst != NULL){
        memset(dst, 0, sizeof(*dst));
        //
        dst->alloc      = nixPCMBuffer_alloc;
        dst->free       = nixPCMBuffer_free;
        dst->setData    = nixPCMBuffer_setData;
        dst->fillWithZeroes = nixPCMBuffer_fillWithZeroes;
        //
        NixApiBufferItf_fillMissingMembers(dst);
        //
        r = NIX_TRUE;
    }
    return r;
}


//------
//PCMBuffer API
//------

STNixApiBufferRef nixPCMBuffer_alloc(STNixContextRef ctx, const STNixAudioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes){
    STNixApiBufferRef r = STNixApiBufferRef_Zero;
    if(audioDesc != NULL && audioDesc->blockAlign > 0){
        STNixPCMBuffer* obj = (STNixPCMBuffer*)NixContext_malloc(ctx, sizeof(STNixPCMBuffer), "STNixPCMBuffer");
        STNixApiBufferItf* itf = NULL;
        if(obj != NULL){
            NixPCMBuffer_init(ctx, obj);
            if(!NixPCMBuffer_setData(obj, audioDesc, audioDataPCM, audioDataPCMBytes)){
                NIX_PRINTF_ERROR("nixPCMBuffer_alloc::NixPCMBuffer_setData failed.\n");
            } else {
                itf = (STNixApiBufferItf*)NixContext_malloc(ctx, sizeof(STNixApiBufferItf), "STNixApiBufferItf");
                if(itf == NULL){
                    NIX_PRINTF_ERROR("nixPCMBuffer_alloc::malloc(STNixApiBufferItf) failed.\n");
                } else if(!NixPCMBuffer_getApiItf(itf)){
                    NIX_PRINTF_ERROR("nixPCMBuffer_alloc::NixPCMBuffer_getApiItf failed.\n");
                } else if(NULL == (r.ptr = NixSharedPtr_alloc(ctx.itf, obj))){
                    NIX_PRINTF_ERROR("nixPCMBuffer_alloc::NixSharedPtr_alloc failed.\n");
                } else {
                    NIX_ASSERT(r.ptr != NULL)
                    r.itf = itf; itf = NULL; //consume
                    obj = NULL; //consume
                }
            }
        }
        //release (if not consumed)
        if(itf != NULL){
            NixContext_mfree(ctx, itf);
            itf = NULL;
        }
        if(obj != NULL){
            NixPCMBuffer_destroy(obj);
            NixContext_mfree(ctx, obj);
            obj = NULL;
        }
    }
    return r;
}

void nixPCMBuffer_free(STNixApiBufferRef pObj){
    if(pObj.ptr != NULL){
        STNixPCMBuffer* obj = (STNixPCMBuffer*)NixSharedPtr_getOpq(pObj.ptr);
        if(obj != NULL){
            STNixContextRef ctxCpy = obj->ctx;
            //
            {
                NixPCMBuffer_destroy(obj);
            }
            NixContext_mfree(ctxCpy, obj);
            if(pObj.itf != NULL){
                NixContext_mfree(ctxCpy, pObj.itf);
                pObj.itf = NULL;
            }
        }
        //
        NixSharedPtr_free(pObj.ptr);
    }
}
   
NixBOOL nixPCMBuffer_setData(STNixApiBufferRef pObj, const STNixAudioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes){
    NixBOOL r = NIX_FALSE;
    if(pObj.ptr != NULL && audioDesc != NULL && audioDesc->blockAlign > 0){
        STNixPCMBuffer* obj = (STNixPCMBuffer*)NixSharedPtr_getOpq(pObj.ptr);
        r = NixPCMBuffer_setData(obj, audioDesc, audioDataPCM, audioDataPCMBytes);
    }
    return r;
}

NixBOOL nixPCMBuffer_fillWithZeroes(STNixApiBufferRef pObj){
    NixBOOL r = NIX_FALSE;
    if(pObj.ptr != NULL){
        STNixPCMBuffer* obj = (STNixPCMBuffer*)NixSharedPtr_getOpq(pObj.ptr);
        r = NixPCMBuffer_fillWithZeroes(obj);
    }
    return r;
}

#define NIX_FMT_CONVERTER_FREQ_PRECISION    512 //fixed point-denominator
#define NIX_FMT_CONVERTER_CHANNELS_MAX      2

//PCMFormat converter

typedef struct STNix_FmtConverterChannel_ {
    void*       ptr;
    NixUI32     sampleAlign; //jumps to get the next sample
} STNix_FmtConverterChannel;

typedef struct STNix_FmtConverterSide_ {
    STNixAudioDesc             desc;
    STNix_FmtConverterChannel   channels[NIX_FMT_CONVERTER_CHANNELS_MAX];
} STNix_FmtConverterSide;

typedef struct STNix_FmtConverter_ {
    STNixContextRef         ctx;
    STNix_FmtConverterSide  src;
    STNix_FmtConverterSide  dst;
    //accum
    struct {
        NixUI32 fixed;
        NixUI32 count;
        //accumulated values
        union {
            NixFLOAT accumFloat[NIX_FMT_CONVERTER_CHANNELS_MAX];
            NixSI64  accumSI64[NIX_FMT_CONVERTER_CHANNELS_MAX];
            NixSI32  accumSI32[NIX_FMT_CONVERTER_CHANNELS_MAX];
        };
    } samplesAccum;
} STNix_FmtConverter;

void* nixFmtConverter_alloc(STNixContextRef ctx){
    STNix_FmtConverter* r = (STNix_FmtConverter*)NixContext_malloc(ctx, sizeof(STNix_FmtConverter), "STNix_FmtConverter");
    memset(r, 0, sizeof(STNix_FmtConverter));
    NixContext_set(&r->ctx, ctx);
    return r;
}

void nixFmtConverter_free(void* pObj){
    STNix_FmtConverter* obj = (STNix_FmtConverter*)pObj;
    if(obj != NULL){
        STNixContextRef ctxCpy = obj->ctx;
        {
            NixContext_null(&obj->ctx);
            NixContext_mfree(ctxCpy, obj);
        }
        NixContext_release(&ctxCpy); //release ctx after memory is freed
        //
        obj = NULL;
    }
}

NixBOOL nixFmtConverter_prepare(void* pObj, const STNixAudioDesc* srcDesc, const STNixAudioDesc* dstDesc){
    NixBOOL r = NIX_FALSE;
    STNix_FmtConverter* obj = (STNix_FmtConverter*)pObj;
    if(obj != NULL && srcDesc != NULL && dstDesc != NULL && srcDesc->channels > 0 && srcDesc->channels <= NIX_FMT_CONVERTER_CHANNELS_MAX && dstDesc->channels > 0 && dstDesc->channels <= NIX_FMT_CONVERTER_CHANNELS_MAX){
        if(
           //src
           (srcDesc->samplesFormat == ENNixSampleFmt_Float && srcDesc->bitsPerSample == 32)
           || (srcDesc->samplesFormat == ENNixSampleFmt_Int && (srcDesc->bitsPerSample == 8 || srcDesc->bitsPerSample == 16 || srcDesc->bitsPerSample == 32))
           //dst
           || (dstDesc->samplesFormat == ENNixSampleFmt_Float && dstDesc->bitsPerSample == 32)
           || (dstDesc->samplesFormat == ENNixSampleFmt_Int && (dstDesc->bitsPerSample == 8 || dstDesc->bitsPerSample == 16 || dstDesc->bitsPerSample == 32))
           )
        {
            memset(&obj->src, 0, sizeof(obj->src));
            memset(&obj->dst, 0, sizeof(obj->dst));
            obj->src.desc   = *srcDesc;
            obj->dst.desc   = *dstDesc;
            r = NIX_TRUE;
        }
    }
    return r;
}

NixBOOL nixFmtConverter_setPtrAtSrc(void* pObj, const NixUI32 iChannel, void* ptr, const NixUI32 sampleAlign){
    NixBOOL r = NIX_FALSE;
    STNix_FmtConverter* obj = (STNix_FmtConverter*)pObj;
    if(obj != NULL && ptr != NULL && sampleAlign > 0 && iChannel < obj->src.desc.channels && iChannel < (sizeof(obj->src.channels) / sizeof(obj->src.channels[0]))){
        STNix_FmtConverterChannel* ch = &obj->src.channels[iChannel];
        ch->ptr = ptr;
        ch->sampleAlign = sampleAlign;
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL nixFmtConverter_setPtrAtDst(void* pObj, const NixUI32 iChannel, void* ptr, const NixUI32 sampleAlign){
    NixBOOL r = NIX_FALSE;
    STNix_FmtConverter* obj = (STNix_FmtConverter*)pObj;
    if(obj != NULL && ptr != NULL && sampleAlign > 0 && iChannel < obj->dst.desc.channels && iChannel < (sizeof(obj->dst.channels) / sizeof(obj->dst.channels[0]))){
        STNix_FmtConverterChannel* ch = &obj->dst.channels[iChannel];
        ch->ptr = ptr;
        ch->sampleAlign = sampleAlign;
        r = NIX_TRUE;
    }
    return r;
}

NixBOOL nixFmtConverter_setPtrAtSrcInterlaced(void* pObj, const STNixAudioDesc* desc, void* ptr, const NixUI32 iFirstSample){
    NixBOOL r = NIX_FALSE;
    STNix_FmtConverter* obj = (STNix_FmtConverter*)pObj;
    if(obj != NULL && desc != NULL){
        r = NIX_TRUE;
        NixUI32 iCh, chCountDst = desc->channels;
        for(iCh = 0; iCh < chCountDst && iCh < NIX_FMT_CONVERTER_CHANNELS_MAX; ++iCh){
            const NixUI32 samplePos = (iFirstSample * desc->blockAlign);
            const NixUI32 sampleChannelPos = samplePos + ((desc->bitsPerSample / 8) * iCh);
            if(!nixFmtConverter_setPtrAtSrc(pObj, iCh, &(((NixBYTE*)ptr)[sampleChannelPos]), desc->blockAlign)){
                NIX_PRINTF_ERROR("nixFmtConverter_setPtrAtSrcInterlaced::nixFmtConverter_setPtrAtSrc(iCh:%d, blockAlign:%d, ptr:%s) failed.\n", iCh, desc->blockAlign, (ptr != NULL ? "not-null" : "NULL"));
                r = NIX_FALSE;
                break;
            }
        }
    }
    return r;
}

NixBOOL nixFmtConverter_setPtrAtDstInterlaced(void* pObj, const STNixAudioDesc* desc, void* ptr, const NixUI32 iFirstSample){
    NixBOOL r = NIX_FALSE;
    STNix_FmtConverter* obj = (STNix_FmtConverter*)pObj;
    if(obj != NULL && desc != NULL){
        r = NIX_TRUE;
        NixUI32 iCh, chCountDst = desc->channels;
        for(iCh = 0; iCh < chCountDst && iCh < NIX_FMT_CONVERTER_CHANNELS_MAX; ++iCh){
            const NixUI32 samplePos = (iFirstSample * desc->blockAlign);
            const NixUI32 sampleChannelPos = samplePos + ((desc->bitsPerSample / 8) * iCh);
            if(!nixFmtConverter_setPtrAtDst(pObj, iCh, &(((NixBYTE*)ptr)[sampleChannelPos]), desc->blockAlign)){
                r = NIX_FALSE;
                break;
            }
        }
    }
    return r;
}

NixBOOL nixFmtConverter_convertSameFreq_(STNix_FmtConverter* obj, const NixUI32 srcBlocks, NixUI32 dstBlocks, NixUI32* dstAmmBlocksRead, NixUI32* dstAmmBlocksWritten);
NixBOOL nixFmtConverter_convertIncFreq_(STNix_FmtConverter* obj, const NixUI32 srcBlocks, NixUI32 dstBlocks, NixUI32* dstAmmBlocksRead, NixUI32* dstAmmBlocksWritten);
NixBOOL nixFmtConverter_convertDecFreq_(STNix_FmtConverter* obj, const NixUI32 srcBlocks, NixUI32 dstBlocks, NixUI32* dstAmmBlocksRead, NixUI32* dstAmmBlocksWritten);

NixBOOL nixFmtConverter_convert(void* pObj, const NixUI32 srcBlocks, NixUI32 dstBlocks, NixUI32* dstAmmBlocksRead, NixUI32* dstAmmBlocksWritten){
    NixBOOL r = NIX_FALSE;
    STNix_FmtConverter* obj = (STNix_FmtConverter*)pObj;
    if(obj != NULL){
        if(obj->src.desc.samplerate == obj->dst.desc.samplerate){
            //same freq
            r = nixFmtConverter_convertSameFreq_(obj, srcBlocks, dstBlocks, dstAmmBlocksRead, dstAmmBlocksWritten);
        } else if(obj->src.desc.samplerate < obj->dst.desc.samplerate){
            //increasing freq
            r = nixFmtConverter_convertIncFreq_(obj, srcBlocks, dstBlocks, dstAmmBlocksRead, dstAmmBlocksWritten);
        } else {
            //decreasing freq
            r = nixFmtConverter_convertDecFreq_(obj, srcBlocks, dstBlocks, dstAmmBlocksRead, dstAmmBlocksWritten);
        }
    }
    return r;
}

#define FMT_CONVERTER_IS_FLOAT32(DESC)  ((DESC).samplesFormat == ENNixSampleFmt_Float && (DESC).bitsPerSample == 32)
#define FMT_CONVERTER_IS_SI32(DESC)     ((DESC).samplesFormat == ENNixSampleFmt_Int && (DESC).bitsPerSample == 32)
#define FMT_CONVERTER_IS_SI16(DESC)     ((DESC).samplesFormat == ENNixSampleFmt_Int && (DESC).bitsPerSample == 16)
#define FMT_CONVERTER_IS_UI8(DESC)      ((DESC).samplesFormat == ENNixSampleFmt_Int && (DESC).bitsPerSample == 8)

#define FMT_CONVERTER_SAME_FREQ_1_CH(LEFT_TYPE, RIGHT_TYPE, RIGHT_MATH_CAST, RIGHT_MATH_OP1, RIGHT_MATH_OP2) \
    STNix_FmtConverterChannel* srcCh0 = &obj->src.channels[0]; \
    STNix_FmtConverterChannel* dstCh0 = &obj->dst.channels[0]; \
    NixBYTE *src0 = (NixBYTE*)srcCh0->ptr; NixUI32 srcAlign0 = srcCh0->sampleAlign; \
    NixBYTE *dst0 = (NixBYTE*)dstCh0->ptr; NixUI32 dstAlign0 = dstCh0->sampleAlign; \
    const NixBYTE *dst0AfterEnd = dst0 + (dstAlign0 * dstBlocks); \
    for(i = 0; i < srcBlocks && dst0 < dst0AfterEnd; ++i){ \
        *(LEFT_TYPE*)dst0 = (LEFT_TYPE) (((RIGHT_MATH_CAST *(RIGHT_TYPE*)src0) RIGHT_MATH_OP1) RIGHT_MATH_OP2); \
        src0 += srcAlign0; \
        dst0 += dstAlign0; \
    } \
    if(dstAmmBlocksRead != NULL) *dstAmmBlocksRead = i; \
    if(dstAmmBlocksWritten != NULL) *dstAmmBlocksWritten = (NixUI32)(dst0 - (NixBYTE*)dstCh0->ptr) / dstAlign0; \
    r = NIX_TRUE;

#define FMT_CONVERTER_SAME_FREQ_2_CH(LEFT_TYPE, RIGHT_TYPE, RIGHT_MATH_CAST, RIGHT_MATH_OP1, RIGHT_MATH_OP2) \
    STNix_FmtConverterChannel* srcCh0 = &obj->src.channels[0]; \
    STNix_FmtConverterChannel* dstCh0 = &obj->dst.channels[0]; \
    STNix_FmtConverterChannel* srcCh1 = &obj->src.channels[1]; \
    STNix_FmtConverterChannel* dstCh1 = &obj->dst.channels[1]; \
    NixBYTE *src0 = (NixBYTE*)srcCh0->ptr; NixUI32 srcAlign0 = srcCh0->sampleAlign; \
    NixBYTE *src1 = (NixBYTE*)srcCh1->ptr; NixUI32 srcAlign1 = srcCh1->sampleAlign; \
    NixBYTE *dst0 = (NixBYTE*)dstCh0->ptr; NixUI32 dstAlign0 = dstCh0->sampleAlign; \
    NixBYTE *dst1 = (NixBYTE*)dstCh1->ptr; NixUI32 dstAlign1 = dstCh1->sampleAlign; \
    const NixBYTE *dst0AfterEnd = dst0 + (dstAlign0 * dstBlocks); \
    for(i = 0; i < srcBlocks && dst0 < dst0AfterEnd; ++i){ \
        *(LEFT_TYPE*)dst0 = (LEFT_TYPE) (((RIGHT_MATH_CAST *(RIGHT_TYPE*)src0) RIGHT_MATH_OP1) RIGHT_MATH_OP2); \
        *(LEFT_TYPE*)dst1 = (LEFT_TYPE) (((RIGHT_MATH_CAST *(RIGHT_TYPE*)src1) RIGHT_MATH_OP1) RIGHT_MATH_OP2); \
        src0 += srcAlign0; \
        src1 += srcAlign1; \
        dst0 += dstAlign0; \
        dst1 += dstAlign1; \
    } \
    if(dstAmmBlocksRead != NULL) *dstAmmBlocksRead = i; \
    if(dstAmmBlocksWritten != NULL) *dstAmmBlocksWritten = (NixUI32)(dst0 - (NixBYTE*)dstCh0->ptr) / dstAlign0; \
    r = NIX_TRUE;

#define FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(LEFT_TYPE, RIGHT_TYPE, RIGHT_MATH_CAST, RIGHT_MATH_OP1, RIGHT_MATH_OP2) \
    STNix_FmtConverterChannel* srcCh0 = &obj->src.channels[0]; \
    STNix_FmtConverterChannel* dstCh0 = &obj->dst.channels[0]; \
    STNix_FmtConverterChannel* dstCh1 = &obj->dst.channels[1]; \
    NixBYTE *src0 = (NixBYTE*)srcCh0->ptr; NixUI32 srcAlign0 = srcCh0->sampleAlign; \
    NixBYTE *dst0 = (NixBYTE*)dstCh0->ptr; NixUI32 dstAlign0 = dstCh0->sampleAlign; \
    NixBYTE *dst1 = (NixBYTE*)dstCh1->ptr; NixUI32 dstAlign1 = dstCh1->sampleAlign; \
    const NixBYTE *dst0AfterEnd = dst0 + (dstAlign0 * dstBlocks); \
    for(i = 0; i < srcBlocks && dst0 < dst0AfterEnd; ++i){ \
        *(LEFT_TYPE*)dst0 = *(LEFT_TYPE*)dst1 = (LEFT_TYPE) (((RIGHT_MATH_CAST *(RIGHT_TYPE*)src0) RIGHT_MATH_OP1) RIGHT_MATH_OP2); \
        src0 += srcAlign0; \
        dst0 += dstAlign0; \
        dst1 += dstAlign1; \
    } \
    if(dstAmmBlocksRead != NULL) *dstAmmBlocksRead = i; \
    if(dstAmmBlocksWritten != NULL) *dstAmmBlocksWritten = (NixUI32)(dst0 - (NixBYTE*)dstCh0->ptr) / dstAlign0; \
    r = NIX_TRUE;

#define FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(LEFT_TYPE, RIGHT_TYPE, RIGHT_MATH_CAST, RIGHT_MATH_OP1, RIGHT_MATH_OP2, DIV_2_WITH_SUFIX) \
    STNix_FmtConverterChannel* srcCh0 = &obj->src.channels[0]; \
    STNix_FmtConverterChannel* srcCh1 = &obj->src.channels[1]; \
    STNix_FmtConverterChannel* dstCh0 = &obj->dst.channels[0]; \
    NixBYTE *src0 = (NixBYTE*)srcCh0->ptr; NixUI32 srcAlign0 = srcCh0->sampleAlign; \
    NixBYTE *src1 = (NixBYTE*)srcCh1->ptr; NixUI32 srcAlign1 = srcCh1->sampleAlign; \
    NixBYTE *dst0 = (NixBYTE*)dstCh0->ptr; NixUI32 dstAlign0 = dstCh0->sampleAlign; \
    const NixBYTE *dst0AfterEnd = dst0 + (dstAlign0 * dstBlocks); \
    for(i = 0; i < srcBlocks && dst0 < dst0AfterEnd; ++i){ \
        *(LEFT_TYPE*)dst0 = (LEFT_TYPE) (((((RIGHT_MATH_CAST *(RIGHT_TYPE*)src0) RIGHT_MATH_OP1) RIGHT_MATH_OP2) + (((RIGHT_MATH_CAST *(RIGHT_TYPE*)src1) RIGHT_MATH_OP1) RIGHT_MATH_OP2)) / DIV_2_WITH_SUFIX); \
        src0 += srcAlign0; \
        src1 += srcAlign1; \
        dst0 += dstAlign0; \
    } \
    if(dstAmmBlocksRead != NULL) *dstAmmBlocksRead = i; \
    if(dstAmmBlocksWritten != NULL) *dstAmmBlocksWritten = (NixUI32)(dst0 - (NixBYTE*)dstCh0->ptr) / dstAlign0; \
    r = NIX_TRUE;

NixBOOL nixFmtConverter_convertSameFreq_(STNix_FmtConverter* obj, const NixUI32 srcBlocks, NixUI32 dstBlocks, NixUI32* dstAmmBlocksRead, NixUI32* dstAmmBlocksWritten){
    NixBOOL r = NIX_FALSE;
    NixUI32 i = 0;
    if(obj->src.desc.channels == 1 && obj->dst.desc.channels == 1){
        //same 1-channel
        if(FMT_CONVERTER_IS_FLOAT32(obj->dst.desc)){            //to FLOAT32
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_1_CH(NixFLOAT, NixFLOAT, , ,); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_CH(NixFLOAT, NixSI32, (NixDOUBLE), , / 2147483648.); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_CH(NixFLOAT, NixSI16, (NixFLOAT), , / 32768.f); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_1_CH(NixFLOAT, NixUI8, (NixFLOAT), -128.f , / 128.f); }
        } else if(FMT_CONVERTER_IS_SI32(obj->dst.desc)){        //to SI32
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_1_CH(NixSI32, NixFLOAT, (NixDOUBLE), , * 2147483647.); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_CH(NixSI32, NixSI32, , , ); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_CH(NixSI32, NixSI16, (NixSI32), + 1, * (0x7FFFFFFF / 0x7FFF)); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_1_CH(NixSI32, NixUI8, (NixSI64), - 127, * (0x7FFFFFFF / 0x80)); }
        } else if(FMT_CONVERTER_IS_SI16(obj->dst.desc)){        //to SI16
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_1_CH(NixSI16, NixFLOAT, , , * 32767.f); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_CH(NixSI16, NixSI32, , , / (0x7FFFFFFF / 0x7FFF)); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_CH(NixSI16, NixSI16, , , ); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_1_CH(NixSI16, NixUI8, (NixSI32), - 127, * (0x7FFF / 0x80)); }
        } else if(FMT_CONVERTER_IS_UI8(obj->dst.desc)){         //to UI8
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_1_CH(NixUI8, NixFLOAT, , + 1.f, * 127.f); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_CH(NixUI8, NixSI32, , / (0x7FFFFFFF / 0x7F), + 127); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_CH(NixUI8, NixSI16, , / (0x7FFF / 0x7F), + 127); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_1_CH(NixUI8, NixUI8, , , ); }
        }
    } else if(obj->src.desc.channels == 2 && obj->dst.desc.channels == 2){
        //same 2-channels
        if(FMT_CONVERTER_IS_FLOAT32(obj->dst.desc)){            //to FLOAT32
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_2_CH(NixFLOAT, NixFLOAT, , ,); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_CH(NixFLOAT, NixSI32, (NixDOUBLE), , / 2147483648.); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_CH(NixFLOAT, NixSI16, (NixFLOAT), , / 32768.f); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_2_CH(NixFLOAT, NixUI8, (NixFLOAT), -128.f , / 128.f); }
        } else if(FMT_CONVERTER_IS_SI32(obj->dst.desc)){        //to SI32
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_2_CH(NixSI32, NixFLOAT, (NixDOUBLE), , * 2147483647.); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_CH(NixSI32, NixSI32, , , ); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_CH(NixSI32, NixSI16, (NixSI32), + 1, * (0x7FFFFFFF / 0x7FFF)); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_2_CH(NixSI32, NixUI8, (NixSI64), - 127, * (0x7FFFFFFF / 0x80)); }
        } else if(FMT_CONVERTER_IS_SI16(obj->dst.desc)){        //to SI16
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_2_CH(NixSI16, NixFLOAT, , , * 32767.f); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_CH(NixSI16, NixSI32, , , / (0x7FFFFFFF / 0x7FFF)); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_CH(NixSI16, NixSI16, , , ); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_2_CH(NixSI16, NixUI8, (NixSI32), - 127, * (0x7FFF / 0x80)); }
        } else if(FMT_CONVERTER_IS_UI8(obj->dst.desc)){         //to UI8
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_2_CH(NixUI8, NixFLOAT, , + 1.f, * 127.f); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_CH(NixUI8, NixSI32, , / (0x7FFFFFFF / 0x7F), + 127); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_CH(NixUI8, NixSI16, , / (0x7FFF / 0x7F), + 127); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_2_CH(NixUI8, NixUI8, , , ); }
        }
    } else if(obj->src.desc.channels == 1 && obj->dst.desc.channels == 2){
        //duplicate src channels
        if(FMT_CONVERTER_IS_FLOAT32(obj->dst.desc)){            //to FLOAT32
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixFLOAT, NixFLOAT, , ,); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixFLOAT, NixSI32, (NixDOUBLE), , / 2147483648.); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixFLOAT, NixSI16, (NixFLOAT), , / 32768.f); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixFLOAT, NixUI8, (NixFLOAT), -128.f , / 128.f); }
        } else if(FMT_CONVERTER_IS_SI32(obj->dst.desc)){        //to SI32
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixSI32, NixFLOAT, (NixDOUBLE), , * 2147483647.); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixSI32, NixSI32, , , ); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixSI32, NixSI16, (NixSI32), + 1, * (0x7FFFFFFF / 0x7FFF)); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixSI32, NixUI8, (NixSI64), - 127, * (0x7FFFFFFF / 0x80)); }
        } else if(FMT_CONVERTER_IS_SI16(obj->dst.desc)){        //to SI16
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixSI16, NixFLOAT, , , * 32767.f); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixSI16, NixSI32, , , / (0x7FFFFFFF / 0x7FFF)); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixSI16, NixSI16, , , ); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixSI16, NixUI8, (NixSI32), - 127, * (0x7FFF / 0x80)); }
        } else if(FMT_CONVERTER_IS_UI8(obj->dst.desc)){         //to UI8
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixUI8, NixFLOAT, , + 1.f, * 127.f); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixUI8, NixSI32, , / (0x7FFFFFFF / 0x7F), + 127); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixUI8, NixSI16, , / (0x7FFF / 0x7F), + 127); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_1_TO_2_CH(NixUI8, NixUI8, , , ); }
        }
    } else if(obj->src.desc.channels == 2 && obj->dst.desc.channels == 1){
        //merge src channels
        if(FMT_CONVERTER_IS_FLOAT32(obj->dst.desc)){            //to FLOAT32
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixFLOAT, NixFLOAT, , , , 2.f); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixFLOAT, NixSI32, (NixDOUBLE), , / 2147483648., 2); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixFLOAT, NixSI16, (NixFLOAT), , / 32768.f, 2); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixFLOAT, NixUI8, (NixFLOAT), -128.f , / 128.f, 2); }
        } else if(FMT_CONVERTER_IS_SI32(obj->dst.desc)){        //to SI32
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixSI32, NixFLOAT, (NixDOUBLE), , * 2147483647., 2.); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixSI32, NixSI32, , , , 2); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixSI32, NixSI16, (NixSI32), + 1, * (0x7FFFFFFF / 0x7FFF), 2); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixSI32, NixUI8, (NixSI64), - 127, * (0x7FFFFFFF / 0x80), 2); }
        } else if(FMT_CONVERTER_IS_SI16(obj->dst.desc)){        //to SI16
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixSI16, NixFLOAT, , , * 32767.f, 2.f); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixSI16, NixSI32, , , / (0x7FFFFFFF / 0x7FFF), 2); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixSI16, NixSI16, (NixSI32), , , 2); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixSI16, NixUI8, (NixSI32), - 127, * (0x7FFF / 0x80), 2); }
        } else if(FMT_CONVERTER_IS_UI8(obj->dst.desc)){         //to UI8
            if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixUI8, NixFLOAT, , + 1.f, * 127.f, 2.f); }
            else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixUI8, NixSI32, , / (0x7FFFFFFF / 0x7F), + 127, 2); }
            else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixUI8, NixSI16, , / (0x7FFF / 0x7F), + 127, 2); }
            else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_SAME_FREQ_2_TO_1_CH(NixUI8, NixUI8, , , , 2); }
        }
    }
    return r;
}

#define FMT_CONVERTER_INC_FREQ_1_CH(LEFT_TYPE, RIGHT_TYPE, RIGHT_MATH_CAST, RIGHT_MATH_OP1, RIGHT_MATH_OP2) \
    STNix_FmtConverterChannel* srcCh0 = &obj->src.channels[0]; \
    STNix_FmtConverterChannel* dstCh0 = &obj->dst.channels[0]; \
    NixBYTE *src0 = (NixBYTE*)srcCh0->ptr; NixUI32 srcAlign0 = srcCh0->sampleAlign; \
    NixBYTE *dst0 = (NixBYTE*)dstCh0->ptr; NixUI32 dstAlign0 = dstCh0->sampleAlign; \
    const NixBYTE *dst0AfterEnd = dst0 + (dstAlign0 * dstBlocks); \
    for(i = 0; i < srcBlocks && dst0 < dst0AfterEnd; ++i){ \
        *(LEFT_TYPE*)dst0 = (LEFT_TYPE) (((RIGHT_MATH_CAST *(RIGHT_TYPE*)src0) RIGHT_MATH_OP1) RIGHT_MATH_OP2); \
        src0 += srcAlign0; \
        dst0 += dstAlign0; \
        obj->samplesAccum.fixed += repeatPerOrgSample; \
        while(obj->samplesAccum.fixed >= NIX_FMT_CONVERTER_FREQ_PRECISION && dst0 < dst0AfterEnd){ \
            obj->samplesAccum.fixed -= NIX_FMT_CONVERTER_FREQ_PRECISION; \
            *(LEFT_TYPE*)dst0 = *(LEFT_TYPE*)(dst0 - dstAlign0); \
            dst0 += dstAlign0; \
        } \
    } \
    if(dstAmmBlocksRead != NULL) *dstAmmBlocksRead = i; \
    if(dstAmmBlocksWritten != NULL) *dstAmmBlocksWritten = (NixUI32)(dst0 - (NixBYTE*)dstCh0->ptr) / dstAlign0; \
    r = NIX_TRUE;

#define FMT_CONVERTER_INC_FREQ_2_CH(LEFT_TYPE, RIGHT_TYPE, RIGHT_MATH_CAST, RIGHT_MATH_OP1, RIGHT_MATH_OP2) \
    STNix_FmtConverterChannel* srcCh0 = &obj->src.channels[0]; \
    STNix_FmtConverterChannel* dstCh0 = &obj->dst.channels[0]; \
    STNix_FmtConverterChannel* srcCh1 = &obj->src.channels[1]; \
    STNix_FmtConverterChannel* dstCh1 = &obj->dst.channels[1]; \
    NixBYTE *src0 = (NixBYTE*)srcCh0->ptr; NixUI32 srcAlign0 = srcCh0->sampleAlign; \
    NixBYTE *src1 = (NixBYTE*)srcCh1->ptr; NixUI32 srcAlign1 = srcCh1->sampleAlign; \
    NixBYTE *dst0 = (NixBYTE*)dstCh0->ptr; NixUI32 dstAlign0 = dstCh0->sampleAlign; \
    NixBYTE *dst1 = (NixBYTE*)dstCh1->ptr; NixUI32 dstAlign1 = dstCh1->sampleAlign; \
    const NixBYTE *dst0AfterEnd = dst0 + (dstAlign0 * dstBlocks); \
    for(i = 0; i < srcBlocks && dst0 < dst0AfterEnd; ++i){ \
        *(LEFT_TYPE*)dst0 = (LEFT_TYPE) (((RIGHT_MATH_CAST *(RIGHT_TYPE*)src0) RIGHT_MATH_OP1) RIGHT_MATH_OP2); \
        *(LEFT_TYPE*)dst1 = (LEFT_TYPE) (((RIGHT_MATH_CAST *(RIGHT_TYPE*)src1) RIGHT_MATH_OP1) RIGHT_MATH_OP2); \
        src0 += srcAlign0; \
        src1 += srcAlign1; \
        dst0 += dstAlign0; \
        dst1 += dstAlign1; \
        obj->samplesAccum.fixed += repeatPerOrgSample; \
        while(obj->samplesAccum.fixed >= NIX_FMT_CONVERTER_FREQ_PRECISION && dst0 < dst0AfterEnd){ \
            obj->samplesAccum.fixed -= NIX_FMT_CONVERTER_FREQ_PRECISION; \
            *(LEFT_TYPE*)dst0 = *(LEFT_TYPE*)(dst0 - dstAlign0); \
            *(LEFT_TYPE*)dst1 = *(LEFT_TYPE*)(dst1 - dstAlign1); \
            dst0 += dstAlign0; \
            dst1 += dstAlign1; \
        } \
    } \
    if(dstAmmBlocksRead != NULL) *dstAmmBlocksRead = i; \
    if(dstAmmBlocksWritten != NULL) *dstAmmBlocksWritten = (NixUI32)(dst0 - (NixBYTE*)dstCh0->ptr) / dstAlign0; \
    r = NIX_TRUE;

#define FMT_CONVERTER_INC_FREQ_1_TO_2_CH(LEFT_TYPE, RIGHT_TYPE, RIGHT_MATH_CAST, RIGHT_MATH_OP1, RIGHT_MATH_OP2) \
    STNix_FmtConverterChannel* srcCh0 = &obj->src.channels[0]; \
    STNix_FmtConverterChannel* dstCh0 = &obj->dst.channels[0]; \
    STNix_FmtConverterChannel* dstCh1 = &obj->dst.channels[1]; \
    NixBYTE *src0 = (NixBYTE*)srcCh0->ptr; NixUI32 srcAlign0 = srcCh0->sampleAlign; \
    NixBYTE *dst0 = (NixBYTE*)dstCh0->ptr; NixUI32 dstAlign0 = dstCh0->sampleAlign; \
    NixBYTE *dst1 = (NixBYTE*)dstCh1->ptr; NixUI32 dstAlign1 = dstCh1->sampleAlign; \
    const NixBYTE *dst0AfterEnd = dst0 + (dstAlign0 * dstBlocks); \
    for(i = 0; i < srcBlocks && dst0 < dst0AfterEnd; ++i){ \
        *(LEFT_TYPE*)dst0 = *(LEFT_TYPE*)dst1 = (LEFT_TYPE) (((RIGHT_MATH_CAST *(RIGHT_TYPE*)src0) RIGHT_MATH_OP1) RIGHT_MATH_OP2); \
        src0 += srcAlign0; \
        dst0 += dstAlign0; \
        dst1 += dstAlign1; \
        obj->samplesAccum.fixed += repeatPerOrgSample; \
        while(obj->samplesAccum.fixed >= NIX_FMT_CONVERTER_FREQ_PRECISION && dst0 < dst0AfterEnd){ \
            obj->samplesAccum.fixed -= NIX_FMT_CONVERTER_FREQ_PRECISION; \
            *(LEFT_TYPE*)dst0 = *(LEFT_TYPE*)dst1 = *(LEFT_TYPE*)(dst0 - dstAlign0); \
            dst0 += dstAlign0; \
            dst1 += dstAlign1; \
        } \
    } \
    if(dstAmmBlocksRead != NULL) *dstAmmBlocksRead = i; \
    if(dstAmmBlocksWritten != NULL) *dstAmmBlocksWritten = (NixUI32)(dst0 - (NixBYTE*)dstCh0->ptr) / dstAlign0; \
    r = NIX_TRUE;

#define FMT_CONVERTER_INC_FREQ_2_TO_1_CH(LEFT_TYPE, RIGHT_TYPE, RIGHT_MATH_CAST, RIGHT_MATH_OP1, RIGHT_MATH_OP2, DIV_2_WITH_SUFIX) \
    STNix_FmtConverterChannel* srcCh0 = &obj->src.channels[0]; \
    STNix_FmtConverterChannel* srcCh1 = &obj->src.channels[1]; \
    STNix_FmtConverterChannel* dstCh0 = &obj->dst.channels[0]; \
    NixBYTE *src0 = (NixBYTE*)srcCh0->ptr; NixUI32 srcAlign0 = srcCh0->sampleAlign; \
    NixBYTE *src1 = (NixBYTE*)srcCh1->ptr; NixUI32 srcAlign1 = srcCh1->sampleAlign; \
    NixBYTE *dst0 = (NixBYTE*)dstCh0->ptr; NixUI32 dstAlign0 = dstCh0->sampleAlign; \
    const NixBYTE *dst0AfterEnd = dst0 + (dstAlign0 * dstBlocks); \
    for(i = 0; i < srcBlocks && dst0 < dst0AfterEnd; ++i){ \
        *(LEFT_TYPE*)dst0 = (LEFT_TYPE) (((((RIGHT_MATH_CAST *(RIGHT_TYPE*)src0) RIGHT_MATH_OP1) RIGHT_MATH_OP2) + (((RIGHT_MATH_CAST *(RIGHT_TYPE*)src1) RIGHT_MATH_OP1) RIGHT_MATH_OP2)) / DIV_2_WITH_SUFIX); \
        src0 += srcAlign0; \
        src1 += srcAlign1; \
        dst0 += dstAlign0; \
        obj->samplesAccum.fixed += repeatPerOrgSample; \
        while(obj->samplesAccum.fixed >= NIX_FMT_CONVERTER_FREQ_PRECISION && dst0 < dst0AfterEnd){ \
            obj->samplesAccum.fixed -= NIX_FMT_CONVERTER_FREQ_PRECISION; \
            *(LEFT_TYPE*)dst0 = *(LEFT_TYPE*)(dst0 - dstAlign0); \
            dst0 += dstAlign0; \
        } \
    } \
    if(dstAmmBlocksRead != NULL) *dstAmmBlocksRead = i; \
    if(dstAmmBlocksWritten != NULL) *dstAmmBlocksWritten = (NixUI32)(dst0 - (NixBYTE*)dstCh0->ptr) / dstAlign0; \
    r = NIX_TRUE;

NixBOOL nixFmtConverter_convertIncFreq_(STNix_FmtConverter* obj, const NixUI32 srcBlocks, NixUI32 dstBlocks, NixUI32* dstAmmBlocksRead, NixUI32* dstAmmBlocksWritten){
    NixBOOL r = NIX_FALSE;
    //increasing freq
    const NixUI32 delta = (obj->dst.desc.samplerate - obj->src.desc.samplerate);
    const NixUI32 repeatPerOrgSample = delta * NIX_FMT_CONVERTER_FREQ_PRECISION / obj->src.desc.samplerate;
    if(repeatPerOrgSample == 0){
        //freq difference is below 'NIX_FMT_CONVERTER_FREQ_PRECISION', just ignore
        r = nixFmtConverter_convertSameFreq_(obj, srcBlocks, dstBlocks, dstAmmBlocksRead, dstAmmBlocksWritten);
    } else {
        NixUI32 i = 0;
        if(obj->src.desc.channels == 1 && obj->dst.desc.channels == 1){
            //same 1-channel
            if(FMT_CONVERTER_IS_FLOAT32(obj->dst.desc)){            //to FLOAT32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_1_CH(NixFLOAT, NixFLOAT, , ,); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_CH(NixFLOAT, NixSI32, (NixDOUBLE), , / 2147483648.); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_CH(NixFLOAT, NixSI16, (NixFLOAT), , / 32768.f); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_1_CH(NixFLOAT, NixUI8, (NixFLOAT), -128.f , / 128.f); }
            } else if(FMT_CONVERTER_IS_SI32(obj->dst.desc)){        //to SI32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_1_CH(NixSI32, NixFLOAT, (NixDOUBLE), , * 2147483647.); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_CH(NixSI32, NixSI32, , , ); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_CH(NixSI32, NixSI16, (NixSI32), + 1, * (0x7FFFFFFF / 0x7FFF)); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_1_CH(NixSI32, NixUI8, (NixSI64), - 127, * (0x7FFFFFFF / 0x80)); }
            } else if(FMT_CONVERTER_IS_SI16(obj->dst.desc)){        //to SI16
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_1_CH(NixSI16, NixFLOAT, , , * 32767.f); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_CH(NixSI16, NixSI32, , , / (0x7FFFFFFF / 0x7FFF)); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_CH(NixSI16, NixSI16, , , ); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_1_CH(NixSI16, NixUI8, (NixSI32), - 127, * (0x7FFF / 0x80)); }
            } else if(FMT_CONVERTER_IS_UI8(obj->dst.desc)){         //to UI8
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_1_CH(NixUI8, NixFLOAT, , + 1.f, * 127.f); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_CH(NixUI8, NixSI32, , / (0x7FFFFFFF / 0x7F), + 127); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_CH(NixUI8, NixSI16, , / (0x7FFF / 0x7F), + 127); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_1_CH(NixUI8, NixUI8, , , ); }
            }
        } else if(obj->src.desc.channels == 2 && obj->dst.desc.channels == 2){
            //same 2-channels
            if(FMT_CONVERTER_IS_FLOAT32(obj->dst.desc)){            //to FLOAT32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_2_CH(NixFLOAT, NixFLOAT, , ,); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_CH(NixFLOAT, NixSI32, (NixDOUBLE), , / 2147483648.); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_CH(NixFLOAT, NixSI16, (NixFLOAT), , / 32768.f); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_2_CH(NixFLOAT, NixUI8, (NixFLOAT), -128.f , / 128.f); }
            } else if(FMT_CONVERTER_IS_SI32(obj->dst.desc)){        //to SI32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_2_CH(NixSI32, NixFLOAT, (NixDOUBLE), , * 2147483647.); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_CH(NixSI32, NixSI32, , , ); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_CH(NixSI32, NixSI16, (NixSI32), + 1, * (0x7FFFFFFF / 0x7FFF)); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_2_CH(NixSI32, NixUI8, (NixSI64), - 127, * (0x7FFFFFFF / 0x80)); }
            } else if(FMT_CONVERTER_IS_SI16(obj->dst.desc)){        //to SI16
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_2_CH(NixSI16, NixFLOAT, , , * 32767.f); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_CH(NixSI16, NixSI32, , , / (0x7FFFFFFF / 0x7FFF)); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_CH(NixSI16, NixSI16, , , ); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_2_CH(NixSI16, NixUI8, (NixSI32), - 127, * (0x7FFF / 0x80)); }
            } else if(FMT_CONVERTER_IS_UI8(obj->dst.desc)){         //to UI8
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_2_CH(NixUI8, NixFLOAT, , + 1.f, * 127.f); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_CH(NixUI8, NixSI32, , / (0x7FFFFFFF / 0x7F), + 127); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_CH(NixUI8, NixSI16, , / (0x7FFF / 0x7F), + 127); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_2_CH(NixUI8, NixUI8, , , ); }
            }
        } else if(obj->src.desc.channels == 1 && obj->dst.desc.channels == 2){
            //duplicate src channels
            if(FMT_CONVERTER_IS_FLOAT32(obj->dst.desc)){            //to FLOAT32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixFLOAT, NixFLOAT, , ,); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixFLOAT, NixSI32, (NixDOUBLE), , / 2147483648.); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixFLOAT, NixSI16, (NixFLOAT), , / 32768.f); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixFLOAT, NixUI8, (NixFLOAT), -128.f , / 128.f); }
            } else if(FMT_CONVERTER_IS_SI32(obj->dst.desc)){        //to SI32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixSI32, NixFLOAT, (NixDOUBLE), , * 2147483647.); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixSI32, NixSI32, , , ); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixSI32, NixSI16, (NixSI32), + 1, * (0x7FFFFFFF / 0x7FFF)); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixSI32, NixUI8, (NixSI64), - 127, * (0x7FFFFFFF / 0x80)); }
            } else if(FMT_CONVERTER_IS_SI16(obj->dst.desc)){        //to SI16
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixSI16, NixFLOAT, , , * 32767.f); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixSI16, NixSI32, , , / (0x7FFFFFFF / 0x7FFF)); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixSI16, NixSI16, , , ); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixSI16, NixUI8, (NixSI32), - 127, * (0x7FFF / 0x80)); }
            } else if(FMT_CONVERTER_IS_UI8(obj->dst.desc)){         //to UI8
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixUI8, NixFLOAT, , + 1.f, * 127.f); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixUI8, NixSI32, , / (0x7FFFFFFF / 0x7F), + 127); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixUI8, NixSI16, , / (0x7FFF / 0x7F), + 127); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_1_TO_2_CH(NixUI8, NixUI8, , , ); }
            }
        } else if(obj->src.desc.channels == 2 && obj->dst.desc.channels == 1){
            //merge src channels
            if(FMT_CONVERTER_IS_FLOAT32(obj->dst.desc)){            //to FLOAT32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixFLOAT, NixFLOAT, , , , 2.f); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixFLOAT, NixSI32, (NixDOUBLE), , / 2147483648., 2); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixFLOAT, NixSI16, (NixFLOAT), , / 32768.f, 2); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixFLOAT, NixUI8, (NixFLOAT), -128.f , / 128.f, 2); }
            } else if(FMT_CONVERTER_IS_SI32(obj->dst.desc)){        //to SI32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixSI32, NixFLOAT, (NixDOUBLE), , * 2147483647., 2.); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixSI32, NixSI32, , , , 2); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixSI32, NixSI16, (NixSI32), + 1, * (0x7FFFFFFF / 0x7FFF), 2); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixSI32, NixUI8, (NixSI64), - 127, * (0x7FFFFFFF / 0x80), 2); }
            } else if(FMT_CONVERTER_IS_SI16(obj->dst.desc)){        //to SI16
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixSI16, NixFLOAT, , , * 32767.f, 2.f); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixSI16, NixSI32, , , / (0x7FFFFFFF / 0x7FFF), 2); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixSI16, NixSI16, , , , 2); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixSI16, NixUI8, (NixSI32), - 127, * (0x7FFF / 0x80), 2); }
            } else if(FMT_CONVERTER_IS_UI8(obj->dst.desc)){         //to UI8
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixUI8, NixFLOAT, , + 1.f, * 127.f, 2.f); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixUI8, NixSI32, , / (0x7FFFFFFF / 0x7F), + 127, 2); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixUI8, NixSI16, , / (0x7FFF / 0x7F), + 127, 2); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_INC_FREQ_2_TO_1_CH(NixUI8, NixUI8, , , , 2); }
            }
        }
    }
    return r;
}

#define FMT_CONVERTER_DEC_FREQ_1_CH(LEFT_TYPE, RIGHT_TYPE, RIGHT_MATH_CAST, RIGHT_MATH_OP1, RIGHT_MATH_OP2, ACCUM_VAR_NAME) \
    STNix_FmtConverterChannel* srcCh0 = &obj->src.channels[0]; \
    STNix_FmtConverterChannel* dstCh0 = &obj->dst.channels[0]; \
    NixBYTE *src0 = (NixBYTE*)srcCh0->ptr; NixUI32 srcAlign0 = srcCh0->sampleAlign; \
    NixBYTE *dst0 = (NixBYTE*)dstCh0->ptr; NixUI32 dstAlign0 = dstCh0->sampleAlign; \
    const NixBYTE *dst0AfterEnd = dst0 + (dstAlign0 * dstBlocks); \
    for(i = 0; i < srcBlocks && dst0 < dst0AfterEnd; ++i){ \
        obj->samplesAccum.ACCUM_VAR_NAME[0] += (LEFT_TYPE) (((RIGHT_MATH_CAST *(RIGHT_TYPE*)src0) RIGHT_MATH_OP1) RIGHT_MATH_OP2); \
        obj->samplesAccum.count++; \
        obj->samplesAccum.fixed += accumPerOrgSample; \
        src0 += srcAlign0; \
        while(obj->samplesAccum.fixed >= NIX_FMT_CONVERTER_FREQ_PRECISION && dst0 < dst0AfterEnd){ \
            obj->samplesAccum.fixed -= NIX_FMT_CONVERTER_FREQ_PRECISION; \
            *(LEFT_TYPE*)dst0 = (LEFT_TYPE)(obj->samplesAccum.ACCUM_VAR_NAME[0] / obj->samplesAccum.count); \
            dst0 += dstAlign0; \
            if(obj->samplesAccum.fixed < NIX_FMT_CONVERTER_FREQ_PRECISION){ \
                obj->samplesAccum.ACCUM_VAR_NAME[0] = 0; \
                obj->samplesAccum.count = 0; \
            } \
        } \
    } \
    if(dstAmmBlocksRead != NULL) *dstAmmBlocksRead = i; \
    if(dstAmmBlocksWritten != NULL) *dstAmmBlocksWritten = (NixUI32)(dst0 - (NixBYTE*)dstCh0->ptr) / dstAlign0; \
    r = NIX_TRUE;

#define FMT_CONVERTER_DEC_FREQ_2_CH(LEFT_TYPE, RIGHT_TYPE, RIGHT_MATH_CAST, RIGHT_MATH_OP1, RIGHT_MATH_OP2, ACCUM_VAR_NAME) \
    STNix_FmtConverterChannel* srcCh0 = &obj->src.channels[0]; \
    STNix_FmtConverterChannel* dstCh0 = &obj->dst.channels[0]; \
    STNix_FmtConverterChannel* srcCh1 = &obj->src.channels[1]; \
    STNix_FmtConverterChannel* dstCh1 = &obj->dst.channels[1]; \
    NixBYTE *src0 = (NixBYTE*)srcCh0->ptr; NixUI32 srcAlign0 = srcCh0->sampleAlign; \
    NixBYTE *src1 = (NixBYTE*)srcCh1->ptr; NixUI32 srcAlign1 = srcCh1->sampleAlign; \
    NixBYTE *dst0 = (NixBYTE*)dstCh0->ptr; NixUI32 dstAlign0 = dstCh0->sampleAlign; \
    NixBYTE *dst1 = (NixBYTE*)dstCh1->ptr; NixUI32 dstAlign1 = dstCh1->sampleAlign; \
    const NixBYTE *dst0AfterEnd = dst0 + (dstAlign0 * dstBlocks); \
    for(i = 0; i < srcBlocks && dst0 < dst0AfterEnd; ++i){ \
        obj->samplesAccum.ACCUM_VAR_NAME[0] += (LEFT_TYPE) (((RIGHT_MATH_CAST *(RIGHT_TYPE*)src0) RIGHT_MATH_OP1) RIGHT_MATH_OP2); \
        obj->samplesAccum.ACCUM_VAR_NAME[1] += (LEFT_TYPE) (((RIGHT_MATH_CAST *(RIGHT_TYPE*)src1) RIGHT_MATH_OP1) RIGHT_MATH_OP2); \
        obj->samplesAccum.count++; \
        obj->samplesAccum.fixed += accumPerOrgSample; \
        src0 += srcAlign0; \
        src1 += srcAlign1; \
        while(obj->samplesAccum.fixed >= NIX_FMT_CONVERTER_FREQ_PRECISION && dst0 < dst0AfterEnd){ \
            obj->samplesAccum.fixed -= NIX_FMT_CONVERTER_FREQ_PRECISION; \
            *(LEFT_TYPE*)dst0 = (LEFT_TYPE)(obj->samplesAccum.ACCUM_VAR_NAME[0] / obj->samplesAccum.count); \
            *(LEFT_TYPE*)dst1 = (LEFT_TYPE)(obj->samplesAccum.ACCUM_VAR_NAME[1] / obj->samplesAccum.count); \
            dst0 += dstAlign0; \
            dst1 += dstAlign1; \
            if(obj->samplesAccum.fixed < NIX_FMT_CONVERTER_FREQ_PRECISION){ \
                obj->samplesAccum.ACCUM_VAR_NAME[0] = obj->samplesAccum.ACCUM_VAR_NAME[1] = 0; \
                obj->samplesAccum.count = 0; \
            } \
        } \
    } \
    if(dstAmmBlocksRead != NULL) *dstAmmBlocksRead = i; \
    if(dstAmmBlocksWritten != NULL) *dstAmmBlocksWritten = (NixUI32)(dst0 - (NixBYTE*)dstCh0->ptr) / dstAlign0; \
    r = NIX_TRUE;

#define FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(LEFT_TYPE, RIGHT_TYPE, RIGHT_MATH_CAST, RIGHT_MATH_OP1, RIGHT_MATH_OP2, ACCUM_VAR_NAME) \
    STNix_FmtConverterChannel* srcCh0 = &obj->src.channels[0]; \
    STNix_FmtConverterChannel* dstCh0 = &obj->dst.channels[0]; \
    STNix_FmtConverterChannel* dstCh1 = &obj->dst.channels[1]; \
    NixBYTE *src0 = (NixBYTE*)srcCh0->ptr; NixUI32 srcAlign0 = srcCh0->sampleAlign; \
    NixBYTE *dst0 = (NixBYTE*)dstCh0->ptr; NixUI32 dstAlign0 = dstCh0->sampleAlign; \
    NixBYTE *dst1 = (NixBYTE*)dstCh1->ptr; NixUI32 dstAlign1 = dstCh1->sampleAlign; \
    const NixBYTE *dst0AfterEnd = dst0 + (dstAlign0 * dstBlocks); \
    for(i = 0; i < srcBlocks && dst0 < dst0AfterEnd; ++i){ \
        obj->samplesAccum.ACCUM_VAR_NAME[0] += *(LEFT_TYPE*)dst1 = (LEFT_TYPE) (((RIGHT_MATH_CAST *(RIGHT_TYPE*)src0) RIGHT_MATH_OP1) RIGHT_MATH_OP2); \
        src0 += srcAlign0; \
        obj->samplesAccum.count++; \
        obj->samplesAccum.fixed += accumPerOrgSample; \
        while(obj->samplesAccum.fixed >= NIX_FMT_CONVERTER_FREQ_PRECISION && dst0 < dst0AfterEnd){ \
            obj->samplesAccum.fixed -= NIX_FMT_CONVERTER_FREQ_PRECISION; \
            *(LEFT_TYPE*)dst0 = *(LEFT_TYPE*)dst1 = (LEFT_TYPE)(obj->samplesAccum.ACCUM_VAR_NAME[0] / obj->samplesAccum.count); \
            dst0 += dstAlign0; \
            dst1 += dstAlign1; \
            if(obj->samplesAccum.fixed < NIX_FMT_CONVERTER_FREQ_PRECISION){ \
                obj->samplesAccum.ACCUM_VAR_NAME[0] = obj->samplesAccum.ACCUM_VAR_NAME[1] = 0; \
                obj->samplesAccum.count = 0; \
            } \
        } \
    } \
    if(dstAmmBlocksRead != NULL) *dstAmmBlocksRead = i; \
    if(dstAmmBlocksWritten != NULL) *dstAmmBlocksWritten = (NixUI32)(dst0 - (NixBYTE*)dstCh0->ptr) / dstAlign0; \
    r = NIX_TRUE;

#define FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(LEFT_TYPE, RIGHT_TYPE, RIGHT_MATH_CAST, RIGHT_MATH_OP1, RIGHT_MATH_OP2, DIV_2_WITH_SUFIX, ACCUM_VAR_NAME) \
    STNix_FmtConverterChannel* srcCh0 = &obj->src.channels[0]; \
    STNix_FmtConverterChannel* srcCh1 = &obj->src.channels[1]; \
    STNix_FmtConverterChannel* dstCh0 = &obj->dst.channels[0]; \
    NixBYTE *src0 = (NixBYTE*)srcCh0->ptr; NixUI32 srcAlign0 = srcCh0->sampleAlign; \
    NixBYTE *src1 = (NixBYTE*)srcCh1->ptr; NixUI32 srcAlign1 = srcCh1->sampleAlign; \
    NixBYTE *dst0 = (NixBYTE*)dstCh0->ptr; NixUI32 dstAlign0 = dstCh0->sampleAlign; \
    const NixBYTE *dst0AfterEnd = dst0 + (dstAlign0 * dstBlocks); \
    for(i = 0; i < srcBlocks && dst0 < dst0AfterEnd; ++i){ \
        obj->samplesAccum.ACCUM_VAR_NAME[0] += (LEFT_TYPE) (((((RIGHT_MATH_CAST *(RIGHT_TYPE*)src0) RIGHT_MATH_OP1) RIGHT_MATH_OP2) + (((RIGHT_MATH_CAST *(RIGHT_TYPE*)src1) RIGHT_MATH_OP1) RIGHT_MATH_OP2)) / DIV_2_WITH_SUFIX); \
        src0 += srcAlign0; \
        src1 += srcAlign1; \
        obj->samplesAccum.count++; \
        obj->samplesAccum.fixed += accumPerOrgSample; \
        while(obj->samplesAccum.fixed >= NIX_FMT_CONVERTER_FREQ_PRECISION && dst0 < dst0AfterEnd){ \
            obj->samplesAccum.fixed -= NIX_FMT_CONVERTER_FREQ_PRECISION; \
            *(LEFT_TYPE*)dst0 = (LEFT_TYPE)(obj->samplesAccum.ACCUM_VAR_NAME[0] / obj->samplesAccum.count); \
            dst0 += dstAlign0; \
        } \
        if(obj->samplesAccum.fixed < NIX_FMT_CONVERTER_FREQ_PRECISION){ \
            obj->samplesAccum.ACCUM_VAR_NAME[0] = obj->samplesAccum.ACCUM_VAR_NAME[1] = 0; \
            obj->samplesAccum.count = 0; \
        } \
    } \
    if(dstAmmBlocksRead != NULL) *dstAmmBlocksRead = i; \
    if(dstAmmBlocksWritten != NULL) *dstAmmBlocksWritten = (NixUI32)(dst0 - (NixBYTE*)dstCh0->ptr) / dstAlign0; \
    r = NIX_TRUE;


NixBOOL nixFmtConverter_convertDecFreq_(STNix_FmtConverter* obj, const NixUI32 srcBlocks, NixUI32 dstBlocks, NixUI32* dstAmmBlocksRead, NixUI32* dstAmmBlocksWritten){
    NixBOOL r = NIX_FALSE;
    //decreasing freq
    const NixUI32 accumPerOrgSample = obj->dst.desc.samplerate * NIX_FMT_CONVERTER_FREQ_PRECISION / obj->src.desc.samplerate;
    if(accumPerOrgSample <= 0){
        //freq difference is below 'NIX_FMT_CONVERTER_FREQ_PRECISION', just ignore
        r = nixFmtConverter_convertSameFreq_(obj, srcBlocks, dstBlocks, dstAmmBlocksRead, dstAmmBlocksWritten);
    } else {
        NixUI32 i = 0;
        if(obj->src.desc.channels == 1 && obj->dst.desc.channels == 1){
            //same 1-channel
            if(FMT_CONVERTER_IS_FLOAT32(obj->dst.desc)){            //to FLOAT32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_1_CH(NixFLOAT, NixFLOAT, , , , accumFloat); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_CH(NixFLOAT, NixSI32, (NixDOUBLE), , / 2147483648., accumFloat); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_CH(NixFLOAT, NixSI16, (NixFLOAT), , / 32768.f, accumFloat); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_1_CH(NixFLOAT, NixUI8, (NixFLOAT), -128.f , / 128.f, accumFloat); }
            } else if(FMT_CONVERTER_IS_SI32(obj->dst.desc)){        //to SI32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_1_CH(NixSI32, NixFLOAT, (NixDOUBLE), , * 2147483647., accumSI64); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_CH(NixSI32, NixSI32, , , , accumSI64); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_CH(NixSI32, NixSI16, (NixSI32), + 1, * (0x7FFFFFFF / 0x7FFF), accumSI64); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_1_CH(NixSI32, NixUI8, (NixSI64), - 127, * (0x7FFFFFFF / 0x80), accumSI64); }
            } else if(FMT_CONVERTER_IS_SI16(obj->dst.desc)){        //to SI16
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_1_CH(NixSI16, NixFLOAT, , , * 32767.f, accumSI32); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_CH(NixSI16, NixSI32, , , / (0x7FFFFFFF / 0x7FFF), accumSI32); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_CH(NixSI16, NixSI16, , , , accumSI32); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_1_CH(NixSI16, NixUI8, (NixSI32), - 127, * (0x7FFF / 0x80), accumSI32); }
            } else if(FMT_CONVERTER_IS_UI8(obj->dst.desc)){         //to UI8
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_1_CH(NixUI8, NixFLOAT, , + 1.f, * 127.f, accumSI32); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_CH(NixUI8, NixSI32, , / (0x7FFFFFFF / 0x7F), + 127, accumSI32); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_CH(NixUI8, NixSI16, , / (0x7FFF / 0x7F), + 127, accumSI32); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_1_CH(NixUI8, NixUI8, , , , accumSI32); }
            }
        } else if(obj->src.desc.channels == 2 && obj->dst.desc.channels == 2){
            //same 2-channels
            if(FMT_CONVERTER_IS_FLOAT32(obj->dst.desc)){            //to FLOAT32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_2_CH(NixFLOAT, NixFLOAT, , , , accumFloat); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_CH(NixFLOAT, NixSI32, (NixDOUBLE), , / 2147483648., accumFloat); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_CH(NixFLOAT, NixSI16, (NixFLOAT), , / 32768.f, accumFloat); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_2_CH(NixFLOAT, NixUI8, (NixFLOAT), -128.f , / 128.f, accumFloat); }
            } else if(FMT_CONVERTER_IS_SI32(obj->dst.desc)){        //to SI32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_2_CH(NixSI32, NixFLOAT, (NixDOUBLE), , * 2147483647., accumSI64); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_CH(NixSI32, NixSI32, , , , accumSI64); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_CH(NixSI32, NixSI16, (NixSI32), + 1, * (0x7FFFFFFF / 0x7FFF), accumSI64); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_2_CH(NixSI32, NixUI8, (NixSI64), - 127, * (0x7FFFFFFF / 0x80), accumSI64); }
            } else if(FMT_CONVERTER_IS_SI16(obj->dst.desc)){        //to SI16
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_2_CH(NixSI16, NixFLOAT, , , * 32767.f, accumSI32); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_CH(NixSI16, NixSI32, , , / (0x7FFFFFFF / 0x7FFF), accumSI32); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_CH(NixSI16, NixSI16, , , , accumSI32); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_2_CH(NixSI16, NixUI8, (NixSI32), - 127, * (0x7FFF / 0x80), accumSI32); }
            } else if(FMT_CONVERTER_IS_UI8(obj->dst.desc)){         //to UI8
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_2_CH(NixUI8, NixFLOAT, , + 1.f, * 127.f, accumSI32); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_CH(NixUI8, NixSI32, , / (0x7FFFFFFF / 0x7F), + 127, accumSI32); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_CH(NixUI8, NixSI16, , / (0x7FFF / 0x7F), + 127, accumSI32); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_2_CH(NixUI8, NixUI8, , , , accumSI32); }
            }
        } else if(obj->src.desc.channels == 1 && obj->dst.desc.channels == 2){
            //duplicate src channels
            if(FMT_CONVERTER_IS_FLOAT32(obj->dst.desc)){            //to FLOAT32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixFLOAT, NixFLOAT, , , , accumFloat); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixFLOAT, NixSI32, (NixDOUBLE), , / 2147483648., accumFloat); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixFLOAT, NixSI16, (NixFLOAT), , / 32768.f, accumFloat); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixFLOAT, NixUI8, (NixFLOAT), -128.f , / 128.f, accumFloat); }
            } else if(FMT_CONVERTER_IS_SI32(obj->dst.desc)){        //to SI32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixSI32, NixFLOAT, (NixDOUBLE), , * 2147483647., accumSI64); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixSI32, NixSI32, , , , accumSI64); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixSI32, NixSI16, (NixSI32), + 1, * (0x7FFFFFFF / 0x7FFF), accumSI64); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixSI32, NixUI8, (NixSI64), - 127, * (0x7FFFFFFF / 0x80), accumSI64); }
            } else if(FMT_CONVERTER_IS_SI16(obj->dst.desc)){        //to SI16
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixSI16, NixFLOAT, , , * 32767.f, accumSI32); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixSI16, NixSI32, , , / (0x7FFFFFFF / 0x7FFF), accumSI32); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixSI16, NixSI16, , , , accumSI32); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixSI16, NixUI8, (NixSI32), - 127, * (0x7FFF / 0x80), accumSI32); }
            } else if(FMT_CONVERTER_IS_UI8(obj->dst.desc)){         //to UI8
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixUI8, NixFLOAT, , + 1.f, * 127.f, accumSI32); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixUI8, NixSI32, , / (0x7FFFFFFF / 0x7F), + 127, accumSI32); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixUI8, NixSI16, , / (0x7FFF / 0x7F), + 127, accumSI32); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_1_TO_2_CH(NixUI8, NixUI8, , , , accumSI32); }
            }
        } else if(obj->src.desc.channels == 2 && obj->dst.desc.channels == 1){
            //merge src channels
            if(FMT_CONVERTER_IS_FLOAT32(obj->dst.desc)){            //to FLOAT32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixFLOAT, NixFLOAT, , , , 2.f, accumFloat); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixFLOAT, NixSI32, (NixDOUBLE), , / 2147483648., 2, accumFloat); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixFLOAT, NixSI16, (NixFLOAT), , / 32768.f, 2, accumFloat); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixFLOAT, NixUI8, (NixFLOAT), -128.f , / 128.f, 2, accumFloat); }
            } else if(FMT_CONVERTER_IS_SI32(obj->dst.desc)){        //to SI32
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixSI32, NixFLOAT, (NixDOUBLE), , * 2147483647., 2., accumSI64); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixSI32, NixSI32, , , , 2, accumSI64); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixSI32, NixSI16, (NixSI32), + 1, * (0x7FFFFFFF / 0x7FFF), 2, accumSI64); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixSI32, NixUI8, (NixSI64), - 127, * (0x7FFFFFFF / 0x80), 2, accumSI64); }
            } else if(FMT_CONVERTER_IS_SI16(obj->dst.desc)){        //to SI16
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixSI16, NixFLOAT, , , * 32767.f, 2.f, accumSI32); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixSI16, NixSI32, , , / (0x7FFFFFFF / 0x7FFF), 2, accumSI32); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixSI16, NixSI16, , , , 2, accumSI32); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixSI16, NixUI8, (NixSI32), - 127, * (0x7FFF / 0x80), 2, accumSI32); }
            } else if(FMT_CONVERTER_IS_UI8(obj->dst.desc)){         //to UI8
                if(FMT_CONVERTER_IS_FLOAT32(obj->src.desc)){        FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixUI8, NixFLOAT, , + 1.f, * 127.f, 2.f, accumSI32); }
                else if(FMT_CONVERTER_IS_SI32(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixUI8, NixSI32, , / (0x7FFFFFFF / 0x7F), + 127, 2, accumSI32); }
                else if(FMT_CONVERTER_IS_SI16(obj->src.desc)){      FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixUI8, NixSI16, , / (0x7FFF / 0x7F), + 127, 2, accumSI32); }
                else if(FMT_CONVERTER_IS_UI8(obj->src.desc)){       FMT_CONVERTER_DEC_FREQ_2_TO_1_CH(NixUI8, NixUI8, , , , 2, accumSI32); }
            }
        }
    }
    return r;
}

//

NixUI32 nixFmtConverter_maxChannels(void){
    return NIX_FMT_CONVERTER_CHANNELS_MAX;
}

NixUI32 nixFmtConverter_samplesForNewFrequency(const NixUI32 ammSampesOrg, const NixUI32 freqOrg, const NixUI32 freqNew){   //ammount of output samples from one frequeny to another, +1 for safety
    NixUI32 r = ammSampesOrg;
    if(freqOrg < freqNew){
        //increasing freq
        const NixUI32 delta = (freqNew - freqOrg);
        const NixUI32 repeatPerOrgSample = delta * NIX_FMT_CONVERTER_FREQ_PRECISION / freqOrg;
        r = ammSampesOrg + ( ammSampesOrg * repeatPerOrgSample / NIX_FMT_CONVERTER_FREQ_PRECISION ) + 1;
    } else if(freqOrg > freqNew){
        //decreasing freq
        const NixUI32 accumPerOrgSample = freqNew * NIX_FMT_CONVERTER_FREQ_PRECISION / freqOrg;
        if(accumPerOrgSample > 0){
            r = ( ammSampesOrg * accumPerOrgSample / NIX_FMT_CONVERTER_FREQ_PRECISION) + 1;
        } else {
            r = ammSampesOrg + 1;
        }
    }
    return r;
}
