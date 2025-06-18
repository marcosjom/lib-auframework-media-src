//
//  AUAnimadorSonido.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 09/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUAnimadorSonido.h"

#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO

AUAnimadorSonido::AUAnimadorSonido() : AUObjeto(), NBAnimador(), _animFloatAnim(this), _animBoolAnim(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorSonido::AUAnimadorSonido")
	//
	_animandoProps	= false;
	//
	this->reanudarAnimacion();
	NBGestorAnimadores::agregarAnimador(this, this);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUAnimadorSonido::~AUAnimadorSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorSonido::~AUAnimadorSonido")
	NBGestorAnimadores::quitarAnimador(this);
	//
	UI32 iAnim;
	for(iAnim=0; iAnim<_animFloatAnim.conteo; iAnim++)		_animFloatAnim.elemento[iAnim].objAnimando->liberar(NB_RETENEDOR_THIS);
	for(iAnim=0; iAnim<_animBoolAnim.conteo; iAnim++)		_animBoolAnim.elemento[iAnim].objAnimando->liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

SI32 AUAnimadorSonido::conteoAnimacionesEjecutando(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorSonido::conteoAnimacionesEjecutando")
	SI32 conteoAnim = _animFloatAnim.conteo + _animBoolAnim.conteo;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return conteoAnim;
}

void AUAnimadorSonido::animarFloat(AUEscenaSnd* objAnimando, float valDestino, float segundosAnimacion, PTRfuncObtenerFloatSnd funcionObtenerValor, PTRfuncEstablecerFloatSnd funcionEstablecerValor, ENAnimPropVelocidad velocidad){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorSonido::animarFloat")
	NBASSERT(segundosAnimacion>=0.0f);
	if(segundosAnimacion==0.0f){
		(objAnimando->*funcionEstablecerValor)(valDestino);
	} else {
		STAnimPropFloatSnd datosAnim;
		datosAnim.objAnimando	= objAnimando; objAnimando->retener(NB_RETENEDOR_THIS);
		datosAnim.segsAnimar	= segundosAnimacion;
		datosAnim.segsAnimando	= 0.0f;
		datosAnim.velocidadAnim	= velocidad;
		//
		datosAnim.valorInicial	= (objAnimando->*funcionObtenerValor)();
		datosAnim.valorDestino	= valDestino;
		datosAnim.funcion		= funcionEstablecerValor;
		_animFloatAnim.agregarElemento(datosAnim);
		_animandoProps			= true; //PRINTF_INFO("Animando float desde %f hacia %f en %f segundos\n", datosAnim.valorInicial, datosAnim.valorDestino, datosAnim.segsAnimar);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorSonido::animarEstado(AUEscenaSnd* objAnimando, bool estadoDestinoEsActivo, float segundosAnimacion, PTRfuncObtenerEstadoSnd funcionObtenerEstado, PTRfuncEstablecerEstadoSnd funcionEstablecerEstadoActivo, PTRfuncEstablecerEstadoSnd funcionEstablecerEstadoDestino, PTRfuncObtenerFloatSnd funcionObtenerValorFloat, PTRfuncEstablecerFloatSnd funcionEstablecerValorFloat, ENAnimPropVelocidad velocidad){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorSonido::animarBool")
	NBASSERT(segundosAnimacion>=0.0f);
	if((objAnimando->*funcionObtenerEstado)()!=estadoDestinoEsActivo){
		if(segundosAnimacion==0.0f){
			(objAnimando->*funcionEstablecerEstadoDestino)();
		} else {
			STAnimPropBoolSnd datosAnim;
			datosAnim.objAnimando		= objAnimando; objAnimando->retener(NB_RETENEDOR_THIS);
			datosAnim.segsAnimar		= segundosAnimacion;
			datosAnim.segsAnimando		= 0.0f;
			datosAnim.velocidadAnim		= velocidad;
			//
			float valorNormalF			= (objAnimando->*funcionObtenerValorFloat)();
			//
			datosAnim.valorInicialF		= estadoDestinoEsActivo?0.0f:valorNormalF;
			datosAnim.valorDestinoF		= estadoDestinoEsActivo?valorNormalF:0.0f;
			(objAnimando->*funcionEstablecerValorFloat)(datosAnim.valorInicialF);
			(objAnimando->*funcionEstablecerEstadoActivo)();
			datosAnim.funcionEstadoFinal = funcionEstablecerEstadoDestino;
			datosAnim.funcionFloat		= funcionEstablecerValorFloat;
			_animBoolAnim.agregarElemento(datosAnim);
			_animandoProps				= true;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

void AUAnimadorSonido::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorSonido::tickAnimacion")
	if(_animandoProps){
		SI32 iAnim;
		//Animar Floats
		for(iAnim=_animFloatAnim.conteo-1; iAnim>=0; iAnim--){
			float segsAnimar	= _animFloatAnim.elemento[iAnim].segsAnimar;
			float segsAnimando	= _animFloatAnim.elemento[iAnim].segsAnimando;
			float valInicial	= _animFloatAnim.elemento[iAnim].valorInicial;
			float valDestino	= _animFloatAnim.elemento[iAnim].valorDestino;
			//
			segsAnimando		+= segsTranscurridos; _animFloatAnim.elemento[iAnim].segsAnimando = segsAnimando;
			if(segsAnimando>=segsAnimar){
				(_animFloatAnim.elemento[iAnim].objAnimando->*_animFloatAnim.elemento[iAnim].funcion)(valDestino);
				_animFloatAnim.elemento[iAnim].objAnimando->liberar(NB_RETENEDOR_THIS);
				_animFloatAnim.quitarElementoEnIndice(iAnim);
			} else {
				float avance	= (segsAnimando/segsAnimar);
				switch (_animFloatAnim.elemento[iAnim].velocidadAnim){
					case ENAnimPropVelocidad_Acelerada:		avance	= (avance*avance); break;
					case ENAnimPropVelocidad_Desacelerada:	avance	= pow(avance, 1.0f/2.0f); break;
					case ENAnimPropVelocidad_Acelerada3:	avance	= (avance*avance*avance); break;
					case ENAnimPropVelocidad_Desacelerada3: avance	= pow(avance, 1.0f/3.0f); break;
					default: break;
				}
				float valIntermedio = valInicial+((valDestino-valInicial)*avance);
				(_animFloatAnim.elemento[iAnim].objAnimando->*_animFloatAnim.elemento[iAnim].funcion)(valIntermedio);
			}
		}
		//Animar Bools
		for(iAnim=_animBoolAnim.conteo-1; iAnim>=0; iAnim--){
			float segsAnimar	= _animBoolAnim.elemento[iAnim].segsAnimar;
			float segsAnimando	= _animBoolAnim.elemento[iAnim].segsAnimando;
			float valInicial	= _animBoolAnim.elemento[iAnim].valorInicialF;
			float valDestino	= _animBoolAnim.elemento[iAnim].valorDestinoF;
			//
			segsAnimando		+= segsTranscurridos; _animBoolAnim.elemento[iAnim].segsAnimando = segsAnimando;
			if(segsAnimando>=segsAnimar){
				(_animBoolAnim.elemento[iAnim].objAnimando->*_animBoolAnim.elemento[iAnim].funcionFloat)(valDestino);
				(_animBoolAnim.elemento[iAnim].objAnimando->*_animBoolAnim.elemento[iAnim].funcionEstadoFinal)();
				_animBoolAnim.elemento[iAnim].objAnimando->liberar(NB_RETENEDOR_THIS);
				_animBoolAnim.quitarElementoEnIndice(iAnim);
			} else {
				float avance	= (segsAnimando/segsAnimar);
				switch (_animBoolAnim.elemento[iAnim].velocidadAnim){
					case ENAnimPropVelocidad_Acelerada:		avance	= (avance*avance); break;
					case ENAnimPropVelocidad_Desacelerada:	avance	= pow(avance, 1.0f/2.0f); break;
					case ENAnimPropVelocidad_Acelerada3:	avance	= (avance*avance*avance); break;
					case ENAnimPropVelocidad_Desacelerada3: avance	= pow(avance, 1.0f/3.0f); break;
					default: break;
				}
				float valIntermedio;
				valIntermedio	= valInicial+((float)(valDestino-valInicial)*avance);
				(_animBoolAnim.elemento[iAnim].objAnimando->*_animBoolAnim.elemento[iAnim].funcionFloat)(valIntermedio);
			}
		}
		_animandoProps = (this->conteoAnimacionesEjecutando()!=0);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUAnimadorSonido)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUAnimadorSonido, "AUAnimadorSonido")
AUOBJMETODOS_CLONAR_NULL(AUAnimadorSonido)


#endif //#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO


