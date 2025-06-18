//
//  AUEscenaPanelTexto.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 10/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaPanelTexto.h"
#include "NBGestorAnimadores.h"
#include "NBGestorEscena.h"

AUEscenaPanelTexto::AUEscenaPanelTexto(AUFuenteRender* font, AUTextura* scrollBarTex, const STNBColor8 scrollBarColor, IEscuchadorPanelTexto* lstnr)
: AUEscenaContenedorLimitado()
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaPanelTexto")
	_font				= font; if(_font != NULL) _font->retener(NB_RETENEDOR_THIS);
	_lastSize.width		= 0.0f;
	_lastSize.height	= 0.0f;
	_margins.left		= _margins.right = _margins.top = _margins.bottom = 0.0f;
	_lastWidth			= 0.0f;
	_lstnr				= lstnr;
	{
		//Visual
		_content.txt	= new(this) AUEscenaTexto(_font);
		_content.txt->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_content.txt->establecerMultiplicadorColor8(0, 0, 0, 255);
		_content.txt->establecerEscuchadorTouches(this, this);
		_content.txt->setTouchScrollEnabled(TRUE);
		this->agregarObjetoEscena(_content.txt);
		//Filter
		_content.txtVisualFilter = new(this) AUTextoFiltroLimitado();
		_content.txt->pushVisualFilter(_content.txtVisualFilter);
		//
		_content.touchFirst				= NULL;
		_content.touchItmsStartPos		= NBST_P(STNBPoint, 0, 0 );
		_content.touchDeltaYLast 		= 0.0f;	//deltaY from last tick (to add to velocity)
		_content.touchDeltaYAccum		= 0.0f;
		_content.touchDeltaYAccumSecs	= 0.0f;
		//Scroll
		{
			//Vertical
			{
				NBMemory_setZero(_content.scroll.v);
				_content.scroll.v.objLimitMax	= 0.0f;
				_content.scroll.v.objLimitMin	= 0.0f;
				_content.scroll.v.marginTop		= 0.0f;
				_content.scroll.v.marginBtm		= 0.0f;
				_content.scroll.v.objSize		= 0.0f;
				_content.scroll.v.visibleSize	= 0.0f;
				_content.scroll.v.curSpeed		= 0.0f;	//cur scroll speed
				//
				_content.scroll.v.bar.maxAlpha8			= scrollBarColor.a;
				_content.scroll.v.bar.secsForAppear		= 0.20f;
				_content.scroll.v.bar.secsFullVisible	= 0.60f;
				_content.scroll.v.bar.secsForDisapr		= 0.20f;
				_content.scroll.v.bar.secsAccum			= _content.scroll.v.bar.secsForAppear + _content.scroll.v.bar.secsFullVisible + _content.scroll.v.bar.secsForDisapr; //Start hidden
				if(scrollBarTex == NULL){
					_content.scroll.v.bar.obj	= new AUEscenaSpriteElastico();
				} else {
					_content.scroll.v.bar.obj	= new AUEscenaSpriteElastico(scrollBarTex);
				}
				_content.scroll.v.bar.obj->establecerMultiplicadorColor8(scrollBarColor.r, scrollBarColor.g, scrollBarColor.b, _content.scroll.v.bar.maxAlpha8);
				_content.scroll.v.bar.obj->establecerVisible(false);
				this->agregarObjetoEscena(_content.scroll.v.bar.obj);
			}
		}
	}
	//Organizar
	this->privOrganizarContenido(_lastSize.width, _lastSize.height);
	//
	NBGestorAnimadores::agregarAnimador(this, this);
}

AUEscenaPanelTexto::~AUEscenaPanelTexto(){
	NBGestorAnimadores::quitarAnimador(this);
	if(_font != NULL) _font->liberar(NB_RETENEDOR_THIS); _font = NULL;
	//
	{
		_content.touchFirst		= NULL;
		_content.touchItmsStartPos = NBST_P(STNBPoint, 0, 0 );
		if(_content.txt != NULL) _content.txt->liberar(NB_RETENEDOR_THIS); _content.txt = NULL;
		if(_content.txtVisualFilter != NULL) _content.txtVisualFilter->liberar(NB_RETENEDOR_THIS); _content.txtVisualFilter = NULL;
		//Scroll
		{
			if(_content.scroll.v.bar.obj != NULL) _content.scroll.v.bar.obj->liberar(NB_RETENEDOR_THIS); _content.scroll.v.bar.obj = NULL;
		}
	}
}

