//
//  AUISelectBox.cpp
//  lib-auframework-media
//
//  Created by Marcos Ortega on 9/3/18.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUISelectBox.h"
//
#include "nb/core/NBMemory.h"
#include "nb/core/NBString.h"
#include "NBGestorAnimadores.h"
#include "NBGestorEscena.h"

#define AUISELECT_BOX_ANIM_DURATION 0.30

AUISelectBox::AUISelectBox(AUFuenteRender* font, AUFuenteRender* fontSels, AUTextura* icoDropMore, const float boxWidth, const float marginI)
: AUEscenaContenedor()
{
	_marginI	= marginI;
	_margins.left = _margins.right = _margins.top = _margins.bottom	= 0;
	_boxWidth	= boxWidth;
	_isEnabled	= TRUE;
	_onFocus	= FALSE;
	_font		= font; if(_font != NULL) _font->retener(NB_RETENEDOR_THIS);
	_fontSels	= fontSels; if(_fontSels != NULL) _fontSels->retener(NB_RETENEDOR_THIS);
	//
	_textColor	= NBST_P(STNBColor8, 0, 0, 0, 255 );
	//Options
	_optsBgTex	= NULL;
	_optsMarginIn.left = _optsMarginIn.right = _optsMarginIn.top = _optsMarginIn.bottom	= 0;
	_optsMarginOut.left = _optsMarginOut.right = _optsMarginOut.top = _optsMarginOut.bottom	= 0;
	//Items
	{
		NBArray_init(&_itms, sizeof(STISelectBoxItem), NULL);
		_top.layer.obj = new(this) AUEscenaContenedor();
		{
			_top.layer.glass = new(this) AUEscenaSprite();
			_top.layer.glass->establecerMultiplicadorColor8(0, 0, 0, 220);
			_top.layer.glass->establecerEscuchadorTouches(this, this);
			_top.layer.obj->agregarObjetoEscena(_top.layer.glass);
		}
		{
			_top.layer.opts.layer = new(this) AUEscenaContenedor();
			_top.layer.opts.layer->establecerEscuchadorTouches(this, this);
			_top.layer.obj->agregarObjetoEscena(_top.layer.opts.layer);
		}
		{
			_top.layer.opts.bg = new(this) AUEscenaSpriteElastico();
			_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.bg);
		}
		{
			_top.layer.opts.itms.layer = new(this) AUEscenaContenedorLimitado();
			_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.itms.layer);
			_top.layer.opts.itms.boxOuter	= NBST_P(STNBAABox, 0, 0, 0, 0 );
			_top.layer.opts.itms.boxInner	= NBST_P(STNBAABox, 0, 0, 0, 0 );
			{
				_top.layer.opts.itms.valuesLayer = new(this) AUEscenaContenedor();
				_top.layer.opts.itms.layer->agregarObjetoEscena(_top.layer.opts.itms.valuesLayer);
				_top.layer.opts.itms.linesLayer	= new(this) AUEscenaContenedor();
				_top.layer.opts.itms.layer->agregarObjetoEscena(_top.layer.opts.itms.linesLayer);
			}
		}
		_itmOrganizedIdx	= -1;	//For incremental-organization (to avoid ordering all the list when adding items)
		_itmOrganizedTopY	= 0.0f;	//For incremental-organization (to avoid ordering all the list when adding items)
		_itmWidth			= 0.0f;
		_itmIdxSeletected	= -1;
	}
	//Top
	{
		_top.posIn				= NBST_P(STNBPoint, 0, 0 );
		_top.posOut				= NBST_P(STNBPoint, 0, 0 );
		_top.touchFirst			= NULL;
		_top.touchItmsStartPos	= NBST_P(STNBPoint, 0, 0 );
		_top.touchDeltaYLast 	= 0.0f;	//deltaY from last tick (to add to velocity)
		_top.touchDeltaYAccum	= 0.0f;
		_top.touchDeltaYAccumSecs = 0.0f;
		_top.scrollSpeedY		= 0.0f ;	//cur scroll speed
		{
			_top.anim.isAnimIn	= FALSE;
			_top.anim.secsDur	= 0.0f;
			_top.anim.secsCur	= 0.0f;
		}
	}
	{
		_bg = new(this) AUEscenaSpriteElastico();
		_bg->establecerEscuchadorTouches(this, this);
		this->agregarObjetoEscena(_bg);
	}
	{
		_visual		= new(this) AUEscenaTexto(_font);
		_visual->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_visual->establecerMultiplicadorColor8(0, 0, 0, 255);
		this->agregarObjetoEscena(_visual);
		//
		_visualIcoLeft	= new(this) AUEscenaSprite();
		_visualIcoLeft->establecerVisible(FALSE);
		this->agregarObjetoEscena(_visualIcoLeft);
		//
		_visualIcoRight	= new(this) AUEscenaSprite();
		_visualIcoRight->establecerVisible(FALSE);
		this->agregarObjetoEscena(_visualIcoRight);
		//
		_visualIcoMore	= new(this) AUEscenaSprite();
		if(icoDropMore == NULL){
			_visualIcoMore->establecerTextura(NULL);
			_visualIcoMore->establecerVisible(FALSE);
		} else {
			_visualIcoMore->establecerTextura(icoDropMore);
			_visualIcoMore->redimensionar(icoDropMore);
			_visualIcoMore->establecerVisible(TRUE);
		}
		this->agregarObjetoEscena(_visualIcoMore);
		//
		_visualHelp		= new(this) AUEscenaTexto(_font);
		_visualHelp->establecerVisible(TRUE);
		_visualHelp->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_visualHelp->establecerMultiplicadorColor8(155, 155, 155, 255);
		this->agregarObjetoEscena(_visualHelp);
	}
	_lstnr	= NULL;
	//
	this->privOrganizar();
}

