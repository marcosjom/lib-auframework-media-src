//
//  AUFrameworkDefiniciones.h
//  DodoTown
//
//  Created by Marcos Ortega Morales on 30/07/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUFRAMEWORKDEFINICIONES_H_INCLUIDO
#define AUFRAMEWORKDEFINICIONES_H_INCLUIDO

#include "AUFrameworkBaseStdAfx.h"

/****************************************************/
/*** DEFINICIONES USADAS EN TIEMPO DE COMPILACION ***/
/****************************************************/

#define ESCALA_PIXELES_POR_METRO_MUNDO_FISICA 	64.0f

// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ DEFINICIONES RECOMENDADAS PARA RELEASE
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++

//Si defined, entonces los modelos de renderizado se definen mediante los indices
//de los vertices.
#define CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES

//Si defined, entonces se omite el codigo y referencias a librerias Box2d.
#define CONFIG_NB_UNSUPPORT_BOX2D

//Si defined, entonces se omite el codigo y referencias a librerias de audio.
#define CONFIG_NB_UNSUPPORT_AUDIO_IO

// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ DEFINICIONES RECOMENDADAS PARA DEBUG GENERAL
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++

//Si defined, entonces llama a glGetError despues de cada comando NBGestorGL.
//Nota: Llamar a glGetError obliga al hardware a ejecutar operaciones pendientes,
//lo cual reduce el rendimiento del renderizado.
//#define CONFIG_NB_GESTOR_GL_VERIFICAR_ERROR_SIEMPRE

//Si defined, entonces se agrega un valor de verficacion al inicio y final
//de cada bloque de datos de un objeto renderizable. Esto valores se revisan
//para verificar si algun objeto sobreescribe los datos de otro (bugs dificiles de rastrear).
//#define CONFIG_NB_GESTOR_ESCENAS_VERIFICAR_FRONTERAS_BLOQUES_DATOS_RENDER

// +++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++ DEFINICIONES RECOMENDADAS PARA DEBUG AVANZADO
// +++++++++++++++++++++++++++++++++++++++++++++++++++++

//Si defined, entonces se imprimen en consola todos los comando GL invocados.
//#define CONFIG_NB_GESTOR_GL_IMPRIMIR_COMANDOS_GL

//Si defined, entonces llama a glGet para verificar que
//el estado de NBGestorGL corresponde con el estado grafico.
//#define CONFIG_NB_GESTOR_GL_VERIFICAR_CACHE_SIEMPRE

//#define CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_ILUMINACION					//Valida las figuras que se genera el motor de iluminacion
//#define CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP						//Valida llamadasGL que no generan acciones y pudiesen haberse omitido
//#define CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_FRAMEBUFFERS_COMPLETOS	//Valida que los framebuffers esten debidamente creados/completos antes de utilizarlos

//Si defined, entonces el NBSegmentadorFiguras escribe sus acciones en una cadena de texto.
//Util para depuracion de errores al segmentar las figuras
//#define CONFIG_NB_SEGMENTADOR_FIGURAS_ESCRIBIR_LOG

//Si defined, entonces se dibuja la informacion de depuracion del motor de iluminacion
//#define CONFIG_NB_ILUMINACION_DIBUJAR_PORCIONES_ILUMINADAS_MASCARAS
//#define CONFIG_NB_ILUMINACION_DIBUJAR_SOMBRAS_PROYECTADAS
//#define CONFIG_NB_ILUMINACION_DIBUJAR_CAJA_MASCARAS

//Si defined entonces, advierte cuando las texturas se presentan ligeramiente estiradas.
//Permite identificar casos en los que un archivo contenedor hace referencia a una imagen
//que posteriormente fue editada y por tanto la muestra a una 99%-101%.
//#define CONFIG_NB_SPRITES_ESCENA_ADVERTIR_ESTIRAMIENTO_LEVE_TEXTURAS

//Validacion de parametros
#if defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_ILUMINACION) && !defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT)
#error "ERROR DE CONFIGURACION: ASSERT para iluminacion depende de NBASSERT"
#endif

//Valores de verificacion de bloques de datos render
#ifdef CONFIG_NB_GESTOR_ESCENAS_VERIFICAR_FRONTERAS_BLOQUES_DATOS_RENDER
	#define NBGESTORESCENA_VAL_VERIF_BLOQUE_DATOS_RENDER_INI	123456
	#define NBGESTORESCENA_VAL_VERIF_BLOQUE_DATOS_RENDER_FIN	654321
#endif

#endif