//

AUFuenteRender* AUEscenaPanelTexto::getFont() const {
	return _font;
}

const char* AUEscenaPanelTexto::getText() const {
	return _content.txt->texto();
}

UI32 AUEscenaPanelTexto::getTextLen() const {
	return _content.txt->textoLen();
}

UI32 AUEscenaPanelTexto::getCharDefsCount() const {
	return _content.txt->getCharDefsCount();
}

UI32 AUEscenaPanelTexto::getLinesCount() const {
	return _content.txt->getLinesCount();
}

STNBSize AUEscenaPanelTexto::getCurSize() const {
	return _lastSize;
}

//

NBMargenes AUEscenaPanelTexto::getMargins(){
	return _margins;
}

void AUEscenaPanelTexto::setMargins(const float left, const float right, const float top, const float botm){
	_margins.left	= left;
	_margins.right	= right;
	_margins.top	= top;
	_margins.bottom	= botm;
}

void AUEscenaPanelTexto::setMargins(const NBMargenes margins){
	_margins = margins;
}

//

void AUEscenaPanelTexto::privOrganizarContenido(const float anchoMax, const float altoMax){
	const float szObjBefore	= _content.scroll.v.objSize;
	const float szVisBefore	= _content.scroll.v.visibleSize;
	if(_content.txt != NULL){
		const float wAval	= anchoMax - _margins.left - _margins.right;
		const float hAval	= altoMax - _margins.top - _margins.bottom;
		const STNBRect col = NBST_P(STNBRect,0, 0, wAval, 0);
		_content.txt->organizarTextoEnCol(col);
		{
			const NBCajaAABB cajaCont = _content.txt->cajaAABBLocalCalculada();
			const float txtHeight = (cajaCont.yMax - cajaCont.yMin);
			if(txtHeight <= hAval || anchoMax <= 0.0f || altoMax <= 0.0f){
				this->establecerLimites(0.0f, (anchoMax <= 0 ? (cajaCont.xMax - cajaCont.xMin) : anchoMax), 0.0f - _margins.top - txtHeight - _margins.bottom, 0.0f);
				_content.txt->emptyVisualFilters();
				_content.txt->establecerTraslacion(_margins.left/* - cajaCont.xMin*/, 0.0f - _margins.top - cajaCont.yMax);
				_content.scroll.v.objLimitMax		= cajaCont.yMax;
				_content.scroll.v.objLimitMin		= cajaCont.yMin;
				_content.scroll.v.marginTop			= _margins.top;
				_content.scroll.v.marginBtm			= _margins.bottom;
				_content.scroll.v.objSize			= txtHeight;
				_content.scroll.v.visibleSize		= txtHeight;
			} else {
				this->establecerLimites(0.0f, (anchoMax <= 0 ? (cajaCont.xMax - cajaCont.xMin) : anchoMax), 0.0f - _margins.top - hAval - _margins.bottom, 0.0f);
				_content.txt->emptyVisualFilters();
				_content.txt->pushVisualFilter(_content.txtVisualFilter);
				_content.txt->establecerTraslacion(_margins.left /*- cajaCont.xMin*/, 0.0f - _margins.top - cajaCont.yMax);
				_content.txtVisualFilter->setOuterLimits(0.0f, (anchoMax <= 0 ? (cajaCont.xMax - cajaCont.xMin) : anchoMax), 0.0f - _margins.top - hAval - _margins.bottom, 0.0f);
				_content.txtVisualFilter->setInnerLimits(_margins.left, (anchoMax <= 0 ? (cajaCont.xMax - cajaCont.xMin) : anchoMax) - _margins.right,  0.0f - _margins.top - hAval, 0.0f - _margins.top);
				_content.scroll.v.objLimitMax		= cajaCont.yMax;
				_content.scroll.v.objLimitMin		= cajaCont.yMin;
				_content.scroll.v.marginTop			= _margins.top;
				_content.scroll.v.marginBtm			= _margins.bottom;
				_content.scroll.v.objSize			= txtHeight;
				_content.scroll.v.visibleSize		= hAval;
			}
		}
		_lastWidth = anchoMax;
	}
	//Reset scroll-bar 'show' animation.
	if(szObjBefore != _content.scroll.v.objSize || szVisBefore != _content.scroll.v.visibleSize){
		if(_content.scroll.v.objSize > (_content.scroll.v.visibleSize + 1.0f)){
			_content.scroll.v.bar.secsAccum = 0.0f;
		}
	}
	//
	_lastSize.width		= anchoMax;
	_lastSize.height	= altoMax;
	//Update filterBoxes
	this->privScrollTopContent(0.0f, FALSE);
}

//

void AUEscenaPanelTexto::establecerAlineacionH(const ENNBTextLineAlignH alineacionH){
	_content.txt->establecerAlineacionH(alineacionH);
}

void AUEscenaPanelTexto::establecerAlineacionV(const ENNBTextAlignV alineacionV){
	_content.txt->establecerAlineacionV(alineacionV);
}

void AUEscenaPanelTexto::establecerAlineaciones(const ENNBTextLineAlignH alineacionH, const ENNBTextAlignV alineacionV){
	_content.txt->establecerAlineaciones(alineacionH, alineacionV);
}

//

void AUEscenaPanelTexto::establecerTexto(const char* txt, const float anchoMax, const float altoMax){
	_content.txt->establecerTexto(txt, anchoMax, altoMax);
	_content.txt->establecerMultiplicadorColor8(0, 0, 0, 255); //Force black color
	this->privOrganizarContenido(anchoMax, altoMax);
}

void AUEscenaPanelTexto::establecerTexto(const STNBText* texto, const float anchoMax, const float altoMax){
	_content.txt->establecerTexto(texto, anchoMax, altoMax);
	_content.txt->establecerMultiplicadorColor8(255, 255, 255, 255); //Use formats color
	this->privOrganizarContenido(anchoMax, altoMax);
}

void AUEscenaPanelTexto::organizaContenido(const float anchoMax, const float altoMax){
	this->privOrganizarContenido(anchoMax, altoMax);
}

void AUEscenaPanelTexto::privScrollTopContent(const float yScroll, const BOOL showScrollbar){
	this->privScrollTopContentToPos(_content.txt->traslacion().y + yScroll, showScrollbar);
}

void AUEscenaPanelTexto::privScrollTopContentToPos(const float yPos, const BOOL showScrollbar){
	const float maxYPos = _content.scroll.v.objSize - _content.scroll.v.objLimitMax - _content.scroll.v.visibleSize - _content.scroll.v.marginBtm;
	const float minYPos = -_content.scroll.v.objLimitMax - _content.scroll.v.marginTop;
	NBPunto pos = _content.txt->traslacion();
	pos.y = yPos;
	if(pos.y < minYPos){
		pos.y						= minYPos;
		_content.touchDeltaYLast	= 0.0f;
		_content.touchDeltaYAccum	= 0.0f;
		_content.scroll.v.curSpeed	= 0.0f;
	} else if(pos.y > maxYPos){
		pos.y						= maxYPos;
		_content.touchDeltaYLast	= 0.0f;
		_content.touchDeltaYAccum	= 0.0f;
		_content.scroll.v.curSpeed	= 0.0f;
	}
	_content.txt->establecerTraslacion(pos.x, pos.y);
	//Update filter
	{
		const float yMax = _content.scroll.v.objLimitMax - pos.y - _content.scroll.v.marginTop;
		const float yMin = yMax - _content.scroll.v.visibleSize;
		//Inner
		{
			const STNBAABox lims = _content.txtVisualFilter->getInnerLimits();
			_content.txtVisualFilter->setInnerLimits(lims.xMin, lims.xMax, yMin, yMax);
		}
		//Outer
		{
			const STNBAABox lims = _content.txtVisualFilter->getOuterLimits();
			_content.txtVisualFilter->setOuterLimits(lims.xMin, lims.xMax, yMin - _content.scroll.v.marginBtm, yMax + _content.scroll.v.marginTop);
		}
	}
	//Update scroll.v.bar
	if(maxYPos != minYPos){
		const float relScroll	= (pos.y - minYPos) / (maxYPos - minYPos); NBASSERT(relScroll >= -0.001f && relScroll < 1.001f)
		const float relSz		= _content.scroll.v.visibleSize / _content.scroll.v.objSize;
		const float height		= _content.scroll.v.visibleSize * relSz;
		const NBTamano texSz	= _content.scroll.v.bar.obj->textura()->tamanoHD();
		_content.scroll.v.bar.obj->redimensionar(0.0f, 0.0f, height, -texSz.alto);
		_content.scroll.v.bar.obj->establecerRotacion(-90.0f);
		_content.scroll.v.bar.obj->establecerTraslacion(_lastWidth, -_content.scroll.v.marginTop + ((_content.scroll.v.visibleSize - height) * -relScroll));
		if(showScrollbar){
			if(_content.scroll.v.bar.secsAccum >= (_content.scroll.v.bar.secsForAppear + _content.scroll.v.bar.secsFullVisible)){
				//Start visible
				_content.scroll.v.bar.secsAccum = 0.0f;
			} else if(_content.scroll.v.bar.secsAccum >= _content.scroll.v.bar.secsForAppear){
				//Keep visible
				_content.scroll.v.bar.secsAccum = _content.scroll.v.bar.secsForAppear;
			}
		}
	}
}

//

void AUEscenaPanelTexto::tickAnimacion(float segsTranscurridos){
	//Consume last deltaY
	{
		_content.touchDeltaYAccumSecs += segsTranscurridos;
		if((_content.touchDeltaYAccum > 0.0f && _content.touchDeltaYLast < 0.0f) || (_content.touchDeltaYAccum < 0.0f && _content.touchDeltaYLast > 0.0f)){
			//Direction changed
			_content.scroll.v.curSpeed			= 0.0f;
			_content.touchDeltaYAccum		= 0.0f;
			_content.touchDeltaYAccumSecs	= 0.0f;
		} else {
			//Acumulate this (is same direction)
			_content.touchDeltaYAccum		+= _content.touchDeltaYLast;
		}
		_content.touchDeltaYLast			= 0.0f;
	}
	//Scroll (only if not touching)
	if(_content.touchFirst == NULL){
		//Apply accumulated speed
		if(_content.touchDeltaYAccum != 0.0f){
			NBASSERT(_content.touchDeltaYAccumSecs > 0.0f)
			if(_content.touchDeltaYAccumSecs != 0.0f){
				_content.scroll.v.curSpeed		+= (_content.touchDeltaYAccum / _content.touchDeltaYAccumSecs);
				//PRINTF_INFO("_content.touchDeltaYLast(%f); prevScrollSpeedY(%f) => _content.scroll.v.curSpeed(%f).\n", _content.touchDeltaYLast, prevScrollSpeedY, _content.scroll.v.curSpeed);
			}
			_content.touchDeltaYAccum		= 0.0f;
			_content.touchDeltaYAccumSecs	= 0.0f;
		}
		//Apply scroll
		if(_content.scroll.v.curSpeed != 0.0f){
			//PRINTF_INFO("_content.scroll.v.curSpeed(%f).\n", _content.scroll.v.curSpeed);
			this->privScrollTopContent(_content.scroll.v.curSpeed * segsTranscurridos, TRUE);
			_content.scroll.v.curSpeed -= (_content.scroll.v.curSpeed * pow(segsTranscurridos, 2.0f / 3.0f));
			if(_content.scroll.v.curSpeed >= -5.0f && _content.scroll.v.curSpeed <= 5.0f){
				_content.scroll.v.curSpeed = 0.0f;
			}
		}
	}
	//Scrollbar visibility
	{
		const float maxAlpha8 = (float)_content.scroll.v.bar.maxAlpha8;
		_content.scroll.v.bar.secsAccum += segsTranscurridos;
		if(_content.scroll.v.bar.secsAccum < _content.scroll.v.bar.secsForAppear){
			//Showing bar
			const float rel = (_content.scroll.v.bar.secsAccum / _content.scroll.v.bar.secsForAppear);
			_content.scroll.v.bar.obj->establecerMultiplicadorAlpha8(maxAlpha8 * (rel));
			_content.scroll.v.bar.obj->establecerVisible(true);
		} else if(_content.scroll.v.bar.secsAccum < (_content.scroll.v.bar.secsForAppear + _content.scroll.v.bar.secsFullVisible)){
			//Full visibility
			_content.scroll.v.bar.obj->establecerMultiplicadorAlpha8(maxAlpha8);
			_content.scroll.v.bar.obj->establecerVisible(true);
		} else {
			const float rel = ((_content.scroll.v.bar.secsAccum - _content.scroll.v.bar.secsForAppear - _content.scroll.v.bar.secsFullVisible) / _content.scroll.v.bar.secsForDisapr);
			if(rel < 1.0f){
				//Hidding
				_content.scroll.v.bar.obj->establecerMultiplicadorAlpha8(maxAlpha8 * (1.0f - (rel)));
				_content.scroll.v.bar.obj->establecerVisible(true);
			} else {
				//Hidde
				_content.scroll.v.bar.obj->establecerVisible(false);
				_content.scroll.v.bar.secsAccum = (_content.scroll.v.bar.secsForAppear + _content.scroll.v.bar.secsFullVisible + _content.scroll.v.bar.secsForDisapr + 1.0f); //to avoid big numbers
			}
		}
	}
}

