//
//  AUCargadorAnimacionesI.h
//  AUFramework
//
//  Created by Nicaragua Binary on 17/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUFramework_AUCargadorAnimacionesI_h
#define AUFramework_AUCargadorAnimacionesI_h

#include "AUObjeto.h"
#include "AUEscenaObjeto.h"

//Clase intermediaria para la carga de animaciones.
//Necesaria para poder implementar Animaciones en Cuerpos y Cuerpos en Animaciones.

class AUCargadorAnimacionesI : public AUObjeto {
	public:
		AUCargadorAnimacionesI() : AUObjeto() {
			//
		}
		virtual ~AUCargadorAnimacionesI(){
			//
		}
		//
		virtual AUEscenaObjeto*		cargarAnimacion(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaAnimacion) = 0;
		//
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_UNICLASE(AUCargadorAnimacionesI)
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_UNICLASE(AUCargadorAnimacionesI, "AUCargadorAnimacionesI")
		virtual AUObjeto*	clonar(ENMemoriaTipo memoriaResidencia, ENClonadoTipo tipoClonado){
			return NULL;
		}
};

#endif

