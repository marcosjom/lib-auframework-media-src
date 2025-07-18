//
//  nixtla.h
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

#ifndef NixtlaAudioLib_nixtla_h
#define NixtlaAudioLib_nixtla_h

#ifdef __cplusplus
extern "C" {
#endif

//+++++++++++++++++++
//++ PUBLIC HEADER ++
//+++++++++++++++++++

//-------------------------------
//-- BASIC DEFINITIONS
//-------------------------------

//Data types
typedef unsigned char 		NixBOOL;	//BOOL, Unsigned 8-bit integer value
typedef unsigned char 		NixBYTE;	//BYTE, Unsigned 8-bit integer value
typedef char 				NixSI8;		//NIXSI8, Signed 8-bit integer value
typedef	short int 			NixSI16;	//NIXSI16, Signed 16-bit integer value
typedef	int 				NixSI32;	//NIXSI32, Signed 32-bit integer value
typedef	long long 			NixSI64;	//NixSI64, Signed 64-bit integer value
typedef unsigned char 		NixUI8;		//NixUI8, Unsigned 8-bit integer value
typedef	unsigned short int 	NixUI16;	//NixUI16, Unsigned 16-bit integer value
typedef	unsigned int 		NixUI32;	//NixUI32, Unsigned 32-bit integer value
typedef	unsigned long long	NixUI64;	//NixUI64[n], Unsigned 64-bit array—n is the number of array elements
typedef float				NixFLOAT;	//float
typedef double              NixDOUBLE;  //double

#define NIX_FALSE			0
#define NIX_TRUE			1

//Capabilities mask
#define NIX_CAP_AUDIO_CAPTURE           1
#define NIX_CAP_AUDIO_STATIC_BUFFERS    2
#define NIX_CAP_AUDIO_SOURCE_OFFSETS    4

//Audio description
typedef enum ENNixSampleFmt_ {
    ENNixSampleFmt_Unknown = 0,
    ENNixSampleFmt_Int,         //unsigned byte for 8-bits, signed short/int/long for 16/24/32/64 bits
    ENNixSampleFmt_Float,       //float 32 bits
    //
    ENNixSampleFmt_Count        //float 32 bits
} ENNixSampleFmt;

#define STNixAudioDesc_Zero    { 0, 0, 0, 0, 0 }

typedef struct STNixAudioDesc_ {
    NixUI8  samplesFormat;  //ENNixSampleFmt
    NixUI8  channels;
    NixUI8  bitsPerSample;
    NixUI16 samplerate;
    NixUI16 blockAlign;     //bytes per sample-block: (bitsPerSample / 8) * channels <= for interlaced byte-aligned sizes usually
} STNixAudioDesc;

NixBOOL STNixAudioDesc_IsEqual(const STNixAudioDesc* obj, const STNixAudioDesc* other);

//Context

struct STNixSharedPtr_;
struct STNixMemoryItf_;
struct STNixMutexItf_;
struct STNixContextItf_;

//API

struct STNixApiEngineItf_;
struct STNixApiBufferItf_;
struct STNixApiSourceItf_;
struct STNixApiRecorderItf_;

// STNixSharedPtr (provides retain/release model)

struct STNixSharedPtr_* NixSharedPtr_alloc(struct STNixContextItf_* ctx, void* opq);     //can be casted to void** to obtain the 'opq' value back
void    NixSharedPtr_destroy(struct STNixSharedPtr_* obj);
void*   NixSharedPtr_getOpq(struct STNixSharedPtr_* obj);
void    NixSharedPtr_retain(struct STNixSharedPtr_* obj);
NixSI32 NixSharedPtr_release(struct STNixSharedPtr_* obj); //returns the retainCount

//STNixApiEngineRef (shared pointer)

#define STNixApiEngineRef_Zero     { NULL, NULL }

typedef struct STNixApiEngineRef_ {
    struct STNixSharedPtr_*     ptr;
    struct STNixApiEngineItf_*  itf;
} STNixApiEngineRef;

void NixApiEngineRef_retain(STNixApiEngineRef* obj);
void NixApiEngineRef_release(STNixApiEngineRef* obj);

//STNixApiBufferRef (shared pointer)

#define STNixApiBufferRef_Zero     { NULL, NULL }

typedef struct STNixApiBufferRef_ {
    struct STNixSharedPtr_*     ptr;
    struct STNixApiBufferItf_*  itf;
} STNixApiBufferRef;

void NixApiBufferRef_retain(STNixApiBufferRef* obj);
void NixApiBufferRef_release(STNixApiBufferRef* obj);

//STNixApiSourceRef (shared pointer)

#define STNixApiSourceRef_Zero     { NULL, NULL }

typedef struct STNixApiSourceRef_ {
    struct STNixSharedPtr_*     ptr;
    struct STNixApiSourceItf_*  itf;
} STNixApiSourceRef;

void NixApiSourceRef_retain(STNixApiSourceRef* obj);
void NixApiSourceRef_release(STNixApiSourceRef* obj);

//STNixApiRecorderRef (shared pointer)

#define STNixApiRecorderRef_Zero     { NULL, NULL }

typedef struct STNixApiRecorderRef_ {
    struct STNixSharedPtr_*     ptr;
    struct STNixApiRecorderItf_* itf;
} STNixApiRecorderRef;

void NixApiRecorderRef_retain(STNixApiRecorderRef* obj);
void NixApiRecorderRef_release(STNixApiRecorderRef* obj);

//Callbacks

typedef void (*NixApiRecorderCallback)(STNixApiEngineRef eng, STNixApiRecorderRef rec, const STNixAudioDesc audioDesc, const NixUI8* audioData, const NixUI32 audioDataBytes, const NixUI32 audioDataSamples, void* userdata);

//STNixMemoryItf (API)

typedef struct STNixMemoryItf_ {
    void*   (*malloc)(const NixUI32 newSz, const char* dbgHintStr);
    void*   (*realloc)(void* ptr, const NixUI32 newSz);
    void    (*free)(void* ptr);
} STNixMemoryItf;

//Links NULL methods to a DEFAULT implementation,
//this reduces the need to check for functions NULL pointers.
void NixMemoryItf_fillMissingMembers(STNixMemoryItf* itf);

//STNixMutexItf (API)

typedef struct STNixMutexItf_ {
    void*   (*alloc)(struct STNixContextItf_* ctx);
    void    (*free)(void* obj);
    void    (*lock)(void* obj);
    void    (*unlock)(void* obj);
} STNixMutexItf;

//Links NULL methods to a DEFAULT implementation,
//this reduces the need to check for functions NULL pointers.
void NixMutexItf_fillMissingMembers(STNixMutexItf* itf);

//STNixContextItf (API)

typedef struct STNixContextItf_ {
    STNixMemoryItf  mem;
    STNixMutexItf   mutex;
} STNixContextItf;

//Links NULL methods to a DEFAULT implementation,
//this reduces the need to check for functions NULL pointers.
void NixContextItf_fillMissingMembers(STNixContextItf* itf);

//STNixApiEngineItf (API)

typedef struct STNixApiEngineItf_ {
    STNixApiEngineRef (*alloc)(STNixContextItf* ctx);
    void            (*free)(STNixApiEngineRef ref);
    void            (*printCaps)(STNixApiEngineRef ref);
    NixBOOL         (*ctxIsActive)(STNixApiEngineRef ref);
    NixBOOL         (*ctxActivate)(STNixApiEngineRef ref);
    NixBOOL         (*ctxDeactivate)(STNixApiEngineRef ref);
    void            (*tick)(STNixApiEngineRef ref);
} STNixApiEngineItf;

//Links NULL methods to a NOP implementation,
//this reduces the need to check for functions NULL pointers.
void NixApiEngineItf_fillMissingMembers(STNixApiEngineItf* itf);

//STNixApiBufferItf (API)

typedef struct STNixApiBufferItf_ {
    STNixApiBufferRef (*alloc)(STNixContextItf* ctx, const STNixAudioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes);
    void            (*free)(STNixApiBufferRef ref);
    NixBOOL         (*setData)(STNixApiBufferRef ref, const STNixAudioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes);
    NixBOOL         (*fillWithZeroes)(STNixApiBufferRef ref);
} STNixApiBufferItf;

//Links NULL methods to a NOP implementation,
//this reduces the need to check for functions NULL pointers.
void NixApiBufferItf_fillMissingMembers(STNixApiBufferItf* itf);

//STNixApiSourceItf (API)

typedef struct STNixApiSourceItf_ {
    STNixApiSourceRef (*alloc)(STNixApiEngineRef eng);
    void            (*free)(STNixApiSourceRef ref);
    void            (*setCallback)(STNixApiSourceRef ref, void (*callback)(void* pEng, const NixUI32 sourceIndex, const NixUI32 ammBuffs), void* callbackEng, NixUI32 callbackSourceIndex);
    NixBOOL         (*setVolume)(STNixApiSourceRef ref, const float vol);
    NixBOOL         (*setRepeat)(STNixApiSourceRef ref, const NixBOOL isRepeat);
    void            (*play)(STNixApiSourceRef ref);
    void            (*pause)(STNixApiSourceRef ref);
    void            (*stop)(STNixApiSourceRef ref);
    NixBOOL         (*isPlaying)(STNixApiSourceRef ref);
    NixBOOL         (*isPaused)(STNixApiSourceRef ref);
    NixBOOL         (*setBuffer)(STNixApiSourceRef ref, STNixApiBufferRef buff);  //static-source
    NixBOOL         (*queueBuffer)(STNixApiSourceRef ref, STNixApiBufferRef buff); //stream-source
} STNixApiSourceItf;

//Links NULL methods to a NOP implementation,
//this reduces the need to check for functions NULL pointers.
void NixApiSourceItf_fillMissingMembers(STNixApiSourceItf* itf);

//STNixApiRecorderItf (API)

typedef struct STNixApiRecorderItf_ {
    STNixApiRecorderRef (*alloc)(STNixApiEngineRef eng, const STNixAudioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer);
    void            (*free)(STNixApiRecorderRef ref);
    NixBOOL         (*setCallback)(STNixApiRecorderRef ref, NixApiRecorderCallback callback, void* callbackData);
    NixBOOL         (*start)(STNixApiRecorderRef ref);
    NixBOOL         (*stop)(STNixApiRecorderRef ref);
} STNixApiRecorderItf;

//Links NULL methods to a NOP implementation,
//this reduces the need to check for functions NULL pointers.
void NixApiRecorderItf_fillMissingMembers(STNixApiRecorderItf* itf);

//STNixApiItf (API)

typedef struct STNixApiItf_ {
    STNixApiEngineItf   engine;
    STNixApiBufferItf   buffer;
    STNixApiSourceItf   source;
    STNixApiRecorderItf recorder;
} STNixApiItf;

//Links NULL methods to a NOP implementation,
//this reduces the need to check for functions NULL pointers.
void NixApiItf_fillMissingMembers(STNixApiItf* itf);

//------
//PCMBuffer
//------

typedef struct STNixPCMBuffer_ {
    NixUI8*         ptr;
    NixUI32         use;
    NixUI32         sz;
    STNixAudioDesc desc;
    STNixContextItf ctx;
} STNixPCMBuffer;

void NixPCMBuffer_init(STNixContextItf* ctx, STNixPCMBuffer* obj);
void NixPCMBuffer_destroy(STNixPCMBuffer* obj);
NixBOOL NixPCMBuffer_setData(STNixPCMBuffer* obj, const STNixAudioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes);
NixBOOL NixPCMBuffer_fillWithZeroes(STNixPCMBuffer* obj);

//------
//PCMBuffer (API)
//------

NixBOOL NixPCMBuffer_getApiItf(STNixApiBufferItf* dst);

//------
//PCMFormat converter
//------
// * 1 or 2 channels only
// * float32, int32, int16 or uint8 sample-types only
// * from-to any frequency
//
void*   nixFmtConverter_alloc(STNixContextItf* ctx);
void    nixFmtConverter_free(void* obj);
NixBOOL nixFmtConverter_prepare(void* obj, const STNixAudioDesc* srcDesc, const STNixAudioDesc* dstDesc);
NixBOOL nixFmtConverter_setPtrAtSrc(void* obj, const NixUI32 iChannel, void* ptr, const NixUI32 sampleAlign); //one channel
NixBOOL nixFmtConverter_setPtrAtDst(void* obj, const NixUI32 iChannel, void* ptr, const NixUI32 sampleAlign); //one channel
NixBOOL nixFmtConverter_setPtrAtSrcInterlaced(void* obj, const STNixAudioDesc* desc, void* ptr, const NixUI32 iFirstSample); //all channels at once
NixBOOL nixFmtConverter_setPtrAtDstInterlaced(void* obj, const STNixAudioDesc* desc, void* ptr, const NixUI32 iFirstSample); //all channels at once
NixBOOL nixFmtConverter_convert(void* obj, const NixUI32 srcBlocks, NixUI32 dstBlocks, NixUI32* dstAmmBlocksRead, NixUI32* dstAmmBlocksWritten);
//
NixUI32 nixFmtConverter_maxChannels(void); //= 2, defined at compile-time
NixUI32 nixFmtConverter_samplesForNewFrequency(const NixUI32 ammSampesOrg, const NixUI32 freqOrg, const NixUI32 freqNew); //ammount of output samples from one frequeny to another, +1 for safety



//-------------------------------
//-- ENGINES
//-------------------------------
	
//Engine object
typedef struct STNix_Engine_ {
	void* o; //Abstract objects pointer
} STNix_Engine;

//Engine status
typedef struct STNix_StatusDesc_ {
	//Sources
	NixUI16	countSources;			//All sources
	NixUI16	countSourcesReusable;	//Only reusable sources. Not-reusable = (countSources - countSourcesReusable);
	NixUI16	countSourcesAssigned;	//Only sources retained by ussers. Not-assigned = (countSources - countSourcesAssigned);
	NixUI16	countSourcesStatic;		//Only static sounds sources.
	NixUI16	countSourcesStream;		//Only stream sounds sources. Undefined-sources = (countSources - countSourcesStatic - countSourcesStream);
	//PLay buffers
	NixUI16	countPlayBuffers;		//All play-buffers
	NixUI32	sizePlayBuffers;		//Bytes of all play-buffers
	NixUI32	sizePlayBuffersAtSW;	//Only bytes of play-buffers that resides in nixtla's memory. sizeBuffersAtExternal = (sizeBuffers - sizeBuffersAtSW); //this includes Hardware-buffers
	//Record buffers
	NixUI16	countRecBuffers;		//All rec-buffers
	NixUI32	sizeRecBuffers;			//Bytes of all rec-buffers
	NixUI32	sizeRecBuffersAtSW;		//Only bytes of rec-buffers that resides in nixtla's memory. sizeBuffersAtExternal = (sizeBuffers - sizeBuffersAtSW); //this includes Hardware-buffers
} STNix_StatusDesc;

	
//Callbacks
typedef void (*PTRNIX_SourceReleaseCallback)(STNix_Engine* engAbs, void* userdata, const NixUI32 sourceIndex);
typedef void (*PTRNIX_StreamBufferUnqueuedCallback)(STNix_Engine* engAbs, void* userdata, const NixUI32 sourceIndex, const NixUI16 buffersUnqueuedCount);
typedef void (*PTRNIX_CaptureBufferFilledCallback)(STNix_Engine* engAbs, void* userdata, const STNixAudioDesc audioDesc, const NixUI8* audioData, const NixUI32 audioDataBytes, const NixUI32 audioDataSamples);

//Engine
NixBOOL		nixInit(STNixContextItf* ctx, STNix_Engine* engAbs, const NixUI16 pregeneratedSources);
//NixBOOL   nixInitWithOpenAL(STNix_Engine* engAbs, const NixUI16 pregeneratedSources);     //Cross-platform
//NixBOOL   nixInitWithAVFAudio(STNix_Engine* engAbs, const NixUI16 pregeneratedSources);   //Mac/iOS
//NixBOOL   nixInitWithAAudio(STNix_Engine* engAbs, const NixUI16 pregeneratedSources);     //Android
void		nixFinalize(STNix_Engine* engAbs);
NixBOOL		nixContextIsActive(STNix_Engine* engAbs);
NixBOOL		nixctxActivate(STNix_Engine* engAbs);
NixBOOL		nixctxDeactivate(STNix_Engine* engAbs);
//void		nixGetContext(STNix_Engine* engAbs, void* dest); //deprecated and removed: 2025-07-16
void		nixGetStatusDesc(STNix_Engine* engAbs, STNix_StatusDesc* dest);
NixUI32		nixCapabilities(STNix_Engine* engAbs);
void		nixPrintCaps(STNix_Engine* engAbs);
void		nixTick(STNix_Engine* engAbs);

//Buffers
NixUI16		nixBufferWithData(STNix_Engine* engAbs, const STNixAudioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes);
NixBOOL		nixBufferSetData(STNix_Engine* engAbs, const NixUI16 buffIndex, const STNixAudioDesc* audioDesc, const NixUI8* audioDataPCM, const NixUI32 audioDataPCMBytes);
NixBOOL     nixBufferFillZeroes(STNix_Engine* engAbs, const NixUI16 buffIndex); //the unused range of the bufer will be zeroed
NixUI32		nixBufferRetainCount(STNix_Engine* engAbs, const NixUI16 buffIndex);
void		nixBufferRetain(STNix_Engine* engAbs, const NixUI16 buffIndex);
void		nixBufferRelease(STNix_Engine* engAbs, const NixUI16 buffIndex);
float		nixBufferSeconds(STNix_Engine* engAbs, const NixUI16 buffIndex);
STNixAudioDesc nixBufferAudioDesc(STNix_Engine* engAbs, const NixUI16 buffIndex);

//Sources
NixUI16		nixSourceAssignStatic(STNix_Engine* engAbs, NixUI8 lookIntoReusable, NixUI8 audioGroupIndex, PTRNIX_SourceReleaseCallback releaseCallBack, void* releaseCallBackUserData);
NixUI16		nixSourceAssignStream(STNix_Engine* engAbs, NixUI8 lookIntoReusable, NixUI8 audioGroupIndex, PTRNIX_SourceReleaseCallback releaseCallBack, void* releaseCallBackUserData, const NixUI16 queueSize, PTRNIX_StreamBufferUnqueuedCallback bufferUnqueueCallback, void* bufferUnqueueCallbackData);
NixUI32		nixSourceRetainCount(STNix_Engine* engAbs, const NixUI16 sourceIndex);
void		nixSourceRetain(STNix_Engine* engAbs, const NixUI16 sourceIndex);
void		nixSourceRelease(STNix_Engine* engAbs, const NixUI16 sourceIndex);
void		nixSourceSetRepeat(STNix_Engine* engAbs, const NixUI16 sourceIndex, const NixBOOL repeat);
NixUI32		nixSourceGetSamples(STNix_Engine* engAbs, const NixUI16 sourceIndex);
NixUI32		nixSourceGetBytes(STNix_Engine* engAbs, const NixUI16 sourceIndex);
NixFLOAT	nixSourceGetSeconds(STNix_Engine* engAbs, const NixUI16 sourceIndex);
NixFLOAT	nixSourceGetVoume(STNix_Engine* engAbs, const NixUI16 sourceIndex);
void		nixSourceSetVolume(STNix_Engine* engAbs, const NixUI16 sourceIndex, const float volume);
NixUI16		nixSourceGetBuffersCount(STNix_Engine* engAbs, const NixUI16 sourceIndex);
NixUI32		nixSourceGetOffsetSamples(STNix_Engine* engAbs, const NixUI16 sourceIndex);
NixUI32		nixSourceGetOffsetBytes(STNix_Engine* engAbs, const NixUI16 sourceIndex);
void		nixSourceSetOffsetSamples(STNix_Engine* engAbs, const NixUI16 sourceIndex, const NixUI32 offsetSamples);
void		nixSourcePlay(STNix_Engine* engAbs, const NixUI16 sourceIndex);
NixBOOL		nixSourceIsPlaying(STNix_Engine* engAbs, const NixUI16 sourceIndex);
void		nixSourcePause(STNix_Engine* engAbs, const NixUI16 sourceIndex);
void		nixSourceStop(STNix_Engine* engAbs, const NixUI16 sourceIndex);
void		nixSourceRewind(STNix_Engine* engAbs, const NixUI16 sourceIndex);
NixBOOL		nixSourceSetBuffer(STNix_Engine* engAbs, const NixUI16 sourceIndex, const NixUI16 bufferIndex);
NixBOOL		nixSourceStreamAppendBuffer(STNix_Engine* engAbs, const NixUI16 sourceIndex, const NixUI16 streamBufferIndex);
NixBOOL		nixSourceEmptyQueue(STNix_Engine* engAbs, const NixUI16 sourceIndex);
NixBOOL		nixSourceHaveBuffer(STNix_Engine* engAbs, const NixUI16 sourceIndex, const NixUI16 bufferIndex);
	
//Audio groups
NixBOOL		nixSrcGroupIsEnabled(STNix_Engine* engAbs, const NixUI8 groupIndex);
NixFLOAT	nixSrcGroupGetVolume(STNix_Engine* engAbs, const NixUI8 groupIndex);
void		nixSrcGroupSetEnabled(STNix_Engine* engAbs, const NixUI8 groupIndex, const NixBOOL enabled);
void		nixSrcGroupSetVolume(STNix_Engine* engAbs, const NixUI8 groupIndex, const NixFLOAT volume);

//Capture
NixBOOL		nixCaptureInit(STNix_Engine* engAbs, const STNixAudioDesc* audioDesc, const NixUI16 buffersCount, const NixUI16 samplesPerBuffer, PTRNIX_CaptureBufferFilledCallback bufferCaptureCallback, void* bufferCaptureCallbackUserData);
void		nixCaptureFinalize(STNix_Engine* engAbs);
NixBOOL		nixCaptureIsOnProgress(STNix_Engine* engAbs);
NixBOOL		nixCaptureStart(STNix_Engine* engAbs);
void		nixCaptureStop(STNix_Engine* engAbs);
NixUI32		nixCaptureFilledBuffersCount(STNix_Engine* engAbs);
NixUI32		nixCaptureFilledBuffersSamples(STNix_Engine* engAbs);
float		nixCaptureFilledBuffersSeconds(STNix_Engine* engAbs);
//Note: when a BufferFilledCallback is defined, the buffers are automatically released after invoking the callback.
void		nixCaptureFilledBuffersRelease(STNix_Engine* engAbs, NixUI32 quantBuffersToRelease);

//Debug
void		nixDbgPrintSourcesStatus(STNix_Engine* engAbs);

#ifdef __cplusplus
}
#endif
		
#endif
