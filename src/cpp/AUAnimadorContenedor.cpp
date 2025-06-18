//
//  AUAnimadorContenedor.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 09/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUAnimadorContenedor.h"

AUAnimadorContenedor::AUAnimadorContenedor() : AUObjeto(), NBAnimador(), _animColorCont(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorContenedor::AUAnimadorContenedor")
	//
	_animandoProps	= false;
	//
	this->reanudarAnimacion();
	NBGestorAnimadores::agregarAnimador(this, this);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUAnimadorContenedor::~AUAnimadorContenedor(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorContenedor::~AUAnimadorContenedor")
	NBGestorAnimadores::quitarAnimador(this);
	this->purgarAnimaciones();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

SI32 AUAnimadorContenedor::conteoAnimacionesEjecutando(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorContenedor::conteoAnimacionesEjecutando")
	SI32 conteoAnim = _animColorCont.conteo;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return conteoAnim;
}

bool AUAnimadorContenedor::animandoObjeto(AUEscenaContenedor* objAnimando){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorContenedor::animandoObjeto")
	bool r = false;
	SI32 iAnim;
	//Animar colores
	for(iAnim=_animColorCont.conteo-1; iAnim>=0 && !r; iAnim--){
		if(_animColorCont.elemento[iAnim].objAnimando == objAnimando){
			r = true; break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

void AUAnimadorContenedor::quitarAnimacionesDeObjeto(AUEscenaObjeto* objAnimando){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorContenedor::quitarAnimacionesDeObjeto")
	SI32 iAnim;
	//Animar colores
	for(iAnim=_animColorCont.conteo-1; iAnim>=0; iAnim--){
		STAnimPropColorContenedor* animDat = &_animColorCont.elemento[iAnim];
		if(animDat->objAnimando == objAnimando){
			animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
			_animColorCont.quitarElementoEnIndice(iAnim);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorContenedor::purgarAnimaciones(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorContenedor::purgarAnimaciones")
	SI32 iAnim;
	//Animar colores
	for(iAnim=_animColorCont.conteo-1; iAnim>=0; iAnim--){
		STAnimPropColorContenedor* animDat = &_animColorCont.elemento[iAnim];
		const NBColor8 valDestino	= animDat->valorDestino;
		(animDat->objAnimando->*animDat->funcion)(valDestino);
		animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
	}
	_animColorCont.vaciar();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

void AUAnimadorContenedor::animarColor(AUEscenaContenedor* objAnimando, NBColor8 valDestino, float segundosAnimacion, PTRfuncObtenerColorCont funcionObtenerValor, PTRfuncEstablecerColorCont funcionEstablecerValor, ENAnimPropVelocidad velocidad){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorContenedor::animarColor")
	NBASSERT(segundosAnimacion>=0.0f);
	if(segundosAnimacion==0.0f){
		(objAnimando->*funcionEstablecerValor)(valDestino);
	} else {
		STAnimPropColorContenedor datosAnim;
		datosAnim.objAnimando	= objAnimando; objAnimando->retener(NB_RETENEDOR_THIS);
		datosAnim.segsAnimar	= segundosAnimacion;
		datosAnim.segsAnimando	= 0.0f;
		datosAnim.velocidadAnim	= velocidad;
		//
		datosAnim.valorInicial	= (objAnimando->*funcionObtenerValor)();
		datosAnim.valorDestino	= valDestino;
		datosAnim.funcion		= funcionEstablecerValor;
		_animColorCont.agregarElemento(datosAnim);
		_animandoProps			= true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorContenedor::animarColor(AUEscenaContenedor* objAnimando, float rDestino, float gDestino, float bDestino, float aDestino, float segundosAnimacion, PTRfuncObtenerColorCont funcionObtenerValor, PTRfuncEstablecerColorCont funcionEstablecerValor, ENAnimPropVelocidad velocidad){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorContenedor::animarColor")
	NBASSERT(segundosAnimacion>=0.0f);
	if(segundosAnimacion==0.0f){
		NBColor8 valDestino; NBCOLOR_ESTABLECER(valDestino, rDestino, gDestino, bDestino, aDestino)
		(objAnimando->*funcionEstablecerValor)(valDestino);
	} else {
		STAnimPropColorContenedor datosAnim;
		datosAnim.objAnimando	= objAnimando; objAnimando->retener(NB_RETENEDOR_THIS);
		datosAnim.segsAnimar	= segundosAnimacion;
		datosAnim.segsAnimando	= 0.0f;
		datosAnim.velocidadAnim	= velocidad;
		//
		datosAnim.valorInicial	= (objAnimando->*funcionObtenerValor)();
		datosAnim.valorDestino.r	= rDestino;
		datosAnim.valorDestino.g	= gDestino;
		datosAnim.valorDestino.b	= bDestino;
		datosAnim.valorDestino.a	= aDestino;
		datosAnim.funcion		= funcionEstablecerValor;
		_animColorCont.agregarElemento(datosAnim);
		_animandoProps			= true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

void AUAnimadorContenedor::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorContenedor::tickAnimacion")
	if(_animandoProps){
		SI32 iAnim;
		//Animar colores
		for(iAnim=_animColorCont.conteo-1; iAnim>=0; iAnim--){
			STAnimPropColorContenedor* animDat = &_animColorCont.elemento[iAnim];
			//
			float segsAnimar	= animDat->segsAnimar;
			float segsAnimando	= animDat->segsAnimando;
			NBColor8 valInicial	= animDat->valorInicial;
			NBColor8 valDestino	= animDat->valorDestino;
			//
			segsAnimando		+= segsTranscurridos; animDat->segsAnimando = segsAnimando;
			if(segsAnimando>=segsAnimar){
				(animDat->objAnimando->*animDat->funcion)(valDestino);
				animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
				_animColorCont.quitarElementoEnIndice(iAnim);
			} else {
				float avance	= (segsAnimando/segsAnimar);
				switch (animDat->velocidadAnim){
					case ENAnimPropVelocidad_Acelerada:		avance	= (avance*avance); break;
					case ENAnimPropVelocidad_Desacelerada:	avance	= pow(avance, 1.0f/2.0f); break;
					case ENAnimPropVelocidad_Acelerada3:	avance	= (avance*avance*avance); break;
					case ENAnimPropVelocidad_Desacelerada3: avance	= pow(avance, 1.0f/3.0f); break;
					default: break;
				}
				NBColor8 valIntermedio;
				valIntermedio.r = valInicial.r+((float)(valDestino.r-valInicial.r)*avance);
				valIntermedio.g = valInicial.g+((float)(valDestino.g-valInicial.g)*avance);
				valIntermedio.b = valInicial.b+((float)(valDestino.b-valInicial.b)*avance);
				valIntermedio.a = valInicial.a+((float)(valDestino.a-valInicial.a)*avance);
				(animDat->objAnimando->*animDat->funcion)(valIntermedio);
			}
		}
		_animandoProps = (this->conteoAnimacionesEjecutando()!=0);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUAnimadorContenedor)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUAnimadorContenedor, "AUAnimadorContenedor")
AUOBJMETODOS_CLONAR_NULL(AUAnimadorContenedor)



