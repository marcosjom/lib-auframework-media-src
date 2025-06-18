//
//  AUEscenaMalla3.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 06/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaMalla3_H_INCLUIDO
#define AUEscenaMalla3_H_INCLUIDO

#include "AUEscenaMalla.h"
#include "AUEscenaMallaXBase.h"

class AUEscenaMalla3: public AUEscenaMallaXBase {
	public:
		AUEscenaMalla3(const char* prefijoRutaTexturas3);
		virtual ~AUEscenaMalla3();
		//
		virtual NBCajaAABB		cajaMinima();
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUEscenaMalla*			_mallaFondo;
		AUTextura*				_texturasMarco[3];
		//
		virtual void			actualizarPresentacionMalla(const NBCajaAABB &cajaMarco);
};

#endif
