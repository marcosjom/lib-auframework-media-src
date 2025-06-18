//
//  AUTextoFiltroLimitado.cpp
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUTextoFiltroLimitado.h"
//
#include "nb/fonts/NBTextMetricsBuilder.h"
#include "NBGestorAnimadores.h"

AUTextoFiltroLimitado::AUTextoFiltroLimitado()
: AUTextoFiltro()
, _limsInner(NBST_P(STNBAABox, 0, 0, 0, 0 ))
, _limsOuter(NBST_P(STNBAABox, 0, 0, 0, 0 ))
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUTextoFiltroLimitado")
	
}

AUTextoFiltroLimitado::AUTextoFiltroLimitado(const STNBAABox innerLimits, const STNBAABox outerLimits)
: AUTextoFiltro()
, _limsInner(innerLimits)
, _limsOuter(outerLimits)
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUTextoFiltroLimitado")
}

AUTextoFiltroLimitado::AUTextoFiltroLimitado(AUTextoFiltroLimitado* other)
: AUTextoFiltro()
, _limsInner(other->_limsInner)
, _limsOuter(other->_limsOuter)
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUTextoFiltroLimitado")
}

AUTextoFiltroLimitado::~AUTextoFiltroLimitado(){
	//
}

//

STNBAABox AUTextoFiltroLimitado::getInnerLimits() const {	//inner limit
	return _limsInner;
}

STNBAABox AUTextoFiltroLimitado::getOuterLimits() const {	//outer limit
	return _limsOuter;
}

//

void AUTextoFiltroLimitado::emptyLimits(){
	_limsInner = NBST_P(STNBAABox, 0, 0, 0, 0 );
	_limsOuter = NBST_P(STNBAABox, 0, 0, 0, 0 );
}

void AUTextoFiltroLimitado::setInnerLimits(const float xMin, const float xMax, const float yMin, const float yMax){
	_limsInner.xMin = xMin;
	_limsInner.xMax = xMax;
	_limsInner.yMin = yMin;
	_limsInner.yMax = yMax;
}

void AUTextoFiltroLimitado::setInnerLimits(const STNBAABox limits){
	_limsInner = limits;
}

void AUTextoFiltroLimitado::setOuterLimits(const float xMin, const float xMax, const float yMin, const float yMax){
	_limsOuter.xMin = xMin;
	_limsOuter.xMax = xMax;
	_limsOuter.yMin = yMin;
	_limsOuter.yMax = yMax;
}

void AUTextoFiltroLimitado::setOuterLimits(const STNBAABox limits){
	_limsOuter = limits;
}

//

STNBAABox AUTextoFiltroLimitado::boxFiltered(const STNBAABox other){
	STNBAABox r = other;
	NBASSERT(r.xMin <= r.xMax && r.yMin <= r.yMax)
	if(r.xMin < _limsOuter.xMin){ r.xMin = _limsOuter.xMin; } else if(r.xMin > _limsOuter.xMax){ r.xMin = _limsOuter.xMax; }
	if(r.xMax < _limsOuter.xMin){ r.xMax = _limsOuter.xMin; } else if(r.xMax > _limsOuter.xMax){ r.xMax = _limsOuter.xMax; }
	if(r.yMin < _limsOuter.yMin){ r.yMin = _limsOuter.yMin; } else if(r.yMin > _limsOuter.yMax){ r.yMin = _limsOuter.yMax; }
	if(r.yMax < _limsOuter.yMin){ r.yMax = _limsOuter.yMin; } else if(r.yMax > _limsOuter.yMax){ r.yMax = _limsOuter.yMax; }
	return r;
}

STNBColor8 AUTextoFiltroLimitado::colorMultForCharPoint(const float xLocal, const float yLocal){
	STNBColor8 r = { 255, 255, 255, 255 };
	//Apply vertical limit
	if(_limsOuter.xMin != _limsOuter.xMax || _limsOuter.yMin != _limsOuter.yMax){
		if(yLocal < _limsInner.yMin){
			if(yLocal <= _limsOuter.yMin){
				r.a = 0;
			} else {
				NBASSERT(_limsInner.yMin != _limsOuter.yMin)
				r.a = (float)r.a * (1.0f - ((yLocal - _limsOuter.yMin) / (_limsInner.yMin - _limsOuter.yMin)));
			}
		} else if(_limsInner.yMax < yLocal){
			if(_limsOuter.yMax < yLocal){
				r.a = 0;
			} else {
				NBASSERT(_limsInner.yMax != _limsOuter.yMax)
				r.a = (float)r.a * ((_limsOuter.yMax - yLocal) / (_limsOuter.yMax - _limsInner.yMax));
			}
		}
	}
	return r;
}

STNBColor8 AUTextoFiltroLimitado::colorMultForCharRect(const float x, const float y, const float w, const float h){
	STNBColor8 r = { 255, 255, 255, 255 };
	//Apply vertical limit
	if(_limsOuter.xMin != _limsOuter.xMax || _limsOuter.yMin != _limsOuter.yMax){
		float yMax, yMin;
		if(h < 0){
			yMin = y + h;
			yMax = y;
		} else {
			yMin = y;
			yMax = y + h;
		} NBASSERT(yMin <= yMax)
		if(yMin < _limsInner.yMin){
			if(yMin <= _limsOuter.yMin){
				r.a = 0;
			} else {
				NBASSERT(_limsInner.yMin != _limsOuter.yMin)
				r.a = (float)r.a * ((yMin - _limsOuter.yMin) / (_limsInner.yMin - _limsOuter.yMin));
			}
		} else if(_limsInner.yMax < yMax){
			if(_limsOuter.yMax < yMax){
				r.a = 0;
			} else {
				NBASSERT(_limsInner.yMax != _limsOuter.yMax)
				r.a = (float)r.a * ((_limsOuter.yMax - yMax) / (_limsOuter.yMax - _limsInner.yMax));
			}
		}
	}
	return r;
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUTextoFiltroLimitado, AUTextoFiltro)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUTextoFiltroLimitado, "AUTextoFiltroLimitado", AUTextoFiltro)
AUOBJMETODOS_CLONAR_THIS(AUTextoFiltroLimitado)






