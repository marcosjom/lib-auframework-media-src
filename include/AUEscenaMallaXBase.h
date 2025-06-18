//
//  AUEscenaMalla3.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 06/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaMallaXBase_H_INCLUIDO
#define AUEscenaMallaXBase_H_INCLUIDO

#include "NBCajaAABB.h"
#include "AUEscenaContenedor.h"

class AUEscenaMallaXBase: public AUEscenaContenedor {
	public:
		AUEscenaMallaXBase();
		virtual ~AUEscenaMallaXBase();
		//
		virtual NBCajaAABB		cajaMinima()=0;
		NBCajaAABB				cajaMarcoMalla();
		void					establecerCajaMarcoMalla(const NBCajaAABB &cajaMarco);
		void					establecerCajaMarcoMalla(float xMin, float yMin, float xMax, float yMax);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		NBCajaAABB				_cajaActualMalla;
		//
		virtual void			actualizarPresentacionMalla(const NBCajaAABB &cajaMarco)=0;
};

#endif
