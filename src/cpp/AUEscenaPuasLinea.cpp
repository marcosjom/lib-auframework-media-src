//
//  AUEscenaPuasLinea.cpp
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaPuasLinea.h"

AUEscenaPuasLinea::AUEscenaPuasLinea() : AUEscenaObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLinea::AUEscenaPuasLinea")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaPuasLinea")
	_puasLongitudHaciaDerecha	= 64.0f;
	_puasSeparacion				= 8.0f;
	//
	_puasTipoTamano				= ENPuasLineaTamano_Pequeno;
	_puaTextura					= NBGestorTexturas::texturaDesdeArchivo("Escenarios/obsPuas01Peq.png"); _puaTextura->retener(NB_RETENEDOR_THIS);
	_puaTexturaTamano			= _puaTextura->tamanoHD();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaPuasLinea::AUEscenaPuasLinea(AUEscenaPuasLinea* otraInstancia) : AUEscenaObjeto(otraInstancia) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLinea::AUEscenaPuasLinea")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaPuasLinea")
	_puasLongitudHaciaDerecha	= otraInstancia->_puasLongitudHaciaDerecha;
	_puasSeparacion				= otraInstancia->_puasSeparacion;
	//
	_puasTipoTamano				= otraInstancia->_puasTipoTamano;
	_puaTextura					= otraInstancia->_puaTextura; _puaTextura->retener(NB_RETENEDOR_THIS);
	_puaTexturaTamano			= otraInstancia->_puaTexturaTamano;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaPuasLinea::~AUEscenaPuasLinea(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLinea::~AUEscenaPuasLinea")
	if(_puaTextura != NULL) _puaTextura->liberar(NB_RETENEDOR_THIS); _puaTextura = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaPuasLinea::privActualizarTexturaPuas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLinea::privActualizarTexturaPuas")
	if(_puaTextura != NULL) _puaTextura->liberar(NB_RETENEDOR_THIS); _puaTextura = NULL;
	_puaTextura					= NBGestorTexturas::texturaDesdeArchivo(_puasTipoTamano==ENPuasLineaTamano_Pequeno?"Escenarios/obsPuas01Peq.png":_puasTipoTamano==ENPuasLineaTamano_Mediano?"Escenarios/obsPuas01Med.png":_puasTipoTamano==ENPuasLineaTamano_Grande?"Escenarios/obsPuas01Gran.png":_puasTipoTamano==ENPuasLineaTamano_Gigante?"Escenarios/obsPuas01Gig.png":""); _puaTextura->retener(NB_RETENEDOR_THIS);
	_puaTexturaTamano			= _puaTextura->tamanoHD();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

float AUEscenaPuasLinea::puasLongitudHaciaDerecha() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLinea::puasLongitudHaciaDerecha")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasLongitudHaciaDerecha;
}

float AUEscenaPuasLinea::puasSeparacion() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLinea::puasSeparacion")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasSeparacion;
}

ENPuasLineaTamano AUEscenaPuasLinea::puasTamano() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLinea::puasTamano")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return (ENPuasLineaTamano)_puasTipoTamano;
}

