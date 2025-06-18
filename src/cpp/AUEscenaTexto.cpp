//
//  AUEscenaTexto.cpp
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaTexto.h"
//
#include "nb/fonts/NBTextMetricsBuilder.h"

#define NB_SCENE_TEXT_BOXES_GROUPS_CHARS_MAX	32

typedef struct STEscenaTextoRenderHead_ {
	UI32	grpsCount;
} STEscenaTextoRenderHead;

typedef struct STEscenaTextoRenderGrpHead_ {
	PTRfunCmdsGL	func;
	UI32			dataSz;
} STEscenaTextoRenderGrpHead;

AUEscenaTexto::AUEscenaTexto()
: AUEscenaObjeto()
, _defFont(NULL)
, _defCol(NBST_P(STNBRect,  0, 0, 0, 0 ))
, _defAllowMultiline(TRUE)
, _textDataSeqId(0)
, _textVisualFilters(NULL)
, _textVisuals(this)
, _textVisualsSeqId(0)
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTexto")
	NBText_init(&this->_textData);
	//Optimizations (chars boxes multi-levels)
	NBArray_init(&_textsBoxes, sizeof(STNBFontRenderBox), NULL);
	NBArray_init(&_textsBoxesLvls, sizeof(STNBRangeI), NULL);
}

AUEscenaTexto::AUEscenaTexto(AUFuenteRender* fuenteRender)
: AUEscenaObjeto()
, _defFont(NULL)
, _defCol(NBST_P(STNBRect,  0, 0, 0, 0 ))
, _defAllowMultiline(TRUE)
, _textDataSeqId(0)
, _textVisualFilters(NULL)
, _textVisuals(this)
, _textVisualsSeqId(0)
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTexto")
	NBText_init(&this->_textData);
	if(fuenteRender != NULL){
		NBText_setFont(&this->_textData, fuenteRender->getFontMetricsIRef(0));
		_defFont = fuenteRender;
		_defFont->retener(NB_RETENEDOR_THIS);
	}
	//Optimizations (chars boxes multi-levels)
	NBArray_init(&_textsBoxes, sizeof(STNBFontRenderBox), NULL);
	NBArray_init(&_textsBoxesLvls, sizeof(STNBRangeI), NULL);
}

AUEscenaTexto::AUEscenaTexto(AUEscenaTexto* otraInstancia)
: AUEscenaObjeto(otraInstancia)
, _defFont(NULL)
, _defCol(otraInstancia->_defCol)
, _defAllowMultiline(otraInstancia->allowMultiline())
, _textDataSeqId(otraInstancia->_textDataSeqId)
, _textVisualFilters(otraInstancia->_textVisualFilters)
, _textVisuals(this)
, _textVisualsSeqId(0)
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTexto")
	this->copiarPropiedadesDe(otraInstancia);
	NBText_initWithOther(&this->_textData, &otraInstancia->_textData);
	if(otraInstancia->_defFont != NULL){
		_defFont = otraInstancia->_defFont;
		_defFont->retener(NB_RETENEDOR_THIS);
	}
	//Optimizations (chars boxes multi-levels)
	NBArray_init(&_textsBoxes, sizeof(STNBFontRenderBox), NULL);
	NBArray_init(&_textsBoxesLvls, sizeof(STNBRangeI), NULL);
}

AUEscenaTexto::~AUEscenaTexto(){
	NBText_release(&this->_textData);
	if(_textVisualFilters != NULL) _textVisualFilters->liberar(NB_RETENEDOR_THIS); _textVisualFilters = NULL;
	if(_defFont != NULL) _defFont->liberar(NB_RETENEDOR_THIS); _defFont = NULL;
	//Optimizations (chars boxes multi-levels)
	NBArray_release(&_textsBoxes);
	NBArray_release(&_textsBoxesLvls);
}

