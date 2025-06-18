//
//  AUCargadorCuerpos.h
//  AUFramework
//
//  Created by Nicaragua Binary on 17/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUFramework_AUCargadorCuerpos_h
#define AUFramework_AUCargadorCuerpos_h

#include "AUObjeto.h"
#include "AUCargadorCuerposI.h"
#include "AUEscenaObjeto.h"
#include "NBGestorCuerpos.h"

//Clase intermediaria para la carga de animaciones.
//Necesaria para poder implementar Animaciones en Cuerpos y Cuerpos en Animaciones.

class AUCargadorCuerpos : public AUCargadorCuerposI {
	public:
		AUCargadorCuerpos();
		virtual ~AUCargadorCuerpos();
		//
		virtual AUEscenaObjeto*		cargarCuerpo(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaCuerpo);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
};

#endif