void AUEscenaPuasLinea::establecerPuasLongitudHaciaDerecha(const float longitud){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLinea::establecerPuasLongitudHaciaDerecha")
	NBASSERT(longitud>0.0f);
	_puasLongitudHaciaDerecha = longitud;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLinea::establecerPuasSeparacion(const float separacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLinea::establecerPuasSeparacion")
	NBASSERT(separacion>=0.0f);
	_puasSeparacion = separacion;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLinea::establecerPuasTamano(const ENPuasLineaTamano tamano){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLinea::establecerPuasTamano")
	NBASSERT(tamano>=0 && tamano<ENPuasLineaTamano_Conteo);
	if(tamano!=_puasTipoTamano){
		_puasTipoTamano = tamano;
		privActualizarTexturaPuas();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

NBCajaAABB AUEscenaPuasLinea::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::cajaAABBLocalCalculada")
	NBCajaAABB cajaLocal;
	cajaLocal.xMin	= 0.0f;
	cajaLocal.xMax	= _puasLongitudHaciaDerecha;
	cajaLocal.yMin	= 0.0f;
	cajaLocal.yMax	= _puaTexturaTamano.ancho;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return cajaLocal;
}

void AUEscenaPuasLinea::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLinea::agregadoEnEscena")
	NBGestorPersonajes::agregarVictimario(this, 0);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLinea::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLinea::quitandoDeEscena")
	NBGestorPersonajes::quitarVictimario(this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool AUEscenaPuasLinea::personajeDanoHaciaVictima(const NBCajaAABB &cajaEscenaVictima, STPersonajeDano* guardarDanoEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLinea::personajeDanoHaciaVictima")
	bool existeDano = false;
	if(NBCAJAAABB_INTERSECTA_CAJA(_cacheObjEscena.cajaEscena, cajaEscenaVictima)){ //Caja que inclute al sprite y los rayos
		NBPunto posIniEscena;	NBMATRIZ_MULTIPLICAR_PUNTO(posIniEscena, _cacheObjEscena.matrizEscena, 0.0f, 0.0f)
		NBPunto posFinEscena;	NBMATRIZ_MULTIPLICAR_PUNTO(posFinEscena, _cacheObjEscena.matrizEscena, _puasLongitudHaciaDerecha, 0.0f)
		NBPunto vecPuas;		NBPUNTO_ESTABLECER(vecPuas, posFinEscena.x-posIniEscena.x, posFinEscena.y-posIniEscena.y)
		float modVectorPuas		= NBPUNTO_DISTANCIA_VECTOR(vecPuas.x, vecPuas.y); NBASSERT(modVectorPuas!=0.0f)
		NBPunto vecUnitPuas;	NBPUNTO_ESTABLECER(vecUnitPuas, vecPuas.x/modVectorPuas, vecPuas.y/modVectorPuas)
		NBPunto vectorCaja[4];
		float alcancePuas		= _puaTexturaTamano.ancho; NBASSERT(alcancePuas!=0.0f)
		float productoCruz[4];
		NBPUNTO_ESTABLECER(vectorCaja[0], cajaEscenaVictima.xMin-posIniEscena.x, cajaEscenaVictima.yMin-posIniEscena.y)
		NBPUNTO_ESTABLECER(vectorCaja[1], cajaEscenaVictima.xMin-posIniEscena.x, cajaEscenaVictima.yMax-posIniEscena.y)
		NBPUNTO_ESTABLECER(vectorCaja[2], cajaEscenaVictima.xMax-posIniEscena.x, cajaEscenaVictima.yMin-posIniEscena.y)
		NBPUNTO_ESTABLECER(vectorCaja[3], cajaEscenaVictima.xMax-posIniEscena.x, cajaEscenaVictima.yMax-posIniEscena.y)
		productoCruz[0]	= NBPUNTO_PRODUCTO_VECTORIAL(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[0].x, vectorCaja[0].y);
		productoCruz[1]	= NBPUNTO_PRODUCTO_VECTORIAL(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[1].x, vectorCaja[1].y);
		productoCruz[2]	= NBPUNTO_PRODUCTO_VECTORIAL(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[2].x, vectorCaja[2].y);
		productoCruz[3]	= NBPUNTO_PRODUCTO_VECTORIAL(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[3].x, vectorCaja[3].y);
		float distanciaMasCercano = alcancePuas;
		if(productoCruz[0]>0.0f && productoCruz[0]<distanciaMasCercano) distanciaMasCercano = productoCruz[0];
		if(productoCruz[1]>0.0f && productoCruz[1]<distanciaMasCercano) distanciaMasCercano = productoCruz[1];
		if(productoCruz[2]>0.0f && productoCruz[2]<distanciaMasCercano) distanciaMasCercano = productoCruz[2];
		if(productoCruz[3]>0.0f && productoCruz[3]<distanciaMasCercano) distanciaMasCercano = productoCruz[3];
		if(!((productoCruz[0]<0.0f && productoCruz[1]<0.0f && productoCruz[2]<0.0f && productoCruz[3]<0.0f) || (productoCruz[0]>=0.0f && productoCruz[1]>=0.0f && productoCruz[2]>=0.0f && productoCruz[3]>=0.0f))) distanciaMasCercano = 0.0f; //La linea atraviesa a la caja
		if(distanciaMasCercano<alcancePuas){ //Por lo menos un vertice esta cerca
			float productoPunto[4];
			productoPunto[0]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[0].x, vectorCaja[0].y);
			productoPunto[1]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[1].x, vectorCaja[1].y);
			productoPunto[2]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[2].x, vectorCaja[2].y);
			productoPunto[3]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[3].x, vectorCaja[3].y);
			if(!((productoPunto[0]<0.0f && productoPunto[1]<0.0f && productoPunto[2]<0.0f && productoPunto[3]<0.0f) || (productoPunto[0]>modVectorPuas && productoPunto[1]>modVectorPuas && productoPunto[2]>modVectorPuas && productoPunto[3]>modVectorPuas))){
				//La caja tiene por lo menos un vertice dentro del rayo
				existeDano = true;
				guardarDanoEn->factorDanoRecibido = 1.0f;
				float promedioPuntoCaja = (productoPunto[0] + productoPunto[1] + productoPunto[2] + productoPunto[3]) / 4.0f;
				guardarDanoEn->posEscenaOrigenDano.x = posIniEscena.x + (vecUnitPuas.x * promedioPuntoCaja);
				guardarDanoEn->posEscenaOrigenDano.y = posIniEscena.y + (vecUnitPuas.y * promedioPuntoCaja);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return existeDano;
}

PTRfunCmdsGL AUEscenaPuasLinea::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLinea::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	//Conteo de todas las puas a renderizar
	SI16 cantPuasCompletas	= _puasLongitudHaciaDerecha / (_puaTexturaTamano.alto + _puasSeparacion); if(cantPuasCompletas==0) cantPuasCompletas = 1;
	if(cantPuasCompletas!=0){
		NBColor8 colorMultiplicado8; NBCOLOR_ESTABLECER(colorMultiplicado8, propsHeredadas.colorPadre.r * _propiedades.color8.r, propsHeredadas.colorPadre.g * _propiedades.color8.g, propsHeredadas.colorPadre.b * _propiedades.color8.b, propsHeredadas.colorPadre.a * _propiedades.color8.a)
		STBloqueGL bloqueVerticesGL;
		bloqueVerticesGL.cantidadElementos	= cantPuasCompletas * 4;
		bloqueVerticesGL.primerElemento		= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL.cantidadElementos);
		//Arreglos de grupos
		float avanceEnLinea					= 0.0f;
		NBVerticeGL* verticesGL				= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]);
		UI16 iPua, conteoPuas				= cantPuasCompletas;
		for(iPua=0; iPua<conteoPuas; iPua++){
			float xFinPua					= avanceEnLinea + _puaTexturaTamano.alto;
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], _cacheObjEscena.matrizEscena, avanceEnLinea, 0.0f)
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[1], _cacheObjEscena.matrizEscena, avanceEnLinea, _puaTexturaTamano.ancho)
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[2], _cacheObjEscena.matrizEscena, xFinPua, 0.0f)
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[3], _cacheObjEscena.matrizEscena, xFinPua, _puaTexturaTamano.ancho)
			NBCOLOR_ESTABLECER(verticesGL[0], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a)
			NBCOLOR_ESTABLECER(verticesGL[1], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a)
			NBCOLOR_ESTABLECER(verticesGL[2], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a)
			NBCOLOR_ESTABLECER(verticesGL[3], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a)
			_puaTextura->cargarCoordenadasTextura(verticesGL);
			verticesGL		+= 4;
			//
			avanceEnLinea	+= _puaTexturaTamano.alto + _puasSeparacion;
		}
		NBASSERT(verticesGL==&((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento+bloqueVerticesGL.cantidadElementos]))
		NBASSERT(cantPuasCompletas > 0)
		//Datos render
		const SI32 indiceDatosRender	= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STPuasLineaRender));
		STPuasLineaRender* datosRender	= (STPuasLineaRender*)&(propsRenderizado.bytesDatosModelos->elemento[indiceDatosRender]);
		datosRender->idTexturaGL		= _puaTextura->idTexturaGL; propsRenderizado.piscinaRetecionRender->agregarElemento(_puaTextura);
		datosRender->bloqueIndicesGL	= NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_4(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL, cantPuasCompletas);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (cantPuasCompletas!=0 ? &AUEscenaPuasLinea::enviarComandosGL : NULL);
}