//Sync data
void AUEscenaTexto::syncDataWithOther(const AUEscenaTexto* other){
	if(this != other){
		//Font
		{
			if(other->_defFont != NULL) other->_defFont->retener(NB_RETENEDOR_THIS);
			if(_defFont != NULL) _defFont->liberar(NB_RETENEDOR_THIS);
			_defFont	= other->_defFont;
		}
		//Behavior
		_defCol				= other->_defCol;
		_defAllowMultiline	= other->_defAllowMultiline;
		NBText_syncDataWithOther(&_textData, &other->_textData);
		_textDataSeqId		= other->_textDataSeqId;
		//Do not sync visuals
		//
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
}

//

ENNBTextLineAlignH AUEscenaTexto::alineacionH() const {
	return NBText_getDefAlignH(&this->_textData);
}

ENNBTextAlignV AUEscenaTexto::alineacionV() const {
	return NBText_getTextAlign(&this->_textData);
}

AUFuenteRender* AUEscenaTexto::fuenteRender() const {
	return _defFont;
}

float AUEscenaTexto::factorSeparacionCaracteres() const {
	return NBText_getDefCharsSeptFactor(&this->_textData);
}

float AUEscenaTexto::factorSeparacionLineas() const {
	return NBText_getDefLinesSeptFactor(&this->_textData);
}

const char* AUEscenaTexto::texto() const {
	return NBText_getText(&this->_textData, NULL);
}

UI32 AUEscenaTexto::textoLen() const {
	UI32 len = 0; NBText_getText(&this->_textData, &len);
	return len;
}

const STNBText* AUEscenaTexto::getVisualTextDef() const {
	return &this->_textData;
}

UI32 AUEscenaTexto::getCharDefsCount() const {
	return NBText_getCharsCount(&this->_textData);
}

UI32 AUEscenaTexto::getLinesCount() const {
	return NBText_getLinesCount(&this->_textData);
}

UI32 AUEscenaTexto::textoDataSeqId() const {
	return _textDataSeqId;
}

STNBRect AUEscenaTexto::columnLimit() const {
	return _defCol;
}

void AUEscenaTexto::setColumnLimit(const STNBRect defCol){
	_defCol = defCol;
}

BOOL AUEscenaTexto::allowMultiline() const {
	return _defAllowMultiline;
}

const STNBTextMetrics* AUEscenaTexto::textMetrics() const {
	return &_textData.metrics;
}

void AUEscenaTexto::establecerAlineacionH(ENNBTextLineAlignH alineacionH){
	NBText_setLineAlign(&_textData, alineacionH);
}

void AUEscenaTexto::establecerAlineacionV(ENNBTextAlignV alineacionV){
	NBText_setTextAlign(&_textData, alineacionV);
}

void AUEscenaTexto::establecerAlineaciones(ENNBTextLineAlignH alineacionH, ENNBTextAlignV alineacionV){
	NBText_setLineAlign(&_textData, alineacionH);
	NBText_setTextAlign(&_textData, alineacionV);
}

void AUEscenaTexto::establecerFactorSeparacionCaracteres(float factorSeparacionCaracteres){
	//ToDo: implement
	//NBText_setCharsSeptFactor(&_textData, factorSeparacionCaracteres);
}

void AUEscenaTexto::establecerFactorSeparacionLineas(float factorSeparacionLineas){
	//ToDo: implement
	//NBText_setLinesSeptFactor(&_textData, factorSeparacionLineas);
}

void AUEscenaTexto::establecerFuenteRender(AUFuenteRender* fuenteRender){
	//Retain new (first)
	if(fuenteRender != NULL){
		fuenteRender->retener(NB_RETENEDOR_THIS);
	}
	//Release old (then)
	if(this->_defFont != NULL){
		this->_defFont->liberar(NB_RETENEDOR_THIS);
	}
	//set
	this->_defFont = fuenteRender;
	if(fuenteRender != NULL){
		NBText_setFont(&this->_textData, fuenteRender->getFontMetricsIRef(0));
	} else {
		STNBFontMetricsIRef emptyRef; NBMemory_setZeroSt(emptyRef, STNBFontMetricsIRef);
		NBText_setFont(&this->_textData, emptyRef);
	}
}

void AUEscenaTexto::vaciarTexto(){
	//Empty
	NBText_empty(&this->_textData);
	_textDataSeqId++; //mark for visual sync
	//
	if(this->idEscena >= 0){
		this->setModelChangedFlag();
	}
}

void AUEscenaTexto::organizarTexto(const float width, const float height){
	this->privOrganizarTexto(this->privDefaultColumn(width, height), TRUE);
}

void AUEscenaTexto::organizarTextoEnCol(const STNBRect column, const BOOL allowMultiline){
	this->privOrganizarTexto(column, allowMultiline);
}

void AUEscenaTexto::privOrganizarTexto(const STNBRect column, const BOOL allowMultiline){
	//Organize
	NBText_organizeText(&this->_textData, column, allowMultiline);
	this->_defCol = column;
	_textDataSeqId++; //mark for visual sync
	//
	if(this->idEscena >= 0){
		this->setModelChangedFlag();
	}
}

//

STNBRect AUEscenaTexto::rectForCursor(const UI32 iChar){
	const STNBRect rr = NBTextMetrics_rectForCursor(&_textData.metrics, iChar);
	return NBST_P(STNBRect, rr.x, -rr.y - rr.height, rr.width, rr.height);
}

//Mono-format

STNBRect AUEscenaTexto::privDefaultColumn(const float width, const float height){
	const ENNBTextLineAlignH alignH = NBText_getDefAlignH(&this->_textData);
	const ENNBTextAlignV alignV = NBText_getTextAlign(&this->_textData);
	return AUEscenaTexto::privDefaultColumnForAligns(width, height, alignH, alignV);
}

STNBRect AUEscenaTexto::privDefaultColumnForAligns(const float width, const float height, const ENNBTextLineAlignH alignH, const ENNBTextAlignV alignV){
	return NBST_P(STNBRect
		, alignH == ENNBTextLineAlignH_Right ? -width : alignH == ENNBTextLineAlignH_Center ? width * -0.5f : 0.0f
		, alignV == ENNBTextAlignV_FromBottom ? -height : alignV == ENNBTextAlignV_Center ? height * -0.5f : 0.0f
		, width
		, height
	);
}

void AUEscenaTexto::establecerTexto(const char* texto, const float ancho, const float alto){
	this->establecerTextoEnCol(texto, this->privDefaultColumn(ancho, alto), TRUE);
}

void AUEscenaTexto::establecerTextoBytes(const char* texto, const UI32 textoLen, const float ancho, const float alto){
	this->establecerTextoBytesEnCol(texto, textoLen, this->privDefaultColumn(ancho, alto), TRUE);
}

void AUEscenaTexto::establecerTextoEnCol(const char* texto, const STNBRect column, const BOOL allowMultiline){
	NBText_setText(&this->_textData, column, allowMultiline, texto);
	this->_defCol = column;
	_textDataSeqId++; //mark for visual sync
	//
	if(this->idEscena >= 0){
		this->setModelChangedFlag();
	}
}

void AUEscenaTexto::establecerTextoBytesEnCol(const char* texto, const UI32 textoLen, const STNBRect column, const BOOL allowMultiline){
	NBText_setTextBytes(&this->_textData, column, allowMultiline, texto, textoLen);
	this->_defCol = column;
	_textDataSeqId++; //mark for visual sync
	//
	if(this->idEscena >= 0){
		this->setModelChangedFlag();
	}
}


void AUEscenaTexto::establecerTextoRepitiendo(const char* strRepetir, const SI32 veces, const float ancho, const float alto){
	this->establecerTextoRepitiendoEnCol(strRepetir, veces, this->privDefaultColumn(ancho, alto), TRUE);
}

void AUEscenaTexto::establecerTextoRepitiendoEnCol(const char* strRepetir, const SI32 veces, const STNBRect column, const BOOL allowMultiline){
	NBText_setTextRepeating(&this->_textData, column, allowMultiline, strRepetir, NBString_strLenBytes(strRepetir), veces);
	_textDataSeqId++; //mark for visual sync
	//
	if(this->idEscena >= 0){
		this->setModelChangedFlag();
	}
}

void AUEscenaTexto::establecerTexto(const STNBText* texto, const float ancho, const float alto){
	this->establecerTextoEnCol(texto, this->privDefaultColumn(ancho, alto), TRUE);
}

void AUEscenaTexto::establecerTextoEnCol(const STNBText* texto, const STNBRect column, const BOOL allowMultiline){
	NBText_syncDataWithOther(&this->_textData, texto);
	NBText_organizeText(&this->_textData, column, allowMultiline);
	this->_defCol = column;
	_textDataSeqId++; //mark for visual sync
	//
	if(this->idEscena >= 0){
		this->setModelChangedFlag();
	}
}

NBCajaAABB AUEscenaTexto::cajaParaTexto(const char* texto, const float ancho, const float alto){
	return this->cajaParaTextoEnCol(texto, this->privDefaultColumn(ancho, alto), TRUE);
}

NBCajaAABB AUEscenaTexto::cajaParaTextoEnCol(const char* texto, const STNBRect column, const BOOL allowMultiline){
	const STNBAABox box = NBText_boxForText(&this->_textData, column, allowMultiline, texto, NBString_strLenBytes(texto));
	NBCajaAABB r;
	r.xMin = box.xMin;
	r.xMax = box.xMax;
	r.yMin = box.yMin;
	r.yMax = box.yMax;
	return r;
}

NBCajaAABB AUEscenaTexto::cajaParaTexto(const char* texto, AUFuenteRender* objFuente, const ENNBTextLineAlignH alineacionH, const ENNBTextAlignV alineacionV, const float ancho, const float alto){
	return AUEscenaTexto::cajaParaTextoEnCol(texto, objFuente, alineacionH, alineacionV, AUEscenaTexto::privDefaultColumnForAligns(ancho, alto, alineacionH, alineacionV), TRUE);
}

NBCajaAABB AUEscenaTexto::cajaParaTextoEnCol(const char* texto, AUFuenteRender* objFuente, const ENNBTextLineAlignH alineacionH, const ENNBTextAlignV alineacionV, const STNBRect column, const BOOL allowMultiline){
	const STNBAABox box = NBText_boxForTextWithFont(column, allowMultiline, texto, NBString_strLenBytes(texto), objFuente->getFontMetricsIRef(0), 0, alineacionH, alineacionV);
	NBCajaAABB r;
	r.xMin = box.xMin;
	r.xMax = box.xMax;
	r.yMin = box.yMin;
	r.yMax = box.yMax;
	return r;
}

//Multi-format

void AUEscenaTexto::appendText(AUFuenteRender* font, const char* text){
	NBText_appendTextWithFont(&this->_textData, text, font->getFontMetricsIRef(0), 0);
}

void AUEscenaTexto::appendTextFinish(){
	NBText_organizeText(&this->_textData, this->_defCol, this->_defAllowMultiline);
	_textDataSeqId++; //mark for visual sync
	//
	if(this->idEscena >= 0){
		this->setModelChangedFlag();
	}
}

//

void AUEscenaTexto::pushVisualFilter(AUTextoFiltro* filter){
	if(filter != NULL){
		if(_textVisualFilters == NULL){
			_textVisualFilters = new(this) AUArregloMutable();
		}
		_textVisualFilters->agregarElemento(filter);
	}
}

void AUEscenaTexto::emptyVisualFilters(){
	if(_textVisualFilters != NULL){
		_textVisualFilters->vaciar();
		_textVisualFilters->liberar(NB_RETENEDOR_THIS);
		_textVisualFilters = NULL;
	}
}

//

NBCajaAABB AUEscenaTexto::boxForCurrentText(const BOOL withCursor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::boxForCurrentText")
	const STNBAABox box = NBText_metricsBox(&this->_textData, withCursor);
	NBCajaAABB r;
	r.xMin	= box.xMin;
	r.xMax	= box.xMax;
	r.yMax = -box.yMin; //From bitmap to scene coords
	r.yMin = -box.yMax; //From bitmap to scene coords
	NBASSERT(r.xMin <= r.xMax)
	NBASSERT(r.yMin <= r.yMax)
	/*if(withFilters){
		if(_textVisualFilters != NULL){
			const SI32 filtersCount = _textVisualFilters->conteo;
			if(filtersCount > 0){
				//Apply filters
				STNBAABox boxF;
				boxF.xMin = r.xMin;
				boxF.xMax = r.xMax;
				boxF.yMin = r.yMin;
				boxF.yMax = r.yMax;
				{
					AUTextoFiltro** filters = (AUTextoFiltro**)_textVisualFilters->elemPtr(0);
					NBASSERT(filters[0]->esClase(AUTextoFiltro::idTipoClase))
					SI32 i; for(i = 0; i < filtersCount; i++){
						boxF	= filters[i]->boxFiltered(boxF);
					}
				}
				r.xMin = boxF.xMin;
				r.xMax = boxF.xMax;
				r.yMin = boxF.yMin;
				r.yMax = boxF.yMax;
			}
		}
	}*/
	//PRINTF_INFO("AUEscenaTexto (%llu), box: x(%f, %f)-y(%f, %f).\n", (UI64)this, r.xMin, r.xMax, r.yMin, r.yMax);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

NBCajaAABB AUEscenaTexto::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::cajaAABBLocalCalculada")
	const NBCajaAABB r = this->boxForCurrentText(FALSE);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

void AUEscenaTexto::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaTexto::agregadoEnEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaTexto::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaTexto::quitandoDeEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//Sync metrics with visual representations

void AUEscenaTexto::privSyncVisualModels(){
	//Release previous visual models
	{
		_textVisuals.vaciar();
		//Optimizations (chars boxes multi-levels)
		NBArray_empty(&_textsBoxes);
		NBArray_empty(&_textsBoxesLvls);
	}
	//Create new models
	{
		const STNBTextMetrics* textMetrics = &this->_textData.metrics;
		if(textMetrics->chars.use > 0){
			STNBFontRenderBox curBox;
			NBMemory_setZeroSt(curBox, STNBFontRenderBox);
			{
				//Generate visual models
				const UI32 totalFormats = NBTextMetricsBuilder_getFormatsCount(&this->_textData.builder);
				if(totalFormats <= 1){
					//Create new model (optimization, only one font)
					STNBTextMetricsChar* c	= NBArray_itmPtrAtIndex(&textMetrics->chars, STNBTextMetricsChar, 0);
					AUFuenteRender* font	= (AUFuenteRender*)c->itfObj; NBASSERT(font->esClase(AUFuenteRender::idTipoClase));
					AUFuenteVisualModel* vm = font->newVisualModelForChars(textMetrics, 0, textMetrics->chars.use, &_textsBoxes, &curBox, NB_SCENE_TEXT_BOXES_GROUPS_CHARS_MAX, _textVisualFilters);
					if(vm != NULL){
						_textVisuals.agregarElemento(vm);
						vm->liberar(NB_RETENEDOR_THIS);
						//Flush current box
						if(curBox.rng.size > 0){
                            curBox.rngChars.start   = 0;
                            curBox.rngChars.size    = textMetrics->chars.use;
							NBArray_addValue(&_textsBoxes, curBox);
							//Reset
							NBMemory_setZeroSt(curBox, STNBFontRenderBox);
						}
					}
				} else {
					//Create new models (detecting font changes)
					STNBTextMetricsChar* c		= NBArray_itmPtrAtIndex(&textMetrics->chars, STNBTextMetricsChar, 0);
					AUFuenteRender* prevFont	= (AUFuenteRender*)c->itfObj; NBASSERT(prevFont->esClase(AUFuenteRender::idTipoClase));
                    UI32 prevFontCharStart      = c->iByte;
					UI32 prevFontStart			= 0;
					UI32 i; for(i = 1; i < textMetrics->chars.use; i++){
						STNBTextMetricsChar* c	= NBArray_itmPtrAtIndex(&textMetrics->chars, STNBTextMetricsChar, i);
						AUFuenteRender* font	= (AUFuenteRender*)c->itfObj; NBASSERT(font->esClase(AUFuenteRender::idTipoClase));
						if(prevFont != font){
							NBASSERT(prevFontStart < i)
							AUFuenteVisualModel* vm = prevFont->newVisualModelForChars(textMetrics, prevFontStart, (i - prevFontStart), &_textsBoxes, &curBox, NB_SCENE_TEXT_BOXES_GROUPS_CHARS_MAX, _textVisualFilters);
#                           ifdef NB_CONFIG_INCLUDE_ASSERTS
                            /*{
                                STNBString str;
                                NBString_initWithStrBytes(&str, &_textData.text.str[prevFontCharStart], (c->iByte - prevFontCharStart));
                                PRINTF_INFO("Visual Model for font (%d bytes) charRng(%d, +%d) boxRng(%d, +%d): -->%s<--\n", str.length, prevFontStart, (i - prevFontStart), curBox.rng.start, curBox.rng.size, str.str);
                                NBString_release(&str);
                            }*/
#                           endif
							if(vm != NULL){
								_textVisuals.agregarElemento(vm);
								vm->liberar(NB_RETENEDOR_THIS);
								//Flush current box
								if(curBox.rng.size > 0){
                                    curBox.rngChars.start   = prevFontStart;
                                    curBox.rngChars.size    = (i - prevFontStart);
									NBArray_addValue(&_textsBoxes, curBox);
									//Reset
									NBMemory_setZeroSt(curBox, STNBFontRenderBox);
								}
							}
							prevFont = font;
							prevFontStart = i;
                            prevFontCharStart = c->iByte;
						}
					}
					//Last font's model
					if(prevFontStart < textMetrics->chars.use){
						AUFuenteVisualModel* vm = prevFont->newVisualModelForChars(textMetrics, prevFontStart, (textMetrics->chars.use - prevFontStart), &_textsBoxes, &curBox, NB_SCENE_TEXT_BOXES_GROUPS_CHARS_MAX, _textVisualFilters);
#                       ifdef NB_CONFIG_INCLUDE_ASSERTS
                        /*{
                            STNBTextMetricsChar* c = NBArray_itmPtrAtIndex(&textMetrics->chars, STNBTextMetricsChar, textMetrics->chars.use - 1);
                            STNBString str;
                            NBString_initWithStrBytes(&str, &_textData.text.str[prevFontCharStart], (c->iByte + c->bytesLen - prevFontCharStart));
                            PRINTF_INFO("Visual Model for last-font (%d bytes) charRng(%d, +%d) boxRng(%d, +%d): -->%s<--\n", str.length, prevFontStart, (textMetrics->chars.use - prevFontStart), curBox.rng.start, curBox.rng.size, str.str);
                            NBString_release(&str);
                        }*/
#                       endif
						if(vm != NULL){
							_textVisuals.agregarElemento(vm);
							vm->liberar(NB_RETENEDOR_THIS);
							//Flush current box
							if(curBox.rng.size > 0){
                                curBox.rngChars.start   = prevFontStart;
                                curBox.rngChars.size    = (textMetrics->chars.use - prevFontStart);
								NBArray_addValue(&_textsBoxes, curBox);
								//Reset
								NBMemory_setZeroSt(curBox, STNBFontRenderBox);
							}
						}
					}
				}
			}
		}
	}
	//Create levels
	if(_textsBoxes.use > 0){
		//Add level zero (grouped-lines boxes)
		STNBRangeI lvlRng;
		lvlRng.start	= 0;
		lvlRng.size		= (_textsBoxes.use - lvlRng.start);
#       ifdef NB_CONFIG_INCLUDE_ASSERTS
        /*const UI32 lvlZeroBoxesSz = _textsBoxes.use;
        if(lvlRng.size > 0){
            const STNBTextMetrics* textMetrics  = &_textData.metrics;
            UI32 iFirstBox = lvlRng.start;
            UI32 iLastBox = lvlRng.start + lvlRng.size - 1;
            //translate grp-boxes to root-boxes
            while(iFirstBox >= lvlZeroBoxesSz){
                const STNBFontRenderBox* bFirst = NBArray_itmPtrAtIndex(&_textsBoxes, STNBFontRenderBox, iFirstBox);
                iFirstBox = bFirst->rng.start;
            }
            while(iLastBox >= lvlZeroBoxesSz){
                const STNBFontRenderBox* bLast  = NBArray_itmPtrAtIndex(&_textsBoxes, STNBFontRenderBox, iLastBox);
                iLastBox = bLast->rng.start + bLast->rng.size - 1;
            }
            {
                const STNBFontRenderBox* bFirst     = NBArray_itmPtrAtIndex(&_textsBoxes, STNBFontRenderBox, iFirstBox);
                const STNBFontRenderBox* bLast      = NBArray_itmPtrAtIndex(&_textsBoxes, STNBFontRenderBox, iLastBox);
                const STNBTextMetricsChar* cFirst   = NBArray_itmPtrAtIndex(&textMetrics->chars, STNBTextMetricsChar, bFirst->rngChars.start);
                const STNBTextMetricsChar* cLast    = NBArray_itmPtrAtIndex(&textMetrics->chars, STNBTextMetricsChar, bLast->rngChars.start + bLast->rngChars.size - 1);
                STNBString str;
                NBString_initWithStrBytes(&str, &_textData.text.str[cFirst->iByte], cLast->iByte + cLast->bytesLen - cFirst->iByte);
                PRINTF_INFO("NBTextMetrics adding level-#%d unique-box: -->%s<--.\n", _textsBoxesLvls.use + 1, str.str);
                NBString_release(&str);
            }
        }*/
#       endif
		NBArray_addValue(&_textsBoxesLvls, lvlRng);
		//Add sub-levels (if groups can be made)
		if(NB_SCENE_TEXT_BOXES_GROUPS_CHARS_MAX > 1){
            //while last lvl can still be divided into groups
			while(lvlRng.size > NB_SCENE_TEXT_BOXES_GROUPS_CHARS_MAX){
				//Build new level group
				STNBFontRenderBox curGrp;
				NBMemory_setZeroSt(curGrp, STNBFontRenderBox);
				NBAABox_init(&curGrp.box);
				curGrp.rng.start	= lvlRng.start;
				curGrp.rng.size		= 0;
				{
					const SI32 iEndAfter = lvlRng.start + lvlRng.size;
					SI32 i; for(i = lvlRng.start; i < iEndAfter; i++){
						const STNBFontRenderBox grpBox = NBArray_itmValueAtIndex(&_textsBoxes, STNBFontRenderBox, i);
						//Inflate current group box
						if(curGrp.rng.size <= 0){
							NBAABox_wrapFirstPoint(&curGrp.box, NBST_P(STNBPoint, grpBox.box.xMin, grpBox.box.yMin));
							NBAABox_wrapNextPoint(&curGrp.box, NBST_P(STNBPoint, grpBox.box.xMax, grpBox.box.yMax));
						} else {
							NBAABox_wrapNextPoint(&curGrp.box, NBST_P(STNBPoint, grpBox.box.xMin, grpBox.box.yMin));
							NBAABox_wrapNextPoint(&curGrp.box, NBST_P(STNBPoint, grpBox.box.xMax, grpBox.box.yMax));
						}
						curGrp.rng.size++;
						//Start new group
						if(curGrp.rng.size >= NB_SCENE_TEXT_BOXES_GROUPS_CHARS_MAX){
							NBArray_addValue(&_textsBoxes, curGrp);
#                           ifdef NB_CONFIG_INCLUDE_ASSERTS
                            /*if(curGrp.rng.size > 0){
                                const STNBTextMetrics* textMetrics  = &_textData.metrics;
                                UI32 iFirstBox = curGrp.rng.start;
                                UI32 iLastBox = curGrp.rng.start + curGrp.rng.size - 1;
                                //translate grp-boxes to root-boxes
                                while(iFirstBox >= lvlZeroBoxesSz){
                                    const STNBFontRenderBox* bFirst = NBArray_itmPtrAtIndex(&_textsBoxes, STNBFontRenderBox, iFirstBox);
                                    iFirstBox = bFirst->rng.start;
                                }
                                while(iLastBox >= lvlZeroBoxesSz){
                                    const STNBFontRenderBox* bLast  = NBArray_itmPtrAtIndex(&_textsBoxes, STNBFontRenderBox, iLastBox);
                                    iLastBox = bLast->rng.start + bLast->rng.size - 1;
                                }
                                {
                                    const STNBFontRenderBox* bFirst     = NBArray_itmPtrAtIndex(&_textsBoxes, STNBFontRenderBox, iFirstBox);
                                    const STNBFontRenderBox* bLast      = NBArray_itmPtrAtIndex(&_textsBoxes, STNBFontRenderBox, iLastBox);
                                    const STNBTextMetricsChar* cFirst   = NBArray_itmPtrAtIndex(&textMetrics->chars, STNBTextMetricsChar, bFirst->rngChars.start);
                                    const STNBTextMetricsChar* cLast    = NBArray_itmPtrAtIndex(&textMetrics->chars, STNBTextMetricsChar, bLast->rngChars.start + bLast->rngChars.size - 1);
                                    STNBString str;
                                    NBString_initWithStrBytes(&str, &_textData.text.str[cFirst->iByte], cLast->iByte + cLast->bytesLen - cFirst->iByte);
                                    PRINTF_INFO("NBTextMetrics adding level-#%d truncated-box: -->%s<--.\n", _textsBoxesLvls.use + 1, str.str);
                                    NBString_release(&str);
                                }
                            }*/
#                           endif
							//Reset
							NBMemory_setZeroSt(curGrp, STNBFontRenderBox);
							NBAABox_init(&curGrp.box);
							curGrp.rng.start	= (i + 1);
							curGrp.rng.size		= 0;
						}
					}
					//Add remaining group
					if(curGrp.rng.size > 0){
						NBArray_addValue(&_textsBoxes, curGrp);
#                       ifdef NB_CONFIG_INCLUDE_ASSERTS
                        /*if(curGrp.rng.size > 0){
                            const STNBTextMetrics* textMetrics  = &_textData.metrics;
                            UI32 iFirstBox = curGrp.rng.start;
                            UI32 iLastBox = curGrp.rng.start + curGrp.rng.size - 1;
                            //translate grp-boxes to root-boxes
                            while(iFirstBox >= lvlZeroBoxesSz){
                                const STNBFontRenderBox* bFirst = NBArray_itmPtrAtIndex(&_textsBoxes, STNBFontRenderBox, iFirstBox);
                                iFirstBox = bFirst->rng.start;
                            }
                            while(iLastBox >= lvlZeroBoxesSz){
                                const STNBFontRenderBox* bLast  = NBArray_itmPtrAtIndex(&_textsBoxes, STNBFontRenderBox, iLastBox);
                                iLastBox = bLast->rng.start + bLast->rng.size - 1;
                            }
                            {
                                const STNBFontRenderBox* bFirst     = NBArray_itmPtrAtIndex(&_textsBoxes, STNBFontRenderBox, iFirstBox);
                                const STNBFontRenderBox* bLast      = NBArray_itmPtrAtIndex(&_textsBoxes, STNBFontRenderBox, iLastBox);
                                const STNBTextMetricsChar* cFirst   = NBArray_itmPtrAtIndex(&textMetrics->chars, STNBTextMetricsChar, bFirst->rngChars.start);
                                const STNBTextMetricsChar* cLast    = NBArray_itmPtrAtIndex(&textMetrics->chars, STNBTextMetricsChar, bLast->rngChars.start + bLast->rngChars.size - 1);
                                STNBString str;
                                NBString_initWithStrBytes(&str, &_textData.text.str[cFirst->iByte], cLast->iByte + cLast->bytesLen - cFirst->iByte);
                                PRINTF_INFO("NBTextMetrics adding level-#%d final-box: -->%s<--.\n", _textsBoxesLvls.use + 1, str.str);
                                NBString_release(&str);
                            }
                        }*/
#                       endif
					}
				}
				//Add sublevel
				lvlRng.start	= (lvlRng.start + lvlRng.size);
				lvlRng.size		= (_textsBoxes.use - lvlRng.start);
				NBArray_addValue(&_textsBoxesLvls, lvlRng);
				//PRINTF_INFO("NBTextMetrics added level-#%d (%d groups).\n", obj->boxesLvls.use, lvlRng.size);
			}
		}
	}
	//Sync seq-ids
	_textVisualsSeqId = _textDataSeqId;
}

//

void AUEscenaTexto::privDbgAccumRenderModelsGL(const STNBRangeI rngAtLvl, const SI32 iLvl, UI32* dstCharsCount){
	const SI32 iEndAfter = rngAtLvl.start + rngAtLvl.size;
	SI32 i; for(i = rngAtLvl.start; i < iEndAfter; i++){
		const STNBFontRenderBox* box = NBArray_itmPtrAtIndex(&_textsBoxes, STNBFontRenderBox, i);
		if(iLvl > 0){
			//Process lower level
			NBASSERT(box->model == NULL)
			this->privDbgAccumRenderModelsGL(box->rng, (iLvl - 1), dstCharsCount);
		} else if(iLvl == 0){
			if(dstCharsCount != NULL){
				*dstCharsCount = *dstCharsCount + box->rng.size;
			}
		}
	}
}

void AUEscenaTexto::privAddRenderModelsGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas, const NBColor colorMultiplicado, const STNBRangeI rngAtLvl, const SI32 iLvl, UI32* dstGrpsAdded, UI32* dstCharsIgnored){
	UI32 grpsAdded = 0;
	if(dstGrpsAdded != NULL) grpsAdded = *dstGrpsAdded;
	{
		const SI32 iEndAfter = rngAtLvl.start + rngAtLvl.size;
		SI32 i; for(i = rngAtLvl.start; i < iEndAfter; i++){
			const STNBFontRenderBox* box = NBArray_itmPtrAtIndex(&_textsBoxes, STNBFontRenderBox, i);
			STNBPoint pScn0, pScn1; STNBAABox boxScn;
			NBASSERT(box->box.xMin <= box->box.xMax && box->box.yMin <= box->box.yMax)
			NBMATRIZ_MULTIPLICAR_PUNTO(pScn0, _cacheObjEscena.matrizEscena, box->box.xMin, box->box.yMin)
			NBMATRIZ_MULTIPLICAR_PUNTO(pScn1, _cacheObjEscena.matrizEscena, box->box.xMax, box->box.yMax)
			if(pScn0.x <= pScn1.x){ boxScn.xMin = pScn0.x; boxScn.xMax = pScn1.x; } else { boxScn.xMin = pScn1.x; boxScn.xMax = pScn0.x; }
			if(pScn0.y <= pScn1.y){ boxScn.yMin = pScn0.y; boxScn.yMax = pScn1.y; } else { boxScn.yMin = pScn1.y; boxScn.yMax = pScn0.y; }
			NBASSERT(boxScn.xMin <= boxScn.xMax && boxScn.yMin <= boxScn.yMax)
			if(!(boxScn.xMax <= propsRenderizado.cajaProyeccion.xMin || boxScn.xMin >= propsRenderizado.cajaProyeccion.xMax || boxScn.yMax <= propsRenderizado.cajaProyeccion.yMin || boxScn.yMin >= propsRenderizado.cajaProyeccion.yMax)){
				//Elements are inside the scene
				if(iLvl > 0){
					//Process lower level
					NBASSERT(box->model == NULL)
					this->privAddRenderModelsGL(propsRenderizado, propsIluminacion, propsHeredadas, colorMultiplicado, box->rng, (iLvl - 1), &grpsAdded, dstCharsIgnored);
				} else if(iLvl == 0){
					//Add models
					NBASSERT(box->model != NULL)
					if(box->model != NULL){
						AUFuenteVisualModel* vm = (AUFuenteVisualModel*)box->model;
						NBASSERT(vm->esClase(AUFuenteVisualModel::idTipoClase));
						//Reserve header
						const UI32 headPos	= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STEscenaTextoRenderGrpHead));
						//Create model
						PTRfunCmdsGL func	= vm->produceRenderModel(propsRenderizado, _cacheObjEscena.matrizEscena, colorMultiplicado, box->rng, NULL, NULL);
						//Update header (after interal model)
						STEscenaTextoRenderGrpHead* h = (STEscenaTextoRenderGrpHead*)propsRenderizado.bytesDatosModelos->elemPtr(headPos);
						h->func		= func;
						h->dataSz	= propsRenderizado.bytesDatosModelos->conteo - (headPos + sizeof(STEscenaTextoRenderGrpHead));
						//Retain the model
						propsRenderizado.piscinaRetecionRender->agregarElemento(vm);
						grpsAdded++;
					}
				}
			} /*else {
				//Elements are outside the scene
				if(iLvl > 0){
					this->privDbgAccumRenderModelsGL(box->rng, (iLvl - 1), dstCharsIgnored);
				} else if(iLvl == 0){
					if(dstCharsIgnored != NULL){
						*dstCharsIgnored = *dstCharsIgnored + box->rng.size;
					}
				}
			}*/
		}
	}
	if(dstGrpsAdded != NULL) *dstGrpsAdded = grpsAdded;
}

