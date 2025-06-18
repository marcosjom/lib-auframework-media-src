//
//  AUFijacion.cpp
//  EditorCuerpos
//
//  Created by Nicaragua Binary on 30/10/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaFigura.h"

AUEscenaFigura::AUEscenaFigura(const ENEscenaFiguraTipo tipo) : AUEscenaObjeto(), _coordenadasLocales(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::AUEscenaFigura")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaFigura")
	inicializarVariables();
	_tipo						= tipo;
	_modo						= ENEscenaFiguraModo_Relleno;
	_maxVertices				= AU_FIJACION_VERTICES_MAXIMO;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaFigura::AUEscenaFigura(AUEscenaFigura* otro) : AUEscenaObjeto(otro), _coordenadasLocales(this, &otro->_coordenadasLocales) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::AUEscenaFigura")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaFigura")
	inicializarVariables();
	_tipo						= otro->_tipo;	
	_modo						= otro->_modo;
	_maxVertices				= otro->_maxVertices;
	this->establecerTexturaPatron(otro->_texturaPatron, (otro->_mascaraPropiedades & AUESCENAFIGURA_ES_PATRON_GLOBAL) != 0/*, otro->_texturaPatronMatriz*/);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaFigura::inicializarVariables(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::inicializarVariables")
	_texturaPatron				= NULL;
	_mascaraPropiedades			= 0;
	//NBMATRIZ_ESTABLECER_IDENTIDAD(_texturaPatronMatriz);
	_mascaraPropiedades			|= AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL;
	NBCAJAAABB_INICIALIZAR(_cacheCajaLocalCalculada);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaFigura::~AUEscenaFigura(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::~AUEscenaFigura")
	if(_texturaPatron != NULL) _texturaPatron->liberar(NB_RETENEDOR_THIS); _texturaPatron = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUTextura* AUEscenaFigura::texturaPatron(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::texturaPatron")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _texturaPatron;
}

bool AUEscenaFigura::coordenadasTexturasSonGlobal(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::coordenadasTexturasSonGlobal")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (_mascaraPropiedades & AUESCENAFIGURA_ES_PATRON_GLOBAL) != 0;
}

void AUEscenaFigura::establecerTexturaPatron(AUTextura* nuevaTextura, const bool basadaEnCoordsGlobal/*, const NBMatriz &matrizTransformaciones*/){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::establecerTexturaPatron")
	if(_texturaPatron != nuevaTextura || ((_mascaraPropiedades & AUESCENAFIGURA_ES_PATRON_GLOBAL) ? true : false) != basadaEnCoordsGlobal /*|| _texturaPatronMatriz != matrizTransformaciones*/){
		if(nuevaTextura != NULL) nuevaTextura->retener(NB_RETENEDOR_THIS);
		if(_texturaPatron != NULL) _texturaPatron->liberar(NB_RETENEDOR_THIS);
		_texturaPatron			= nuevaTextura;
		if(basadaEnCoordsGlobal){
			_mascaraPropiedades |= AUESCENAFIGURA_ES_PATRON_GLOBAL;
		} else {
			_mascaraPropiedades &= ~AUESCENAFIGURA_ES_PATRON_GLOBAL;
		}
		//_texturaPatronMatriz	= matrizTransformaciones;
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

ENEscenaFiguraTipo AUEscenaFigura::tipo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::tipo")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (ENEscenaFiguraTipo)_tipo;
}

ENEscenaFiguraModo AUEscenaFigura::modo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::modo")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (ENEscenaFiguraModo)_modo;
}

void AUEscenaFigura::establecerTipo(ENEscenaFiguraTipo tipo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::establecerTipo")
	if(_tipo != tipo){
		_tipo = tipo;
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	_mascaraPropiedades	|= AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaFigura::establecerModo(const ENEscenaFiguraModo modo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::establecerModo")
	if(_modo != modo){
		_modo = modo;
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaFigura::establecerMaximoVertices(const UI8 maxVertices){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::establecerMaximoVertices")
	//if(maxVertices>AU_FIJACION_VERTICES_MAXIMO) maxVertices = AU_FIJACION_VERTICES_MAXIMO;
	if(_maxVertices != maxVertices){
		_maxVertices = maxVertices;
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUArregloNativoP<NBPuntoColor>* AUEscenaFigura::puntosLocales(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::puntosLocales")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return &_coordenadasLocales;
}

bool AUEscenaFigura::agregarCoordenadaLocal(const float xLocal, const float yLocal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::agregarCoordenadaLocal")
	bool r = this->agregarCoordenadaLocalEnIndice(_coordenadasLocales.conteo, xLocal, yLocal, 255, 255, 255, 255);
	if(this->idEscena >= 0){
		this->setModelChangedFlag();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUEscenaFigura::agregarCoordenadaLocal(const NBPuntoColor &puntoColor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::agregarCoordenadaLocal")
	bool r = this->agregarCoordenadaLocalEnIndice(_coordenadasLocales.conteo, puntoColor.coordenada.x, puntoColor.coordenada.y, puntoColor.color.r, puntoColor.color.g, puntoColor.color.b, puntoColor.color.a);
	if(this->idEscena >= 0){
		this->setModelChangedFlag();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUEscenaFigura::agregarCoordenadaLocal(const float xLocal, const float yLocal, const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::agregarCoordenadaLocal")
	bool res = this->agregarCoordenadaLocalEnIndice(_coordenadasLocales.conteo, xLocal, yLocal, r, g, b, a);
	if(this->idEscena >= 0){
		this->setModelChangedFlag();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return res;
}

bool AUEscenaFigura::agregarCoordenadaLocal(const float xLocal, const float yLocal, const NBColor8 color){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::agregarCoordenadaLocal")
	bool res = this->agregarCoordenadaLocalEnIndice(_coordenadasLocales.conteo, xLocal, yLocal, color.r, color.g, color.b, color.a);
	if(this->idEscena >= 0){
		this->setModelChangedFlag();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return res;
}

bool AUEscenaFigura::agregarCoordenadaLocalEnIndice(const UI16 indice, const NBPuntoColor &puntoColor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::agregarCoordenadaLocalEnIndice")
	bool r = this->agregarCoordenadaLocalEnIndice(indice, puntoColor.coordenada.x, puntoColor.coordenada.y, puntoColor.color.r, puntoColor.color.g, puntoColor.color.b, puntoColor.color.a);
	if(this->idEscena >= 0){
		this->setModelChangedFlag();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUEscenaFigura::agregarCoordenadaLocalEnIndice(const UI16 indice, const float xLocal, const float yLocal, const NBColor8 color){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::agregarCoordenadaLocalEnIndice")
	bool r = this->agregarCoordenadaLocalEnIndice(indice, xLocal, yLocal, color.r, color.g, color.b, color.a);
	if(this->idEscena >= 0){
		this->setModelChangedFlag();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUEscenaFigura::agregarCoordenadaLocalEnIndice(const UI16 indice, const float xLocal, const float yLocal, const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::agregarCoordenadaLocalEnIndice")
	NBASSERT(indice<=_coordenadasLocales.conteo)
	bool exito = false;
	NBPuntoColor datosVertice;
	datosVertice.coordenada.x = xLocal;
	datosVertice.coordenada.y = yLocal;
	NBCOLOR_ESTABLECER(datosVertice.color, r, g, b, a)
	_coordenadasLocales.agregarElementoEnIndice(datosVertice, indice);
	_mascaraPropiedades |= AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL;
	//
	if(this->idEscena >= 0){
		this->setModelChangedFlag();
	}
	//
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaFigura::agregarCoordenadaEnPuntoMedioDeSegmento(const UI32 indiceVerticeIniSegmento){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::agregarCoordenadaEnPuntoMedioDeSegmento")
	bool exito = false;
	if(_tipo == ENEscenaFiguraTipo_PoligonoCerrado || _tipo == ENEscenaFiguraTipo_SecuenciaLineas){
		if(indiceVerticeIniSegmento < _coordenadasLocales.conteo){
			UI32 indiceUltimoVertice = (_coordenadasLocales.conteo - 1);
			NBPunto finSeg, iniSeg = _coordenadasLocales.elemento[indiceVerticeIniSegmento].coordenada;
			if(indiceVerticeIniSegmento == indiceUltimoVertice){
				finSeg = _coordenadasLocales.elemento[0].coordenada;
			} else {
				finSeg = _coordenadasLocales.elemento[indiceVerticeIniSegmento+1].coordenada;
			}
			if(_coordenadasLocales.conteo >= _maxVertices){
				PRINTF_INFO("No se permiten mas de %d vertices por poligono\n", _maxVertices);
			} else {
				NBPuntoColor datosVertice;
				datosVertice.coordenada.x = iniSeg.x + ((finSeg.x - iniSeg.x) / 2.0f);
				datosVertice.coordenada.y = iniSeg.y + ((finSeg.y - iniSeg.y) / 2.0f);
				NBCOLOR_ESTABLECER(datosVertice.color, 255, 255, 255, 255)
				_coordenadasLocales.agregarElementoEnIndice(datosVertice, indiceVerticeIniSegmento+1);
				_mascaraPropiedades |= AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL;
				if(this->idEscena >= 0){
					this->setModelChangedFlag();
				}
				exito = true;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

void AUEscenaFigura::quitarCoordenadaLocalEnIndice(const UI32 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::quitarCoordenadaLocalEnIndice")
	NBASSERT(indice < _coordenadasLocales.conteo)
	_coordenadasLocales.quitarElementoEnIndice(indice);
	if(this->idEscena >= 0){
		this->setModelChangedFlag();
	}
	_mascaraPropiedades |= AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaFigura::moverVerticeHacia(const UI32 indice, const float xLocal, const float yLocal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::moverVerticeHacia")
	BOOL chgd = FALSE;
	if(_tipo == ENEscenaFiguraTipo_Circulo && _coordenadasLocales.conteo>=2){
		NBASSERT(_tipo != ENEscenaFiguraTipo_Circulo || indice != 0) //No se debe mover el vertice cero del circulo
		if(_coordenadasLocales.conteo==2){
			if(indice == 1){	//el radio
				if(_coordenadasLocales.elemento[1].coordenada.x != xLocal){
					_coordenadasLocales.elemento[1].coordenada.x = xLocal; //solo x, solo radio
					_mascaraPropiedades |= AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL;
					chgd = TRUE;
				}
			}
		} else {
			if(indice == 1 || indice == 2){
				if(indice == 1){	//el radio/angulo1
					if(_coordenadasLocales.elemento[1].coordenada.x != xLocal || _coordenadasLocales.elemento[1].coordenada.y != yLocal){
						_coordenadasLocales.elemento[1].coordenada.x = xLocal;
						_coordenadasLocales.elemento[1].coordenada.y = yLocal;
						_mascaraPropiedades |= AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL;
						chgd = TRUE;
					}
				}
				if(indice == 2){	//el angulo2
					if(_coordenadasLocales.elemento[2].coordenada.x != xLocal || _coordenadasLocales.elemento[2].coordenada.y != yLocal){
						_coordenadasLocales.elemento[2].coordenada.x = xLocal;
						_coordenadasLocales.elemento[2].coordenada.y = yLocal;
						_mascaraPropiedades |= AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL;
						chgd = TRUE;
					}
				}
				if(this->privValidarVerticesCircunferencia()){
					chgd = TRUE;
				}
			}
		}
	} else if((_tipo == ENEscenaFiguraTipo_Linea && _coordenadasLocales.conteo>=2) || (_tipo == ENEscenaFiguraTipo_PoligonoCerrado && _coordenadasLocales.conteo>=3) || (_tipo == ENEscenaFiguraTipo_SecuenciaLineas && _coordenadasLocales.conteo>=3)){
		if(indice < _coordenadasLocales.conteo){
			if(_coordenadasLocales.elemento[indice].coordenada.x != xLocal || _coordenadasLocales.elemento[indice].coordenada.y != yLocal){
				_coordenadasLocales.elemento[indice].coordenada.x = xLocal;
				_coordenadasLocales.elemento[indice].coordenada.y = yLocal;
				_mascaraPropiedades |= AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL;
				chgd = TRUE;
			}
		}
	} else {
		PRINTF_INFO("Tipo de fijacion no identificada\n");
		NBASSERT(false)
	}
	//
	if(this->idEscena >= 0 && chgd){
		this->setModelChangedFlag();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaFigura::colorearVertice(const UI32 indice, const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::colorearVertice")
	NBASSERT(indice < _coordenadasLocales.conteo)
	if(indice < _coordenadasLocales.conteo){
		NBPuntoColor* v = &_coordenadasLocales.elemento[indice];
		if(v->color.r != r || v->color.g != g || v->color.b != b || v->color.a != a){
			v->color.r = r;
			v->color.g = g;
			v->color.b = b;
			v->color.a = a;
			if(this->idEscena >= 0){
				this->setModelChangedFlag();
			}
		}
	}
	//PRINTF_INFO("Vertice coloreado: (%d, %d, %d, %d)\n", (SI32)r, (SI32)g, (SI32)b, (SI32)a);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

SI32 AUEscenaFigura::indiceVerticeMasCercano(const float xLocal, const float yLocal, float distanciaAbsMaxima){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::indiceVerticeMasCercano")
	int indice = -1;
	if(distanciaAbsMaxima < 0.0f) distanciaAbsMaxima = -distanciaAbsMaxima;
	//PRINTF_INFO("Validando distancia vertice: %f\n", distanciaAbsMaxima);
	if(_tipo == ENEscenaFiguraTipo_Circulo && _coordenadasLocales.conteo>=2){
		NBPunto vertice = _coordenadasLocales.elemento[1].coordenada;
		float distancia = NBPUNTO_DISTANCIA(xLocal, yLocal, vertice.x, vertice.y);
		if(distancia >= -distanciaAbsMaxima && distancia <= distanciaAbsMaxima){
			indice = 1;
		} else if(_coordenadasLocales.conteo>2){
			NBPunto vertice = _coordenadasLocales.elemento[2].coordenada;
			float distancia = NBPUNTO_DISTANCIA(xLocal, yLocal, vertice.x, vertice.y);
			if(distancia >= -distanciaAbsMaxima && distancia <= distanciaAbsMaxima){
				indice = 2;
			}
		}
	} else {
		SI32 i, iConteo = _coordenadasLocales.conteo;
		for(i = 0; i<iConteo; i++){
			NBPunto vertice = _coordenadasLocales.elemento[i].coordenada;
			float distancia = NBPUNTO_DISTANCIA(xLocal, yLocal,vertice.x, vertice.y);
			if(distancia >= -distanciaAbsMaxima && distancia <= distanciaAbsMaxima){
				indice = i;
				break;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return indice;
}

BOOL AUEscenaFigura::privValidarVerticesCircunferencia(){
	BOOL r = FALSE;
	const NBPuntoColor* v0 = &_coordenadasLocales.elemento[0];
	const NBPuntoColor* v1 = &_coordenadasLocales.elemento[1];
	NBPuntoColor* v2	= &_coordenadasLocales.elemento[2];
	float radio			= NBPUNTO_DISTANCIA(v0->coordenada.x, v0->coordenada.y, v1->coordenada.x, v1->coordenada.y);
	float angulo1;		NBPUNTO_ANGULO_VECTOR(angulo1, v0->coordenada.x, v0->coordenada.y, v1->coordenada.x, v1->coordenada.y);
	float angulo2;		NBPUNTO_ANGULO_VECTOR(angulo2, v0->coordenada.x, v0->coordenada.y, v2->coordenada.x, v2->coordenada.y);
	NBPunto v2Normal;	NBPUNTO_ESTABLECER(v2Normal, radio, 0.0f);
	NBPunto v2Rotado;	NBPUNTO_ROTAR(v2Rotado, v2Normal, angulo2);
	STNBPoint p;
	p.x = v0->coordenada.x + v2Rotado.x;
	p.y = v0->coordenada.y + v2Rotado.y;
	if(v2->coordenada.x != p.x || v2->coordenada.y != p.y){
		v2->coordenada.x = p.x;
		v2->coordenada.y = p.y;
		r = TRUE;
	}
	return r;
}

void AUEscenaFigura::establecerCoordenadasComoCaja(const float centroLocalX, const float centroLocalY, const float ancho, const float alto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::establecerCoordenadasComoCaja")
	if(_tipo == ENEscenaFiguraTipo_PoligonoCerrado || _tipo == ENEscenaFiguraTipo_SecuenciaLineas){
		STNBPoint p;
		BOOL chgd = FALSE;
		float hW = ancho / 2.0f, hH = alto / 2.0f;
		if(_coordenadasLocales.conteo < 1){
			this->agregarCoordenadaLocal(centroLocalX - hW, centroLocalY - hH);
		} else {
			p.x = centroLocalX - hW; p.y = centroLocalY - hH;
			if(_coordenadasLocales.elemento[0].coordenada.x != p.x || _coordenadasLocales.elemento[0].coordenada.y != p.y){
				_coordenadasLocales.elemento[0].coordenada.x = p.x;
				_coordenadasLocales.elemento[0].coordenada.y = p.y;
				chgd = TRUE;
			}
		}
		if(_coordenadasLocales.conteo < 2){
			this->agregarCoordenadaLocal(centroLocalX + hW, centroLocalY - hH);
		} else {
			p.x = centroLocalX + hW; p.y = centroLocalY - hH;
			if(_coordenadasLocales.elemento[1].coordenada.x != p.x || _coordenadasLocales.elemento[1].coordenada.y != p.y){
				_coordenadasLocales.elemento[1].coordenada.x = p.x;
				_coordenadasLocales.elemento[1].coordenada.y = p.y;
				chgd = TRUE;
			}
		}
		if(_coordenadasLocales.conteo < 3){
			this->agregarCoordenadaLocal(centroLocalX + hW, centroLocalY + hH);
		} else {
			p.x = centroLocalX + hW; p.y = centroLocalY + hH;
			if(_coordenadasLocales.elemento[2].coordenada.x != p.x || _coordenadasLocales.elemento[2].coordenada.y != p.y){
				_coordenadasLocales.elemento[2].coordenada.x = p.x;
				_coordenadasLocales.elemento[2].coordenada.y = p.y;
				chgd = TRUE;
			}
		}
		if(_coordenadasLocales.conteo < 4){
			this->agregarCoordenadaLocal(centroLocalX - hW, centroLocalY + hH);
		} else {
			p.x = centroLocalX - hW; p.y = centroLocalY + hH;
			if(_coordenadasLocales.elemento[3].coordenada.x != p.x || _coordenadasLocales.elemento[3].coordenada.y != p.y){
				_coordenadasLocales.elemento[3].coordenada.x = p.x;
				_coordenadasLocales.elemento[3].coordenada.y = p.y;
				chgd = TRUE;
			}
		}
		//
		if(this->idEscena >= 0 && chgd){
			this->setModelChangedFlag();
		}
		//
		_mascaraPropiedades |= AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaFigura::escalarCoordenadasDesdeCentroLocal(const float centroLocalX, const float centroLocalY, const float escalaAncho, const float escalaAlto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::escalarCoordenadasDesdeCentroLocal")
	BOOL chgd = FALSE; STNBPoint p;
	SI32 i; const SI32 conteo = _coordenadasLocales.conteo;
	for(i = 0; i < conteo; i++){
		NBPunto* vertice = &(_coordenadasLocales.elemento[i].coordenada);
		const float distanciaXCentro = vertice->x - centroLocalX;
		const float distanciaYCentro = vertice->y - centroLocalY;
		p.x = centroLocalX + (distanciaXCentro*escalaAncho);
		p.y = centroLocalY + (distanciaYCentro*escalaAlto);
		if(vertice->x != p.x || vertice->y != p.y){
			vertice->x = p.x;
			vertice->y = p.y;
			chgd = TRUE;
		}
		//
		_mascaraPropiedades |= AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL;
	}
	//
	if(this->idEscena >= 0 && chgd){
		this->setModelChangedFlag();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

bool AUEscenaFigura::esPoligonoConvexoContraReloj(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::esPoligonoConvexoContraReloj")
	bool resultado = false;
	if((_tipo == ENEscenaFiguraTipo_PoligonoCerrado || _tipo == ENEscenaFiguraTipo_SecuenciaLineas) && _coordenadasLocales.conteo>=3){
		resultado = true;
		NBPunto segAntIni = _coordenadasLocales.elemento[_coordenadasLocales.conteo-2].coordenada;
		NBPunto segAntFin = _coordenadasLocales.elemento[_coordenadasLocales.conteo-1].coordenada;
		SI32 iVert, iVertConteo = _coordenadasLocales.conteo;
		for(iVert = 0; iVert<iVertConteo && resultado; iVert++){
			NBPunto vertice =  _coordenadasLocales.elemento[iVert].coordenada;
			resultado		= NBPUNTO_ESTA_IZQUIERDA_VECTOR(segAntIni.x, segAntIni.y, segAntFin.x, segAntFin.y, vertice.x, vertice.y);
			segAntIni		= segAntFin;
			segAntFin		= vertice;
		}
		/*NBPunto puntoIni, puntoFin;
		int i, ultimoIndice = _coordenadasLocales.conteo - 1;
		//averiguar donde inicia el vector con menor angulo
		int indiceVerticeConMenorAngulo = -1; float anguloMenor = 0.0f;
		for(i = 0; i<=ultimoIndice; i++){
			puntoIni = _coordenadasLocales.elemento[i];
			if(i==ultimoIndice){
				puntoFin = _coordenadasLocales.elemento[0];	
			} else {
				puntoFin = _coordenadasLocales.elemento[i+1];
			}
			float angulo; NBPUNTO_ANGULO_VECTOR(angulo, puntoIni.x, puntoIni.y, puntoFin.x, puntoFin.y);
			if(indiceVerticeConMenorAngulo==-1 || anguloMenor>angulo){
				anguloMenor = angulo;
				indiceVerticeConMenorAngulo = i;
			}
		}
		//PRINTF_INFO("Vertice con menor angulo %d\n", indiceVerticeConMenorAngulo);
		//Recorrer la figura y analizar si todos los angulos vana en incremento
		bool esConvexoCR = true; float anguloEnRevision = 0.0f;
		int indiceV = indiceVerticeConMenorAngulo;
		for(i = 0; i<_coordenadasLocales.conteo && esConvexoCR; i++){
			int indiceIni = indiceV; indiceV  = (indiceV==ultimoIndice?0:indiceV+1);
			int indiceFin = indiceV;
			puntoIni = _coordenadasLocales.elemento[indiceIni];
			puntoFin = _coordenadasLocales.elemento[indiceFin];
			float angulo; NBPUNTO_ANGULO_VECTOR(angulo, puntoIni.x, puntoIni.y, puntoFin.x, puntoFin.y);
			//PRINTF_INFO("Angulo verticeFinal[%d - %d]: %f\n", indiceIni, indiceFin, angulo);
			esConvexoCR = (angulo>=anguloEnRevision);
			anguloEnRevision = angulo;
		}
		resultado = esConvexoCR;*/
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return resultado;
}

NBCajaAABB AUEscenaFigura::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::cajaAABBLocalCalculada")
	if((_mascaraPropiedades & AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL) != 0){
		NBCAJAAABB_INICIALIZAR(_cacheCajaLocalCalculada);
		if(_tipo == ENEscenaFiguraTipo_Circulo && _coordenadasLocales.conteo>=2){
			NBPunto centroLocal		= _coordenadasLocales.elemento[0].coordenada;
			NBPunto circunLocal		= _coordenadasLocales.elemento[1].coordenada;
			float radioLocal		= NBPUNTO_DISTANCIA(centroLocal.x, centroLocal.y, circunLocal.x, circunLocal.y);
			//Envolver las dos o tres coordenadas centro + radio [+ anguloMax]
			if(_coordenadasLocales.conteo==2){
				NBCAJAAABB_ENVOLVER_PUNTO(_cacheCajaLocalCalculada, centroLocal.x - radioLocal, centroLocal.y - radioLocal);
				NBCAJAAABB_ENVOLVER_PUNTO(_cacheCajaLocalCalculada, centroLocal.x + radioLocal, centroLocal.y + radioLocal);
			} else {
				NBPunto radioMax	= _coordenadasLocales.elemento[2].coordenada;
				NBCAJAAABB_ENVOLVER_PUNTO(_cacheCajaLocalCalculada, centroLocal.x, centroLocal.y);
				NBCAJAAABB_ENVOLVER_PUNTO(_cacheCajaLocalCalculada, circunLocal.x, circunLocal.y);
				NBCAJAAABB_ENVOLVER_PUNTO(_cacheCajaLocalCalculada, radioMax.x, radioMax.y);
				//Envolver cuartos de circunferencias completos
				float radMin;		NBPUNTO_RADIANES_VECTOR(radMin, centroLocal.x, centroLocal.y, circunLocal.x, circunLocal.y)
				float radMax;		NBPUNTO_RADIANES_VECTOR(radMax, centroLocal.x, centroLocal.y, radioMax.x, radioMax.y)
				if(radMin>radMax)	radMin -= PIx2;
				float radAct		= radMin;
				float radAvance		= (radMax-radMin) / 8.0f;
				NBPunto posAct;
				radAct += radAvance;
				NBPUNTO_OBTENER_VECTOR_ROTADO(posAct, radioLocal, radAct); NBCAJAAABB_ENVOLVER_PUNTO(_cacheCajaLocalCalculada, posAct.x, posAct.y); radAct += radAvance;
				NBPUNTO_OBTENER_VECTOR_ROTADO(posAct, radioLocal, radAct); NBCAJAAABB_ENVOLVER_PUNTO(_cacheCajaLocalCalculada, posAct.x, posAct.y); radAct += radAvance;
				NBPUNTO_OBTENER_VECTOR_ROTADO(posAct, radioLocal, radAct); NBCAJAAABB_ENVOLVER_PUNTO(_cacheCajaLocalCalculada, posAct.x, posAct.y); radAct += radAvance;
				NBPUNTO_OBTENER_VECTOR_ROTADO(posAct, radioLocal, radAct); NBCAJAAABB_ENVOLVER_PUNTO(_cacheCajaLocalCalculada, posAct.x, posAct.y); radAct += radAvance;
				NBPUNTO_OBTENER_VECTOR_ROTADO(posAct, radioLocal, radAct); NBCAJAAABB_ENVOLVER_PUNTO(_cacheCajaLocalCalculada, posAct.x, posAct.y); radAct += radAvance;
				NBPUNTO_OBTENER_VECTOR_ROTADO(posAct, radioLocal, radAct); NBCAJAAABB_ENVOLVER_PUNTO(_cacheCajaLocalCalculada, posAct.x, posAct.y); radAct += radAvance;
				NBPUNTO_OBTENER_VECTOR_ROTADO(posAct, radioLocal, radAct); NBCAJAAABB_ENVOLVER_PUNTO(_cacheCajaLocalCalculada, posAct.x, posAct.y);
			}
		} else {
			SI32 iVert, iVertConteo = _coordenadasLocales.conteo;
			for(iVert = 0; iVert<iVertConteo; iVert++){
				NBPunto pos = _coordenadasLocales.elemento[iVert].coordenada;
				NBCAJAAABB_ENVOLVER_PUNTO(_cacheCajaLocalCalculada, pos.x, pos.y);
			}
		}
		//Brindar un poco de volumen
		if(_cacheCajaLocalCalculada.xMin==_cacheCajaLocalCalculada.xMax){ _cacheCajaLocalCalculada.xMin -= 8; _cacheCajaLocalCalculada.xMax += 8; }
		if(_cacheCajaLocalCalculada.yMin==_cacheCajaLocalCalculada.yMax){ _cacheCajaLocalCalculada.yMin -= 8; _cacheCajaLocalCalculada.yMax += 8; }
		_mascaraPropiedades &= ~AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _cacheCajaLocalCalculada;
}

void AUEscenaFigura::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaFigura::agregadoEnEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaFigura::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaFigura::quitandoDeEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

PTRfunCmdsGL AUEscenaFigura::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::actualizarModeloGL")
	NBASSERT(idEscena != -1) //Solo si esta en escena
	//Averiguar la cantidad total de vertices necesarios
	UI16 conteoVerticesNecesitar = 0;
	if(_tipo == ENEscenaFiguraTipo_Linea){
		NBASSERT(_coordenadasLocales.conteo>=2)
		conteoVerticesNecesitar	= 2;		//linea
	} else if(_tipo == ENEscenaFiguraTipo_Circulo){
		NBASSERT(_coordenadasLocales.conteo>=2)
		conteoVerticesNecesitar	= 38;		//circulo
	} else if(_tipo == ENEscenaFiguraTipo_PoligonoCerrado){
		NBASSERT(_coordenadasLocales.conteo>=3)
		conteoVerticesNecesitar	= _coordenadasLocales.conteo; //poligono
	} else if(_tipo == ENEscenaFiguraTipo_SecuenciaLineas){
		NBASSERT(_coordenadasLocales.conteo>=3)
		conteoVerticesNecesitar	= _coordenadasLocales.conteo; //poligono
	} else {
		NBASSERT(false)
	}
	if(conteoVerticesNecesitar != 0){
		NBColor colorMultiplicado;  NBCOLOR_MULTIPLICAR_CON_UI8(colorMultiplicado, propsHeredadas.colorPadre, _propiedades.color8)
		//Reservar los vertices necesarios
		STBloqueGL bloqueVerticesGL;
		bloqueVerticesGL.primerElemento		= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, conteoVerticesNecesitar);
		bloqueVerticesGL.cantidadElementos	= conteoVerticesNecesitar;
		STBloqueGL bloqueIndicesGL;			bloqueIndicesGL.primerElemento = bloqueIndicesGL.cantidadElementos = 0;
		NBVerticeGL* verticesGL				= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]);
		//ACTUALIZAR VERTICES Y TEXTURA-COORDS
		if(_tipo == ENEscenaFiguraTipo_Linea){
			NBASSERT(_coordenadasLocales.conteo>1); //PRINTF_INFO("La linea no cuenta con dos vertices\n");
			NBPuntoColor vertices[2];
			vertices[0] = _coordenadasLocales.elemento[0];
			vertices[1] = _coordenadasLocales.elemento[1];
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], _cacheObjEscena.matrizEscena, vertices[0].coordenada.x, vertices[0].coordenada.y);
			NBCOLOR_ESTABLECER(verticesGL[0], vertices[0].color.r*colorMultiplicado.r, vertices[0].color.g*colorMultiplicado.g, vertices[0].color.b*colorMultiplicado.b, vertices[0].color.a*colorMultiplicado.a);
			NBPUNTO_ESTABLECER(verticesGL[0].tex, 0.0f, 0.0f);
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[1], _cacheObjEscena.matrizEscena, vertices[1].coordenada.x, vertices[1].coordenada.y);
			NBCOLOR_ESTABLECER(verticesGL[1], vertices[1].color.r*colorMultiplicado.r, vertices[1].color.g*colorMultiplicado.g, vertices[1].color.b*colorMultiplicado.b, vertices[1].color.a*colorMultiplicado.a);
			NBPUNTO_ESTABLECER(verticesGL[1].tex, 0.0f, 0.0f);
		} else if(_tipo == ENEscenaFiguraTipo_Circulo){
			NBASSERT(_coordenadasLocales.conteo>1);  //PRINTF_WARNING("figura, el circulo no cuenta con dos vertices\n");
			NBColor8 colorVertice1; NBCOLOR_ESTABLECER(colorVertice1, _coordenadasLocales.elemento[1].color.r*colorMultiplicado.r, _coordenadasLocales.elemento[1].color.g*colorMultiplicado.g, _coordenadasLocales.elemento[1].color.b*colorMultiplicado.b, _coordenadasLocales.elemento[1].color.a*colorMultiplicado.a);
			NBPunto centro		= _coordenadasLocales.elemento[0].coordenada;
			NBPunto circun		= _coordenadasLocales.elemento[1].coordenada;
			float radianesIni, radianesFin;
			if(_coordenadasLocales.conteo>2){ //circulo parcial
				NBPunto circun2		= _coordenadasLocales.elemento[2].coordenada;
				NBPUNTO_RADIANES_VECTOR(radianesIni, centro.x, centro.y, circun.x, circun.y);
				NBPUNTO_RADIANES_VECTOR(radianesFin, centro.x, centro.y, circun2.x, circun2.y);
				if(radianesIni>radianesFin) radianesIni -= PIx2;
			} else {
				NBPUNTO_RADIANES_VECTOR(radianesIni, centro.x, centro.y, circun.x, circun.y);
				radianesFin = radianesIni + PIx2;
			}
			NBASSERT(radianesIni<=radianesFin)
			NBPunto radioUnitario;
			radioUnitario.y				= 0.0f;
			radioUnitario.x				= NBPUNTO_DISTANCIA(centro.x, centro.y, circun.x, circun.y);
			int i; float avanceAngulo	= ((radianesFin-radianesIni)/36.0f);
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], _cacheObjEscena.matrizEscena, centro.x, centro.y);
			NBCOLOR_ESTABLECER(verticesGL[0], colorVertice1.r, colorVertice1.g, colorVertice1.b, colorVertice1.a);
			NBPUNTO_ESTABLECER(verticesGL[0].tex, 0.0f, 0.0f);
			for(i=1; i<38; i++){
				float radianes			= radianesIni + ((i-1) * avanceAngulo);
				NBPunto rotado;			NBPUNTO_ROTAR_RADIANES(rotado, radioUnitario, radianes);
				rotado.x				+= centro.x;
				rotado.y				+= centro.y;
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[i], _cacheObjEscena.matrizEscena, rotado.x, rotado.y);
				NBCOLOR_ESTABLECER(verticesGL[i], colorVertice1.r, colorVertice1.g, colorVertice1.b, colorVertice1.a);
				NBPUNTO_ESTABLECER(verticesGL[i].tex, 0.0f, 0.0f);
			}
			if(_modo == ENEscenaFiguraModo_Relleno){
				bloqueIndicesGL = NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_N_DESDE_TRIANGFAN(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL, bloqueVerticesGL.cantidadElementos);
			}
		} else if(_tipo == ENEscenaFiguraTipo_PoligonoCerrado || _tipo == ENEscenaFiguraTipo_SecuenciaLineas){
			NBASSERT(_coordenadasLocales.conteo>2); //PRINTF_ERROR("el poligono no cuenta con tres o mas vertices\n");
			UI16 i, conteo = _coordenadasLocales.conteo;
			for(i = 0; i < conteo; i++){
				NBPuntoColor vertice = _coordenadasLocales.elemento[i];
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[i], _cacheObjEscena.matrizEscena, vertice.coordenada.x, vertice.coordenada.y);
				NBCOLOR_ESTABLECER(verticesGL[i], vertice.color.r*colorMultiplicado.r, vertice.color.g*colorMultiplicado.g, vertice.color.b*colorMultiplicado.b, vertice.color.a*colorMultiplicado.a)
				NBPUNTO_ESTABLECER(verticesGL[i].tex, 0.0f, 0.0f);
			}
			if(_tipo == ENEscenaFiguraTipo_PoligonoCerrado && _modo == ENEscenaFiguraModo_Relleno){
				bloqueIndicesGL = NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_N_DESDE_TRIANGFAN(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL, bloqueVerticesGL.cantidadElementos);
			}
		} else {
			NBASSERT(false)
		}
		//Actualizar coordenadas de textura
		if(_texturaPatron != NULL && conteoVerticesNecesitar != 0){
			if(_mascaraPropiedades & AUESCENAFIGURA_ES_PATRON_GLOBAL){
				_texturaPatron->cargarCoordenadasPatron(verticesGL, _coordenadasLocales.conteo, verticesGL/*, _texturaPatronMatriz*/);
			} else {
				_texturaPatron->cargarCoordenadasPatron(_coordenadasLocales.elemento, _coordenadasLocales.conteo, verticesGL/*, _texturaPatronMatriz*/);
			}
		}
		const SI32 posicionDatos	= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STFiguraRender));
		STFiguraRender* propsRender	= (STFiguraRender*)&(propsRenderizado.bytesDatosModelos->elemento[posicionDatos]);
		propsRender->tipo			= (ENEscenaFiguraTipo)_tipo;
		propsRender->modo			= (ENEscenaFiguraModo)_modo;
		propsRender->conteoVertices	= _coordenadasLocales.conteo;
		if(_texturaPatron != NULL){
			propsRender->idTexturaGL	= _texturaPatron->idTexturaGL; propsRenderizado.piscinaRetecionRender->agregarElemento(_texturaPatron);
		} else {
			//required on some systems where texture-0 will not be drawn
			propsRender->idTexturaGL	= NBGestorTexturas::texturaBlanca->idTexturaGL;
		}
		propsRender->bloqueVerticesGL	= bloqueVerticesGL;
		propsRender->bloqueIndicesGL	= bloqueIndicesGL;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (conteoVerticesNecesitar != 0 ? &AUEscenaFigura::enviarComandosGL : NULL);
}

void AUEscenaFigura::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::enviarComandosGL")
	STFiguraRender* propsRender = (STFiguraRender*)punteroDatosModelo;
	NBASSERT(propsRender->bloqueVerticesGL.primerElemento != 0);
	NBASSERT(propsRender->conteoVertices>0)
	NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaFigura")
	NBGestorGL::bindTexture(0, propsRender->idTexturaGL);
	if(propsRender->tipo == ENEscenaFiguraTipo_Linea && propsRender->conteoVertices>=2){
		NBGestorGL::drawArrays(GL_LINES, propsRender->bloqueVerticesGL.primerElemento, 2); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFigura(0)")
	} else if(propsRender->tipo == ENEscenaFiguraTipo_Circulo && propsRender->conteoVertices>=2){
		if(propsRender->modo == ENEscenaFiguraModo_Relleno){
			NBASSERT(propsRender->bloqueIndicesGL.cantidadElementos != 0)
			NB_GESTOR_GL_RENDER_TRIANGFAN(propsRender->bloqueIndicesGL.primerElemento, propsRender->bloqueIndicesGL.cantidadElementos);
		} else {
			NBGestorGL::drawArrays(GL_LINE_LOOP, propsRender->bloqueVerticesGL.primerElemento, 38); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFigura(2)")
		}
	} else if(propsRender->tipo == ENEscenaFiguraTipo_PoligonoCerrado && propsRender->conteoVertices>=3){
		if(propsRender->modo == ENEscenaFiguraModo_Relleno){
			NBASSERT(propsRender->bloqueIndicesGL.cantidadElementos != 0)
			NB_GESTOR_GL_RENDER_TRIANGFAN(propsRender->bloqueIndicesGL.primerElemento, propsRender->bloqueIndicesGL.cantidadElementos);
		} else {
			NBGestorGL::drawArrays(GL_LINE_LOOP, propsRender->bloqueVerticesGL.primerElemento, propsRender->conteoVertices); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFigura(4)")
		}
	} else if(propsRender->tipo == ENEscenaFiguraTipo_SecuenciaLineas && propsRender->conteoVertices>=3){
		NBGestorGL::drawArrays(GL_LINE_STRIP, propsRender->bloqueVerticesGL.primerElemento, propsRender->conteoVertices); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFigura(5)")
	} else {
		NBASSERT(false)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

bool AUEscenaFigura::agregarXmlInternoEn(AUEscenaFigura* figura, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(figura, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<tp>%d</tp>\r\n", espaciosBaseIzq, (int)figura->_tipo);
	guardarEn->agregarConFormato("%s<texGlob>%d</texGlob>\r\n", espaciosBaseIzq, (figura->_mascaraPropiedades & AUESCENAFIGURA_ES_PATRON_GLOBAL)?1:0);
	/*guardarEn->agregarConFormato("%s<texMatriz>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s <e00>%f</e00>\r\n", espaciosBaseIzq, NBMATRIZ_ELEM00(figura->_texturaPatronMatriz));
	guardarEn->agregarConFormato("%s <e01>%f</e01>\r\n", espaciosBaseIzq, NBMATRIZ_ELEM01(figura->_texturaPatronMatriz));
	guardarEn->agregarConFormato("%s <e02>%f</e02>\r\n", espaciosBaseIzq, NBMATRIZ_ELEM02(figura->_texturaPatronMatriz));
	guardarEn->agregarConFormato("%s <e10>%f</e10>\r\n", espaciosBaseIzq, NBMATRIZ_ELEM10(figura->_texturaPatronMatriz));
	guardarEn->agregarConFormato("%s <e11>%f</e11>\r\n", espaciosBaseIzq, NBMATRIZ_ELEM11(figura->_texturaPatronMatriz));
	guardarEn->agregarConFormato("%s <e12>%f</e12>\r\n", espaciosBaseIzq, NBMATRIZ_ELEM12(figura->_texturaPatronMatriz));
	guardarEn->agregarConFormato("%s <e20>%f</e20>\r\n", espaciosBaseIzq, NBMATRIZ_ELEM20(figura->_texturaPatronMatriz));
	guardarEn->agregarConFormato("%s <e21>%f</e21>\r\n", espaciosBaseIzq, NBMATRIZ_ELEM21(figura->_texturaPatronMatriz));
	guardarEn->agregarConFormato("%s <e22>%f</e22>\r\n", espaciosBaseIzq, NBMATRIZ_ELEM22(figura->_texturaPatronMatriz));
	guardarEn->agregarConFormato("%s</texMatriz>\r\n", espaciosBaseIzq);*/
	if(figura->_texturaPatron != NULL){
		const char* nombreTextura = NBGestorTexturas::nombreRecursoTextura(figura->_texturaPatron);
		if(nombreTextura != NULL){
			guardarEn->agregarConFormato("%s<tex>%s</tex>\r\n", espaciosBaseIzq, nombreTextura);
		}
	}
	guardarEn->agregarConFormato("%s<vs>\r\n", espaciosBaseIzq);
	UI32 i, iConteo = figura->_coordenadasLocales.conteo;
	for(i = 0; i<iConteo; i++){
		NBPuntoColor vertice = figura->_coordenadasLocales.elemento[i];
		guardarEn->agregarConFormato("%s\t<v>%f|%f|%d|%d|%d|%d</v>\r\n", espaciosBaseIzq, vertice.coordenada.x, vertice.coordenada.y, (SI32)vertice.color.r, (SI32)vertice.color.g, (SI32)vertice.color.b, (SI32)vertice.color.a);
	}
	guardarEn->agregarConFormato("%s</vs>\r\n", espaciosBaseIzq);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUEscenaFigura::interpretarXmlInterno(AUEscenaFigura* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
		const sNodoXML* nodoEscenaObj; XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
		if(nodoEscenaObj != NULL){
			exito = AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj);
			int tipo;					XML_VALOR_NODO_HIJO(datosXml, tipo, "tp", "tipo", 0, nodoXml, NULL)
			bool texGlobal				= datosXml->nodoHijo("texGlob", false, nodoXml);
			/*NBMatriz matrizTex; 
			NBMATRIZ_ESTABLECER_IDENTIDAD(matrizTex);
			//PENDIENTE: eliminar las cargas de nodos "texAng", "texPos" y "texEsc" (confirmar que ya no existen en los archivos XML)
			float texAng = datosXml->nodoHijo("texAng", 0.0f, nodoXml);
			NBMATRIZ_ROTAR_GRADOS(matrizTex, texAng);
			const sNodoXML* nodoEscTex	= datosXml->nodoHijo("texEsc", nodoXml);
			if(nodoEscTex != NULL){
				float x = datosXml->nodoHijo("x", 1.0f, nodoEscTex);
				float y = datosXml->nodoHijo("y", 1.0f, nodoEscTex);
				NBMATRIZ_ESCALAR(matrizTex, x, y);
			}
			const sNodoXML* nodoEscPos	= datosXml->nodoHijo("texPos", nodoXml);
			if(nodoEscPos != NULL){
				float x = datosXml->nodoHijo("x", 0.0f, nodoEscPos);
				float y = datosXml->nodoHijo("y", 0.0f, nodoEscPos);
				NBMATRIZ_TRASLADAR(matrizTex, x, y);
			}
			const sNodoXML* nodoTexMat	= datosXml->nodoHijo("texMatriz", nodoXml);
			if(nodoTexMat != NULL){
				NBMATRIZ_ELEM00(matrizTex) = datosXml->nodoHijo("e00", 1.0f, nodoTexMat);
				NBMATRIZ_ELEM01(matrizTex) = datosXml->nodoHijo("e01", 0.0f, nodoTexMat);
				NBMATRIZ_ELEM02(matrizTex) = datosXml->nodoHijo("e02", 0.0f, nodoTexMat);
				NBMATRIZ_ELEM10(matrizTex) = datosXml->nodoHijo("e10", 0.0f, nodoTexMat);
				NBMATRIZ_ELEM11(matrizTex) = datosXml->nodoHijo("e11", 1.0f, nodoTexMat);
				NBMATRIZ_ELEM12(matrizTex) = datosXml->nodoHijo("e12", 0.0f, nodoTexMat);
				//NBMATRIZ_ELEM20(matrizTex) = datosXml->nodoHijo("e20", 0.0f, nodoTexMat);
				//NBMATRIZ_ELEM21(matrizTex) = datosXml->nodoHijo("e21", 0.0f, nodoTexMat);
				//NBMATRIZ_ELEM22(matrizTex) = datosXml->nodoHijo("e22", 1.0f, nodoTexMat);
			}*/
			AUTextura* textura			= NULL;
			AUCadenaMutable8* nomTextura = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
			datosXml->nodoHijo("tex", nomTextura, "", nodoXml);
			if(nomTextura->tamano() != 0){
				textura = NBGestorTexturas::texturaPatronDesdeArchivoPNG(nomTextura->str());
			}
			nomTextura->liberar(NB_RETENEDOR_NULL);
			//
			cargarEn->_tipo				= (ENEscenaFiguraTipo)tipo;
			cargarEn->establecerTexturaPatron(textura, texGlobal/*, matrizTex*/);
			//vertices
			const sNodoXML* nodoVerts; XML_NODO_HIJO(datosXml, nodoVerts, "vs", "vertices", nodoXml, NULL)
			if(nodoVerts != NULL){
				const sNodoXML* nodoV	= datosXml->nodoHijo("v", nodoVerts);
				while(nodoV != NULL){
					NBPunto pos; NBColor8 color;
					if(nodoV->nodosHijos == NULL){
						//Formato nuevo CSV (mas compacto)
						const char* cadenaCSV		= datosXml->cadenaValorDeNodo(nodoV);
						SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
						SI32 posEnCadenaCSV		= 0;
						pos.x		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						pos.y		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						color.r		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
						color.g		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
						color.b		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
						color.a		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
					} else {
						//Formato viejo XML (muy inflado)
						pos.x		= datosXml->nodoHijo("x", 0.0f, nodoV);
						pos.y		= datosXml->nodoHijo("y", 0.0f, nodoV);
						color.r		= datosXml->nodoHijo("r", 255, nodoV);
						color.g		= datosXml->nodoHijo("g", 255, nodoV);
						color.b		= datosXml->nodoHijo("b", 255, nodoV);
						color.a		= datosXml->nodoHijo("a", 255, nodoV);
					}
					cargarEn->agregarCoordenadaLocal(pos.x, pos.y, color.r, color.g, color.b, color.a);
					nodoV	= datosXml->nodoHijo("v", nodoVerts, nodoV);
				}
			}
		}
		cargarEn->_mascaraPropiedades			|= AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL;
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaFigura::agregarBitsInternosEn(AUEscenaFigura* figura, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::agregarBitsInternosEn")
	bool exito = false;
	//
	SI32 valorVerificacion = AUESCENAFIGURA_VALOR_VERIFICACION_BINARIO;
	guardarEn->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(figura, guardarEn)){
		SI32 tipo = figura->_tipo;	guardarEn->escribir(&tipo, sizeof(tipo), 1, guardarEn);
		SI32 modo = figura->_modo;	guardarEn->escribir(&modo, sizeof(modo), 1, guardarEn);
		SI32 maxVert = figura->_maxVertices;	guardarEn->escribir(&maxVert, sizeof(maxVert), 1, guardarEn);
		figura->_coordenadasLocales.agregarBitsInternosEn(guardarEn);
		//
		bool esPatronGlobal = (figura->_mascaraPropiedades & AUESCENAFIGURA_ES_PATRON_GLOBAL);
		guardarEn->escribir(&esPatronGlobal, sizeof(esPatronGlobal), 1, guardarEn);
		//guardarEn->escribir(&figura->_texturaPatronMatriz, sizeof(figura->_texturaPatronMatriz), 1, guardarEn);
		SI32 tamUrlTex = 0; const char* nombreTextura = NULL;
		if(figura->_texturaPatron != NULL){
			nombreTextura = NBGestorTexturas::nombreRecursoTextura(figura->_texturaPatron);
			if(nombreTextura != NULL) tamUrlTex = AUCadena8::tamano(nombreTextura);
		}
		guardarEn->escribir(&tamUrlTex, sizeof(tamUrlTex), 1, guardarEn);
		if(tamUrlTex != 0) guardarEn->escribir(nombreTextura, sizeof(UI8), tamUrlTex, guardarEn);
		//
		guardarEn->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn);
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaFigura::interpretarBitsInternos(AUEscenaFigura* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaFigura::interpretarBitsInternos")
	bool exito = false;
	SI32 valorVerificacion;
	cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe);
	if(valorVerificacion!=AUESCENAFIGURA_VALOR_VERIFICACION_BINARIO){
		NBASSERT(false);
	} else {
		if(!AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			NBASSERT(false);
		} else {
			SI32 tipo; cargarDe->leer(&tipo, sizeof(tipo), 1, cargarDe); cargarEn->_tipo = (ENEscenaFiguraTipo)tipo;
			SI32 modo; cargarDe->leer(&modo, sizeof(modo), 1, cargarDe); cargarEn->_modo = (ENEscenaFiguraModo)modo;
			SI32 maxVert; cargarDe->leer(&maxVert, sizeof(maxVert), 1, cargarDe); cargarEn->_maxVertices = maxVert; NBASSERT(maxVert>0 && maxVert<=255)
			cargarEn->_coordenadasLocales.interpretarBitsInternos(cargarDe);
			//
			bool esPatronGlobal; cargarDe->leer(&esPatronGlobal, sizeof(esPatronGlobal), 1, cargarDe);
			if(esPatronGlobal)	cargarEn->_mascaraPropiedades |= AUESCENAFIGURA_ES_PATRON_GLOBAL;
			else cargarEn->_mascaraPropiedades &= ~AUESCENAFIGURA_ES_PATRON_GLOBAL;
			//cargarDe->leer(&cargarEn->_texturaPatronMatriz, sizeof(cargarEn->_texturaPatronMatriz), 1, cargarDe);
			//
			SI32 tamUrlTex; cargarDe->leer(&tamUrlTex, sizeof(tamUrlTex), 1, cargarDe); NBASSERT(tamUrlTex >= 0 && tamUrlTex<1024)
			if(tamUrlTex>0){
				UI8* cacheUrlTex = (UI8*)NBGestorMemoria::reservarMemoria(tamUrlTex+1, ENMemoriaTipo_Temporal);  NB_DEFINE_NOMBRE_PUNTERO(cacheUrlTex, "AUEscenaFigura::cacheUrlTex")
				cargarDe->leer(cacheUrlTex, sizeof(UI8), tamUrlTex, cargarDe);
				cacheUrlTex[tamUrlTex] = '\0';
				AUTextura* tex = NBGestorTexturas::texturaPatronDesdeArchivoPNG((const char*)cacheUrlTex);
				if(tex != NULL) cargarEn->establecerTexturaPatron(tex, esPatronGlobal/*, cargarEn->_texturaPatronMatriz*/);
				NBGestorMemoria::liberarMemoria(cacheUrlTex);
			}
			cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe);
			if(valorVerificacion!=AUESCENAFIGURA_VALOR_VERIFICACION_BINARIO){
				NBASSERT(false);
			} else {
				exito = true;
			}
		}
		cargarEn->_mascaraPropiedades			|= AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaFigura, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaFigura, "AUEscenaFigura", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaFigura)



