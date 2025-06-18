//
//  AUAnimadorObjetoEscena.h
//  AUFramework
//
//  Created by Nicaragua Binary on 09/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUAnimadorObjetoEscena_H_INCLUIDO
#define AUAnimadorObjetoEscena_H_INCLUIDO

#include "AUObjeto.h"
#include "AUEscenaObjeto.h"
#include "NBGestorAnimadores.h"
#include "NBAnimador.h"
#include "AUArregloNativoMutableP.h"
#include "NBPunto.h"
#include "NBColor.h"
#include "NBTamano.h"
#include "AUAnimadorPropsDef.h"

struct STAnimProp {
	AUEscenaObjeto*		objAnimando;
	float				segsEspera;		//segundos antes de iniciar la animacion
	float				segsAnimar;
	float				segsAnimando;
	ENAnimPropVelocidad	velocidadAnim;
	//
	bool operator==(const STAnimProp &otro) const {
		return (objAnimando==otro.objAnimando && segsEspera==otro.segsEspera && segsAnimar==otro.segsAnimar && segsAnimando==otro.segsAnimando && velocidadAnim==otro.velocidadAnim);
	}
	bool operator!=(const STAnimProp &otro) const {
		return !(objAnimando==otro.objAnimando && segsEspera==otro.segsEspera && segsAnimar==otro.segsAnimar && segsAnimando==otro.segsAnimando && velocidadAnim==otro.velocidadAnim);
	}
};

//ANIMACION DE PUNTOS

typedef NBPunto (AUEscenaObjeto::*PTRfuncObtenerPunto)();
typedef void (AUEscenaObjeto::*PTRfuncEstablecerPunto)(const NBPunto &valor);

struct STAnimPropPunto : public STAnimProp {
	NBPunto valorInicial;
	NBPunto valorDestino;
	PTRfuncObtenerPunto funcionObtenerValor;
	PTRfuncEstablecerPunto funcion;
};

//ANIMACION DE COLORES

typedef NBColor8 (AUEscenaObjeto::*PTRfuncObtenerColor)();
typedef void (AUEscenaObjeto::*PTRfuncEstablecerColor)(const NBColor8 &valor);

struct STAnimPropColor : public STAnimProp {
	NBColor8 valorInicial;
	NBColor8 valorDestino;
	PTRfuncObtenerColor funcionObtenerValor;
	PTRfuncEstablecerColor funcion;
};

//ANIMACION DE TAMANOS

typedef NBTamano (AUEscenaObjeto::*PTRfuncObtenerTamano)();
typedef void (AUEscenaObjeto::*PTRfuncEstablecerTamano)(const NBTamano &valor);

struct STAnimPropTamano : public STAnimProp {
	NBTamano valorInicial;
	NBTamano valorDestino;
	PTRfuncObtenerTamano funcionObtenerValor;
	PTRfuncEstablecerTamano funcion;
};

//ANIMACION DE FLOATS

typedef float (AUEscenaObjeto::*PTRfuncObtenerFloat)();
typedef void (AUEscenaObjeto::*PTRfuncEstablecerFloat)(float valor);

struct STAnimPropFloat : public STAnimProp {
	float valorInicial;
	float valorDestino;
	PTRfuncObtenerFloat funcionObtenerValor;
	PTRfuncEstablecerFloat funcion;
};

//ANIMACION DE BOOLEANOS (interpretado como animacion de float)

typedef bool (AUEscenaObjeto::*PTRfuncObtenerBool)();
typedef void (AUEscenaObjeto::*PTRfuncEstablecerBool)(bool valor);
typedef UI8  (AUEscenaObjeto::*PTRfuncObtenerUI8)();
typedef void (AUEscenaObjeto::*PTRfuncEstablecerUI8)(UI8 valor);

struct STAnimPropBool : public STAnimProp {
	bool valorInicialB;
	bool valorDestinoB;
	float valorInicialUI8;
	float valorDestinoUI8;
	PTRfuncObtenerBool		funcionObtenerBool;
	PTRfuncEstablecerBool	funcionBool;
	PTRfuncObtenerUI8		funcionObtenerUI8;
	PTRfuncEstablecerUI8	funcionUI8;
};