AUISelectBox::AUISelectBox(AUFuenteRender* font, AUFuenteRender* fontSels, AUTextura* icoDropMore, const float boxWidth, const float marginI, const NBMargenes margins)
: AUEscenaContenedor()
{
	_marginI	= marginI;
	_margins	= margins;
	_boxWidth	= boxWidth;
	_isEnabled	= TRUE;
	_onFocus	= FALSE;
	_font		= font; if(_font != NULL) _font->retener(NB_RETENEDOR_THIS);
	_fontSels	= fontSels; if(_fontSels != NULL) _fontSels->retener(NB_RETENEDOR_THIS);
	_textColor	= NBST_P(STNBColor8, 0, 0, 0, 255 );
	//Options
	_optsBgTex	= NULL;
	_optsMarginIn.left = _optsMarginIn.right = _optsMarginIn.top = _optsMarginIn.bottom	= 0;
	_optsMarginOut.left = _optsMarginOut.right = _optsMarginOut.top = _optsMarginOut.bottom	= 0;
	//Items
	{
		NBArray_init(&_itms, sizeof(STISelectBoxItem), NULL);
		_top.layer.obj = new(this) AUEscenaContenedor();
		{
			_top.layer.glass = new(this) AUEscenaSprite();
			_top.layer.glass->establecerMultiplicadorColor8(0, 0, 0, 220);
			_top.layer.glass->establecerEscuchadorTouches(this, this);
			_top.layer.obj->agregarObjetoEscena(_top.layer.glass);
		}
		{
			_top.layer.opts.layer = new(this) AUEscenaContenedor();
			_top.layer.opts.layer->establecerEscuchadorTouches(this, this);
			_top.layer.obj->agregarObjetoEscena(_top.layer.opts.layer);
		}
		{
			_top.layer.opts.bg = new(this) AUEscenaSpriteElastico();
			_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.bg);
		}
		{
			_top.layer.opts.itms.layer	= new(this) AUEscenaContenedorLimitado();
			_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.itms.layer);
			_top.layer.opts.itms.boxOuter	= NBST_P(STNBAABox, 0, 0, 0, 0 );
			_top.layer.opts.itms.boxInner	= NBST_P(STNBAABox, 0, 0, 0, 0 );
			{
				_top.layer.opts.itms.valuesLayer = new(this) AUEscenaContenedor();
				_top.layer.opts.itms.layer->agregarObjetoEscena(_top.layer.opts.itms.valuesLayer);
				_top.layer.opts.itms.linesLayer	= new(this) AUEscenaContenedor();
				_top.layer.opts.itms.layer->agregarObjetoEscena(_top.layer.opts.itms.linesLayer);
			}
		}
		_itmOrganizedIdx	= -1;	//For incremental-organization (to avoid ordering all the list when adding items)
		_itmOrganizedTopY	= 0.0f;	//For incremental-organization (to avoid ordering all the list when adding items)
		_itmWidth			= 0.0f;
		_itmIdxSeletected	= -1;
	}
	//Top
	{
		_top.posIn				= NBST_P(STNBPoint, 0, 0 );
		_top.posOut				= NBST_P(STNBPoint, 0, 0 );
		_top.touchFirst			= NULL;
		_top.touchItmsStartPos	= NBST_P(STNBPoint, 0, 0 );
		_top.touchDeltaYLast 	= 0.0f;	//deltaY from last tick (to add to velocity)
		_top.touchDeltaYAccum	= 0.0f;
		_top.touchDeltaYAccumSecs = 0.0f;
		_top.scrollSpeedY		= 0.0f ;	//cur scroll speed
		{
			_top.anim.isAnimIn	= FALSE;
			_top.anim.secsDur	= 0.0f;
			_top.anim.secsCur	= 0.0f;
		}
	}
	{
		_bg = new(this) AUEscenaSpriteElastico();
		_bg->establecerEscuchadorTouches(this, this);
		this->agregarObjetoEscena(_bg);
	}
	{
		_visual		= new(this) AUEscenaTexto(_font);
		_visual->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_visual->establecerMultiplicadorColor8(0, 0, 0, 255);
		this->agregarObjetoEscena(_visual);
		//
		_visualIcoLeft	= new(this) AUEscenaSprite();
		_visualIcoLeft->establecerVisible(FALSE);
		this->agregarObjetoEscena(_visualIcoLeft);
		//
		_visualIcoRight	= new(this) AUEscenaSprite();
		_visualIcoRight->establecerVisible(FALSE);
		this->agregarObjetoEscena(_visualIcoRight);
		//
		_visualIcoMore	= new(this) AUEscenaSprite();
		if(icoDropMore == NULL){
			_visualIcoMore->establecerTextura(NULL);
			_visualIcoMore->establecerVisible(FALSE);
		} else {
			_visualIcoMore->establecerTextura(icoDropMore);
			_visualIcoMore->redimensionar(icoDropMore);
			_visualIcoMore->establecerVisible(TRUE);
		}
		this->agregarObjetoEscena(_visualIcoMore);
		//
		_visualHelp		= new(this) AUEscenaTexto(_font);
		_visualHelp->establecerVisible(TRUE);
		_visualHelp->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_visualHelp->establecerMultiplicadorColor8(155, 155, 155, 255);
		this->agregarObjetoEscena(_visualHelp);
	}
	_lstnr	= NULL;
	//
	this->privOrganizar();
}

AUISelectBox::AUISelectBox(AUFuenteRender* font, AUFuenteRender* fontSels, AUTextura* icoDropMore, const float boxWidth, const float marginI, const float mLeft, const float mRight, const float mTop, const float mBottom)
: AUEscenaContenedor()
{
	_marginI		= marginI;
	_margins.left	= mLeft;
	_margins.right	= mRight;
	_margins.top	= mTop;
	_margins.bottom	= mBottom;
	_boxWidth		= boxWidth;
	_isEnabled		= TRUE;
	_onFocus		= FALSE;
	_font			= font; if(_font != NULL) _font->retener(NB_RETENEDOR_THIS);
	_fontSels		= fontSels; if(_fontSels != NULL) _fontSels->retener(NB_RETENEDOR_THIS);
	_textColor		= NBST_P(STNBColor8, 0, 0, 0, 255 );
	//Options
	_optsBgTex		= NULL;
	_optsMarginIn.left = _optsMarginIn.right = _optsMarginIn.top = _optsMarginIn.bottom	= 0;
	_optsMarginOut.left = _optsMarginOut.right = _optsMarginOut.top = _optsMarginOut.bottom	= 0;
	//Items
	{
		NBArray_init(&_itms, sizeof(STISelectBoxItem), NULL);
		_top.layer.obj = new(this) AUEscenaContenedor();
		{
			_top.layer.glass = new(this) AUEscenaSprite();
			_top.layer.glass->establecerMultiplicadorColor8(0, 0, 0, 220);
			_top.layer.glass->establecerEscuchadorTouches(this, this);
			_top.layer.obj->agregarObjetoEscena(_top.layer.glass);
		}
		{
			_top.layer.opts.layer = new(this) AUEscenaContenedor();
			_top.layer.opts.layer->establecerEscuchadorTouches(this, this);
			_top.layer.obj->agregarObjetoEscena(_top.layer.opts.layer);
		}
		{
			_top.layer.opts.bg = new(this) AUEscenaSpriteElastico();
			_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.bg);
		}
		{
			_top.layer.opts.itms.layer = new(this) AUEscenaContenedorLimitado();
			_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.itms.layer);
			_top.layer.opts.itms.boxOuter	= NBST_P(STNBAABox, 0, 0, 0, 0 );
			_top.layer.opts.itms.boxInner	= NBST_P(STNBAABox, 0, 0, 0, 0 );
			{
				_top.layer.opts.itms.valuesLayer = new(this) AUEscenaContenedor();
				_top.layer.opts.itms.layer->agregarObjetoEscena(_top.layer.opts.itms.valuesLayer);
				_top.layer.opts.itms.linesLayer	= new(this) AUEscenaContenedor();
				_top.layer.opts.itms.layer->agregarObjetoEscena(_top.layer.opts.itms.linesLayer);
			}
		}
		_itmOrganizedIdx	= -1;	//For incremental-organization (to avoid ordering all the list when adding items)
		_itmOrganizedTopY	= 0.0f;	//For incremental-organization (to avoid ordering all the list when adding items)
		_itmWidth			= 0.0f;
		_itmIdxSeletected	= -1;
	}
	//Top
	{
		_top.posIn				= NBST_P(STNBPoint, 0, 0 );
		_top.posOut				= NBST_P(STNBPoint, 0, 0 );
		_top.touchFirst			= NULL;
		_top.touchItmsStartPos	= NBST_P(STNBPoint, 0, 0 );
		_top.touchDeltaYLast	= 0.0f;	//deltaY from last tick (to add to velocity)
		_top.touchDeltaYAccum	= 0.0f;
		_top.touchDeltaYAccumSecs = 0.0f;
		_top.scrollSpeedY		= 0.0f ;	//cur scroll speed
		{
			_top.anim.isAnimIn	= FALSE;
			_top.anim.secsDur	= 0.0f;
			_top.anim.secsCur	= 0.0f;
		}
	}
	{
		_bg = new(this) AUEscenaSpriteElastico();
		_bg->establecerEscuchadorTouches(this, this);
		this->agregarObjetoEscena(_bg);
	}
	{
		_visual		= new(this) AUEscenaTexto(_font);
		_visual->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_visual->establecerMultiplicadorColor8(0, 0, 0, 255);
		this->agregarObjetoEscena(_visual);
		//
		_visualIcoLeft	= new(this) AUEscenaSprite();
		_visualIcoLeft->establecerVisible(FALSE);
		this->agregarObjetoEscena(_visualIcoLeft);
		//
		_visualIcoRight	= new(this) AUEscenaSprite();
		_visualIcoRight->establecerVisible(FALSE);
		this->agregarObjetoEscena(_visualIcoRight);
		//
		_visualIcoMore	= new(this) AUEscenaSprite();
		if(icoDropMore == NULL){
			_visualIcoMore->establecerTextura(NULL);
			_visualIcoMore->establecerVisible(FALSE);
		} else {
			_visualIcoMore->establecerTextura(icoDropMore);
			_visualIcoMore->redimensionar(icoDropMore);
			_visualIcoMore->establecerVisible(TRUE);
		}
		this->agregarObjetoEscena(_visualIcoMore);
		//
		_visualHelp		= new(this) AUEscenaTexto(_font);
		_visualHelp->establecerVisible(TRUE);
		_visualHelp->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_visualHelp->establecerMultiplicadorColor8(155, 155, 155, 255);
		this->agregarObjetoEscena(_visualHelp);
	}
	_lstnr	= NULL;
	//
	this->privOrganizar();
}

