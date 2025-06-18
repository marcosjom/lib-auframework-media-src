//
//  AUFuenteTextura.cpp
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUFuenteTextura.h"
#include "nb/core/NBEncoding.h"

typedef struct STFuenteTexturaVisualData_ {
	STNBArray	visualChars; //STFuenteVisualChar
	UI32		retainCount;
} STFuenteTexturaVisualData;

typedef enum ENFuenteTexturaRenderType_ {
	ENFuenteTexturaRenderType_Geom = 0,	//Geometrics/trinagles drawing
	ENFuenteTexturaRenderType_Tex, 		//Texture directly to viewport
	ENFuenteTexturaRenderType_Count,
} ENFuenteTexturaRenderType;

//Root header
typedef struct STFuenteTexturaRender_ {
	ENFuenteTexturaRenderType type;		//goemetric or texture-to-view-port?
	UI32		conteoGrpsTextura;
} STFuenteTexturaRender;

//First header (when geometrics is used)
typedef struct STFuenteTexturaRenderGeo_ {
	STBloqueGL	bloqueIndicesGL;
	STBloqueGL	bloqueVerticesGL;
} STFuenteTexturaRenderGeo;

//Second headers (when geometrics is used)
typedef struct STFuenteTexturaRenderGeoGrp_ {
	UI32		idTextura;
	UI32		iPrimerModelo;
	UI32		cantModelos;
} STFuenteTexturaRenderGeoGrp;

//Second headers (when texture-to-view-port is used)
typedef struct STFuenteTexturaRenderTexGrp_ {
	UI32		idTextura;
	UI32		cantModelos;
} STFuenteTexturaRenderTexGrp;

//Third headers (when texture-to-view-port is used)
typedef struct STFuenteTexturaRenderTexModel_ {
	STNBRectI16		texRect;	//texture rect
	STNBRect		viewRect;	//viewport rect
	STNBColor8		color;		//color
} STFuenteTexturaRenderTexModel;


AUFuenteTextura::AUFuenteTextura(STNBFontBitmaps* fuenteDef, const ENTexturaModoPintado modoPintado, const float escalaSubtexturas)
: AUFuenteRender()
, _mapasAtlas(this)
{
	_escalaSubtexturas	= escalaSubtexturas;
	_fuenteDef			= fuenteDef; if(fuenteDef != NULL) NBFontBitmaps_retain(fuenteDef);
	_modoPintado		= modoPintado;
	_isDisabled			= FALSE;
	{
		NBFontBitmaps_getStyle(_fuenteDef, &_family, &_subFamily, &_styleMask);
		_sceneFontSz	= NBFontBitmaps_getSize(_fuenteDef);
		_sceneMetrics	= NBFontBitmaps_getMetricsForSz(_fuenteDef, _sceneFontSz);
	}
}

AUFuenteTextura::~AUFuenteTextura(){
	if(_fuenteDef != NULL) NBFontBitmaps_release(_fuenteDef); _fuenteDef = NULL;
	this->privEmptyAtlases();
}

//

BOOL AUFuenteTextura::isDisabled(){
	return _isDisabled;
}

void AUFuenteTextura::setDisabled(const BOOL isDisabled){
	_isDisabled = isDisabled;
}

//

void AUFuenteTextura::privEmptyAtlases(){
	SI32 i; const SI32 conteo = _mapasAtlas.conteo;
	for(i = 0; i < conteo; i++){
		STFuenteTexturaAtlas* datos = _mapasAtlas.elemPtr(i);
		datos->idTexturaGL = 0;
		//Liberar subtexturas
		SI32 iTex; const SI32 conteoTex = datos->subtexturas->conteo;
		for(iTex = 0; iTex < conteoTex; iTex++){
			STFuenteTexturaAtlasTex* t  = datos->subtexturas->elemPtr(iTex);
			t->texture->liberar(NB_RETENEDOR_THIS);
			t->texture = NULL;
		}
		datos->subtexturas->liberar(NB_RETENEDOR_THIS);
		datos->subtexturas = NULL;
	}
	_mapasAtlas.vaciar();
}

//

STNBFontBitmaps* AUFuenteTextura::fuenteDef(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFuenteTextura::fuenteDef")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _fuenteDef;
}

const char* AUFuenteTextura::familia(){
	return _family;
}

const char* AUFuenteTextura::subfamilia(){
	return _subFamily;
}

bool AUFuenteTextura::esNegrilla(){
	return ((_styleMask & ENNBFontStyleBit_Bold) != 0);
}

bool AUFuenteTextura::esCursiva(){
	return ((_styleMask & ENNBFontStyleBit_Italic) != 0);
}

float AUFuenteTextura::tamanoEscena(){
	return _sceneFontSz;
}

float AUFuenteTextura::ascendenteEscena(){
	return _sceneMetrics.ascender;
}

float AUFuenteTextura::descendenteEscena(){
	return _sceneMetrics.descender;
}

float AUFuenteTextura::altoGuiaLineaEscena(){
	return _sceneMetrics.height;
}

float AUFuenteTextura::altoParaSiguienteLineaEscena(){
	return (_sceneMetrics.height - (_sceneMetrics.ascender - _sceneMetrics.descender));
}

//Preload font shapes

UI32 AUFuenteTextura::preloadCharShapes(const char* strChars){
	return this->preloadCharShapesBytes(strChars, NBString_strLenBytes(strChars));
}

UI32 AUFuenteTextura::preloadCharShapesBytes(const char* strChars, const UI32 strCharslen){
	UI32 r = 0;
	if(_fuenteDef != NULL){
		BOOL unicsInited = FALSE;
		STNBArray unics;
		NBMemory_setZeroSt(unics, STNBArray);
		{
			UI32 iByte = 0, charSz = 0, unicode = 0;
			while(iByte < strCharslen){
				charSz		= NBEncoding_utf8BytesExpected(strChars[iByte]);
				unicode		= NBEncoding_unicodeFromUtf8s(&strChars[iByte], charSz, 0);
				if(unicode <= 0){
					break;
				} else {
					if(!unicsInited){
						NBArray_init(&unics, sizeof(UI32), NULL);
						unicsInited = TRUE;
					}
					NBArray_addValue(&unics, unicode);
				}
				iByte += charSz;
			}
		}
		if(unicsInited){
			//Sync
			{
				const UI32* unicodes = (const UI32*)NBArray_data(&unics);
				r = NBFontBitmaps_syncBitmapShapesOfUnicodes(_fuenteDef, unicodes, unics.use);
			}
			//Release
			NBArray_release(&unics);
			unicsInited = FALSE;
		}
	}
	return r;
}

//

AUArregloNativoP<STFuenteTexturaAtlas>* AUFuenteTextura::mapasAtlas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFuenteTextura::mapasAtlas")
	AU_GESTOR_PILA_LLAMADAS_POP
	return &_mapasAtlas;
}

