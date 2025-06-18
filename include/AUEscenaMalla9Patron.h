//
//  AUEscenaMalla9.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 06/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUESCENAMALLA9PATRON_H_INCLUIDO
#define AUESCENAMALLA9PATRON_H_INCLUIDO

#include "AUEscenaSpritePorcion.h"
#include "AUEscenaMallaXBase.h"

class AUEscenaMalla9Patron: public AUEscenaMallaXBase {
	public:
		AUEscenaMalla9Patron(const char* prefijoRutaTexturas9);
		virtual ~AUEscenaMalla9Patron();
		//
		virtual NBCajaAABB		cajaMinima();
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUTextura*				_texturasMarco[9];
		AUEscenaSprite*			_esquinaSupIzq;
		AUEscenaSprite*			_esquinaSupDer;
		AUEscenaSprite*			_esquinaInfIzq;
		AUEscenaSprite*			_esquinaInfDer;
		AUEscenaSprite*			_spriteCentro;
		AUArregloMutable		_spritesSup;
		AUArregloMutable		_spritesInf;
		AUArregloMutable		_spritesIzq;
		AUArregloMutable		_spritesDer;
		//
		virtual void			actualizarPresentacionMalla(const NBCajaAABB &cajaMarco);
};

#endif