AUISelectBox::~AUISelectBox(){
	_marginI	= 0.0f;
	_margins.left = _margins.right = _margins.top = _margins.bottom	= 0.0f;
	_boxWidth	= 0.0f;
	//Remove from focus
	this->privRemoveFromTopPriv();
	//Top
	{
		_top.touchFirst		= NULL;
		_top.touchItmsStartPos = NBST_P(STNBPoint, 0, 0 );
	}
	//Options
	{
		if(_optsBgTex != NULL) _optsBgTex->liberar(NB_RETENEDOR_THIS); _optsBgTex = NULL;
	}
	//Items
	{
		this->empty(); NBArray_release(&_itms);
		if(_top.layer.obj != NULL) _top.layer.obj->liberar(NB_RETENEDOR_THIS); _top.layer.obj = NULL;
		if(_top.layer.glass != NULL) _top.layer.glass->liberar(NB_RETENEDOR_THIS); _top.layer.glass = NULL;
		if(_top.layer.opts.layer != NULL) _top.layer.opts.layer->liberar(NB_RETENEDOR_THIS); _top.layer.opts.layer = NULL;
		if(_top.layer.opts.bg != NULL) _top.layer.opts.bg->liberar(NB_RETENEDOR_THIS); _top.layer.opts.bg = NULL;
		if(_top.layer.opts.itms.layer != NULL) _top.layer.opts.itms.layer->liberar(NB_RETENEDOR_THIS); _top.layer.opts.itms.layer = NULL;
		if(_top.layer.opts.itms.valuesLayer != NULL) _top.layer.opts.itms.valuesLayer->liberar(NB_RETENEDOR_THIS); _top.layer.opts.itms.valuesLayer = NULL;
		if(_top.layer.opts.itms.linesLayer != NULL) _top.layer.opts.itms.linesLayer->liberar(NB_RETENEDOR_THIS); _top.layer.opts.itms.linesLayer = NULL;
	}
	if(_font != NULL) _font->liberar(NB_RETENEDOR_THIS); _font = NULL;
	if(_fontSels != NULL) _fontSels->liberar(NB_RETENEDOR_THIS); _fontSels = NULL;
	if(_bg != NULL) _bg->liberar(NB_RETENEDOR_THIS); _bg = NULL;
	//
	if(_visual != NULL) _visual->liberar(NB_RETENEDOR_THIS); _visual = NULL;
	if(_visualIcoLeft != NULL) _visualIcoLeft->liberar(NB_RETENEDOR_THIS); _visualIcoLeft = NULL;
	if(_visualIcoRight != NULL) _visualIcoRight->liberar(NB_RETENEDOR_THIS); _visualIcoRight = NULL;
	if(_visualIcoMore != NULL) _visualIcoMore->liberar(NB_RETENEDOR_THIS); _visualIcoMore = NULL;
	if(_visualHelp != NULL) _visualHelp->liberar(NB_RETENEDOR_THIS); _visualHelp = NULL;
	//
	_lstnr	= NULL;
}

//

void AUISelectBox::agregadoEnEscena(){
	AUEscenaContenedor::agregadoEnEscena();
	NBASSERT(!_onFocus) //cant have the focus
	//if(this->idEscena != -1){
	//	NBGestorEscena::habilitarEscena(_subscn.Id);
	//}
}

void AUISelectBox::quitandoDeEscena(){
	AUEscenaContenedor::quitandoDeEscena();
	//Remove from focus (from topLayer)
	{
		NBASSERT((_onFocus && _top.layer.obj->contenedor() != NULL) || (!_onFocus && _top.layer.obj->contenedor() == NULL))
		if(_onFocus){
			//Init out-animation
			_top.anim.isAnimIn	= FALSE;
			_top.anim.secsCur	= 0.0f;
			_top.anim.secsDur	= AUISELECT_BOX_ANIM_DURATION;
			this->privTopAnimate(0.0f); //initial location
		}
	}
	//NBGestorTeclas::escuchadorRenunciar(this);
	//if(this->idEscena != -1){
	//	NBGestorEscena::deshabilitarEscena(_subscn.Id);
	//}
}

//List

AUTextura* AUISelectBox::getOptionsBgTexture() const {			//options bg textye
	return _optsBgTex;
}

NBMargenes AUISelectBox::getOptionsMarginIn() const {		//options inner margin
	return _optsMarginIn;
}

NBMargenes AUISelectBox::getOptionsMarginOut() const {	//options outer margin
	return _optsMarginOut;
}

void AUISelectBox::setOptionsBgTexture(AUTextura* tex){	//options bg textye
	if(tex != NULL) tex->retener(NB_RETENEDOR_THIS);
	if(_optsBgTex != NULL) _optsBgTex->liberar(NB_RETENEDOR_THIS);
	_optsBgTex = tex;
	_top.layer.opts.bg->establecerTextura(_optsBgTex);
}

void AUISelectBox::setOptionsMarginsIn(const NBMargenes margins){ //options inner margin
	_optsMarginIn			= margins;
	//Update list
	this->privOrganizeItmsList(-1, _itmWidth);
}

void AUISelectBox::setOptionsMarginsIn(const float mLeft, const float mRight, const float mTop, const float mBottom){ //options inner margin
	_optsMarginIn.left		= mLeft;
	_optsMarginIn.right		= mRight;
	_optsMarginIn.top		= mTop;
	_optsMarginIn.bottom	= mBottom;
	//Update list
	this->privOrganizeItmsList(-1, _itmWidth);
}

void AUISelectBox::setOptionsMarginsOut(const NBMargenes margins){ //options outer margin
	_optsMarginOut = margins;
}

void AUISelectBox::setOptionsMarginsOut(const float mLeft, const float mRight, const float mTop, const float mBottom){ //options outer margin
	_optsMarginOut.left		= mLeft;
	_optsMarginOut.right	= mRight;
	_optsMarginOut.top		= mTop;
	_optsMarginOut.bottom	= mBottom;
}

//List

