//
//  AUFuenteTexturas.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUFUENTETEXTURA_H_INCLUIDO
#define AUFUENTETEXTURA_H_INCLUIDO

#include "AUFuenteRender.h"
#include "nb/fonts/NBFontMetrics.h"
#include "nb/fonts/NBFontBitmaps.h"
#include "AUAtlasMapa.h"
#include "AUTextura.h"
#include "NBPunto.h"

typedef struct STFuenteVisualChar_ {
	NBRectangulo	areaSprite;
	AUTextura*		texturaSprite;
	STNBColor8		color;
	float			lineTop;
	float			lineBtm;
} STFuenteVisualChar;

typedef struct STFuenteTexturaAtlasTex_ {
	UI32		shapeId;
	AUTextura*	texture;
} STFuenteTexturaAtlasTex;

typedef struct STFuenteTexturaAtlas_ {
	GLuint	idTexturaGL;
	AUArregloNativoMutableP<STFuenteTexturaAtlasTex>* subtexturas;
} STFuenteTexturaAtlas;

class AUFuenteTextura : public AUFuenteRender {
	public:
		AUFuenteTextura(STNBFontBitmaps* fuenteDef, const ENTexturaModoPintado modoPintado, const float escalaSubtexturas);
		virtual ~AUFuenteTextura();
		//
		BOOL						isDisabled();
		void						setDisabled(const BOOL isDisabled);
		//
		float						escalaSubTexturas();
		//Font properties
		STNBFontBitmaps*			fuenteDef();
		const char*					familia();
		const char*					subfamilia();
		bool						esNegrilla();
		bool 						esCursiva();
		float						tamanoEscena();
		float						ascendenteEscena();
		float						descendenteEscena();
		float						altoGuiaLineaEscena();
		float						altoParaSiguienteLineaEscena();
		//Preload font shapes
		UI32						preloadCharShapes(const char* strChars);
		UI32						preloadCharShapesBytes(const char* strChars, const UI32 strCharslen);
		//
		AUArregloNativoP<STFuenteTexturaAtlas>* mapasAtlas();
		void						ligarAtlasConTexturaGl(const SI32 iAtlas, const GLuint idTexturaGL);
		void						updateFontDef(STNBFontBitmaps* fuenteDef, const ENTexturaModoPintado modoPintado, const float escalaSubtexturas);
		//
		STNBFontShapeMetrics		propiedadesParaCaracter(const float tamFuentePx, const UI32 unicode32);
		float						propiedadesKerning(const float tamFuentePx, const UI32 unicode32, const UI32 unicode32Anterior);
		static STNBFontShapeMetrics propiedadesParaCaracter(void* objBiblioteca, const float tamFuentePx, const UI32 unicode32);
		static float				propiedadesKerning(void* objBiblioteca, const float tamFuentePx, const UI32 unicode32, const UI32 unicode32Anterior);
		//Set fontProperties to metricsBuilder
		STNBFontMetricsIRef			getFontMetricsIRef(const float defFontSz);
		void						configureMetricsBuilder(STNBTextMetricsBuilder* mBuilder);
		//Metrics to VisualModel
		bool						charMetricToRenderChar(const STNBTextMetricsChar* src, STFuenteVisualChar* dst, const STNBTextMetricsLine* optLnSrc); //false when isControlChar
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
		STNBFontBitmaps*			_fuenteDef;
		const char*					_family;
		const char*					_subFamily;
		UI8							_styleMask;
		BOOL						_isDisabled;
		float						_sceneFontSz;
		STNBFontMetrics				_sceneMetrics;
		float						_escalaSubtexturas;
		AUArregloNativoMutableP<STFuenteTexturaAtlas> _mapasAtlas; //Este arreglo debe corresponder con el de _fuenteDef->mapasAtlas().
		ENTexturaModoPintado _modoPintado;
		//
		void						privEmptyAtlases();
		//Metrics Itf
		static void 				retain_(void* obj);
		static void 				release_(void* obj);
		static float 				getNativeSz_(void* obj);
		static UI32 				getShapeId_(void* obj, const UI32 unicode);
		static STNBFontMetrics 		getFontMetrics_(void* obj);
		static STNBFontMetrics 		getFontMetricsForSz_(void* obj, const float fontSz);
		static STNBFontShapeMetrics	getFontShapeMetrics_(void* obj, const UI32 shapeId);
		static STNBFontShapeMetrics	getFontShapeMetricsForSz_(void* obj, const UI32 shapeId, const float fontSz);
};

#endif
