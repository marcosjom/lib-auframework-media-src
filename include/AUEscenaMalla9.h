//
//  AUEscenaMalla9.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 06/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUESCENAMALLA9_H_INCLUIDO
#define AUESCENAMALLA9_H_INCLUIDO

#include "AUEscenaMalla.h"
#include "AUEscenaMallaXBase.h"

class AUEscenaMalla9: public AUEscenaMallaXBase {
	public:
		AUEscenaMalla9(const char* prefijoRutaTexturas9);
		virtual ~AUEscenaMalla9();
		//
		virtual NBCajaAABB		cajaMinima();
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUEscenaMalla*			_mallaFondo;
		AUTextura*				_texturasMarco[9];
		//
		virtual void			actualizarPresentacionMalla(const NBCajaAABB &cajaMarco);
};

#endif