void AUISelectBox::empty(){
	SI32 i; for (i = ((SI32)_itms.use - 1); i >= 0; i--){
		STISelectBoxItem* itm = NBArray_itmPtrAtIndex(&_itms, STISelectBoxItem, i);
		NBString_release(&itm->text);
		if(itm->icoLeft != NULL) itm->icoLeft->liberar(NB_RETENEDOR_THIS); itm->icoLeft = NULL;
		if(itm->icoRight != NULL) itm->icoRight->liberar(NB_RETENEDOR_THIS); itm->icoRight = NULL;
		//scene
		{
			if(itm->scn.line != NULL){
				AUEscenaContenedor* parent = (AUEscenaContenedor*)itm->scn.line->contenedor();
				if(parent != NULL) parent->quitarObjetoEscena(itm->scn.line);
				itm->scn.line->liberar(NB_RETENEDOR_THIS);
				itm->scn.line = NULL;
			}
			if(itm->scn.icoRight != NULL) itm->scn.icoRight->liberar(NB_RETENEDOR_THIS); itm->scn.icoRight = NULL;
			if(itm->scn.icoLeft != NULL) itm->scn.icoLeft->liberar(NB_RETENEDOR_THIS); itm->scn.icoLeft = NULL;
			if(itm->scn.fontCustom != NULL) itm->scn.fontCustom->liberar(NB_RETENEDOR_THIS); itm->scn.fontCustom = NULL;
			if(itm->scn.text != NULL) itm->scn.text->liberar(NB_RETENEDOR_THIS); itm->scn.text = NULL;
			if(itm->scn.bg != NULL){
				AUEscenaContenedor* parent = (AUEscenaContenedor*)itm->scn.bg->contenedor();
				if(parent != NULL) parent->quitarObjetoEscena(itm->scn.bg);
				itm->scn.bg->liberar(NB_RETENEDOR_THIS);
				itm->scn.bg = NULL;
			}
			if(itm->scn.layer != NULL){
				AUEscenaContenedor* parent = (AUEscenaContenedor*)itm->scn.layer->contenedor();
				if(parent != NULL) parent->quitarObjetoEscena(itm->scn.layer);
				itm->scn.layer->liberar(NB_RETENEDOR_THIS);
				itm->scn.layer = NULL;
			}
		}
	}
	NBArray_empty(&_itms);
	{
		_itmIdxSeletected = -1;
		_visual->establecerVisible(FALSE);
		_visualIcoLeft->establecerVisible(FALSE);
		_visualIcoRight->establecerVisible(FALSE);
		_visualHelp->establecerVisible(TRUE);
	}
}

UI32 AUISelectBox::getItemsCount() const {
	return _itms.use;
}

BOOL AUISelectBox::addItm(const char* text, AUTextura* icoLeft, AUTextura* icoRight){
	return this->addItm(text, icoLeft, icoRight, _fontSels);
}

BOOL AUISelectBox::addItm(const char* text, AUTextura* icoLeft, AUTextura* icoRight, AUFuenteRender* font){
	BOOL r = FALSE;
	if(font != NULL){
		STISelectBoxItem itm;
		NBMemory_setZeroSt(itm, STISelectBoxItem);
		//Data
		{
			NBString_initWithStr(&itm.text, (text != NULL ? text : ""));
			itm.icoLeft = icoLeft; if(itm.icoLeft != NULL) itm.icoLeft->retener(NB_RETENEDOR_THIS);
			itm.icoRight = icoRight; if(itm.icoRight != NULL) itm.icoRight->retener(NB_RETENEDOR_THIS);
		}
		//Scene
		{
			itm.scn.yTop		= _itmOrganizedTopY;
			itm.scn.layer		= new(this) AUEscenaContenedorLimitado();
			itm.scn.layer->establecerEscuchadorTouches(this, this);
			//
			itm.scn.bg			= new(this) AUEscenaSprite();
			itm.scn.layer->agregarObjetoEscena(itm.scn.bg);
			//
			itm.scn.text		= NULL;
			if(text != NULL){
				if(text[0] != '\0'){
					itm.scn.text = new(this) AUEscenaTexto(font);
					itm.scn.text->establecerMultiplicadorColor8(_textColor.r, _textColor.g, _textColor.b, _textColor.a);
					itm.scn.text->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
					itm.scn.text->establecerTexto(text);
					itm.scn.layer->agregarObjetoEscena(itm.scn.text);
				}
			}
			//Save font
			itm.scn.fontCustom	= font; font->retener();
			//
			itm.scn.icoLeft		= NULL;
			if(icoLeft != NULL){
				itm.scn.icoLeft	= new(this) AUEscenaSprite(icoLeft);
				itm.scn.layer->agregarObjetoEscena(itm.scn.icoLeft);
			}
			//
			itm.scn.icoRight	= NULL;
			if(icoRight != NULL){
				itm.scn.icoRight	= new(this) AUEscenaSprite(icoRight);
				itm.scn.layer->agregarObjetoEscena(itm.scn.icoRight);
			}
			//Line is added at the top layer (is not part of the content)
			itm.scn.line		= new(this) AUEscenaFigura(ENEscenaFiguraTipo_Linea);
			itm.scn.line->establecerMultiplicadorColor8(_textColor.r, _textColor.g, _textColor.b, _textColor.a);
			itm.scn.line->agregarCoordenadaLocal(0.0f, 0.0f);
			itm.scn.line->agregarCoordenadaLocal(0.0f, 0.0f);
		}
		//Add
		NBArray_addValue(&_itms, itm);
		//Post
		{
			//Add to layers
			_top.layer.opts.itms.valuesLayer->agregarObjetoEscena(itm.scn.layer);
			_top.layer.opts.itms.linesLayer->agregarObjetoEscena(itm.scn.line);
			//Organize
			this->privOrganizeItmsList(_itmOrganizedIdx + 1, _itmWidth);
		}
		r = TRUE;
	}
	return r;
}

SI32 AUISelectBox::selectedIdx() const {
	return _itmIdxSeletected;
}

SI32 AUISelectBox::setSelectedIdx(const SI32 idx){
	if(_itmIdxSeletected != idx){
		if(_itmIdxSeletected >= 0 && _itmIdxSeletected < _itms.use){
			STISelectBoxItem* prevSel = NBArray_itmPtrAtIndex(&_itms, STISelectBoxItem, _itmIdxSeletected);
			prevSel->scn.bg->establecerMultiplicadorColor8(255, 255, 255, 255);
		}
		if(idx < 0){
			_visualIcoLeft->establecerTextura(NULL);
			_visualIcoLeft->establecerVisible(FALSE);
			_visualIcoRight->establecerTextura(NULL);
			_visualIcoRight->establecerVisible(FALSE);
			_visual->establecerTexto("");
			_visual->establecerVisible(FALSE);
			_visualHelp->establecerVisible(TRUE);
		} else {
			STISelectBoxItem* newSel = NBArray_itmPtrAtIndex(&_itms, STISelectBoxItem, idx);
			newSel->scn.bg->establecerMultiplicadorColor8(255, 225, 225, 255);
			if(newSel->icoLeft == NULL){
				_visualIcoLeft->establecerTextura(NULL);
				_visualIcoLeft->establecerVisible(FALSE);
			} else {
				_visualIcoLeft->establecerTextura(newSel->icoLeft);
				_visualIcoLeft->redimensionar(newSel->icoLeft);
				_visualIcoLeft->establecerVisible(TRUE);
			}
			if(newSel->icoRight == NULL){
				_visualIcoRight->establecerTextura(NULL);
				_visualIcoRight->establecerVisible(FALSE);
			} else {
				_visualIcoRight->establecerTextura(newSel->icoRight);
				_visualIcoRight->redimensionar(newSel->icoRight);
				_visualIcoRight->establecerVisible(TRUE);
			}
			_visual->establecerTexto(newSel->text.str);
			_visual->establecerVisible(TRUE);
			_visualHelp->establecerVisible(FALSE);
		}
		this->privOrganizar();
		_itmIdxSeletected = idx;
		//Selection
		if(_lstnr != NULL){
			if(_itmIdxSeletected >= 0 && _itmIdxSeletected < _itms.use){
				const STISelectBoxItem* curSel = NBArray_itmPtrAtIndex(&_itms, STISelectBoxItem, _itmIdxSeletected);
				_lstnr->selectboxItemSelected(this, _itmIdxSeletected, curSel->text.str);
			} else {
				_lstnr->selectboxItemSelected(this, -1, NULL);
			}
		}
	}
	return _itmIdxSeletected;
}

//

IEscuchadorISelectBox* AUISelectBox::getTextBoxListener(){
	return _lstnr;
}

void AUISelectBox::setTextBoxListener(IEscuchadorISelectBox* lstnr){
	_lstnr = lstnr;
}

//

NBMargenes AUISelectBox::getMargins() const {
	return _margins;
}

