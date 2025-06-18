//
//  AUEscenaCuerpoI.cpp
//  Gameplay_iOS
//
//  Created by Marcos Ortega on 25/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaCuerpoI.h"

AUEscenaCuerpoI::AUEscenaCuerpoI() : AUEscenaContenedor() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::AUEscenaCuerpoI")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaCuerpoI")
	_propsCreacionCuerpo.tipoCuerpo			= ENCuerpoTipo_Dinamico;
	_propsCreacionCuerpo.tipoAnimacion		= ENAnimFisicaTipo_SeguirCuerpo;
	_propsCreacionCuerpo.mascaraProps		= AUCUERPODEF_MASCARA_ACTIVO | AUCUERPODEF_MASCARA_PERMITIR_DORMIR;
	_propsCreacionCuerpo.filtroCuerpos		= 0;
	_propsCreacionCuerpo.factorGravedad		= 1.0f;
	_propsCreacionCuerpo.datoUsuario		= NULL;
	//
	_propsMultiplicadores.multDensidad		= 1.0f;
	_propsMultiplicadores.multFriccion		= 1.0f;
	_propsMultiplicadores.multRestitucion	= 1.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaCuerpoI::AUEscenaCuerpoI(AUEscenaContenedor* contendorPlantilla) : AUEscenaContenedor(contendorPlantilla) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::AUEscenaCuerpoI")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaCuerpoI")
	_propsCreacionCuerpo.tipoCuerpo			= ENCuerpoTipo_Dinamico;
	_propsCreacionCuerpo.tipoAnimacion		= ENAnimFisicaTipo_SeguirCuerpo;
	_propsCreacionCuerpo.mascaraProps		= AUCUERPODEF_MASCARA_ACTIVO | AUCUERPODEF_MASCARA_PERMITIR_DORMIR;
	_propsCreacionCuerpo.filtroCuerpos		= 0;
	_propsCreacionCuerpo.factorGravedad		= 1.0f;
	_propsCreacionCuerpo.datoUsuario		= NULL;
	//
	_propsMultiplicadores.multDensidad		= 1.0f;
	_propsMultiplicadores.multFriccion		= 1.0f;
	_propsMultiplicadores.multRestitucion	= 1.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaCuerpoI::~AUEscenaCuerpoI(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::~AUEscenaCuerpoI")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

STCuerpoFisicaDef AUEscenaCuerpoI::definicionCreacionCuerpo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::definicionCreacionCuerpo")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _propsCreacionCuerpo;
}

STCuerpoMultDef AUEscenaCuerpoI::multiplicadoresPropsCuerpo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::multiplicadoresPropsCuerpo")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _propsMultiplicadores;
}

float AUEscenaCuerpoI::multiplicadorDensidad(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::multiplicadorDensidad")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _propsMultiplicadores.multDensidad;
}

float AUEscenaCuerpoI::multiplicadorFriccion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::multiplicadorFriccion")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _propsMultiplicadores.multFriccion;
}

float AUEscenaCuerpoI::multiplicadorRestitucion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::multiplicadorRestitucion")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _propsMultiplicadores.multRestitucion;
}

void AUEscenaCuerpoI::establecerCuerpoTipo(ENCuerpoTipo tipoCuerpo, ENAnimFisicaTipo tipoAnimacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::establecerCuerpoTipo")
	_propsCreacionCuerpo.tipoCuerpo		= tipoCuerpo;
	_propsCreacionCuerpo.tipoAnimacion	= tipoAnimacion;
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	b2Body* cpo = (b2Body*)this->cuerpo();
	if(cpo != NULL){
		cpo->SetType(tipoCuerpo == ENCuerpoTipo_Kinematico ? b2_kinematicBody : tipoCuerpo == ENCuerpoTipo_Estatico ? b2_staticBody : b2_dynamicBody);
		NBGestorFisica::actualizarUnion(this, cpo, tipoAnimacion);
	}
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoI::establecerCreacionCuerpoTipo(ENCuerpoTipo tipoCuerpo, ENAnimFisicaTipo tipoAnimacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::establecerCreacionCuerpoTipo")
	_propsCreacionCuerpo.tipoCuerpo		= tipoCuerpo;
	_propsCreacionCuerpo.tipoAnimacion	= tipoAnimacion;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoI::establecerCreacionCuerpoMascara(UI8 mascaraProps){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::establecerCreacionCuerpoMascara")
	_propsCreacionCuerpo.mascaraProps	= mascaraProps;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoI::establecerCreacionCuerpoFiltro(SI16 filtroCuerpo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::establecerCreacionCuerpoFiltro")
	_propsCreacionCuerpo.filtroCuerpos	= filtroCuerpo;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoI::establecerCreacionCuerpoFactorGravedad(float factorGravedad){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::establecerCreacionCuerpoFactorGravedad")
	_propsCreacionCuerpo.factorGravedad	= factorGravedad;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoI::establecerCreacionCuerpoDatoUsuario(void* datoUsuario){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::establecerCreacionCuerpoDatoUsuario")
	_propsCreacionCuerpo.datoUsuario	= datoUsuario;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoI::establecerMultiplicadoresPropsCuerpo(const STCuerpoMultDef &multiplicadores){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::establecerMultiplicadoresPropsCuerpo")
	NBASSERT(multiplicadores.multDensidad>=0.0f)
	NBASSERT(multiplicadores.multFriccion>=0.0f)
	NBASSERT(multiplicadores.multRestitucion>=0.0f)
	_propsMultiplicadores	= multiplicadores;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoI::establecerMultiplicadorDensidad(const float multiplicador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::establecerMultiplicadorDensidad")
	NBASSERT(multiplicador>=0.0f)
	_propsMultiplicadores.multDensidad	= multiplicador;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoI::establecerMultiplicadorFriccion(const float multiplicador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::establecerMultiplicadorDensidad")
	NBASSERT(multiplicador>=0.0f)
	_propsMultiplicadores.multFriccion	= multiplicador;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoI::establecerMultiplicadorRestitucion(const float multiplicador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::establecerMultiplicadorDensidad")
	NBASSERT(multiplicador>=0.0f)
	_propsMultiplicadores.multFriccion	= multiplicador;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool AUEscenaCuerpoI::privGenerarParticula(AUTextura* textura, const NBPunto &posEscena, const NBPunto &vectorImpacto, AUEscenaGrupoParticulas* grupoParticulasDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoI::privGenerarParticula")
	bool particulaGenerada = false;
	NBPunto difDir; NBPUNTO_ESTABLECER(difDir, ((rand() % 150) - 150) / 100.0f, ((rand() % 150) - 150) / 100.0f)
	grupoParticulasDestino->agregarParticulaLibre(textura, 2.0f, 0.5f, posEscena.x, posEscena.y, 1.0f, GRADOS_A_RADIANES((rand()%180)-90), vectorImpacto.x*difDir.x*30.0f, vectorImpacto.y*difDir.y*30.0f, GRADOS_A_RADIANES((rand()%90)-45));
	particulaGenerada = true;
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return particulaGenerada;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaCuerpoI, AUEscenaContenedor)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaCuerpoI, "AUEscenaCuerpoI", AUEscenaContenedor)
AUOBJMETODOS_CLONAR_NULL(AUEscenaCuerpoI)









