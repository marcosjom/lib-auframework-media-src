//
//  AUEscenaPuasLineaAnimIndiv.cpp
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaPuasLineaAnimIndiv.h"

AUEscenaPuasLineaAnimIndiv::AUEscenaPuasLineaAnimIndiv() : AUEscenaObjeto(), _puasAnimEstados(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::AUEscenaPuasLineaAnimIndiv")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaPuasLineaAnimIndiv")
	_puasLongitudHaciaDerecha		= 64.0f;
	_puasSeparacion					= 8.0f;
	//
	_puasTipoTamano					= ENPuasLineaTamano_Pequeno;
	_puaTextura						= NBGestorTexturas::texturaDesdeArchivo("Escenarios/obsPuas01Peq.png"); _puaTextura->retener(NB_RETENEDOR_THIS);
	_puaTexturaTamano				= _puaTextura->tamanoHD();
	//
	_puasSegsParaMostrar			= _puasSegsParaOcultar	= 0.25f;
	//
	_puasAnimTipo					= ENPuasLineaAnimIndiv_UnHueco;
	_puasAnimUnHuecoVolver			= true;
	_puasAnimUnHuecoIniciarVolviendo = false;
	_puasAnimUnHuecoTamano			= 64.0f;
	_puasAnimUnHuecoVelMov			= 64.0f;
	_puasAnimUnHuecoSegsAcumEspera	= 0.0f;
	_puasAnimUnHuecoSegsEsperar		= 2.0f;
	_puasAnimUnHuecoVolviendo		= false;
	_puasAnimUnHuecoXInicioHueco	= -_puasAnimUnHuecoTamano;
	//
	this->privPuasActualizar();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaPuasLineaAnimIndiv::AUEscenaPuasLineaAnimIndiv(AUEscenaPuasLineaAnimIndiv* otraInstancia) : AUEscenaObjeto(otraInstancia), _puasAnimEstados(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::AUEscenaPuasLineaAnimIndiv")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaPuasLineaAnimIndiv")
	_puasLongitudHaciaDerecha		= otraInstancia->_puasLongitudHaciaDerecha;
	_puasSeparacion					= otraInstancia->_puasSeparacion;
	//
	_puasTipoTamano					= otraInstancia->_puasTipoTamano;
	_puaTextura						= otraInstancia->_puaTextura; _puaTextura->retener(NB_RETENEDOR_THIS);
	_puaTexturaTamano				= otraInstancia->_puaTexturaTamano;
	//
	_puasSegsParaMostrar			= otraInstancia->_puasSegsParaMostrar;
	_puasSegsParaOcultar			= otraInstancia->_puasSegsParaOcultar;
	//
	_puasAnimTipo					= ENPuasLineaAnimIndiv_UnHueco;
	_puasAnimUnHuecoVolver			= otraInstancia->_puasAnimUnHuecoVolver;
	_puasAnimUnHuecoIniciarVolviendo = otraInstancia->_puasAnimUnHuecoIniciarVolviendo;
	
	_puasAnimUnHuecoTamano			= otraInstancia->_puasAnimUnHuecoTamano;
	_puasAnimUnHuecoVelMov			= otraInstancia->_puasAnimUnHuecoVelMov;
	_puasAnimUnHuecoSegsEsperar		= otraInstancia->_puasAnimUnHuecoSegsEsperar;
	_puasAnimUnHuecoSegsAcumEspera	= 0.0f;
	if(_puasAnimUnHuecoIniciarVolviendo){
		_puasAnimUnHuecoXInicioHueco	= _puasLongitudHaciaDerecha;
		_puasAnimUnHuecoVolviendo		= true;
	} else {
		_puasAnimUnHuecoXInicioHueco	= -_puasAnimUnHuecoTamano;
		_puasAnimUnHuecoVolviendo		= false;
	}
	//
	this->privPuasActualizar();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaPuasLineaAnimIndiv::~AUEscenaPuasLineaAnimIndiv(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::~AUEscenaPuasLineaAnimIndiv")
	if(_puaTextura != NULL) _puaTextura->liberar(NB_RETENEDOR_THIS); _puaTextura = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaPuasLineaAnimIndiv::privActualizarTexturaPuas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::privActualizarTexturaPuas")
	if(_puaTextura != NULL) _puaTextura->liberar(NB_RETENEDOR_THIS); _puaTextura = NULL;
	_puaTextura					= NBGestorTexturas::texturaDesdeArchivo(_puasTipoTamano==ENPuasLineaTamano_Pequeno?"Escenarios/obsPuas01Peq.png":_puasTipoTamano==ENPuasLineaTamano_Mediano?"Escenarios/obsPuas01Med.png":_puasTipoTamano==ENPuasLineaTamano_Grande?"Escenarios/obsPuas01Gran.png":_puasTipoTamano==ENPuasLineaTamano_Gigante?"Escenarios/obsPuas01Gig.png":""); _puaTextura->retener(NB_RETENEDOR_THIS);
	_puaTexturaTamano			= _puaTextura->tamanoHD();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaPuasLineaAnimIndiv::privPuasActualizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::privPuasActualizar")
	_puasAnimEstados.vaciar();
	//
	SI16 cantidadPuas = _puasLongitudHaciaDerecha / (_puaTexturaTamano.alto + _puasSeparacion); if(cantidadPuas==0) cantidadPuas = 1;
	UI16 iPua;
	for(iPua=0; iPua<cantidadPuas; iPua++){
		ENPuasLineaPropsAnim datosAnimPua;
		datosAnimPua.factorTamanoAct		= 1.0f;
		datosAnimPua.segsAcumEtapaActual	= 0.0f;
		datosAnimPua.animEstapaActual		= ENPuasLineaAnimEstado_Mostradas;
		_puasAnimEstados.agregarElemento(datosAnimPua);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

float AUEscenaPuasLineaAnimIndiv::puasLongitudHaciaDerecha() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::puasLongitudHaciaDerecha")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasLongitudHaciaDerecha;
}

float AUEscenaPuasLineaAnimIndiv::puasSeparacion() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::puasSeparacion")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasSeparacion;
}