void AUISelectBox::setMargins(const NBMargenes margins){
	_margins	= margins;
}

void AUISelectBox::setMargins(const float mLeft, const float mRight, const float mTop, const float mBottom){
	_margins.left	= mLeft;
	_margins.right	= mRight;
	_margins.top	= mTop;
	_margins.bottom	= mBottom;
}

//

float AUISelectBox::getBoxWidth() const {
	return _boxWidth;
}

void AUISelectBox::setBoxWidth(const float width){
	_boxWidth = width;
	this->privOrganizar();
}

//

AUTextura* AUISelectBox::getBoxBgTexture() const {
	return _bg->textura();
}

void AUISelectBox::setBoxBgTexture(AUTextura* textura){
	_bg->establecerTextura(textura);
}

//Enabled

BOOL AUISelectBox::isEnabled() const {
	return _isEnabled;
}

void AUISelectBox::setEnabled(const BOOL enabled){
	_isEnabled = enabled;
}

//Value

void AUISelectBox::establecerTextoColor(const NBColor8 color){
	_visual->establecerMultiplicadorColor8(color);
	_visualIcoMore->establecerMultiplicadorColor8(color);
}

void AUISelectBox::establecerTextoColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	_visual->establecerMultiplicadorColor8(r, g, b, a);
	_visualIcoMore->establecerMultiplicadorColor8(r, g, b, a);
}

void AUISelectBox::establecerTextoAyuda(const char* valor){
	const STNBRect column = {0, 0, _boxWidth - _margins.left - _margins.right, _font->ascendenteEscena() + _font->descendenteEscena() };
	_visualHelp->establecerTextoEnCol(valor, column);
}

void AUISelectBox::establecerTextoAyudaColor(const NBColor8 color){
	_visualHelp->establecerMultiplicadorColor8(color);
}

void AUISelectBox::establecerTextoAyudaColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	_visualHelp->establecerMultiplicadorColor8(r, g, b, a);
}

//

BOOL AUISelectBox::atFocus(){
	return _onFocus;
}

void AUISelectBox::focusObtain(){
	this->privStartAddToTop();
}

void AUISelectBox::focusRelease(){
	this->privRemoveFromTopPriv();
}

//

void AUISelectBox::setTextColor(const NBColor8 color){
	this->setTextColor(color.r, color.g, color.b, color.a);
}

void AUISelectBox::setTextColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	_textColor.r	= r;
	_textColor.g	= g;
	_textColor.b	= b;
	_textColor.a	= a;
	//Apply color
	{
		SI32 i; for (i = ((SI32)_itms.use - 1); i >= 0; i--){
			STISelectBoxItem* itm = NBArray_itmPtrAtIndex(&_itms, STISelectBoxItem, i);
			if(itm->scn.line != NULL) itm->scn.line->establecerMultiplicadorColor8(_textColor.r, _textColor.g, _textColor.b, _textColor.a);
			if(itm->scn.text != NULL) itm->scn.text->establecerMultiplicadorColor8(_textColor.r, _textColor.g, _textColor.b, _textColor.a);
		}
	}
}

float AUISelectBox::altoParaLineasVisibles(const float lineasVisibles) const {
	return AUISelectBox::altoParaLineasVisibles(lineasVisibles, _font);
}

float AUISelectBox::altoParaLineasVisibles(const float lineasVisibles, AUFuenteRender* fuenteRender) {
	/*const float lineasVisiblesE	= (float)((SI32)lineasVisibles);
	 const float ascendente		= fuenteRender->ascendenteEscena();
	 const float descendente		= fuenteRender->descendenteEscena(); //is negative
	 const float saltoPorLinea	= fuenteRender->altoGuiaLineaEscena();
	 const float separaLinea		= (saltoPorLinea > (ascendente - descendente) ? (saltoPorLinea - ascendente + descendente) : 0.0f);
	 const float altoParaTexto	= ((ascendente - descendente) * lineasVisiblesE) + (lineasVisiblesE >= 1.0f ? (separaLinea * (lineasVisiblesE - 1.0f)) : 0.0f);
	 return altoParaTexto;*/
	//const float linesE			= (float)((SI32)lineasVisibles);
	const float ascendente		= fuenteRender->ascendenteEscena();
	const float descendente		= fuenteRender->descendenteEscena(); //is negative
	const float fontHeight		= fuenteRender->altoGuiaLineaEscena();
	const float textHeight		= (lineasVisibles * fontHeight) - (fontHeight - ascendente + descendente);
	NBASSERT(lineasVisibles == 0 || textHeight > 0)
	return textHeight;
}

//

void AUISelectBox::privOrganizar(){
	const float lineHeight = AUISelectBox::altoParaLineasVisibles(1.0f, _font);
	const float itmHeight = _margins.top + lineHeight + _margins.bottom;
	//Content
	{
		float xLeft = _margins.left, xRight = _boxWidth - _margins.right;
		//Help text
		if(_visualHelp != NULL){
			const NBCajaAABB box = _visualHelp->cajaAABBLocal();
			_visualHelp->establecerTraslacion(xLeft - box.xMin, - box.yMax - ((itmHeight - (box.yMax - box.yMin)) * 0.5f));
		}
		//Ico "more"
		if(_visualIcoMore != NULL){
			const NBCajaAABB box = _visualIcoMore->cajaAABBLocal();
			_visualIcoMore->establecerTraslacion(xRight - box.xMax, - box.yMax - ((itmHeight - (box.yMax - box.yMin)) * 0.5f));
			if(_visualIcoMore->visible()){
				xRight -= (box.xMax - box.xMin) + _marginI;
			}
		}
		//Ico left
		if(_visualIcoLeft != NULL){
			const NBCajaAABB box = _visualIcoLeft->cajaAABBLocal();
			_visualIcoLeft->establecerTraslacion(xLeft - box.xMin, - box.yMax - ((itmHeight - (box.yMax - box.yMin)) * 0.5f));
			if(_visualIcoLeft->visible()){
				xLeft += (box.xMax - box.xMin) + _marginI;
			}
		}
		//Ico right
		if(_visualIcoRight != NULL){
			const NBCajaAABB box = _visualIcoRight->cajaAABBLocal();
			_visualIcoRight->establecerTraslacion(xRight - box.xMax, - box.yMax - ((itmHeight - (box.yMax - box.yMin)) * 0.5f));
			if(_visualIcoRight->visible()){
				xRight -= (box.xMax - box.xMin) + _marginI;
			}
		}
		//Text
		if(_visual != NULL){
			const NBCajaAABB box = _visual->cajaAABBLocal();
			_visual->establecerTraslacion(xLeft - box.xMin, - box.yMax - ((itmHeight - (box.yMax - box.yMin)) * 0.5f));
		}
	}
	//Bg
	_bg->redimensionar(0.0f, 0.0f, _boxWidth, -itmHeight);
}

