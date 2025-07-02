#ifndef NBGESTORGL_H_INCLUIDO
#define NBGESTORGL_H_INCLUIDO

//
//  NBGestorGL.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 09/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "NBColor.h"
#include "NBCajaAABB.h"
#include "NBTriangulo.h"
#include "NBRectangulo.h"
#include "NBMatriz.h"
#include "NBGestorPilaLlamadas.h"
#include "AUNumerico.h"
#include "AUCadena.h"
#include "AUCadenaMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUArregloNativoOrdenadoMutable.h"
#include "AUPilaNativaMutable.h"
#include "NBGestorGLDef.h"
#include "nb/2d/NBRect.h"
#include "nb/scene/NBScnRender.h"

#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_4_ESCNA(TIPOVERTICE, BLOQUEVERTICESGL)											NBGestorEscena::reservarIndicesParaTriangStrip4Independizado(TIPOVERTICE, BLOQUEVERTICESGL.primerElemento)
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_N_ESCNA(TIPOVERTICE, BLOQUEVERTICESGL, CANT_VERTICES_STRIP)					    NBGestorEscena::reservarIndicesParaTriangStripIndependizado(TIPOVERTICE, BLOQUEVERTICESGL.primerElemento, CANT_VERTICES_STRIP)
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_N_DESDE_TRIANGFAN_ESCNA(TIPOVERTICE, BLOQUEVERTICESGL, CANT_VERTICES_TRIANGFAN) NBGestorEscena::reservarIndicesParaTriangStripIndependizadoDesdeTriangFan(TIPOVERTICE, BLOQUEVERTICESGL.primerElemento, CANT_VERTICES_TRIANGFAN)
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_4_ESCNA(TIPOVERTICE, BLOQUEVERTICESGL, CANT_TRIANGSTRIPS)						NBGestorEscena::reservarIndicesParaTriangStrip4IndependizadoMultiples(TIPOVERTICE, BLOQUEVERTICESGL.primerElemento, CANT_TRIANGSTRIPS)
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_N_ESCNA(TIPOVERTICE, BLOQUEVERTICESGL, CANT_VERTICES_POR_STRIP, CANT_STRIPS)	NBGestorEscena::reservarIndicesParaTriangStripIndependizadoMultiples(TIPOVERTICE, BLOQUEVERTICESGL.primerElemento, CANT_VERTICES_POR_STRIP, CANT_STRIPS);
	//
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_4(TIPOVERTICE, BLOQUEVERTICESGL)											(*propsRenderizado.funcReservarIndicesParaTriangStrip4Independizado)(TIPOVERTICE, BLOQUEVERTICESGL.primerElemento)
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_N(TIPOVERTICE, BLOQUEVERTICESGL, CANT_VERTICES_STRIP)						(*propsRenderizado.funcReservarIndicesParaTriangStripIndependizado)(TIPOVERTICE, BLOQUEVERTICESGL.primerElemento, CANT_VERTICES_STRIP)
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_N_DESDE_TRIANGFAN(TIPOVERTICE, BLOQUEVERTICESGL, CANT_VERTICES_TRIANGFAN)	(*propsRenderizado.funcReservarIndicesParaTriangStripIndependizadoDesdeTriangFan)(TIPOVERTICE, BLOQUEVERTICESGL.primerElemento, CANT_VERTICES_TRIANGFAN)
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_4(TIPOVERTICE, BLOQUEVERTICESGL, CANT_TRIANGSTRIPS)						(*propsRenderizado.funcReservarIndicesParaTriangStrip4IndependizadoMultiples)(TIPOVERTICE, BLOQUEVERTICESGL.primerElemento, CANT_TRIANGSTRIPS)
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_N(TIPOVERTICE, BLOQUEVERTICESGL, CANT_VERTICES_POR_STRIP, CANT_STRIPS)	(*propsRenderizado.funcReservarIndicesParaTriangStripIndependizadoMultiples)(TIPOVERTICE, BLOQUEVERTICESGL.primerElemento, CANT_VERTICES_POR_STRIP, CANT_STRIPS);
	//
