//
//  AUCargadorCuerpos.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 17/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUCargadorCuerpos.h"

AUCargadorCuerpos::AUCargadorCuerpos() : AUCargadorCuerposI() {
	//
}

AUCargadorCuerpos::~AUCargadorCuerpos(){
	//
}

//

AUEscenaObjeto* AUCargadorCuerpos::cargarCuerpo(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaCuerpo) {
	return NBGestorCuerpos::cuerpoDesdeArchivo(tipoMemoriaResidencia, rutaCuerpo);
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUCargadorCuerpos, AUCargadorCuerposI)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUCargadorCuerpos, "AUCargadorCuerpos", AUCargadorCuerposI)


AUObjeto* AUCargadorCuerpos::clonar(ENMemoriaTipo memoriaResidencia, ENClonadoTipo tipoClonado){
	return NULL;
}