void AUISelectBox::privOrganizeItmsList(const SI32 pStartIdx, const float itmWidth){
	NBASSERT(pStartIdx == 0 || pStartIdx < _itms.use)
	if(itmWidth != 0.0f){
		SI32 startIdx	= pStartIdx;
		float topY		= _itmOrganizedTopY;
		//Validate
		if(startIdx <= 0 || _itmWidth != itmWidth){
			//Do all items
			startIdx	= 0;
			topY		= 0.0f;
			_itmWidth	= itmWidth;
		}
		//PRINTF_INFO("AUISelectBox, organizing from #%d to %d (yTop %.2f, itmWidth %.2f, _itmHeight %.2f).\n", (startIdx + 1), _itms.use, topY, itmWidth, _itmHeight);
		//Organize
		{
			SI32 i; for(i = startIdx; i < _itms.use; i++){
				STISelectBoxItem* itm = NBArray_itmPtrAtIndex(&_itms, STISelectBoxItem, i);
				float xLeft = _optsMarginIn.left, xRight = itmWidth - _optsMarginIn.right;
				float itmHeight = 0.0f;
				NBCajaAABB txtBox;
				//Calculate height
				if(itm->scn.text == NULL){
					itmHeight = _fontSels->ascendenteEscena() + _fontSels->descendenteEscena() + _optsMarginIn.top + _optsMarginIn.bottom;
					txtBox.xMin = txtBox.xMax = txtBox.yMin = txtBox.yMax = 0.0f;
				} else {
					itm->scn.text->organizarTexto(xRight - xLeft);
					txtBox		= itm->scn.text->cajaAABBLocal();
					itmHeight	= (txtBox.yMax - txtBox.yMin) + _optsMarginIn.top + _optsMarginIn.bottom;
				}
				//Organize
				if(itm->scn.bg != NULL){
					itm->scn.bg->redimensionar(0.0f, 0.0f, itmWidth, -itmHeight);
				}
				if(itm->scn.icoLeft != NULL){
					const NBCajaAABB box = itm->scn.icoLeft->cajaAABBLocal();
					itm->scn.icoLeft->establecerTraslacion(xLeft - box.xMin, - box.yMax - ((itmHeight - (box.yMax - box.yMin)) * 0.5f));
					xLeft += (box.xMax - box.xMin) + _marginI;
				}
				if(itm->scn.icoRight != NULL){
					const NBCajaAABB box = itm->scn.icoRight->cajaAABBLocal();
					itm->scn.icoRight->establecerTraslacion(xRight - box.xMax, - box.yMax - ((itmHeight - (box.yMax - box.yMin)) * 0.5f));
					xRight -= (box.xMax - box.xMin) + _marginI;
				}
				if(itm->scn.text != NULL){
					itm->scn.text->establecerTraslacion(xLeft - txtBox.xMin, - txtBox.yMax - ((itmHeight - (txtBox.yMax - txtBox.yMin)) * 0.5f));
				}
				if(itm->scn.line != NULL){
					itm->scn.line->establecerMultiplicadorColor8(_visualHelp->multiplicadorColor8Func());
					itm->scn.line->establecerMultiplicadorAlpha8(150);
					itm->scn.line->moverVerticeHacia(0, 0.0f, topY - itmHeight + 1);
					itm->scn.line->moverVerticeHacia(1, itmWidth, topY - itmHeight + 1);
				}
				if(itm->scn.layer != NULL){
					itm->scn.layer->establecerLimites(0.0f, itmWidth, - itmHeight, 0.0f);
					itm->scn.layer->establecerTraslacion(0.0f, topY);
				}
				itm->scn.yTop	= topY;
				topY			-= itmHeight;
			}
			_itmOrganizedIdx	= (i - 1);
			_itmOrganizedTopY	= topY;
		}
		//Set layer fixed size
		_top.layer.opts.itms.layer->establecerLimites(0.0f, itmWidth, topY, 0.0f);
	}
}

void AUISelectBox::privOrganizeTopContent(){
	const SI32 iScene = _top.layer.obj->idEscena;
	const ENGestorEscenaGrupo iGrp = (ENGestorEscenaGrupo)_top.layer.obj->idGrupo;
	if(iScene >= 0 && iGrp >= 0){
		const NBCajaAABB sceneBox	= NBGestorEscena::cajaProyeccionGrupo(iScene, iGrp);
		NBTamano sceneSz;			NBCAJAAABB_TAMANO(sceneSz, sceneBox)
		const NBTamano scnSzInch	= NBGestorEscena::tamanoEscenaAPulgadas(iScene, sceneSz);
		//
		NBTamano topBgSzMaxScn;
		topBgSzMaxScn.ancho				= (sceneBox.xMax - sceneBox.xMin) - _optsMarginOut.right - _optsMarginOut.right;
		topBgSzMaxScn.alto				= (sceneBox.yMax - sceneBox.yMin) - _optsMarginOut.top - _optsMarginOut.bottom;
		const NBTamano topBgSzMaxInch	= NBGestorEscena::tamanoEscenaAPulgadas(iScene, topBgSzMaxScn);
		STNBSize topBgSzScn; float relY = 0.5f;
		if(topBgSzMaxInch.ancho <= 3.5f){
			//Align bottom (small device)
			topBgSzScn.width	= topBgSzMaxScn.ancho;
			topBgSzScn.height	= topBgSzMaxScn.alto * 0.60f;
			relY				= 0.0f; //bottom
		} else {
			//Align center (big device)
			topBgSzScn.width	= (topBgSzMaxScn.ancho * 3.0f) / topBgSzMaxInch.ancho;
			topBgSzScn.height	= topBgSzMaxScn.alto * 0.5f;
			relY				= 0.5f; //center
		}
		//Set bg
		{
			STNBAABox* boxOuter = &_top.layer.opts.itms.boxOuter;
			boxOuter->xMin = 0.0f;
			boxOuter->xMax = topBgSzScn.width;
			boxOuter->yMin = 0.0f;
			boxOuter->yMax = topBgSzScn.height;
			_top.layer.opts.bg->redimensionar(boxOuter->xMin, boxOuter->yMin, (boxOuter->xMax - boxOuter->xMin), (boxOuter->yMax - boxOuter->yMin));
			//Set itms
			{
				STNBAABox* boxInner = &_top.layer.opts.itms.boxInner;
				boxInner->xMin = boxOuter->xMin + _optsMarginIn.left;
				boxInner->xMax = boxOuter->xMax - _optsMarginIn.right;
				boxInner->yMin = boxOuter->yMin + _optsMarginIn.bottom;
				boxInner->yMax = boxOuter->yMax - _optsMarginIn.top;
				this->privOrganizeItmsList(-1, (boxInner->xMax - boxInner->xMin));
				_top.layer.opts.itms.layer->establecerTraslacion(boxInner->xMin - _top.layer.opts.itms.layer->cajaAABBLocal().xMin, 0.0f); //y will be corrected later
				this->privScrollTopContent(0.0f);
			}
			_top.posIn	= NBST_P(STNBPoint, sceneBox.xMin - boxOuter->xMin + (((sceneBox.xMax - sceneBox.xMin) - topBgSzScn.width) * 0.5f), sceneBox.yMin - boxOuter->yMin + _optsMarginOut.bottom + ((((sceneBox.yMax - sceneBox.yMin) - _optsMarginOut.bottom - _optsMarginOut.top) - topBgSzScn.height) * relY) );
			_top.posOut	= NBST_P(STNBPoint, _top.posIn.x, sceneBox.yMin - boxOuter->yMax - 1.0f);
		}
		//Glass
		_top.layer.glass->redimensionar(sceneBox.xMin - 1.0f, sceneBox.yMin - 1.0f, (sceneBox.xMax - sceneBox.xMin) + 2.0f, (sceneBox.yMax - sceneBox.yMin) + 2.0f);
	}
}

void AUISelectBox::privScrollTopContent(const float yScroll){
	this->privScrollTopContentToPos(_top.layer.opts.itms.layer->traslacion().y + yScroll);
}

void AUISelectBox::privScrollTopContentToItmIdx(const SI32 itmIdx){
	if(itmIdx >= 0 && itmIdx < _itms.use){
		const STISelectBoxItem* itm = NBArray_itmPtrAtIndex(&_itms, STISelectBoxItem, itmIdx);
		const STNBAABox* boxInner = &_top.layer.opts.itms.boxInner;
		const NBPunto posI	= itm->scn.layer->traslacion();
		NBCajaAABB boxI		= itm->scn.layer->cajaAABBLocal();
		boxI.xMin			+= posI.x;
		boxI.xMax			+= posI.x;
		boxI.yMin			+= posI.y;
		boxI.yMax			+= posI.y;
		this->privScrollTopContentToPos(boxInner->yMax - boxI.yMax - (((boxInner->yMax - boxInner->yMin) - (boxI.yMax - boxI.yMin)) * 0.5f));
	}
}