#	define NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP																					6
#	define NB_GESTOR_GL_RENDER_TRIANGSTRIP_4(PRIMERO, CONTEO)																	NBGestorGL::acumularIndicesTriangStripIndependiente(PRIMERO, CONTEO)
#	define NB_GESTOR_GL_RENDER_TRIANGSTRIP_N(PRIMERO, CONTEO)																	NBGestorGL::acumularIndicesTriangStripIndependiente(PRIMERO, CONTEO)
#	define NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(PRIMERO, CONTEO)																	NBGestorGL::acumularIndicesTriangStripIndependiente(PRIMERO, CONTEO)
#	define NB_GESTOR_GL_RENDER_TRIANGSTRIPS_N(PRIMERO, CONTEO, CANT_STRIPS)													    NBGestorGL::acumularIndicesTriangStripIndependiente(PRIMERO, CONTEO)
#	define NB_GESTOR_GL_RENDER_TRIANGFAN(PRIMERO, CONTEO)																		NBGestorGL::acumularIndicesTriangStripIndependiente(PRIMERO, CONTEO)
#else
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_4_ESCNA(TIPOVERTICE, BLOQUEVERTICESGL)											BLOQUEVERTICESGL
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_N_ESCNA(TIPOVERTICE, BLOQUEVERTICESGL, CANT_VERTICES_STRIP)					    BLOQUEVERTICESGL
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_N_DESDE_TRIANGFAN_ESCNA(TIPOVERTICE, BLOQUEVERTICESGL, CANT_VERTICES_TRIANGFAN) BLOQUEVERTICESGL
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_4_ESCNA(TIPOVERTICE, BLOQUEVERTICESGL, CANT_TRIANGSTRIPS)						BLOQUEVERTICESGL
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_N_ESCNA(TIPOVERTICE, BLOQUEVERTICESGL, CANT_VERTICES_POR_STRIP, CANT_STRIPS)	BLOQUEVERTICESGL
	//
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_4(TIPOVERTICE, BLOQUEVERTICESGL)											BLOQUEVERTICESGL
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_N(TIPOVERTICE, BLOQUEVERTICESGL, CANT_VERTICES_STRIP)						BLOQUEVERTICESGL
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_N_DESDE_TRIANGFAN(TIPOVERTICE, BLOQUEVERTICESGL, CANT_VERTICES_TRIANGFAN)	BLOQUEVERTICESGL
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_4(TIPOVERTICE, BLOQUEVERTICESGL, CANT_TRIANGSTRIPS)						BLOQUEVERTICESGL
#	define NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_N(TIPOVERTICE, BLOQUEVERTICESGL, CANT_VERTICES_POR_STRIP, CANT_STRIPS)	BLOQUEVERTICESGL
	//
#	define NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP																					4
#	define NB_GESTOR_GL_RENDER_TRIANGSTRIP_4(PRIMERO, CONTEO)																	NBGestorGL::drawArrays(GL_TRIANGLE_STRIP, PRIMERO, CONTEO)
#	define NB_GESTOR_GL_RENDER_TRIANGSTRIP_N(PRIMERO, CONTEO)																	NBGestorGL::drawArrays(GL_TRIANGLE_STRIP, PRIMERO, CONTEO)
#	define NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(PRIMERO, CONTEO)																	{ NBASSERT(CONTEO > 3) NBASSERT((CONTEO % 4) == 0) GLint iElem = PRIMERO; const GLint iElemSigUlt = PRIMERO + CONTEO; while(iElem < iElemSigUlt){ NBGestorGL::drawArrays(GL_TRIANGLE_STRIP, iElem, 4); iElem += 4; }}
#	define NB_GESTOR_GL_RENDER_TRIANGSTRIPS_N(PRIMERO, CONTEO, CANT_STRIPS)													    { NBASSERT(CONTEO > 2) NBASSERT(CANT_STRIPS > 0) NBASSERT((CONTEO % CANT_STRIPS) == 0) const GLsizei cantVertsPorStrip = CONTEO / CANT_STRIPS; const GLint iElemSigUlt = PRIMERO + CONTEO; GLint iElem = PRIMERO; while(iElem < iElemSigUlt){ NBGestorGL::drawArrays(GL_TRIANGLE_STRIP, iElem, cantVertsPorStrip); iElem += cantVertsPorStrip; }}
#	define NB_GESTOR_GL_RENDER_TRIANGFAN(PRIMERO, CONTEO)																		NBGestorGL::drawArrays(GL_TRIANGLE_FAN, PRIMERO, CONTEO)
#endif

#define NB_GESTOR_GL_CRECIMIENTO_BLOQUE_VERTICES_GL		1024 //8192
#define NB_GESTOR_GL_CRECIMIENTO_BLOQUE_INDICES_GL		1024 //4096
#define NB_GESTOR_GL_MAX_TEXTURA_UNIDADES				32
#define NB_GESTOR_GL_MAX_LOTE_PRIMITIVAS				256

#ifdef NB_LIB_GRAFICA_ES_EMBEDDED
#	ifndef GL_FRAMEBUFFER_EXT
#		define GL_FRAMEBUFFER_EXT						GL_FRAMEBUFFER_OES
#	endif
#	ifndef GL_RENDERBUFFER_EXT
#		define GL_RENDERBUFFER_EXT						GL_RENDERBUFFER_OES
#	endif
#	ifndef GL_DEPTH24_STENCIL8_EXT
#		define GL_DEPTH24_STENCIL8_EXT					GL_DEPTH24_STENCIL8_OES
#	endif
#	ifndef GL_DEPTH_ATTACHMENT_EXT
#		define GL_DEPTH_ATTACHMENT_EXT					GL_DEPTH_ATTACHMENT_OES
#	endif
#	ifndef GL_STENCIL_ATTACHMENT_EXT
#		define GL_STENCIL_ATTACHMENT_EXT				GL_STENCIL_ATTACHMENT_OES
#	endif
#	ifndef GL_COLOR_ATTACHMENT0_EXT
#		define GL_COLOR_ATTACHMENT0_EXT					GL_COLOR_ATTACHMENT0_OES
#	endif
#	ifndef GL_FRAMEBUFFER_COMPLETE_EXT
#		define	GL_FRAMEBUFFER_COMPLETE_EXT				GL_FRAMEBUFFER_COMPLETE_OES
#	endif
#	ifndef	GL_FRAMEBUFFER_BINDING_EXT
#		define GL_FRAMEBUFFER_BINDING_EXT				GL_FRAMEBUFFER_BINDING_OES
#	endif
#	ifndef	GL_RGBA8_EXT
#		define GL_RGBA8_EXT								GL_RGBA8_OES
#	endif
#	ifndef GL_INVALID_FRAMEBUFFER_OPERATION
#		define GL_INVALID_FRAMEBUFFER_OPERATION			GL_INVALID_FRAMEBUFFER_OPERATION_OES
#	endif
#	ifndef GL_INVALID_FRAMEBUFFER_OPERATION_EXT
#		define GL_INVALID_FRAMEBUFFER_OPERATION_EXT		GL_INVALID_FRAMEBUFFER_OPERATION_OES
#	endif
#	ifndef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT
#		define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT	GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_OES
#	endif
#	ifndef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT
#		define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT	GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_OES
#	endif
#endif

