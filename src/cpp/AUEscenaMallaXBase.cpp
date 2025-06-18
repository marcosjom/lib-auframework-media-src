//
//  AUEscenaMalla3.cpp
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 06/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaMallaXBase.h"

AUEscenaMallaXBase::AUEscenaMallaXBase() : AUEscenaContenedor() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMallaXBase::AUEscenaMallaXBase")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMallaXBase")
	NBCAJAAABB_INICIALIZAR(_cajaActualMalla);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaMallaXBase::~AUEscenaMallaXBase(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMallaXBase::~AUEscenaMallaXBase")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

NBCajaAABB AUEscenaMallaXBase::cajaMarcoMalla(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMallaXBase::cajaMarcoMalla")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _cajaActualMalla;
}

void AUEscenaMallaXBase::establecerCajaMarcoMalla(const NBCajaAABB &cajaMarco){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMallaXBase::establecerCajaMarcoMalla")
	_cajaActualMalla = cajaMarco;
	this->actualizarPresentacionMalla(_cajaActualMalla);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaMallaXBase::establecerCajaMarcoMalla(float xMin, float yMin, float xMax, float yMax){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMalla3::establecerCajaMarcoMalla")
	_cajaActualMalla.xMin		= xMin;
	_cajaActualMalla.xMax		= xMax;
	_cajaActualMalla.yMin		= yMin;
	_cajaActualMalla.yMax		= yMax;
	this->actualizarPresentacionMalla(_cajaActualMalla);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaMallaXBase, AUEscenaContenedor)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaMallaXBase, "AUEscenaMallaXBase", AUEscenaContenedor)
AUOBJMETODOS_CLONAR_NULL(AUEscenaMallaXBase)






