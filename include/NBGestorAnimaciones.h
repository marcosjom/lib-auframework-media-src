//
//  NBGestorAnimaciones.h
//  DodoTown
//
//  Created by Marcos Ortega on 23/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBGestorAnimaciones_H_INCLUIDO
#define NBGestorAnimaciones_H_INCLUIDO

#include "NBGestorPilaLlamadas.h"
#include "NBGestorMemoria.h"
#include "AUArregloNativoMutableP.h"
#include "AUAnimacionDef.h"
#include "AUEscenaAnimacion.h"
#include "AUArchivo.h"
#include "AUEscenaCuerpoI.h"
#include "AUCargadorCuerposI.h"

#define K_NBGESTORANIMACIONES_VALOR_VERIFICACION_BIN						-878787	//para la verificacion de los datos binarios leidos

#define NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(FLUJOARCHIVO)			if(FLUJOARCHIVO != NULL){ \
																				SI32 valVerif = K_NBGESTORANIMACIONES_VALOR_VERIFICACION_BIN; \
																				FLUJOARCHIVO->escribir(&valVerif, sizeof(valVerif), 1, FLUJOARCHIVO); \
																			}

#define NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(FLUJOARCHIVO, BOOL_DEST)	if(BOOL_DEST) { \
																				SI32 valVerif = 0; \
																				FLUJOARCHIVO->leer(&valVerif, sizeof(valVerif), 1, FLUJOARCHIVO); \
																				BOOL_DEST = BOOL_DEST && (valVerif==K_NBGESTORANIMACIONES_VALOR_VERIFICACION_BIN); \
																				NBASSERT(BOOL_DEST) \
																			}

//Objetos Escena que son generados durante el proceso de carga,
//pero que sus datos deberan ser interpretados hasta el final (sus datos dependen de la lista completa)
//Caso de ejemplo: los sensores, puede que un sensor apunte a un objeto que esta despues que el en la lista.
struct STAnimacionObjEscenaPrecargadoXML {
	AUObjeto* objPrecargado;
	const sNodoXML* nodoDatosXML;
	bool operator==(const STAnimacionObjEscenaPrecargadoXML &otro) const {
		return (objPrecargado==otro.objPrecargado && nodoDatosXML==otro.nodoDatosXML);
	}
	bool operator!=(const STAnimacionObjEscenaPrecargadoXML &otro) const {
		return !(objPrecargado==otro.objPrecargado && nodoDatosXML==otro.nodoDatosXML);
	}
};
	
class NBGestorAnimaciones {
	public:
		static bool					inicializar(bool leerPreacheAnimaciones, bool leerCacheAnimaciones, bool escribirCacheAnimaciones, AUCargadorCuerposI* cargadorCuerpos);
		static void					finalizar();
		static bool					gestorInicializado();
		//Cache binaria
		static bool					leyendoCacheAnimaciones();
		static bool					escribiendoCacheAnimaciones();
		static void					establecerLecturaCacheAnimaciones(bool leerCacheAnimaciones);
		static void					establecerEscrituraCacheAnimaciones(bool escribirCacheAnimaciones);
		//Prefijos de rutas
		static const char*			pefijoRutas();
		static void					establecerPrefijoRutas(const char* prefijoStr, const char* prefijoStrCache);
		//Carga de animaciones
		static AUEscenaAnimacion*	animacionDesdeArchivo(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaVirtualArchivo, ENAnimacion_TipoEncapsulamiento tipoEncapsulamiento = ENAnimacion_TipoEncapsulamiento_UnContenedorNormal);
		static UI16					liberarAnimacionesSinReferencias();
	private:
		static bool					_gestorInicializado;
		static AUCargadorCuerposI*	_cargadorCuerpos;
		static bool					_leerPrecacheAnimaciones;
		static bool					_leerCacheAnimaciones;
		static bool					_escribirCacheAnimaciones;
		static AUCadenaMutable8*	_prefijoRutas;
		static AUCadenaMutable8*	_prefijoRutasCache;
		//Animaciones
		static SI32 				_secuencialIDsAnimaciones;
		static AUArregloMutable* 	_plantillasAnimaciones;
		//Carga de animaciones
		static AUEscenaAnimacion* 	privSubanimacionDesdeNodoXml(const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUAnimacionBibliotecaDef* plantillaAnim, AUArregloNativoMutableP<NBPunto>* bibTraslaciones, AUArregloNativoMutableP<NBTamano>* bibEscalaciones, AUArregloNativoMutableP<float>* bibRotaciones, AUArregloNativoMutableP<NBColor8>* bibColores, ENAnimacion_TipoEncapsulamiento tipoEncapsulamiento);
		static AUEscenaAnimacion*	privAnimacionDesdeFlujoBinario(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaFisicaArchivo, const char* rutaRecurso, const UI32 scaleBase2ToLoad, ENAnimacion_TipoEncapsulamiento tipoEncapsulamiento=ENAnimacion_TipoEncapsulamiento_UnContenedorNormal);
		static AUEscenaAnimacion*	privSubanimacionDesdeFlujoBinario(AUArchivo* archivoBinario, AUAnimacionBibliotecaDef* plantillaAnim, AUArregloNativoMutableP<NBPunto>* bibTraslaciones, AUArregloNativoMutableP<NBTamano>* bibEscalaciones, AUArregloNativoMutableP<float>* bibRotaciones, AUArregloNativoMutableP<NBColor8>* bibColores, ENAnimacion_TipoEncapsulamiento tipoEncapsulamiento);
		//
		static bool					privGuardarAnimacionEnBinario(AUEscenaAnimacion* animacionRaiz, const char* rutaFisicaArchivo);
		static bool					privAgregarBitsSubanimacion(AUArchivo* guardarBitsEn, AUEscenaAnimacion* animacion);
};

#endif