PTRfunCmdsGL AUEscenaTexto::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::actualizarModeloGL")
	//Sync visual representation (if necesary)
	if(_textVisualsSeqId != _textDataSeqId){
		this->privSyncVisualModels();
		_textVisualsSeqId = _textDataSeqId;
	}
	NBColor colorMultiplicado; NBCOLOR_MULTIPLICAR_CON_UI8(colorMultiplicado, propsHeredadas.colorPadre, _propiedades.color8);
	//Global-Header
	const UI32 headPos = propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STEscenaTextoRenderHead));
	UI32 grpsAdded = 0, charsIgnored = 0;
	//Visual models (filtered)
	if(_textsBoxesLvls.use > 0){
		const SI32 topLvlIdx = (_textsBoxesLvls.use - 1);
		const STNBRangeI topLvlRng = NBArray_itmValueAtIndex(&_textsBoxesLvls, STNBRangeI, topLvlIdx);
		this->privAddRenderModelsGL(propsRenderizado, propsIluminacion, propsHeredadas, colorMultiplicado, topLvlRng, topLvlIdx, &grpsAdded, &charsIgnored);
		//Debug
		/*if(charsIgnored > 0){
			PRINTF_INFO("AUEscenaTexto %d chars models ignored.\n", charsIgnored);
		}*/
	}
	//Visual models (all)
	/*{
		UI32 i; for(i = 0; i < _textVisuals.conteo; i++){
			AUFuenteVisualModel* vm = (AUFuenteVisualModel*)_textVisuals.elem(i);
			NBASSERT(vm->esClase(AUFuenteVisualModel::idTipoClase));
			//Reserve header
			const UI32 headPos	= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STEscenaTextoRenderGrpHead));
			//Create model
			PTRfunCmdsGL func	= vm->produceRenderModel(propsRenderizado, _cacheObjEscena.matrizEscena, colorMultiplicado, NBST_P(STNRangeI, 0, -1), NULL, NULL);
			//Update header (after interal model)
			STEscenaTextoRenderGrpHead* h = (STEscenaTextoRenderGrpHead*)propsRenderizado.bytesDatosModelos->elemPtr(headPos);
			h->func		= func;
			h->dataSz	= propsRenderizado.bytesDatosModelos->conteo - (headPos + sizeof(STEscenaTextoRenderGrpHead));
			//Retain the model
			propsRenderizado.piscinaRetecionRender->agregarElemento(vm);
			grpsAdded++;
		}
	}*/
	//Update global header
	{
		STEscenaTextoRenderHead* h = (STEscenaTextoRenderHead*)propsRenderizado.bytesDatosModelos->elemPtr(headPos);
		h->grpsCount = grpsAdded;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return AUEscenaTexto::enviarComandosGL;
}