#ifndef	GL_RGBA8_EXT
#	define GL_RGBA8_EXT GL_RGBA8			//usado para la creacion de los render-buffers
#endif



#define STR_GL_MATRIX_MODE(MODE)		(MODE == GL_MODELVIEW ? "GL_MODELVIEW" : MODE  == GL_PROJECTION ? "GL_PROJECTION" : MODE == GL_TEXTURE ? "GL_TEXTURE" : "GL_OTHER")
#define STR_GL_DRAW_MODE(MODE)			(MODE == GL_POINTS ? "GL_POINTS" : MODE == GL_LINE_STRIP ? "GL_LINE_STRIP" : MODE == GL_LINE_LOOP ? "GL_LINE_LOOP" : MODE == GL_LINES ? "GL_LINES" : MODE == GL_TRIANGLE_STRIP ? "GL_TRIANGLE_STRIP" : MODE == GL_TRIANGLE_FAN ? "GL_TRIANGLE_FAN" : MODE == GL_TRIANGLES ? "GL_TRIANGLES" : "GL_OTHER")
#define STR_GL_TEX_UNIT(TEXU)			(TEXU == GL_TEXTURE0 ? "GL_TEXTURE0" : TEXU == GL_TEXTURE1 ? "GL_TEXTURE1" : TEXU == GL_TEXTURE2 ? "GL_TEXTURE2" : TEXU == GL_TEXTURE3 ? "GL_TEXTURE3" : TEXU == GL_TEXTURE4 ? "GL_TEXTURE4" : TEXU == GL_TEXTURE5 ? "GL_TEXTURE5" : TEXU == GL_TEXTURE6 ? "GL_TEXTURE6" : TEXU == GL_TEXTURE7 ? "GL_TEXTURE7" : TEXU == GL_TEXTURE8 ? "GL_TEXTURE8" : TEXU == GL_TEXTURE9 ? "GL_TEXTURE9" : TEXU == GL_TEXTURE10 ? "GL_TEXTURE10" : TEXU == GL_TEXTURE11 ? "GL_TEXTURE11" : TEXU == GL_TEXTURE12 ? "GL_TEXTURE12" : TEXU == GL_TEXTURE13 ? "GL_TEXTURE13" : TEXU == GL_TEXTURE14 ? "GL_TEXTURE14" : TEXU == GL_TEXTURE15 ? "GL_TEXTURE15" : TEXU == GL_TEXTURE16 ? "GL_TEXTURE16" : TEXU == GL_TEXTURE17 ? "GL_TEXTURE17" : TEXU == GL_TEXTURE18 ? "GL_TEXTURE18" : TEXU == GL_TEXTURE19 ? "GL_TEXTURE19" : TEXU == GL_TEXTURE20 ? "GL_TEXTURE20" : TEXU == GL_TEXTURE21 ? "GL_TEXTURE21" : TEXU == GL_TEXTURE22 ? "GL_TEXTURE22" : TEXU == GL_TEXTURE23 ? "GL_TEXTURE23" : TEXU == GL_TEXTURE24 ? "GL_TEXTURE24" : TEXU == GL_TEXTURE25 ? "GL_TEXTURE25" : TEXU == GL_TEXTURE26 ? "GL_TEXTURE26" : TEXU == GL_TEXTURE27 ? "GL_TEXTURE27" : TEXU == GL_TEXTURE28 ? "GL_TEXTURE28" : TEXU == GL_TEXTURE29 ? "GL_TEXTURE29" : TEXU == GL_TEXTURE30 ? "GL_TEXTURE30" : TEXU == GL_TEXTURE31 ? "GL_TEXTURE31" : "GL_OTHER")
#define STR_GL_TEX_FILTER(MODE)			(MODE == GL_NEAREST ? "GL_NEAREST" : MODE == GL_LINEAR ? "GL_LINEAR" : MODE == GL_NEAREST_MIPMAP_NEAREST ? "GL_NEAREST_MIPMAP_NEAREST" : MODE == GL_LINEAR_MIPMAP_NEAREST ? "GL_LINEAR_MIPMAP_NEAREST" : MODE == GL_NEAREST_MIPMAP_LINEAR ? "GL_NEAREST_MIPMAP_LINEAR" : MODE == GL_LINEAR_MIPMAP_LINEAR ? "GL_LINEAR_MIPMAP_LINEAR" : "GL_OTHER")
#define STR_GL_TEX_WRAP(MODE)			(/*MODE == GL_CLAMP? "GL_CLAMP" : (no disponible en Android)*/ MODE == GL_REPEAT? "GL_REPEAT" : /*MODE == GL_CLAMP_TO_BORDER ? "GL_CLAMP_TO_BORDER" : (no disponible en Android)*/ MODE == GL_CLAMP_TO_EDGE ? "GL_CLAMP_TO_EDGE" : "GL_OTHER")
#define STR_GL_TEX_PARAM(PARAM, MODE)	(PARAM == GL_TEXTURE_MIN_FILTER || PARAM == GL_TEXTURE_MAG_FILTER ? STR_GL_TEX_FILTER(MODE) : PARAM == GL_TEXTURE_WRAP_S || PARAM == GL_TEXTURE_WRAP_T ? STR_GL_TEX_WRAP(MODE) : "GL_OTHER")
#define STR_GL_CAP(CAP)					(CAP == GL_ALPHA_TEST ?  "GL_ALPHA_TEST" : CAP == GL_COLOR_LOGIC_OP ?  "GL_COLOR_LOGIC_OP": CAP == GL_CULL_FACE ?  "GL_CULL_FACE": CAP == GL_DEPTH_TEST ?  "GL_DEPTH_TEST": CAP == GL_DITHER ?  "GL_DITHER": CAP == GL_FOG ?  "GL_FOG": CAP == GL_LINE_SMOOTH ?  "GL_LINE_SMOOTH": CAP == GL_MULTISAMPLE ?  "GL_MULTISAMPLE": CAP == GL_NORMALIZE ?  "GL_NORMALIZE": CAP == GL_POINT_SMOOTH ?  "GL_POINT_SMOOTH": CAP == GL_POLYGON_OFFSET_FILL ?  "GL_POLYGON_OFFSET_FILL": CAP == GL_RESCALE_NORMAL ?  "GL_RESCALE_NORMAL": CAP == GL_SAMPLE_ALPHA_TO_COVERAGE ?  "GL_SAMPLE_ALPHA_TO_COVERAGE": CAP == GL_SAMPLE_ALPHA_TO_ONE ?  "GL_SAMPLE_ALPHA_TO_ONE": CAP == GL_SAMPLE_COVERAGE ?  "GL_SAMPLE_COVERAGE": CAP == GL_LIGHTING ?  "GL_LIGHTING": CAP == GL_COLOR_MATERIAL ?  "GL_COLOR_MATERIAL": CAP == GL_BLEND ?  "GL_BLEND": CAP == GL_TEXTURE_2D ?  "GL_TEXTURE_2D": CAP == GL_SCISSOR_TEST ?  "GL_SCISSOR_TEST": CAP == GL_STENCIL_TEST ?  "GL_STENCIL_TEST": CAP == GL_LIGHT_MODEL_TWO_SIDE ? "GL_LIGHT_MODEL_TWO_SIDE" : "GL_OTHER" )
#define STR_GL_ERROR(idError)			(idError==GL_INVALID_ENUM?"GL_INVALID_ENUM":idError==GL_INVALID_VALUE?"GL_INVALID_VALUE":idError==GL_INVALID_OPERATION?"GL_INVALID_OPERATION":idError==GL_STACK_OVERFLOW?"GL_STACK_OVERFLOW":idError==GL_STACK_UNDERFLOW?"GL_STACK_UNDERFLOW":idError==GL_OUT_OF_MEMORY?"GL_OUT_OF_MEMORY":idError==GL_NO_ERROR?"GL_NO_ERROR":idError==GL_INVALID_FRAMEBUFFER_OPERATION?"GL_INVALID_FRAMEBUFFER_OPERATION":idError==GL_INVALID_FRAMEBUFFER_OPERATION_EXT?"GL_INVALID_FRAMEBUFFER_OPERATION_EXT":"GL_ERROR_DESCONOCIDO")

