//
//  AUEscenaMalla3.cpp
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 06/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaMalla3.h"

AUEscenaMalla3::AUEscenaMalla3(const char* prefijoRutaTexturas3) : AUEscenaMallaXBase() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMalla3::AUEscenaMalla3")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMalla3")
	_mallaFondo				= NULL;
	AUCadenaMutable8* rutaPNG = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	rutaPNG->establecer(prefijoRutaTexturas3); rutaPNG->agregar("1.png"); _texturasMarco[0]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[0]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas3); rutaPNG->agregar("2.png"); _texturasMarco[1]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[1]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas3); rutaPNG->agregar("3.png"); _texturasMarco[2]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[2]->retener(NB_RETENEDOR_THIS);
	rutaPNG->liberar(NB_RETENEDOR_THIS);
	establecerCajaMarcoMalla(cajaMinima());
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaMalla3::~AUEscenaMalla3(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMalla3::~AUEscenaMalla3")
	if(_mallaFondo != NULL){
		_mallaFondo->liberar(NB_RETENEDOR_THIS);
		_mallaFondo = NULL;
	}
	if(_texturasMarco[0] != NULL) _texturasMarco[0]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[1] != NULL) _texturasMarco[1]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[2] != NULL) _texturasMarco[2]->liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

NBCajaAABB AUEscenaMalla3::cajaMinima(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMalla3::cajaMinima")
	NBTamano tamanoMinimo;
	tamanoMinimo.ancho = _texturasMarco[0]->tamanoHD().ancho /*+ _texturasMarco[1]->tamanoHD().ancho*/ + _texturasMarco[2]->tamanoHD().ancho;
	tamanoMinimo.alto = _texturasMarco[0]->tamanoHD().alto;
	if(tamanoMinimo.alto<_texturasMarco[1]->tamanoHD().alto) tamanoMinimo.alto = _texturasMarco[1]->tamanoHD().alto;
	if(tamanoMinimo.alto<_texturasMarco[2]->tamanoHD().alto) tamanoMinimo.alto = _texturasMarco[2]->tamanoHD().alto;
	NBCajaAABB cajaMinima;
	NBCAJAAABB_INICIALIZAR(cajaMinima);
	NBCAJAAABB_ENVOLVER_PUNTO(cajaMinima, tamanoMinimo.ancho/-2.0f, tamanoMinimo.alto/-2.0f);
	NBCAJAAABB_ENVOLVER_PUNTO(cajaMinima, tamanoMinimo.ancho/2.0f, tamanoMinimo.alto/2.0f);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cajaMinima;
}

void AUEscenaMalla3::actualizarPresentacionMalla(const NBCajaAABB &cajaMarco){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMalla3::actualizarPresentacionMalla")
	float xMin	= cajaMarco.xMin;
	//float yMin	= cajaMarco.yMin;
	float xMax	= cajaMarco.xMax;
	float yMax	= cajaMarco.yMax;
	//NBTamano tamCajaMarco; NBTAMANO_ESTABLECER(tamCajaMarco, xMax-xMin, yMax-yMin);
	AUArregloNativoMutableP<NBPuntoColor>* puntosLocales	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBPuntoColor>(); NB_DEFINE_NOMBRE_PUNTERO(puntosLocales, "AUEscenaMalla3::puntosLocales");
	NBPuntoColor punto; NBCOLOR_ESTABLECER(punto.color, 255, 255, 255, 255)
	NBPUNTO_ESTABLECER(punto.coordenada, xMin, yMax); puntosLocales->agregarElemento(punto);
	NBPUNTO_ESTABLECER(punto.coordenada, xMin+_texturasMarco[0]->tamanoHD().ancho, yMax); puntosLocales->agregarElemento(punto);
	NBPUNTO_ESTABLECER(punto.coordenada, xMax-_texturasMarco[2]->tamanoHD().ancho, yMax); puntosLocales->agregarElemento(punto);
	NBPUNTO_ESTABLECER(punto.coordenada, xMax, yMax); puntosLocales->agregarElemento(punto);
	//
	NBPUNTO_ESTABLECER(punto.coordenada, xMin, yMax-_texturasMarco[0]->tamanoHD().alto); puntosLocales->agregarElemento(punto);
	NBPUNTO_ESTABLECER(punto.coordenada, xMin+_texturasMarco[0]->tamanoHD().ancho, yMax-_texturasMarco[0]->tamanoHD().alto); puntosLocales->agregarElemento(punto);
	NBPUNTO_ESTABLECER(punto.coordenada, xMax-_texturasMarco[2]->tamanoHD().ancho, yMax-_texturasMarco[0]->tamanoHD().alto); puntosLocales->agregarElemento(punto);
	NBPUNTO_ESTABLECER(punto.coordenada, xMax, yMax-_texturasMarco[0]->tamanoHD().alto); puntosLocales->agregarElemento(punto);
	//
	if(_mallaFondo == NULL){
		AUArregloNativoMutableP<AUTextura*>* texturas		= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<AUTextura*>();	 NB_DEFINE_NOMBRE_PUNTERO(texturas, "AUEscenaMalla3::texturas");
		AUArregloNativoMutableP<NBRectangulo>* areasTex		= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBRectangulo>();	 NB_DEFINE_NOMBRE_PUNTERO(areasTex, "AUEscenaMalla3::areasTex");
		texturas->agregarElemento(_texturasMarco[0]);
		texturas->agregarElemento(_texturasMarco[1]);
		texturas->agregarElemento(_texturasMarco[2]);
		NBRectangulo areaTextua; 
		areaTextua.x		= 0.0f; 
		areaTextua.y		= 0.0f; 
		areaTextua.ancho	= 1.0f; 
		areaTextua.alto		= 1.0f;
		areasTex->agregarElemento(areaTextua);
		_mallaFondo = new(this) AUEscenaMalla(3, 1, puntosLocales, texturas, areasTex);
		this->agregarObjetoEscena(_mallaFondo);
		texturas->liberar(NB_RETENEDOR_THIS);
		areasTex->liberar(NB_RETENEDOR_THIS);
	} else {
		_mallaFondo->establecerVerticesLocales(puntosLocales);
	}
	puntosLocales->liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaMalla3, AUEscenaMallaXBase)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaMalla3, "AUEscenaMalla3", AUEscenaMallaXBase)
AUOBJMETODOS_CLONAR_NULL(AUEscenaMalla3)






