//
//  NBGestorCuerpos.h
//  AUFramework
//
//  Created by Nicaragua Binary on 21/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef NBGestorCuerpos_H_INCLUIDO
#define NBGestorCuerpos_H_INCLUIDO

#include "AUArregloMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUCadenaMutable.h"
#include "AUTextura.h"
#include "nb/crypto/NBSha1.h"
#include "AUEscenaCuerpo.h"
#include "AUEscenaFigura.h"
#include "AUEscenaLuzEspejo.h"
//
#include "AUCargadorAnimacionesI.h" //Intermediario que permite incluir cuerpos en las animaciones, y animaciones en los cuerpos

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
#include "NBSegmentadorFiguras.h" //Para acceder a figuraEsConvexaContrareloj(...)"
#endif

#define K_NBGESTORCUERPOS_VALOR_VERIFICACION_BIN						1023	//para la verificacion de los datos binarios leidos

#define NBGESTORCUERPOS_ESCRIBIR_VERIFICACION_BIN(FLUJOARCHIVO)			if(FLUJOARCHIVO != NULL){ \
																			SI32 valVerif = K_NBGESTORCUERPOS_VALOR_VERIFICACION_BIN; \
																			FLUJOARCHIVO->escribir(&valVerif, sizeof(valVerif), 1, FLUJOARCHIVO); \
																		}

#define NBGESTORCUERPOS_LEER_VERIFICACION_BIN(FLUJOARCHIVO, BOOL_DEST)	if(BOOL_DEST){ \
																			SI32 valVerif = 0; \
																			FLUJOARCHIVO->leer(&valVerif, sizeof(valVerif), 1, FLUJOARCHIVO); \
																			BOOL_DEST = BOOL_DEST && (valVerif==K_NBGESTORCUERPOS_VALOR_VERIFICACION_BIN); \
																			NBASSERT(BOOL_DEST) \
																		}


//Objetos Escena que son generados durante el proceso de carga,
//pero que sus datos deberan ser interpretados hasta el final (sus datos dependen de la lista completa)
//Caso de ejemplo: los sensores, puede que un sensor apunte a un objeto que esta despues que el en la lista.
struct STCuerpoObjEscenaPrecargadoXML {
	AUObjeto* objPrecargado;
	const sNodoXML* nodoDatosXML;
	bool operator==(const STCuerpoObjEscenaPrecargadoXML &otro) const {
		return (objPrecargado==otro.objPrecargado && nodoDatosXML==otro.nodoDatosXML);
	}
	bool operator!=(const STCuerpoObjEscenaPrecargadoXML &otro) const {
		return !(objPrecargado==otro.objPrecargado && nodoDatosXML==otro.nodoDatosXML);
	}
};
	
class NBGestorCuerpos {
	public:
		static void												inicializar(bool leerPrecacheCuerpos, bool leerCacheCuerpos, bool escribirCacheCuerpos, AUCargadorAnimacionesI* cargadorAnimaciones);
		static void												finalizar();
		static bool												gestorInicializado();
		//Prefijos de rutas
		static const char*										pefijoRutas();
		static void												establecerPrefijoRutas(const char* prefijoStr, const char* prefijoStrCache);
		//
		static UI16												liberarPlantillasSinReferencias();
		static AUCuerpoDef*										definicionCuerpoDesdeArchivo(const char* rutaCuerpo, bool buscarEnBibliotecaLocal=true);
		static AUEscenaCuerpo*									cuerpoDesdeArchivo(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaCuerpo, bool buscarEnBibliotecaLocal=true);
	private:
		static bool												_gestorInicializado;
		static AUCargadorAnimacionesI*							_cargadorAnimaciones;
		//USO DE LA CACHE
		static bool												_leerPrecacheCuerpos;
		static bool												_leerCacheCuerpos;
		static bool												_escribirCacheCuerpos;
		//PREFIJO DE RUTAS
		static AUCadenaMutable8*								_prefijoRutas;
		static AUCadenaMutable8*								_prefijoRutasCache;
		//
		static AUArregloMutable*								_plantillasCuerpos;
		//
		//static void											privInicializarRegistroPlantilla(AUCuerpoDef* plantilla);
		//static void											privFinalizarRegistroPlantilla(AUCuerpoDef* plantilla);
		static bool												privPuntoEstaDentroAlgunaFigura(float x, float y, AUArreglo* arregloFiguras);
		static bool												privCargarPlantillaCuerpoDesdeXml(AUCuerpoDef* nuevaPlantilla, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		static AUCuerpoDef*										privCargarPlantillaCuerpoDesdeBinario(const char* rutaArchivo, const char* nombreRecurso);
		static bool												privGuardarPlantillaCuerpoEnBinario(AUCuerpoDef* plantilla, const char* rutaArchivo);
};

#endif