//ToDo: remove
/*PTRfunCmdsGL AUEscenaTexto::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas, STBloqueGL* dstVertexBlock, UI8* dstVertexType){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	/ *PTRfunCmdsGL r = NULL;
	//Release previoud model (if available)
	if(_textVisual.produceRenderFunc != NULL){
		NBColor colorMultiplicado; NBCOLOR_MULTIPLICAR_CON_UI8(colorMultiplicado, propsHeredadas.colorPadre, _propiedades.color8);
		r = (_textVisual.produceRenderFunc)(&_textVisual, propsRenderizado, _cacheObjEscena.matrizEscena, colorMultiplicado, dstVertexBlock, dstVertexType);
		if(r != NULL){
			//Retain the font
			propsRenderizado.piscinaRetecionRender->agregarElemento(_textVisual.font);
			//
			_renderMatrixLast = _cacheObjEscena.matrizEscena;
			_renderColorLast = colorMultiplicado;
			_renderForceUpdate = false;
		}
	}* /
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return this->actualizarModeloGL(propsRenderizado, propsIluminacion, propsHeredadas);
}*/

void AUEscenaTexto::enviarComandosGL(UI8* punteroDatosModelo){
	BYTE* ptr = (BYTE*)punteroDatosModelo;
	//Print models
	STEscenaTextoRenderHead* h = (STEscenaTextoRenderHead*)ptr; ptr += sizeof(STEscenaTextoRenderHead);
	UI32 i; for(i =0; i < h->grpsCount; i++){
		STEscenaTextoRenderGrpHead* h = (STEscenaTextoRenderGrpHead*)ptr; ptr += sizeof(STEscenaTextoRenderGrpHead);
		if(h->func != NULL){
			(*h->func)(ptr);
		}
		ptr += h->dataSz;
	}
}