void AUISelectBox::privScrollTopContentToPos(const float yPos){
	const STNBAABox* boxInner = &_top.layer.opts.itms.boxInner;
	NBPunto pos		= _top.layer.opts.itms.layer->traslacion();
	NBCajaAABB box	= _top.layer.opts.itms.layer->cajaAABBLocal(); //use BGs layer to avoid holes in the glass
	pos.y			= yPos;
	//Validate limits
	{
		if(pos.y < (boxInner->yMax - box.yMax) || (box.yMax - box.yMin) <= (boxInner->yMax - boxInner->yMin)){
			pos.y = (boxInner->yMax - box.yMax);
		} else if(pos.y > (boxInner->yMin - box.yMin)){
			pos.y = (boxInner->yMin - box.yMin);
		}
	}
	//Set itms visibility
	{
		const STNBAABox* boxOuter = &_top.layer.opts.itms.boxOuter;
		const float outMarginTop = (boxOuter->yMax - boxInner->yMax);
		const float outMarginBtm = (boxInner->yMin - boxOuter->yMin);
		SI32 i; for(i = 0; i < _itms.use; i++){
			STISelectBoxItem* itm = NBArray_itmPtrAtIndex(&_itms, STISelectBoxItem, i);
			BOOL visibleI		= TRUE;
			UI8 alphaI			= 255;
			const NBPunto posI	= itm->scn.layer->traslacion();
			NBCajaAABB boxI		= itm->scn.layer->cajaAABBLocal();
			boxI.xMin			+= pos.x + posI.x;
			boxI.xMax			+= pos.x + posI.x;
			boxI.yMin			+= pos.y + posI.y;
			boxI.yMax			+= pos.y + posI.y;
			if(boxI.yMin <= boxInner->yMin){
				float relDelta	= 0.0f;
				if(outMarginBtm < -1.0f || outMarginBtm > 1.0f){
					relDelta	= (boxI.yMin - boxOuter->yMin) / outMarginBtm;
				} NBASSERT(relDelta < 1.01f)
				if(relDelta <= 0.0f){
					visibleI	= FALSE;
				} else {
					visibleI	= TRUE;
					alphaI		= 255.0f * relDelta;
				}
			} else if(boxI.yMax >= boxInner->yMax){
				float relDelta	= 0.0f;
				if(outMarginBtm < -1.0f || outMarginBtm > 1.0f){
					relDelta	= (boxOuter->yMax - boxI.yMax) / outMarginTop;
				} NBASSERT(relDelta < 1.01f)
				if(relDelta <= 0.0f){
					visibleI	= FALSE;
				} else {
					visibleI	= TRUE;
					alphaI		= 255.0f * relDelta;
				}
			} else {
				visibleI		= TRUE;
				alphaI			= 255;
			}
			itm->scn.layer->establecerVisible(visibleI);
			itm->scn.line->establecerVisible(visibleI);
			itm->scn.layer->establecerMultiplicadorAlpha8(alphaI);
			itm->scn.line->establecerMultiplicadorAlpha8(alphaI);
		}
	}
	_top.layer.opts.itms.layer->establecerTraslacion(pos.x, pos.y); //preserve-x
}

//

void AUISelectBox::puertoDeVisionModificado(const SI32 iEscena, const STNBViewPortSize before, const STNBViewPortSize after){
	if(_onFocus){
		this->privOrganizeTopContent();
	}
}

//

void AUISelectBox::privTopAnimate(const float secs){
	if(_top.anim.secsDur > 0.0f){
		_top.anim.secsCur	+= secs;
		float rel			= (_top.anim.secsCur / _top.anim.secsDur);
		if(_top.anim.secsCur >= _top.anim.secsDur){
			rel				= 1.0f;
			_top.anim.secsCur = 0.0f;
			_top.anim.secsDur = 0.0f;
		} NBASSERT(rel >= 0.0f && rel <= 1.0f)
		//Apply acceleration
		if(_top.anim.isAnimIn){
			NBPunto pos;
			//const float relAcel = (rel * rel); //accelerated
			const float relAcel = pow(rel, 1.0f / 4.0f); //deaccelerated
			pos.x = _top.posOut.x + ((_top.posIn.x - _top.posOut.x) * relAcel);
			pos.y = _top.posOut.y + ((_top.posIn.y - _top.posOut.y) * relAcel);
			_top.layer.opts.layer->establecerTraslacion(pos);
			_top.layer.glass->establecerMultiplicadorAlpha8(200.0f * rel);
			//PRINTF_INFO("Anim(%.2f) pos(%f, %f).\n", rel, pos.x, pos.y);
		} else {
			NBPunto pos;
			const float relAcel = 1.0f - (rel * rel); //accelerated
			pos.x = _top.posOut.x + ((_top.posIn.x - _top.posOut.x) * relAcel);
			pos.y = _top.posOut.y + ((_top.posIn.y - _top.posOut.y) * relAcel);
			_top.layer.opts.layer->establecerTraslacion(pos);
			_top.layer.glass->establecerMultiplicadorAlpha8(200.0f * (1.0f - rel));
			//PRINTF_INFO("Anim(%.2f) pos(%f, %f).\n", rel, pos.x, pos.y);
			//Remove from top
			if(rel >= 1.0f){
				this->privRemoveFromTopPriv();
			}
		}
	}
}

void AUISelectBox::privStartAddToTop(){
	if(!_onFocus){
		BOOL addToTop = TRUE;
		if(_lstnr != NULL){
			addToTop = _lstnr->selectboxFocusObtaining(this);
		}
		if(addToTop){
			if(this->addToTop(_top.layer.obj)){
				NBASSERT(_top.layer.obj->idEscena >= 0 && _top.layer.obj->idGrupo >= 0)
				this->privOrganizeTopContent();
				NBGestorEscena::agregarEscuchadorCambioPuertoVision(this->idEscena, this);
				NBGestorAnimadores::agregarAnimador(this, this);
				//stop scroll
				{
					_top.touchDeltaYLast	= 0.0f;	//deltaY from last tick (to add to velocity)
					_top.scrollSpeedY		= 0.0f ;	//cur scroll speed
				}
				//Init in-animation
				{
					_top.anim.isAnimIn	= TRUE;
					_top.anim.secsCur	= 0.0f;
					_top.anim.secsDur	= AUISELECT_BOX_ANIM_DURATION;
					this->privTopAnimate(0.0f); //initial location
					this->privScrollTopContentToItmIdx(_itmIdxSeletected);
				}
				_onFocus = TRUE;
				if(_lstnr != NULL){
					_lstnr->selectboxFocusObtained(this);
				}
			}
		}
	}
}

void AUISelectBox::privRemoveFromTopPriv(){
	NBASSERT((_onFocus && _top.layer.obj->contenedor() != NULL) || (!_onFocus && _top.layer.obj->contenedor() == NULL))
	if(_onFocus){
		NBGestorAnimadores::quitarAnimador(this);
		NBGestorEscena::quitarEscuchadorCambioPuertoVision(this->idEscena, this);
		//Remove from focus (from topLayer)
		if(_top.layer.obj != NULL){
			AUEscenaContenedor* parent = (AUEscenaContenedor*)_top.layer.obj->contenedor(); NBASSERT(parent != NULL)
			if(parent != NULL) parent->quitarObjetoEscena(_top.layer.obj);
		}
		_onFocus = FALSE;
		if(_lstnr != NULL){
			_lstnr->selectboxFocusLost(this);
		}
	}
}

//

