//
//  AUEscenaSnd.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 28/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaSnd.h"

#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO

AUEscenaSnd::AUEscenaSnd() : AUEscenaObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSnd::AUEscenaSnd")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSnd")
	_iniciarReproduciendo	= false;
	_esEspacial				= false;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSnd::AUEscenaSnd(AUEscenaSnd* otraInstancia) : AUEscenaObjeto(otraInstancia) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSnd::AUEscenaSnd")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSnd")
	_iniciarReproduciendo	= otraInstancia->_iniciarReproduciendo;
	_esEspacial				= otraInstancia->_esEspacial;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSnd::~AUEscenaSnd(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSnd::~AUEscenaSnd")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

bool AUEscenaSnd::iniciarReproduciendo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSnd::iniciarReproduciendo")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _iniciarReproduciendo;
}

bool AUEscenaSnd::esEspacial(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSnd::esEspacial")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _esEspacial;
}

void AUEscenaSnd::establecerIniciarReproduciendo(bool iniciarReproduciendo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSnd::establecerIniciarReproduciendo")
	_iniciarReproduciendo = iniciarReproduciendo;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSnd::establecerEsEspacial(bool esEspacial){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSnd::establecerEsEspacial")
	_esEspacial = esEspacial;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

void AUEscenaSnd::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSnd::agregadoEnEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSnd::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSnd::quitandoDeEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

NBCajaAABB AUEscenaSnd::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSnd::cajaAABBLocalCalculada")
	NBCajaAABB cajaLocal;
	cajaLocal.xMin = -32.0f;
	cajaLocal.yMin = -32.0f;
	cajaLocal.xMax = 32.0f;
	cajaLocal.yMax = 32.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return cajaLocal;
}

PTRfunCmdsGL AUEscenaSnd::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSnd::actualizarModeloGL")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return NULL;
}

//

bool AUEscenaSnd::agregarXmlInternoEn(AUEscenaSnd* sonido, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSnd::agregarXmlInternoEn")
	bool exito = false;
	guardarEn->agregarConFormato("%s  <eOb>\r\n", espaciosBaseIzq);
	exito = AUEscenaObjeto::agregarXmlInternoEn(sonido, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s  </eOb>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s  <iniRep>%d</iniRep>\r\n", espaciosBaseIzq, sonido->_iniciarReproduciendo?1:0);
	guardarEn->agregarConFormato("%s  <espac>%d</espac>\r\n", espaciosBaseIzq, sonido->_esEspacial?1:0);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaSnd::interpretarXmlInterno(AUEscenaSnd* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSnd::interpretarXmlInterno")
	bool exito = false;
	const sNodoXML* nodoObj;	XML_NODO_HIJO(datosXml, nodoObj, "eOb", "escObj", nodoXml, NULL)
	if(nodoObj != NULL){
		if(AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoObj)){
			exito = true;
			cargarEn->_iniciarReproduciendo = datosXml->nodoHijo("iniRep", false, nodoXml);
			cargarEn->_esEspacial = datosXml->nodoHijo("espac", false, nodoXml);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaSnd::agregarBitsInternosEn(AUEscenaSnd* sonido, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSnd::agregarBitsInternosEn")
	bool exito = false;
	if(AUEscenaObjeto::agregarBitsInternosEn(sonido, guardarEn)){
		exito = true;
		guardarEn->escribir(&sonido->_iniciarReproduciendo, sizeof(sonido->_iniciarReproduciendo), 1, guardarEn);
		guardarEn->escribir(&sonido->_esEspacial, sizeof(sonido->_esEspacial), 1, guardarEn);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaSnd::interpretarBitsInternos(AUEscenaSnd* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSnd::interpretarBitsInternos")
	bool exito = false;
	if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
		exito = true;
		cargarDe->leer(&cargarEn->_iniciarReproduciendo, sizeof(cargarEn->_iniciarReproduciendo), 1, cargarDe);
		cargarDe->leer(&cargarEn->_esEspacial, sizeof(cargarEn->_esEspacial), 1, cargarDe);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaSnd, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaSnd, "AUEscenaSnd", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_NULL(AUEscenaSnd)


#endif //#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO

