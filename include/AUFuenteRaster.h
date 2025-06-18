//
//  AUFuenteRasters.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUFuenteRaster_H_INCLUIDO
#define AUFuenteRaster_H_INCLUIDO

#include "AUFuenteRender.h"
#include "nb/fonts/NBFontMetrics.h"
#include "nb/fonts/NBFontLines.h"
#include "AUAtlasMapa.h"
#include "AUTextura.h"
#include "NBPunto.h"

class AUFuenteRaster : public AUFuenteRender {
	public:
		AUFuenteRaster(STNBFontLines* fuenteDef, const float sceneFontSz, const float escalaEspaciado);
		virtual ~AUFuenteRaster();
		//
		BOOL						isDisabled();
		void						setDisabled(const BOOL isDisabled);
		//Font properties
		STNBFontLines*				fuenteDef();
		const char*					familia();
		const char*					subfamilia();
		bool						esNegrilla();
		bool 						esCursiva();
		float						tamanoEscena();
		float						ascendenteEscena();
		float						descendenteEscena();
		float						altoGuiaLineaEscena();
		float						altoParaSiguienteLineaEscena();
		//
		void						updateFontSize(const float sceneFontSz);
		//
		STNBFontShapeMetrics		propiedadesParaCaracter(const float tamFuentePx, const UI32 unicode32);
		float						propiedadesKerning(const float tamFuentePx, const UI32 unicode32, const UI32 unicode32Anterior);
		static STNBFontShapeMetrics propiedadesParaCaracter(void* objBiblioteca, const float tamFuentePx, const UI32 unicode32);
		static float				propiedadesKerning(void* objBiblioteca, const float tamFuentePx, const UI32 unicode32, const UI32 unicode32Anterior);
		//Set fontProperties to metricsBuilder
		STNBFontMetricsIRef			getFontMetricsIRef(const float defFontSz);
		void						configureMetricsBuilder(STNBTextMetricsBuilder* mBuilder);
		//Metrics to VisualModel
		AUFuenteVisualModel*		newVisualModelForChars(const STNBTextMetrics* textMetrics, const UI32 firstChar, const UI32 countChars, STNBArray* dstBoxes, STNBFontRenderBox* dstBoxesCur, const UI32 maxBoxesCharsCount, AUArreglo* visualFilters);
		static void					destroyVisualModelData(void* data);
		//VisualModel to RenderModel
		static PTRfunCmdsGL			produceRenderModel(const void* data, const NBPropRenderizado &propsRenderizado, const NBMatriz matrizEscena, const NBColor colorMultiplicado, const STNBRangeI range, STBloqueGL* dstVertexBlock, UI8* dstVertexBlockType, AUArreglo* visualFilters);
		static void					consumeRenderModel(BYTE* punteroDatosModelo);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		//
		STNBFontLines*				_fuenteDef;
		const char*					_family;
		const char*					_subFamily;
		BOOL						_isDisabled;		//Font was disabled by Mngr
		UI8							_styleMask;
		float						_sceneFontSz;
		STNBFontMetrics				_sceneMetrics;
		float						_escalaEspaciado;
		//Metrics Itf
		static void 				retain_(void* obj);
		static void 				release_(void* obj);
		static UI32 				getShapeId_(void* obj, const UI32 unicode);
		static STNBFontMetrics 		getFontMetrics_(void* obj);
		static STNBFontMetrics 		getFontMetricsForSz_(void* obj, const float fontSz);
		static STNBFontShapeMetrics	getFontShapeMetrics_(void* obj, const UI32 shapeId);
		static STNBFontShapeMetrics	getFontShapeMetricsForSz_(void* obj, const UI32 shapeId, const float fontSz);
};

#endif