ENPuasLineaTamano AUEscenaPuasLineaAnimIndiv::puasTamano() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::puasTamano")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return (ENPuasLineaTamano)_puasTipoTamano;
}

float AUEscenaPuasLineaAnimIndiv::puasSegsParaMostrar() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::puasSegsParaMostrar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasSegsParaMostrar;
}

float AUEscenaPuasLineaAnimIndiv::puasSegsParaOcultar() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::puasSegsParaOcultar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasSegsParaOcultar;
}

float AUEscenaPuasLineaAnimIndiv::puasAnimHuecoAncho() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::puasAnimHuecoAncho")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasAnimUnHuecoTamano;
}

float AUEscenaPuasLineaAnimIndiv::puasAnimHuecoVel() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::puasAnimHuecoVel")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasAnimUnHuecoVelMov;
}

float AUEscenaPuasLineaAnimIndiv::puasAnimHuecoEspera() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::puasAnimHuecoEspera")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasAnimUnHuecoSegsEsperar;
}

bool AUEscenaPuasLineaAnimIndiv::puasAnimHuecoVolver() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::puasAnimHuecoVolver")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasAnimUnHuecoVolver;
}

bool AUEscenaPuasLineaAnimIndiv::puasAnimHuecoIniciarVolviendo() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::puasAnimHuecoIniciarVolviendo")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _puasAnimUnHuecoIniciarVolviendo;
}

