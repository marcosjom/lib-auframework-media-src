//
//  NBGestorSonidos.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 09/07/25.
//

#include "AUFrameworkMediaStdAfx.h"
#include "nb/core/NBMemory.h"
#include "nb/core/NBThreadMutex.h"

#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO

//NIXTLA config -- start
#define NIX_MALLOC(POINTER_DEST, POINTER_TYPE, SIZE_BYTES, STR_HINT)    \
                                    POINTER_DEST = (POINTER_TYPE*)NBMemory_alloc(SIZE_BYTES)
#define NIX_FREE(POINTER)           NBMemory_free(POINTER)
#define NIX_SOURCES_GROWTH      	32
#define NIX_BUFFERS_GROWTH	        32
#define NIX_AUDIO_GROUPS_SIZE       2


#define NIX_MUTEX_T                 STNBThreadMutex
#define NIX_MUTEX_INIT(PTR)         NBThreadMutex_init(PTR)
#define NIX_MUTEX_DESTROY(PTR)      NBThreadMutex_release(PTR)
#define NIX_MUTEX_LOCK(PTR)         NBThreadMutex_lock(PTR)
#define NIX_MUTEX_UNLOCK(PTR)       NBThreadMutex_unlock(PTR)

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	#define NIX_ASSERTS_ACTIVATED
#endif
//
#ifdef CONFIG_NB_DESHABILITAR_IMPRESIONES_PRINTF
	#define NIX_SILENT_MODE
#endif

//#define NIX_VERBOSE_MODE

//NIXTLA config -- end

#include "nixtla-avfaudio.m"

#endif

