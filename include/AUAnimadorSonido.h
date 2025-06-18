//
//  AUAnimadorSonido.h
//  AUFramework
//
//  Created by Nicaragua Binary on 09/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#if !defined(AUAnimadorSonido_H_INCLUIDO) && !defined(CONFIG_NB_UNSUPPORT_AUDIO_IO)
#define AUAnimadorSonido_H_INCLUIDO

#include "AUObjeto.h"
#include "AUAnimadorContenedor.h"
#include "AUEscenaSnd.h"
#include "AUAnimadorPropsDef.h"

struct STAnimPropSnd {
	AUEscenaSnd*		objAnimando;
	float				segsAnimar;
	float				segsAnimando;
	ENAnimPropVelocidad	velocidadAnim;
	//
	bool operator==(const STAnimPropSnd &otro) const {
		return (objAnimando==otro.objAnimando && segsAnimar==otro.segsAnimar && segsAnimando==otro.segsAnimando && velocidadAnim==otro.velocidadAnim);
	}
	bool operator!=(const STAnimPropSnd &otro) const {
		return !(objAnimando==otro.objAnimando && segsAnimar==otro.segsAnimar && segsAnimando==otro.segsAnimando && velocidadAnim==otro.velocidadAnim);
	}
};

//ANIMACION DE FLOATS

typedef float (AUEscenaSnd::*PTRfuncObtenerFloatSnd)();
typedef void (AUEscenaSnd::*PTRfuncEstablecerFloatSnd)(float valor);

struct STAnimPropFloatSnd : public STAnimPropSnd {
	float valorInicial;
	float valorDestino;
	PTRfuncEstablecerFloatSnd funcion;
};

//ANIMACION DE ESTADOS

typedef bool (AUEscenaSnd::*PTRfuncObtenerEstadoSnd)();
typedef void (AUEscenaSnd::*PTRfuncEstablecerEstadoSnd)();

struct STAnimPropBoolSnd : public STAnimPropSnd {
	float valorInicialF;
	float valorDestinoF;
	PTRfuncEstablecerFloatSnd funcionFloat;
	PTRfuncEstablecerEstadoSnd funcionEstadoFinal;
};

//

class AUAnimadorSonido : public AUObjeto, public NBAnimador {
	public:
		AUAnimadorSonido();
		virtual ~AUAnimadorSonido();
		//
		SI32						conteoAnimacionesEjecutando();
		//
		void						animarFloat(AUEscenaSnd* objAnimando, float valDestino, float segundosAnimacion, PTRfuncObtenerFloatSnd funcionObtenerValor, PTRfuncEstablecerFloatSnd funcionEstablecerValor, ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal);
		void						animarEstado(AUEscenaSnd* objAnimando, bool estadoDestinoEsActivo, float segundosAnimacion, PTRfuncObtenerEstadoSnd funcionObtenerEstado, PTRfuncEstablecerEstadoSnd funcionEstablecerEstadoActivo, PTRfuncEstablecerEstadoSnd funcionEstablecerEstadoDestino, PTRfuncObtenerFloatSnd funcionObtenerValorFloat, PTRfuncEstablecerFloatSnd funcionEstablecerValorFloat, ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal);
		//
		virtual void				tickAnimacion(float segsTranscurridos);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		bool											_animandoProps;
		AUArregloNativoMutableP<STAnimPropFloatSnd>		_animFloatAnim;
		AUArregloNativoMutableP<STAnimPropBoolSnd>		_animBoolAnim;
};

#endif