void AUEscenaPuasLineaAnimIndiv::establecerPuasLongitudHaciaDerecha(const float longitud){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::establecerPuasLongitudHaciaDerecha")
	NBASSERT(longitud>0.0f);
	if(_puasLongitudHaciaDerecha!=longitud){
		_puasLongitudHaciaDerecha = longitud;
		this->privPuasActualizar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimIndiv::establecerPuasSeparacion(const float separacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::establecerPuasSeparacion")
	NBASSERT(separacion>=0.0f);
	if(_puasSeparacion != separacion){
		_puasSeparacion = separacion;
		this->privPuasActualizar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimIndiv::establecerPuasTamano(const ENPuasLineaTamano tamano){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::establecerPuasTamano")
	NBASSERT(tamano>=0 && tamano<ENPuasLineaTamano_Conteo);
	if(tamano!=_puasTipoTamano){
		_puasTipoTamano = tamano;
		privActualizarTexturaPuas();
		this->privPuasActualizar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimIndiv::establecerPuasSegsParaMostrar(const float segs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::establecerPuasSegsParaMostrar")
	NBASSERT(segs>=0.0f);
	_puasSegsParaMostrar = segs;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimIndiv::establecerPuasSegsParaOcultar(const float segs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::establecerPuasSegsParaOcultar")
	NBASSERT(segs>=0.0f);
	_puasSegsParaOcultar = segs;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}


void AUEscenaPuasLineaAnimIndiv::establecerPuasAnimHuecoAncho(const float ancho){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::establecerPuasAnimHuecoAncho")
	NBASSERT(ancho>=0.0f);
	_puasAnimUnHuecoTamano = ancho;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimIndiv::establecerPuasAnimHuecoVel(const float vel){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::establecerPuasAnimHuecoVel")
	NBASSERT(vel>=0.0f);
	_puasAnimUnHuecoVelMov = vel;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimIndiv::establecerPuasAnimHuecoEspera(const float segs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::establecerPuasAnimHuecoEspera")
	NBASSERT(segs>=0.0f);
	_puasAnimUnHuecoSegsEsperar = segs;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimIndiv::establecerPuasAnimHuecoVolver(const bool volver){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::establecerPuasAnimHuecoVolver")
	_puasAnimUnHuecoVolver = volver;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimIndiv::establecerPuasAnimHuecoIniciarVolviendo(const bool iniciarVolviendo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::establecerPuasAnimHuecoIniciarVolviendo")
	_puasAnimUnHuecoIniciarVolviendo = iniciarVolviendo;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

NBCajaAABB AUEscenaPuasLineaAnimIndiv::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::cajaAABBLocalCalculada")
	NBCajaAABB cajaLocal;
	cajaLocal.xMin	= 0.0f;
	cajaLocal.xMax	= _puasLongitudHaciaDerecha;
	cajaLocal.yMin	= 0.0f;
	cajaLocal.yMax	= _puaTexturaTamano.ancho;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return cajaLocal;
}

void AUEscenaPuasLineaAnimIndiv::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::agregadoEnEscena")
	NBGestorPersonajes::agregarVictimario(this, 0);
	NBGestorAnimadores::agregarAnimador(this, this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPuasLineaAnimIndiv::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::quitandoDeEscena")
	NBGestorAnimadores::quitarAnimador(this);
	NBGestorPersonajes::quitarVictimario(this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool AUEscenaPuasLineaAnimIndiv::personajeDanoHaciaVictima(const NBCajaAABB &cajaEscenaVictima, STPersonajeDano* guardarDanoEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPuasLineaAnimIndiv::personajeDanoHaciaVictima")
	bool existeDano = false;
	if(NBCAJAAABB_INTERSECTA_CAJA(_cacheObjEscena.cajaEscena, cajaEscenaVictima)){ //Caja que inclute al sprite y los rayos
		NBPunto posIniEscena;	NBMATRIZ_MULTIPLICAR_PUNTO(posIniEscena, _cacheObjEscena.matrizEscena, 0.0f, 0.0f)
		NBPunto posFinEscena;	NBMATRIZ_MULTIPLICAR_PUNTO(posFinEscena, _cacheObjEscena.matrizEscena, _puasLongitudHaciaDerecha, 0.0f)
		NBPunto vecPuas;		NBPUNTO_ESTABLECER(vecPuas, posFinEscena.x-posIniEscena.x, posFinEscena.y-posIniEscena.y)
		float modVectorPuas		= NBPUNTO_DISTANCIA_VECTOR(vecPuas.x, vecPuas.y); NBASSERT(modVectorPuas!=0.0f)
		NBPunto vecUnitPuas;	NBPUNTO_ESTABLECER(vecUnitPuas, vecPuas.x/modVectorPuas, vecPuas.y/modVectorPuas)
		NBPunto vectorCaja[4];
		float alcanceMaxPuas	= _puaTexturaTamano.ancho; NBASSERT(alcanceMaxPuas!=0.0f)
		float productoCruz[4];
		NBPUNTO_ESTABLECER(vectorCaja[0], cajaEscenaVictima.xMin-posIniEscena.x, cajaEscenaVictima.yMin-posIniEscena.y)
		NBPUNTO_ESTABLECER(vectorCaja[1], cajaEscenaVictima.xMin-posIniEscena.x, cajaEscenaVictima.yMax-posIniEscena.y)
		NBPUNTO_ESTABLECER(vectorCaja[2], cajaEscenaVictima.xMax-posIniEscena.x, cajaEscenaVictima.yMin-posIniEscena.y)
		NBPUNTO_ESTABLECER(vectorCaja[3], cajaEscenaVictima.xMax-posIniEscena.x, cajaEscenaVictima.yMax-posIniEscena.y)
		productoCruz[0]	= NBPUNTO_PRODUCTO_VECTORIAL(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[0].x, vectorCaja[0].y);
		productoCruz[1]	= NBPUNTO_PRODUCTO_VECTORIAL(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[1].x, vectorCaja[1].y);
		productoCruz[2]	= NBPUNTO_PRODUCTO_VECTORIAL(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[2].x, vectorCaja[2].y);
		productoCruz[3]	= NBPUNTO_PRODUCTO_VECTORIAL(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[3].x, vectorCaja[3].y);
		float distanciaMasCercano = alcanceMaxPuas;
		if(productoCruz[0]>0.0f && productoCruz[0]<distanciaMasCercano) distanciaMasCercano = productoCruz[0];
		if(productoCruz[1]>0.0f && productoCruz[1]<distanciaMasCercano) distanciaMasCercano = productoCruz[1];
		if(productoCruz[2]>0.0f && productoCruz[2]<distanciaMasCercano) distanciaMasCercano = productoCruz[2];
		if(productoCruz[3]>0.0f && productoCruz[3]<distanciaMasCercano) distanciaMasCercano = productoCruz[3];
		if(!((productoCruz[0]<0.0f && productoCruz[1]<0.0f && productoCruz[2]<0.0f && productoCruz[3]<0.0f) || (productoCruz[0]>=0.0f && productoCruz[1]>=0.0f && productoCruz[2]>=0.0f && productoCruz[3]>=0.0f))) distanciaMasCercano = 0.0f; //La linea atraviesa a la caja
		if(distanciaMasCercano<alcanceMaxPuas){ //Por lo menos un vertice esta cerca
			float productoPunto[4];
			productoPunto[0]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[0].x, vectorCaja[0].y);
			productoPunto[1]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[1].x, vectorCaja[1].y);
			productoPunto[2]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[2].x, vectorCaja[2].y);
			productoPunto[3]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitPuas.x, vecUnitPuas.y, vectorCaja[3].x, vectorCaja[3].y);
			if(!((productoPunto[0]<0.0f && productoPunto[1]<0.0f && productoPunto[2]<0.0f && productoPunto[3]<0.0f) || (productoPunto[0]>modVectorPuas && productoPunto[1]>modVectorPuas && productoPunto[2]>modVectorPuas && productoPunto[3]>modVectorPuas))){
				//Se ha confirmado contacto con la caja general de las puas
				//Ahora analizar cada pua individual
				float xLocalIniPua		= 0.0f;
				UI16 iPua, conteoPuas	= _puasAnimEstados.conteo;
				for(iPua=0; iPua<conteoPuas; iPua++){
					ENPuasLineaPropsAnim* datosPua = &_puasAnimEstados.elemento[iPua];
					if(datosPua->factorTamanoAct!=0.0f){
						float xLocalFinPua		= xLocalIniPua + _puaTexturaTamano.alto;
						float alturaLocalPua	= _puaTexturaTamano.ancho * datosPua->factorTamanoAct;
						NBPunto posEscenaIniPua; NBMATRIZ_MULTIPLICAR_PUNTO(posEscenaIniPua, _cacheObjEscena.matrizEscena, xLocalIniPua, 0.0f)
						NBPunto posEscenaFinPua; NBMATRIZ_MULTIPLICAR_PUNTO(posEscenaFinPua, _cacheObjEscena.matrizEscena, xLocalFinPua, 0.0f)
						NBPunto posEscenaAltoPua; NBMATRIZ_MULTIPLICAR_PUNTO(posEscenaAltoPua, _cacheObjEscena.matrizEscena, 0.0f, alturaLocalPua)
						float avanceMinEscena	= NBPUNTO_DISTANCIA(posIniEscena.x, posIniEscena.y, posEscenaIniPua.x, posEscenaIniPua.y);
						float avanceMaxEscena	= NBPUNTO_DISTANCIA(posIniEscena.x, posIniEscena.y, posEscenaFinPua.x, posEscenaFinPua.y);
						float alturaPuaEscena	= NBPUNTO_DISTANCIA(posIniEscena.x, posIniEscena.y, posEscenaAltoPua.x, posEscenaAltoPua.y);
						if(distanciaMasCercano>=0.0f && distanciaMasCercano<alturaPuaEscena){
							if(!((productoPunto[0]<avanceMinEscena && productoPunto[1]<avanceMinEscena && productoPunto[2]<avanceMinEscena && productoPunto[3]<avanceMinEscena) || (productoPunto[0]>avanceMaxEscena && productoPunto[1]>avanceMaxEscena && productoPunto[2]>avanceMaxEscena && productoPunto[3]>avanceMaxEscena))){
								//La caja tiene por lo menos un vertice dentro del rayo
								guardarDanoEn->factorDanoRecibido = 1.0f;
								float promedioPuntoCaja = (productoPunto[0] + productoPunto[1] + productoPunto[2] + productoPunto[3]) / 4.0f;
								guardarDanoEn->posEscenaOrigenDano.x = posIniEscena.x + (vecUnitPuas.x * promedioPuntoCaja);
								guardarDanoEn->posEscenaOrigenDano.y = posIniEscena.y + (vecUnitPuas.y * promedioPuntoCaja);
								existeDano = true;
								break;
							}
						}
					}
					xLocalIniPua += _puaTexturaTamano.alto + _puasSeparacion;
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return existeDano;
}

void AUEscenaPuasLineaAnimIndiv::tickAnimacion(float segsTrancurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLineaAnimIndiv::tickAnimacion")
	SI16 iPua, conteoPuas	= _puasAnimEstados.conteo;
	//Animacion general
	if(_puasAnimTipo==ENPuasLineaAnimIndiv_UnHueco){
		if((!_puasAnimUnHuecoVolviendo && _puasAnimUnHuecoXInicioHueco<_puasLongitudHaciaDerecha) || (_puasAnimUnHuecoVolviendo && _puasAnimUnHuecoXInicioHueco>-_puasAnimUnHuecoTamano)){
			NBASSERT(_puasAnimUnHuecoTamano>=0.0f)
			_puasAnimUnHuecoSegsAcumEspera	= 0.0f;
			_puasAnimUnHuecoXInicioHueco	+= ((_puasAnimUnHuecoVolviendo?-_puasAnimUnHuecoVelMov:_puasAnimUnHuecoVelMov) * segsTrancurridos);
			SI16 iPrimeraPuaHueco			= _puasAnimUnHuecoXInicioHueco / (_puaTexturaTamano.alto + _puasSeparacion); if(iPrimeraPuaHueco>conteoPuas) iPrimeraPuaHueco = conteoPuas;
			SI16 iPrimeraPuaDespHueco		= ((_puasAnimUnHuecoXInicioHueco + _puasAnimUnHuecoTamano) / (_puaTexturaTamano.alto + _puasSeparacion)) + 1; if(iPrimeraPuaDespHueco>conteoPuas) iPrimeraPuaDespHueco = conteoPuas;
			NBASSERT(iPrimeraPuaHueco<=conteoPuas)
			NBASSERT(iPrimeraPuaDespHueco<=conteoPuas)
			//Puas anteriores a hueco
			for(iPua=0; iPua<iPrimeraPuaHueco; iPua++){
				ENPuasLineaPropsAnim* datosPua = &_puasAnimEstados.elemento[iPua];
				if(datosPua->animEstapaActual==ENPuasLineaAnimEstado_Ocultas){
					datosPua->segsAcumEtapaActual	= 0.0f;
					datosPua->animEstapaActual		= ENPuasLineaAnimEstado_Showing;
				}
			}
			//Puas de hueco
			for(iPua=(iPrimeraPuaHueco<0?0:iPrimeraPuaHueco); iPua<iPrimeraPuaDespHueco; iPua++){
				ENPuasLineaPropsAnim* datosPua = &_puasAnimEstados.elemento[iPua];
				if(datosPua->animEstapaActual==ENPuasLineaAnimEstado_Mostradas){
					datosPua->segsAcumEtapaActual	= 0.0f;
					datosPua->animEstapaActual		= ENPuasLineaAnimEstado_Ocultando;
				}
			}
			//Puas posteriores a hueco
			for(iPua=(iPrimeraPuaDespHueco<0?0:iPrimeraPuaDespHueco); iPua<conteoPuas; iPua++){
				ENPuasLineaPropsAnim* datosPua = &_puasAnimEstados.elemento[iPua];
				if(datosPua->animEstapaActual==ENPuasLineaAnimEstado_Ocultas){
					datosPua->segsAcumEtapaActual	= 0.0f;
					datosPua->animEstapaActual		= ENPuasLineaAnimEstado_Showing;
				}
			}
		} else {
			_puasAnimUnHuecoSegsAcumEspera += segsTrancurridos;
			if(_puasAnimUnHuecoSegsAcumEspera>=_puasAnimUnHuecoSegsEsperar){
				_puasAnimUnHuecoSegsAcumEspera	= 0.0f;
				if(_puasAnimUnHuecoVolver) _puasAnimUnHuecoVolviendo = !_puasAnimUnHuecoVolviendo;
				_puasAnimUnHuecoXInicioHueco	= _puasAnimUnHuecoVolviendo?_puasLongitudHaciaDerecha:-_puasAnimUnHuecoTamano;
			}
		}
	} else { NBASSERT(false) }
	//Animacion individual
	for(iPua=0; iPua<conteoPuas; iPua++){
		ENPuasLineaPropsAnim* datosPua = &_puasAnimEstados.elemento[iPua];
		datosPua->segsAcumEtapaActual += segsTrancurridos;
		switch (datosPua->animEstapaActual) {
			case ENPuasLineaAnimEstado_Ocultando:
				if(datosPua->segsAcumEtapaActual<_puasSegsParaOcultar){
					datosPua->factorTamanoAct		= 1.0f - (datosPua->segsAcumEtapaActual / _puasSegsParaOcultar);
				} else {
					datosPua->factorTamanoAct		= 0.0f;
					datosPua->animEstapaActual		= ENPuasLineaAnimEstado_Ocultas;
					datosPua->segsAcumEtapaActual	= 0.0f;
				}
				break;
			case ENPuasLineaAnimEstado_Showing:
				if(datosPua->segsAcumEtapaActual<_puasSegsParaMostrar){
					datosPua->factorTamanoAct		= datosPua->segsAcumEtapaActual / _puasSegsParaMostrar;
				} else {
					datosPua->factorTamanoAct		= 1.0f;
					datosPua->animEstapaActual		= ENPuasLineaAnimEstado_Mostradas;
					datosPua->segsAcumEtapaActual	= 0.0f;
				}
				break;
			default:
				break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

PTRfunCmdsGL AUEscenaPuasLineaAnimIndiv::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLineaAnimIndiv::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	SI16 cantPuasCompletas					= _puasAnimEstados.conteo; NBASSERT(cantPuasCompletas!=0)
	if(cantPuasCompletas!=0) {
		NBColor8 colorMultiplicado8; NBCOLOR_ESTABLECER(colorMultiplicado8, propsHeredadas.colorPadre.r * _propiedades.color8.r, propsHeredadas.colorPadre.g * _propiedades.color8.g, propsHeredadas.colorPadre.b * _propiedades.color8.b, propsHeredadas.colorPadre.a * _propiedades.color8.a)
		STBloqueGL bloqueVerticesGL;
		bloqueVerticesGL.cantidadElementos	= cantPuasCompletas * 4;
		bloqueVerticesGL.primerElemento		= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL.cantidadElementos);
		//Arreglos de grupos
		float avanceEnLinea					= 0.0f;
		NBVerticeGL* verticesGL				= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]);
		UI16 iPua, conteoPuas				= cantPuasCompletas;
		for(iPua=0; iPua<conteoPuas; iPua++){
			ENPuasLineaPropsAnim* datosAnim	= &_puasAnimEstados.elemento[iPua];
			float xFinPua					= avanceEnLinea + _puaTexturaTamano.alto;
			float altoPua					= _puaTexturaTamano.ancho * datosAnim->factorTamanoAct;
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], _cacheObjEscena.matrizEscena, avanceEnLinea, 0.0f)
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[1], _cacheObjEscena.matrizEscena, avanceEnLinea, altoPua)
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[2], _cacheObjEscena.matrizEscena, xFinPua, 0.0f)
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[3], _cacheObjEscena.matrizEscena, xFinPua, altoPua)
			NBCOLOR_ESTABLECER(verticesGL[0], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a)
			NBCOLOR_ESTABLECER(verticesGL[1], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a)
			NBCOLOR_ESTABLECER(verticesGL[2], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a)
			NBCOLOR_ESTABLECER(verticesGL[3], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a)
			_puaTextura->cargarCoordenadasTextura(verticesGL, 1.0f - datosAnim->factorTamanoAct, 1.0f, 0.0f, 1.0f, ENTexturaModoCoordenadas_Entrelazadas);
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
	return (cantPuasCompletas!=0 ? &AUEscenaPuasLineaAnimIndiv::enviarComandosGL : NULL);
}

void AUEscenaPuasLineaAnimIndiv::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLineaAnimIndiv::enviarComandosGL")
	STPuasLineaRender* datosRender		= (STPuasLineaRender*)punteroDatosModelo;
	NBASSERT(datosRender->bloqueIndicesGL.cantidadElementos>0)
	NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaPuasLineaAnimIndiv")
	NBGestorGL::bindTexture(0, datosRender->idTexturaGL);
	NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(datosRender->bloqueIndicesGL.primerElemento, datosRender->bloqueIndicesGL.cantidadElementos); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaPuasLineaAnimIndiv")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

bool AUEscenaPuasLineaAnimIndiv::agregarXmlInternoEn(AUEscenaPuasLineaAnimIndiv* lineaParts, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLineaAnimIndiv::agregarXmlInternoEn")
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
	guardarEn->agregarConFormato("%s<hTam>%f</hTam>\r\n", espaciosBaseIzq, lineaParts->_puasAnimUnHuecoTamano);
	guardarEn->agregarConFormato("%s<hVel>%f</hVel>\r\n", espaciosBaseIzq, lineaParts->_puasAnimUnHuecoVelMov);
	guardarEn->agregarConFormato("%s<hEsp>%f</hEsp>\r\n", espaciosBaseIzq, lineaParts->_puasAnimUnHuecoSegsEsperar);
	guardarEn->agregarConFormato("%s<hVol>%d</hVol>\r\n", espaciosBaseIzq, lineaParts->_puasAnimUnHuecoVolver?1:0);
	guardarEn->agregarConFormato("%s<hVolI>%d</hVolI>\r\n", espaciosBaseIzq, lineaParts->_puasAnimUnHuecoIniciarVolviendo?1:0);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
	
}

bool AUEscenaPuasLineaAnimIndiv::interpretarXmlInterno(AUEscenaPuasLineaAnimIndiv* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLineaAnimIndiv::interpretarXmlInterno")
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
		cargarEn->_puasAnimUnHuecoTamano	= datosXml->nodoHijo("hTam", 64.0f, nodoXml);
		cargarEn->_puasAnimUnHuecoVelMov	= datosXml->nodoHijo("hVel", 64.0f, nodoXml);
		cargarEn->_puasAnimUnHuecoSegsEsperar = datosXml->nodoHijo("hEsp", 3.0f, nodoXml);
		cargarEn->_puasAnimUnHuecoVolver	= datosXml->nodoHijo("hVol", true, nodoXml);
		cargarEn->_puasAnimUnHuecoIniciarVolviendo	= datosXml->nodoHijo("hVolI", false, nodoXml);
		cargarEn->establecerPuasTamano((ENPuasLineaTamano)datosXml->nodoHijo("tam", 0, nodoXml));
		cargarEn->privPuasActualizar();
		exito = true;
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaPuasLineaAnimIndiv::agregarBitsInternosEn(AUEscenaPuasLineaAnimIndiv* lineaParts, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLineaAnimIndiv::agregarBitsInternosEn")
	bool exito = false;
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(lineaParts, guardarEn)){
		guardarEn->escribir(&lineaParts->_puasSeparacion, sizeof(lineaParts->_puasSeparacion), 1, guardarEn);
		SI32 tamPuas = lineaParts->_puasTipoTamano; guardarEn->escribir(&tamPuas, sizeof(tamPuas), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_puasLongitudHaciaDerecha, sizeof(lineaParts->_puasLongitudHaciaDerecha), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_puasSegsParaOcultar, sizeof(lineaParts->_puasSegsParaOcultar), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_puasSegsParaMostrar, sizeof(lineaParts->_puasSegsParaMostrar), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_puasAnimUnHuecoTamano, sizeof(lineaParts->_puasAnimUnHuecoTamano), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_puasAnimUnHuecoVelMov, sizeof(lineaParts->_puasAnimUnHuecoVelMov), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_puasAnimUnHuecoSegsEsperar, sizeof(lineaParts->_puasAnimUnHuecoSegsEsperar), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_puasAnimUnHuecoVolver, sizeof(lineaParts->_puasAnimUnHuecoVolver), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_puasAnimUnHuecoIniciarVolviendo, sizeof(lineaParts->_puasAnimUnHuecoIniciarVolviendo), 1, guardarEn);
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaPuasLineaAnimIndiv::interpretarBitsInternos(AUEscenaPuasLineaAnimIndiv* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaPuasLineaAnimIndiv::interpretarBitsInternos")
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
			cargarDe->leer(&cargarEn->_puasAnimUnHuecoTamano, sizeof(cargarEn->_puasAnimUnHuecoTamano), 1, cargarDe);
			cargarDe->leer(&cargarEn->_puasAnimUnHuecoVelMov, sizeof(cargarEn->_puasAnimUnHuecoVelMov), 1, cargarDe);
			cargarDe->leer(&cargarEn->_puasAnimUnHuecoSegsEsperar, sizeof(cargarEn->_puasAnimUnHuecoSegsEsperar), 1, cargarDe);
			cargarDe->leer(&cargarEn->_puasAnimUnHuecoVolver, sizeof(cargarEn->_puasAnimUnHuecoVolver), 1, cargarDe);
			cargarDe->leer(&cargarEn->_puasAnimUnHuecoIniciarVolviendo, sizeof(cargarEn->_puasAnimUnHuecoIniciarVolviendo), 1, cargarDe);
			cargarEn->privPuasActualizar();
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUEscenaPuasLineaAnimIndiv, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaPuasLineaAnimIndiv, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaPuasLineaAnimIndiv, "AUEscenaPuasLineaAnimIndiv", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaPuasLineaAnimIndiv)






