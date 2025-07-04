//
//  AUEscenaPolea.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 10/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaPolea.h"

AUEscenaPolea::AUEscenaPolea(AUTextura* texPoleaA, AUTextura* texPoleaB, AUTextura* texSegmentosCuerda, /*b2PulleyJoint*/void* unionPolea, ENCuerdaTipo tipoCuerda, float superposicionSegmentos, float masaSegmentos, float factorEstiramiento, float k2, float k3, float factorLongitud, float segsCortaCuerda) : AUEscenaContenedor() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPolea::AUEscenaPolea")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaPolea")
	_unionPolea			= unionPolea; NBASSERT(_unionPolea != NULL)
	_factorLongitud		= factorLongitud; if(_factorLongitud<0.75f) _factorLongitud = 0.75f;
	_segsCortaCuerda	= segsCortaCuerda;
	NBTamano tamPoleaA	= texPoleaA->tamanoHD();
	NBTamano tamPoleaB	= texPoleaB->tamanoHD();
	_poleaA				= new AUEscenaSprite(texPoleaA);
	_poleaB				= new AUEscenaSprite(texPoleaB);
	float diamExtPoleaA	= (tamPoleaA.ancho<tamPoleaA.alto?tamPoleaA.ancho:tamPoleaA.alto);
	float diamExtPoleaB	= (tamPoleaB.ancho<tamPoleaB.alto?tamPoleaB.ancho:tamPoleaB.alto);
	_radioPoleaA		= diamExtPoleaA / 3.0f; //la mitad de radio se utiliza para ocultar la cuerda
	_radioPoleaB		= diamExtPoleaB / 3.0f; //la mitad de radio se utiliza para ocultar la cuerda
	_longitudPoleaA		= PI * _radioPoleaA * 2.0f;
	_longitudPoleaB		= PI * _radioPoleaB * 2.0f;
	_escuchadorPolea	= NULL;
	//
	float longitudPolA	= 0.0f;
	float longitudPolB	= 0.0f;
	float longitudPuente = 0.0f;
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	{
		b2PulleyJoint* unionPolea = (b2PulleyJoint*)_unionPolea;
		b2Vec2 posAnclaA	= unionPolea->GetGroundAnchorA();
		b2Vec2 posAnclaB	= unionPolea->GetGroundAnchorB();
		posAnclaA			*= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
		posAnclaB			*= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
		//
		longitudPolA	= unionPolea->GetLengthA() * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
		longitudPolB	= unionPolea->GetLengthB() * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
		longitudPuente	= NBPUNTO_DISTANCIA(posAnclaA.x, posAnclaA.y, posAnclaB.x, posAnclaB.y);
	}