class AUAnimadorObjetoEscena : public AUObjeto, public NBAnimador {
	public:
		AUAnimadorObjetoEscena();
		virtual ~AUAnimadorObjetoEscena();
		//
		SI32						conteoAnimacionesEjecutando();
		bool						animandoObjeto(AUEscenaObjeto* objAnimando);
		void						quitarAnimaciones();
		void						quitarAnimacionesDeObjeto(AUEscenaObjeto* objAnimando);
		void						purgarAnimaciones();
		void						purgarAnimacionesDeObjeto(AUEscenaObjeto* objAnimando);
		//Animadores especificos
		void						animarPosicion(AUEscenaObjeto* objAnimando, const NBPunto valDestino, const float segundosAnimacion, const ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, const float segsEspera=0.0f);
		void						animarPosicion(AUEscenaObjeto* objAnimando, const float xDestino, const float yDestino, const float segundosAnimacion, const ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, const float segsEspera=0.0f);
		void						animarColorMult(AUEscenaObjeto* objAnimando, const NBColor8 valDestino, const float segundosAnimacion, const ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, const float segsEspera=0.0f);
		void						animarColorMult(AUEscenaObjeto* objAnimando, const UI8 rDestino, const UI8 gDestino, const UI8 bDestino, const UI8 aDestino, const float segundosAnimacion, const ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, const float segsEspera=0.0f);
		void						animarEscalacion(AUEscenaObjeto* objAnimando, const NBTamano valDestino, float segundosAnimacion, const ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, const float segsEspera=0.0f);
		void						animarEscalacion(AUEscenaObjeto* objAnimando, const float anchoDestino, const float altoDestino, const float segundosAnimacion, const ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, const float segsEspera=0.0f);
		void						animarRotacion(AUEscenaObjeto* objAnimando, const float valDestino, const float segundosAnimacion, const ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, const float segsEspera=0.0f);
		void						animarVisible(AUEscenaObjeto* objAnimando, const bool valDestino, const float segundosAnimacion, const ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, const float segsEspera=0.0f);
		//Animadores genericos
		//void						animarPunto(AUEscenaObjeto* objAnimando, NBPunto valDestino, float segundosAnimacion, PTRfuncObtenerPunto funcionObtenerValor, PTRfuncEstablecerPunto funcionEstablecerValor, ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, float segsEspera=0.0f);
		void						animarPunto(AUEscenaObjeto* objAnimando, float xDestino, float yDestino, float segundosAnimacion, PTRfuncObtenerPunto funcionObtenerValor, PTRfuncEstablecerPunto funcionEstablecerValor, ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, float segsEspera=0.0f);
		//void						animarColor(AUEscenaObjeto* objAnimando, NBColor8 valDestino, float segundosAnimacion, PTRfuncObtenerColor funcionObtenerValor, PTRfuncEstablecerColor funcionEstablecerValor, ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, float segsEspera=0.0f);
		void						animarColor(AUEscenaObjeto* objAnimando, UI8 rDestino, UI8 gDestino, UI8 bDestino, UI8 aDestino, float segundosAnimacion, PTRfuncObtenerColor funcionObtenerValor, PTRfuncEstablecerColor funcionEstablecerValor, ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, float segsEspera=0.0f)	;
		//void						animarTamano(AUEscenaObjeto* objAnimando, NBTamano valDestino, float segundosAnimacion, PTRfuncObtenerTamano funcionObtenerValor, PTRfuncEstablecerTamano funcionEstablecerValor, ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, float segsEspera=0.0f);
		void						animarTamano(AUEscenaObjeto* objAnimando, float anchoDestino, float altoDestino, float segundosAnimacion, PTRfuncObtenerTamano funcionObtenerValor, PTRfuncEstablecerTamano funcionEstablecerValor, ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, float segsEspera=0.0f);
		void						animarFloat(AUEscenaObjeto* objAnimando, float valDestino, float segundosAnimacion, PTRfuncObtenerFloat funcionObtenerValor, PTRfuncEstablecerFloat funcionEstablecerValor, ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, float segsEspera=0.0f);
		void						animarBool(AUEscenaObjeto* objAnimando, bool valDestino, float segundosAnimacion, PTRfuncObtenerBool funcionObtenerValorBool, PTRfuncEstablecerBool funcionEstablecerValorBool, PTRfuncObtenerUI8 funcionObtenerValorUI8, PTRfuncEstablecerUI8 funcionEstablecerValorUI8, ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal, float segsEspera=0.0f);
		//
		virtual void				tickAnimacion(float segsTranscurridos);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		bool										_animandoProps;
		AUArregloNativoMutableP<STAnimPropPunto>	_animPunto;
		AUArregloNativoMutableP<STAnimPropColor>	_animColor;
		AUArregloNativoMutableP<STAnimPropTamano>	_animTamano;
		AUArregloNativoMutableP<STAnimPropFloat>	_animFloat;
		AUArregloNativoMutableP<STAnimPropBool>		_animBool;
		//
		void						privPurgarAnimaciones(AUEscenaObjeto* objEnBusqueda);
};

#endif