void AUEscenaPuasLinea::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLinea::enviarComandosGL")
	STPuasLineaRender* datosRender		= (STPuasLineaRender*)punteroDatosModelo;
	NBASSERT(datosRender->bloqueIndicesGL.cantidadElementos>0)
	NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaPuasLinea")
	NBGestorGL::bindTexture(0, datosRender->idTexturaGL);
	NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(datosRender->bloqueIndicesGL.primerElemento, datosRender->bloqueIndicesGL.cantidadElementos); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaPuasLinea")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

bool AUEscenaPuasLinea::agregarXmlInternoEn(AUEscenaPuasLinea* lineaParts, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLinea::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(lineaParts, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	//
	guardarEn->agregarConFormato("%s<sep>%f</sep>\r\n", espaciosBaseIzq, lineaParts->_puasSeparacion);
	guardarEn->agregarConFormato("%s<lng>%f</lng>\r\n", espaciosBaseIzq, lineaParts->_puasLongitudHaciaDerecha);
	guardarEn->agregarConFormato("%s<tam>%d</tam>\r\n", espaciosBaseIzq, lineaParts->_puasTipoTamano);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
	
}

bool AUEscenaPuasLinea::interpretarXmlInterno(AUEscenaPuasLinea* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLinea::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	const sNodoXML* nodoEscenaObj;			XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
	if(nodoEscenaObj != NULL){
		exito = AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj);
		cargarEn->_puasSeparacion			= datosXml->nodoHijo("sep", 8.0f, nodoXml);
		cargarEn->_puasLongitudHaciaDerecha	= datosXml->nodoHijo("lng", 32.0f, nodoXml);
		cargarEn->establecerPuasTamano((ENPuasLineaTamano)datosXml->nodoHijo("tam", 0, nodoXml));
		exito = true;
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaPuasLinea::agregarBitsInternosEn(AUEscenaPuasLinea* lineaParts, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLinea::agregarBitsInternosEn")
	bool exito = false;
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(lineaParts, guardarEn)){
		guardarEn->escribir(&lineaParts->_puasSeparacion, sizeof(lineaParts->_puasSeparacion), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_puasLongitudHaciaDerecha, sizeof(lineaParts->_puasLongitudHaciaDerecha), 1, guardarEn);
		SI32 tamPuas = lineaParts->_puasTipoTamano; guardarEn->escribir(&tamPuas, sizeof(tamPuas), 1, guardarEn);
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaPuasLinea::interpretarBitsInternos(AUEscenaPuasLinea* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLinea::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			cargarDe->leer(&cargarEn->_puasSeparacion, sizeof(cargarEn->_puasSeparacion), 1, cargarDe);
			cargarDe->leer(&cargarEn->_puasLongitudHaciaDerecha, sizeof(cargarEn->_puasLongitudHaciaDerecha), 1, cargarDe);
			SI32 tamPuas; cargarDe->leer(&tamPuas, sizeof(tamPuas), 1, cargarDe); cargarEn->establecerPuasTamano((ENPuasLineaTamano)tamPuas);
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUEscenaSprite, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaPuasLinea, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaPuasLinea, "AUEscenaPuasLinea", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaPuasLinea)