//

float AUFuenteTextura::escalaSubTexturas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFuenteTextura::escalaSubTexturas")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _escalaSubtexturas;
}

void AUFuenteTextura::ligarAtlasConTexturaGl(const SI32 iAtlas, const GLuint idTexturaGL){
	//Add atlases
	while(_mapasAtlas.conteo <= iAtlas){
		STFuenteTexturaAtlas a;
		a.idTexturaGL	= 0;
		a.subtexturas	= new(this->tipoMemoriaResidencia()) AUArregloNativoMutableP<STFuenteTexturaAtlasTex>();
		_mapasAtlas.agregarElemento(a);
	}
	//Link
	{
		NBASSERT(iAtlas >= 0 && iAtlas < _mapasAtlas.conteo)
		STFuenteTexturaAtlas* datAtlas = _mapasAtlas.elemPtr(iAtlas);
		datAtlas->idTexturaGL = idTexturaGL;
		SI32 i; const SI32 conteo = datAtlas->subtexturas->conteo;
		for(i = 0; i < conteo; i++){
			STFuenteTexturaAtlasTex* t = datAtlas->subtexturas->elemPtr(i);
			t->texture->relinkearTextura(idTexturaGL);
		}
	}
}

void AUFuenteTextura::updateFontDef(STNBFontBitmaps* fuenteDef, const ENTexturaModoPintado modoPintado, const float escalaSubtexturas){
	//
	if(fuenteDef != NULL) NBFontBitmaps_retain(fuenteDef);
	if(_fuenteDef != NULL) NBFontBitmaps_release(_fuenteDef);
	//
	_escalaSubtexturas	= escalaSubtexturas;
	_fuenteDef			= fuenteDef;
	_modoPintado		= modoPintado;
	{
		NBFontBitmaps_getStyle(_fuenteDef, &_family, &_subFamily, &_styleMask);
		_sceneFontSz	= NBFontBitmaps_getSize(_fuenteDef);
		_sceneMetrics	= NBFontBitmaps_getMetricsForSz(_fuenteDef, _sceneFontSz);
	}
	//Discard current atlases
	this->privEmptyAtlases();
}

STNBFontShapeMetrics AUFuenteTextura::propiedadesParaCaracter(const float tamFuentePx, const UI32 unicode32){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFuenteTextura::propiedadesParaCaracter")
	STNBFontShapeMetrics datos;
	NBMemory_setZeroSt(datos, STNBFontShapeMetrics);
	/*datos.metricasH.ascendente	= _ascendenteEscena;
	datos.metricasH.descendente	= _descententeEscena;
	datos.metricasH.altoParaSigLinea = _altoParaSigLineaEscena;
	datos.objFuente				= this;
	datos.objGrafico			= NULL;
	datos.objAtlas				= NULL;
	datos.iAreaEnAtlas			= -1;
	datos.areaEnAtlas.codigo	= 0; datos.areaEnAtlas.centroX = datos.areaEnAtlas.centroY = 0;
	datos.areaEnAtlas.margenDer = datos.areaEnAtlas.margenInf = datos.areaEnAtlas.margenIzq = datos.areaEnAtlas.margenSup = 0;
	datos.areaEnAtlas.area.x	= datos.areaEnAtlas.area.y = datos.areaEnAtlas.area.ancho = datos.areaEnAtlas.area.alto = 0;
	datos.metricasH.avance		= datos.metricasH.bearingIzq = 0;
	//
	STFuenteMapaBitsMapeo* mapeoGlyph = _fuenteDef->mapeoParaGlyph(unicode32);
	if(mapeoGlyph != NULL){
		NBASSERT(_mapasAtlas.conteo == _fuenteDef->mapasAtlas()->conteo)
		NBASSERT(mapeoGlyph->iAtlas < _mapasAtlas.conteo)
		NBASSERT(mapeoGlyph->iAtlas < _fuenteDef->mapasAtlas()->conteo)
		STFuenteTexturaAtlas* atlasTex = _mapasAtlas.elemPtr(mapeoGlyph->iAtlas);
		STFuenteMapaBitsAtlas* datosAtlas = _fuenteDef->mapasAtlas()->elemPtr(mapeoGlyph->iAtlas);
		//
		NBASSERT(atlasTex->subtexturas->conteo == datosAtlas->mapaAtlas->areasOcupadas()->conteo);
		NBASSERT(mapeoGlyph->iArea < atlasTex->subtexturas->conteo)
		NBASSERT(atlasTex->subtexturas->elem(mapeoGlyph->iArea)->esClase(AUTextura::idTipoClase))
		datos.objGrafico			= atlasTex->subtexturas->elem(mapeoGlyph->iArea); NBASSERT(datos.objGrafico->esClase(AUTextura::idTipoClase))
		datos.objAtlas				= datosAtlas->mapaAtlas; NBASSERT(datos.objAtlas->esClase(AUAtlasMapa::idTipoClase))
		datos.iAreaEnAtlas			= mapeoGlyph->iArea;
		//
		STFuenteLineasGlyphForma* formaGlyph = _fuenteDef->fuenteDef()->formaParaGlyph(tamFuentePx, unicode32, 0); NBASSERT(formaGlyph != NULL)
		datos.metricasH.avance		= (float)formaGlyph->avance * _escalaSubtexturas;
		datos.metricasH.bearingIzq	= (float)formaGlyph->hBearX * _escalaSubtexturas;
		NBASSERT(mapeoGlyph->idForma == formaGlyph->idForma)
		NBASSERT(datosAtlas->mapaAtlas->areasOcupadas()->elemPtr(mapeoGlyph->iArea)->codigo == formaGlyph->idForma)
		//
		const STAtlasArea* datosArea = &datosAtlas->mapaAtlas->areasOcupadas()->elemento[mapeoGlyph->iArea];
		datos.areaEnAtlas.codigo	= datosArea->codigo;
		datos.areaEnAtlas.area.x	= datosArea->area.x * _escalaSubtexturas;
		datos.areaEnAtlas.area.y	= datosArea->area.y * _escalaSubtexturas;
		datos.areaEnAtlas.area.ancho= datosArea->area.ancho * _escalaSubtexturas;
		datos.areaEnAtlas.area.alto	= datosArea->area.alto * _escalaSubtexturas;
		datos.areaEnAtlas.centroX	= datosArea->centroX * _escalaSubtexturas;
		datos.areaEnAtlas.centroY	= datosArea->centroY * _escalaSubtexturas;
		datos.areaEnAtlas.margenIzq	= datosArea->margenIzq * _escalaSubtexturas;
		datos.areaEnAtlas.margenDer	= datosArea->margenDer * _escalaSubtexturas;
		datos.areaEnAtlas.margenSup	= datosArea->margenSup * _escalaSubtexturas;
		datos.areaEnAtlas.margenInf	= datosArea->margenInf * _escalaSubtexturas;
	}*/
	AU_GESTOR_PILA_LLAMADAS_POP
	return datos;
}

