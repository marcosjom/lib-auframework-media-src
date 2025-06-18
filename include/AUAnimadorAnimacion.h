//
//  AUAnimadorAnimacion.h
//  AUFramework
//
//  Created by Nicaragua Binary on 09/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUAnimadorAnimacion_H_INCLUIDO
#define AUAnimadorAnimacion_H_INCLUIDO

#include "AUAnimadorContenedor.h"
#include "AUEscenaAnimacionI.h"
#include "AUAnimadorPropsDef.h"

struct STAnimPropAnim {
	AUEscenaAnimacionI*	objAnimando;
	float				segsAnimar;
	float				segsAnimando;
	ENAnimPropVelocidad	velocidadAnim;
	//
	bool operator==(const STAnimPropAnim &otro) const {
		return (objAnimando==otro.objAnimando && segsAnimar==otro.segsAnimar && segsAnimando==otro.segsAnimando && velocidadAnim==otro.velocidadAnim);
	}
	bool operator!=(const STAnimPropAnim &otro) const {
		return !(objAnimando==otro.objAnimando && segsAnimar==otro.segsAnimar && segsAnimando==otro.segsAnimando && velocidadAnim==otro.velocidadAnim);
	}
};

//ANIMACION DE FLOATS

typedef float (AUEscenaAnimacionI::*PTRfuncObtenerFloatAnim)();
typedef void (AUEscenaAnimacionI::*PTRfuncEstablecerFloatAnim)(float valor);

struct STAnimPropFloatAnim : public STAnimPropAnim {
	float valorInicial;
	float valorDestino;
	PTRfuncEstablecerFloatAnim funcion;
};

//ANIMACION DE ESTADOS

typedef bool (AUEscenaAnimacionI::*PTRfuncObtenerEstado)();
typedef void (AUEscenaAnimacionI::*PTRfuncEstablecerEstado)();

struct STAnimPropBoolAnim : public STAnimPropAnim {
	float valorInicialF;
	float valorDestinoF;
	PTRfuncEstablecerFloatAnim funcionFloat;
	PTRfuncEstablecerEstado funcionEstadoFinal;
};

//

class AUAnimadorAnimacion : public AUObjeto, public NBAnimador {
	public:
		AUAnimadorAnimacion();
		virtual ~AUAnimadorAnimacion();
		//
		SI32						conteoAnimacionesEjecutando();
		//
		void						animarFloat(AUEscenaAnimacionI* objAnimando, float valDestino, float segundosAnimacion, PTRfuncObtenerFloatAnim funcionObtenerValor, PTRfuncEstablecerFloatAnim funcionEstablecerValor, ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal);
		void						animarEstado(AUEscenaAnimacionI* objAnimando, bool estadoDestinoEsActivo, float segundosAnimacion, PTRfuncObtenerEstado funcionObtenerEstado, PTRfuncEstablecerEstado funcionEstablecerEstadoActivo, PTRfuncEstablecerEstado funcionEstablecerEstadoDestino, PTRfuncObtenerFloatAnim funcionObtenerValorFloat, PTRfuncEstablecerFloatAnim funcionEstablecerValorFloat, ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal);
		//
		virtual void				tickAnimacion(float segsTranscurridos);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		bool											_animandoProps;
		AUArregloNativoMutableP<STAnimPropFloatAnim>	_animFloatAnim;
		AUArregloNativoMutableP<STAnimPropBoolAnim>		_animBoolAnim;
};

#endif