#ifdef CONFIG_NB_GESTOR_GL_VERIFICAR_ERROR_SIEMPRE
#	define VERIFICA_ERROR_GL(nomFunc) 	{ GLenum idErrorGL=glGetError(); if(idErrorGL!=GL_NO_ERROR) PRINTF_ERROR("GL '%s' (#%d) en %s\n", STR_GL_ERROR(idErrorGL), idErrorGL, nomFunc); NBASSERT(idErrorGL==GL_NO_ERROR);}
#else
#	define VERIFICA_ERROR_GL(nomFunc)
#endif

#ifdef CONFIG_NB_GESTOR_GL_IMPRIMIR_COMANDOS_GL
#	define PRINTF_COMANDO_GL(STR_FMT, ...)		PRINTF_INFO(STR_FMT"\n", ##__VA_ARGS__)
#else
#	define PRINTF_COMANDO_GL(STR_FMT, ...)
#endif

#define GL_CMD_EJECUTADO(STR_FMT, ...)			{ PRINTF_COMANDO_GL(STR_FMT, ##__VA_ARGS__); VERIFICA_ERROR_GL(STR_FMT) }

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
#	define NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL(STRNOMBRE) NBGestorGL::dbgNombrarActivadorVerticesGL(STRNOMBRE);
#else
#	define NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL(STRNOMBRE)
#endif

//----------
// LOTES DE COMANDOS
//----------
//MACROS para lote de indices de TRIANG_STRIPS_INDEPENDIENTES
#if defined(CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES)
#	define NB_GESTOR_ESCENAS_USA_CACHE_LOTE_INSTRUCCIONES
#endif

#ifdef NB_GESTOR_ESCENAS_USA_CACHE_LOTE_INSTRUCCIONES
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
#		define NBGESTORGL_LOTE_TRIANGSTRIPS_PURGAR(STRPURGADOR)	_cacheGL.dbgLoteTringStripsNombrePurgador->establecer(STRPURGADOR); if(_loteDrawElemsAcumTriangStrips.cantAcumuladaElems!=0) { GLint cant = _loteDrawElemsAcumTriangStrips.cantAcumuladaElems; _loteDrawElemsAcumTriangStrips.cantAcumuladaElems = 0; NBGestorGL::drawElements(GL_TRIANGLE_STRIP, _loteDrawElemsAcumTriangStrips.iPrimerElem, cant); }
#		define NBGESTORGL_LOTE_TRIANGSTRIPS_ASSERT_ESTA_VACIO		NBASSERT(_loteDrawElemsAcumTriangStrips.cantAcumuladaElems==0)
#	else
#		define NBGESTORGL_LOTE_TRIANGSTRIPS_PURGAR(STRPURGADOR)	if(_loteDrawElemsAcumTriangStrips.cantAcumuladaElems!=0) { GLint cant = _loteDrawElemsAcumTriangStrips.cantAcumuladaElems; _loteDrawElemsAcumTriangStrips.cantAcumuladaElems = 0; NBGestorGL::drawElements(GL_TRIANGLE_STRIP, _loteDrawElemsAcumTriangStrips.iPrimerElem, cant); }
#		define NBGESTORGL_LOTE_TRIANGSTRIPS_ASSERT_ESTA_VACIO		NBASSERT(_loteDrawElemsAcumTriangStrips.cantAcumuladaElems==0)
#	endif
#else
#	define NBGESTORGL_LOTE_TRIANGSTRIPS_PURGAR(STRPURGADOR)		//Vacio
#	define NBGESTORGL_LOTE_TRIANGSTRIPS_ASSERT_ESTA_VACIO			//Vacio
#endif

//MACROS para TODOS los LOTES
#define NBGESTORGL_LOTES_TODOS_PURGAR(STRPURGADOR)			NBGESTORGL_LOTE_TRIANGSTRIPS_PURGAR(STRPURGADOR)
#define NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS			NBGESTORGL_LOTE_TRIANGSTRIPS_ASSERT_ESTA_VACIO

//-----------
// Validacion de Operaciones Inecesarias u Optimizables
//-----------
#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
	#define NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY(STR_NOM)					NBGestorGL::dbgNombrarConvocadorDrawArrays(STR_NOM);
	#define NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP(STR_NOM)	NBGestorGL::dbgNombrarConvocadorAcumTriangStripsIndep(STR_NOM);
	#define NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_LINES_INDEP(STR_NOM)
#else
	#define NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY(STR_NOM)
	#define NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP(STR_NOM)
	#define NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_LINES_INDEP(STR_NOM)
#endif

//---------------------------
//-- Modo de renderizado de textura
//---------------------------
#define NB_TEXTURA_BIT_IMAGEN_PATRON		0x10
#define NB_TEXTURA_BIT_IMAGEN_REGULAR		0x20
//
#define NB_TEXTURA_BIT_PINTADO_PRECISO		0x1		//GL_NEAREST
#define NB_TEXTURA_BIT_PINTADO_SUAVIZADO	0x2		//GL_LINEAR
//
enum ENTexturaModoPintado { //Nota: en la clase "AUTextura" se almacena en un BYTE (valores deben permanecer dentro de ese rango)
	//Patron
	ENTexturaModoPintado_Patron_Preciso		= 0x11,
	ENTexturaModoPintado_Patron_Suavizado	= 0x12,		//Determina el MIN_FILTER, MAX_FILTER, WRAP_S y WRAP_T de la textura
	//Imagen (no repetible)
	ENTexturaModoPintado_Imagen_Precisa		= 0x21,
	ENTexturaModoPintado_Imagen_Suavizada	= 0x22,
};


//---------------------------
//-- Cache de la configuracion de punteros clientes
//---------------------------

struct STGLEstadoTexCords {
	bool				coordsHabilitado;
	UI32				coordsSaltos;
	void*				coordsPuntero;
};

struct STGLEstadoVAO {
	bool				verticesHabilitado;
	bool				coloresHabilitado;
	UI32				verticesSaltos;
	UI32				coloresSaltos;
	void*				verticesPuntero;
	void*				coloresPuntero;
	STGLEstadoTexCords	unidadesTex[NB_GESTOR_GL_MAX_TEXTURA_UNIDADES];
};

//---------------------------
//-- Vertices GL
//---------------------------
enum ENVerticeGlTipo {				//Nota: los valores de esta enumaracion sirven de indice de arreglo
	ENVerticeGlTipo_SinTextura = 0,	//Vertices sin textura
	ENVerticeGlTipo_MonoTextura,	//Vertices para una textura
	ENVerticeGlTipo_BiTextura,		//Vertice para multitextura de DOS (glClientActiveTexture, glActiveTexture, glTexEnvf, ...)
	ENVerticeGlTipo_TriTextura,		//Vertice para multitextura de TRES (glClientActiveTexture, glActiveTexture, glTexEnvf, ...)
	ENVerticeGlTipo_Conteo
};

