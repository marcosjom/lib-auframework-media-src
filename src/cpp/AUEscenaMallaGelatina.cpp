//
//  AUEscenaMallaGelatinaAnimada.cpp
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 24/01/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaMallaGelatina.h"

AUEscenaMallaGelatina::AUEscenaMallaGelatina() : AUEscenaMalla(), _verticesGelatina(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::AUEscenaMallaGelatina")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMallaGelatina")
	_verticesAnimar						= ENMallaGelatinaVertices_Todos;
	NBCajaAABB cajaMallaOriginal;		NBCAJAAABB_INICIALIZAR(cajaMallaOriginal);
	UI32 iVertice, iVerticeConteo = this->_verticesLocales.conteo;
	for(iVertice=0; iVertice<iVerticeConteo; iVertice++){
		STMallaGelatinaVertice vertGel;
		vertGel.coordNormal	= this->_verticesLocales.elemento[iVertice].coordenada;
		vertGel.coordDestino = vertGel.coordNormal;
		vertGel.factorMovimiento = 1.0f;
		_verticesGelatina.agregarElemento(vertGel);
		NBCAJAAABB_ENVOLVER_PUNTO(cajaMallaOriginal, vertGel.coordNormal.x, vertGel.coordNormal.y)
		
	}
	//Parametros de movimiento
	NBTamano tamCajaMallaOriginal;		NBCAJAAABB_TAMANO(tamCajaMallaOriginal, cajaMallaOriginal);
	NBTAMANO_ESTABLECER(_desplazamientoMaximo, (tamCajaMallaOriginal.ancho/(float)this->_columnas*0.10f), tamCajaMallaOriginal.alto/(float)this->_filas*0.10f)
	NBTAMANO_ESTABLECER(_velocidadMaxima, _desplazamientoMaximo.ancho * 0.25f, _desplazamientoMaximo.alto * 0.25f);
	NBCAJAAABB_CENTRO(_centroCalculoDesplazamiento, cajaMallaOriginal);
	_radioCalculoDesplazamiento			= 64.0f;
	_metodoCalculoDesplazamiento		= ENMallaGelatinaCalculo_Uniforme;
	//
	this->reanudarAnimacion();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaMallaGelatina::AUEscenaMallaGelatina(UI8 columnas, UI8 filas, AUArregloNativoP<NBPuntoColor>* verticesLocales, AUArregloNativoP<AUTextura*>* texturas, AUArregloNativoP<NBRectangulo>* areasTex) : AUEscenaMalla(columnas, filas, verticesLocales, texturas, areasTex), _verticesGelatina(this, verticesLocales->conteo) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::AUEscenaMallaGelatina")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMallaGelatina")
	_verticesAnimar						= ENMallaGelatinaVertices_Todos;
	NBCajaAABB cajaMallaOriginal;		NBCAJAAABB_INICIALIZAR(cajaMallaOriginal);
	UI32 iVertice, iVerticeConteo = this->_verticesLocales.conteo;
	for(iVertice=0; iVertice<iVerticeConteo; iVertice++){
		STMallaGelatinaVertice vertGel;
		vertGel.coordNormal	= this->_verticesLocales.elemento[iVertice].coordenada;
		vertGel.coordDestino = vertGel.coordNormal;
		vertGel.factorMovimiento = 1.0f;
		_verticesGelatina.agregarElemento(vertGel);
		NBCAJAAABB_ENVOLVER_PUNTO(cajaMallaOriginal, vertGel.coordNormal.x, vertGel.coordNormal.y)
		
	}
	//Parametros de movimiento
	NBTamano tamCajaMallaOriginal;		NBCAJAAABB_TAMANO(tamCajaMallaOriginal, cajaMallaOriginal);
	NBTAMANO_ESTABLECER(_desplazamientoMaximo, (tamCajaMallaOriginal.ancho/(float)this->_columnas*0.10f), tamCajaMallaOriginal.alto/(float)this->_filas*0.10f)
	NBTAMANO_ESTABLECER(_velocidadMaxima, _desplazamientoMaximo.ancho * 0.25f, _desplazamientoMaximo.alto * 0.25f);
	NBCAJAAABB_CENTRO(_centroCalculoDesplazamiento, cajaMallaOriginal);
	_radioCalculoDesplazamiento			= 64.0f;
	_metodoCalculoDesplazamiento		= ENMallaGelatinaCalculo_Uniforme;
	//
	this->reanudarAnimacion();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaMallaGelatina::AUEscenaMallaGelatina(AUEscenaMallaGelatina* otraInstancia) : AUEscenaMalla(otraInstancia), _verticesGelatina(this, &otraInstancia->_verticesGelatina) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::AUEscenaMallaGelatina")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMallaGelatina")
	_verticesAnimar						= otraInstancia->_verticesAnimar;
	//Parametros de movimiento
	_desplazamientoMaximo				= otraInstancia->_desplazamientoMaximo;
	_velocidadMaxima					= otraInstancia->_velocidadMaxima;
	_centroCalculoDesplazamiento		= otraInstancia->_centroCalculoDesplazamiento;
	_radioCalculoDesplazamiento			= otraInstancia->_radioCalculoDesplazamiento;
	_metodoCalculoDesplazamiento		= otraInstancia->_metodoCalculoDesplazamiento;
	//
	this->reanudarAnimacion();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaMallaGelatina::~AUEscenaMallaGelatina(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::~AUEscenaMallaGelatina")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaGelatina::establecerVerticesAnimar(ENMallaGelatinaVertices verticesAnimar){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::establecerVerticesAnimar")
	_verticesAnimar = verticesAnimar;
	//Actualizar factores de movimientos de los vertices
	privInlineActualizaFactoresMovimientosVertices();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaGelatina::establecerVelocidadMaxima(float xVelMax, float yVelMax){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::establecerVelocidadMaxima")
	_velocidadMaxima.ancho = xVelMax;
	_velocidadMaxima.alto = yVelMax;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaGelatina::establecerDesplazamientoMaximo(float anchoDesplazamientoMax, float altoDesplazamientoMax){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::establecerDesplazamientoMaximo")
	_desplazamientoMaximo.ancho = anchoDesplazamientoMax;
	_desplazamientoMaximo.alto = altoDesplazamientoMax;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaGelatina::establecerMetodoCalculoDesplazamiento(ENMallaGelatinaCalculo metodoCalculoDesplazamiento){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::establecerCalculoDesplazamiento")
	_metodoCalculoDesplazamiento		= metodoCalculoDesplazamiento;
	//Actualizar factores de movimientos de los vertices
	privInlineActualizaFactoresMovimientosVertices();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaGelatina::establecerCentroCalculoDesplazamiento(float centroX, float centroY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::establecerCentroCalculoDesplazamiento")
	_centroCalculoDesplazamiento.x		= centroX;
	_centroCalculoDesplazamiento.y		= centroY;
	//Actualizar factores de movimientos de los vertices
	privInlineActualizaFactoresMovimientosVertices();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaGelatina::establecerRadioCalculoDesplazamiento(float radio){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::establecerRadioCalculoDesplazamiento")
	_radioCalculoDesplazamiento		= radio;
	//Actualizar factores de movimientos de los vertices
	privInlineActualizaFactoresMovimientosVertices();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaGelatina::establecerCalculoDesplazamiento(ENMallaGelatinaCalculo metodoCalculoDesplazamiento, float centroX, float centroY, float radio){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::establecerCalculoDesplazamiento")
	_metodoCalculoDesplazamiento		= metodoCalculoDesplazamiento;
	_centroCalculoDesplazamiento.x		= centroX;
	_centroCalculoDesplazamiento.y		= centroY;
	_radioCalculoDesplazamiento			= radio;
	//Actualizar factores de movimientos de los vertices
	privInlineActualizaFactoresMovimientosVertices();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaGelatina::establecerDesplazamientoMaximoSegunFactor(float factorX, float factorY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::establecerDesplazamientoMaximoSegunFactor")
	NBCajaAABB cajaMallaOriginal;		NBCAJAAABB_INICIALIZAR(cajaMallaOriginal); 
	UI32 iVert, iVertConteo = _verticesGelatina.conteo;
	for(iVert=0; iVert<iVertConteo; iVert++){
		NBPunto vNormal = _verticesGelatina.elemento[iVert].coordNormal;
		NBCAJAAABB_ENVOLVER_PUNTO(cajaMallaOriginal, vNormal.x, vNormal.y)
	}
	NBTamano tamCajaMallaOriginal;		NBCAJAAABB_TAMANO(tamCajaMallaOriginal, cajaMallaOriginal);
	NBTAMANO_ESTABLECER(_desplazamientoMaximo, (tamCajaMallaOriginal.ancho/(float)this->_columnas*factorX), tamCajaMallaOriginal.alto/(float)this->_filas*factorY)
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaGelatina::establecerVelocidadMaximaSegunFactor(float factorX, float factorY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::establecerVelocidadMaximaSegunFactor")
	NBCajaAABB cajaMallaOriginal;		NBCAJAAABB_INICIALIZAR(cajaMallaOriginal);
	UI32 iVert, iVertConteo = _verticesGelatina.conteo;
	for(iVert=0; iVert<iVertConteo; iVert++){
		NBPunto vNormal = _verticesGelatina.elemento[iVert].coordNormal;
		NBCAJAAABB_ENVOLVER_PUNTO(cajaMallaOriginal, vNormal.x, vNormal.y)
	}
	NBTamano tamCajaMallaOriginal;		NBCAJAAABB_TAMANO(tamCajaMallaOriginal, cajaMallaOriginal);
	NBTAMANO_ESTABLECER(_velocidadMaxima, (tamCajaMallaOriginal.ancho/(float)this->_columnas*factorX), tamCajaMallaOriginal.alto/(float)this->_filas*factorY)
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaGelatina::moverVerticeReposo(UI32 iVertice, float posX, float posY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::moverVerticeReposo")
	NBASSERT(iVertice<_verticesGelatina.conteo)
	_verticesGelatina.elemento[iVertice].coordNormal.x = posX;
	_verticesGelatina.elemento[iVertice].coordNormal.y = posY;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

NBPunto AUEscenaMallaGelatina::verticeReposo(UI32 iVertice) const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::verticeReposo")
	NBASSERT(iVertice<_verticesGelatina.conteo)
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _verticesGelatina.elemento[iVertice].coordNormal;
}

void AUEscenaMallaGelatina::privInlineActualizaFactoresMovimientosVertices(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::privInlineActualizaFactoresMovimientosVertices")
	NBASSERT(_verticesGelatina.conteo==this->_verticesLocales.conteo)
	UI16 iVertice, conteoVertices = _verticesGelatina.conteo;
	for(iVertice=0; iVertice<conteoVertices; iVertice++){
		STMallaGelatinaVertice verticeGel = _verticesGelatina.elemento[iVertice];
		float factorMovimiento = 0.0f;
		bool moverVertice; 
		if(_verticesAnimar==ENMallaGelatinaVertices_Todos){
			moverVertice = true;
		} else if(_verticesAnimar==ENMallaGelatinaVertices_Internos){
			moverVertice = !(iVertice<=this->_filas || iVertice>=_verticesLocales.conteo-this->_filas-1 || (iVertice%(this->_filas+1))==0 || ((iVertice+1)%(this->_filas+1))==0);
		} else if(_verticesAnimar==ENMallaGelatinaVertices_Bordes){
			moverVertice = (iVertice<=this->_filas || iVertice>=_verticesLocales.conteo-this->_filas-1 || (iVertice%(this->_filas+1))==0 || ((iVertice+1)%(this->_filas+1))==0);
		} else if(_verticesAnimar==ENMallaGelatinaVertices_BordesSuperior){
			moverVertice = (iVertice<=this->_columnas);
		} else if(_verticesAnimar==ENMallaGelatinaVertices_Esquinas){
			moverVertice = (iVertice==0 || iVertice==this->_columnas || iVertice==(((this->_filas+1)*(this->_columnas+1)-(this->_columnas+1))) || iVertice==(((this->_filas+1)*(this->_columnas+1)-1)));
		} else {
			NBASSERT(false) //no implementado o no valido
		}
		if(moverVertice){
			if(_metodoCalculoDesplazamiento==ENMallaGelatinaCalculo_Uniforme || _radioCalculoDesplazamiento<=0.01f){
				factorMovimiento = 1.0f;
			} else {
				float distancia = NBPUNTO_DISTANCIA(verticeGel.coordNormal.x, verticeGel.coordNormal.y, _centroCalculoDesplazamiento.x, _centroCalculoDesplazamiento.y);
				if(distancia>_radioCalculoDesplazamiento) distancia = _radioCalculoDesplazamiento;
				if(_metodoCalculoDesplazamiento==ENMallaGelatinaCalculo_CentroEsCero){
					factorMovimiento = (distancia/_radioCalculoDesplazamiento);
				} else if(_metodoCalculoDesplazamiento==ENMallaGelatinaCalculo_CentroEsMaximo){
					factorMovimiento = 1.0f - (distancia/_radioCalculoDesplazamiento);
				} else {
					NBASSERT(false) //Metodo de calculo no definido
				}
				NBASSERT(factorMovimiento>=0.0f && factorMovimiento<=1.0f)
			}
		}
		if(factorMovimiento==0.0f){
			_verticesGelatina.elemento[iVertice].coordDestino = _verticesGelatina.elemento[iVertice].coordNormal;
			this->_verticesLocales.elemento[iVertice].coordenada = _verticesGelatina.elemento[iVertice].coordNormal;
		}
		_verticesGelatina.elemento[iVertice].factorMovimiento = factorMovimiento;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUArregloNativoP<STMallaGelatinaVertice>* AUEscenaMallaGelatina::verticesGelatina(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::verticesGelatina")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return &_verticesGelatina;
}

ENMallaGelatinaVertices AUEscenaMallaGelatina::verticesAnimados() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::verticesAnimados")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (ENMallaGelatinaVertices)_verticesAnimar;
}

NBTamano AUEscenaMallaGelatina::velocidadMaxima() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::velocidadMaxima")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _velocidadMaxima;
}

NBTamano AUEscenaMallaGelatina::desplazamientoMaximo() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::desplazamientoMaximo")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _desplazamientoMaximo;
}

NBPunto AUEscenaMallaGelatina::centroCalculoDesplazamiento() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::centroCalculoDesplazamiento")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _centroCalculoDesplazamiento;
}

float AUEscenaMallaGelatina::radioCalculoDesplazamiento() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::radioCalculoDesplazamiento")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _radioCalculoDesplazamiento;
}

ENMallaGelatinaCalculo AUEscenaMallaGelatina::metodoCalculoDesplazamiento() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::metodoCalculoDesplazamiento")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (ENMallaGelatinaCalculo)_metodoCalculoDesplazamiento;
}

void AUEscenaMallaGelatina::desplazarCoordenadasVertices(SI32 desplazamientoFilas, SI32 desplazamientoColumnas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::desplazarCoordenadasVertices")
	AUArregloNativoMutableP<NBPuntoColor>* copiaPosActuales	= (AUArregloNativoMutableP<NBPuntoColor>*)this->_verticesLocales.clonar(this->tipoMemoriaResidencia(), ENClonadoTipo_ClonarInstancias);
	AUArregloNativoMutableP<STMallaGelatinaVertice>* copiaPosDestinos	= (AUArregloNativoMutableP<STMallaGelatinaVertice>*)this->_verticesGelatina.clonar(this->tipoMemoriaResidencia(), ENClonadoTipo_ClonarInstancias);
	//
	UI32 iVertice, iVerticeConteo = this->_verticesLocales.conteo;
	for(iVertice=0; iVertice<iVerticeConteo; iVertice++){
		SI32 iVerticeOrig			= iVertice - (desplazamientoFilas * (this->_columnas+1)) - desplazamientoColumnas;
		if(iVerticeOrig<0 || iVerticeOrig>=(SI32)iVerticeConteo){
			this->_verticesLocales.elemento[iVertice].coordenada	= this->_verticesGelatina.elemento[iVertice].coordNormal;
			this->_verticesGelatina.elemento[iVertice].coordDestino	= this->_verticesGelatina.elemento[iVertice].coordNormal;
		} else {
			this->_verticesLocales.elemento[iVertice].coordenada.x = this->_verticesGelatina.elemento[iVertice].coordNormal.x + (copiaPosActuales->elemento[iVerticeOrig].coordenada.x - this->_verticesGelatina.elemento[iVerticeOrig].coordNormal.x);
			this->_verticesLocales.elemento[iVertice].coordenada.y = this->_verticesGelatina.elemento[iVertice].coordNormal.y + (copiaPosActuales->elemento[iVerticeOrig].coordenada.y - this->_verticesGelatina.elemento[iVerticeOrig].coordNormal.y);
			this->_verticesGelatina.elemento[iVertice].coordDestino.x = this->_verticesGelatina.elemento[iVertice].coordNormal.x + (copiaPosDestinos->elemento[iVerticeOrig].coordDestino.x - this->_verticesGelatina.elemento[iVerticeOrig].coordNormal.x);
			this->_verticesGelatina.elemento[iVertice].coordDestino.y = this->_verticesGelatina.elemento[iVertice].coordNormal.y + (copiaPosDestinos->elemento[iVerticeOrig].coordDestino.y - this->_verticesGelatina.elemento[iVerticeOrig].coordNormal.y);
		}
	}
	copiaPosActuales->liberar(NB_RETENEDOR_THIS);
	copiaPosDestinos->liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaGelatina::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMallaGelatina::agregadoEnEscena")
	NBGestorAnimadores::agregarAnimador(this, this);
	AUEscenaMalla::agregadoEnEscena();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaMallaGelatina::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMallaGelatina::quitandoDeEscena")
	AUEscenaMalla::quitandoDeEscena();
	NBGestorAnimadores::quitarAnimador(this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaMallaGelatina::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::tickAnimacion")
	NBASSERT(this->idEscena!=-1)
	if(this->_dormido){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3() }
	UI32 iVertice, iVerticeConteo = this->_verticesLocales.conteo;
	for(iVertice=0; iVertice<iVerticeConteo; iVertice++){
		STMallaGelatinaVertice verticeGel = _verticesGelatina.elemento[iVertice];
		if(this->_verticesLocales.elemento[iVertice].coordenada==verticeGel.coordDestino){
			//Determinar nuevo destino de este vertice
			if(verticeGel.factorMovimiento==0.0f){
				_verticesGelatina.elemento[iVertice].coordDestino = _verticesGelatina.elemento[iVertice].coordNormal;
			} else {
				float xRel = (float)(rand() % 1000) / ((rand() % 2)==0?1000.0f:-1000.0f);
				float yRel = (float)(rand() % 1000) / ((rand() % 2)==0?1000.0f:-1000.0f);
				_verticesGelatina.elemento[iVertice].coordDestino.x = verticeGel.coordNormal.x+(_desplazamientoMaximo.ancho * verticeGel.factorMovimiento * xRel);
				_verticesGelatina.elemento[iVertice].coordDestino.y = verticeGel.coordNormal.y+(_desplazamientoMaximo.alto * verticeGel.factorMovimiento * yRel);
			}
		} else {
			NBPunto nuevaPos = this->_verticesLocales.elemento[iVertice].coordenada;
			if(verticeGel.coordDestino.x>this->_verticesLocales.elemento[iVertice].coordenada.x){
				nuevaPos.x += (_velocidadMaxima.ancho * verticeGel.factorMovimiento * segsTranscurridos);
				if(nuevaPos.x>verticeGel.coordDestino.x) nuevaPos.x = verticeGel.coordDestino.x;
			} else if(verticeGel.coordDestino.x<this->_verticesLocales.elemento[iVertice].coordenada.x){
				nuevaPos.x -= (_velocidadMaxima.ancho * verticeGel.factorMovimiento * segsTranscurridos);
				if(nuevaPos.x<verticeGel.coordDestino.x) nuevaPos.x = verticeGel.coordDestino.x;
			}
			if(verticeGel.coordDestino.y>this->_verticesLocales.elemento[iVertice].coordenada.y){
				nuevaPos.y += (_velocidadMaxima.alto * verticeGel.factorMovimiento * segsTranscurridos);
				if(nuevaPos.y>verticeGel.coordDestino.y) nuevaPos.y = verticeGel.coordDestino.y;
			} else if(verticeGel.coordDestino.y<this->_verticesLocales.elemento[iVertice].coordenada.y){
				nuevaPos.y -= (_velocidadMaxima.alto * verticeGel.factorMovimiento * segsTranscurridos);
				if(nuevaPos.y<verticeGel.coordDestino.y) nuevaPos.y = verticeGel.coordDestino.y;
			}
			this->_verticesLocales.elemento[iVertice].coordenada = nuevaPos;
			AUESCENAMALLA_ENSUCIAR_MODELO
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

AUEscenaMallaGelatina* AUEscenaMallaGelatina::crearMallaGelatinaSprite(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaSprite, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::crearMallaGelatinaSprite")
	AUArregloNativoMutableP<NBPuntoColor>* vertices	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBPuntoColor>();
	AUArregloNativoMutableP<AUTextura*>* texturas	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<AUTextura*>();
	AUArregloNativoMutableP<NBRectangulo>* areasTex	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBRectangulo>();
	NB_DEFINE_NOMBRE_PUNTERO(vertices, "AUEscenaMalla::vertices");
	NB_DEFINE_NOMBRE_PUNTERO(texturas, "AUEscenaMalla::texturas");
	NB_DEFINE_NOMBRE_PUNTERO(areasTex, "AUEscenaMalla::areasTex");
	//
	AUEscenaMalla::llenarParametrosMallaSprite(texturaSprite, columnas, filas, vertices, texturas, areasTex, alineacionH, alineacionV);
	AUEscenaMallaGelatina* nuevaMalla = new(tipoMemoriaResidencia) AUEscenaMallaGelatina(columnas, filas, vertices, texturas, areasTex);
	nuevaMalla->autoLiberar(NB_RETENEDOR_NULL);
	//
	vertices->liberar(NB_RETENEDOR_NULL);
	texturas->liberar(NB_RETENEDOR_NULL);
	areasTex->liberar(NB_RETENEDOR_NULL);
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return nuevaMalla;
}

AUEscenaMallaGelatina* AUEscenaMallaGelatina::crearMallaGelatinaPatron(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaRepetir, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::crearMallaGelatinaPatron")
	AUArregloNativoMutableP<NBPuntoColor>* vertices	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBPuntoColor>();
	AUArregloNativoMutableP<AUTextura*>* texturas	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<AUTextura*>();
	AUArregloNativoMutableP<NBRectangulo>* areasTex	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBRectangulo>();
	NB_DEFINE_NOMBRE_PUNTERO(vertices, "AUEscenaMalla::vertices");
	NB_DEFINE_NOMBRE_PUNTERO(texturas, "AUEscenaMalla::texturas");
	NB_DEFINE_NOMBRE_PUNTERO(areasTex, "AUEscenaMalla::areasTex");
	//
	AUEscenaMalla::llenarParametrosMallaPatron(texturaRepetir, columnas, filas, vertices, texturas, areasTex, alineacionH, alineacionV);
	AUEscenaMallaGelatina* nuevaMalla = new(tipoMemoriaResidencia) AUEscenaMallaGelatina(columnas, filas, vertices, texturas, areasTex);
	nuevaMalla->autoLiberar(NB_RETENEDOR_NULL);
	//
	vertices->liberar(NB_RETENEDOR_NULL);
	texturas->liberar(NB_RETENEDOR_NULL);
	areasTex->liberar(NB_RETENEDOR_NULL);
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return nuevaMalla;
}

AUEscenaMallaGelatina* AUEscenaMallaGelatina::crearMallaGelatinaDesdeArchivoXml(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaArchivoMalla, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::crearMallaGelatinaDesdeArchivoXml")
	AUArregloNativoMutableP<NBPuntoColor>* vertices	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBPuntoColor>();
	AUArregloNativoMutableP<AUTextura*>* texturas	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<AUTextura*>();
	AUArregloNativoMutableP<NBRectangulo>* areasTex	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBRectangulo>();
	NB_DEFINE_NOMBRE_PUNTERO(vertices, "AUEscenaMalla::vertices");
	NB_DEFINE_NOMBRE_PUNTERO(texturas, "AUEscenaMalla::texturas");
	NB_DEFINE_NOMBRE_PUNTERO(areasTex, "AUEscenaMalla::areasTex");
	//
	UI8 columnas, filas;
	AUEscenaMalla::llenarParametrosMallaDesdeArchivoXML(rutaArchivoMalla, &columnas, &filas, vertices, texturas, areasTex, alineacionH, alineacionV);
	AUEscenaMallaGelatina* nuevaMalla = new(tipoMemoriaResidencia) AUEscenaMallaGelatina(columnas, filas, vertices, texturas, areasTex);
	nuevaMalla->autoLiberar(NB_RETENEDOR_NULL);
	//
	vertices->liberar(NB_RETENEDOR_NULL);
	texturas->liberar(NB_RETENEDOR_NULL);
	areasTex->liberar(NB_RETENEDOR_NULL);
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return nuevaMalla;
}

//

bool AUEscenaMallaGelatina::agregarXmlInternoEn(AUEscenaMallaGelatina* malla, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaMalla::agregarXmlInternoEn(malla, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	//Datos de gelatina
	guardarEn->agregarConFormato("%s<vertsAnimar>%d</vertsAnimar>\r\n", espaciosBaseIzq, (SI32)malla->_verticesAnimar);
	guardarEn->agregarConFormato("%s<metCalcDesp>%d</metCalcDesp>\r\n", espaciosBaseIzq, (SI32)malla->_metodoCalculoDesplazamiento);
	guardarEn->agregarConFormato("%s<velMaxX>%f</velMaxX><velMaxY>%f</velMaxY>\r\n", espaciosBaseIzq, malla->_velocidadMaxima.ancho, malla->_velocidadMaxima.alto);
	guardarEn->agregarConFormato("%s<despMaxX>%f</despMaxX><despMaxY>%f</despMaxY>\r\n", espaciosBaseIzq, malla->_desplazamientoMaximo.ancho, malla->_desplazamientoMaximo.alto);
	guardarEn->agregarConFormato("%s<centCalcDespX>%f</centCalcDespX><centCalcDespY>%f</centCalcDespY><radioCalcDesp>%f</radioCalcDesp>\r\n", espaciosBaseIzq, malla->_centroCalculoDesplazamiento.x, malla->_centroCalculoDesplazamiento.y, espaciosBaseIzq, malla->_radioCalculoDesplazamiento);
	guardarEn->agregarConFormato("%s<verticesGelatina>\r\n", espaciosBaseIzq);
	UI32 i, iConteo = malla->_verticesGelatina.conteo;
	for(i=0; i<iConteo; i++){
		STMallaGelatinaVertice verticeGel = malla->_verticesGelatina.elemento[i];
		//Formato nuevo CSV (mas compacto)
		guardarEn->agregarConFormato("%s  <v>%f|%f|%f|%f|%f</v>\r\n", espaciosBaseIzq, verticeGel.coordNormal.x, verticeGel.coordNormal.y, verticeGel.coordDestino.x, verticeGel.coordDestino.y, verticeGel.factorMovimiento);
		//Formato viejo XML (muy inflado)
		//guardarEn->agregarConFormato("%s  <v><xN>%f</xN><yN>%f</yN><xD>%f</xD><yD>%f</yD><fM>%f</fM></v>\r\n", espaciosBaseIzq, verticeGel.coordNormal.x, verticeGel.coordNormal.y, verticeGel.coordDestino.x, verticeGel.coordDestino.y, verticeGel.factorMovimiento);
	}
	guardarEn->agregarConFormato("%s</verticesGelatina>\r\n", espaciosBaseIzq);
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUEscenaMallaGelatina::interpretarXmlInterno(AUEscenaMallaGelatina* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
		const sNodoXML* nodoEscenaObj;	XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
		if(nodoEscenaObj != NULL){
			if(AUEscenaMalla::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj)){
				//Vaciar arreglos
				cargarEn->_verticesGelatina.vaciar();
				//Cargar datos XML
				cargarEn->_verticesAnimar					= datosXml->nodoHijo("vertsAnimar", 0, nodoXml);
				cargarEn->_metodoCalculoDesplazamiento		= datosXml->nodoHijo("metCalcDesp", 0, nodoXml);
				cargarEn->_velocidadMaxima.ancho			= datosXml->nodoHijo("velMaxX", 0.0f, nodoXml);
				cargarEn->_velocidadMaxima.alto				= datosXml->nodoHijo("velMaxY", 0.0f, nodoXml);
				cargarEn->_desplazamientoMaximo.ancho		= datosXml->nodoHijo("despMaxX", 0.0f, nodoXml);
				cargarEn->_desplazamientoMaximo.alto		= datosXml->nodoHijo("despMaxY", 0.0f, nodoXml);
				cargarEn->_centroCalculoDesplazamiento.x	= datosXml->nodoHijo("centCalcDespX", 0.0f, nodoXml);
				cargarEn->_centroCalculoDesplazamiento.y	= datosXml->nodoHijo("centCalcDespY", 0.0f, nodoXml);
				cargarEn->_radioCalculoDesplazamiento		= datosXml->nodoHijo("radioCalcDesp", 0.0f, nodoXml);
				const sNodoXML* nodoVertices = datosXml->nodoHijo("verticesGelatina", nodoXml);
				if(nodoVertices != NULL){
					const sNodoXML* nodoV = datosXml->nodoHijo("v", nodoVertices);
					while(nodoV != NULL){
						STMallaGelatinaVertice nuevoVertice;
						if(nodoV->nodosHijos == NULL){
							//Formato nuevo CSV (mas compacto)
							const char* cadenaCSV				= datosXml->cadenaValorDeNodo(nodoV);
							SI32 tamCadenaCSV					= AUCadena8::tamano(cadenaCSV);
							SI32 posEnCadenaCSV					= 0;
							nuevoVertice.coordNormal.x			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							nuevoVertice.coordNormal.y			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							nuevoVertice.coordDestino.x			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							nuevoVertice.coordDestino.y			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							nuevoVertice.factorMovimiento		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						} else {
							//Formato viejo XML (muy inflado)
							nuevoVertice.coordNormal.x			= datosXml->nodoHijo("xN", 0.0f, nodoV);
							nuevoVertice.coordNormal.y			= datosXml->nodoHijo("yN", 0.0f, nodoV);
							nuevoVertice.coordDestino.x			= datosXml->nodoHijo("xD", 0.0f, nodoV);
							nuevoVertice.coordDestino.y			= datosXml->nodoHijo("yD", 0.0f, nodoV);
							nuevoVertice.factorMovimiento		= datosXml->nodoHijo("fM", 0.0f, nodoV);
						}
						cargarEn->_verticesGelatina.agregarElemento(nuevoVertice);
						//siguiente
						nodoV = datosXml->nodoHijo("v", nodoVertices, nodoV);
					}
				}
				exito = true;
				AUESCENAMALLA_ENSUCIAR_MODELO_DE(cargarEn);
			}
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaMallaGelatina::agregarBitsInternosEn(AUEscenaMallaGelatina* malla, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaMalla::agregarBitsInternosEn(malla, guardarEn)){
		guardarEn->escribir(&malla->_verticesAnimar, sizeof(malla->_verticesAnimar), 1, guardarEn);
		guardarEn->escribir(&malla->_metodoCalculoDesplazamiento, sizeof(malla->_metodoCalculoDesplazamiento), 1, guardarEn);
		guardarEn->escribir(&malla->_velocidadMaxima, sizeof(malla->_velocidadMaxima), 1, guardarEn);
		guardarEn->escribir(&malla->_desplazamientoMaximo, sizeof(malla->_desplazamientoMaximo), 1, guardarEn);
		guardarEn->escribir(&malla->_centroCalculoDesplazamiento, sizeof(malla->_centroCalculoDesplazamiento), 1, guardarEn);
		guardarEn->escribir(&malla->_radioCalculoDesplazamiento, sizeof(malla->_radioCalculoDesplazamiento), 1, guardarEn);
		//Arreglos simples
		malla->_verticesGelatina.agregarBitsInternosEn(guardarEn);
		exito = true;
	} else {
		NBASSERT(false)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaMallaGelatina::interpretarBitsInternos(AUEscenaMallaGelatina* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaGelatina::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaMalla::interpretarBitsInternos(cargarEn, cargarDe)){
			cargarDe->leer(&cargarEn->_verticesAnimar, sizeof(cargarEn->_verticesAnimar), 1, cargarDe);
			cargarDe->leer(&cargarEn->_metodoCalculoDesplazamiento, sizeof(cargarEn->_metodoCalculoDesplazamiento), 1, cargarDe);
			cargarDe->leer(&cargarEn->_velocidadMaxima, sizeof(cargarEn->_velocidadMaxima), 1, cargarDe);
			cargarDe->leer(&cargarEn->_desplazamientoMaximo, sizeof(cargarEn->_desplazamientoMaximo), 1, cargarDe);
			cargarDe->leer(&cargarEn->_centroCalculoDesplazamiento, sizeof(cargarEn->_centroCalculoDesplazamiento), 1, cargarDe);
			cargarDe->leer(&cargarEn->_radioCalculoDesplazamiento, sizeof(cargarEn->_radioCalculoDesplazamiento), 1, cargarDe);
			//Arreglos simples
			cargarEn->_verticesGelatina.interpretarBitsInternos(cargarDe);
			//
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUEscenaSprite, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaMallaGelatina, AUEscenaMalla)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaMallaGelatina, "AUEscenaMallaGelatina", AUEscenaMalla)
AUOBJMETODOS_CLONAR_THIS(AUEscenaMallaGelatina)






