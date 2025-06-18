//
//  AUEscenaFiltro.cpp
//  EditorCuerpos
//
//  Created by Nicaragua Binary on 24/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaFiltro.h"

AUEscenaFiltro::AUEscenaFiltro() : AUEscenaObjeto(), _objetosEnFiltro(this) {
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaFiltro")
	//
}

AUEscenaFiltro::AUEscenaFiltro(AUEscenaFiltro* otraInstancia) : AUEscenaObjeto(otraInstancia), _objetosEnFiltro(this, otraInstancia->_objetosEnFiltro.conteo) {
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaFiltro")
	//
}

AUEscenaFiltro::~AUEscenaFiltro(){
	vaciarObjetosFiltrados();
}

//

void AUEscenaFiltro::actualizarObjetosEnFiltroSegunInstancia(AUEscenaFiltro* otraInstancia, AUArreglo* hijosOriginales, AUArreglo* hijosDestino){
	UI16 i;
	for(i=0; i<otraInstancia->_objetosEnFiltro.conteo; i++){
		SI32 indice = hijosOriginales->indiceDe(otraInstancia->_objetosEnFiltro.elemento[i]);
		if(indice>=0 && indice<hijosDestino->conteo){
			this->agregarObjetoFiltrado((AUEscenaObjeto*)hijosDestino->elemento[indice]);
		}
	}
	PRINTF_INFO("%d objetos en filtro despues de actualizar referencias\n", _objetosEnFiltro.conteo);
}

SI32 AUEscenaFiltro::conteoObjetosEnFiltro(){
	return _objetosEnFiltro.conteo;
}

void AUEscenaFiltro::agregarObjetoFiltrado(AUEscenaObjeto* objetoFiltrar){
	NBASSERT(_objetosEnFiltro.indiceDe(objetoFiltrar)==-1) //Si fala, se esta duplicando
	if(!_dormido) objetoFiltrar->cafeinizar();
	_objetosEnFiltro.agregarElemento(objetoFiltrar);
}

void AUEscenaFiltro::vaciarObjetosFiltrados(){
	if(!_dormido){
		UI16 i; for(i=0; i<_objetosEnFiltro.conteo; i++) ((AUEscenaObjeto*)_objetosEnFiltro.elemento[i])->decafeinizar();
	}
	_objetosEnFiltro.vaciar();
}

//

void AUEscenaFiltro::establecerDormido(bool dormido){
	if(dormido != _dormido){
		UI16 i;
		if(dormido){
			for(i=0; i<_objetosEnFiltro.conteo; i++) ((AUEscenaObjeto*)_objetosEnFiltro.elemento[i])->decafeinizar();
		} else {
			for(i=0; i<_objetosEnFiltro.conteo; i++) ((AUEscenaObjeto*)_objetosEnFiltro.elemento[i])->cafeinizar();
		}
	}
	AUEscenaObjeto::establecerDormido(dormido);
}

//

NBCajaAABB AUEscenaFiltro::cajaAABBLocalCalculada(){
	NBCajaAABB cajaLocal;
	cajaLocal.xMin = cajaLocal.yMin = -32.0f;
	cajaLocal.xMax = cajaLocal.yMax = 32.0f;
	return cajaLocal;
}

PTRfunCmdsGL AUEscenaFiltro::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	return NULL;
}

//

bool AUEscenaFiltro::interpretarXmlInterno(AUEscenaFiltro* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos, AUArregloNativoP<SI32>* arregloIDs){
	bool exito = false;
	if(AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoXml)){
		cargarEn->vaciarObjetosFiltrados();
		const sNodoXML* nodoEnlaces = datosXml->nodoHijo("es", nodoXml);
		if(nodoEnlaces != NULL){
			const sNodoXML* nodoE = datosXml->nodoHijo("e", nodoEnlaces);
			while(nodoE != NULL){
				const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoE);
				SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
				SI32 posEnCadenaCSV		= 0;
				SI32 idObj				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, -1);
				SI32 indiceObjeto		= arregloIDs->indiceDe(idObj);
				if(indiceObjeto!=-1)	cargarEn->agregarObjetoFiltrado(arregloObjetos->elemento[indiceObjeto]);
				//Siguiente
				nodoE = datosXml->nodoHijo("e", nodoEnlaces, nodoE);
			}
		}
		exito = true;
	}
	return exito;
}

bool AUEscenaFiltro::agregarBitsInternosEn(AUEscenaFiltro* filtro, AUArchivo* guardarEn, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaFiltro::agregarBitsInternosEn")
	bool exito = false;
	if(AUEscenaObjeto::agregarBitsInternosEn(filtro, guardarEn)){
		SI32 iEnlace, conteoEnlaces = filtro->_objetosEnFiltro.conteo;
		guardarEn->escribir(&conteoEnlaces, sizeof(conteoEnlaces), 1, guardarEn);
		for(iEnlace=0; iEnlace<conteoEnlaces; iEnlace++){
			SI32 indiceObjeto = arregloObjetosParaID->indiceDe(filtro->_objetosEnFiltro.elemento[iEnlace]);
			guardarEn->escribir(&indiceObjeto, sizeof(indiceObjeto), 1, guardarEn);
		}
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

bool AUEscenaFiltro::interpretarBitsInternos(AUEscenaFiltro* cargarEn, AUArchivo* cargarDe, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaFiltro::interpretarBitsInternos")
	bool exito = false;
	if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
		SI32 iEnlace, conteoEnlaces;
		cargarDe->leer(&conteoEnlaces, sizeof(conteoEnlaces), 1, cargarDe);
		for(iEnlace=0; iEnlace<conteoEnlaces; iEnlace++){
			SI32 idObjeto;		cargarDe->leer(&idObjeto, sizeof(idObjeto), 1, cargarDe);
			if(idObjeto>=0 && idObjeto<arregloObjetosParaID->conteo){
				cargarEn->agregarObjetoFiltrado((AUEscenaObjeto*)arregloObjetosParaID->elemento[idObjeto]);
			}
		}
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUEscenaFiltro)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUEscenaFiltro, "AUEscenaFiltro")
AUOBJMETODOS_CLONAR_THIS(AUEscenaFiltro)




