//
//  AUCargadorCuerposI.h
//  AUFramework
//
//  Created by Nicaragua Binary on 17/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUFramework_AUCargadorCuerposI_h
#define AUFramework_AUCargadorCuerposI_h

#include "AUObjeto.h"
#include "AUEscenaObjeto.h"

//Clase intermediaria para la carga de animaciones.
//Necesaria para poder implementar Animaciones en Cuerpos y Cuerpos en Animaciones.

class AUCargadorCuerposI : public AUObjeto {
	public:
		AUCargadorCuerposI() : AUObjeto() {
			//
		}
		virtual ~AUCargadorCuerposI(){
			//
		}
		//
		virtual AUEscenaObjeto*		cargarCuerpo(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaCuerpo) = 0;
		//
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_UNICLASE(AUCargadorCuerposI)
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_UNICLASE(AUCargadorCuerposI, "AUCargadorCuerposI")
		virtual AUObjeto*	clonar(ENMemoriaTipo memoriaResidencia, ENClonadoTipo tipoClonado){
			return NULL;
		}
};

#endif

