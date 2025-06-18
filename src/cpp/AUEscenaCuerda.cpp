//
//  AUEscenaCuerda.cpp
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 10/04/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaCuerda.h"

#ifndef CONFIG_NB_UNSUPPORT_BOX2D
#include "b2Rope.h"
#include "b2RopeJoint.h"
#endif

AUEscenaCuerda::AUEscenaCuerda(ENCuerdaTipo tipoCuerda, AUTextura* texturaSegmentos, float longitudCuerdaEscena, float masaCadaSegmento, float adicionalSegmentos) : AUEscenaObjeto(), _datosVertices(this), _datosSegmentos(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::AUEscenaCuerda")
	//
	NBASSERT(texturaSegmentos != NULL)
	NBASSERT(longitudCuerdaEscena>0.0f)
	NBASSERT(masaCadaSegmento>=0.0f)
	//
	_registradoComoCortable		= false;
	_tipoCuerda					= tipoCuerda;
	_mascaraPropsCuerda			= 0;
	NBPUNTO_ESTABLECER(_anclaIniPos, 0.0f, 0.0f);
	NBPUNTO_ESTABLECER(_anclaFinPos, 0.0f, 0.0f);
	//
	_masaCadaSegmento			= masaCadaSegmento;
	_longitudHabilitadaCuerda	= longitudCuerdaEscena;
	_desplazamientoCuerdaInicio	= 0.0f;
	_texSegmentos				= texturaSegmentos; _texSegmentos->retener(NB_RETENEDOR_THIS);
	_adicionalEntreSegmentos	= _tipoCuerda!=ENCuerdaTipo_Cadena?0.0f:adicionalSegmentos;
	_segsNecesariosParaCorte	= 0.0f;
	//
	NBTamano tamSegmentoCuerda	= _texSegmentos->tamanoHD();
	float longitudSegmento		= tamSegmentoCuerda.ancho - _adicionalEntreSegmentos;
	if(longitudSegmento<8.0f){
		longitudSegmento		= 8.0f;
		_adicionalEntreSegmentos = tamSegmentoCuerda.ancho - longitudSegmento; if(_adicionalEntreSegmentos<0.0f) _adicionalEntreSegmentos = 0.0f;
	}
	//
	float totalSegmentos		= longitudCuerdaEscena/longitudSegmento; //PRINTF_INFO("Conteo segmentos: %f (longitud cuerda %f)\n", totalSegmentos, longitudCuerdaEscena);
	float totalSegmentosFull	= (float)((SI32)totalSegmentos);
	float parcialPrimerSegmento	= totalSegmentos-totalSegmentosFull;
	SI32 conteoSegmentos		= totalSegmentosFull+(parcialPrimerSegmento!=0.0f?1:0); NBASSERT(conteoSegmentos>0 && conteoSegmentos<254)
	_conteoSegmentos			= conteoSegmentos;
	//PRINTF_INFO("CUERDA: segmentos(%d)=full(%f)+parcial(%f)\n", _conteoSegmentos, totalSegmentosFull, parcialPrimerSegmento);
	//
	_damping								= 0.1f;
	_k2										= 0.9f;
	_k3										= 0.1f;
	_indicePrimerSegmentoHabilitado			= 0;
	_indiceUltimoSegmentoHabilitado			= 0;
	privInicializarCuerda(_conteoSegmentos+1, longitudSegmento/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, _masaCadaSegmento);
	//
	if(parcialPrimerSegmento!=0.0f){
		_datosSegmentos.elemento[_indicePrimerSegmentoHabilitado].Ls = parcialPrimerSegmento*longitudSegmento/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	}
	//float longitudTotalCuerda = distanciaTotalSegmentosHabilitados();
	//PRINTF_INFO("CUERDA LONGITUD TOTAL SEGMENTOS: %f\n", longitudTotalCuerda);
	//
	_escuchadorCuerda		= NULL;
	_unionCuerdaOpcional	= NULL;
	//
	_cuerdaCajaLocalSucia	= true;
	NBCAJAAABB_INICIALIZAR(_cuerdaCajaLocal);
	//
	this->reanudarAnimacion();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaCuerda::AUEscenaCuerda(AUEscenaCuerda* otraInstancia) : AUEscenaObjeto(otraInstancia), _datosVertices(this, &otraInstancia->_datosVertices), _datosSegmentos(this, &otraInstancia->_datosSegmentos) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::AUEscenaCuerda")
	_registradoComoCortable					= false;
	_tipoCuerda								= otraInstancia->_tipoCuerda;
	_mascaraPropsCuerda						= otraInstancia->_mascaraPropsCuerda;
	_anclaIniPos							= otraInstancia->_anclaIniPos;
	_anclaFinPos							= otraInstancia->_anclaFinPos;
	//
	_masaCadaSegmento						= otraInstancia->_masaCadaSegmento;
	_longitudHabilitadaCuerda				= otraInstancia->_longitudHabilitadaCuerda;
	_desplazamientoCuerdaInicio				= otraInstancia->_desplazamientoCuerdaInicio;
	_texSegmentos							= otraInstancia->_texSegmentos; _texSegmentos->retener(NB_RETENEDOR_THIS);
	_conteoSegmentos						= otraInstancia->_conteoSegmentos;
	_adicionalEntreSegmentos				= otraInstancia->_adicionalEntreSegmentos;
	_segsNecesariosParaCorte				= otraInstancia->_segsNecesariosParaCorte;
	//
	_damping								= otraInstancia->_damping;
	_k2										= otraInstancia->_k2;
	_k3										= otraInstancia->_k3;
	_indicePrimerSegmentoHabilitado			= otraInstancia->_indicePrimerSegmentoHabilitado;
	_indiceUltimoSegmentoHabilitado			= otraInstancia->_indiceUltimoSegmentoHabilitado;
	//Crear cuerda
	_escuchadorCuerda		= NULL;
	_unionCuerdaOpcional	= NULL;
	//
	_cuerdaCajaLocalSucia	= true;
	NBCAJAAABB_INICIALIZAR(_cuerdaCajaLocal);
	//
	this->reanudarAnimacion();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::privInicializarCuerda(SI32 cantVertices, float longitudSegmentos, float masaSegmentos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::privInicializarCuerda")
	NBASSERT(cantVertices >= 3);
	_datosVertices.vaciar();
	_datosSegmentos.vaciar();
	SI32 i;
	for (i = 0; i < cantVertices; ++i){
		STCuerdaVertice v;
		v.ps.x		= i * longitudSegmentos;
		v.ps.y		= 3.0f;
		v.p0s		= v.ps;
		v.vs.x = v.vs.y = 0; //.SetZero();
		if (masaSegmentos > 0.0f){
			v.ims = 1.0f / masaSegmentos;
		} else {
			v.ims = 0.0f;
		}
		_datosVertices.agregarElemento(v);
	}
	SI32 count2 = cantVertices - 1;
	for (i = 0; i < count2; ++i){
		STCuerdaSegmento s;
		NBPunto p1				= _datosVertices.elemento[i].ps;
		NBPunto p2				= _datosVertices.elemento[i+1].ps;
		s.Ls					= NBPUNTO_DISTANCIA(p1.x, p1.y, p2.x, p2.y); //b2Distance(p1, p2);
		s.factorDanoPorCorte	= 0.0f;
		_datosSegmentos.agregarElemento(s);
	}
	NBASSERT(_datosVertices.conteo==_datosSegmentos.conteo+1)
	//
	_indicePrimerSegmentoHabilitado			= 0;
	_indiceUltimoSegmentoHabilitado			= cantVertices-2;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaCuerda::~AUEscenaCuerda(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::~AUEscenaCuerda")
	NBASSERT(!_registradoComoCortable)
	if(_texSegmentos != NULL) { _texSegmentos->liberar(NB_RETENEDOR_THIS); _texSegmentos = NULL; }
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaCuerda* AUEscenaCuerda::crearCuerdaAPartirDeSegmento(UI16 iSegmentoInicial){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::crearCuerdaAPartirDeSegmento")
	NBASSERT(iSegmentoInicial>=_indicePrimerSegmentoHabilitado && iSegmentoInicial<=_indiceUltimoSegmentoHabilitado)
	AUEscenaCuerda* cuerdaNueva = new(this) AUEscenaCuerda(this);
	//Quitar sobrantes al inicio
	UI16 cantSegmentosInicialesQuitar = iSegmentoInicial;
	if(cantSegmentosInicialesQuitar!=0){
		cuerdaNueva->_datosVertices.quitarElementosEnIndice(0, cantSegmentosInicialesQuitar);
		cuerdaNueva->_datosSegmentos.quitarElementosEnIndice(0, cantSegmentosInicialesQuitar);
	}
	//Deshabilitar ancla al inicio
	cuerdaNueva->_mascaraPropsCuerda				&= ~AUESCENACUERDA_BIT_ANCLA_INI_ACTIVA;
	//Inicializar variables necesarias
	cuerdaNueva->_conteoSegmentos					= cuerdaNueva->_datosSegmentos.conteo;
	cuerdaNueva->_indicePrimerSegmentoHabilitado	= 0;
	cuerdaNueva->_indiceUltimoSegmentoHabilitado	= cuerdaNueva->_datosVertices.conteo-2;
	cuerdaNueva->_longitudHabilitadaCuerda			= (cuerdaNueva->_texSegmentos->tamanoHD().ancho - cuerdaNueva->_adicionalEntreSegmentos) * cuerdaNueva->_datosSegmentos.conteo;
	//Verificacion final
	NBASSERT(cuerdaNueva->_datosVertices.conteo==cuerdaNueva->_datosSegmentos.conteo+1)
	NBASSERT(cuerdaNueva->_conteoSegmentos==cuerdaNueva->_datosSegmentos.conteo)
	//
	cuerdaNueva->autoLiberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cuerdaNueva;
}

void AUEscenaCuerda::quitarSegmentosDespuesDeInclusive(UI16 iSegmentoInicial){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::quitarSegmentosDespuesDeInclusive")
	NBASSERT(iSegmentoInicial>=_indicePrimerSegmentoHabilitado && iSegmentoInicial<=_indiceUltimoSegmentoHabilitado)
	//Quitar sobrantes al final
	UI16 cantSegmentosFinalesQuitar = _datosSegmentos.conteo - iSegmentoInicial;
	if(cantSegmentosFinalesQuitar!=0){
		_datosVertices.quitarElementosEnIndice(iSegmentoInicial+1, cantSegmentosFinalesQuitar);
		_datosSegmentos.quitarElementosEnIndice(iSegmentoInicial, cantSegmentosFinalesQuitar);
	}
	//Deshabilitar ancla al final
	_mascaraPropsCuerda						&= ~AUESCENACUERDA_BIT_ANCLA_FIN_ACTIVA;
	//Inicializar variables necesarias
	_conteoSegmentos						= _datosSegmentos.conteo;
	_indicePrimerSegmentoHabilitado			= 0;
	_indiceUltimoSegmentoHabilitado			= _datosVertices.conteo-2;
	_longitudHabilitadaCuerda				= (_texSegmentos->tamanoHD().ancho - _adicionalEntreSegmentos) * _datosSegmentos.conteo;
	//Verificacion final
	NBASSERT(_datosVertices.conteo==_datosSegmentos.conteo+1)
	NBASSERT(_conteoSegmentos==_datosSegmentos.conteo)
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::inicializarCuerdaSegunLinea(const float xFisicaIni, const float yFisicaIni, const float xFisicaFin, const float yFisicaFin){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::inicializarCuerdaSegunLinea")
	UI16 iVert, iUltVert	= _indiceUltimoSegmentoHabilitado + 1;
	UI16 conteoSegmentos	= _indiceUltimoSegmentoHabilitado - _indicePrimerSegmentoHabilitado + 1;
	NBPunto avancePorSegmento; NBPUNTO_ESTABLECER(avancePorSegmento, (xFisicaFin-xFisicaIni)/conteoSegmentos, (yFisicaFin-yFisicaIni)/conteoSegmentos)
	NBPunto posActual;		NBPUNTO_ESTABLECER(posActual, xFisicaIni, yFisicaIni)
	for(iVert=_indicePrimerSegmentoHabilitado; iVert<=iUltVert; iVert++){
		_datosVertices.elemento[iVert].ps = posActual;
		_datosVertices.elemento[iVert].p0s = posActual;
		//
		posActual.x			+=  avancePorSegmento.x;
		posActual.y			+=  avancePorSegmento.y;
	}
	_mascaraPropsCuerda		&= ~AUESCENACUERDA_BIT_VERTICES_SON_LOCALES;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::agregarSegmentosDeCuerda(AUEscenaCuerda* otraCuerda, UI16 iPrimerSegmento, UI16 iUltimoSegmento){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::agregarSegmentosDeCuerda")
	NBASSERT(otraCuerda != NULL)
	NBASSERT(iPrimerSegmento>=otraCuerda->_indicePrimerSegmentoHabilitado)
	NBASSERT(iUltimoSegmento<=otraCuerda->_indiceUltimoSegmentoHabilitado)
	NBASSERT(iPrimerSegmento<=iUltimoSegmento)
	UI16 iUltVerticeAntes	= _indiceUltimoSegmentoHabilitado + 1;
	UI16 iUltSegmentoAntes	= _indiceUltimoSegmentoHabilitado;
	//Determinar movimiento desde primer segmento
	NBPunto posUltVert		= _datosVertices.elemento[iUltVerticeAntes].ps;
	NBPunto posPriVert		= otraCuerda->_datosVertices.elemento[iPrimerSegmento].ps;
	NBPunto desplazamiento;	NBPUNTO_ESTABLECER(desplazamiento, posPriVert.x-posUltVert.x, posPriVert.y-posUltVert.y)
	//Agregar nuevos vertices
	UI32 iVertOrig			= iUltVerticeAntes + 1;
	UI32 iVert, iUltVert	= iUltimoSegmento + 1;
	for(iVert=iPrimerSegmento+1; iVert<=iUltVert; iVert++){
		STCuerdaVertice v	= otraCuerda->_datosVertices.elemento[iVert];
		v.ps.x				-= desplazamiento.x;
		v.ps.y				-= desplazamiento.y;
		v.p0s.x				-= desplazamiento.x;
		v.p0s.y				-= desplazamiento.y;
		if (otraCuerda->_masaCadaSegmento > 0.0f){
			v.ims = 1.0f / otraCuerda->_masaCadaSegmento;
		} else {
			v.ims = 0.0f;
		}
		if(iVertOrig<_datosVertices.conteo){
			_datosVertices.elemento[iVertOrig] = v;
		} else {
			_datosVertices.agregarElemento(v);
		}
		iVertOrig++;
	}
	//Agregar nuevos segmentos
	float nuevaLongitudAcumulada = 0.0f;
	UI16 iSegOrig			= iUltSegmentoAntes + 1;
	UI16 iSeg;
	for(iSeg=iPrimerSegmento; iSeg<=iUltimoSegmento; iSeg++){
		STCuerdaSegmento s	= otraCuerda->_datosSegmentos.elemento[iSeg];
		nuevaLongitudAcumulada += s.Ls;
		if(iSegOrig<_datosSegmentos.conteo){
			_datosSegmentos.elemento[iSegOrig] = s;
		} else {
			_datosSegmentos.agregarElemento(s);
		}
		iSegOrig++;
	}
	_longitudHabilitadaCuerda		+= (nuevaLongitudAcumulada * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
	_indiceUltimoSegmentoHabilitado	+= (iUltimoSegmento - iPrimerSegmento + 1);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::agregarSegmentosDeCuerdaOrdenInverso(AUEscenaCuerda* otraCuerda, UI16 iPrimerSegmento, UI16 iUltimoSegmento){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::agregarSegmentosDeCuerdaOrdenInverso")
	NBASSERT(otraCuerda != NULL)
	NBASSERT(iPrimerSegmento>=otraCuerda->_indicePrimerSegmentoHabilitado)
	NBASSERT(iUltimoSegmento<=otraCuerda->_indiceUltimoSegmentoHabilitado)
	NBASSERT(iPrimerSegmento<=iUltimoSegmento)
	UI16 iUltVerticeAntes	= _indiceUltimoSegmentoHabilitado + 1;
	UI16 iUltSegmentoAntes	= _indiceUltimoSegmentoHabilitado;
	//Determinar movimiento desde primer segmento
	NBPunto posUltVert		= _datosVertices.elemento[iUltVerticeAntes].ps;
	NBPunto posPriVert		= otraCuerda->_datosVertices.elemento[iUltimoSegmento+1].ps;
	NBPunto desplazamiento;	NBPUNTO_ESTABLECER(desplazamiento, posPriVert.x-posUltVert.x, posPriVert.y-posUltVert.y)
	//Agregar nuevos vertices
	UI32 iVertOrig			= iUltVerticeAntes + 1;
	SI16 iVert, iUltVert	= iPrimerSegmento+1;
	for(iVert=iUltimoSegmento+1; iVert>=iUltVert; iVert--){
		STCuerdaVertice v	= otraCuerda->_datosVertices.elemento[iVert];
		v.ps.x				-= desplazamiento.x;
		v.ps.y				-= desplazamiento.y;
		v.p0s.x				-= desplazamiento.x;
		v.p0s.y				-= desplazamiento.y;
		if (otraCuerda->_masaCadaSegmento > 0.0f){
			v.ims = 1.0f / otraCuerda->_masaCadaSegmento;
		} else {
			v.ims = 0.0f;
		}
		if(iVertOrig<_datosVertices.conteo){
			_datosVertices.elemento[iVertOrig] = v;
		} else {
			_datosVertices.agregarElemento(v);
		}
		iVertOrig++;
	}
	//Agregar nuevos segmentos
	float nuevaLongitudAcumulada = 0.0f;
	UI32 iSegOrig			= iUltSegmentoAntes + 1;
	SI32 iSeg;
	for(iSeg=iUltimoSegmento; iSeg>=iPrimerSegmento; iSeg--){
		STCuerdaSegmento s	= otraCuerda->_datosSegmentos.elemento[iSeg];
		nuevaLongitudAcumulada += s.Ls;
		if(iSegOrig<_datosSegmentos.conteo){
			_datosSegmentos.elemento[iSegOrig] = s;
		} else {
			_datosSegmentos.agregarElemento(s);
		}
		iSegOrig++;
	}
	_longitudHabilitadaCuerda		+= (nuevaLongitudAcumulada * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
	_indiceUltimoSegmentoHabilitado	+= (iUltimoSegmento - iPrimerSegmento + 1);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

ENCuerdaTipo AUEscenaCuerda::tipoCuerda(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::anclaIniHabilitada")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return (ENCuerdaTipo)_tipoCuerda;
}

float AUEscenaCuerda::adicionalSegmentos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::adicionalSegmentos")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _adicionalEntreSegmentos;
}

float AUEscenaCuerda::masaSegmentos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::masaSegmentos")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _masaCadaSegmento;
}

float AUEscenaCuerda::factorEstiramiento(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::factorEstiramiento")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _damping;
}

float AUEscenaCuerda::segsNecesariosParaCorte(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::segsNecesariosParaCorte")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _segsNecesariosParaCorte;
	
}

float AUEscenaCuerda::k2(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::k2")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _k2;
}

float AUEscenaCuerda::k3(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::k3")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _k3;
}

UI16 AUEscenaCuerda::indicePrimerSegmentoHabilitado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::indicePrimerSegmentoHabilitado")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _indicePrimerSegmentoHabilitado;
}

UI16 AUEscenaCuerda::indiceUltimoSegmentoHabilitado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::indiceUltimoSegmentoHabilitado")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _indiceUltimoSegmentoHabilitado;
}

bool AUEscenaCuerda::anclaIniHabilitada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::anclaIniHabilitada")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return (_mascaraPropsCuerda & AUESCENACUERDA_BIT_ANCLA_INI_ACTIVA)!=0;
}

bool AUEscenaCuerda::anclaFinHabilitada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::anclaFinHabilitada")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return (_mascaraPropsCuerda & AUESCENACUERDA_BIT_ANCLA_FIN_ACTIVA)!=0;
}

NBPunto AUEscenaCuerda::posAnclaIni(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::posAnclaIni")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _anclaIniPos;
}

NBPunto AUEscenaCuerda::posAnclaFin(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::posAnclaFin")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _anclaFinPos;
}

float AUEscenaCuerda::longitudHabilitadaCuerda(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::longitudHabilitadaCuerda")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _longitudHabilitadaCuerda;
}

NBPunto AUEscenaCuerda::posLocalPrimerVerticeHabilitado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::posLocalPrimerVerticeHabilitado")
	NBPunto vertice = _datosVertices.elemento[_indicePrimerSegmentoHabilitado].ps;
	vertice.x *= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	vertice.y *= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	NBMatriz matrizInversa; NBMATRIZ_INVERSA(matrizInversa, _cacheObjEscena.matrizEscena)
	NBPunto posLocal; NBMATRIZ_MULTIPLICAR_PUNTO(posLocal, matrizInversa, vertice.x, vertice.y)
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return posLocal;
}

NBPunto AUEscenaCuerda::posLocalUltimoVerticeHabilitado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::posLocalUltimoVerticeHabilitado")
	NBPunto vertice = _datosVertices.elemento[_indiceUltimoSegmentoHabilitado+1].ps;
	vertice.x *= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	vertice.y *= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	NBMatriz matrizInversa; NBMATRIZ_INVERSA(matrizInversa, _cacheObjEscena.matrizEscena)
	NBPunto posLocal; NBMATRIZ_MULTIPLICAR_PUNTO(posLocal, matrizInversa, vertice.x, vertice.y)
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return posLocal;
}

AUTextura* AUEscenaCuerda::texturaSegmentosCuerda(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::texturaSegmentosCuerda")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _texSegmentos;
}

IEscuchadorCuerda* AUEscenaCuerda::escuchadorCuerda(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::escuchadorCuerda")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _escuchadorCuerda;
}

/*b2RopeJoint*/ void* AUEscenaCuerda::unionCuerda(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::unionCuerda")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _unionCuerdaOpcional;
}

void AUEscenaCuerda::establecerTipoCuerda(ENCuerdaTipo tipo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::establecerTipoCuerda")
	NBASSERT(tipo>=0 && tipo<ENCuerdaTipo_Conteo)
	_tipoCuerda = tipo;
	if(_tipoCuerda!=ENCuerdaTipo_Cadena && _adicionalEntreSegmentos!=0.0f){
		this->establecerAdicionalSegmentos(0.0f); //Esto tambien actualiza la cuerda
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::establecerTexturaSegmentos(AUTextura* textura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::establecerTexturaSegmentos")
	NBASSERT(textura != NULL)
	textura->retener(NB_RETENEDOR_THIS);
	_texSegmentos->liberar(NB_RETENEDOR_THIS);
	_texSegmentos = textura;
	//Actualizar la cuerda
	establecerLongitudCuerdaHabilitada(_longitudHabilitadaCuerda, _desplazamientoCuerdaInicio);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::establecerAdicionalSegmentos(float longitudAdicional){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::establecerAdicionalSegmentos")
	NBASSERT(longitudAdicional>=0.0f)
	_adicionalEntreSegmentos	= _tipoCuerda!=ENCuerdaTipo_Cadena?0.0f:longitudAdicional;
	NBTamano tamSegmentoCuerda	= _texSegmentos->tamanoHD();
	float longitudSegmento		= tamSegmentoCuerda.ancho - _adicionalEntreSegmentos;
	if(longitudSegmento<8.0f){
		longitudSegmento		= 8.0f;
		_adicionalEntreSegmentos = tamSegmentoCuerda.ancho - longitudSegmento; if(_adicionalEntreSegmentos<0.0f) _adicionalEntreSegmentos = 0.0f;
	}
	//Actualizar la cuerda
	establecerLongitudCuerdaHabilitada(_longitudHabilitadaCuerda, _desplazamientoCuerdaInicio);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::establecerMasaSegmentos(float masa){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::establecerMasaSegmentos")
	NBASSERT(masa>=0.0f)
	_masaCadaSegmento = masa;
	float invMasa = 0.0f; if(_masaCadaSegmento!=0.0f) invMasa = 1.0f / _masaCadaSegmento;
	UI16 iV, conteoVertices = _datosVertices.conteo;
	for(iV=0; iV<conteoVertices; iV++){
		_datosVertices.elemento[iV].ims = invMasa;
	}
	if((_mascaraPropsCuerda & AUESCENACUERDA_BIT_ANCLA_INI_ACTIVA)!=0) _datosVertices.elemento[_indicePrimerSegmentoHabilitado].ims = 0.0f;
	if((_mascaraPropsCuerda & AUESCENACUERDA_BIT_ANCLA_FIN_ACTIVA)!=0) _datosVertices.elemento[_indiceUltimoSegmentoHabilitado].ims = 0.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::establecerFactorEstiramiento(float factor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::establecerFactorEstiramiento")
	NBASSERT(factor>=0.0f)
	_damping = factor;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::establecerSegsNecesariosParaCorte(float segundos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::establecerSegsNecesariosParaCorte")
	NBASSERT(segundos>=0.0f)
	_segsNecesariosParaCorte = segundos;
	if(idEscena!=-1){
		if(_segsNecesariosParaCorte!=0.0f && !_registradoComoCortable){
			_registradoComoCortable = true;
			NBGestorEscena::agregarDescriptorCuerda(idEscena, idCapa, this);
		} else if(_segsNecesariosParaCorte==0.0f && _registradoComoCortable) {
			_registradoComoCortable = false;
			NBGestorEscena::quitarDescriptorCuerda(idEscena, idCapa, this);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::establecerK2(float k2){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::establecerK2")
	NBASSERT(k2>=0.0f)
	_k2 = k2;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::establecerK3(float k3){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::establecerK2")
	NBASSERT(k3>=0.0f)
	_k3 = k3;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::establecerUnionCuerda(/*b2RopeJoint*/void* unionCuerdaOpcional){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::establecerUnionCuerda")
	_unionCuerdaOpcional = unionCuerdaOpcional;
	if(_unionCuerdaOpcional != NULL){
		_mascaraPropsCuerda |= AUESCENACUERDA_BIT_ANCLA_INI_ACTIVA | AUESCENACUERDA_BIT_ANCLA_FIN_ACTIVA;
#		ifndef CONFIG_NB_UNSUPPORT_BOX2D
		b2RopeJoint* unionCuerdaOpcional = (b2RopeJoint*)_unionCuerdaOpcional;
		b2Vec2 posA = unionCuerdaOpcional->GetAnchorA();
		b2Vec2 posB = unionCuerdaOpcional->GetAnchorB();
		this->inicializarCuerdaSegunLinea(posA.x, posA.y, posB.x, posB.y);
#		endif
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::establecerEscuchadorCuerda(IEscuchadorCuerda* escuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::establecerEscuchadorCuerda")
	_escuchadorCuerda = escuchador;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::habilitarAnclaCuerdaIni(float xLocalAncla, float yLocalAncla){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::habilitarAnclaCuerdaIni")
	_anclaIniPos.x	= xLocalAncla;
	_anclaIniPos.y	= yLocalAncla;
	_mascaraPropsCuerda |= AUESCENACUERDA_BIT_ANCLA_INI_ACTIVA;
	//_cuerda->anclarVertice(_cuerda->indicePrimerSegmentoHabilitado(), _anclaIniPos.x/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, _anclaIniPos.y/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::habilitarAnclaCuerdaFin(float xLocalAncla, float yLocalAncla){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::habilitarAnclaCuerdaFin")
	_anclaFinPos.x	= xLocalAncla;
	_anclaFinPos.y	= yLocalAncla;
	_mascaraPropsCuerda |= AUESCENACUERDA_BIT_ANCLA_FIN_ACTIVA;
	//_cuerda->anclarVertice(_cuerda->indiceUltimoSegmentoHabilitado()+1, _anclaFinPos.x/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, _anclaFinPos.y/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::deshabilitarAnclaCuerdaIni(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::deshabilitarAnclaCuerdaIni")
	_mascaraPropsCuerda &= ~AUESCENACUERDA_BIT_ANCLA_INI_ACTIVA;
	desanclarVertice(_indicePrimerSegmentoHabilitado, _masaCadaSegmento);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::deshabilitarAnclaCuerdaFin(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::deshabilitarAnclaCuerdaFin")
	_mascaraPropsCuerda &= ~AUESCENACUERDA_BIT_ANCLA_FIN_ACTIVA;
	desanclarVertice(_indiceUltimoSegmentoHabilitado+1, _masaCadaSegmento);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::establecerLongitudCuerdaHabilitada(float longitudHabilitadaCuerdaEscena, float desplazamientoAlInicio){
	NBASSERT(longitudHabilitadaCuerdaEscena>=0.0f) //No se admiten valores negativos
	NBASSERT(desplazamientoAlInicio>=0.0f) //No se admiten valores negativos
	NBTamano tamSegmentoCuerda	= _texSegmentos->tamanoHD();
	float longitudSegmento		= tamSegmentoCuerda.ancho - _adicionalEntreSegmentos;
	//Restablecer el largo de todos los segmentos
	UI32 iSeg, iSegConteo = _datosSegmentos.conteo;
	for(iSeg=0; iSeg<iSegConteo; iSeg++){
		_datosSegmentos.elemento[iSeg].Ls = longitudSegmento / ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	}
	//Agregar los segmentos necesarios adicionales
	float actualLongitudCuerda	= _datosSegmentos.conteo * longitudSegmento;
	float nuevaLongitudCuerda	= desplazamientoAlInicio + longitudHabilitadaCuerdaEscena;
	if(nuevaLongitudCuerda>actualLongitudCuerda){
		float longitudAdicional = nuevaLongitudCuerda - actualLongitudCuerda;
		float segementosAdicF	= longitudAdicional / longitudSegmento;
		SI32 segmentosAdicional = (SI32)segementosAdicF; if((float)segmentosAdicional!=segementosAdicF) segmentosAdicional++;
		SI32 iPrimerVertRelleno = _datosVertices.conteo;
		SI32 nuevoConteoSegmento = _datosVertices.conteo + segmentosAdicional;
		//
		NBPunto posAnt;
		if(_datosVertices.conteo!=0){
			posAnt = _datosVertices.elemento[_datosVertices.conteo-1].ps;
		} else {
			NBPUNTO_ESTABLECER(posAnt, 0.0f, 0.0f);
		}
		SI32 i; 
		for (i = iPrimerVertRelleno; i<nuevoConteoSegmento; ++i){
			STCuerdaVertice v;
			posAnt.x	+= longitudSegmento/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
			v.ps		= posAnt;
			v.p0s		= v.ps;
			v.vs.x		= 0; //SetZero();
			v.vs.y		= 0;
			float m		= _masaCadaSegmento;
			if (m > 0.0f){
				v.ims = 1.0f / m;
			} else {
				v.ims = 0.0f;
			}
			_datosVertices.agregarElemento(v);
		}
		SI32 count2 = nuevoConteoSegmento - 1;
		for (i = iPrimerVertRelleno-1; i < count2; ++i){
			STCuerdaSegmento s;
			s.Ls					= longitudSegmento / ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
			s.factorDanoPorCorte	= 0.0f;
			_datosSegmentos.agregarElemento(s);
		}
		//
		NBASSERT(_datosVertices.conteo==_datosSegmentos.conteo+1)
	}
	//
	float iInicioSegmento		= desplazamientoAlInicio / longitudSegmento;
	float iFinalSegmento		= (desplazamientoAlInicio + longitudHabilitadaCuerdaEscena) / longitudSegmento;
	UI32 indicePrimerSegmento	= iInicioSegmento;
	UI32 indiceUltimoSegmento	= iFinalSegmento;
	float porcionPrimerSegmento	= 1.0f - (iInicioSegmento - (float)indicePrimerSegmento);	if(porcionPrimerSegmento<0.0f) porcionPrimerSegmento = 0.0f; if(porcionPrimerSegmento>1.0f) porcionPrimerSegmento = 1.0f; NBASSERT(porcionPrimerSegmento>=0.0f && porcionPrimerSegmento<=1.0f)
	float porcionUltimoSegmento	= iFinalSegmento - (float)indiceUltimoSegmento;				if(porcionUltimoSegmento<0.0f) porcionUltimoSegmento = 0.0f; if(porcionUltimoSegmento>1.0f) porcionUltimoSegmento = 1.0f; NBASSERT(porcionUltimoSegmento>=0.0f && porcionUltimoSegmento<=1.0f)
	if(porcionUltimoSegmento<=0.001f){ //Si el ultimo segmento no tiene longitud, entonces corresponde el segmento anterior completo
		indiceUltimoSegmento--;
		porcionUltimoSegmento = 1.0f;
	}
	NBASSERT(indicePrimerSegmento<_datosVertices.conteo-1)
	NBASSERT(indiceUltimoSegmento<_datosVertices.conteo-1)
	NBASSERT(indicePrimerSegmento<=indiceUltimoSegmento)
	//PRINTF_INFO("Cuerda redimensionada: %f segmentos, iInicioSeg %f (porcion %f), iFinalSeg %f (porcion %f)\n", segmentosCuerda, iInicioSegmento, porcionPrimerSegmento, iFinalSegmento, porcionUltimoSegmento);
	//Restablecer ancla de inicio (si esta definida)
	if((_mascaraPropsCuerda & AUESCENACUERDA_BIT_ANCLA_INI_ACTIVA)!=0){
		desanclarVertice(_indicePrimerSegmentoHabilitado, _masaCadaSegmento);	
		anclarVertice(indicePrimerSegmento, _anclaIniPos.x/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, _anclaIniPos.y/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
	}
	//Restablecer ancla de fin (si esta definida)
	if((_mascaraPropsCuerda & AUESCENACUERDA_BIT_ANCLA_FIN_ACTIVA)!=0){
		desanclarVertice(_indiceUltimoSegmentoHabilitado+1, _masaCadaSegmento);
		anclarVertice(indiceUltimoSegmento+1, _anclaFinPos.x/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, _anclaFinPos.y/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
	}
	//
	_indicePrimerSegmentoHabilitado = indicePrimerSegmento;
	_indiceUltimoSegmentoHabilitado = indiceUltimoSegmento;
	_datosSegmentos.elemento[_indicePrimerSegmentoHabilitado].Ls = porcionPrimerSegmento*longitudSegmento/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	_datosSegmentos.elemento[_indiceUltimoSegmentoHabilitado].Ls = porcionUltimoSegmento*longitudSegmento/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	//
	_longitudHabilitadaCuerda = longitudHabilitadaCuerdaEscena;
	_desplazamientoCuerdaInicio = desplazamientoAlInicio;
}

float AUEscenaCuerda::distanciaTotalSegmentosHabilitados(){
	UI32 i; float longitudTotalCuerda = 0;
	for(i=_indicePrimerSegmentoHabilitado; i<=_indiceUltimoSegmentoHabilitado; i++){
		longitudTotalCuerda += _datosSegmentos.elemento[i].Ls;
	}
	return longitudTotalCuerda;
}

void AUEscenaCuerda::anclarVertice(UI32 indiceVertice, float posX, float posY){
	NBASSERT(indiceVertice<_datosVertices.conteo);
	_datosVertices.elemento[indiceVertice].ps.x		= posX;
	_datosVertices.elemento[indiceVertice].ps.y		= posY;
	_datosVertices.elemento[indiceVertice].p0s.x	= posX;
	_datosVertices.elemento[indiceVertice].p0s.y	= posY;
	_datosVertices.elemento[indiceVertice].vs.x		= 0; //.SetZero();
	_datosVertices.elemento[indiceVertice].vs.y		= 0;
	_datosVertices.elemento[indiceVertice].ims		= 0.0f; //esto inabilita el movimiento
}

void AUEscenaCuerda::desanclarVertice(UI32 indiceVertice, float masaVertice){
	if (masaVertice > 0.0f){
		_datosVertices.elemento[indiceVertice].ims	= 1.0f / masaVertice;
	} else {
		_datosVertices.elemento[indiceVertice].ims	= 0.0f;
	}
}

//

void AUEscenaCuerda::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::agregadoEnEscena")
	NBASSERT(idEscena!=-1)
	NBASSERT(!_registradoComoCortable)
	NBGestorAnimadores::agregarAnimador(this, this);
	//Convertir los vertices locales a escena
	if((_mascaraPropsCuerda & AUESCENACUERDA_BIT_VERTICES_SON_LOCALES)!=0){
		NBMatriz matrizEscena = this->matrizEscenaCalculada();
		STCuerdaVertice* vertices = _datosVertices.elemento;
		UI16 iVert, conteoVerts = _datosVertices.conteo;
		for(iVert=0; iVert<conteoVerts; iVert++){
			NBPunto psEscena;	NBMATRIZ_MULTIPLICAR_PUNTO(psEscena, matrizEscena, vertices[iVert].ps.x, vertices[iVert].ps.y)
			NBPunto p0sEscena;	NBMATRIZ_MULTIPLICAR_PUNTO(p0sEscena, matrizEscena, vertices[iVert].p0s.x, vertices[iVert].p0s.y)
			vertices[iVert].ps.x	= psEscena.x / ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
			vertices[iVert].ps.y	= psEscena.y / ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
			vertices[iVert].p0s.x	= p0sEscena.x / ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
			vertices[iVert].p0s.y	= p0sEscena.y / ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
		}
		_mascaraPropsCuerda &= ~AUESCENACUERDA_BIT_VERTICES_SON_LOCALES;
	}
	if(_segsNecesariosParaCorte!=0.0f){
		_registradoComoCortable = true;
		NBGestorEscena::agregarDescriptorCuerda(idEscena, idCapa, this);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::quitandoDeEscena")
	NBGestorAnimadores::quitarAnimador(this);
	if(_registradoComoCortable){
		_registradoComoCortable = false;
		NBGestorEscena::quitarDescriptorCuerda(idEscena, idCapa, this);
	}
	NBASSERT(!_registradoComoCortable)
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool AUEscenaCuerda::cuerdaAnalizarSiRayoCorta(const STEscenaLaserPos* arrPosEscena, const UI16 conteoPosEscena, STCuerdaCorte* guardarCorteEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::cuerdaAnalizarSiRayoCorta")
	NBASSERT(conteoPosEscena>=2)
	bool cortaCuerda				= false;
	float avanceBaseRayo			= 0.0f;
	UI16 iPos;
	for(iPos=1; iPos<conteoPosEscena; iPos++){
		STEscenaLaserPos posAnt		= arrPosEscena[iPos-1];
		STEscenaLaserPos posAct		= arrPosEscena[iPos];
		//Validar si la cajaEscena de la cuerda intersecta la caja del segmento-laser
		if(! (posAnt.posEscena.x<=_cacheObjEscena.cajaEscena.xMin && posAct.posEscena.x<=_cacheObjEscena.cajaEscena.xMin) || (posAnt.posEscena.x>=_cacheObjEscena.cajaEscena.xMax && posAct.posEscena.x>=_cacheObjEscena.cajaEscena.xMax)
		  || (posAnt.posEscena.y<=_cacheObjEscena.cajaEscena.yMin && posAct.posEscena.y<=_cacheObjEscena.cajaEscena.yMin) || (posAnt.posEscena.y>=_cacheObjEscena.cajaEscena.yMax && posAct.posEscena.y>=_cacheObjEscena.cajaEscena.yMax)){
			NBPunto posIniEnFisica;		NBPUNTO_ESTABLECER(posIniEnFisica, posAnt.posEscena.x / ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posAnt.posEscena.y / ESCALA_PIXELES_POR_METRO_MUNDO_FISICA)
			NBPunto posFinEnFisica;		NBPUNTO_ESTABLECER(posFinEnFisica, posAct.posEscena.x / ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posAct.posEscena.y / ESCALA_PIXELES_POR_METRO_MUNDO_FISICA)
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			NBPunto vecRayoDesdePosIni;	NBPUNTO_ESTABLECER(vecRayoDesdePosIni, posFinEnFisica.x - posIniEnFisica.x, posFinEnFisica.y - posIniEnFisica.y)
			float dbgModVectUnit		= NBPUNTO_DISTANCIA_VECTOR(posAnt.vecUnitSegmento.x, posAnt.vecUnitSegmento.y);
			float dbgDistVectorUnit		= NBPUNTO_PRODUCTO_VECTORIAL(posAnt.vecUnitSegmento.x, posAnt.vecUnitSegmento.y, vecRayoDesdePosIni.x, vecRayoDesdePosIni.y);
			NBASSERT(dbgModVectUnit>0.999f && dbgModVectUnit<1.001f)		//Si falla, el vector no es unitario
			NBASSERT(dbgDistVectorUnit>-0.001f && dbgDistVectorUnit<0.001f)	//Si falla, el vector-unitario-cacheado no esta alineado con el vector-ini-fin
			#endif
			SI32 cuerdaSegColision		= -1;
			float cuerdaSegAvanceRelColision = 0.0f;
			float rayoSegAvanceRelColision	= 0.0f;
			NBPunto posAnterior			= _datosVertices.elemento[_indicePrimerSegmentoHabilitado].ps;
			NBPunto vectAnterior;		NBPUNTO_ESTABLECER(vectAnterior, posAnterior.x - posIniEnFisica.x, posAnterior.y - posIniEnFisica.y)
			float prodCruzAnterior		= NBPUNTO_PRODUCTO_VECTORIAL(posAnt.vecUnitSegmento.x, posAnt.vecUnitSegmento.y, vectAnterior.x, vectAnterior.y);
			UI16 iVertAct, iVertAntUlt	= _indiceUltimoSegmentoHabilitado + 1;
			for(iVertAct=_indicePrimerSegmentoHabilitado+1; iVertAct<=iVertAntUlt; iVertAct++){
				NBPunto posActual		= _datosVertices.elemento[iVertAct].ps;
				NBPunto vectActual;		NBPUNTO_ESTABLECER(vectActual, posActual.x - posIniEnFisica.x, posActual.y - posIniEnFisica.y)
				float prodCruzActual	= NBPUNTO_PRODUCTO_VECTORIAL(posAnt.vecUnitSegmento.x, posAnt.vecUnitSegmento.y, vectActual.x, vectActual.y);
				if(!((prodCruzAnterior<0.0f && prodCruzActual<0.0f) || (prodCruzAnterior>=0.0f && prodCruzActual>=0.0f))){
					float relRayo, relSegmento, denom; NBLINEA_INSTERSECCION_RELATIVOS(relRayo, relSegmento, denom, posIniEnFisica.x, posIniEnFisica.y, posFinEnFisica.x, posFinEnFisica.y, posAnterior.x, posAnterior.y, posActual.x, posActual.y) NBASSERT(relSegmento>=-0.001f && relSegmento<=1.001f)
					if(denom!=0.0f && relRayo>=0.0f && relRayo<=1.0f && (cuerdaSegColision==-1 || relRayo<rayoSegAvanceRelColision)){
						cuerdaSegColision			= iVertAct - 1;
						cuerdaSegAvanceRelColision	= relSegmento;
						rayoSegAvanceRelColision	= relRayo;
						cortaCuerda					= true;
					}
				}
				//
				posAnterior				= posActual;
				prodCruzAnterior		= prodCruzActual;
			}
			if(cortaCuerda){
				guardarCorteEn->cuerdaISegmentoCorte	= cuerdaSegColision;
				guardarCorteEn->cuerdaAvanceRelSegmento	= cuerdaSegAvanceRelColision;
				guardarCorteEn->rayoAvanceRelEnRayo		= avanceBaseRayo + rayoSegAvanceRelColision;
				NBPUNTO_ESTABLECER(guardarCorteEn->rayoPosEscenaColision, posAnt.posEscena.x + ((posAct.posEscena.x - posAnt.posEscena.x) * rayoSegAvanceRelColision), posAnt.posEscena.y + ((posAct.posEscena.y - posAnt.posEscena.y) * rayoSegAvanceRelColision))
				NBPUNTO_ESTABLECER(guardarCorteEn->rayoNormalColision, 1.0f, 0.0f)
				break;
			}
		}
		avanceBaseRayo++;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cortaCuerda;
}

void AUEscenaCuerda::cuerdaConsumirCorteDeRayo(const UI16 &iSegmentoCorte, const float factorCorteRayo, const float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::cuerdaConsumirCorteDeRayo")
	NBASSERT(iSegmentoCorte<_datosSegmentos.conteo)
	if(_datosSegmentos.elemento[iSegmentoCorte].factorDanoPorCorte<1.0f){
		NBASSERT(_segsNecesariosParaCorte>0.0f)
		//Danar un poco el segmento anterior
		if(iSegmentoCorte>_indicePrimerSegmentoHabilitado){
			_datosSegmentos.elemento[iSegmentoCorte-1].factorDanoPorCorte += (segsTranscurridos * 0.33f / _segsNecesariosParaCorte);
			if(_datosSegmentos.elemento[iSegmentoCorte-1].factorDanoPorCorte>1.0f) _datosSegmentos.elemento[iSegmentoCorte-1].factorDanoPorCorte = 1.0f;
		}
		//Danar este segmento
		_datosSegmentos.elemento[iSegmentoCorte].factorDanoPorCorte += (segsTranscurridos / _segsNecesariosParaCorte);
		if(_datosSegmentos.elemento[iSegmentoCorte].factorDanoPorCorte>1.0f) _datosSegmentos.elemento[iSegmentoCorte].factorDanoPorCorte = 1.0f;
		//Danar un poco el segmento posterior
		if(iSegmentoCorte<_indiceUltimoSegmentoHabilitado){
			_datosSegmentos.elemento[iSegmentoCorte+1].factorDanoPorCorte += (segsTranscurridos * 0.33f / _segsNecesariosParaCorte);
			if(_datosSegmentos.elemento[iSegmentoCorte+1].factorDanoPorCorte>1.0f) _datosSegmentos.elemento[iSegmentoCorte+1].factorDanoPorCorte = 1.0f;
		}
	}
	NBASSERT(_datosSegmentos.elemento[iSegmentoCorte].factorDanoPorCorte>=0.0f && _datosSegmentos.elemento[iSegmentoCorte].factorDanoPorCorte<=1.0f);
	if(_escuchadorCuerda != NULL && _datosSegmentos.elemento[iSegmentoCorte].factorDanoPorCorte==1.0f){
		_escuchadorCuerda->cuerdaCortada(this, iSegmentoCorte);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::cuerdaRetenerParaAnalisisCorte(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::cuerdaRetenerParaAnalisisCorte")
	this->retener(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::cuerdaLiberarDeAnalisisCorte(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::cuerdaLiberarDeAnalisisCorte")
	this->liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerda::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::tickAnimacion")
	NBASSERT(this->idEscena!=-1)
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	if(!this->_dormido){
		if(_unionCuerdaOpcional != NULL){
#			ifndef CONFIG_NB_UNSUPPORT_BOX2D
			b2RopeJoint* unionCuerdaOpcional = (b2RopeJoint*)_unionCuerdaOpcional;
			b2Vec2 posA = unionCuerdaOpcional->GetAnchorA();
			b2Vec2 posB = unionCuerdaOpcional->GetAnchorB();
			anclarVertice(_indicePrimerSegmentoHabilitado, posA.x, posA.y);
			anclarVertice(_indiceUltimoSegmentoHabilitado+1, posB.x, posB.y);
			//PRINTF_INFO("Anclando extremo ini(%d)-(%f, %f) fin(%d)-(%f, %f) verticesConteo(%d)\n", _cuerda->indicePrimerSegmentoHabilitado(), posA.x, posA.y, _cuerda->indiceUltimoSegmentoHabilitado()+1, posB.x, posB.y, _cuerda->GetVertexCount());
#			endif
		} else {
			if((_mascaraPropsCuerda & AUESCENACUERDA_BIT_ANCLA_INI_ACTIVA)!=0){
				NBPunto posAnclaEscena;	NBMATRIZ_MULTIPLICAR_PUNTO(posAnclaEscena, _cacheObjEscena.matrizEscena, _anclaIniPos.x, _anclaIniPos.y);
				anclarVertice(_indicePrimerSegmentoHabilitado, posAnclaEscena.x/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posAnclaEscena.y/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
			}
			if((_mascaraPropsCuerda & AUESCENACUERDA_BIT_ANCLA_FIN_ACTIVA)!=0){
				NBPunto posAnclaEscena;	NBMATRIZ_MULTIPLICAR_PUNTO(posAnclaEscena, _cacheObjEscena.matrizEscena, _anclaFinPos.x, _anclaFinPos.y);
				anclarVertice(_indiceUltimoSegmentoHabilitado+1, posAnclaEscena.x/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posAnclaEscena.y/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
			}
		}
#		ifndef CONFIG_NB_UNSUPPORT_BOX2D
		this->privStep(segsTranscurridos, 3);
#		endif
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

#ifndef CONFIG_NB_UNSUPPORT_BOX2D
void AUEscenaCuerda::privStep(float segs, UI32 iterations) {
	//NBASSERT(false) //Pendiente habilitar lo de abajo
	if (segs == 0.0){
		return;
	}
	float d = expf(- segs * _damping);
	float radianesDesvioGravedad = NBGestorFisica::radianesDesvioGravedad() / 2.0f;
	if(radianesDesvioGravedad<(PI/-9.0f)) radianesDesvioGravedad = PI/-9.0f;
	if(radianesDesvioGravedad>(PI/9.0f)) radianesDesvioGravedad = PI/9.0f;
	b2Vec2 gravedad;
	if(this->_idMundoFisica != -1){
		const NBPunto g = NBGestorFisica::vectorGravedadFisica(this->_idMundoFisica);
		gravedad.Set(g.x, g.y);
	} else {
		gravedad.Set(0.0f, -9.8f);
	}
	b2Vec2 gravAplicar; NBPUNTO_ROTAR_RADIANES(gravAplicar, gravedad, radianesDesvioGravedad);
	UI32 i;
	for (i = _indicePrimerSegmentoHabilitado/*0*/; i <= _indiceUltimoSegmentoHabilitado+1 /*m_count*/; ++i) {
		_datosVertices.elemento[i].p0s = _datosVertices.elemento[i].ps;
		if (_datosVertices.elemento[i].ims > 0.0f) {
			_datosVertices.elemento[i].vs.x += segs * gravAplicar.x;
			_datosVertices.elemento[i].vs.y += segs * gravAplicar.y;
		}
		_datosVertices.elemento[i].vs.x *= d;
		_datosVertices.elemento[i].vs.y *= d;
		_datosVertices.elemento[i].ps.x += segs * _datosVertices.elemento[i].vs.x;
		_datosVertices.elemento[i].ps.y += segs * _datosVertices.elemento[i].vs.y;
	}
	for (i = 0; i < iterations; ++i){
		privSolveC2();
		privSolveC3();
		privSolveC2();
	}
	float inv_h = 1.0f / segs;
	for (i = _indicePrimerSegmentoHabilitado/*0*/; i <= _indiceUltimoSegmentoHabilitado+1 /*m_count*/; ++i) {
		const NBPunto nps = _datosVertices.elemento[i].ps;
		const NBPunto np0s = _datosVertices.elemento[i].p0s;
		b2Vec2 ps, p0s;
		NBPUNTO_ESTABLECER(ps, nps.x, nps.y)
		NBPUNTO_ESTABLECER(p0s, np0s.x, np0s.y)
		const b2Vec2 vs = inv_h * (ps - p0s);
		_datosVertices.elemento[i].vs.x = vs.x;
		_datosVertices.elemento[i].vs.y = vs.y;
	}
}
#endif

#ifndef CONFIG_NB_UNSUPPORT_BOX2D
void AUEscenaCuerda::privSolveC2() {
	//NBASSERT(false) //Pendiente habilitar lo de abajo
	SI32 count2 = _indiceUltimoSegmentoHabilitado + 1 /*m_count - 1*/;
	for (SI32 i = _indicePrimerSegmentoHabilitado/*0*/; i < count2; ++i) {
		const NBPunto np1 = _datosVertices.elemento[i].ps;
		const NBPunto np2 = _datosVertices.elemento[i + 1].ps;
		b2Vec2 p1, p2;
		NBPUNTO_ESTABLECER(p1, np1.x, np1.y)
		NBPUNTO_ESTABLECER(p2, np2.x, np2.y)
		
		b2Vec2 d = p2 - p1;
		float L = d.Normalize();
		
		float im1 = _datosVertices.elemento[i].ims;
		float im2 = _datosVertices.elemento[i + 1].ims;
		
		if (im1 + im2 == 0.0f)
		{
			continue;
		}
		
		float s1 = im1 / (im1 + im2);
		float s2 = im2 / (im1 + im2);
		
		p1 -= _k2 * s1 * (_datosSegmentos.elemento[i].Ls - L) * d;
		p2 += _k2 * s2 * (_datosSegmentos.elemento[i].Ls - L) * d;
		
		_datosVertices.elemento[i].ps.x = p1.x;
		_datosVertices.elemento[i].ps.y = p1.y;
		_datosVertices.elemento[i + 1].ps.x = p2.x;
		_datosVertices.elemento[i + 1].ps.y = p2.y;
	}
}
#endif

#ifndef CONFIG_NB_UNSUPPORT_BOX2D
void AUEscenaCuerda::privSolveC3(){
	//NBASSERT(false) //Pendiente habilitar lo de abajo
	SI32 count3 = _indiceUltimoSegmentoHabilitado /*m_count - 2;*/;
	for (SI32 i = _indicePrimerSegmentoHabilitado/*0*/; i < count3; ++i) {
		const NBPunto np1 = _datosVertices.elemento[i].ps;
		const NBPunto np2 = _datosVertices.elemento[i + 1].ps;
		const NBPunto np3 = _datosVertices.elemento[i + 2].ps;
		b2Vec2 p1, p2, p3;
		NBPUNTO_ESTABLECER(p1, np1.x, np1.y)
		NBPUNTO_ESTABLECER(p2, np2.x, np2.y)
		NBPUNTO_ESTABLECER(p3, np3.x, np3.y)
		float m1 = _datosVertices.elemento[i].ims;
		float m2 = _datosVertices.elemento[i + 1].ims;
		float m3 = _datosVertices.elemento[i + 2].ims;
		b2Vec2 d1 = p2 - p1;
		b2Vec2 d2 = p3 - p2;
		float L1sqr = d1.LengthSquared();
		float L2sqr = d2.LengthSquared();
		if (L1sqr * L2sqr == 0.0f) {
			continue;
		}
		float a = b2Cross(d1, d2);
		float b = b2Dot(d1, d2);
		float angle = b2Atan2(a, b);
		b2Vec2 Jd1 = (-1.0f / L1sqr) * d1.Skew();
		b2Vec2 Jd2 = (1.0f / L2sqr) * d2.Skew();
		b2Vec2 J1 = -Jd1;
		b2Vec2 J2 = Jd1 - Jd2;
		b2Vec2 J3 = Jd2;
		float mass = m1 * b2Dot(J1, J1) + m2 * b2Dot(J2, J2) + m3 * b2Dot(J3, J3);
		if (mass == 0.0f){
			continue;
		}
		mass = 1.0f / mass;
		float C = angle/* - _datosArticulaciones.elemento[i].as*/;
		while (C > b2_pi){
			angle -= 2 * b2_pi;
			C = angle /*- _datosArticulaciones.elemento[i].as*/;
		}
		while (C < -b2_pi){
			angle += 2.0f * b2_pi;
			C = angle /*- _datosArticulaciones.elemento[i].as*/;
		}
		float impulse = - _k3 * mass * C;
		p1 += (m1 * impulse) * J1;
		p2 += (m2 * impulse) * J2;
		p3 += (m3 * impulse) * J3;
		_datosVertices.elemento[i].ps.x = p1.x;
		_datosVertices.elemento[i].ps.y = p1.y;
		_datosVertices.elemento[i + 1].ps.x = p2.x;
		_datosVertices.elemento[i + 1].ps.y = p2.y;
		_datosVertices.elemento[i + 2].ps.x = p3.x;
		_datosVertices.elemento[i + 2].ps.y = p3.y;
	}
}
#endif

//

NBCajaAABB AUEscenaCuerda::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::cajaAABBLocalCalculada")
	//_cuerdaCajaLocalSucia	= true;
	//NBCAJAAABB_INICIALIZAR(_cuerdaCajaLocal);
	NBMatriz matrizEscenaInversa; NBMATRIZ_INVERSA(matrizEscenaInversa, _cacheObjEscena.matrizEscena);
	NBCajaAABB cajaLocal; NBCAJAAABB_INICIALIZAR(cajaLocal)
	UI16 iVert, iUltVert = _indiceUltimoSegmentoHabilitado+1;
	for(iVert=_indicePrimerSegmentoHabilitado; iVert<=iUltVert; iVert++){
		NBPunto vertice = _datosVertices.elemento[iVert].ps;
		vertice.x *= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA; //A coordenadas de escena
		vertice.y *= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA; //A coordenadas de escena
		NBPunto posLocal;
		NBMATRIZ_MULTIPLICAR_PUNTO(posLocal, matrizEscenaInversa, vertice.x, vertice.y) //A coordenadas locales
		NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, posLocal.x, posLocal.y)
	}
	if(cajaLocal.xMin==cajaLocal.xMax) cajaLocal.xMin -= 6.0f;
	if(cajaLocal.yMin==cajaLocal.yMax) cajaLocal.yMin -= 6.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cajaLocal;
}

PTRfunCmdsGL AUEscenaCuerda::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	STBloqueGL bloqueIndicesGL;			bloqueIndicesGL.primerElemento = bloqueIndicesGL.cantidadElementos = 0;
	UI16 conteoSegmentos				= _indiceUltimoSegmentoHabilitado - _indicePrimerSegmentoHabilitado + 1; NBASSERT(_indicePrimerSegmentoHabilitado>=0) NBASSERT(_indiceUltimoSegmentoHabilitado>=0) NBASSERT(_indicePrimerSegmentoHabilitado<=_indiceUltimoSegmentoHabilitado)
	if(conteoSegmentos != 0){
		NBColor8 colorMultiplicado8; NBCOLOR_ESTABLECER(colorMultiplicado8, propsHeredadas.colorPadre.r*_propiedades.color8.r, propsHeredadas.colorPadre.g*_propiedades.color8.g, propsHeredadas.colorPadre.b*_propiedades.color8.b, propsHeredadas.colorPadre.a*_propiedades.color8.a);
		STBloqueGL bloqueVerticesGL;
		bloqueVerticesGL.primerElemento		= 0;
		bloqueVerticesGL.cantidadElementos	= (conteoSegmentos * 4);
		//Calcular posicion y color por iluminacion
		NBASSERT(_indicePrimerSegmentoHabilitado<_datosVertices.conteo-1)
		NBASSERT(_indiceUltimoSegmentoHabilitado<_datosVertices.conteo-1)
		NBASSERT(_indicePrimerSegmentoHabilitado<=_indiceUltimoSegmentoHabilitado)
		//
		bloqueVerticesGL.primerElemento			= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL.cantidadElementos);
		NBVerticeGL* verticesGL					= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]);
		const STCuerdaVertice* verticesFisica	= _datosVertices.elemento;
		UI16 iVerticeFinal						= _indicePrimerSegmentoHabilitado + 1;
		const UI16 iUltimoVerticeFinal			= _indiceUltimoSegmentoHabilitado + 1;
		const float mitadAnchoSegmento			= _texSegmentos->tamanoHD().alto / 2.0f;
		if(_tipoCuerda==ENCuerdaTipo_Continua){
			// -----------------------
			// -- CUERDA CONTINUA
			// -----------------------
			//PRINTF_INFO("\nCuerda continnua:\n");
			NBPunto vecUnitSegmentoAnt;
			NBPUNTO_ESTABLECER(vecUnitSegmentoAnt, verticesFisica[iVerticeFinal].ps.x - verticesFisica[iVerticeFinal-1].ps.x, verticesFisica[iVerticeFinal].ps.y - verticesFisica[iVerticeFinal-1].ps.y)
			if(vecUnitSegmentoAnt.x!=0.0f || vecUnitSegmentoAnt.y!=0.0f){
				float longSegmentoActual	= NBPUNTO_DISTANCIA_VECTOR(vecUnitSegmentoAnt.x, vecUnitSegmentoAnt.y); NBASSERT(longSegmentoActual>0.0f)
				vecUnitSegmentoAnt.x /= longSegmentoActual; vecUnitSegmentoAnt.y /= longSegmentoActual;
			} else { NBPUNTO_ESTABLECER(vecUnitSegmentoAnt, 1.0f, 0.0f); }
			NBPunto posAnt;		NBPUNTO_ESTABLECER(posAnt, verticesFisica[iVerticeFinal].ps.x * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, verticesFisica[iVerticeFinal].ps.y * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA)
			NBPunto posIzqAnt;	NBPUNTO_ESTABLECER(posIzqAnt, posAnt.x - vecUnitSegmentoAnt.y * mitadAnchoSegmento, posAnt.y + vecUnitSegmentoAnt.x * mitadAnchoSegmento)
			NBPunto posDerAnt;	NBPUNTO_ESTABLECER(posDerAnt, posAnt.x + vecUnitSegmentoAnt.y * mitadAnchoSegmento, posAnt.y - vecUnitSegmentoAnt.x * mitadAnchoSegmento)
			//PRINTF_INFO("Pos ini(%.2f, %.2f) ... izq(%.2f, %.2f), der(%.2f, %.2f)\n", posAnt.x, posAnt.y, posIzqAnt.x, posIzqAnt.y, posDerAnt.x, posDerAnt.y);
			iVerticeFinal++;
			for(; iVerticeFinal<=iUltimoVerticeFinal; iVerticeFinal++){
				NBPunto posAct; NBPUNTO_ESTABLECER(posAct, verticesFisica[iVerticeFinal].ps.x * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, verticesFisica[iVerticeFinal].ps.y * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA)
				NBPunto vecUnitSegmentoAct; NBPUNTO_ESTABLECER(vecUnitSegmentoAct, posAct.x - posAnt.x, posAct.y - posAnt.y)
				if(vecUnitSegmentoAct.x!=0.0f || vecUnitSegmentoAct.y!=0.0f){
					float longSegmentoActual	= NBPUNTO_DISTANCIA_VECTOR(vecUnitSegmentoAct.x, vecUnitSegmentoAct.y); NBASSERT(longSegmentoActual>0.0f)
					vecUnitSegmentoAct.x /= longSegmentoActual; vecUnitSegmentoAct.y /= longSegmentoActual;
				} else {
					NBPUNTO_ESTABLECER(vecUnitSegmentoAct, 1.0f, 0.0f);
				}
				NBPunto vecUnitSumado;	NBPUNTO_ESTABLECER(vecUnitSumado, vecUnitSegmentoAnt.x + vecUnitSegmentoAct.x, vecUnitSegmentoAnt.y + vecUnitSegmentoAct.y)
				if(vecUnitSumado.x!=0.0f || vecUnitSumado.y!=0.0f){
					float longSegmentoActual	= NBPUNTO_DISTANCIA_VECTOR(vecUnitSumado.x, vecUnitSumado.y); NBASSERT(longSegmentoActual>0.0f)
					vecUnitSumado.x /= longSegmentoActual; vecUnitSumado.y /= longSegmentoActual;
				} else { NBPUNTO_ESTABLECER(vecUnitSumado, 1.0f, 0.0f); }
				NBPunto posIzqAct;		NBPUNTO_ESTABLECER(posIzqAct, posAct.x - vecUnitSumado.y * mitadAnchoSegmento, posAct.y + vecUnitSumado.x * mitadAnchoSegmento)
				NBPunto posDerAct;		NBPUNTO_ESTABLECER(posDerAct, posAct.x + vecUnitSumado.y * mitadAnchoSegmento, posAct.y - vecUnitSumado.x * mitadAnchoSegmento)
				//PRINTF_INFO("Pos sig(%.2f, %.2f) ... izq(%.2f, %.2f), der(%.2f, %.2f)\n", posAct.x, posAct.y, posIzqAct.x, posIzqAct.y, posDerAct.x, posDerAct.y);
				float factorInvQuemado	= 1.0f - _datosSegmentos.elemento[iVerticeFinal-1].factorDanoPorCorte; NBASSERT(factorInvQuemado>=0.0f && factorInvQuemado<=1.0f)
				NBColor8 colorSegmento; NBCOLOR_ESTABLECER(colorSegmento, colorMultiplicado8.r*factorInvQuemado, colorMultiplicado8.g*factorInvQuemado, colorMultiplicado8.b*factorInvQuemado, colorMultiplicado8.a)
				NBPUNTO_ESTABLECER(verticesGL[0], posDerAnt.x,	posDerAnt.y)
				NBPUNTO_ESTABLECER(verticesGL[1], posDerAct.x,	posDerAct.y)
				NBPUNTO_ESTABLECER(verticesGL[2], posIzqAnt.x,	posIzqAnt.y)
				NBPUNTO_ESTABLECER(verticesGL[3], posIzqAct.x,	posIzqAct.y)
				NBCOLOR_ESTABLECER(verticesGL[0], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
				NBCOLOR_ESTABLECER(verticesGL[1], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
				NBCOLOR_ESTABLECER(verticesGL[2], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
				NBCOLOR_ESTABLECER(verticesGL[3], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
				_texSegmentos->cargarCoordenadasTextura(verticesGL);
				//Siguiente
				posAnt				= posAct;
				posIzqAnt			= posIzqAct;
				posDerAnt			= posDerAct;
				vecUnitSegmentoAnt	= vecUnitSegmentoAct;
				verticesGL			+= 4;
			}
			NBPunto posIzqAct;	NBPUNTO_ESTABLECER(posIzqAct, posAnt.x - vecUnitSegmentoAnt.y * mitadAnchoSegmento, posAnt.y + vecUnitSegmentoAnt.x * mitadAnchoSegmento)
			NBPunto posDerAct;	NBPUNTO_ESTABLECER(posDerAct, posAnt.x + vecUnitSegmentoAnt.y * mitadAnchoSegmento, posAnt.y - vecUnitSegmentoAnt.x * mitadAnchoSegmento)
			//PRINTF_INFO("Por fin(%.2f, %.2f) ... izq(%.2f, %.2f), der(%.2f, %.2f)\n", posAnt.x, posAnt.y, posIzqAct.x, posIzqAct.y, posDerAct.x, posDerAct.y);
			float factorInvQuemado	= 1.0f - _datosSegmentos.elemento[iVerticeFinal-2].factorDanoPorCorte; NBASSERT(factorInvQuemado>=0.0f && factorInvQuemado<=1.0f)
			NBColor8 colorSegmento; NBCOLOR_ESTABLECER(colorSegmento, colorMultiplicado8.r*factorInvQuemado, colorMultiplicado8.g*factorInvQuemado, colorMultiplicado8.b*factorInvQuemado, colorMultiplicado8.a)
			NBPUNTO_ESTABLECER(verticesGL[0], posDerAnt.x,	posDerAnt.y)
			NBPUNTO_ESTABLECER(verticesGL[1], posDerAct.x,	posDerAct.y)
			NBPUNTO_ESTABLECER(verticesGL[2], posIzqAnt.x,	posIzqAnt.y)
			NBPUNTO_ESTABLECER(verticesGL[3], posIzqAct.x,	posIzqAct.y)
			NBCOLOR_ESTABLECER(verticesGL[0], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
			NBCOLOR_ESTABLECER(verticesGL[1], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
			NBCOLOR_ESTABLECER(verticesGL[2], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
			NBCOLOR_ESTABLECER(verticesGL[3], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
			_texSegmentos->cargarCoordenadasTextura(verticesGL);
			verticesGL			+= 4;
			NBASSERT(verticesGL == &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento+bloqueVerticesGL.cantidadElementos]))
		} else if(_tipoCuerda==ENCuerdaTipo_Cadena) {
			// -----------------------
			// -- CUERDA TIPO CADENA
			// -----------------------
			NBPunto posAnt;	NBPUNTO_ESTABLECER(posAnt, verticesFisica[iVerticeFinal-1].ps.x * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, verticesFisica[iVerticeFinal-1].ps.y * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA)
			for(; iVerticeFinal<=iUltimoVerticeFinal; iVerticeFinal++){
				NBPunto posAct; NBPUNTO_ESTABLECER(posAct, verticesFisica[iVerticeFinal].ps.x * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, verticesFisica[iVerticeFinal].ps.y * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA)
				NBPunto vecUnitAct; NBPUNTO_ESTABLECER(vecUnitAct, posAct.x - posAnt.x, posAct.y - posAnt.y)
				float moduloVector	= NBPUNTO_DISTANCIA_VECTOR(vecUnitAct.x, vecUnitAct.y);
				vecUnitAct.x		/= moduloVector;
				vecUnitAct.y		/= moduloVector;
				float factorInvQuemado	= 1.0f - _datosSegmentos.elemento[iVerticeFinal-1].factorDanoPorCorte; NBASSERT(factorInvQuemado>=0.0f && factorInvQuemado<=1.0f)
				NBColor8 colorSegmento; NBCOLOR_ESTABLECER(colorSegmento, colorMultiplicado8.r*factorInvQuemado, colorMultiplicado8.g*factorInvQuemado, colorMultiplicado8.b*factorInvQuemado, colorMultiplicado8.a)
				//
				NBPUNTO_ESTABLECER(verticesGL[0], posAnt.x + (vecUnitAct.y * mitadAnchoSegmento),	posAnt.y - (vecUnitAct.x * mitadAnchoSegmento))
				NBPUNTO_ESTABLECER(verticesGL[1], posAct.x + (vecUnitAct.y * mitadAnchoSegmento) + (vecUnitAct.x * _adicionalEntreSegmentos),	posAct.y - (vecUnitAct.x * mitadAnchoSegmento) + (vecUnitAct.y * _adicionalEntreSegmentos))
				NBPUNTO_ESTABLECER(verticesGL[2], posAnt.x - (vecUnitAct.y * mitadAnchoSegmento),	posAnt.y + (vecUnitAct.x * mitadAnchoSegmento))
				NBPUNTO_ESTABLECER(verticesGL[3], posAct.x - (vecUnitAct.y * mitadAnchoSegmento) + (vecUnitAct.x * _adicionalEntreSegmentos),	posAct.y + (vecUnitAct.x * mitadAnchoSegmento) + (vecUnitAct.y * _adicionalEntreSegmentos))
				NBCOLOR_ESTABLECER(verticesGL[0], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
				NBCOLOR_ESTABLECER(verticesGL[1], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
				NBCOLOR_ESTABLECER(verticesGL[2], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
				NBCOLOR_ESTABLECER(verticesGL[3], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
				_texSegmentos->cargarCoordenadasTextura(verticesGL);
				//Siguiente
				posAnt			= posAct;
				verticesGL		+= 4;
			}
			NBASSERT(verticesGL == &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento+bloqueVerticesGL.cantidadElementos]))
		} else if(_tipoCuerda==ENCuerdaTipo_CintaDoblable) {
			// -----------------------
			// -- CUERDA TIPO CINTA DOBABLE
			// -----------------------
			NBPunto posAnt;	NBPUNTO_ESTABLECER(posAnt, verticesFisica[iVerticeFinal-1].ps.x * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, verticesFisica[iVerticeFinal-1].ps.y * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA)
			for(; iVerticeFinal<=iUltimoVerticeFinal; iVerticeFinal++){
				NBPunto posAct; NBPUNTO_ESTABLECER(posAct, verticesFisica[iVerticeFinal].ps.x * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, verticesFisica[iVerticeFinal].ps.y * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA)
				float factorInvQuemado	= 1.0f - _datosSegmentos.elemento[iVerticeFinal-1].factorDanoPorCorte; NBASSERT(factorInvQuemado>=0.0f && factorInvQuemado<=1.0f)
				NBColor8 colorSegmento; NBCOLOR_ESTABLECER(colorSegmento, colorMultiplicado8.r*factorInvQuemado, colorMultiplicado8.g*factorInvQuemado, colorMultiplicado8.b*factorInvQuemado, colorMultiplicado8.a)
				//
				NBPUNTO_ESTABLECER(verticesGL[0], posAnt.x + mitadAnchoSegmento,	posAnt.y + mitadAnchoSegmento)
				NBPUNTO_ESTABLECER(verticesGL[1], posAct.x + mitadAnchoSegmento,	posAct.y + mitadAnchoSegmento)
				NBPUNTO_ESTABLECER(verticesGL[2], posAnt.x - mitadAnchoSegmento,	posAnt.y - mitadAnchoSegmento)
				NBPUNTO_ESTABLECER(verticesGL[3], posAct.x - mitadAnchoSegmento,	posAct.y - mitadAnchoSegmento)
				NBCOLOR_ESTABLECER(verticesGL[0], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
				NBCOLOR_ESTABLECER(verticesGL[1], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
				NBCOLOR_ESTABLECER(verticesGL[2], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
				NBCOLOR_ESTABLECER(verticesGL[3], colorSegmento.r, colorSegmento.g, colorSegmento.b, colorSegmento.a)
				_texSegmentos->cargarCoordenadasTextura(verticesGL);
				//Siguiente
				posAnt			= posAct;
				verticesGL		+= 4;
			}
			NBASSERT(verticesGL == &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento+bloqueVerticesGL.cantidadElementos]))
		} else { NBASSERT(false) }
		//Datos render
		NBASSERT(conteoSegmentos > 0)
		bloqueIndicesGL					= NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_4(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL, conteoSegmentos);
		const SI32 indiceDatosRender	= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STCuerdaRender));
		STCuerdaRender* datosRender		= (STCuerdaRender*)&(propsRenderizado.bytesDatosModelos->elemento[indiceDatosRender]);
		datosRender->idTextura			= _texSegmentos->idTexturaGL;	propsRenderizado.piscinaRetecionRender->agregarElemento(_texSegmentos);
		datosRender->bloqueIndicesGL	= bloqueIndicesGL;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return (bloqueIndicesGL.cantidadElementos!=0 ? &AUEscenaCuerda::enviarComandosGL : NULL);
}

void AUEscenaCuerda::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::enviarComandosGL")
	STCuerdaRender* datosRender	= (STCuerdaRender*)punteroDatosModelo;
	NBASSERT(datosRender->bloqueIndicesGL.cantidadElementos>0)
	NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaCuerda")
	NBGestorGL::bindTexture(0, datosRender->idTextura);
	NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(datosRender->bloqueIndicesGL.primerElemento, datosRender->bloqueIndicesGL.cantidadElementos); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaElectroRayo")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

bool AUEscenaCuerda::agregarXmlInternoEn(AUEscenaCuerda* cuerda, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<tp>%d</tp>\r\n", espaciosBaseIzq, (SI32)cuerda->_tipoCuerda);
	guardarEn->agregarConFormato("%s<segms>%d</segms>\r\n", espaciosBaseIzq, cuerda->_conteoSegmentos);
	guardarEn->agregarConFormato("%s<masa>%f</masa>\r\n", espaciosBaseIzq, cuerda->_masaCadaSegmento);
	guardarEn->agregarConFormato("%s<adicion>%f</adicion>\r\n", espaciosBaseIzq, cuerda->_adicionalEntreSegmentos);
	guardarEn->agregarConFormato("%s<longHab>%f</longHab>\r\n", espaciosBaseIzq, cuerda->_longitudHabilitadaCuerda);
	guardarEn->agregarConFormato("%s<sCrta>%f</sCrta>\r\n", espaciosBaseIzq, cuerda->_segsNecesariosParaCorte);
	guardarEn->agregarConFormato("%s<despIni>%f</despIni>\r\n", espaciosBaseIzq, cuerda->_desplazamientoCuerdaInicio);
	guardarEn->agregarConFormato("%s<ancIni>%d</ancIni>\r\n", espaciosBaseIzq, (cuerda->_mascaraPropsCuerda & AUESCENACUERDA_BIT_ANCLA_INI_ACTIVA)!=0?1:0);
	guardarEn->agregarConFormato("%s<ancFin>%d</ancFin>\r\n", espaciosBaseIzq, (cuerda->_mascaraPropsCuerda & AUESCENACUERDA_BIT_ANCLA_FIN_ACTIVA)!=0?1:0);
	guardarEn->agregarConFormato("%s<iX>%f</iX><iY>%f</iY>\r\n", espaciosBaseIzq, cuerda->_anclaIniPos.x, cuerda->_anclaIniPos.y);
	guardarEn->agregarConFormato("%s<fX>%f</fX><fY>%f</fY>\r\n", espaciosBaseIzq, cuerda->_anclaFinPos.x, cuerda->_anclaFinPos.y);
	if(cuerda->_texSegmentos != NULL){
		const char* nombreTextura = NBGestorTexturas::nombreRecursoTextura(cuerda->_texSegmentos);
		if(nombreTextura != NULL){
			guardarEn->agregarConFormato("%s<tex>%s</tex>\r\n", espaciosBaseIzq, nombreTextura);
		}
	}
	//DAROS CUERDA
	guardarEn->agregarConFormato("%s<cuerda>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<m_count>%d</m_count>\r\n", espaciosBaseIzq, (SI32)cuerda->_datosVertices.conteo);
	//guardarEn->agregarConFormato("%s<grav><x>%f</x><y>%f</y></grav>\r\n", espaciosBaseIzq, cuerda->_gravity.x, cuerda->_gravity.y);
	guardarEn->agregarConFormato("%s<m_damping>%f</m_damping>\r\n", espaciosBaseIzq, cuerda->_damping);
	guardarEn->agregarConFormato("%s<m_k2>%f</m_k2>\r\n", espaciosBaseIzq, cuerda->_k2);
	guardarEn->agregarConFormato("%s<m_k3>%f</m_k3>\r\n", espaciosBaseIzq, cuerda->_k3);
	guardarEn->agregarConFormato("%s<iPSH>%d</iPSH>\r\n", espaciosBaseIzq, (SI32)cuerda->_indicePrimerSegmentoHabilitado);
	guardarEn->agregarConFormato("%s<iUSH>%d</iUSH>\r\n", espaciosBaseIzq, (SI32)cuerda->_indiceUltimoSegmentoHabilitado);
	guardarEn->agregarConFormato("%s<vsL>\r\n", espaciosBaseIzq);
	UI32 iVert, iVertConteo = cuerda->_datosVertices.conteo; NBMatriz matrizEscenaInversa = cuerda->matrizEscenaInversaCalculada();
	for(iVert=0; iVert<iVertConteo; iVert++){
		STCuerdaVertice v = cuerda->_datosVertices.elemento[iVert];
		NBPunto psEscena; NBPUNTO_ESTABLECER(psEscena, v.ps.x*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, v.ps.y*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA)
		NBPunto p0sEscena; NBPUNTO_ESTABLECER(p0sEscena, v.p0s.x*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, v.p0s.y*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA)
		NBPunto psLocal; NBMATRIZ_MULTIPLICAR_PUNTO(psLocal, matrizEscenaInversa, psEscena.x, psEscena.y)
		NBPunto p0sLocal; NBMATRIZ_MULTIPLICAR_PUNTO(p0sLocal, matrizEscenaInversa, p0sEscena.x, p0sEscena.y)
		guardarEn->agregarConFormato("%s<v>%f|%f|%f|%f|%f|%f|%f</v>\r\n", espaciosBaseIzq, psLocal.x, psLocal.y, p0sLocal.x, p0sLocal.y, v.vs.x, v.vs.y, v.ims);
	}
	guardarEn->agregarConFormato("%s</vsL>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<segmentos>\r\n", espaciosBaseIzq);
	UI32 iSeg, iSegConteo = cuerda->_datosSegmentos.conteo;
	for(iSeg=0; iSeg<iSegConteo; iSeg++){
		guardarEn->agregarConFormato("%s<s>%f</s>\r\n", espaciosBaseIzq, cuerda->_datosSegmentos.elemento[iSeg].Ls);
	}
	guardarEn->agregarConFormato("%s</segmentos>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</cuerda>\r\n", espaciosBaseIzq);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return true;
}


bool AUEscenaCuerda::interpretarXmlInterno(AUEscenaCuerda* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerda::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
		XML_VALOR_NODO_HIJO(datosXml, cargarEn->_tipoCuerda, "tp", "tipo", (SI32)ENCuerdaTipo_Continua, nodoXml, NULL) NBASSERT(cargarEn->_tipoCuerda<ENCuerdaTipo_Conteo)
		cargarEn->_conteoSegmentos			= datosXml->nodoHijo("segms", 0, nodoXml); NBASSERT(cargarEn->_conteoSegmentos>=2)
		//
		cargarEn->_masaCadaSegmento			= datosXml->nodoHijo("masa", 0.0f, nodoXml);
		cargarEn->_adicionalEntreSegmentos	= datosXml->nodoHijo("adicion", 0.0f, nodoXml);
		cargarEn->_longitudHabilitadaCuerda = datosXml->nodoHijo("longHab", 0.0f, nodoXml);
		cargarEn->_desplazamientoCuerdaInicio = datosXml->nodoHijo("despIni", 0.0f, nodoXml);
		cargarEn->_anclaIniPos.x			= datosXml->nodoHijo("iX", 0.0f, nodoXml);
		cargarEn->_anclaIniPos.y			= datosXml->nodoHijo("iY", 0.0f, nodoXml);
		cargarEn->_anclaFinPos.x			= datosXml->nodoHijo("fX", 0.0f, nodoXml);
		cargarEn->_anclaFinPos.y			= datosXml->nodoHijo("fY", 0.0f, nodoXml);
		bool anclaIniHab					= datosXml->nodoHijo("ancIni", true, nodoXml);
		bool anclaFinHab					= datosXml->nodoHijo("ancFin", true, nodoXml);
		if(anclaIniHab)						cargarEn->_mascaraPropsCuerda |= AUESCENACUERDA_BIT_ANCLA_INI_ACTIVA;
		else								cargarEn->_mascaraPropsCuerda &= ~AUESCENACUERDA_BIT_ANCLA_INI_ACTIVA;
		if(anclaFinHab)						cargarEn->_mascaraPropsCuerda |= AUESCENACUERDA_BIT_ANCLA_FIN_ACTIVA;
		else								cargarEn->_mascaraPropsCuerda &= ~AUESCENACUERDA_BIT_ANCLA_FIN_ACTIVA;
		cargarEn->establecerSegsNecesariosParaCorte(datosXml->nodoHijo("sCrta", 0.0f, nodoXml));
		//
		AUCadenaMutable8* nomTextura = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
		datosXml->nodoHijo("tex", nomTextura, "", nodoXml);
		if(nomTextura->tamano()!=0){
			cargarEn->_texSegmentos = NBGestorTexturas::texturaDesdeArchivo(nomTextura->str());
			cargarEn->_texSegmentos->retener(NB_RETENEDOR_NULL);
		}
		nomTextura->liberar(NB_RETENEDOR_NULL);
		//
		const sNodoXML* nodoCuerda			= datosXml->nodoHijo("cuerda", nodoXml);
		if(nodoCuerda != NULL){
			SI32 conteoVertices				= datosXml->nodoHijo("m_count", 0, nodoCuerda); NBASSERT(conteoVertices>=3)
			//
			cargarEn->_damping				= datosXml->nodoHijo("m_damping", 0.0f, nodoCuerda);
			cargarEn->_k2					= datosXml->nodoHijo("m_k2", 0.0f, nodoCuerda);
			cargarEn->_k3					= datosXml->nodoHijo("m_k3", 0.0f, nodoCuerda);
			XML_VALOR_NODO_HIJO(datosXml, cargarEn->_indicePrimerSegmentoHabilitado, "iPSH", "_indicePrimerSegmentoHabilitado", 0, nodoCuerda, NULL)
			XML_VALOR_NODO_HIJO(datosXml, cargarEn->_indiceUltimoSegmentoHabilitado, "iUSH", "_indiceUltimoSegmentoHabilitado", 0, nodoCuerda, NULL)
			/*const sNodoXML* nodoGrav = datosXml->nodoHijo("grav", nodoCuerda);
			if(nodoGrav != NULL){
				cargarEn->_gravity.x = datosXml->nodoHijo("x", 0.0f, nodoGrav);
				cargarEn->_gravity.y = datosXml->nodoHijo("y", 0.0f, nodoGrav);
			}*/
			cargarEn->_datosVertices.vaciar();
			cargarEn->_datosSegmentos.vaciar();
			cargarEn->_mascaraPropsCuerda &= ~AUESCENACUERDA_BIT_VERTICES_SON_LOCALES;
			const sNodoXML* nodoVertices; XML_NODO_HIJO(datosXml, nodoVertices, "vs", "vertices", nodoCuerda, NULL)
			if(nodoVertices == NULL){
				nodoVertices	= datosXml->nodoHijo("vsL", nodoCuerda);
				if(nodoVertices != NULL) cargarEn->_mascaraPropsCuerda |= AUESCENACUERDA_BIT_VERTICES_SON_LOCALES;
			}
			if(nodoVertices != NULL){
				SI32 iVert = 0;
				const sNodoXML* nodoV	= datosXml->nodoHijo("v", nodoVertices);
				while(nodoV != NULL){
					NBASSERT(iVert<conteoVertices);
					if(iVert<conteoVertices){
						STCuerdaVertice v;
						if(nodoV->nodosHijos == NULL){
							//Formato nuevo CSV (mas compacto)
							const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoV);
							SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
							SI32 posEnCadenaCSV		= 0;
							v.ps.x					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							v.ps.y					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							v.p0s.x					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							v.p0s.y					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							v.vs.x					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							v.vs.y					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							v.ims					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						} else {
							//Formato viejo XML (muy inflado)
							v.ps.x		= datosXml->nodoHijo("pX", 0.0f, nodoV);
							v.ps.y		= datosXml->nodoHijo("pY", 0.0f, nodoV);
							v.p0s.x		= datosXml->nodoHijo("p0X", 0.0f, nodoV);
							v.p0s.y		= datosXml->nodoHijo("p0Y", 0.0f, nodoV);
							v.vs.x		= datosXml->nodoHijo("vX", 0.0f, nodoV);
							v.vs.y		= datosXml->nodoHijo("vY", 0.0f, nodoV);
							v.ims		= datosXml->nodoHijo("ims", 0.0f, nodoV);
						}
						cargarEn->_datosVertices.agregarElemento(v);
					}
					//
					iVert++;
					nodoV	= datosXml->nodoHijo("v", nodoVertices, nodoV);
				}
			}
			const sNodoXML* nodoSegmentos	= datosXml->nodoHijo("segmentos", nodoCuerda);
			if(nodoSegmentos != NULL){
				SI32 iSeg = 0;
				const sNodoXML* nodoS	= datosXml->nodoHijo("s", nodoSegmentos);
				while(nodoS != NULL){
					NBASSERT(iSeg<conteoVertices-1);
					if(iSeg<conteoVertices-1){
						STCuerdaSegmento s;
						if(nodoS->nodosHijos == NULL){
							//Formato nuevo CSV (mas compacto)
							const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoS);
							SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
							SI32 posEnCadenaCSV		= 0;
							s.Ls					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						} else {
							//Formato viejo XML (muy inflado)
							s.Ls = datosXml->nodoHijo("l", 0.0f, nodoS);
						}
						s.factorDanoPorCorte		= 0.0f;
						cargarEn->_datosSegmentos.agregarElemento(s);
					}
					//
					iSeg++;
					nodoS	= datosXml->nodoHijo("s", nodoSegmentos, nodoS);
				}
			}
			exito = true;
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUEscenaCuerda)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUEscenaCuerda, "AUEscenaCuerda")
AUOBJMETODOS_CLONAR_THIS(AUEscenaCuerda)





