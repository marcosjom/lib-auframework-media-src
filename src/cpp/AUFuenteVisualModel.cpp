//
//  AUFuenteVisualModel.cpp
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUFuenteVisualModel.h"

AUFuenteVisualModel::AUFuenteVisualModel(AUObjeto* font, void* data, AUFuenteVisualModelProduceRenderModelFunc funcProduceRenderModel, AUFuenteVisualModelDestroyFunc funcDestroy, AUArreglo* visualFilters) : AUObjeto() {
	_font					= font; if(_font != NULL) _font->retener(NB_RETENEDOR_THIS);
	_data					= data;
	_funcProduceRenderModel	= funcProduceRenderModel;
	_funcDestroy			= funcDestroy;
	_visualFilters			= visualFilters; if(_visualFilters != NULL) _visualFilters->retener(NB_RETENEDOR_THIS);
}

AUFuenteVisualModel::~AUFuenteVisualModel(){
	if(_visualFilters != NULL) _visualFilters->liberar(NB_RETENEDOR_THIS); _visualFilters = NULL;
	{
		if(_data != NULL){
			if(_funcDestroy != NULL){
				(*_funcDestroy)(_data);
			}
			_data = NULL;
		}
		_funcProduceRenderModel = NULL;
		_funcDestroy = NULL;
	}
	if(_font != NULL) _font->liberar(NB_RETENEDOR_THIS); _font = NULL;
}

//

PTRfunCmdsGL AUFuenteVisualModel::produceRenderModel(const NBPropRenderizado &propsRenderizado, const NBMatriz matrizEscena, const NBColor colorMultiplicado, const STNBRangeI range, STBloqueGL* dstVertexBlock, UI8* dstVertexBlockType){
	PTRfunCmdsGL r = NULL;
	if(_funcProduceRenderModel != NULL){
		r = (*_funcProduceRenderModel)(_data, propsRenderizado, matrizEscena, colorMultiplicado, range, dstVertexBlock, dstVertexBlockType, _visualFilters);
	}
	return r;
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUFuenteVisualModel)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUFuenteVisualModel, "AUFuenteVisualModel")
AUOBJMETODOS_CLONAR_NULL(AUFuenteVisualModel)




