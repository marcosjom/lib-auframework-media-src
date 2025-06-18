//
//  AUTextoFiltroLimitado.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUTextoFiltroLimitado_H_INCLUIDO
#define AUTextoFiltroLimitado_H_INCLUIDO

#include "AUEscenaTexto.h"
#include "nb/2d/NBAABox.h"

class AUTextoFiltroLimitado : public AUTextoFiltro {
	public:
		AUTextoFiltroLimitado();
		AUTextoFiltroLimitado(const STNBAABox innerLimits, const STNBAABox outerLimits);
		AUTextoFiltroLimitado(AUTextoFiltroLimitado* other);
		virtual ~AUTextoFiltroLimitado();
		//
		STNBAABox	getInnerLimits() const;	//inner limit
		STNBAABox	getOuterLimits() const;	//outer limit
		//
		void		emptyLimits();
		void		setInnerLimits(const float xMin, const float xMax, const float yMin, const float yMax);
		void		setInnerLimits(const STNBAABox limits);
		void		setOuterLimits(const float xMin, const float xMax, const float yMin, const float yMax);
		void		setOuterLimits(const STNBAABox limits);
		//
		STNBAABox	boxFiltered(const STNBAABox other);
		STNBColor8	colorMultForCharPoint(const float xLocal, const float yLocal);
		STNBColor8	colorMultForCharRect(const float x, const float y, const float w, const float h);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		STNBAABox	_limsInner;	//inner limit
		STNBAABox	_limsOuter;	//outer limit
};

#endif