struct STArregloVerticesGL {
	bool				registroOcupado;
	ENVerticeGlTipo		tipoVertices;
	GLuint				idVertexArrayObjectGl;
	GLuint				idBufferGlVertices;
	#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
	GLuint				idBufferGlIndices;
	#endif
	STGLEstadoVAO		vaoConfig;
	//Enlace con administrador
	PTRfuncObtenerDatosVao	funcObtenerDatos;
	void*					funcObtenerDatosParam;
};

//Optimizacion, lote de glDrawArrays/glDrawElements consecutivos que pueden rederizarse en una sola instruccion.
#ifdef NB_GESTOR_ESCENAS_USA_CACHE_LOTE_INSTRUCCIONES
struct STGestorGLLoteDraw {
	SI32 iPrimerElem;
	SI32 cantAcumuladaElems;
};
#endif

//---------------------------
//-- Cache del GestorGL
//---------------------------
struct STGestorGLUnidadTex {
	bool				texture2DHabilitada;
	GLuint				idTexturaLigada;
	GLenum				modoEntorno;			//GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE: GL_ADD, GL_MODULATE, GL_DECAL, GL_BLEND, GL_REPLACE, or GL_COMBINE
	FLOAT				escalaCoordenadas;		//De la matriz Textura actual
	NBMatriz			matrizTextura;			//Matriz textura actual
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
	UI32				dbgCantOperacionesConTex;
	#endif
};

//---------------------------
//-- Cache de propiedades de textura
//---------------------------
typedef struct STGestorGLTextura_ {
	UI32				idTexturaGL;		//IdentificadorGL de la Textura
	UI16				anchoGL;			//Tamano de textura
	UI16				altoGL;				//Tamano de textura
	GLenum				formatoGL;			//Formato de pixeles
	bool				mipMapHabilitado;	//Si tiene el MipMap habilitado
	bool				mipMapSucio;		//Si se han producido cambios desde la ultima vez que se genero el MipMap
	//Configuracon de textura
	UI8					configPintado;		//ENTexturaModoPintado
	GLenum				configWarpST;		//Wrap en coordenadas S y T (imagen o patron)
	GLenum				configMagFil;		//Filtro magnificador
	GLenum				configMinFil;		//Filtro minificador
} STGestorGLTextura;

typedef struct STGLEstado_ {
	SI32				idFrameBufferGlLigado;
	SI32				idRenderBufferGlLigado;
	SI32				iVaoLigado;
	GLuint				idBufferArrayLigado;
	#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
	GLuint				idBufferElemsLigado;
	#endif
	SI32				ultimoFiltroMinMagTextura;
	bool				iluminacionGLHabilitada;
	NBColor				colorDeLimpieza;
	NBColor				colorActivo;
	NBColor				colorLuzAmbienteActivo;
	NBRectanguloI		areaTijeras;
	NBRectanguloI		puertoDeVision;
	NBCajaAABB			cajaProyeccion;
	GLenum				modoMatriz;
	NBMatriz			matrizModelo;
	NBMatriz			matrizProyeccion;
	//
	SI32				iUnidadTexturaActivaCliente;
	SI32				iUnidadTexturaActivaServidor;
	STGestorGLUnidadTex	unidadesTex[NB_GESTOR_GL_MAX_TEXTURA_UNIDADES];
	//
	STGLEstadoVAO		vaoEstado;	//Cache general cuando no soporta VAO (bufferes VBO ligados y saltos+punteros de arreglos)
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
	//NOP con VAOs
	AUCadenaMutable8*	dbgVAONombreFuncion;
	UI32				dbgVAOCantOperaciones;
	//NOP con VerticesGL
	bool				dbgVerticesUltimaFormaEnIndices[ENVerticeGlTipo_Conteo];		//Si el objeto se renderizo a partir de vertices(glDrawArrays) o de indices(glDrawElements)
	bool				dbgVerticesUltimaFormaEsIndependiente[ENVerticeGlTipo_Conteo];	//Si el objeto ya viene independizado
	bool				dbgVerticesUltimaFormaEsIndependizable[ENVerticeGlTipo_Conteo];	//Si el objeto puede independizarse (agregando vertices o algo mas)
	GLenum				dbgVerticesUltimoUsadoModo[ENVerticeGlTipo_Conteo];
	SI32				dbgVerticesUltimoUsadoIndice[ENVerticeGlTipo_Conteo];
	bool				dbgIndicesUltimaFormaEsIndependiente[ENVerticeGlTipo_Conteo];	//Si el objeto ya viene independizado
	bool				dbgIndicesUltimaFormaEsIndependizable[ENVerticeGlTipo_Conteo];	//Si el objeto puede independizarse (agregando vertices o algo mas)
	GLenum				dbgIndicesUltimoUsadoModo[ENVerticeGlTipo_Conteo];
	SI32				dbgIndicesUltimoUsadoIndice[ENVerticeGlTipo_Conteo];
	AUCadenaMutable8*	dbgDrawArrayConvocador;
	AUCadenaMutable8*	dbgDrawElementsConvocador;
	AUCadenaMutable8*	dbgLoteTringStripsNombrePurgador;
	#endif
} STGLEstado;