float AUFuenteTextura::propiedadesKerning(const float tamFuentePx, const UI32 unicode32, const UI32 unicode32Anterior){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFuenteTextura::propiedadesKerning")
	const float kern = 0; //(float)_fuenteDef->propiedadesKerning(tamFuentePx, unicode32, unicode32Anterior);
	AU_GESTOR_PILA_LLAMADAS_POP
	return kern;
}

STNBFontShapeMetrics AUFuenteTextura::propiedadesParaCaracter(void* objBiblioteca, const float tamFuentePx, const UI32 unicode32){
	NBASSERT(((AUFuenteTextura*)objBiblioteca)->esClase("AUFuenteTextura") && ((AUFuenteTextura*)objBiblioteca)->esClase(AUFuenteTextura::idTipoClase));
	return ((AUFuenteTextura*)objBiblioteca)->propiedadesParaCaracter(tamFuentePx, unicode32);
}

float AUFuenteTextura::propiedadesKerning(void* objBiblioteca, const float tamFuentePx, const UI32 unicode32, const UI32 unicode32Anterior){
	NBASSERT(objBiblioteca != NULL)
	NBASSERT(((AUFuenteTextura*)objBiblioteca)->esClase("AUFuenteTextura") && ((AUFuenteTextura*)objBiblioteca)->esClase(AUFuenteTextura::idTipoClase));
	return ((AUFuenteTextura*)objBiblioteca)->propiedadesKerning(tamFuentePx, unicode32, unicode32Anterior);
}

//Set fontProperties to metricsBuilder

STNBFontMetricsIRef AUFuenteTextura::getFontMetricsIRef(const float defFontSz){
	STNBFontMetricsIRef r;
	NBMemory_setZeroSt(r, STNBFontMetricsIRef);
	r.itf.retain					= retain_;
	r.itf.release					= release_;
	r.itf.getNativeSz				= getNativeSz_;
	r.itf.getShapeId				= getShapeId_;
	r.itf.getFontMetrics			= getFontMetrics_;
	r.itf.getFontMetricsForSz		= getFontMetricsForSz_;
	r.itf.getFontShapeMetrics		= getFontShapeMetrics_;
	r.itf.getFontShapeMetricsForSz	= getFontShapeMetricsForSz_;
	r.itfParam						= this;
	r.fontSz						= (defFontSz <= 0 ? this->_sceneFontSz : defFontSz);
	return r;
}

void AUFuenteTextura::configureMetricsBuilder(STNBTextMetricsBuilder* mBuilder){
	STNBFontMetricsIRef itfRef = this->getFontMetricsIRef(0);
	NBTextMetricsBuilder_setFontItf(mBuilder, &itfRef.itf, itfRef.itfParam);
	NBTextMetricsBuilder_setFontSz(mBuilder, _sceneFontSz);
}

//Metrics to VisualModel
//false when isControlChar
bool AUFuenteTextura::charMetricToRenderChar(const STNBTextMetricsChar* src, STFuenteVisualChar* dst, const STNBTextMetricsLine* optLnSrc){
	bool r = false;
	if(src != NULL && dst != NULL){
		if(!src->isControl){
			STNBFontBitmapShape shape = NBFontBitmaps_getBitmapShape(_fuenteDef, src->shapeId, TRUE);
			if(shape.bitmap != NULL){
				//Add atlases
				while(_mapasAtlas.conteo <= shape.iBitmap){
					STFuenteTexturaAtlas a;
					a.idTexturaGL	= 0;
					a.subtexturas	= new(this->tipoMemoriaResidencia()) AUArregloNativoMutableP<STFuenteTexturaAtlasTex>();
					_mapasAtlas.agregarElemento(a);
				}
				//Add areas
				STFuenteTexturaAtlas* atlas = _mapasAtlas.elemPtr(shape.iBitmap);
				if(atlas->subtexturas->conteo <= shape.iArea){
					STNBFontBitmap bmp = NBFontBitmaps_getBitmapAtIndex(_fuenteDef, shape.iBitmap);
					const STNBBitmapProps bmpProps = NBBitmap_getProps(bmp.bitmap);
					NBASSERT(bmpProps.size.width > 0 && bmpProps.size.height > 0);
					while(atlas->subtexturas->conteo <= shape.iArea){
						NBASSERT(atlas->subtexturas->conteo < bmp.shapeIds->use)
						const UI32 shapeId = NBArray_itmValueAtIndex(bmp.shapeIds, UI32, atlas->subtexturas->conteo);
						const STNBFontBitmapShape shape = NBFontBitmaps_getBitmapShape(_fuenteDef, shapeId, FALSE);
						NBASSERT(shape.bitmap == bmp.bitmap)
						STFuenteTexturaAtlasTex t;
						t.shapeId = shapeId;
						t.texture = new(this) AUTextura(atlas->idTexturaGL, ENTexturaOrdenV_HaciaArriba, ENTexturaEstadoCarga_Cargada, bmpProps.size.width, bmpProps.size.height, shape.area.x, shape.area.y, shape.area.width, shape.area.height, 1/*datos->mapaAtlas->escalaBase2()*/, _escalaSubtexturas, _modoPintado, _family, NULL);
						atlas->subtexturas->agregarElemento(t);
					}
				}
				//Set return object
				{
					STFuenteTexturaAtlasTex* t = atlas->subtexturas->elemPtr(shape.iArea);
					NBASSERT(t->shapeId == src->shapeId)
					dst->texturaSprite		= t->texture;
					dst->areaSprite.x		= src->pos.x + shape.center.x;
					dst->areaSprite.y		= -src->pos.y - shape.center.y; //y-inverted
					dst->areaSprite.ancho	= shape.area.width;
					dst->areaSprite.alto	= -shape.area.height; //y-inverted
					dst->color				= src->color;
					if(optLnSrc == NULL){
						dst->lineTop		= -src->pos.y - shape.center.y;
						dst->lineBtm		= -src->pos.y - shape.center.y - shape.area.height;
						//NBASSERT(dst->lineBtm <= dst->lineTop) //Do not enable, not necesary after test
					} else {
						dst->lineTop		= -src->pos.y + optLnSrc->fontMetricsMax.ascender;
						dst->lineBtm		= -src->pos.y + optLnSrc->fontMetricsMax.descender; //Descender is negative
						//NBASSERT(dst->lineBtm <= dst->lineTop) //Do not enable, not necesary after test
					}
					r = true;
				}
			}
		}
	}
	return r;
}