//

bool AUEscenaTexto::agregarXmlInternoEn(AUEscenaTexto* texto, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(texto, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	//ToDo: implement
	//AUEscenaTextoData::agregarXmlInternoEn(&texto->_textData, guardarEn, espaciosBaseIzq);
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
	
}

bool AUEscenaTexto::interpretarXmlInterno(AUEscenaTexto* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	const sNodoXML* nodoEscenaObj;		XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
	if(nodoEscenaObj != NULL){
		if(AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj)){
			//ToDo: implement
			//if(AUEscenaTextoData::interpretarXmlInterno(&cargarEn->_textData, datosXml, nodoEscenaObj)){
			//	exito = true;
			//}
		}
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaTexto::agregarBitsInternosEn(AUEscenaTexto* texto, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(texto, guardarEn)){
		//ToDo: implement
		//if(AUEscenaTextoData::agregarBitsInternosEn(&texto->_textData, guardarEn)){
		//	exito = true;
		//}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaTexto::interpretarBitsInternos(AUEscenaTexto* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			//ToDo: implement
			//if(AUEscenaTextoData::agregarBitsInternosEn(&cargarEn->_textData, cargarDe)){
			//	exito = true;
			//}
		}
	} else {
		PRINTF_ERROR("AUEscenaTexto, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaTexto, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaTexto, "AUEscenaTexto", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaTexto)






