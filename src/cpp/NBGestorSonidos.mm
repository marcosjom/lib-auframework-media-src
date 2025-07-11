//
//  NBGestorSonidos.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 09/07/25.
//

#include "AUFrameworkMediaStdAfx.h"
#include "NBGestorMemoria.h"

#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO

//Configuracion de importacion de NIXTLA
#define NIX_MALLOC(POINTER_DEST, POINTER_TYPE, SIZE_BYTES, STR_HINT) POINTER_DEST = (POINTER_TYPE*)NBGestorMemoria::reservarMemoria(SIZE_BYTES, ENMemoriaTipo_General); NB_DEFINE_NOMBRE_PUNTERO(POINTER_DEST, STR_HINT)
#define NIX_FREE(POINTER) NBGestorMemoria::liberarMemoria(POINTER);
#define NIX_SOURCES_GROWTH	32
#define NIX_BUFFERS_GROWTH	32
#define NIX_AUDIO_GROUPS_SIZE 2
//
#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	#define NIX_ASSERTS_ACTIVATED
#endif
//
#ifdef CONFIG_NB_DESHABILITAR_IMPRESIONES_PRINTF
	#define NIX_SILENT_MODE
#endif
//
//#define NIX_VERBOSE_MODE

#include "nixtla-avfaudio.m"

#endif

