//
//  AUTextoFiltros.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUTextoFiltro_H_INCLUIDO
#define AUTextoFiltro_H_INCLUIDO

#include "AUObjeto.h"
#include "nb/2d/NBColor.h"
#include "nb/2d/NBAABox.h"

class AUTextoFiltro : public AUObjeto {
	public:
		AUTextoFiltro();
		virtual ~AUTextoFiltro();
		//Filter
		virtual STNBAABox	boxFiltered(const STNBAABox other) = 0;
		virtual STNBColor8	colorMultForCharPoint(const float xLocal, const float yLocal) = 0;
		virtual STNBColor8	colorMultForCharRect(const float x, const float y, const float w, const float h) = 0;
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
};

#endif
