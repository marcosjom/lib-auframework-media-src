//
//  AUFuenteVisualModels.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUFuenteVisualModel_H_INCLUIDO
#define AUFuenteVisualModel_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "nb/core/NBRange.h"

typedef PTRfunCmdsGL	(*AUFuenteVisualModelProduceRenderModelFunc)(const void* data, const NBPropRenderizado &propsRenderizado, const NBMatriz matrizEscena, const NBColor colorMultiplicado, const STNBRangeI range, STBloqueGL* dstVertexBlock, UI8* dstVertexBlockType, AUArreglo* visualFilters);
typedef void			(*AUFuenteVisualModelDestroyFunc)(void* data);

class AUFuenteVisualModel : public AUObjeto {
	public:
		AUFuenteVisualModel(AUObjeto* font, void* data, AUFuenteVisualModelProduceRenderModelFunc funcProduceRenderModel, AUFuenteVisualModelDestroyFunc funcDestroy, AUArreglo* visualFilters);
		virtual ~AUFuenteVisualModel();
		//
		PTRfunCmdsGL produceRenderModel(const NBPropRenderizado &propsRenderizado, const NBMatriz matrizEscena, const NBColor colorMultiplicado, const STNBRangeI range, STBloqueGL* dstVertexBlock, UI8* dstVertexBlockType);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUObjeto*	_font;
		void*		_data;
		AUFuenteVisualModelProduceRenderModelFunc _funcProduceRenderModel;
		AUFuenteVisualModelDestroyFunc _funcDestroy;
		AUArreglo*	_visualFilters;
};

#endif