AUFuenteVisualModel* AUFuenteTextura::newVisualModelForChars(const STNBTextMetrics* textMetrics, const UI32 firstChar, const UI32 countChars, STNBArray* dstBoxes, STNBFontRenderBox* dstBoxesCur, const UI32 maxBoxesCharsCount, AUArreglo* visualFilters){
	AUFuenteVisualModel* r = NULL;
	STFuenteTexturaVisualData* d = NBMemory_allocType(STFuenteTexturaVisualData);
	const BOOL boxesEnabled = (dstBoxes != NULL && dstBoxesCur != NULL);
	//Flush current box
	if(boxesEnabled){
		if(dstBoxesCur->rng.size > 0){
			NBArray_addValue(dstBoxes, *dstBoxesCur);
			//PRINTF_INFO("AUFuenteTextura closed group (%f, %f)-(+%f, +%f) (at start).\n", dstBoxesCur->box.xMin, dstBoxesCur->box.yMin, (dstBoxesCur->box.xMax - dstBoxesCur->box.xMin), (dstBoxesCur->box.yMax - dstBoxesCur->box.yMin));
			//Reset
			NBMemory_setZeroSt(*dstBoxesCur, STNBFontRenderBox);
		}
	}
	{
		const UI32 iFirstBox = dstBoxes->use;
		{
            const UI32 charAfterEnd = firstChar + countChars;
			SI32 lnCurIdx = -1; const STNBTextMetricsLine* lnCur = NULL; UI32 lnCharAfterEndIdx = 0;
			NBArray_init(&d->visualChars, sizeof(STFuenteVisualChar), NULL);
			UI32 i; for(i = firstChar; i < charAfterEnd; i++){
				//Load line
				{
					if(lnCur != NULL){
						if(lnCharAfterEndIdx >= i){
							lnCur = NULL;
						}
					}
					while(lnCur == NULL){
						lnCurIdx++;
						if(lnCurIdx < textMetrics->lines.use){
							const STNBTextMetricsLine* ln = NBArray_itmPtrAtIndex(&textMetrics->lines, STNBTextMetricsLine, lnCurIdx);
							if(ln->rngWords.start < textMetrics->words.use && ln->rngWords.afterEnd <= textMetrics->words.use){
								const STNBTextMetricsWord* w0 = NBArray_itmPtrAtIndex(&textMetrics->words, STNBTextMetricsWord, ln->rngWords.start);
								const STNBTextMetricsWord* w1 = NBArray_itmPtrAtIndex(&textMetrics->words, STNBTextMetricsWord, ln->rngWords.afterEnd - 1);
								if(w0->rngChars.start <= i && i < w1->rngChars.afterEnd){
									lnCur = ln;
								}
							}
						} else {
							break;
						}
					}
				}
				//Load char
				{
					const STNBTextMetricsChar* c = NBArray_itmPtrAtIndex(&textMetrics->chars, STNBTextMetricsChar, i);
					STFuenteVisualChar rc;
					NBMemory_setZeroSt(rc, STFuenteVisualChar);
					if(this->charMetricToRenderChar(c, &rc, lnCur)){ //false when isControlChar
						NBArray_addValue(&d->visualChars, rc);
						//Apply to group
						if(boxesEnabled){
							const float xx = rc.areaSprite.x + rc.areaSprite.ancho;
							const float yy = rc.areaSprite.y + rc.areaSprite.alto;
							//Inflate current group box
							if(dstBoxesCur->rng.size <= 0){
								NBAABox_wrapFirstPoint(&dstBoxesCur->box, NBST_P(STNBPoint, rc.areaSprite.x, rc.areaSprite.y));
								NBAABox_wrapNextPoint(&dstBoxesCur->box, NBST_P(STNBPoint, xx, yy));
							} else {
								NBAABox_wrapNextPoint(&dstBoxesCur->box, NBST_P(STNBPoint, rc.areaSprite.x, rc.areaSprite.y));
								NBAABox_wrapNextPoint(&dstBoxesCur->box, NBST_P(STNBPoint, xx, yy));
							}
                            if(dstBoxesCur->rng.size == 0){
                                dstBoxesCur->rngChars.start = i;
                            }
                            dstBoxesCur->rngChars.size = i - dstBoxesCur->rngChars.start + 1;
							dstBoxesCur->rng.size++;
							//Start new group
							if(dstBoxesCur->rng.size >= maxBoxesCharsCount){
								NBArray_addValue(dstBoxes, *dstBoxesCur);
								//PRINTF_INFO("AUFuenteTextura closed group (%f, %f)-(+%f, +%f).\n", dstBoxesCur->box.xMin, dstBoxesCur->box.yMin, (dstBoxesCur->box.xMax - dstBoxesCur->box.xMin), (dstBoxesCur->box.yMax - dstBoxesCur->box.yMin));
								//Reset
								NBMemory_setZeroSt(*dstBoxesCur, STNBFontRenderBox);
								dstBoxesCur->rng.start	= d->visualChars.use;
								dstBoxesCur->rng.size	= 0;
							}
						}
					}
				}
			}
			//Flush group
			if(boxesEnabled){
				if(dstBoxesCur->rng.size > 0){
					NBArray_addValue(dstBoxes, *dstBoxesCur);
					//PRINTF_INFO("AUFuenteTextura closed group (%f, %f)-(+%f, +%f) (at end).\n", dstBoxesCur->box.xMin, dstBoxesCur->box.yMin, (dstBoxesCur->box.xMax - dstBoxesCur->box.xMin), (dstBoxesCur->box.yMax - dstBoxesCur->box.yMin));
					//Reset
					NBMemory_setZeroSt(*dstBoxesCur, STNBFontRenderBox);
				}
			}
		}
		//Create model
		{
			r = new(this) AUFuenteVisualModel(this, d, AUFuenteTextura::produceRenderModel, AUFuenteTextura::destroyVisualModelData, visualFilters);
		}
		//Set new model to the new boxes
		if(boxesEnabled){
			SI32 i; for(i = iFirstBox; i < dstBoxes->use; i++){
				STNBFontRenderBox* box = NBArray_itmPtrAtIndex(dstBoxes, STNBFontRenderBox, i);
				NBASSERT(box->model == NULL)
				box->model = r;
			}
		}
	}
	return r;
}

void AUFuenteTextura::destroyVisualModelData(void* data){
	if(data != NULL){
		STFuenteTexturaVisualData* d = (STFuenteTexturaVisualData*)data;
		{
			NBArray_empty(&d->visualChars);
			NBArray_release(&d->visualChars);
		}
		//
		NBMemory_free(d);
	}
}

//VisualModel to RenderModel

PTRfunCmdsGL AUFuenteTextura::produceRenderModel(const void* data, const NBPropRenderizado &propsRenderizado, const NBMatriz matrizEscena, const NBColor colorMultiplicado, const STNBRangeI range, STBloqueGL* dstVertexBlock, UI8* dstVertexBlockType, AUArreglo* visualFilters){
	PTRfunCmdsGL r = NULL;
	//Produce renderObj from visualObj
	NBASSERT(data != NULL)
	if(data != NULL){
		STFuenteTexturaVisualData* d	= (STFuenteTexturaVisualData*)data;
		STNBArray* vChars				= &d->visualChars;
		SI32 iStart, iEnfAfter;
		if(range.size < 0){
			iStart		= 0;
			iEnfAfter	= vChars->use;
		} else {
			iStart		= range.start;
			iEnfAfter	= range.start + range.size;
			if(iStart < 0) iStart = 0;
			if(iStart > vChars->use) iStart = vChars->use;
			if(iEnfAfter > vChars->use) iEnfAfter = vChars->use;
		}
		//Create models
		if(iStart < iEnfAfter){
			BOOL useDrawTexExt = FALSE;
			//Analyze if drawTexture extension can be used
			if(!NBMATRIZ_IS_ROTATED(matrizEscena)){
				if(NBGestorGL::soportaDrawTexture()){
					useDrawTexExt = TRUE;
				}
			}
			if(useDrawTexExt){
				//-----------------
				//- DrawTex model
				//-----------------
				UI32 grpTexTotal		= 0;
				const UI16 cr			= 255.f * colorMultiplicado.r;
				const UI16 cg			= 255.f * colorMultiplicado.g;
				const UI16 cb			= 255.f * colorMultiplicado.b;
				const UI16 ca			= 255.f * colorMultiplicado.a;
				const SI32 iDataRndr	= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STFuenteTexturaRender));
				const STNBSize scnUnitsPerPx	= {
					(propsRenderizado.cajaProyeccion.xMax - propsRenderizado.cajaProyeccion.xMin) / propsRenderizado.puertoDeVision.ancho
					, (propsRenderizado.cajaProyeccion.yMax - propsRenderizado.cajaProyeccion.yMin) / propsRenderizado.puertoDeVision.alto
				};
				const BOOL matrixIsStreching = NBMATRIZ_IS_STRECHING(matrizEscena);
				//Process groups
				{
					UI32 grpTexConteo	= 0;
					UI32 grpTexIdAnt	= NBArray_itmPtrAtIndex(vChars, STFuenteVisualChar, 0)->texturaSprite->idTexturaGL;
					SI32 iDataGrpCur	= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STFuenteTexturaRenderTexGrp));
					SI32 i; for(i = iStart; i < iEnfAfter; i++){
						const STFuenteVisualChar* datosChar = NBArray_itmPtrAtIndex(vChars, STFuenteVisualChar, i);
						NBASSERT(datosChar->texturaSprite != NULL)
						STNBColor8 color = {
							(UI8)(((UI16)datosChar->color.r * cr) / 255)
							, (UI8)(((UI16)datosChar->color.g * cg) / 255)
							, (UI8)(((UI16)datosChar->color.b * cb) / 255)
							, (UI8)(((UI16)datosChar->color.a * ca) / 255)
						};
						//PRINTF_INFO("Color char(%d, %d, %d, %d) mutiplied(%d, %d, %d, %d).\n", datosChar->color.r, datosChar->color.g, datosChar->color.b, datosChar->color.a, color.r, color.g, color.b, color.a);
						//Apply filters
						if(visualFilters != NULL){
							const SI32 filtersCount = visualFilters->conteo;
							if(filtersCount > 0){
								//Apply filters
								AUTextoFiltro** filters = (AUTextoFiltro**)visualFilters->elemPtr(0);
								NBASSERT(filters[0]->esClase(AUTextoFiltro::idTipoClase))
								SI32 i; for(i = 0; i < filtersCount; i++){
									STNBColor8 cF	= filters[i]->colorMultForCharRect(datosChar->areaSprite.x, datosChar->lineBtm, datosChar->areaSprite.ancho, (datosChar->lineTop - datosChar->lineBtm));
									color.r	= ((SI32)color.r * (SI32)cF.r) / 255;
									color.g	= ((SI32)color.g * (SI32)cF.g) / 255;
									color.b	= ((SI32)color.b * (SI32)cF.b) / 255;
									color.a	= ((SI32)color.a * (SI32)cF.a) / 255;
									//PRINTF_INFO("Color filtered(%d, %d, %d, %d).\n", color.r, color.g, color.b, color.a);
								}
							}
						}
						//Add model
						{
							SI32 iDataModel	= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STFuenteTexturaRenderTexModel));
							{
								STFuenteTexturaRenderTexModel* model = (STFuenteTexturaRenderTexModel*)propsRenderizado.bytesDatosModelos->elemPtr(iDataModel);
								//Apply color
								{
									model->color = color;
								}
								//Apply texCords
								{
									const NBRectanguloP<UI16> texRect = datosChar->texturaSprite->areaEnAtlas();
									model->texRect.x		= texRect.x;
									model->texRect.y		= texRect.y;
									model->texRect.width	= texRect.ancho;
									model->texRect.height	= texRect.alto;
								}
								//Apply coords
								{
									NBPunto pMinLcl, pMaxLcl, pMinScn, pMaxScn;
									//Local coords
									{
										pMinLcl.x	= datosChar->areaSprite.x;
										pMinLcl.y	= datosChar->areaSprite.y;
										pMaxLcl.x	= pMinLcl.x + datosChar->areaSprite.ancho;
										pMaxLcl.y	= pMinLcl.y + datosChar->areaSprite.alto;
									}
									//Scene coords
									{
										NBMATRIZ_MULTIPLICAR_PUNTO(pMinScn, matrizEscena, pMinLcl.x, pMinLcl.y);
										NBMATRIZ_MULTIPLICAR_PUNTO(pMaxScn, matrizEscena, pMaxLcl.x, pMaxLcl.y);
										//Apply
										if(pMinScn.x <= pMaxScn.x){
											model->viewRect.x		= (pMinScn.x - propsRenderizado.cajaProyeccion.xMin) / scnUnitsPerPx.width;
											model->viewRect.width	= (pMaxScn.x - pMinScn.x) / scnUnitsPerPx.width;
										} else {
											model->viewRect.x		= (pMaxScn.x - propsRenderizado.cajaProyeccion.xMin) / scnUnitsPerPx.width;
											model->viewRect.width	= (pMinScn.x - pMaxScn.x) / scnUnitsPerPx.width;
											//Invert texture rect
											model->texRect.x		+= model->texRect.width;
											model->texRect.width	= -model->texRect.width;
										}
										if(pMinScn.y <= pMaxScn.y){
											model->viewRect.y		= (pMinScn.y - propsRenderizado.cajaProyeccion.yMin) / scnUnitsPerPx.height; //y-inverted
											model->viewRect.height	= (pMaxScn.y - pMinScn.y) / scnUnitsPerPx.height;
										} else {
											model->viewRect.y		= (pMaxScn.y - propsRenderizado.cajaProyeccion.yMin) / scnUnitsPerPx.height; //y-inverted
											model->viewRect.height	= (pMinScn.y - pMaxScn.y) / scnUnitsPerPx.height;
											//Invert texture rect
											model->texRect.y		+= model->texRect.height;
											model->texRect.height	= -model->texRect.height;
										}
										//Pixel precission
										if(!matrixIsStreching){
											model->viewRect.x		= (float)((SI32)model->viewRect.x);
											model->viewRect.y		= (float)((SI32)model->viewRect.y);
											NBASSERT(model->viewRect.width >= 0.0f)		//Must be positive
											NBASSERT(model->viewRect.height >= 0.0f)	//Must be positive
											{
												const float xMax	= (model->viewRect.x + model->viewRect.width);
												const float yMax	= (model->viewRect.y + model->viewRect.height);
												const float xMaxI	= (float)((SI32)xMax);
												const float yMaxI	= (float)((SI32)yMax);
												//if(xMax != xMaxI || yMax != yMaxI){
												//	PRINTF_INFO("...\n");
												//}
												model->viewRect.width	= xMaxI + ((xMax - xMaxI) < 0.5f ? 0.0f : 1.0f) - model->viewRect.x;
												model->viewRect.height	= yMaxI + ((yMax - yMaxI) < 0.5f ? 0.0f : 1.0f) - model->viewRect.y;
												NBASSERT(model->viewRect.width == (float)((SI32)model->viewRect.width))
												NBASSERT(model->viewRect.height == (float)((SI32)model->viewRect.height))
											}
										}
									}
									NBASSERT(model->viewRect.width >= 0.0f)		//Must be positive
									NBASSERT(model->viewRect.height >= 0.0f)	//Must be positive
								}
							}
						}
						//Verificar cambio de grupo-textura
						if(grpTexIdAnt != datosChar->texturaSprite->idTexturaGL){
							NBASSERT(grpTexConteo > 0)
							NBASSERT(propsRenderizado.bytesDatosModelos->conteo == (iDataGrpCur + sizeof(STFuenteTexturaRenderTexGrp) + (sizeof(STFuenteTexturaRenderTexModel) * grpTexConteo)));
							//Update group header
							{
								STFuenteTexturaRenderTexGrp* grpCur = (STFuenteTexturaRenderTexGrp*)propsRenderizado.bytesDatosModelos->elemPtr(iDataGrpCur);
								grpCur->idTextura	= grpTexIdAnt;
								grpCur->cantModelos	= grpTexConteo;
							}
							//Open new group
							{
								iDataGrpCur	= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STFuenteTexturaRenderTexGrp));
							}
							//
							grpTexTotal++;
							grpTexIdAnt		= datosChar->texturaSprite->idTexturaGL;
							grpTexConteo	= 0;
						}
						grpTexConteo++;
					}
					//Update last group header
					if(grpTexConteo > 0){
						NBASSERT(grpTexConteo > 0)
						NBASSERT(propsRenderizado.bytesDatosModelos->conteo == (iDataGrpCur + sizeof(STFuenteTexturaRenderTexGrp) + (sizeof(STFuenteTexturaRenderTexModel) * grpTexConteo)));
						grpTexTotal++;
						{
							STFuenteTexturaRenderTexGrp* grpCur = (STFuenteTexturaRenderTexGrp*)propsRenderizado.bytesDatosModelos->elemPtr(iDataGrpCur);
							grpCur->idTextura	= grpTexIdAnt;
							grpCur->cantModelos	= grpTexConteo;
						}
					}
				}
				//Update header
				{
					STFuenteTexturaRender* dataRndr	= (STFuenteTexturaRender*)propsRenderizado.bytesDatosModelos->elemPtr(iDataRndr);
					dataRndr->type 					= ENFuenteTexturaRenderType_Tex;
					dataRndr->conteoGrpsTextura		= grpTexTotal;
				}
			} else {
				//-----------------
				//- Geometric model
				//-----------------
				const SI32 countModels = (iEnfAfter - iStart);
				const UI16 cr = 255.f * colorMultiplicado.r;
				const UI16 cg = 255.f * colorMultiplicado.g;
				const UI16 cb = 255.f * colorMultiplicado.b;
				const UI16 ca = 255.f * colorMultiplicado.a;
				const SI32 iDataRndr	= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STFuenteTexturaRender));
				const SI32 iDataGeo		= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STFuenteTexturaRenderGeo));
				STBloqueGL bloqueVerticesGL;
				bloqueVerticesGL.cantidadElementos	= (countModels * 4);
				bloqueVerticesGL.primerElemento		= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL.cantidadElementos);
				NBVerticeGL* verticesGL				= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]);
				UI32 grpTexTotal	= 0;
				UI32 grpTexConteo	= 0;
				UI32 grpTexIdAnt	= NBArray_itmPtrAtIndex(vChars, STFuenteVisualChar, 0)->texturaSprite->idTexturaGL;
				SI32 i; for(i = 0; i < countModels; i++){
					const STFuenteVisualChar* datosChar = NBArray_itmPtrAtIndex(vChars, STFuenteVisualChar, iStart + i);
					NBASSERT(datosChar->texturaSprite != NULL)
					STNBColor8 color = {
						(UI8)(((UI16)datosChar->color.r * cr) / 255)
						, (UI8)(((UI16)datosChar->color.g * cg) / 255)
						, (UI8)(((UI16)datosChar->color.b * cb) / 255)
						, (UI8)(((UI16)datosChar->color.a * ca) / 255)
					};
					//PRINTF_INFO("Color char(%d, %d, %d, %d) mutiplied(%d, %d, %d, %d).\n", datosChar->color.r, datosChar->color.g, datosChar->color.b, datosChar->color.a, color.r, color.g, color.b, color.a);
					//Apply filters
					if(visualFilters != NULL){
						const SI32 filtersCount = visualFilters->conteo;
						if(filtersCount > 0){
							//Apply filters
							AUTextoFiltro** filters = (AUTextoFiltro**)visualFilters->elemPtr(0);
							NBASSERT(filters[0]->esClase(AUTextoFiltro::idTipoClase))
							SI32 i; for(i = 0; i < filtersCount; i++){
								//STNBColor8 cF	= filters[i]->colorMultForCharRect(datosChar->areaSprite.x, datosChar->areaSprite.y, datosChar->areaSprite.ancho, datosChar->areaSprite.alto);
								STNBColor8 cF	= filters[i]->colorMultForCharRect(datosChar->areaSprite.x, datosChar->lineBtm, datosChar->areaSprite.ancho, (datosChar->lineTop - datosChar->lineBtm));
								color.r	= ((SI32)color.r * (SI32)cF.r) / 255;
								color.g	= ((SI32)color.g * (SI32)cF.g) / 255;
								color.b	= ((SI32)color.b * (SI32)cF.b) / 255;
								color.a	= ((SI32)color.a * (SI32)cF.a) / 255;
								//PRINTF_INFO("Color filtered(%d, %d, %d, %d).\n", color.r, color.g, color.b, color.a);
							}
						}
					}
					NBPunto pMin; NBPUNTO_ESTABLECER(pMin, datosChar->areaSprite.x, datosChar->areaSprite.y)
					NBPunto pMax; NBPUNTO_ESTABLECER(pMax, datosChar->areaSprite.x + datosChar->areaSprite.ancho, datosChar->areaSprite.y + datosChar->areaSprite.alto)
					NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], matrizEscena, pMin.x, pMin.y);
					NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[1], matrizEscena, pMax.x, pMin.y);
					NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[2], matrizEscena, pMin.x, pMax.y);
					NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[3], matrizEscena, pMax.x, pMax.y);
					NBCOLOR_ESTABLECER(verticesGL[0], color.r, color.g, color.b, color.a)
					NBCOLOR_ESTABLECER(verticesGL[1], color.r, color.g, color.b, color.a)
					NBCOLOR_ESTABLECER(verticesGL[2], color.r, color.g, color.b, color.a)
					NBCOLOR_ESTABLECER(verticesGL[3], color.r, color.g, color.b, color.a)
					datosChar->texturaSprite->cargarCoordenadasTextura(verticesGL);
					verticesGL		+= 4;
					//Verificar cambio de grupo-textura
					if(grpTexIdAnt != datosChar->texturaSprite->idTexturaGL){
						NBASSERT(grpTexConteo > 0)
						NBASSERT((sizeof(STFuenteTexturaRenderGeoGrp) % 4) == 0)
						const SI32 indiceDatosGrpTex = propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STFuenteTexturaRenderGeoGrp));
						STFuenteTexturaRenderGeoGrp* grpTex = (STFuenteTexturaRenderGeoGrp*)propsRenderizado.bytesDatosModelos->elemPtr(indiceDatosGrpTex); NBASSERT((BYTE*)grpTex == propsRenderizado.bytesDatosModelos->elemPtr(iDataRndr + sizeof(STFuenteTexturaRender) + (sizeof(STFuenteTexturaRenderGeoGrp) * grpTexTotal)))
						grpTex->idTextura		= grpTexIdAnt;
						grpTex->iPrimerModelo	= (i - grpTexConteo);
						grpTex->cantModelos		= grpTexConteo;
						grpTexTotal++;
						//
						grpTexIdAnt = datosChar->texturaSprite->idTexturaGL;
						grpTexConteo = 0;
					}
					grpTexConteo++;
				}
				//Depositar grupo-textura restante
				NBASSERT(grpTexConteo > 0)
				NBASSERT((sizeof(STFuenteTexturaRenderGeoGrp) % 4) == 0)
				const SI32 indiceDatosGrpTex	= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STFuenteTexturaRenderGeoGrp));
				STFuenteTexturaRenderGeoGrp* grpTex	= (STFuenteTexturaRenderGeoGrp*)propsRenderizado.bytesDatosModelos->elemPtr(indiceDatosGrpTex); NBASSERT((BYTE*)grpTex == propsRenderizado.bytesDatosModelos->elemPtr(iDataRndr + sizeof(STFuenteTexturaRender) + sizeof(STFuenteTexturaRenderGeo) + (sizeof(STFuenteTexturaRenderGeoGrp) * grpTexTotal)))
				grpTex->idTextura		= grpTexIdAnt;
				grpTex->iPrimerModelo	= (i - grpTexConteo);
				grpTex->cantModelos		= grpTexConteo;
				grpTexTotal++;
				//Pixel precision
				if((NBMATRIZ_ELEM00(matrizEscena) == -1 || NBMATRIZ_ELEM00(matrizEscena) == 0 || NBMATRIZ_ELEM00(matrizEscena) == 1)
				   && (NBMATRIZ_ELEM01(matrizEscena) == -1 || NBMATRIZ_ELEM01(matrizEscena) == 0 || NBMATRIZ_ELEM01(matrizEscena) == 1)
				   && (NBMATRIZ_ELEM10(matrizEscena) == -1 || NBMATRIZ_ELEM10(matrizEscena) == 0 || NBMATRIZ_ELEM10(matrizEscena) == 1)
				   && (NBMATRIZ_ELEM11(matrizEscena) == -1 || NBMATRIZ_ELEM11(matrizEscena) == 0 || NBMATRIZ_ELEM11(matrizEscena) == 1)){
					NBVerticeGL* verticesGL				= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]);
					const NBVerticeGL* verticesGLSigUlt	= verticesGL + bloqueVerticesGL.cantidadElementos;
					NBTamano scnUnitsPerPx;
					scnUnitsPerPx.ancho	= (propsRenderizado.cajaProyeccion.xMax - propsRenderizado.cajaProyeccion.xMin) / propsRenderizado.puertoDeVision.ancho;
					scnUnitsPerPx.alto		= (propsRenderizado.cajaProyeccion.yMax - propsRenderizado.cajaProyeccion.yMin) / propsRenderizado.puertoDeVision.alto;
					while(verticesGL < verticesGLSigUlt){
						NBTamano sobranteDePixel;
						sobranteDePixel.ancho		= (verticesGL[0].x - propsRenderizado.cajaProyeccion.xMin) / scnUnitsPerPx.ancho;
						sobranteDePixel.alto		= (verticesGL[0].y - propsRenderizado.cajaProyeccion.yMin) / scnUnitsPerPx.alto;
						sobranteDePixel.ancho		-= (float)((SI32)sobranteDePixel.ancho);
						sobranteDePixel.alto		-= (float)((SI32)sobranteDePixel.alto);
						//
						if(sobranteDePixel.ancho != 0.0f || sobranteDePixel.alto != 0.0f){
							verticesGL[0].x -= sobranteDePixel.ancho; verticesGL[0].y -= sobranteDePixel.alto;
							verticesGL[1].x -= sobranteDePixel.ancho; verticesGL[1].y -= sobranteDePixel.alto;
							verticesGL[2].x -= sobranteDePixel.ancho; verticesGL[2].y -= sobranteDePixel.alto;
							verticesGL[3].x -= sobranteDePixel.ancho; verticesGL[3].y -= sobranteDePixel.alto;
						}
						verticesGL += 4;
					}
				}
				//
				NBASSERT(verticesGL==&((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento + bloqueVerticesGL.cantidadElementos]))
				//Datos render (encabezado)
				{
					STFuenteTexturaRender* dataRndr		= (STFuenteTexturaRender*)propsRenderizado.bytesDatosModelos->elemPtr(iDataRndr);
					STFuenteTexturaRenderGeo* dataGeo	= (STFuenteTexturaRenderGeo*)propsRenderizado.bytesDatosModelos->elemPtr(iDataGeo);
					dataRndr->type						= ENFuenteTexturaRenderType_Geom;
					dataRndr->conteoGrpsTextura			= grpTexTotal;
					dataGeo->bloqueIndicesGL			= NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_4(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL, countModels);
					dataGeo->bloqueVerticesGL			= bloqueVerticesGL;
					if(dstVertexBlock != NULL) *dstVertexBlock = bloqueVerticesGL;
					if(dstVertexBlockType != NULL) *dstVertexBlockType = 1;
				}
			}
			r = AUFuenteTextura::consumeRenderModel;
		}
	}
	return r;
}

