//
//  AUAnimadorObjetoEscena.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 09/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUAnimadorObjetoEscena.h"

AUAnimadorObjetoEscena::AUAnimadorObjetoEscena() : NBAnimador(), _animPunto(this), _animColor(this), _animTamano(this), _animFloat(this), _animBool(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::AUAnimadorObjetoEscena")
	//
	_animandoProps	= false;
	//
	this->reanudarAnimacion();
	NBGestorAnimadores::agregarAnimador(this, this);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUAnimadorObjetoEscena::~AUAnimadorObjetoEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::~AUAnimadorObjetoEscena")
	NBGestorAnimadores::quitarAnimador(this);
	this->purgarAnimaciones();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

SI32 AUAnimadorObjetoEscena::conteoAnimacionesEjecutando(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::conteoAnimacionesEjecutando")
	SI32 conteoAnim = _animPunto.conteo + _animColor.conteo + _animTamano.conteo + _animFloat.conteo + _animBool.conteo;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return conteoAnim;
}

bool AUAnimadorObjetoEscena::animandoObjeto(AUEscenaObjeto* objAnimando){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animandoObjeto")
	bool encontrado = false;
	if(_animandoProps){
		UI32 iAnim;
		for(iAnim=0; iAnim<_animPunto.conteo && !encontrado; iAnim++)	encontrado = (_animPunto.elemento[iAnim].objAnimando == objAnimando);
		for(iAnim=0; iAnim<_animColor.conteo && !encontrado; iAnim++)	encontrado = (_animColor.elemento[iAnim].objAnimando == objAnimando);
		for(iAnim=0; iAnim<_animTamano.conteo && !encontrado; iAnim++)	encontrado = (_animTamano.elemento[iAnim].objAnimando == objAnimando);
		for(iAnim=0; iAnim<_animFloat.conteo && !encontrado; iAnim++)	encontrado = (_animFloat.elemento[iAnim].objAnimando == objAnimando);
		for(iAnim=0; iAnim<_animBool.conteo && !encontrado; iAnim++)	encontrado = (_animBool.elemento[iAnim].objAnimando == objAnimando);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return encontrado;
}

// Metodos directos

void AUAnimadorObjetoEscena::animarPosicion(AUEscenaObjeto* objAnimando, const NBPunto valDestino, const float segundosAnimacion, const ENAnimPropVelocidad velocidad, const float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarPosicion")
	this->animarPunto(objAnimando, valDestino.x, valDestino.y, segundosAnimacion, &AUEscenaObjeto::traslacion, &AUEscenaObjeto::establecerTraslacion, velocidad, segsEspera);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorObjetoEscena::animarPosicion(AUEscenaObjeto* objAnimando, const float xDestino, const float yDestino, const float segundosAnimacion, const ENAnimPropVelocidad velocidad, const float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarPosicion")
	this->animarPunto(objAnimando, xDestino, yDestino, segundosAnimacion, &AUEscenaObjeto::traslacion, &AUEscenaObjeto::establecerTraslacion, velocidad, segsEspera);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorObjetoEscena::animarColorMult(AUEscenaObjeto* objAnimando, const NBColor8 valDestino, const float segundosAnimacion, const ENAnimPropVelocidad velocidad, const float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarColorMult")
	this->animarColor(objAnimando, valDestino.r, valDestino.g, valDestino.b, valDestino.a, segundosAnimacion, &AUEscenaObjeto::multiplicadorColor8Func, &AUEscenaObjeto::establecerMultiplicadorColor8, velocidad, segsEspera);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorObjetoEscena::animarColorMult(AUEscenaObjeto* objAnimando, const UI8 rDestino, const UI8 gDestino, const UI8 bDestino, const UI8 aDestino, const float segundosAnimacion, const ENAnimPropVelocidad velocidad, const float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarColorMult")
	this->animarColor(objAnimando, rDestino, gDestino, bDestino, aDestino, segundosAnimacion, &AUEscenaObjeto::multiplicadorColor8Func, &AUEscenaObjeto::establecerMultiplicadorColor8, velocidad, segsEspera);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorObjetoEscena::animarEscalacion(AUEscenaObjeto* objAnimando, const NBTamano valDestino, float segundosAnimacion, const ENAnimPropVelocidad velocidad, const float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarEscalacion")
	this->animarTamano(objAnimando, valDestino.ancho, valDestino.alto, segundosAnimacion, &AUEscenaObjeto::escalacion, &AUEscenaObjeto::establecerEscalacion, velocidad, segsEspera);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorObjetoEscena::animarEscalacion(AUEscenaObjeto* objAnimando, const float anchoDestino, const float altoDestino, const float segundosAnimacion, const ENAnimPropVelocidad velocidad, const float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarEscalacion")
	this->animarTamano(objAnimando, anchoDestino, altoDestino, segundosAnimacion, &AUEscenaObjeto::escalacion, &AUEscenaObjeto::establecerEscalacion, velocidad, segsEspera);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorObjetoEscena::animarRotacion(AUEscenaObjeto* objAnimando, const float valDestino, const float segundosAnimacion, const ENAnimPropVelocidad velocidad, const float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarRotacion")
	this->animarFloat(objAnimando, valDestino, segundosAnimacion, &AUEscenaObjeto::rotacion, &AUEscenaObjeto::establecerRotacion, velocidad, segsEspera);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorObjetoEscena::animarVisible(AUEscenaObjeto* objAnimando, const bool valDestino, const float segundosAnimacion, const ENAnimPropVelocidad velocidad, const float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarVisible")
	this->animarBool(objAnimando, valDestino, segundosAnimacion, &AUEscenaObjeto::visible, &AUEscenaObjeto::establecerVisible, &AUEscenaObjeto::multiplicadorAlpha8, &AUEscenaObjeto::establecerMultiplicadorAlpha8, velocidad, segsEspera);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

// Metodos genericos

/*void AUAnimadorObjetoEscena::animarPunto(AUEscenaObjeto* objAnimando, NBPunto valDestino, float segundosAnimacion, PTRfuncObtenerPunto funcionObtenerValor, PTRfuncEstablecerPunto funcionEstablecerValor, ENAnimPropVelocidad velocidad, float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarPunto")
	NBASSERT(segundosAnimacion >= 0.0f);
	if(segsEspera <= 0.0f && segundosAnimacion <= 0.0f){
		(objAnimando->*funcionEstablecerValor)(valDestino);
		objAnimando->establecerDormido(FALSE);
	} else {
		STAnimPropPunto datosAnim;
		datosAnim.objAnimando	= objAnimando; objAnimando->retener(NB_RETENEDOR_THIS);
		datosAnim.segsEspera	= segsEspera;
		datosAnim.segsAnimar	= segundosAnimacion;
		datosAnim.segsAnimando	= 0.0f;
		datosAnim.velocidadAnim	= velocidad;
		//
		datosAnim.valorInicial	= (objAnimando->*funcionObtenerValor)();
		datosAnim.valorDestino	= valDestino;
		datosAnim.funcion		= funcionEstablecerValor;
		if(segsEspera <= 0.0f){
			objAnimando->establecerDormido(FALSE);
		}
		_animPunto.agregarElemento(datosAnim);
		_animandoProps			= true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}*/

void AUAnimadorObjetoEscena::animarPunto(AUEscenaObjeto* objAnimando, float xDestino, float yDestino, float segundosAnimacion, PTRfuncObtenerPunto funcionObtenerValor, PTRfuncEstablecerPunto funcionEstablecerValor, ENAnimPropVelocidad velocidad, float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarPunto")
	NBASSERT(segundosAnimacion >= 0.0f);
	if(segsEspera <= 0.0f && segundosAnimacion <= 0.0f){
		NBPunto valDestino; NBPUNTO_ESTABLECER(valDestino, xDestino, yDestino)
		(objAnimando->*funcionEstablecerValor)(valDestino);
		objAnimando->establecerDormido(FALSE);
	} else {
		STAnimPropPunto datosAnim;
		datosAnim.objAnimando	= objAnimando; objAnimando->retener(NB_RETENEDOR_THIS);
		datosAnim.segsEspera	= segsEspera;
		datosAnim.segsAnimar	= segundosAnimacion;
		datosAnim.segsAnimando	= 0.0f;
		datosAnim.velocidadAnim	= velocidad;
		//
		if(segsEspera <= 0.0f){
			datosAnim.valorInicial = (objAnimando->*funcionObtenerValor)();
			objAnimando->establecerDormido(FALSE);
		}
		datosAnim.funcionObtenerValor = funcionObtenerValor;
		datosAnim.valorDestino.x = xDestino; NBASSERT(xDestino == xDestino) //Not NaN
		datosAnim.valorDestino.y = yDestino; NBASSERT(yDestino == yDestino) //Not NaN
		datosAnim.funcion		= funcionEstablecerValor;
		_animPunto.agregarElemento(datosAnim);
		_animandoProps			= true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

/*void AUAnimadorObjetoEscena::animarColor(AUEscenaObjeto* objAnimando, NBColor8 valDestino, float segundosAnimacion, PTRfuncObtenerColor funcionObtenerValor, PTRfuncEstablecerColor funcionEstablecerValor, ENAnimPropVelocidad velocidad, float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarColor")
	NBASSERT(segundosAnimacion >= 0.0f);
	if(segsEspera <= 0.0f && segundosAnimacion <= 0.0f){
		(objAnimando->*funcionEstablecerValor)(valDestino);
		objAnimando->establecerDormido(FALSE);
	} else {
		STAnimPropColor datosAnim;
		datosAnim.objAnimando	= objAnimando; objAnimando->retener(NB_RETENEDOR_THIS);
		datosAnim.segsEspera	= segsEspera;
		datosAnim.segsAnimar	= segundosAnimacion;
		datosAnim.segsAnimando	= 0.0f;
		datosAnim.velocidadAnim	= velocidad;
		//
		datosAnim.valorInicial	= (objAnimando->*funcionObtenerValor)();
		datosAnim.valorDestino	= valDestino;
		datosAnim.funcion		= funcionEstablecerValor;
		if(segsEspera <= 0.0f){
			objAnimando->establecerDormido(FALSE);
		}
		_animColor.agregarElemento(datosAnim);
		_animandoProps			= true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}*/

void AUAnimadorObjetoEscena::animarColor(AUEscenaObjeto* objAnimando, UI8 rDestino, UI8 gDestino, UI8 bDestino, UI8 aDestino, float segundosAnimacion, PTRfuncObtenerColor funcionObtenerValor, PTRfuncEstablecerColor funcionEstablecerValor, ENAnimPropVelocidad velocidad, float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarColor")
	NBASSERT(segundosAnimacion >= 0.0f);
	if(segsEspera <= 0.0f && segundosAnimacion <= 0.0f){
		NBColor8 valDestino; NBCOLOR_ESTABLECER(valDestino, rDestino, gDestino, bDestino, aDestino)
		(objAnimando->*funcionEstablecerValor)(valDestino);
		objAnimando->establecerDormido(FALSE);
	} else {
		STAnimPropColor datosAnim;
		datosAnim.objAnimando	= objAnimando; objAnimando->retener(NB_RETENEDOR_THIS);
		datosAnim.segsEspera	= segsEspera;
		datosAnim.segsAnimar	= segundosAnimacion;
		datosAnim.segsAnimando	= 0.0f;
		datosAnim.velocidadAnim	= velocidad;
		//
		if(segsEspera <= 0.0f){
			datosAnim.valorInicial = (objAnimando->*funcionObtenerValor)();
			objAnimando->establecerDormido(FALSE);
		}
		datosAnim.funcionObtenerValor = funcionObtenerValor;
		datosAnim.valorDestino.r	= rDestino; NBASSERT(rDestino == rDestino) //Not NaN
		datosAnim.valorDestino.g	= gDestino; NBASSERT(gDestino == gDestino) //Not NaN
		datosAnim.valorDestino.b	= bDestino; NBASSERT(bDestino == bDestino) //Not NaN
		datosAnim.valorDestino.a	= aDestino; NBASSERT(aDestino == aDestino) //Not NaN
		datosAnim.funcion		= funcionEstablecerValor;
		_animColor.agregarElemento(datosAnim);
		_animandoProps			= true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}


/*void AUAnimadorObjetoEscena::animarTamano(AUEscenaObjeto* objAnimando, NBTamano valDestino, float segundosAnimacion, PTRfuncObtenerTamano funcionObtenerValor, PTRfuncEstablecerTamano funcionEstablecerValor, ENAnimPropVelocidad velocidad, float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarTamano")
	NBASSERT(segundosAnimacion >= 0.0f);
	if(segsEspera <= 0.0f && segundosAnimacion <= 0.0f){
		(objAnimando->*funcionEstablecerValor)(valDestino);
		objAnimando->establecerDormido(FALSE);
	} else {
		STAnimPropTamano datosAnim;
		datosAnim.objAnimando	= objAnimando; objAnimando->retener(NB_RETENEDOR_THIS);
		datosAnim.segsEspera	= segsEspera;
		datosAnim.segsAnimar	= segundosAnimacion;
		datosAnim.segsAnimando	= 0.0f;
		datosAnim.velocidadAnim	= velocidad;
		//
		datosAnim.valorInicial	= (objAnimando->*funcionObtenerValor)();
		datosAnim.valorDestino	= valDestino;
		datosAnim.funcion		= funcionEstablecerValor;
		_animTamano.agregarElemento(datosAnim);
		_animandoProps			= true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}*/

void AUAnimadorObjetoEscena::animarTamano(AUEscenaObjeto* objAnimando, float anchoDestino, float altoDestino, float segundosAnimacion, PTRfuncObtenerTamano funcionObtenerValor, PTRfuncEstablecerTamano funcionEstablecerValor, ENAnimPropVelocidad velocidad, float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarTamano")
	NBASSERT(segundosAnimacion >= 0.0f);
	if(segsEspera <= 0.0f && segundosAnimacion <= 0.0f){
		NBTamano valDestino; NBTAMANO_ESTABLECER(valDestino, anchoDestino, altoDestino)
		(objAnimando->*funcionEstablecerValor)(valDestino);
		objAnimando->establecerDormido(FALSE);
	} else {
		STAnimPropTamano datosAnim;
		datosAnim.objAnimando	= objAnimando; objAnimando->retener(NB_RETENEDOR_THIS);
		datosAnim.segsEspera	= segsEspera;
		datosAnim.segsAnimar	= segundosAnimacion;
		datosAnim.segsAnimando	= 0.0f;
		datosAnim.velocidadAnim	= velocidad;
		//
		if(segsEspera <= 0.0f){
			datosAnim.valorInicial = (objAnimando->*funcionObtenerValor)();
			objAnimando->establecerDormido(FALSE);
		}
		datosAnim.funcionObtenerValor = funcionObtenerValor;
		datosAnim.valorDestino.ancho = anchoDestino; NBASSERT(anchoDestino == anchoDestino) //Not NaN
		datosAnim.valorDestino.alto	 = altoDestino; NBASSERT(altoDestino == altoDestino) //Not NaN
		datosAnim.funcion		= funcionEstablecerValor;
		_animTamano.agregarElemento(datosAnim);
		_animandoProps			= true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorObjetoEscena::animarFloat(AUEscenaObjeto* objAnimando, float valDestino, float segundosAnimacion, PTRfuncObtenerFloat funcionObtenerValor, PTRfuncEstablecerFloat funcionEstablecerValor, ENAnimPropVelocidad velocidad, float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarFloat")
	NBASSERT(segundosAnimacion >= 0.0f);
	if(segsEspera <= 0.0f && segundosAnimacion <= 0.0f){
		(objAnimando->*funcionEstablecerValor)(valDestino);
		objAnimando->establecerDormido(FALSE);
	} else {
		STAnimPropFloat datosAnim;
		datosAnim.objAnimando	= objAnimando; objAnimando->retener(NB_RETENEDOR_THIS);
		datosAnim.segsEspera	= segsEspera;
		datosAnim.segsAnimar	= segundosAnimacion;
		datosAnim.segsAnimando	= 0.0f;
		datosAnim.velocidadAnim	= velocidad;
		//
		if(segsEspera <= 0.0f){
			datosAnim.valorInicial = (objAnimando->*funcionObtenerValor)();
			objAnimando->establecerDormido(FALSE);
		}
		datosAnim.funcionObtenerValor = funcionObtenerValor;
		datosAnim.valorDestino	= valDestino; NBASSERT(valDestino == valDestino) //Not NaN
		datosAnim.funcion		= funcionEstablecerValor;
		_animFloat.agregarElemento(datosAnim);
		_animandoProps			= true; //PRINTF_INFO("Animando float desde %f hacia %f en %f segundos\n", datosAnim.valorInicial, datosAnim.valorDestino, datosAnim.segsAnimar);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorObjetoEscena::animarBool(AUEscenaObjeto* objAnimando, bool valDestino, float segundosAnimacion, PTRfuncObtenerBool funcionObtenerValorBool, PTRfuncEstablecerBool funcionEstablecerValorBool, PTRfuncObtenerUI8 funcionObtenerValorUI8, PTRfuncEstablecerUI8 funcionEstablecerValorUI8, ENAnimPropVelocidad velocidad, float segsEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::animarBool")
	NBASSERT(segundosAnimacion >= 0.0f);
	if(segsEspera <= 0.0f && segundosAnimacion <= 0.0f){
		(objAnimando->*funcionEstablecerValorBool)(valDestino);
		(objAnimando->*funcionEstablecerValorUI8)(255); //Asegurar que el estado final sea consistente como si se hubiese ejecutado la animacion.
		objAnimando->establecerDormido(FALSE);
	} else {
		STAnimPropBool datosAnim;
		bool agregarAnim = true;
		const bool valBoolActual = (objAnimando->*funcionObtenerValorBool)();
		if(segsEspera <= 0.0f){
			if(valDestino == valBoolActual){
				agregarAnim = false;
			} else {
				if(valDestino){
					//De-invisible-hacia-visible
					datosAnim.valorInicialUI8	= 0;
					datosAnim.valorDestinoUI8	= 255;
					(objAnimando->*funcionEstablecerValorUI8)(0);		//Estado inicial: visible-pero-transparente.
					(objAnimando->*funcionEstablecerValorBool)(true);	//Estado inicial: visible-pero-transparente.
				} else {
					//De-visible-hacia-invisible
					datosAnim.valorInicialUI8	= (objAnimando->*funcionObtenerValorUI8)();
					datosAnim.valorDestinoUI8	= 0;
				}
			}
			objAnimando->establecerDormido(FALSE);
		}
		if(!agregarAnim){
			(objAnimando->*funcionEstablecerValorUI8)(255);		//Asegurar que el estado final sea consistente como si se hubiese ejecutado la animacion.
		} else {
			datosAnim.objAnimando		= objAnimando; objAnimando->retener(NB_RETENEDOR_THIS);
			datosAnim.segsEspera		= segsEspera;
			datosAnim.segsAnimar		= segundosAnimacion;
			datosAnim.segsAnimando		= 0.0f;
			datosAnim.velocidadAnim		= velocidad;
			//
			datosAnim.valorInicialB		= valBoolActual;
			datosAnim.valorDestinoB		= valDestino;
			//
			datosAnim.funcionObtenerBool = funcionObtenerValorBool;
			datosAnim.funcionBool		= funcionEstablecerValorBool;
			datosAnim.funcionObtenerUI8	= funcionObtenerValorUI8;
			datosAnim.funcionUI8		= funcionEstablecerValorUI8;
			_animBool.agregarElemento(datosAnim);
			_animandoProps				= true;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorObjetoEscena::quitarAnimaciones(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::quitarAnimaciones")
	SI32 iAnim;
	//Animar puntos
	for(iAnim=_animPunto.conteo-1; iAnim>=0; iAnim--){
		STAnimPropPunto* animDat = &_animPunto.elemento[iAnim];
		animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
	}
	_animPunto.vaciar();
	//Animar colores
	for(iAnim=_animColor.conteo-1; iAnim>=0; iAnim--){
		STAnimPropColor* animDat = &_animColor.elemento[iAnim];
		animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
	}
	_animColor.vaciar();
	//Animar Tamanos
	for(iAnim=_animTamano.conteo-1; iAnim>=0; iAnim--){
		STAnimPropTamano* animDat = &_animTamano.elemento[iAnim];
		animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
	}
	_animTamano.vaciar();
	//Animar Floats
	for(iAnim=_animFloat.conteo-1; iAnim>=0; iAnim--){
		STAnimPropFloat* animDat = &_animFloat.elemento[iAnim];
		animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
	}
	_animFloat.vaciar();
	//Animar Bools
	for(iAnim=_animBool.conteo-1; iAnim>=0; iAnim--){
		STAnimPropBool* animDat = &_animBool.elemento[iAnim];
		animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
	}
	_animBool.vaciar();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorObjetoEscena::quitarAnimacionesDeObjeto(AUEscenaObjeto* objAnimando){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::quitarAnimacionesDeObjeto")
	SI32 iAnim;
	//Animar puntos
	for(iAnim=_animPunto.conteo-1; iAnim>=0; iAnim--){
		STAnimPropPunto* animDat = &_animPunto.elemento[iAnim];
		if(animDat->objAnimando == objAnimando){
			animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
			_animPunto.quitarElementoEnIndice(iAnim);
		}
	}
	//Animar colores
	for(iAnim=_animColor.conteo-1; iAnim>=0; iAnim--){
		STAnimPropColor* animDat = &_animColor.elemento[iAnim];
		if(animDat->objAnimando == objAnimando){
			animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
			_animColor.quitarElementoEnIndice(iAnim);
		}
	}
	//Animar Tamanos
	for(iAnim=_animTamano.conteo-1; iAnim>=0; iAnim--){
		STAnimPropTamano* animDat = &_animTamano.elemento[iAnim];
		if(animDat->objAnimando == objAnimando){
			animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
			_animTamano.quitarElementoEnIndice(iAnim);
		}
	}
	//Animar Floats
	for(iAnim=_animFloat.conteo-1; iAnim>=0; iAnim--){
		STAnimPropFloat* animDat = &_animFloat.elemento[iAnim];
		if(animDat->objAnimando == objAnimando){
			animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
			_animFloat.quitarElementoEnIndice(iAnim);
		}
	}
	//Animar Bools
	for(iAnim=_animBool.conteo-1; iAnim>=0; iAnim--){
		STAnimPropBool* animDat = &_animBool.elemento[iAnim];
		if(animDat->objAnimando == objAnimando){
			animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
			_animBool.quitarElementoEnIndice(iAnim);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorObjetoEscena::purgarAnimaciones(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::purgarAnimaciones")
	this->privPurgarAnimaciones(NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorObjetoEscena::purgarAnimacionesDeObjeto(AUEscenaObjeto* objAnimando){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::purgarAnimacionesDeObjeto")
	this->privPurgarAnimaciones(objAnimando);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAnimadorObjetoEscena::privPurgarAnimaciones(AUEscenaObjeto* objEnBusqueda){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::privPurgarAnimaciones")
	SI32 iAnim;
	//Animar puntos
	while(_animPunto.conteo != 0){
		//Establecer inicial
		SI32 iAnimFinal = 0;
		if(objEnBusqueda != NULL){
			//Buscar la primera aparicion del objeto
			iAnimFinal = -1;
			for(iAnim = 0; iAnim < _animPunto.conteo; iAnim++){
				STAnimPropPunto* animDat2 = &_animPunto.elemento[iAnim];
				if(animDat2->objAnimando == objEnBusqueda){
					iAnimFinal = iAnim;
					break;
				}
			}
			if(iAnimFinal == -1){
				break; //El objeto no existe
			}
		}
		STAnimPropPunto* animData = &_animPunto.elemento[iAnimFinal];
		AUEscenaObjeto* objAnimando = animData->objAnimando;
		float segFinAnim = animData->segsEspera + (animData->segsAnimar - animData->segsAnimando);
		//Determinar el estado final
		for(iAnim = iAnimFinal + 1; iAnim < _animPunto.conteo; iAnim++){
			STAnimPropPunto* animDat2 = &_animPunto.elemento[iAnim];
			if(animDat2->objAnimando == objAnimando){
				const float segFinAnim2 = animDat2->segsEspera + (animDat2->segsAnimar - animDat2->segsAnimando);
				if(segFinAnim <= segFinAnim2){
					//Quitar la anterior
					_animPunto.elemento[iAnimFinal].objAnimando->liberar(NB_RETENEDOR_THIS);
					_animPunto.quitarElementoEnIndice(iAnimFinal);
					iAnimFinal = iAnim - 1;
					segFinAnim = segFinAnim2;
				} else {
					//Quitar esta
					animDat2->objAnimando->liberar(NB_RETENEDOR_THIS);
					_animPunto.quitarElementoEnIndice(iAnim);
				}
				iAnim--;
			}
		}
		//Aplicar el estado final
		NBASSERT(iAnimFinal >= 0 && iAnimFinal < _animPunto.conteo)
		{
			STAnimPropPunto* animDat = &_animPunto.elemento[iAnimFinal];
			(animDat->objAnimando->*animDat->funcion)(animDat->valorDestino);
			if(animDat->segsEspera > 0.0f){
				animDat->objAnimando->establecerDormido(FALSE);
			}
			animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
			_animPunto.quitarElementoEnIndice(iAnimFinal);
		}
		if(objEnBusqueda != NULL){
			break; //Busqueda puntual de objeto, terminar ciclo
		}
	} NBASSERT(objEnBusqueda != NULL || _animPunto.conteo == 0)
	//Animar colores
	while(_animColor.conteo != 0){
		//Establecer inicial
		SI32 iAnimFinal = 0;
		if(objEnBusqueda != NULL){
			//Buscar la primera aparicion del objeto
			iAnimFinal = -1;
			for(iAnim = 0; iAnim < _animColor.conteo; iAnim++){
				STAnimPropColor* animDat2 = &_animColor.elemento[iAnim];
				if(animDat2->objAnimando == objEnBusqueda){
					iAnimFinal = iAnim;
					break;
				}
			}
			if(iAnimFinal == -1){
				break; //El objeto no existe
			}
		}
		STAnimPropColor* animData = &_animColor.elemento[iAnimFinal];
		AUEscenaObjeto* objAnimando = animData->objAnimando;
		float segFinAnim = animData->segsEspera + (animData->segsAnimar - animData->segsAnimando);
		//Determinar el estado final
		for(iAnim = iAnimFinal + 1; iAnim < _animColor.conteo; iAnim++){
			STAnimPropColor* animDat2 = &_animColor.elemento[iAnim];
			if(animDat2->objAnimando == objAnimando){
				const float segFinAnim2 = animDat2->segsEspera + (animDat2->segsAnimar - animDat2->segsAnimando);
				if(segFinAnim <= segFinAnim2){
					//Quitar la anterior
					_animColor.elemento[iAnimFinal].objAnimando->liberar(NB_RETENEDOR_THIS);
					_animColor.quitarElementoEnIndice(iAnimFinal);
					iAnimFinal = iAnim - 1;
					segFinAnim = segFinAnim2;
				} else {
					//Quitar esta
					animDat2->objAnimando->liberar(NB_RETENEDOR_THIS);
					_animColor.quitarElementoEnIndice(iAnim);
				}
				iAnim--;
			}
		}
		//Aplicar el estado final
		NBASSERT(iAnimFinal >= 0 && iAnimFinal < _animColor.conteo)
		{
			STAnimPropColor* animDat = &_animColor.elemento[iAnimFinal];
			(animDat->objAnimando->*animDat->funcion)(animDat->valorDestino);
			if(animDat->segsEspera > 0.0f){
				animDat->objAnimando->establecerDormido(FALSE);
			}
			animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
			_animColor.quitarElementoEnIndice(iAnimFinal);
		}
		if(objEnBusqueda != NULL){
			break; //Busqueda puntual de objeto, terminar ciclo
		}
	} NBASSERT(objEnBusqueda != NULL || _animColor.conteo == 0)
	//Animar Tamanos
	while(_animTamano.conteo != 0){
		//Establecer inicial
		SI32 iAnimFinal = 0;
		if(objEnBusqueda != NULL){
			//Buscar la primera aparicion del objeto
			iAnimFinal = -1;
			for(iAnim = 0; iAnim < _animTamano.conteo; iAnim++){
				STAnimPropTamano* animDat2 = &_animTamano.elemento[iAnim];
				if(animDat2->objAnimando == objEnBusqueda){
					iAnimFinal = iAnim;
					break;
				}
			}
			if(iAnimFinal == -1){
				break; //El objeto no existe
			}
		}
		STAnimPropTamano* animData = &_animTamano.elemento[iAnimFinal];
		AUEscenaObjeto* objAnimando = animData->objAnimando;
		float segFinAnim = animData->segsEspera + (animData->segsAnimar - animData->segsAnimando);
		//Determinar el estado final
		for(iAnim = iAnimFinal + 1; iAnim < _animTamano.conteo; iAnim++){
			STAnimPropTamano* animDat2 = &_animTamano.elemento[iAnim];
			if(animDat2->objAnimando == objAnimando){
				const float segFinAnim2 = animDat2->segsEspera + (animDat2->segsAnimar - animDat2->segsAnimando);
				if(segFinAnim <= segFinAnim2){
					//Quitar la anterior
					_animTamano.elemento[iAnimFinal].objAnimando->liberar(NB_RETENEDOR_THIS);
					_animTamano.quitarElementoEnIndice(iAnimFinal);
					iAnimFinal = iAnim - 1;
					segFinAnim = segFinAnim2;
				} else {
					//Quitar esta
					animDat2->objAnimando->liberar(NB_RETENEDOR_THIS);
					_animTamano.quitarElementoEnIndice(iAnim);
				}
				iAnim--;
			}
		}
		//Aplicar el estado final
		NBASSERT(iAnimFinal >= 0 && iAnimFinal < _animTamano.conteo)
		{
			STAnimPropTamano* animDat = &_animTamano.elemento[iAnimFinal];
			(animDat->objAnimando->*animDat->funcion)(animDat->valorDestino);
			if(animDat->segsEspera > 0.0f){
				animDat->objAnimando->establecerDormido(FALSE);
			}
			animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
			_animTamano.quitarElementoEnIndice(iAnimFinal);
		}
		if(objEnBusqueda != NULL){
			break; //Busqueda puntual de objeto, terminar ciclo
		}
	} NBASSERT(objEnBusqueda != NULL || _animTamano.conteo == 0)
	//Animar Floats
	while(_animFloat.conteo != 0){
		//Establecer inicial
		SI32 iAnimFinal = 0;
		if(objEnBusqueda != NULL){
			//Buscar la primera aparicion del objeto
			iAnimFinal = -1;
			for(iAnim = 0; iAnim < _animFloat.conteo; iAnim++){
				STAnimPropFloat* animDat2 = &_animFloat.elemento[iAnim];
				if(animDat2->objAnimando == objEnBusqueda){
					iAnimFinal = iAnim;
					break;
				}
			}
			if(iAnimFinal == -1){
				break; //El objeto no existe
			}
		}
		STAnimPropFloat* animData = &_animFloat.elemento[iAnimFinal];
		AUEscenaObjeto* objAnimando = animData->objAnimando;
		float segFinAnim = animData->segsEspera + (animData->segsAnimar - animData->segsAnimando);
		//Determinar el estado final
		for(iAnim = iAnimFinal + 1; iAnim < _animFloat.conteo; iAnim++){
			STAnimPropFloat* animDat2 = &_animFloat.elemento[iAnim];
			if(animDat2->objAnimando == objAnimando){
				const float segFinAnim2 = animDat2->segsEspera + (animDat2->segsAnimar - animDat2->segsAnimando);
				if(segFinAnim <= segFinAnim2){
					//Quitar la anterior
					_animFloat.elemento[iAnimFinal].objAnimando->liberar(NB_RETENEDOR_THIS);
					_animFloat.quitarElementoEnIndice(iAnimFinal);
					iAnimFinal = iAnim - 1;
					segFinAnim = segFinAnim2;
				} else {
					//Quitar esta
					animDat2->objAnimando->liberar(NB_RETENEDOR_THIS);
					_animFloat.quitarElementoEnIndice(iAnim);
				}
				iAnim--;
			}
		}
		//Aplicar el estado final
		NBASSERT(iAnimFinal >= 0 && iAnimFinal < _animFloat.conteo)
		{
			STAnimPropFloat* animDat = &_animFloat.elemento[iAnimFinal];
			(animDat->objAnimando->*animDat->funcion)(animDat->valorDestino);
			if(animDat->segsEspera > 0.0f){
				animDat->objAnimando->establecerDormido(FALSE);
			}
			animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
			_animFloat.quitarElementoEnIndice(iAnimFinal);
		}
		if(objEnBusqueda != NULL){
			break; //Busqueda puntual de objeto, terminar ciclo
		}
	} NBASSERT(objEnBusqueda != NULL || _animFloat.conteo == 0)
	//Animar Bools
	while(_animBool.conteo != 0){
		//Establecer inicial
		SI32 iAnimFinal = 0;
		if(objEnBusqueda != NULL){
			//Buscar la primera aparicion del objeto
			iAnimFinal = -1;
			for(iAnim = 0; iAnim < _animBool.conteo; iAnim++){
				STAnimPropBool* animDat2 = &_animBool.elemento[iAnim];
				if(animDat2->objAnimando == objEnBusqueda){
					iAnimFinal = iAnim;
					break;
				}
			}
			if(iAnimFinal == -1){
				break; //El objeto no existe
			}
		}
		STAnimPropBool* animData = &_animBool.elemento[iAnimFinal];
		AUEscenaObjeto* objAnimando = animData->objAnimando;
		float segFinAnim = animData->segsEspera + (animData->segsAnimar - animData->segsAnimando);
		//Determinar el estado final
		for(iAnim = iAnimFinal + 1; iAnim < _animBool.conteo; iAnim++){
			STAnimPropBool* animDat2 = &_animBool.elemento[iAnim];
			if(animDat2->objAnimando == objAnimando){
				const float segFinAnim2 = animDat2->segsEspera + (animDat2->segsAnimar - animDat2->segsAnimando);
				if(segFinAnim <= segFinAnim2){
					//Quitar la anterior
					_animBool.elemento[iAnimFinal].objAnimando->liberar(NB_RETENEDOR_THIS);
					_animBool.quitarElementoEnIndice(iAnimFinal);
					iAnimFinal = iAnim - 1;
					segFinAnim = segFinAnim2;
				} else {
					//Quitar esta
					animDat2->objAnimando->liberar(NB_RETENEDOR_THIS);
					_animBool.quitarElementoEnIndice(iAnim);
				}
				iAnim--;
			}
		}
		//Aplicar el estado final
		NBASSERT(iAnimFinal >= 0 && iAnimFinal < _animBool.conteo)
		{
			STAnimPropBool* animDat = &_animBool.elemento[iAnimFinal];
			(animDat->objAnimando->*animDat->funcionUI8)(255);
			(animDat->objAnimando->*animDat->funcionBool)(animDat->valorDestinoB);
			if(animDat->segsEspera > 0.0f){
				animDat->objAnimando->establecerDormido(FALSE);
			}
			animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
			_animBool.quitarElementoEnIndice(iAnimFinal);
		}
		if(objEnBusqueda != NULL){
			break; //Busqueda puntual de objeto, terminar ciclo
		}
	} NBASSERT(objEnBusqueda != NULL || _animBool.conteo == 0)
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

void AUAnimadorObjetoEscena::tickAnimacion(float pSegsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorObjetoEscena::tickAnimacion")
	NBASSERT(_animandoProps || (_animPunto.conteo == 0 && _animColor.conteo == 0 && _animTamano.conteo == 0 && _animFloat.conteo == 0 && _animBool.conteo == 0))
	if(_animandoProps){
		SI32 iAnim; float segsConsumir;
		//Animar puntos
		for(iAnim = 0; iAnim < _animPunto.conteo; iAnim++){
			STAnimPropPunto* animDat = &_animPunto.elemento[iAnim];
			segsConsumir = pSegsTranscurridos;
			if(animDat->segsEspera > 0.0f){
				if(animDat->segsEspera > segsConsumir){
					animDat->segsEspera		-= segsConsumir; NBASSERT(animDat->segsEspera > 0.0f)
					segsConsumir			= 0.0f;
				} else {
					segsConsumir			-= animDat->segsEspera; NBASSERT(segsConsumir >= 0.0f)
					animDat->segsEspera		= 0.0f;
					animDat->valorInicial	= (animDat->objAnimando->*(animDat->funcionObtenerValor))();
					animDat->objAnimando->establecerDormido(FALSE);
				}
			}
			NBASSERT(segsConsumir >= 0.0f)
			if(segsConsumir != 0.0f){
				float segsAnimar	= animDat->segsAnimar;
				float segsAnimando	= animDat->segsAnimando;
				NBPunto valInicial	= animDat->valorInicial;
				NBPunto valDestino	= animDat->valorDestino;
				//
				segsAnimando		+= segsConsumir; animDat->segsAnimando = segsAnimando;
				if(segsAnimando>=segsAnimar){
					(animDat->objAnimando->*animDat->funcion)(valDestino);
					animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
					_animPunto.quitarElementoEnIndice(iAnim--);
				} else {
					float avance	= (segsAnimando/segsAnimar);
					switch (animDat->velocidadAnim){
						case ENAnimPropVelocidad_Acelerada:		avance	= (avance * avance); break;
						case ENAnimPropVelocidad_Desacelerada:	avance	= pow(avance, 1.0f/2.0f); break;
						case ENAnimPropVelocidad_Acelerada3:	avance	= (avance*avance*avance); break;
						case ENAnimPropVelocidad_Desacelerada3: avance	= pow(avance, 1.0f/3.0f); break;
						default: break;
					}
					NBPunto valIntermedio;
					valIntermedio.x = valInicial.x+((valDestino.x-valInicial.x)*avance);
					valIntermedio.y = valInicial.y+((valDestino.y-valInicial.y)*avance);
					(animDat->objAnimando->*animDat->funcion)(valIntermedio);
				}
			}
		}
		//Animar colores
		for(iAnim = 0; iAnim < _animColor.conteo; iAnim++){
			STAnimPropColor* animDat = &_animColor.elemento[iAnim];
			segsConsumir = pSegsTranscurridos;
			if(animDat->segsEspera > 0.0f){
				if(animDat->segsEspera > segsConsumir){
					animDat->segsEspera		-= segsConsumir; NBASSERT(animDat->segsEspera > 0.0f)
					segsConsumir			= 0.0f;
				} else {
					segsConsumir			-= animDat->segsEspera; NBASSERT(segsConsumir >= 0.0f)
					animDat->segsEspera		= 0.0f;
					animDat->valorInicial	= (animDat->objAnimando->*(animDat->funcionObtenerValor))();
					animDat->objAnimando->establecerDormido(FALSE);
				}
			}
			NBASSERT(segsConsumir >= 0.0f)
			if(segsConsumir != 0.0f){
				float segsAnimar	= animDat->segsAnimar;
				float segsAnimando	= animDat->segsAnimando;
				NBColor8 valInicial	= animDat->valorInicial;
				NBColor8 valDestino	= animDat->valorDestino;
				//
				segsAnimando		+= segsConsumir; animDat->segsAnimando = segsAnimando;
				if(segsAnimando>=segsAnimar){
					(animDat->objAnimando->*animDat->funcion)(valDestino);
					animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
					_animColor.quitarElementoEnIndice(iAnim--);
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
		}
		//Animar Tamanos
		for(iAnim = 0; iAnim < _animTamano.conteo; iAnim++){
			STAnimPropTamano* animDat = &_animTamano.elemento[iAnim];
			segsConsumir = pSegsTranscurridos;
			if(animDat->segsEspera > 0.0f){
				if(animDat->segsEspera > segsConsumir){
					animDat->segsEspera		-= segsConsumir; NBASSERT(animDat->segsEspera > 0.0f)
					segsConsumir			= 0.0f;
				} else {
					segsConsumir			-= animDat->segsEspera; NBASSERT(segsConsumir >= 0.0f)
					animDat->segsEspera		= 0.0f;
					animDat->valorInicial	= (animDat->objAnimando->*(animDat->funcionObtenerValor))();
					animDat->objAnimando->establecerDormido(FALSE);
				}
			}
			NBASSERT(segsConsumir >= 0.0f)
			if(segsConsumir != 0.0f){
				float segsAnimar	= animDat->segsAnimar;
				float segsAnimando	= animDat->segsAnimando;
				NBTamano valInicial	= animDat->valorInicial;
				NBTamano valDestino	= animDat->valorDestino;
				//
				segsAnimando		+= segsConsumir; animDat->segsAnimando = segsAnimando;
				if(segsAnimando>=segsAnimar){
					(animDat->objAnimando->*animDat->funcion)(valDestino);
					animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
					_animTamano.quitarElementoEnIndice(iAnim--);
				} else {
					float avance	= (segsAnimando/segsAnimar);
					switch (animDat->velocidadAnim){
						case ENAnimPropVelocidad_Acelerada:		avance	= (avance*avance); break;
						case ENAnimPropVelocidad_Desacelerada:	avance	= pow(avance, 1.0f/2.0f); break;
						case ENAnimPropVelocidad_Acelerada3:	avance	= (avance*avance*avance); break;
						case ENAnimPropVelocidad_Desacelerada3: avance	= pow(avance, 1.0f/3.0f); break;
						default: break;
					}
					NBTamano valIntermedio;
					valIntermedio.ancho = valInicial.ancho+((valDestino.ancho-valInicial.ancho)*avance);
					valIntermedio.alto = valInicial.alto+((valDestino.alto-valInicial.alto)*avance);
					(animDat->objAnimando->*animDat->funcion)(valIntermedio);
				}
			}
		}
		//Animar Floats
		for(iAnim = 0; iAnim < _animFloat.conteo; iAnim++){
			STAnimPropFloat* animDat = &_animFloat.elemento[iAnim];
			segsConsumir = pSegsTranscurridos;
			if(animDat->segsEspera > 0.0f){
				if(animDat->segsEspera > segsConsumir){
					animDat->segsEspera		-= segsConsumir; NBASSERT(animDat->segsEspera > 0.0f)
					segsConsumir			= 0.0f;
				} else {
					segsConsumir			-= animDat->segsEspera; NBASSERT(segsConsumir >= 0.0f)
					animDat->segsEspera		= 0.0f;
					animDat->valorInicial	= (animDat->objAnimando->*(animDat->funcionObtenerValor))();
					animDat->objAnimando->establecerDormido(FALSE);
				}
			}
			NBASSERT(segsConsumir >= 0.0f)
			if(segsConsumir != 0.0f){
				float segsAnimar	= animDat->segsAnimar;
				float segsAnimando	= animDat->segsAnimando;
				float valInicial	= animDat->valorInicial;
				float valDestino	= animDat->valorDestino;
				//
				segsAnimando		+= segsConsumir; animDat->segsAnimando = segsAnimando;
				if(segsAnimando>=segsAnimar){
					(animDat->objAnimando->*animDat->funcion)(valDestino);
					animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
					_animFloat.quitarElementoEnIndice(iAnim--);
				} else {
					float avance	= (segsAnimando/segsAnimar);
					switch (animDat->velocidadAnim){
						case ENAnimPropVelocidad_Acelerada:		avance	= (avance*avance); break;
						case ENAnimPropVelocidad_Desacelerada:	avance	= pow(avance, 1.0f/2.0f); break;
						case ENAnimPropVelocidad_Acelerada3:	avance	= (avance*avance*avance); break;
						case ENAnimPropVelocidad_Desacelerada3: avance	= pow(avance, 1.0f/3.0f); break;
						default: break;
					}
					float valIntermedio = valInicial+((valDestino-valInicial)*avance);
					(animDat->objAnimando->*animDat->funcion)(valIntermedio);
				}
			}
		}
		//Animar Bools
		for(iAnim = 0; iAnim < _animBool.conteo; iAnim++){
			STAnimPropBool* animDat = &_animBool.elemento[iAnim];
			segsConsumir = pSegsTranscurridos;
			if(animDat->segsEspera > 0.0f){
				if(animDat->segsEspera > segsConsumir){
					animDat->segsEspera	-= segsConsumir; NBASSERT(animDat->segsEspera > 0.0f)
					segsConsumir		= 0.0f;
				} else {
					segsConsumir		-= animDat->segsEspera; NBASSERT(segsConsumir >= 0.0f)
					animDat->segsEspera	= 0.0f;
					animDat->objAnimando->establecerDormido(FALSE);
					//
					{
						bool procesarAnim = true;
						const bool valBoolActual = (animDat->objAnimando->*animDat->funcionObtenerBool)();
						if(valBoolActual == animDat->valorDestinoB){
							procesarAnim = false;
						} else {
							if(animDat->valorDestinoB){
								//De-invisible-hacia-visible
								animDat->valorInicialUI8	= 0;
								animDat->valorDestinoUI8	= 255;
								(animDat->objAnimando->*animDat->funcionUI8)(0);		//Estado inicial: visible-pero-transparente.
								(animDat->objAnimando->*animDat->funcionBool)(true);	//Estado inicial: visible-pero-transparente.
							} else {
								//De-visible-hacia-invisible
								animDat->valorInicialUI8	= (animDat->objAnimando->*animDat->funcionObtenerUI8)();
								animDat->valorDestinoUI8	= 0;
							}
						}
						if(!procesarAnim){
							(animDat->objAnimando->*animDat->funcionUI8)(255);		//Asegurar que el estado final sea consistente como si se hubiese ejecutado la animacion.
							animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
							_animBool.quitarElementoEnIndice(iAnim--);
							continue;
						}
					}
				}
			}
			NBASSERT(segsConsumir >= 0.0f)
			if(segsConsumir != 0.0f){
				float segsAnimar	= animDat->segsAnimar;
				float segsAnimando	= animDat->segsAnimando;
				UI8 valInicial		= animDat->valorInicialUI8;
				UI8 valDestino		= animDat->valorDestinoUI8;
				bool valDestinoB	= animDat->valorDestinoB;
				//
				segsAnimando		+= segsConsumir; animDat->segsAnimando = segsAnimando;
				if(segsAnimando>=segsAnimar){
					(animDat->objAnimando->*animDat->funcionUI8)(255);
					(animDat->objAnimando->*animDat->funcionBool)(valDestinoB);
					animDat->objAnimando->liberar(NB_RETENEDOR_THIS);
					_animBool.quitarElementoEnIndice(iAnim--);
				} else {
					float avance	= (segsAnimando/segsAnimar);
					switch (animDat->velocidadAnim){
						case ENAnimPropVelocidad_Acelerada:		avance	= (avance*avance); break;
						case ENAnimPropVelocidad_Desacelerada:	avance	= pow(avance, 1.0f/2.0f); break;
						case ENAnimPropVelocidad_Acelerada3:	avance	= (avance*avance*avance); break;
						case ENAnimPropVelocidad_Desacelerada3: avance	= pow(avance, 1.0f/3.0f); break;
						default: break;
					}
					UI8 valIntermedio;
					valIntermedio	= valInicial+((float)(valDestino-valInicial)*avance);
					(animDat->objAnimando->*animDat->funcionUI8)(valIntermedio);
					(animDat->objAnimando->*animDat->funcionBool)(true);
				}
			}
		}
		//
		_animandoProps = (this->conteoAnimacionesEjecutando() != 0);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUAnimadorObjetoEscena)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUAnimadorObjetoEscena, "AUAnimadorObjetoEscena")
AUOBJMETODOS_CLONAR_NULL(AUAnimadorObjetoEscena)