//

void AUEscenaPanelTexto::touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto){
	if(_content.txt == objeto){
		if(_content.touchFirst == NULL){ //Must be the first touch
			_content.touchFirst			= touch;
			_content.touchInherEval		= FALSE;
		}
	}
	//Stop scroll
	_content.scroll.v.curSpeed		= 0.0f;
	_content.touchDeltaYLast		= 0.0f;
	_content.touchDeltaYAccum		= 0.0f;
	_content.touchDeltaYAccumSecs	= 0.0f;
}

void AUEscenaPanelTexto::touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	if(_content.txt == objeto){
		if(_content.touchFirst == touch){ //Must be the first touch
			if(_content.touchInherEval){
				this->privScrollTopContentToPos(_content.touchItmsStartPos.y + (posActualEscena.y - posInicialEscena.y), TRUE);
				_content.touchDeltaYLast = (posActualEscena.y - posAnteriorEscena.y);
			} else if(this->idEscena >= 0){
				const STNBSize deltaDrag = {
					posActualEscena.x - posInicialEscena.x
					, posActualEscena.y - posInicialEscena.y
				};
				const float deltaDrag2 = NBPUNTO_DISTANCIA_AL_CUADRADO_VECTOR(deltaDrag.width, deltaDrag.height);
				const float deltaLimit = NBGestorEscena::altoPulgadasAEscena(this->idEscena, 0.05f);
				if(deltaDrag2 >= (deltaLimit * deltaLimit)){
					//Eval inherit
					if(_lstnr != NULL){
						if((deltaDrag.height * deltaDrag.height) < (deltaDrag.width * deltaDrag.width)){
							AUEscenaObjeto* inher = _lstnr->panelTextoHerederoParaTouch(this, posInicialEscena, posActualEscena);
							if(inher != NULL){
								objeto->liberarTouch(touch, posInicialEscena, posAnteriorEscena, posActualEscena,TRUE, inher);
							}
						}
					}
					//Init
					{
						const NBPunto pos = _content.txt->traslacion();
						_content.touchItmsStartPos.x	= pos.x;
						_content.touchItmsStartPos.y	= pos.y;
						//Stop scroll
						_content.scroll.v.curSpeed		= 0.0f;
						_content.touchDeltaYLast		= 0.0f;
						_content.touchDeltaYAccum		= 0.0f;
						_content.touchDeltaYAccumSecs	= 0.0f;
					}
					_content.touchInherEval = TRUE;
				}
			}
		}
	}
}

void AUEscenaPanelTexto::touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	if(_content.txt == objeto){
		if(_content.touchFirst == touch){ //Must be the first touch
			if(_content.touchInherEval){
				this->privScrollTopContentToPos(_content.touchItmsStartPos.y + (posActualEscena.y - posInicialEscena.y), TRUE);
				_content.touchDeltaYLast = (posActualEscena.y - posAnteriorEscena.y);
			}
			_content.touchFirst = NULL;
		}
	}
}

//

void AUEscenaPanelTexto::touchScrollApply(AUEscenaObjeto* objeto, const STNBPoint posScene, const STNBSize size, const BOOL animate){
	if(_content.txt == objeto){
		this->privScrollTopContent(size.height, TRUE);
	}
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaPanelTexto, AUEscenaContenedorLimitado)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaPanelTexto, "AUEscenaPanelTexto", AUEscenaContenedorLimitado)
AUOBJMETODOS_CLONAR_NULL(AUEscenaPanelTexto)