void AUFuenteTextura::consumeRenderModel(BYTE* punteroDatosModelo){
	UI8* ptrEnDatos = punteroDatosModelo;
	if(ptrEnDatos != NULL){
		const STFuenteTexturaRender* dataRndr = (STFuenteTexturaRender*)ptrEnDatos; ptrEnDatos += sizeof(STFuenteTexturaRender);
		if(dataRndr->conteoGrpsTextura > 0){
			switch(dataRndr->type) {
				case ENFuenteTexturaRenderType_Tex:
					{
						NBGestorGL::flushCacheLocal();
						{
							const NBColor colorBefore = NBGestorGL::colorActivo();
							SI32 i; for(i = 0; i < dataRndr->conteoGrpsTextura; i++){
								const STFuenteTexturaRenderTexGrp* grp = (STFuenteTexturaRenderTexGrp*)ptrEnDatos; ptrEnDatos += sizeof(STFuenteTexturaRenderTexGrp);
								NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUFuenteRender")
								NBGestorGL::bindTexture(0, grp->idTextura);
								{
									SI32 i; for(i = 0; i < grp->cantModelos; i++){
										const STFuenteTexturaRenderTexModel* mdl = (STFuenteTexturaRenderTexModel*)ptrEnDatos; ptrEnDatos += sizeof(STFuenteTexturaRenderTexModel);
										NBGestorGL::color4f((float)mdl->color.r / 255.0f, (float)mdl->color.g / 255.0f, (float)mdl->color.b / 255.0f, (float)mdl->color.a / 255.0f);
										NBGestorGL::setTextureCropRect(&mdl->texRect);
										NBGestorGL::drawTex(mdl->viewRect.x, mdl->viewRect.y, 0.0f, mdl->viewRect.width, mdl->viewRect.height);
									}
								}
							}
							NBGestorGL::color4f(colorBefore.r, colorBefore.g, colorBefore.b, colorBefore.a);
						}
					}
					break;
				case ENFuenteTexturaRenderType_Geom:
					{
						const STFuenteTexturaRenderGeo* dataGeo 	= (STFuenteTexturaRenderGeo*)ptrEnDatos;	ptrEnDatos += sizeof(STFuenteTexturaRenderGeo);
						const STFuenteTexturaRenderGeoGrp* arrGrps	= (STFuenteTexturaRenderGeoGrp*)ptrEnDatos;	ptrEnDatos += sizeof(STFuenteTexturaRenderGeoGrp) * dataRndr->conteoGrpsTextura;
						NBASSERT(dataGeo->bloqueIndicesGL.cantidadElementos > 0)
						NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUFuenteRender")
						UI32 i; const UI32 conteo = dataRndr->conteoGrpsTextura;
						for(i = 0; i < conteo; i++){
							NBGestorGL::bindTexture(0, arrGrps[i].idTextura);
							NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(dataGeo->bloqueIndicesGL.primerElemento + (arrGrps[i].iPrimerModelo * NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP), arrGrps[i].cantModelos * NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaTexto")
						}
					}
					break;
#				ifdef NB_CONFIG_INCLUDE_ASSERTS
				default:
					NBASSERT(FALSE) //unxepected ENFuenteTexturaRenderType
					break;
#				endif
			}
		}
	}
}

//Metrics Itf
void AUFuenteTextura::retain_(void* obj){
	NBASSERT(((AUFuenteTextura*)obj)->esClase(AUFuenteTextura::idTipoClase))
	((AUFuenteTextura*)obj)->retener(NB_RETENEDOR_NULL);
}
void AUFuenteTextura::release_(void* obj){
	NBASSERT(((AUFuenteTextura*)obj)->esClase(AUFuenteTextura::idTipoClase))
	((AUFuenteTextura*)obj)->liberar(NB_RETENEDOR_NULL);
}
float AUFuenteTextura::getNativeSz_(void* obj){
	NBASSERT(((AUFuenteTextura*)obj)->esClase(AUFuenteTextura::idTipoClase))
	return ((AUFuenteTextura*)obj)->_sceneFontSz;
}
UI32 AUFuenteTextura::getShapeId_(void* obj, const UI32 unicode){
	NBASSERT(((AUFuenteTextura*)obj)->esClase(AUFuenteTextura::idTipoClase))
	return NBFontBitmaps_getShapeId(((AUFuenteTextura*)obj)->_fuenteDef, unicode);
}
STNBFontMetrics AUFuenteTextura::getFontMetrics_(void* obj){
	NBASSERT(((AUFuenteTextura*)obj)->esClase(AUFuenteTextura::idTipoClase))
	return NBFontBitmaps_getMetrics(((AUFuenteTextura*)obj)->_fuenteDef);
}
STNBFontMetrics AUFuenteTextura::getFontMetricsForSz_(void* obj, const float fontSz){
	NBASSERT(((AUFuenteTextura*)obj)->esClase(AUFuenteTextura::idTipoClase))
	return NBFontBitmaps_getMetricsForSz(((AUFuenteTextura*)obj)->_fuenteDef, fontSz);
}
STNBFontShapeMetrics AUFuenteTextura::getFontShapeMetrics_(void* obj, const UI32 shapeId){
	NBASSERT(((AUFuenteTextura*)obj)->esClase(AUFuenteTextura::idTipoClase))
	return NBFontBitmaps_getShapeMetrics(((AUFuenteTextura*)obj)->_fuenteDef, shapeId);
}
STNBFontShapeMetrics AUFuenteTextura::getFontShapeMetricsForSz_(void* obj, const UI32 shapeId, const float fontSz){
	NBASSERT(((AUFuenteTextura*)obj)->esClase(AUFuenteTextura::idTipoClase))
	return NBFontBitmaps_getShapeMetricsForSz(((AUFuenteTextura*)obj)->_fuenteDef, shapeId, fontSz);
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUFuenteTextura, AUFuenteRender)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUFuenteTextura, "AUFuenteTextura", AUFuenteRender)
AUOBJMETODOS_CLONAR_NULL(AUFuenteTextura)