class NBGestorGL {
	public:
		static bool					inicializar();
		static void					finalizar();
		static bool					gestorInicializado();
		static bool					reiniciarContextoGrafico(const bool elimObjetosAnteriores);
		//Bufferes de verticesGL
		static SI32					bufferVerticesReservar(const ENVerticeGlTipo tipoVerticesGl, PTRfuncObtenerDatosVao funcObtenerDatos, void* funcObtenerDatosParam);
		static void					bufferVerticesLiberar(const SI32 iBuffer);
		//
		static void					enableInit(GLenum cap); //Ignora la cache
		static void					disableInit(GLenum cap); //Ignora la cache
		static void					enable(GLenum cap);		//Ejecuta el comando en dependencia de la cache
		static void					disable(GLenum cap);	//Ejecuta el comando en dependencia de la cache
		static void					blendFuncEstablecerValoresPorDefecto(GLenum sFactorRGBA, GLenum dFactorRGBA);
		static void					blendFuncCargarValoresPorDefecto();
		static void					blendFunc(GLenum sFactor, GLenum dFactor);
		//static void				blendFuncSeparate(GLenum sFactorRGB, GLenum dFactorRGB, GLenum sFactorAlpha, GLenum dFactorAlpha);
		static void					colorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a);
		static void					clear(GLbitfield mask);
		#ifdef NB_GESTOR_ESCENAS_USA_CACHE_LOTE_INSTRUCCIONES
		static void					flushCacheLocal();
		#endif
		static void					flush();
		static void					finish();
		static void					viewport(GLint x, GLint y, GLsizei width, GLsizei height);
		static void					orthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top);
		static void					clearColor(const NBColor &colorDeLimpieza);
 		static void					clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
		static void					scissor(GLint x, GLint y, GLsizei width, GLsizei height);
		//static void				stencilFunc(GLenum func, GLint ref, GLuint mask);
		//static void				stencilOp(GLenum sfail, GLenum dpfail, GLenum dppass);
		//
		static STGLEstado			cacheGL();
		//Luces
		static void					lightModelf(GLenum param, GLfloat val);
		static void					lightModelfv(GLenum param, GLfloat* vals);
		static void					lightModelAmbient(const GLfloat* valsF);
		static void					lightModelAmbient(const float r, const float g, const float b, const float a);
		static void					lightModelAmbient(const NBColor &colorLuzAmbiente);
		//static void				colorMaterial(GLenum face, GLenum mode); //glColorMaterial mut be called before enabling GL_COLOR_MATERIAL.
		//
		static NBColor				colorActivo();
		static void					color4f(const NBColor &color);
		static void					color4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
		static void					matrixModeInit(GLenum mode);	//Ignora la cache e inicializa con la matriz identidad
		static void					matrixMode(GLenum mode);		//Ejecuta el comando en dependencia de la cache
		static void					loadIdentity();
		static void					loadMatrixf(const NBMatriz &matriz);
		static void					scale3f(GLfloat x, GLfloat y, GLfloat z);
		static void					drawTex(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h);
		static void					drawArrays(GLenum mode, GLint first, GLsizei count);
		#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
		static void					drawElements(GLenum mode, GLint first, GLsizei count);
		static void					acumularIndicesTriangStripIndependiente(GLint first, GLsizei count);
		#endif
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
		static void					dbgNombrarConvocadorDrawArrays(const char* strNombre);
		static void					dbgNombrarConvocadorAcumTriangStripsIndep(const char* strNombre);
		static const char*			dbgModoFormaGLNombre(GLenum modo);
		static bool					dbgModoFormaGLEsIndependizable(GLenum modo);
		#endif
		//Texturas
		static bool					soportaTexturas2DNoMultiploDe2();
		static bool					soportaDrawTexture();
		static void					genTextures(GLsizei n, GLuint* textures, STGestorGLTextura* propsTexturas);
		static bool					isTexture(GLuint texture);
		static void					deleteTextures(GLsizei n, GLuint* textures);
		static void					bindTexture(SI32 indiceUnidadTexturaGL, GLuint texture);
		static void					configurarTextura(const ENTexturaModoPintado modoPintado, const bool habilitarMipMap, STGestorGLTextura* propsTextura);
		static void					setTextureCropRect(const STNBRectI16* cropRect);
		static void					texImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels, STGestorGLTextura* propsTextura);
		static void					texSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels, STGestorGLTextura* propsTextura);
        //Unused
		//static void				texEnvi(SI32 indiceUnidadTexturaGL, GLenum target, GLenum pname, GLint param);
		static void					activeTexture(SI32 indiceUnidadTextura);
		static void					generateMipmapEXT(GLenum target, STGestorGLTextura* propsTextura);
		//Frame buffers (EXTENSIONES)
		static void					genFramebuffersEXT(GLsizei n, GLuint* frameBuffers);
		static void					deleteFramebuffersEXT(GLsizei n, GLuint* frameBuffers);
		static void					bindFramebufferEXT(GLenum target, GLuint frameBuffer);
		static void					bindFramebufferInicialEXT(GLenum target);
		static void					framebufferTexture2DEXT(GLenum target, GLenum attachment, GLenum texTarget, GLuint texture, GLint level);
        static void                 framebufferRenderbufferEXT(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
		static GLenum				checkFramebufferStatusEXT(GLenum target);
        //Unused
        //static bool               isFramebufferEXT(GLuint frameBuffer);
        //Unused
		//static void				blitFramebufferEXT(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
		//Render buffers (EXTENSIONES)
		static void					genRenderbuffersEXT(GLsizei n, GLuint* renderBuffers);
		static void					deleteRenderbuffersEXT(GLsizei n, GLuint* renderBuffers);
		static void					bindRenderbufferEXT(GLenum target, GLuint renderBuffers);
		static bool					isRenderbufferEXT(GLuint renderbuffer);
		static void					renderbufferStorageEXT(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
		//Errores
		static GLenum				getError();
		//Arreglos de vertices GL
		static void					prepararVerticesGLParaRenderizado();
		static void					activarVerticesGL(const SI32 iVao);
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
		static void					dbgNombrarActivadorVerticesGL(const char* strNombre);
		#endif
		static SI32					maximaDimensionTextura();
        //
        static BOOL                 getApiItf(STNBScnRenderApiItf* dst);
	private:
		static bool					_gestorInicializado;
		static bool					_soportaVAOs;						//Vertex Array Objects
		static bool					_soportaVAOsAnalizado;
		static bool					_soportaVAOArrayBufferBinding;		//Si el VAO conserva el VBOs ARRAY_BUFFER
		static bool					_soportaVAOIndexBufferBinding;		//Si el VAO conserva el VBOs ELEMENT_ARRAY_BUFFER
		static bool					_soportaTexturas2DNoMultiploDe2;	//EXTENSION XXX_texture_2D_limited_npot
		static bool					_soportaDrawTexture;				//Extension XXX_draw_texture, to render squared images without VBOs nor triangles.
		static SI32					_maxUnidadesTexUsar;
		static SI32					_maxUnidadesTexturas;
		static SI32					_maxDimensionTextura;
		static GLuint				_idFramebufferAlInicializar;
		//VAOs y VBOs (vertices e indices)
		static STArregloVerticesGL*	_vertsGlArrs;
		static SI32					_vertsGlArrsTam;
		//DATOS CACHE
		static STGLEstado			_cacheGL;
		static NBMatriz				_matrizIdentidad;
		static GLenum				_blendSoucFactorRGBAActual;
		static GLenum				_blendDestFactorRGBAActual;
		static GLenum				_blendSoucFactorRGBAPorDefecto;
		static GLenum				_blendDestFactorRGBAPorDefecto;
		#ifdef NB_GESTOR_ESCENAS_USA_CACHE_LOTE_INSTRUCCIONES
		static STGestorGLLoteDraw	_loteDrawElemsAcumTriangStrips;
		#endif
		//
		static void					privInicializarCaches(const bool esPrimeraVez);
		static void					privInicializarVAOs();
		static void					privInicializarCacheVAO(STGLEstadoVAO* datosCache);
		static void					privConfigurarClienteEstado(const ENVerticeGlTipo tipoVertices, void* punteroPrimerVertice, STGLEstadoVAO* datosCache, const bool forzarConfig);
		//VAO
		static void					privGenVertexArraysEXT(GLsizei n, GLuint* ids);						//Disponible a partir de iOS4: OES_vertex_array_object
		static void					privDeleteVertexArraysEXT(GLsizei n, GLuint* ids);					//Disponible a partir de iOS4: OES_vertex_array_object
		static void					privBindVertexArrayEXTParaInicializacion(const SI32 indice, GLuint idVertexArrayGL);	//Disponible a partir de iOS4: OES_vertex_array_object
		static void					privBindVertexArrayEXT(const SI32 indice, const bool prepararParaConsumir);				//Disponible a partir de iOS4: OES_vertex_array_object
		//
		static SI32					privUnidadTexturaIndice(GLenum textura);
		static GLenum				privUnidadTexturaEnumGl(SI32 iTextura);
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
		static void					privDbgAcumularOperacionRealizada();
		static void					privDbgReiniciarCacheVerticesIndicesUsados(const ENVerticeGlTipo modo);
		#endif
		#ifdef CONFIG_NB_GESTOR_GL_VERIFICAR_CACHE_SIEMPRE
		static void					privDbgCacheAssertVAOs();
		static void					privDbgCacheAssertClientState();
		static void					privDbgCacheAssertActiveTexture();
		static void					privDbgCacheAssertTextureActiveState();
		#endif
        //STNBGpuBufferApiItf
        static void*                STNBGpuBufferApiItf_create(const STNBGpuBufferCfg* cfg, void* usrData);
        static void                 STNBGpuBufferApiItf_destroy(void* data, void* usrData);
        static BOOL                 STNBGpuBufferApiItf_sync(void* data, const STNBGpuBufferCfg* cfg, STNBMemoryBlocksRef mem, const STNBGpuBufferChanges* changes, void* usrData);
        //STNBGpuVertexBufferApiItf
        static void*                STNBGpuVertexBufferApiItf_create(const STNBGpuVertexBufferCfg* cfg, STNBGpuBufferRef vertexBuff, STNBGpuBufferRef idxsBuff, void* usrData);
        static void                 STNBGpuVertexBufferApiItf_destroy(void* data, void* usrData);
        static BOOL                 STNBGpuVertexBufferApiItf_activate(void* data, const STNBGpuVertexBufferCfg* cfg, void* usrData);
        static BOOL                 STNBGpuVertexBufferApiItf_deactivate(void* data, void* usrData);
        static void                 STNBGpuVertexBufferApiItf_setClientState(const STNBGpuVertexBufferCfg* cfg, STNBGpuBufferRef vertexBuff, STNBGpuBufferRef idxsBuff);
};

#endif
