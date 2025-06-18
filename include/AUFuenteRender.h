//
//  AUFuenteRenders.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUFuenteRender_H_INCLUIDO
#define AUFuenteRender_H_INCLUIDO

#include "nb/fonts/NBTextMetricsBuilder.h"
#include "AUEscenaObjeto.h"
#include "AUFuenteVisualModel.h"
#include "AUTextoFiltro.h"

typedef struct STNBFontRenderBox_ {
	STNBAABox				box;		//Box
	AUFuenteVisualModel*	model;		//Not-NULL (for level 0), or NULL (for levels > 0)
	STNBRangeI				rng;		//Indexes of chars inside model (for level 0) or indexes of groups (for levels > 0)
    STNBRangeI              rngChars;   //Metrics chars defs
} STNBFontRenderBox;

class AUFuenteRender : public AUObjeto {
	public:
		AUFuenteRender();
		virtual ~AUFuenteRender();
		//Font properties
		virtual const char*			familia() = 0;
		virtual const char*			subfamilia() = 0;
		virtual bool				esNegrilla() = 0;
		virtual bool 				esCursiva() = 0;
		virtual float				tamanoEscena() = 0;
		virtual float				ascendenteEscena() = 0;
		virtual float				descendenteEscena() = 0;
		virtual float				altoGuiaLineaEscena() = 0;
		virtual float				altoParaSiguienteLineaEscena() = 0;
		//Set fontProperties to metricsBuilder
		virtual STNBFontMetricsIRef	getFontMetricsIRef(const float defFontSz) = 0;
		virtual void				configureMetricsBuilder(STNBTextMetricsBuilder* mBuilder) = 0;
		//Metrics to visualModel
		virtual AUFuenteVisualModel* newVisualModelForChars(const STNBTextMetrics* textMetrics, const UI32 firstChar, const UI32 countChars, STNBArray* dstBoxes, STNBFontRenderBox* dstBoxesCur, const UI32 maxBoxesCharsCount, AUArreglo* visualFilters) = 0;
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
};

#endif
