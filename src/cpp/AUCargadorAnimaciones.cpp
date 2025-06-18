//
//  AUCargadorAnimaciones.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 17/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUCargadorAnimaciones.h"

AUCargadorAnimaciones::AUCargadorAnimaciones() : AUCargadorAnimacionesI() {
	//
}

AUCargadorAnimaciones::~AUCargadorAnimaciones(){
	//
}

//

AUEscenaObjeto* AUCargadorAnimaciones::cargarAnimacion(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaAnimacion) {
	return NBGestorAnimaciones::animacionDesdeArchivo(tipoMemoriaResidencia, rutaAnimacion);
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUCargadorAnimaciones, AUCargadorAnimacionesI)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUCargadorAnimaciones, "AUCargadorAnimaciones", AUCargadorAnimacionesI)

AUObjeto* AUCargadorAnimaciones::clonar(ENMemoriaTipo memoriaResidencia, ENClonadoTipo tipoClonado){
	return NULL;
}





