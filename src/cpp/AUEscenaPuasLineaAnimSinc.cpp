//
//  AUEscenaPuasLineaAnimSinc.cpp
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaPuasLineaAnimSinc.h"

AUEscenaPuasLineaAnimSinc::AUEscenaPuasLineaAnimSinc() : AUEscenaObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::AUEscenaPuasLineaAnimSinc")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaPuasLineaAnimSinc")
	_puasLongitudHaciaDerecha	= 64.0f;
	_puasSeparacion				= 8.0f;
	//
	_puasTipoTamano				= ENPuasLineaTamano_Pequeno;
	_puaTextura					= NBGestorTexturas::texturaDesdeArchivo("Escenarios/obsPuas01Peq.png"); _puaTextura->retener(NB_RETENEDOR_THIS);
	_puaTexturaTamano			= _puaTextura->tamanoHD();
	//
	_puasSegsParaMostrar		= _puasSegsParaOcultar	= 0.25f;
	_puasSegsMostrar			= _puasSegsOcultar		= 1.0f;
	//
	_puasAnimEstado.factorTamanoAct		= 1.0f;
	_puasAnimEstado.segsAcumEtapaActual	= 0.0f;
	_puasAnimEstado.animEstapaActual	= ENPuasLineaAnimEstado_Mostradas;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaPuasLineaAnimSinc::AUEscenaPuasLineaAnimSinc(AUEscenaPuasLineaAnimSinc* otraInstancia) : AUEscenaObjeto(otraInstancia) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::AUEscenaPuasLineaAnimSinc")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaPuasLineaAnimSinc")
	_puasLongitudHaciaDerecha	= otraInstancia->_puasLongitudHaciaDerecha;
	_puasSeparacion				= otraInstancia->_puasSeparacion;
	//
	_puasTipoTamano				= otraInstancia->_puasTipoTamano;
	_puaTextura					= otraInstancia->_puaTextura; _puaTextura->retener(NB_RETENEDOR_THIS);
	_puaTexturaTamano			= otraInstancia->_puaTexturaTamano;
	//
	_puasSegsParaMostrar		= otraInstancia->_puasSegsParaMostrar;
	_puasSegsParaOcultar		= otraInstancia->_puasSegsParaOcultar;
	_puasSegsMostrar			= otraInstancia->_puasSegsMostrar;
	_puasSegsOcultar			= otraInstancia->_puasSegsOcultar;
	//
	_puasAnimEstado.factorTamanoAct		= 1.0f;
	_puasAnimEstado.segsAcumEtapaActual	= 0.0f;
	_puasAnimEstado.animEstapaActual	= ENPuasLineaAnimEstado_Mostradas;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaPuasLineaAnimSinc::~AUEscenaPuasLineaAnimSinc(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::~AUEscenaPuasLineaAnimSinc")
	if(_puaTextura != NULL) _puaTextura->liberar(NB_RETENEDOR_THIS); _puaTextura = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaPuasLineaAnimSinc::privActualizarTexturaPuas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::privActualizarTexturaPuas")
	if(_puaTextura != NULL) _puaTextura->liberar(NB_RETENEDOR_THIS); _puaTextura = NULL;
	_puaTextura					= NBGestorTexturas::texturaDesdeArchivo(_puasTipoTamano==ENPuasLineaTamano_Pequeno?"Escenarios/obsPuas01Peq.png":_puasTipoTamano==ENPuasLineaTamano_Mediano?"Escenarios/obsPuas01Med.png":_puasTipoTamano==ENPuasLineaTamano_Grande?"Escenarios/obsPuas01Gran.png":_puasTipoTamano==ENPuasLineaTamano_Gigante?"Escenarios/obsPuas01Gig.png":""); _puaTextura->retener(NB_RETENEDOR_THIS);
	_puaTexturaTamano			= _puaTextura->tamanoHD();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

float AUEscenaPuasLineaAnimSinc::puasLongitudHaciaDerecha() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::puasLongitudHaciaDerecha")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasLongitudHaciaDerecha;
}

float AUEscenaPuasLineaAnimSinc::puasSeparacion() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::puasSeparacion")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasSeparacion;
}

ENPuasLineaTamano AUEscenaPuasLineaAnimSinc::puasTamano() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::puasTamano")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return (ENPuasLineaTamano)_puasTipoTamano;
}

float AUEscenaPuasLineaAnimSinc::puasSegsMostrar() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::puasSegsMostrar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasSegsMostrar;
}

float AUEscenaPuasLineaAnimSinc::puasSegsOcultar() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::puasSegsOcultar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasSegsOcultar;
}

float AUEscenaPuasLineaAnimSinc::puasSegsParaMostrar() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::puasSegsParaMostrar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasSegsParaMostrar;
}

float AUEscenaPuasLineaAnimSinc::puasSegsParaOcultar() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::puasSegsParaOcultar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasSegsParaOcultar;
}

void AUEscenaPuasLineaAnimSinc::establecerPuasLongitudHaciaDerecha(const float longitud){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::establecerPuasLongitudHaciaDerecha")
	NBASSERT(longitud>0.0f);
	_puasLongitudHaciaDerecha = longitud;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimSinc::establecerPuasSeparacion(const float separacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::establecerPuasSeparacion")
	NBASSERT(separacion>=0.0f);
	_puasSeparacion = separacion;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimSinc::establecerPuasTamano(const ENPuasLineaTamano tamano){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::establecerPuasTamano")
	NBASSERT(tamano>=0 && tamano<ENPuasLineaTamano_Conteo);
	if(tamano!=_puasTipoTamano){
		_puasTipoTamano = tamano;
		privActualizarTexturaPuas();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimSinc::establecerPuasSegsMostrar(const float segs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::establecerPuasSegsMostrar")
	NBASSERT(segs>=0.0f);
	_puasSegsMostrar = segs;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimSinc::establecerPuasSegsOcultar(const float segs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::establecerPuasSegsOcultar")
	NBASSERT(segs>=0.0f);
	_puasSegsOcultar = segs;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimSinc::establecerPuasSegsParaMostrar(const float segs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::establecerPuasSegsMostrar")
	NBASSERT(segs>=0.0f);
	_puasSegsParaMostrar = segs;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimSinc::establecerPuasSegsParaOcultar(const float segs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::establecerPuasSegsOcultar")
	NBASSERT(segs>=0.0f);
	_puasSegsParaOcultar = segs;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

NBCajaAABB AUEscenaPuasLineaAnimSinc::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::cajaAABBLocalCalculada")
	NBCajaAABB cajaLocal;
	cajaLocal.xMin	= 0.0f;
	cajaLocal.xMax	= _puasLongitudHaciaDerecha;
	cajaLocal.yMin	= 0.0f;
	cajaLocal.yMax	= _puaTexturaTamano.ancho;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return cajaLocal;
}

void AUEscenaPuasLineaAnimSinc::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::agregadoEnEscena")
	NBGestorPersonajes::agregarVictimario(this, 0);
	NBGestorAnimadores::agregarAnimador(this, this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimSinc::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::quitandoDeEscena")
	NBGestorAnimadores::quitarAnimador(this);
	NBGestorPersonajes::quitarVictimario(this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool AUEscenaPuasLineaAnimSinc::personajeDanoHaciaVictima(const NBCajaAABB &cajaEscenaVictima, STPersonajeDano* guardarDanoEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimSinc::personajeDanoHaciaVictima")
	bool existeDano = false;
	if(_puasAnimEstado.factorTamanoAct!=0.0f && NBCAJAAABB_INTERSECTA_CAJA(_cacheObjEscena.cajaEscena, cajaEscenaVictima)){ //Caja que inclute al sprite y los rayos
		NBPunto posIniEscena;	NBMATRIZ_MULTIPLICAR_PUNTO(posIniEscena, _cacheObjEscena.matrizEscena, 0.0f, 0.0f)
		NBPunto posFinEscena;	NBMATRIZ_MULTIPLICAR_PUNTO(posFinEscena, _cacheObjEscena.matrizEscena, _puasLongitudHaciaDerecha, 0.0f)
		NBPunto vecPuas;		NBPUNTO_ESTABLECER(vecPuas, posFinEscena.x-posIniEscena.x, posFinEscena.y-posIniEscena.y)
		float modVectorPuas		= NBPUNTO_DISTANCIA_VECTOR(vecPuas.x, vecPuas.y); NBASSERT(modVectorPuas!=0.0f)
		NBPunto vecUnitPuas;	NBPUNTO_ESTABLECER(vecUnitPuas, vecPuas.x/modVectorPuas, vecPuas.y/modVectorPuas)
		NBPunto vectorCaja[4];
		float alcancePuas		= _puaTexturaTamano.ancho * _puasAnimEstado.factorTamanoAct; NBASSERT(alcancePuas!=0.0f)
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

void AUEscenaPuasLineaAnimSinc::tickAnimacion(float segsTrancurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLineaAnimSinc::tickAnimacion")
	_puasAnimEstado.segsAcumEtapaActual += segsTrancurridos;
	switch (_puasAnimEstado.animEstapaActual) {
		case ENPuasLineaAnimEstado_Mostradas:
			if(_puasAnimEstado.segsAcumEtapaActual>=_puasSegsMostrar){
				_puasAnimEstado.animEstapaActual		= ENPuasLineaAnimEstado_Ocultando;
				_puasAnimEstado.segsAcumEtapaActual	= 0.0f;
			}
			break;
		case ENPuasLineaAnimEstado_Ocultando:
			if(_puasAnimEstado.segsAcumEtapaActual<_puasSegsParaOcultar){
				_puasAnimEstado.factorTamanoAct		= 1.0f - (_puasAnimEstado.segsAcumEtapaActual / _puasSegsParaOcultar);
			} else {
				_puasAnimEstado.factorTamanoAct		= 0.0f;
				_puasAnimEstado.animEstapaActual	= ENPuasLineaAnimEstado_Ocultas;
				_puasAnimEstado.segsAcumEtapaActual	= 0.0f;
			}
			break;
		case ENPuasLineaAnimEstado_Ocultas:
			if(_puasAnimEstado.segsAcumEtapaActual>=_puasSegsOcultar){
				_puasAnimEstado.animEstapaActual		= ENPuasLineaAnimEstado_Showing;
				_puasAnimEstado.segsAcumEtapaActual	= 0.0f;
			}
			break;
		case ENPuasLineaAnimEstado_Showing:
			if(_puasAnimEstado.segsAcumEtapaActual<_puasSegsParaMostrar){
				_puasAnimEstado.factorTamanoAct		= _puasAnimEstado.segsAcumEtapaActual / _puasSegsParaMostrar;
			} else {
				_puasAnimEstado.factorTamanoAct		= 1.0f;
				_puasAnimEstado.animEstapaActual	= ENPuasLineaAnimEstado_Mostradas;
				_puasAnimEstado.segsAcumEtapaActual	= 0.0f;
			}
			break;
		default: NBASSERT(false) break;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

PTRfunCmdsGL AUEscenaPuasLineaAnimSinc::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLineaAnimSinc::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	//Retorno temprano
	if(_puasAnimEstado.factorTamanoAct==0.0f){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2(NULL); }
	//
	SI16 cantPuasCompletas	= _puasLongitudHaciaDerecha / (_puaTexturaTamano.alto + _puasSeparacion); if(cantPuasCompletas==0) cantPuasCompletas = 1;
	if(cantPuasCompletas!=0){
		NBColor8 colorMultiplicado8; NBCOLOR_ESTABLECER(colorMultiplicado8, propsHeredadas.colorPadre.r * _propiedades.color8.r, propsHeredadas.colorPadre.g * _propiedades.color8.g, propsHeredadas.colorPadre.b * _propiedades.color8.b, propsHeredadas.colorPadre.a * _propiedades.color8.a)
		STBloqueGL bloqueVerticesGL;
		bloqueVerticesGL.cantidadElementos			= cantPuasCompletas * 4;
		bloqueVerticesGL.primerElemento				= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL.cantidadElementos);
		//Arreglos de grupos
		float avanceEnLinea					= 0.0f;
		float altoPuas						= _puaTexturaTamano.ancho * _puasAnimEstado.factorTamanoAct;
		NBVerticeGL* verticesGL				= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]);
		UI16 iPua, conteoPuas				= cantPuasCompletas;
		for(iPua=0; iPua<conteoPuas; iPua++){
			float xFinPua					= avanceEnLinea + _puaTexturaTamano.alto;
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], _cacheObjEscena.matrizEscena, avanceEnLinea, 0.0f)
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[1], _cacheObjEscena.matrizEscena, avanceEnLinea, altoPuas)
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[2], _cacheObjEscena.matrizEscena, xFinPua, 0.0f)
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[3], _cacheObjEscena.matrizEscena, xFinPua, altoPuas)
			NBCOLOR_ESTABLECER(verticesGL[0], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a)
			NBCOLOR_ESTABLECER(verticesGL[1], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a)
			NBCOLOR_ESTABLECER(verticesGL[2], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a)
			NBCOLOR_ESTABLECER(verticesGL[3], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a)
			_puaTextura->cargarCoordenadasTextura(verticesGL, 1.0f-_puasAnimEstado.factorTamanoAct, 1.0f, 0.0f, 1.0f, ENTexturaModoCoordenadas_Entrelazadas);
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
	return (cantPuasCompletas!=0 ? &AUEscenaPuasLineaAnimSinc::enviarComandosGL : NULL);
}

void AUEscenaPuasLineaAnimSinc::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLineaAnimSinc::enviarComandosGL")
	STPuasLineaRender* datosRender		= (STPuasLineaRender*)punteroDatosModelo;
	NBASSERT(datosRender->bloqueIndicesGL.cantidadElementos>0)
	NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaPuasLineaAnimSinc")
	NBGestorGL::bindTexture(0, datosRender->idTexturaGL);
	NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(datosRender->bloqueIndicesGL.primerElemento, datosRender->bloqueIndicesGL.cantidadElementos); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaPuasLineaAnimSinc")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

bool AUEscenaPuasLineaAnimSinc::agregarXmlInternoEn(AUEscenaPuasLineaAnimSinc* lineaParts, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLineaAnimSinc::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(lineaParts, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	//
	guardarEn->agregarConFormato("%s<sep>%f</sep>\r\n", espaciosBaseIzq, lineaParts->_puasSeparacion);
	guardarEn->agregarConFormato("%s<lng>%f</lng>\r\n", espaciosBaseIzq, lineaParts->_puasLongitudHaciaDerecha);
	guardarEn->agregarConFormato("%s<tam>%d</tam>\r\n", espaciosBaseIzq, lineaParts->_puasTipoTamano);
	//
	guardarEn->agregarConFormato("%s<spo>%f</spo>\r\n", espaciosBaseIzq, lineaParts->_puasSegsParaOcultar);
	guardarEn->agregarConFormato("%s<spm>%f</spm>\r\n", espaciosBaseIzq, lineaParts->_puasSegsParaMostrar);
	guardarEn->agregarConFormato("%s<so>%f</so>\r\n", espaciosBaseIzq, lineaParts->_puasSegsOcultar);
	guardarEn->agregarConFormato("%s<sm>%f</sm>\r\n", espaciosBaseIzq, lineaParts->_puasSegsMostrar);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
	
}

bool AUEscenaPuasLineaAnimSinc::interpretarXmlInterno(AUEscenaPuasLineaAnimSinc* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLineaAnimSinc::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	const sNodoXML* nodoEscenaObj;			XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
	if(nodoEscenaObj != NULL){
		exito = AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj);
		cargarEn->_puasSeparacion			= datosXml->nodoHijo("sep", 8.0f, nodoXml);
		cargarEn->_puasLongitudHaciaDerecha	= datosXml->nodoHijo("lng", 32.0f, nodoXml);
		cargarEn->_puasSegsParaOcultar		= datosXml->nodoHijo("spo", 0.25f, nodoXml);
		cargarEn->_puasSegsParaMostrar		= datosXml->nodoHijo("spm", 0.25f, nodoXml);
		cargarEn->_puasSegsOcultar			= datosXml->nodoHijo("so", 1.0f, nodoXml);
		cargarEn->_puasSegsMostrar			= datosXml->nodoHijo("sm", 1.0f, nodoXml);
		cargarEn->establecerPuasTamano((ENPuasLineaTamano)datosXml->nodoHijo("tam", 0, nodoXml));
		exito = true;
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaPuasLineaAnimSinc::agregarBitsInternosEn(AUEscenaPuasLineaAnimSinc* lineaParts, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLineaAnimSinc::agregarBitsInternosEn")
	bool exito = false;
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(lineaParts, guardarEn)){
		guardarEn->escribir(&lineaParts->_puasSeparacion, sizeof(lineaParts->_puasSeparacion), 1, guardarEn);
		SI32 tamPuas = lineaParts->_puasTipoTamano; guardarEn->escribir(&tamPuas, sizeof(tamPuas), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_puasLongitudHaciaDerecha, sizeof(lineaParts->_puasLongitudHaciaDerecha), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_puasSegsParaOcultar, sizeof(lineaParts->_puasSegsParaOcultar), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_puasSegsParaMostrar, sizeof(lineaParts->_puasSegsParaMostrar), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_puasSegsOcultar, sizeof(lineaParts->_puasSegsOcultar), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_puasSegsMostrar, sizeof(lineaParts->_puasSegsMostrar), 1, guardarEn);
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaPuasLineaAnimSinc::interpretarBitsInternos(AUEscenaPuasLineaAnimSinc* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLineaAnimSinc::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			cargarDe->leer(&cargarEn->_puasSeparacion, sizeof(cargarEn->_puasSeparacion), 1, cargarDe);
			SI32 tamPuas; cargarDe->leer(&tamPuas, sizeof(tamPuas), 1, cargarDe); cargarEn->establecerPuasTamano((ENPuasLineaTamano)tamPuas);
			cargarDe->leer(&cargarEn->_puasLongitudHaciaDerecha, sizeof(cargarEn->_puasLongitudHaciaDerecha), 1, cargarDe);
			cargarDe->leer(&cargarEn->_puasSegsParaOcultar, sizeof(cargarEn->_puasSegsParaOcultar), 1, cargarDe);
			cargarDe->leer(&cargarEn->_puasSegsParaMostrar, sizeof(cargarEn->_puasSegsParaMostrar), 1, cargarDe);
			cargarDe->leer(&cargarEn->_puasSegsOcultar, sizeof(cargarEn->_puasSegsOcultar), 1, cargarDe);
			cargarDe->leer(&cargarEn->_puasSegsMostrar, sizeof(cargarEn->_puasSegsMostrar), 1, cargarDe);
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUEscenaSprite, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaPuasLineaAnimSinc, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaPuasLineaAnimSinc, "AUEscenaPuasLineaAnimSinc", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaPuasLineaAnimSinc)






