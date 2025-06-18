//
//  AUAnimadorContenedor.h
//  AUFramework
//
//  Created by Nicaragua Binary on 09/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUAnimadorContenedor_H_INCLUIDO
#define AUAnimadorContenedor_H_INCLUIDO

#include "NBColor.h"
#include "AUObjeto.h"
#include "NBAnimador.h"
#include "AUAnimadorObjetoEscena.h"
#include "AUEscenaContenedor.h"
#include "AUAnimadorPropsDef.h"

struct STAnimPropContenedor {
	AUEscenaContenedor*	objAnimando;
	float				segsAnimar;
	float				segsAnimando;
	ENAnimPropVelocidad	velocidadAnim;
	//
	bool operator==(const STAnimPropContenedor &otro) const {
		return (objAnimando==otro.objAnimando && segsAnimar==otro.segsAnimar && segsAnimando==otro.segsAnimando && velocidadAnim==otro.velocidadAnim);
	}
	bool operator!=(const STAnimPropContenedor &otro) const {
		return !(objAnimando==otro.objAnimando && segsAnimar==otro.segsAnimar && segsAnimando==otro.segsAnimando && velocidadAnim==otro.velocidadAnim);
	}
};

//ANIMACION DE COLORES

typedef NBColor8 (AUEscenaContenedor::*PTRfuncObtenerColorCont)();
typedef void (AUEscenaContenedor::*PTRfuncEstablecerColorCont)(const NBColor8 &valor);

struct STAnimPropColorContenedor : public STAnimPropContenedor {
	NBColor8 valorInicial;
	NBColor8 valorDestino;
	PTRfuncEstablecerColorCont funcion;
};

class AUAnimadorContenedor : public AUObjeto, public NBAnimador {
	public:
		AUAnimadorContenedor();
		virtual ~AUAnimadorContenedor();
		//
		SI32						conteoAnimacionesEjecutando();
		bool						animandoObjeto(AUEscenaContenedor* objAnimando);
		void						quitarAnimacionesDeObjeto(AUEscenaObjeto* objAnimando);
		void						purgarAnimaciones();
		//
		void						animarColor(AUEscenaContenedor* objAnimando, NBColor8 valDestino, float segundosAnimacion, PTRfuncObtenerColorCont funcionObtenerValor, PTRfuncEstablecerColorCont funcionEstablecerValor, ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal);
		void						animarColor(AUEscenaContenedor* objAnimando, float rDestino, float gDestino, float bDestino, float aDestino, float segundosAnimacion, PTRfuncObtenerColorCont funcionObtenerValor, PTRfuncEstablecerColorCont funcionEstablecerValor, ENAnimPropVelocidad velocidad=ENAnimPropVelocidad_Lineal)	;
		//
		virtual void				tickAnimacion(float segsTranscurridos);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		bool												_animandoProps;
		AUArregloNativoMutableP<STAnimPropColorContenedor>	_animColorCont;
};

#endif