#	endif
	//
	_cuerdasInicializadas	= false;
	_cuerdaPoleaA		= new AUEscenaCuerda(tipoCuerda, texSegmentosCuerda, longitudPolA + longitudPolB, masaSegmentos, superposicionSegmentos);
	_cuerdaPoleaB		= new AUEscenaCuerda(tipoCuerda, texSegmentosCuerda, longitudPolA + longitudPolB, masaSegmentos, superposicionSegmentos);
	_cuerdaEntrePoleas	= new AUEscenaCuerda(tipoCuerda, texSegmentosCuerda, longitudPolA + longitudPuente + longitudPolB, masaSegmentos, superposicionSegmentos);
	_cuerdaPoleaA->establecerFactorEstiramiento(factorEstiramiento);
	_cuerdaPoleaA->establecerK2(k2);
	_cuerdaPoleaA->establecerK3(k3);
	_cuerdaPoleaA->establecerSegsNecesariosParaCorte(segsCortaCuerda);
	_cuerdaPoleaA->establecerEscuchadorCuerda(this);
	_cuerdaPoleaB->establecerFactorEstiramiento(factorEstiramiento);
	_cuerdaPoleaB->establecerK2(k2);
	_cuerdaPoleaB->establecerK3(k3);
	_cuerdaPoleaB->establecerSegsNecesariosParaCorte(segsCortaCuerda);
	_cuerdaPoleaB->establecerEscuchadorCuerda(this);
	_cuerdaEntrePoleas->establecerFactorEstiramiento(factorEstiramiento);
	_cuerdaEntrePoleas->establecerK2(k2);
	_cuerdaEntrePoleas->establecerK3(k3);
	_cuerdaEntrePoleas->establecerSegsNecesariosParaCorte(segsCortaCuerda);
	_cuerdaEntrePoleas->establecerEscuchadorCuerda(this);
	privOrganizarObjetosSegunUnion();
	//
	this->agregarObjetoEscena(_cuerdaPoleaA);
	this->agregarObjetoEscena(_cuerdaPoleaB);
	this->agregarObjetoEscena(_cuerdaEntrePoleas);
	this->agregarObjetoEscena(_poleaA);
	this->agregarObjetoEscena(_poleaB);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaPolea::~AUEscenaPolea(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPolea::~AUEscenaPolea")
	if(_poleaA != NULL) _poleaA->liberar(NB_RETENEDOR_THIS); _poleaA = NULL;
	if(_poleaB != NULL) _poleaB->liberar(NB_RETENEDOR_THIS); _poleaB = NULL;
	if(_cuerdaPoleaA != NULL) _cuerdaPoleaA->liberar(NB_RETENEDOR_THIS); _cuerdaPoleaA = NULL;
	if(_cuerdaPoleaB != NULL) _cuerdaPoleaB->liberar(NB_RETENEDOR_THIS); _cuerdaPoleaB= NULL;
	if(_cuerdaEntrePoleas != NULL) _cuerdaEntrePoleas->liberar(NB_RETENEDOR_THIS); _cuerdaEntrePoleas = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

/*b2PulleyJoint*/void* AUEscenaPolea::unionPolea(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPolea::unionPolea")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _unionPolea;
}

IEscuchadorPolea* AUEscenaPolea::escuchadorPolea(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPolea::escuchadorPolea")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _escuchadorPolea;
}

void AUEscenaPolea::establecerUnionPolea(/*b2PulleyJoint*/void* unionPolea){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPolea::establecerUnionPolea")
	_unionPolea = unionPolea;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPolea::establecerEscuchadorPolea(IEscuchadorPolea* escuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPolea::establecerEscuchadorPolea")
	_escuchadorPolea = escuchador;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaPolea::privOrganizarObjetosSegunUnion(){
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	b2PulleyJoint* unionPolea = (b2PulleyJoint*)_unionPolea;
	b2Vec2 posCuerpoAF	= unionPolea->GetAnchorA();
	b2Vec2 posCuerpoBF	= unionPolea->GetAnchorB();
	b2Vec2 posAnclaAF	= unionPolea->GetGroundAnchorA();
	b2Vec2 posAnclaBF	= unionPolea->GetGroundAnchorB();
	b2Vec2 posCuerpoA	= posCuerpoAF; posCuerpoA *= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	b2Vec2 posCuerpoB	= posCuerpoBF; posCuerpoB *= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	b2Vec2 posAnclaA	= posAnclaAF; posAnclaA *= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	b2Vec2 posAnclaB	= posAnclaBF; posAnclaB *= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	float longitudA		= unionPolea->GetLengthA() * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	float longitudB		= unionPolea->GetLengthB() * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	_poleaA->establecerTraslacion(posAnclaA.x+(posAnclaA.x<posAnclaB.x?_radioPoleaA:-_radioPoleaA), posAnclaA.y-_radioPoleaA);
	_poleaB->establecerTraslacion(posAnclaB.x+(posAnclaB.x<posAnclaA.x?_radioPoleaB:-_radioPoleaB), posAnclaB.y-_radioPoleaB);
	NBASSERT(_longitudPoleaA>0.0f) _poleaA->establecerRotacion(longitudA/_longitudPoleaA*360.0f);
	NBASSERT(_longitudPoleaB>0.0f) _poleaB->establecerRotacion(longitudB/_longitudPoleaB*-360.0f);
	//PRINTF_INFO("Longitud de poleaA: %f, rotacion(%f)\n", _longitudPoleaA, _poleaA->rotacion());
	//PRINTF_INFO("Longitud de poleaB: %f, rotacion(%f)\n", _longitudPoleaB, _poleaB->rotacion());
	float longitudCuerdaA		= longitudA - 8.0f; //menos un margen para estirar la cuerda
	float longitudCuerdaB		= longitudB - 8.0f; //menos un margen para estirar la cuerda
	float distanciaEntreAnclas	= NBPUNTO_DISTANCIA(posAnclaA.x, posAnclaA.y, posAnclaB.x, posAnclaB.y);
	if(longitudCuerdaA<8.0f) longitudCuerdaA = 8.0f;
	if(longitudCuerdaB<8.0f) longitudCuerdaB = 8.0f;
	if(distanciaEntreAnclas<8.0f) distanciaEntreAnclas = 8.0f;
	_cuerdaPoleaA->establecerLongitudCuerdaHabilitada(longitudCuerdaA * _factorLongitud, 0.0f);
	_cuerdaPoleaB->establecerLongitudCuerdaHabilitada(longitudCuerdaB * _factorLongitud, 0.0f);
	_cuerdaEntrePoleas->establecerLongitudCuerdaHabilitada(distanciaEntreAnclas * _factorLongitud, longitudCuerdaB * _factorLongitud);
	if(!_cuerdasInicializadas){
		_cuerdasInicializadas = true;
		_cuerdaPoleaA->inicializarCuerdaSegunLinea(posCuerpoAF.x, posCuerpoAF.y, posAnclaAF.x, posAnclaAF.y);
		_cuerdaPoleaB->inicializarCuerdaSegunLinea(posCuerpoBF.x, posCuerpoBF.y, posAnclaBF.x, posAnclaBF.y);
		_cuerdaEntrePoleas->inicializarCuerdaSegunLinea(posAnclaBF.x, posAnclaBF.y, posAnclaAF.x, posAnclaAF.y);
	}
	_cuerdaPoleaA->habilitarAnclaCuerdaIni(posCuerpoA.x, posCuerpoA.y);
	_cuerdaPoleaA->habilitarAnclaCuerdaFin(posAnclaA.x, posAnclaA.y);
	_cuerdaPoleaB->habilitarAnclaCuerdaIni(posCuerpoB.x, posCuerpoB.y);
	_cuerdaPoleaB->habilitarAnclaCuerdaFin(posAnclaB.x, posAnclaB.y);
	_cuerdaEntrePoleas->habilitarAnclaCuerdaIni(posAnclaB.x, posAnclaB.y);
	_cuerdaEntrePoleas->habilitarAnclaCuerdaFin(posAnclaA.x, posAnclaA.y);
#	endif
}

void AUEscenaPolea::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPolea::agregadoEnEscena")
	NBGestorAnimadores::agregarAnimador(this, this);
	AUEscenaContenedor::agregadoEnEscena();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPolea::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPolea::quitandoDeEscena")
	AUEscenaContenedor::quitandoDeEscena();
	NBGestorAnimadores::quitarAnimador(this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPolea::cuerdaCortada(AUEscenaCuerda* objCuerda, UI16 iSegmentoCorte){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPolea::cuerdaCortada")
	if(_escuchadorPolea != NULL){
		_escuchadorPolea->poleaCortada(this, objCuerda, iSegmentoCorte);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPolea::procesarCuerdaCortada(AUEscenaCuerda* objCuerda, UI16 iSegmentoCorte, AUEscenaContenedor* cuerpoA, AUEscenaContenedor* cuerpoB,  AUArregloMutable* arregloParaCuerdasHuerfanas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPolea::procesarCuerdaCortada")
	NBASSERT(objCuerda==_cuerdaPoleaA || objCuerda==_cuerdaPoleaB || objCuerda==_cuerdaEntrePoleas)
	NBASSERT(_unionPolea != NULL)
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	b2PulleyJoint* unionPolea = (b2PulleyJoint*)_unionPolea;
	NBPunto posLocalPrimerVertA	= _cuerdaPoleaA->posLocalPrimerVerticeHabilitado();
	NBPunto posLocalPrimerVertB	= _cuerdaPoleaB->posLocalUltimoVerticeHabilitado();
	b2Vec2 posAEscena			= unionPolea->GetAnchorA(); posAEscena *= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	b2Vec2 posBEscena			= unionPolea->GetAnchorB(); posBEscena *= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	NBPunto posLocalA			= cuerpoA->coordenadaEscenaALocal(posAEscena.x, posAEscena.y);
	NBPunto posLocalB			= cuerpoB->coordenadaEscenaALocal(posBEscena.x, posBEscena.y);
	//
	_cuerdaPoleaA->habilitarAnclaCuerdaIni(posLocalPrimerVertA.x, posLocalPrimerVertA.y);
	_cuerdaPoleaA->deshabilitarAnclaCuerdaFin();
	_cuerdaPoleaA->establecerTraslacion(posLocalA.x-posLocalPrimerVertA.x, posLocalA.y-posLocalPrimerVertA.y);
	cuerpoA->agregarObjetoEscena(_cuerdaPoleaA);
	_cuerdaPoleaB->habilitarAnclaCuerdaIni(posLocalPrimerVertB.x, posLocalPrimerVertB.y);
	_cuerdaPoleaB->deshabilitarAnclaCuerdaFin();
	_cuerdaPoleaB->establecerTraslacion(posLocalB.x-posLocalPrimerVertB.x, posLocalB.y-posLocalPrimerVertB.y);
	cuerpoB->agregarObjetoEscena(_cuerdaPoleaB);
	//
	if(objCuerda==_cuerdaPoleaA){
		_cuerdaPoleaB->agregarSegmentosDeCuerda(_cuerdaEntrePoleas, _cuerdaEntrePoleas->indicePrimerSegmentoHabilitado(), _cuerdaEntrePoleas->indiceUltimoSegmentoHabilitado());
		_cuerdaPoleaB->agregarSegmentosDeCuerdaOrdenInverso(_cuerdaPoleaA, iSegmentoCorte, _cuerdaPoleaA->indiceUltimoSegmentoHabilitado());
		_cuerdaPoleaA->quitarSegmentosDespuesDeInclusive(iSegmentoCorte);
	} else if(objCuerda==_cuerdaPoleaB){
		_cuerdaPoleaA->agregarSegmentosDeCuerdaOrdenInverso(_cuerdaEntrePoleas, _cuerdaEntrePoleas->indicePrimerSegmentoHabilitado(), _cuerdaEntrePoleas->indiceUltimoSegmentoHabilitado());
		_cuerdaPoleaA->agregarSegmentosDeCuerdaOrdenInverso(_cuerdaPoleaB, iSegmentoCorte, _cuerdaPoleaB->indiceUltimoSegmentoHabilitado());
		_cuerdaPoleaB->quitarSegmentosDespuesDeInclusive(iSegmentoCorte);
	} else if(objCuerda==_cuerdaEntrePoleas){
		_cuerdaPoleaA->agregarSegmentosDeCuerdaOrdenInverso(_cuerdaEntrePoleas, iSegmentoCorte, _cuerdaEntrePoleas->indiceUltimoSegmentoHabilitado());
		_cuerdaPoleaB->agregarSegmentosDeCuerda(_cuerdaEntrePoleas, _cuerdaEntrePoleas->indicePrimerSegmentoHabilitado(), iSegmentoCorte);
	}
	//
	_cuerdaPoleaA->establecerSegsNecesariosParaCorte(0.0f);
	_cuerdaPoleaB->establecerSegsNecesariosParaCorte(0.0f);
	_cuerdaEntrePoleas->establecerSegsNecesariosParaCorte(0.0f);
	arregloParaCuerdasHuerfanas->agregarElemento(_cuerdaPoleaA);
	arregloParaCuerdasHuerfanas->agregarElemento(_cuerdaPoleaB);
	this->quitarObjetoEscena(_cuerdaEntrePoleas); _cuerdaEntrePoleas->liberar(NB_RETENEDOR_THIS); _cuerdaEntrePoleas = NULL;
#	endif
	_unionPolea = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPolea::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPolea::tickAnimacion")
	NBASSERT(this->idEscena!=-1)
	if(this->_dormido){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3() }
	if(_unionPolea != NULL) privOrganizarObjetosSegunUnion();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaPolea, AUEscenaContenedor)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaPolea, "AUEscenaPolea", AUEscenaContenedor)
AUOBJMETODOS_CLONAR_NULL(AUEscenaPolea)





