//
//  AUCargadorAnimaciones.h
//  AUFramework
//
//  Created by Nicaragua Binary on 17/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUFramework_AUCargadorAnimaciones_h
#define AUFramework_AUCargadorAnimaciones_h

#include "AUObjeto.h"
#include "AUCargadorAnimacionesI.h"
#include "AUEscenaObjeto.h"
#include "NBGestorAnimaciones.h"

//Clase intermediaria para la carga de animaciones.
//Necesaria para poder implementar Animaciones en Cuerpos y Cuerpos en Animaciones.

class AUCargadorAnimaciones : public AUCargadorAnimacionesI {
	public:
		AUCargadorAnimaciones();
		virtual ~AUCargadorAnimaciones();
		//
		virtual AUEscenaObjeto*		cargarAnimacion(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaAnimacion);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
};

#endif