void AUISelectBox::tickAnimacion(float segsTranscurridos){
	//Animate
	if(_top.anim.secsDur > 0.0f){
		this->privTopAnimate(segsTranscurridos);
	}
	//Consume last deltaY
	{
		_top.touchDeltaYAccumSecs += segsTranscurridos;
		if((_top.touchDeltaYAccum > 0.0f && _top.touchDeltaYLast < 0.0f) || (_top.touchDeltaYAccum < 0.0f && _top.touchDeltaYLast > 0.0f)){
			//Direction changed
			_top.scrollSpeedY			= 0.0f;
			_top.touchDeltaYAccum		= 0.0f;
			_top.touchDeltaYAccumSecs	= 0.0f;
		} else {
			//Acumulate this (is same direction)
			_top.touchDeltaYAccum		+= _top.touchDeltaYLast;
		}
		_top.touchDeltaYLast			= 0.0f;
	}
	//Scroll (only if not touching)
	if(_top.touchFirst == NULL){
		//Apply accumulated speed
		if(_top.touchDeltaYAccum != 0.0f){
			NBASSERT(_top.touchDeltaYAccumSecs > 0.0f)
			if(_top.touchDeltaYAccumSecs != 0.0f){
				_top.scrollSpeedY		+= (_top.touchDeltaYAccum / _top.touchDeltaYAccumSecs);
				//PRINTF_INFO("_top.touchDeltaYLast(%f); prevScrollSpeedY(%f) => _top.scrollSpeedY(%f).\n", _top.touchDeltaYLast, prevScrollSpeedY, _top.scrollSpeedY);
			}
			_top.touchDeltaYAccum		= 0.0f;
			_top.touchDeltaYAccumSecs	= 0.0f;
		}
		//Apply scroll
		if(_top.scrollSpeedY != 0.0f){
			//PRINTF_INFO("_top.scrollSpeedY(%f).\n", _top.scrollSpeedY);
			this->privScrollTopContent(_top.scrollSpeedY * segsTranscurridos);
			_top.scrollSpeedY -= (_top.scrollSpeedY * pow(segsTranscurridos, 2.0f / 3.0f));
			if(_top.scrollSpeedY >= -1.0f && _top.scrollSpeedY <= 1.0f){
				_top.scrollSpeedY = 0.0f;
			}
		}
	}
}

//TOUCHES

void AUISelectBox::touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto){
	if(_bg == objeto){
		_bg->establecerMultiplicadorColor8(225, 225, 225, 255);
	} else if(_top.layer.glass == objeto){
		//Will hide
	} else if(_top.layer.opts.layer == objeto){
		if(_top.touchFirst == NULL){ //Must be the first touch
			const NBPunto pos		= _top.layer.opts.itms.layer->traslacion();
			_top.touchFirst			= touch;
			_top.touchItmsStartPos.x = pos.x;
			_top.touchItmsStartPos.y = pos.y;
		}
	} else {
		SI32 i; for(i = 0; i < _itms.use; i++){
			STISelectBoxItem* itm = NBArray_itmPtrAtIndex(&_itms, STISelectBoxItem, i);
			if(itm->scn.layer == objeto){
				if(i == _itmIdxSeletected){
					itm->scn.bg->establecerMultiplicadorColor8(225, 200, 200, 255);
				} else {
					itm->scn.bg->establecerMultiplicadorColor8(225, 225, 225, 255);
				}
				break;
			}
		}
	}
	//Stop scroll
	_top.scrollSpeedY = 0.0f;
	_top.touchDeltaYLast = 0.0f;
	_top.touchDeltaYAccum = 0.0f;
	_top.touchDeltaYAccumSecs = 0.0f;
}

void AUISelectBox::touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	if(_bg == objeto){
		//nothing
	} else if(_top.layer.glass == objeto){
		//Will hide
	} else if(_top.layer.opts.layer == objeto){
		if(_top.touchFirst == touch){ //Must be the first touch
			this->privScrollTopContentToPos(_top.touchItmsStartPos.y + (posActualEscena.y - posInicialEscena.y));
			_top.touchDeltaYLast = (posActualEscena.y - posAnteriorEscena.y);
		}
	} else {
		SI32 i; for(i = 0; i < _itms.use; i++){
			STISelectBoxItem* itm = NBArray_itmPtrAtIndex(&_itms, STISelectBoxItem, i);
			if(itm->scn.layer == objeto){
				NBTamano deltaScn;
				deltaScn.ancho	= posActualEscena.x - posInicialEscena.x;
				deltaScn.alto	= posActualEscena.y - posInicialEscena.y;
				const NBTamano deltaInch = NBGestorEscena::tamanoEscenaAPulgadas(this->idEscena, deltaScn);
				const float distInch = sqrtf((deltaInch.ancho * deltaInch.ancho) + (deltaInch.alto * deltaInch.alto)); NBASSERT(distInch >= 0.0f)
				if(distInch > 0.10f){
					objeto->liberarTouch(touch, posInicialEscena, posAnteriorEscena, posActualEscena, TRUE, _top.layer.opts.layer);
				}
				break;
			}
		}
	}
}

void AUISelectBox::touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	STISelectBoxItem* itmTouched = NULL;
	if(_bg == objeto){
		_bg->establecerMultiplicadorColor8(255, 255, 255, 255);
	} else if(_top.layer.glass == objeto){
		if(_top.anim.secsDur == 0){
			//Init out-animation
			_top.anim.isAnimIn	= FALSE;
			_top.anim.secsCur	= 0.0f;
			_top.anim.secsDur	= AUISELECT_BOX_ANIM_DURATION;
			this->privTopAnimate(0.0f); //initial location
		} else if(_top.anim.isAnimIn){
			//Invert current animation
			NBASSERT(_top.anim.secsCur < _top.anim.secsDur)
			_top.anim.isAnimIn	= FALSE;
			_top.anim.secsCur	= (_top.anim.secsDur - _top.anim.secsCur);
		}
	} else if(_top.layer.opts.layer == objeto){
		if(_top.touchFirst == touch){ //Must be the first touch
			this->privScrollTopContentToPos(_top.touchItmsStartPos.y + (posActualEscena.y - posInicialEscena.y));
			_top.touchDeltaYLast = (posActualEscena.y - posAnteriorEscena.y);
			_top.touchFirst = NULL;
		}
	} else {
		SI32 i; for(i = 0; i < _itms.use; i++){
			STISelectBoxItem* itm = NBArray_itmPtrAtIndex(&_itms, STISelectBoxItem, i);
			if(itm->scn.layer == objeto){
				if(i == _itmIdxSeletected){
					itm->scn.bg->establecerMultiplicadorColor8(255, 225, 225, 255);
				} else {
					itm->scn.bg->establecerMultiplicadorColor8(255, 255, 255, 255);
				}
				//PRINTF_INFO("Texto: '%s'.\n", itm->text.str);
				itmTouched = itm;
				break;
			}
		}
	}
	//Process
	if(!touch->cancelado){
		const NBCajaAABB cajaEscena = objeto->cajaAABBEnEscena();
		if(NBCAJAAABB_INTERSECTA_PUNTO(cajaEscena, posActualEscena.x, posActualEscena.y) && !touch->cancelado){
			if(itmTouched != NULL){
				//Select itm
				NBASSERT(_itmIdxSeletected >= -1 && _itmIdxSeletected < _itms.use)
				const SI32 iSel = (SI32)(itmTouched - NBArray_itmPtrAtIndex(&_itms, STISelectBoxItem, 0)); NBASSERT(iSel >= 0 && iSel < _itms.use)
				this->setSelectedIdx(iSel);
				//Init out-animation
				{
					_top.anim.isAnimIn	= FALSE;
					_top.anim.secsCur	= 0.0f;
					_top.anim.secsDur	= AUISELECT_BOX_ANIM_DURATION;
					this->privTopAnimate(0.0f); //initial location
				}
			} else if(_bg == objeto){
				if(_isEnabled){
					this->privStartAddToTop();
				}
			}
		}
	}
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUISelectBox, AUEscenaContenedor)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUISelectBox, "AUISelectBox", AUEscenaContenedor)
AUOBJMETODOS_CLONAR_NULL(AUISelectBox)


