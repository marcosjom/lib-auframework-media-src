//
//  AUIDateBox.cpp
//  auframework-media-ios
//
//  Created by Marcos Ortega on 9/3/18.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUIDateBox.h"
//
#include "nb/core/NBMemory.h"
#include "nb/core/NBString.h"
#include "NBGestorAnimadores.h"
#include "NBGestorEscena.h"

#define AUISELECT_BOX_ANIM_DURATION 0.30

static const char* mNames[] = {
	"January", "February", "March", "April"
	, "May", "Jun", "July", "August"
	, "September", "October", "November", "December"
};

AUIDateBox::AUIDateBox(AUFuenteRender* font, AUFuenteRender* fontSels, AUTextura* selFrameTex, AUTextura* icoDropMore, AUTextura* icoRightSelect, const float boxWidth, const float marginI, const UI32 firstYear, const UI16 ammYears, const BOOL includeEmptyOpts)
: AUEscenaContenedor()
{
	_marginI	= marginI;
	_margins.left = _margins.right = _margins.top = _margins.bottom	= 0;
	_boxWidth	= boxWidth;
	_onFocus	= FALSE;
	_font		= font; if(_font != NULL) _font->retener(NB_RETENEDOR_THIS);
	_fontSels	= fontSels; if(_fontSels != NULL) _fontSels->retener(NB_RETENEDOR_THIS);
	//
	NBMemory_setZero(_dateSel);
	_textColor	= NBST_P(STNBColor8, 0, 0, 0, 255 );
	//Options
	_optsIcoRight	= NULL;
	_optsBgTex		= NULL;
	_optsMarginIn.left = _optsMarginIn.right = _optsMarginIn.top = _optsMarginIn.bottom	= 0;
	_optsMarginOut.left = _optsMarginOut.right = _optsMarginOut.top = _optsMarginOut.bottom	= 0;
	//Items
	{
		_top.layer.obj = new(this) AUEscenaContenedor();
		{
			_top.layer.glass = new(this) AUEscenaSprite();
			_top.layer.glass->establecerMultiplicadorColor8(0, 0, 0, 220);
			_top.layer.glass->establecerEscuchadorTouches(this, this);
			_top.layer.obj->agregarObjetoEscena(_top.layer.glass);
		}
		{
			_top.layer.opts.layer = new(this) AUEscenaContenedor();
			_top.layer.obj->agregarObjetoEscena(_top.layer.opts.layer);
		}
		{
			_top.layer.opts.bg = new(this) AUEscenaSpriteElastico();
			_top.layer.opts.bg->establecerEscuchadorTouches(this, this);
			_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.bg);
		}
		{
			_top.layer.opts.boxOuter	= NBST_P(STNBAABox, 0, 0, 0, 0 );
			_top.layer.opts.boxInner	= NBST_P(STNBAABox, 0, 0, 0, 0 );
			_top.layer.opts.boxSelect	= NBST_P(STNBAABox, 0, 0, 0, 0 );
		}
		{
			SI32 iComp; for(iComp = 0; iComp < ENDateBoxComp_Count; iComp++){
				NBArray_init(&_itms[iComp].array, sizeof(STIDateBoxItem), NULL);
				{
					_top.layer.opts.itms[iComp].layer = new(this) AUEscenaContenedorLimitado();
					_top.layer.opts.itms[iComp].layer->establecerEscuchadorTouches(this, this);
					_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.itms[iComp].layer);
					{
						_top.layer.opts.itms[iComp].valuesLayer = new(this) AUEscenaContenedor();
						_top.layer.opts.itms[iComp].layer->agregarObjetoEscena(_top.layer.opts.itms[iComp].valuesLayer);
						_top.layer.opts.itms[iComp].linesLayer	= new(this) AUEscenaContenedor();
						//_top.layer.opts.itms[iComp].layer->agregarObjetoEscena(_top.layer.opts.itms[iComp].linesLayer);
					}
				}
				_itms[iComp].width				= 0.0f;
				_itms[iComp].idxSeletected		= -1;
				{
					_itms[iComp].anim.yStart	= 0.0f;
					_itms[iComp].anim.yEnd		= 0.0f;
					_itms[iComp].anim.secsDur	= 0.0f;
					_itms[iComp].anim.secsCur	= 0.0f;
				}
				//
				_touch[iComp].first				= NULL;
				_touch[iComp].itmsStartPos		= NBST_P(STNBPoint, 0, 0 );
				_touch[iComp].deltaYLast 		= 0.0f;	//deltaY from last tick (to add to velocity)
				_touch[iComp].deltaYAccum		= 0.0f;
				_touch[iComp].deltaYAccumSecs	= 0.0f;
				_touch[iComp].scrollSpeedY		= 0.0f ;	//cur scroll speed
				_touch[iComp].scrollDirtyMoved	= FALSE;
			}
		}
		{
			_top.layer.opts.selFrame = new(this) AUEscenaSpriteElastico(selFrameTex);
			_top.layer.opts.selFrame->establecerEscuchadorTouches(this, this);
			_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.selFrame);
		}
		if(icoRightSelect != NULL){
			_optsIcoRight = new(this) AUEscenaSprite(icoRightSelect);
			_top.layer.opts.layer->agregarObjetoEscena(_optsIcoRight);
		}
	}
	//Top
	{
		_top.posIn				= NBST_P(STNBPoint, 0, 0 );
		_top.posOut				= NBST_P(STNBPoint, 0, 0 );
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
		const STNBRect column = {0, 0, _boxWidth - _margins.left - _margins.right, _font->ascendenteEscena() + _font->descendenteEscena() };
		_visual		= new(this) AUEscenaTexto(_font);
		_visual->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_visual->establecerMultiplicadorColor8(0, 0, 0, 255);
		_visual->establecerVisible(FALSE);
		_visualHelp->establecerTextoEnCol("-- / ---- / -----", column);
		this->agregarObjetoEscena(_visual);
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
		_visualHelp->establecerTextoEnCol("-- / ---- / -----", column);
		_visualHelp->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_visualHelp->establecerMultiplicadorColor8(155, 155, 155, 255);
		this->agregarObjetoEscena(_visualHelp);
	}
	_lstnr	= NULL;
	//
	this->privListsInit(firstYear, ammYears, includeEmptyOpts);
	this->privOrganizar();
}

AUIDateBox::AUIDateBox(AUFuenteRender* font, AUFuenteRender* fontSels, AUTextura* selFrameTex, AUTextura* icoDropMore, AUTextura* icoRightSelect, const float boxWidth, const float marginI, const NBMargenes margins, const UI32 firstYear, const UI16 ammYears, const BOOL includeEmptyOpts)
: AUEscenaContenedor()
{
	_marginI	= marginI;
	_margins	= margins;
	_boxWidth	= boxWidth;
	_onFocus	= FALSE;
	_font		= font; if(_font != NULL) _font->retener(NB_RETENEDOR_THIS);
	_fontSels	= fontSels; if(_fontSels != NULL) _fontSels->retener(NB_RETENEDOR_THIS);
	//
	NBMemory_setZero(_dateSel);
	_textColor	= NBST_P(STNBColor8, 0, 0, 0, 255 );
	//Options
	_optsIcoRight	= NULL;
	_optsBgTex		= NULL;
	_optsMarginIn.left = _optsMarginIn.right = _optsMarginIn.top = _optsMarginIn.bottom	= 0;
	_optsMarginOut.left = _optsMarginOut.right = _optsMarginOut.top = _optsMarginOut.bottom	= 0;
	//Items
	{
		_top.layer.obj = new(this) AUEscenaContenedor();
		{
			_top.layer.glass = new(this) AUEscenaSprite();
			_top.layer.glass->establecerMultiplicadorColor8(0, 0, 0, 220);
			_top.layer.glass->establecerEscuchadorTouches(this, this);
			_top.layer.obj->agregarObjetoEscena(_top.layer.glass);
		}
		{
			_top.layer.opts.layer = new(this) AUEscenaContenedor();
			_top.layer.obj->agregarObjetoEscena(_top.layer.opts.layer);
		}
		{
			_top.layer.opts.bg = new(this) AUEscenaSpriteElastico();
			_top.layer.opts.bg->establecerEscuchadorTouches(this, this);
			_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.bg);
		}
		{
			_top.layer.opts.boxOuter	= NBST_P(STNBAABox, 0, 0, 0, 0 );
			_top.layer.opts.boxInner	= NBST_P(STNBAABox, 0, 0, 0, 0 );
			_top.layer.opts.boxSelect	= NBST_P(STNBAABox, 0, 0, 0, 0 );
		}
		{
			SI32 iComp; for(iComp = 0; iComp < ENDateBoxComp_Count; iComp++){
				NBArray_init(&_itms[iComp].array, sizeof(STIDateBoxItem), NULL);
				{
					_top.layer.opts.itms[iComp].layer	= new(this) AUEscenaContenedorLimitado();
					_top.layer.opts.itms[iComp].layer->establecerEscuchadorTouches(this, this);
					_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.itms[iComp].layer);
					{
						_top.layer.opts.itms[iComp].valuesLayer = new(this) AUEscenaContenedor();
						_top.layer.opts.itms[iComp].layer->agregarObjetoEscena(_top.layer.opts.itms[iComp].valuesLayer);
						_top.layer.opts.itms[iComp].linesLayer	= new(this) AUEscenaContenedor();
						//_top.layer.opts.itms[iComp].layer->agregarObjetoEscena(_top.layer.opts.itms[iComp].linesLayer);
					}
				}
				_itms[iComp].width				= 0.0f;
				_itms[iComp].idxSeletected		= -1;
				{
					_itms[iComp].anim.yStart	= 0.0f;
					_itms[iComp].anim.yEnd		= 0.0f;
					_itms[iComp].anim.secsDur	= 0.0f;
					_itms[iComp].anim.secsCur	= 0.0f;
				}
				//
				_touch[iComp].first				= NULL;
				_touch[iComp].itmsStartPos		= NBST_P(STNBPoint, 0, 0 );
				_touch[iComp].deltaYLast 		= 0.0f;	//deltaY from last tick (to add to velocity)
				_touch[iComp].deltaYAccum		= 0.0f;
				_touch[iComp].deltaYAccumSecs	= 0.0f;
				_touch[iComp].scrollSpeedY		= 0.0f ;	//cur scroll speed
				_touch[iComp].scrollDirtyMoved	= FALSE;
			}
		}
		{
			_top.layer.opts.selFrame = new(this) AUEscenaSpriteElastico(selFrameTex);
			_top.layer.opts.selFrame->establecerEscuchadorTouches(this, this);
			_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.selFrame);
		}
		if(icoRightSelect != NULL){
			_optsIcoRight = new(this) AUEscenaSprite(icoRightSelect);
			_top.layer.opts.layer->agregarObjetoEscena(_optsIcoRight);
		}
	}
	//Top
	{
		_top.posIn				= NBST_P(STNBPoint, 0, 0 );
		_top.posOut				= NBST_P(STNBPoint, 0, 0 );
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
		const STNBRect column = {0, 0, _boxWidth - _margins.left - _margins.right, _font->ascendenteEscena() + _font->descendenteEscena() };
		_visual		= new(this) AUEscenaTexto(_font);
		_visual->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_visual->establecerMultiplicadorColor8(0, 0, 0, 255);
		_visual->establecerTextoEnCol("-- / ---- / -----", column);
		_visual->establecerVisible(FALSE);
		this->agregarObjetoEscena(_visual);
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
		_visualHelp->establecerTextoEnCol("-- / ---- / -----", column);
		_visualHelp->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_visualHelp->establecerMultiplicadorColor8(155, 155, 155, 255);
		this->agregarObjetoEscena(_visualHelp);
	}
	_lstnr	= NULL;
	//
	this->privListsInit(firstYear, ammYears, includeEmptyOpts);
	this->privOrganizar();
}

AUIDateBox::AUIDateBox(AUFuenteRender* font, AUFuenteRender* fontSels, AUTextura* selFrameTex, AUTextura* icoDropMore, AUTextura* icoRightSelect, const float boxWidth, const float marginI, const float mLeft, const float mRight, const float mTop, const float mBottom, const UI32 firstYear, const UI16 ammYears, const BOOL includeEmptyOpts)
: AUEscenaContenedor()
{
	_marginI		= marginI;
	_margins.left	= mLeft;
	_margins.right	= mRight;
	_margins.top	= mTop;
	_margins.bottom	= mBottom;
	_boxWidth		= boxWidth;
	_onFocus		= FALSE;
	_font			= font; if(_font != NULL) _font->retener(NB_RETENEDOR_THIS);
	_fontSels		= fontSels; if(_fontSels != NULL) _fontSels->retener(NB_RETENEDOR_THIS);
	//
	NBMemory_setZero(_dateSel);
	_textColor		= NBST_P(STNBColor8, 0, 0, 0, 255 );
	//Options
	_optsIcoRight	= NULL;
	_optsBgTex		= NULL;
	_optsMarginIn.left = _optsMarginIn.right = _optsMarginIn.top = _optsMarginIn.bottom	= 0;
	_optsMarginOut.left = _optsMarginOut.right = _optsMarginOut.top = _optsMarginOut.bottom	= 0;
	//Items
	{
		_top.layer.obj = new(this) AUEscenaContenedor();
		{
			_top.layer.glass = new(this) AUEscenaSprite();
			_top.layer.glass->establecerMultiplicadorColor8(0, 0, 0, 220);
			_top.layer.glass->establecerEscuchadorTouches(this, this);
			_top.layer.obj->agregarObjetoEscena(_top.layer.glass);
		}
		{
			_top.layer.opts.layer = new(this) AUEscenaContenedor();
			_top.layer.obj->agregarObjetoEscena(_top.layer.opts.layer);
		}
		{
			_top.layer.opts.bg = new(this) AUEscenaSpriteElastico();
			_top.layer.opts.bg->establecerEscuchadorTouches(this, this);
			_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.bg);
		}
		{
			SI32 iComp; for(iComp = 0; iComp < ENDateBoxComp_Count; iComp++){
				NBArray_init(&_itms[iComp].array, sizeof(STIDateBoxItem), NULL);
				{
					_top.layer.opts.itms[iComp].layer = new(this) AUEscenaContenedorLimitado();
					_top.layer.opts.itms[iComp].layer->establecerEscuchadorTouches(this, this);
					_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.itms[iComp].layer);
					_top.layer.opts.boxOuter	= NBST_P(STNBAABox, 0, 0, 0, 0 );
					_top.layer.opts.boxInner	= NBST_P(STNBAABox, 0, 0, 0, 0 );
					_top.layer.opts.boxSelect	= NBST_P(STNBAABox, 0, 0, 0, 0 );
					{
						_top.layer.opts.itms[iComp].valuesLayer = new(this) AUEscenaContenedor();
						_top.layer.opts.itms[iComp].layer->agregarObjetoEscena(_top.layer.opts.itms[iComp].valuesLayer);
						_top.layer.opts.itms[iComp].linesLayer	= new(this) AUEscenaContenedor();
						//_top.layer.opts.itms[iComp].layer->agregarObjetoEscena(_top.layer.opts.itms[iComp].linesLayer);
					}
				}
				_itms[iComp].width				= 0.0f;
				_itms[iComp].idxSeletected		= -1;
				{
					_itms[iComp].anim.yStart	= 0.0f;
					_itms[iComp].anim.yEnd		= 0.0f;
					_itms[iComp].anim.secsDur	= 0.0f;
					_itms[iComp].anim.secsCur	= 0.0f;
				}
				//
				_touch[iComp].first				= NULL;
				_touch[iComp].itmsStartPos		= NBST_P(STNBPoint, 0, 0 );
				_touch[iComp].deltaYLast		= 0.0f;	//deltaY from last tick (to add to velocity)
				_touch[iComp].deltaYAccum		= 0.0f;
				_touch[iComp].deltaYAccumSecs	= 0.0f;
				_touch[iComp].scrollSpeedY		= 0.0f;	//cur scroll speed
				_touch[iComp].scrollDirtyMoved	= FALSE;
			}
		}
		{
			_top.layer.opts.selFrame = new(this) AUEscenaSpriteElastico(selFrameTex);
			_top.layer.opts.selFrame->establecerEscuchadorTouches(this, this);
			_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.selFrame);
		}
		if(icoRightSelect != NULL){
			_optsIcoRight = new(this) AUEscenaSprite(icoRightSelect);
			_top.layer.opts.layer->agregarObjetoEscena(_optsIcoRight);
		}
	}
	//Top
	{
		_top.posIn				= NBST_P(STNBPoint, 0, 0 );
		_top.posOut				= NBST_P(STNBPoint, 0, 0 );
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
		const STNBRect column = {0, 0, _boxWidth - _margins.left - _margins.right, _font->ascendenteEscena() + _font->descendenteEscena() };
		_visual		= new(this) AUEscenaTexto(_font);
		_visual->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_visual->establecerMultiplicadorColor8(0, 0, 0, 255);
		_visual->establecerTextoEnCol("-- / ---- / -----", column);
		_visual->establecerVisible(FALSE);
		this->agregarObjetoEscena(_visual);
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
		_visualHelp->establecerTextoEnCol("-- / ---- / -----", column);
		_visualHelp->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_visualHelp->establecerMultiplicadorColor8(155, 155, 155, 255);
		this->agregarObjetoEscena(_visualHelp);
	}
	_lstnr	= NULL;
	//
	this->privListsInit(firstYear, ammYears, includeEmptyOpts);
	this->privOrganizar();
}

AUIDateBox::~AUIDateBox(){
	_marginI	= 0.0f;
	_margins.left = _margins.right = _margins.top = _margins.bottom	= 0.0f;
	_boxWidth	= 0.0f;
	//Remove from focus
	this->privRemoveFromTopPriv();
	//Options
	{
		if(_optsIcoRight != NULL) _optsIcoRight->liberar(NB_RETENEDOR_THIS); _optsIcoRight = NULL;
		if(_optsBgTex != NULL) _optsBgTex->liberar(NB_RETENEDOR_THIS); _optsBgTex = NULL;
	}
	//Items
	{
		
		if(_top.layer.obj != NULL) _top.layer.obj->liberar(NB_RETENEDOR_THIS); _top.layer.obj = NULL;
		if(_top.layer.glass != NULL) _top.layer.glass->liberar(NB_RETENEDOR_THIS); _top.layer.glass = NULL;
		if(_top.layer.opts.layer != NULL) _top.layer.opts.layer->liberar(NB_RETENEDOR_THIS); _top.layer.opts.layer = NULL;
		if(_top.layer.opts.bg != NULL) _top.layer.opts.bg->liberar(NB_RETENEDOR_THIS); _top.layer.opts.bg = NULL;
		if(_top.layer.opts.selFrame != NULL) _top.layer.opts.selFrame->liberar(NB_RETENEDOR_THIS); _top.layer.opts.selFrame = NULL;
		{
			this->privListsEmpty();
			SI32 iComp; for(iComp = 0; iComp < ENDateBoxComp_Count; iComp++){
				NBArray_release(&_itms[iComp].array);
				if(_top.layer.opts.itms[iComp].layer != NULL) _top.layer.opts.itms[iComp].layer->liberar(NB_RETENEDOR_THIS); _top.layer.opts.itms[iComp].layer = NULL;
				if(_top.layer.opts.itms[iComp].valuesLayer != NULL) _top.layer.opts.itms[iComp].valuesLayer->liberar(NB_RETENEDOR_THIS); _top.layer.opts.itms[iComp].valuesLayer = NULL;
				if(_top.layer.opts.itms[iComp].linesLayer != NULL) _top.layer.opts.itms[iComp].linesLayer->liberar(NB_RETENEDOR_THIS); _top.layer.opts.itms[iComp].linesLayer = NULL;
				//
				_touch[iComp].first		= NULL;
				_touch[iComp].itmsStartPos = NBST_P(STNBPoint, 0, 0 );
			}
		}
	}
	if(_font != NULL) _font->liberar(NB_RETENEDOR_THIS); _font = NULL;
	if(_fontSels != NULL) _fontSels->liberar(NB_RETENEDOR_THIS); _fontSels = NULL;
	if(_bg != NULL) _bg->liberar(NB_RETENEDOR_THIS); _bg = NULL;
	//
	if(_visual != NULL) _visual->liberar(NB_RETENEDOR_THIS); _visual = NULL;
	if(_visualIcoMore != NULL) _visualIcoMore->liberar(NB_RETENEDOR_THIS); _visualIcoMore = NULL;
	if(_visualHelp != NULL) _visualHelp->liberar(NB_RETENEDOR_THIS); _visualHelp = NULL;
	//
	_lstnr	= NULL;
}

//

void AUIDateBox::agregadoEnEscena(){
	AUEscenaContenedor::agregadoEnEscena();
	NBASSERT(!_onFocus) //cant have the focus
	//if(this->idEscena != -1){
	//	NBGestorEscena::habilitarEscena(_subscn.Id);
	//}
}

void AUIDateBox::quitandoDeEscena(){
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

AUTextura* AUIDateBox::getOptionsBgTexture() const {			//options bg textye
	return _optsBgTex;
}

NBMargenes AUIDateBox::getOptionsMarginIn() const {		//options inner margin
	return _optsMarginIn;
}

NBMargenes AUIDateBox::getOptionsMarginOut() const {	//options outer margin
	return _optsMarginOut;
}

void AUIDateBox::setOptionsBgTexture(AUTextura* tex){	//options bg textye
	if(tex != NULL) tex->retener(NB_RETENEDOR_THIS);
	if(_optsBgTex != NULL) _optsBgTex->liberar(NB_RETENEDOR_THIS);
	_optsBgTex = tex;
	_top.layer.opts.bg->establecerTextura(_optsBgTex);
}

void AUIDateBox::setOptionsMarginsIn(const NBMargenes margins){ //options inner margin
	_optsMarginIn			= margins;
	//ToDo: Update list if visible on top (mantaining current selection)
}

void AUIDateBox::setOptionsMarginsIn(const float mLeft, const float mRight, const float mTop, const float mBottom){ //options inner margin
	_optsMarginIn.left		= mLeft;
	_optsMarginIn.right		= mRight;
	_optsMarginIn.top		= mTop;
	_optsMarginIn.bottom	= mBottom;
	//ToDo: Update list if visible on top (mantaining current selection)
}

void AUIDateBox::setOptionsMarginsOut(const NBMargenes margins){ //options outer margin
	_optsMarginOut = margins;
}

void AUIDateBox::setOptionsMarginsOut(const float mLeft, const float mRight, const float mTop, const float mBottom){ //options outer margin
	_optsMarginOut.left		= mLeft;
	_optsMarginOut.right	= mRight;
	_optsMarginOut.top		= mTop;
	_optsMarginOut.bottom	= mBottom;
}

//List

void AUIDateBox::privListsInit(const UI32 firstYear, const UI16 ammYears, const BOOL includeEmptyOpts){
	STNBString str;
	NBString_init(&str);
	//Days
	{
		if(includeEmptyOpts){
			this->privListAddItm(ENDateBoxComp_Day, "--", 0);
		}
		SI32 i; for(i = 0 ; i < 31; i++){
			NBString_empty(&str);
			NBString_concatSI32(&str, (i + 1));
			this->privListAddItm(ENDateBoxComp_Day, str.str, (i + 1));
		}
	}
	//Months
	{
		if(includeEmptyOpts){
			this->privListAddItm(ENDateBoxComp_Month, "----", 0);
		}
		SI32 i; for(i = 0 ; i < 12; i++){
			NBString_empty(&str);
			NBString_concat(&str, mNames[i]);
			this->privListAddItm(ENDateBoxComp_Month, str.str, (i + 1));
		}
	}
	//Years
	{
		if(includeEmptyOpts){
			this->privListAddItm(ENDateBoxComp_Year, "----", 0);
		}
		SI32 i; for(i = 0 ; i < ammYears; i++){
			NBString_empty(&str);
			NBString_concatSI32(&str, (firstYear + i));
			this->privListAddItm(ENDateBoxComp_Year, str.str, (firstYear + i));
		}
	}
	NBString_release(&str);
}

void AUIDateBox::privListsEmpty(){
	{
		SI32 iComp; for(iComp = 0; iComp < ENDateBoxComp_Count; iComp++){
			SI32 i; for (i = ((SI32)_itms[iComp].array.use - 1); i >= 0; i--){
				STIDateBoxItem* itm = NBArray_itmPtrAtIndex(&_itms[iComp].array, STIDateBoxItem, i);
				NBString_release(&itm->text);
				//scene
				{
					if(itm->scn.line != NULL){
						AUEscenaContenedor* parent = (AUEscenaContenedor*)itm->scn.line->contenedor();
						if(parent != NULL) parent->quitarObjetoEscena(itm->scn.line);
						itm->scn.line->liberar(NB_RETENEDOR_THIS);
						itm->scn.line = NULL;
					}
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
			NBArray_empty(&_itms[iComp].array);
			_itms[iComp].idxSeletected = -1;
		}
	}
	{
		_visual->establecerVisible(FALSE);
		_visualHelp->establecerVisible(TRUE);
	}
}

BOOL AUIDateBox::privListAddItm(const ENDateBoxComp iComp, const char* text, const SI32 value){
	BOOL r = FALSE;
	{
		STIDateBoxItem itm;
		//Data
		{
			NBString_initWithStr(&itm.text, (text != NULL ? text : ""));
			itm.value = value;
		}
		//Scene
		{
			itm.scn.yTop		= 0.0f;
			itm.scn.layer		= new(this) AUEscenaContenedorLimitado();
			itm.scn.layer->establecerEscuchadorTouches(this, this);
			//
			itm.scn.bg			= new(this) AUEscenaSprite();
			itm.scn.layer->agregarObjetoEscena(itm.scn.bg);
			//
			itm.scn.color		= 255;
			//
			itm.scn.text		= NULL;
			if(text != NULL){
				if(text[0] != '\0'){
					itm.scn.text = new(this) AUEscenaTexto(_fontSels);
					itm.scn.text->establecerMultiplicadorColor8(_textColor.r, _textColor.g, _textColor.b, _textColor.a);
					itm.scn.text->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
					itm.scn.text->establecerTexto(text);
					itm.scn.layer->agregarObjetoEscena(itm.scn.text);
				}
			}
			//Line is added at the top layer (is not part of the content)
			itm.scn.line		= new(this) AUEscenaFigura(ENEscenaFiguraTipo_Linea);
			itm.scn.line->establecerMultiplicadorColor8(_textColor.r, _textColor.g, _textColor.b, _textColor.a);
			itm.scn.line->agregarCoordenadaLocal(0.0f, 0.0f);
			itm.scn.line->agregarCoordenadaLocal(0.0f, 0.0f);
		}
		//Add
		NBArray_addValue(&_itms[iComp].array, itm);
		//Post
		{
			//Add to layers
			_top.layer.opts.itms[iComp].valuesLayer->agregarObjetoEscena(itm.scn.layer);
			_top.layer.opts.itms[iComp].linesLayer->agregarObjetoEscena(itm.scn.line);
		}
		r = TRUE;
	}
	return r;
}

void AUIDateBox::privListsCalculateWidths(const float pTotalWidth){
	if(pTotalWidth > 0.0f){
		float totalWidth = pTotalWidth;
		float totalMaxWidths = 0.0f;
		float maxWidths[ENDateBoxComp_Count];
		//Reduce the right icon width
		if(_optsIcoRight != NULL){
			const NBCajaAABB box = _optsIcoRight->cajaAABBLocal();
			if(box.xMin < box.xMax){
				totalWidth -= _marginI + (box.xMax - box.xMin) + _marginI;
			}
		}
		//Calculate max widths
		{
			SI32 iComp; for(iComp = 0; iComp < ENDateBoxComp_Count; iComp++){
				maxWidths[iComp] = 0.0f;
				{
					SI32 i; for (i = ((SI32)_itms[iComp].array.use - 1); i >= 0; i--){
						STIDateBoxItem* itm		= NBArray_itmPtrAtIndex(&_itms[iComp].array, STIDateBoxItem, i);
						const NBCajaAABB box	= itm->scn.text->cajaParaTexto(itm->text.str);
						const float width		= (box.xMax - box.xMin);
						if(maxWidths[iComp] < width){
							maxWidths[iComp] = width;
						}
					}
				}
				totalMaxWidths += maxWidths[iComp];
			}
		}
		//Assign relative widths
		{
			const float availWidth = totalWidth - _marginI - _marginI;
			SI32 iComp; for(iComp = 0; iComp < ENDateBoxComp_Count; iComp++){
				if(totalMaxWidths != 0.0f){
					_itms[iComp].width = (availWidth * (maxWidths[iComp] / totalMaxWidths));
				} else {
					_itms[iComp].width = 0.0f;
				}
			}
		}
	}
}

SI32 AUIDateBox::privListItmIdxByValue(const ENDateBoxComp iComp, const SI32 value){
	SI32 r = -1;
	{
		SI32 i; for (i = ((SI32)_itms[iComp].array.use - 1); i >= 0; i--){
			STIDateBoxItem* itm = NBArray_itmPtrAtIndex(&_itms[iComp].array, STIDateBoxItem, i);
			if(itm->value == value){
				r = i;
				break;
			}
		}
	}
	return r;
}

SI32 AUIDateBox::privListItmIdxByCurPosY(const ENDateBoxComp iComp){
	SI32 r = 0;
	const STNBAABox* boxSel	= &_top.layer.opts.boxSelect;
	const NBPunto pos		= _top.layer.opts.itms[iComp].layer->traslacion();
	const NBCajaAABB box	= _top.layer.opts.itms[iComp].layer->cajaAABBLocal();
	float idxF				= ((pos.y + box.yMax) - boxSel->yMax) / (boxSel->yMax - boxSel->yMin);
	const float extraF		= idxF - (float)((SI32)idxF);
	if(extraF >= 0.5f) idxF++;
	r = (SI32)idxF;
	r = (r < 0 ? 0 : r >= _itms[iComp].array.use ? _itms[iComp].array.use - 1 : r);
	NBASSERT(r >= 0 && r < _itms[iComp].array.use)
	return r;
}

SI32 AUIDateBox::privListItmValueByCurPosY(const ENDateBoxComp iComp){
	SI32 r = 0;
	{
		const SI32 idx = this->privListItmIdxByCurPosY(iComp);
		if(idx >= 0 && idx < _itms[iComp].array.use){
			STIDateBoxItem* itm = NBArray_itmPtrAtIndex(&_itms[iComp].array, STIDateBoxItem, idx);
			r = itm->value;
		}
	}
	return r;
}


void AUIDateBox::privListSelectDate(const STNBDate value, const BOOL animate){
	{
		const SI32 idxSel = this->privListItmIdxByValue(ENDateBoxComp_Day, value.day);
		this->privScrollTopContentToItmIdx(ENDateBoxComp_Day, (idxSel < 0 ? 0 : idxSel), animate);
	}
	{
		const SI32 idxSel = this->privListItmIdxByValue(ENDateBoxComp_Month, value.month);
		this->privScrollTopContentToItmIdx(ENDateBoxComp_Month, (idxSel < 0 ? 0 : idxSel), animate);
	}
	{
		const SI32 idxSel = this->privListItmIdxByValue(ENDateBoxComp_Year, value.year);
		this->privScrollTopContentToItmIdx(ENDateBoxComp_Year, (idxSel < 0 ? 0 : idxSel), animate);
	}
}

//

IEscuchadorIDateBox* AUIDateBox::getDateBoxListener(){
	return _lstnr;
}

void AUIDateBox::setDateBoxListener(IEscuchadorIDateBox* lstnr){
	_lstnr = lstnr;
}

STNBDate AUIDateBox::getDateSel() const {
	return _dateSel;
}

void AUIDateBox::setDateSel(const STNBDate date){
	_dateSel = date;
	{
		const STNBRect column = {0, 0, _boxWidth - _margins.left - _margins.right, _font->ascendenteEscena() + _font->descendenteEscena() };
		BOOL hasAnyValue = FALSE;
		STNBString str;
		NBString_init(&str);
		{
			if(date.day <= 0 || date.day > 31){
				NBString_concat(&str, "--");
			} else {
				NBString_concatSI32(&str, date.day);
				hasAnyValue = TRUE;
			}
			
		}
		NBString_concat(&str, " / ");
		{
			if(date.month <= 0 || date.month > 12){
				NBString_concat(&str, "----");
			} else {
				NBString_concat(&str, mNames[date.month - 1]);
				hasAnyValue = TRUE;
			}
		}
		NBString_concat(&str, " / ");
		{
			if(date.year <= 0){
				NBString_concat(&str, "----");
			} else {
				NBString_concatSI32(&str, date.year);
				hasAnyValue = TRUE;
			}
		}
		_visual->establecerTextoEnCol(str.str, column);
		_visual->establecerVisible(hasAnyValue);
		_visualHelp->establecerVisible(!hasAnyValue);
		NBString_release(&str);
	}
	//ToDo: update top's selection if visible
}

//

NBMargenes AUIDateBox::getMargins() const {
	return _margins;
}

void AUIDateBox::setMargins(const NBMargenes margins){
	_margins	= margins;
}

void AUIDateBox::setMargins(const float mLeft, const float mRight, const float mTop, const float mBottom){
	_margins.left	= mLeft;
	_margins.right	= mRight;
	_margins.top	= mTop;
	_margins.bottom	= mBottom;
}

//

float AUIDateBox::getBoxWidth() const {
	return _boxWidth;
}

void AUIDateBox::setBoxWidth(const float width){
	_boxWidth = width;
	this->privOrganizar();
}

//

AUTextura* AUIDateBox::getBoxBgTexture() const {
	return _bg->textura();
}

void AUIDateBox::setBoxBgTexture(AUTextura* textura){
	_bg->establecerTextura(textura);
}

//Value
void AUIDateBox::establecerTextoColor(const NBColor8 color){
	_visual->establecerMultiplicadorColor8(color);
	_visualIcoMore->establecerMultiplicadorColor8(color);
}

void AUIDateBox::establecerTextoColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	_visual->establecerMultiplicadorColor8(r, g, b, a);
	_visualIcoMore->establecerMultiplicadorColor8(r, g, b, a);
}

void AUIDateBox::establecerTextoAyuda(const char* valor){
	const STNBRect column = {0, 0, _boxWidth - _margins.left - _margins.right, _font->ascendenteEscena() + _font->descendenteEscena() };
	_visualHelp->establecerTextoEnCol(valor);
}

void AUIDateBox::establecerTextoAyudaColor(const NBColor8 color){
	_visualHelp->establecerMultiplicadorColor8(color);
}

void AUIDateBox::establecerTextoAyudaColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	_visualHelp->establecerMultiplicadorColor8(r, g, b, a);
}

void AUIDateBox::setSelFrameColor(const NBColor8 color){
	_top.layer.opts.selFrame->establecerMultiplicadorColor8(color);
}

void AUIDateBox::setSelFrameColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	_top.layer.opts.selFrame->establecerMultiplicadorColor8(r, g, b, a);
}

//

void AUIDateBox::focusObtain(){
	//ToDo
}

void AUIDateBox::focusRelease(){
	//ToDo
}

//

void AUIDateBox::setTextColor(const NBColor8 color){
	this->setTextColor(color.r, color.g, color.b, color.a);
}

void AUIDateBox::setTextColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	_textColor.r	= r;
	_textColor.g	= g;
	_textColor.b	= b;
	_textColor.a	= a;
	//Apply color
	{
		SI32 iComp; for(iComp = 0; iComp < ENDateBoxComp_Count; iComp++){
			SI32 i; for (i = ((SI32)_itms[iComp].array.use - 1); i >= 0; i--){
				STIDateBoxItem* itm = NBArray_itmPtrAtIndex(&_itms[iComp].array, STIDateBoxItem, i);
				if(itm->scn.line != NULL) itm->scn.line->establecerMultiplicadorColor8(_textColor.r, _textColor.g, _textColor.b, _textColor.a);
				if(itm->scn.text != NULL) itm->scn.text->establecerMultiplicadorColor8(_textColor.r, _textColor.g, _textColor.b, _textColor.a);
			}
		}
	}
}

float AUIDateBox::altoParaLineasVisibles(const float lineasVisibles) const {
	return AUIDateBox::altoParaLineasVisibles(lineasVisibles, _font);
}

float AUIDateBox::altoParaLineasVisibles(const float lineasVisibles, AUFuenteRender* fuenteRender) {
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

void AUIDateBox::privOrganizar(){
	const float lineHeight = AUIDateBox::altoParaLineasVisibles(1.0f, _font);
	const float itmHeight = _margins.top + lineHeight + _margins.bottom;
	//Content
	{
		float xLeft = _margins.left, xRight = _boxWidth - _margins.right;
		//Ico "more"
		if(_visualIcoMore != NULL){
			const NBCajaAABB box = _visualIcoMore->cajaAABBLocal();
			_visualIcoMore->establecerTraslacion(xRight - box.xMax, - box.yMax - ((itmHeight - (box.yMax - box.yMin)) * 0.5f));
			if(_visualIcoMore->visible()){
				xRight -= (box.xMax - box.xMin) + _marginI;
			}
		}
		//Help text
		if(_visualHelp != NULL){
			const NBCajaAABB box = _visualHelp->cajaAABBLocal();
			_visualHelp->establecerTraslacion(xLeft - box.xMin, - box.yMax - ((itmHeight - (box.yMax - box.yMin)) * 0.5f));
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

void AUIDateBox::privOrganizeItmsList(const ENDateBoxComp iComp){
	if(_itms[iComp].width != 0.0f){
		SI32 startIdx			= 0;
		float topY				= 0.0f;
		const float itmWidth	= _itms[iComp].width;
		const float itmHeight	= _fontSels->ascendenteEscena() + _fontSels->descendenteEscena() + _optsMarginIn.top + _optsMarginIn.bottom;
		//PRINTF_INFO("AUIDateBox, organizing from #%d to %d (yTop %.2f, itmWidth %.2f, _itmHeight %.2f).\n", (startIdx + 1), _itms[iComp].array.use, topY, itmWidth, _itmHeight);
		//Organize
		{
			SI32 i; for(i = startIdx; i < _itms[iComp].array.use; i++){
				STIDateBoxItem* itm = NBArray_itmPtrAtIndex(&_itms[iComp].array, STIDateBoxItem, i);
				float xLeft = _optsMarginIn.left, xRight = itmWidth - _optsMarginIn.right;
				NBCajaAABB txtBox;
				//Calculate height
				if(itm->scn.text == NULL){
					txtBox.xMin = txtBox.xMax = txtBox.yMin = txtBox.yMax = 0.0f;
				} else {
					itm->scn.text->organizarTexto(xRight - xLeft);
					txtBox		= itm->scn.text->cajaAABBLocal();
				}
				//Organize
				if(itm->scn.bg != NULL){
					itm->scn.bg->redimensionar(0.0f, 0.0f, itmWidth, -itmHeight);
				}
				if(itm->scn.text != NULL){
					itm->scn.text->establecerTraslacion(xLeft - txtBox.xMin + (((xRight - xLeft) - (txtBox.xMax - txtBox.xMin)) * 0.5f), - txtBox.yMax - ((itmHeight - (txtBox.yMax - txtBox.yMin)) * 0.5f));
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
		}
		//Set layer fixed size
		_top.layer.opts.itms[iComp].layer->establecerLimites(0.0f, itmWidth, topY, 0.0f);
	}
}

void AUIDateBox::privOrganizeTopContent(){
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
			STNBAABox* boxOuter = &_top.layer.opts.boxOuter;
			boxOuter->xMin = 0.0f;
			boxOuter->xMax = topBgSzScn.width;
			boxOuter->yMin = 0.0f;
			boxOuter->yMax = topBgSzScn.height;
			_top.layer.opts.bg->redimensionar(boxOuter->xMin, boxOuter->yMin, (boxOuter->xMax - boxOuter->xMin), (boxOuter->yMax - boxOuter->yMin));
			//Set itms
			{
				STNBAABox* boxInner = &_top.layer.opts.boxInner;
				boxInner->xMin = boxOuter->xMin + _optsMarginIn.left;
				boxInner->xMax = boxOuter->xMax - _optsMarginIn.right;
				boxInner->yMin = boxOuter->yMin + _optsMarginIn.bottom;
				boxInner->yMax = boxOuter->yMax - _optsMarginIn.top;
				{
					const float itmHeight = _fontSels->ascendenteEscena() + _fontSels->descendenteEscena() + _optsMarginIn.top + _optsMarginIn.bottom;
					STNBAABox* boxSelect = &_top.layer.opts.boxSelect;
					boxSelect->xMin = boxInner->xMin;
					boxSelect->xMax = boxInner->xMax;
					boxSelect->yMin = boxInner->yMin + (((boxInner->yMax - boxInner->yMin) - itmHeight) * 0.5f);
					boxSelect->yMax = boxSelect->yMin + itmHeight;
					_top.layer.opts.selFrame->redimensionar(boxSelect->xMin, boxSelect->yMin, (boxSelect->xMax - boxSelect->xMin), (boxSelect->yMax - boxSelect->yMin));
				}
				//Update columns widths
				this->privListsCalculateWidths(boxInner->xMax - boxInner->xMin);
				//Organize columns
				{
					float xLeft = boxInner->xMin;
					SI32 iComp; for(iComp = 0; iComp < ENDateBoxComp_Count; iComp++){
						this->privOrganizeItmsList((ENDateBoxComp)iComp);
						_top.layer.opts.itms[iComp].layer->establecerTraslacion(xLeft - _top.layer.opts.itms[iComp].layer->cajaAABBLocal().xMin, 0.0f); //y will be corrected later
						this->privScrollTopContent((ENDateBoxComp)iComp, 0.0f);
						xLeft += _itms[iComp].width + _marginI;
					}
					//Reduce the right icon width
					if(_optsIcoRight != NULL){
						STNBAABox* boxSelect = &_top.layer.opts.boxSelect;
						const NBCajaAABB box = _optsIcoRight->cajaAABBLocal();
						_optsIcoRight->establecerTraslacion(xLeft - box.xMin, boxSelect->yMax - box.yMax - (((boxSelect->yMax - boxSelect->yMin) - (box.yMax - box.yMin)) * 0.5f));
						xLeft += (box.xMax - box.xMin) + _marginI;
					}
				}
			}
			_top.posIn	= NBST_P(STNBPoint, sceneBox.xMin - boxOuter->xMin + (((sceneBox.xMax - sceneBox.xMin) - topBgSzScn.width) * 0.5f), sceneBox.yMin - boxOuter->yMin + _optsMarginOut.bottom + ((((sceneBox.yMax - sceneBox.yMin) - _optsMarginOut.bottom - _optsMarginOut.top) - topBgSzScn.height) * relY) );
			_top.posOut	= NBST_P(STNBPoint, _top.posIn.x, sceneBox.yMin - boxOuter->yMax - 1.0f);
		}
		//Glass
		_top.layer.glass->redimensionar(sceneBox.xMin - 1.0f, sceneBox.yMin - 1.0f, (sceneBox.xMax - sceneBox.xMin) + 2.0f, (sceneBox.yMax - sceneBox.yMin) + 2.0f);
	}
}

void AUIDateBox::privScrollTopContent(const ENDateBoxComp iComp, const float yScroll){
	this->privScrollTopContentToPos(iComp, _top.layer.opts.itms[iComp].layer->traslacion().y + yScroll, TRUE);
}

void AUIDateBox::privScrollTopContentToItmIdx(const ENDateBoxComp iComp, const SI32 itmIdx, const BOOL animate){
	if(itmIdx >= 0 && itmIdx < _itms[iComp].array.use){
		const STIDateBoxItem* itm = NBArray_itmPtrAtIndex(&_itms[iComp].array, STIDateBoxItem, itmIdx);
		const NBPunto posLayer = _top.layer.opts.itms[iComp].layer->traslacion();
		const STNBAABox* boxSel = &_top.layer.opts.boxSelect;
		const NBPunto posI	= itm->scn.layer->traslacion();
		NBCajaAABB boxI		= itm->scn.layer->cajaAABBLocal();
		boxI.xMin			+= posI.x;
		boxI.xMax			+= posI.x;
		boxI.yMin			+= posI.y;
		boxI.yMax			+= posI.y;
		if(!animate){
			this->privScrollTopContentToPos(iComp, boxSel->yMax - boxI.yMax, FALSE);
			//Stop current animation
			_itms[iComp].anim.yStart	= 0.0f;
			_itms[iComp].anim.yEnd		= 0.0f;
			_itms[iComp].anim.secsDur	= 0.0f;
			_itms[iComp].anim.secsCur	= 0.0f;
		} else {
			const float deltaY			= (boxSel->yMax - boxI.yMax) - posI.y;
			const float absDelta		= (deltaY < 0.0f ? -deltaY : deltaY);
			_itms[iComp].anim.yStart	= posLayer.y;
			_itms[iComp].anim.yEnd		= boxSel->yMax - boxI.yMax;
			_itms[iComp].anim.secsDur	= (absDelta / 2048.0f);
			_itms[iComp].anim.secsCur	= 0.0f;
			if(_itms[iComp].anim.secsDur > 0.35f){
				_itms[iComp].anim.secsDur = 0.35f;
			}
		}
	}
}

void AUIDateBox::privScrollTopContentToPos(const ENDateBoxComp iComp, const float yPos, const BOOL setAsDirtyMoved){
	const STNBAABox* boxSel = &_top.layer.opts.boxSelect;
	NBPunto pos		= _top.layer.opts.itms[iComp].layer->traslacion();
	NBCajaAABB box	= _top.layer.opts.itms[iComp].layer->cajaAABBLocal(); //use BGs layer to avoid holes in the glass
	pos.y			= yPos;
	//Validate limits
	{
		if(pos.y < (boxSel->yMax - box.yMax)){
			pos.y = (boxSel->yMax - box.yMax);
		} else if(pos.y > (boxSel->yMin - box.yMin)){
			pos.y = (boxSel->yMin - box.yMin);
		}
	}
	//Set itms visibility
	{
		const STNBAABox* boxInner = &_top.layer.opts.boxInner;
		const STNBAABox* boxOuter = &_top.layer.opts.boxOuter;
		const float outMarginTop = (boxOuter->yMax - boxInner->yMax);
		const float outMarginBtm = (boxInner->yMin - boxOuter->yMin);
		SI32 i; for(i = 0; i < _itms[iComp].array.use; i++){
			STIDateBoxItem* itm = NBArray_itmPtrAtIndex(&_itms[iComp].array, STIDateBoxItem, i);
			BOOL visibleI		= TRUE;
			UI8 colorI			= 255;
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
			//Calculate color
			{
				const float maxItmsDim = 2.0f;
				const float relSel = (boxI.yMax - boxSel->yMax) / (boxSel->yMax - boxSel->yMin);
				if(relSel < -maxItmsDim || relSel > maxItmsDim){
					colorI = 75;
				} else if(relSel >= -0.1f && relSel <= 0.1f){
					colorI = 255;
				} else {
					NBASSERT(relSel >= -maxItmsDim && relSel <= maxItmsDim)
					colorI = 75 + (180.0f * (1.0f - ((relSel < 0.0f ? -relSel : relSel) / maxItmsDim)));
				}
			}
			itm->scn.layer->establecerVisible(visibleI);
			itm->scn.line->establecerVisible(FALSE && visibleI);
			itm->scn.layer->establecerMultiplicadorAlpha8((alphaI * colorI) / 255);
			itm->scn.line->establecerMultiplicadorAlpha8((alphaI * colorI) / 255);
			itm->scn.color = colorI;
		}
	}
	_top.layer.opts.itms[iComp].layer->establecerTraslacion(pos.x, pos.y); //preserve-x
	//Set as moved
	if(setAsDirtyMoved){
		_touch[iComp].scrollDirtyMoved = TRUE;
	}
}

//

void AUIDateBox::puertoDeVisionModificado(const SI32 iEscena, const STNBViewPortSize before, const STNBViewPortSize after){
	if(_onFocus){
		this->privOrganizeTopContent();
	}
}

//

void AUIDateBox::privTopAnimate(const float secs){
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

void AUIDateBox::privRemoveFromTopPriv(){
	NBASSERT((_onFocus && _top.layer.obj->contenedor() != NULL) || (!_onFocus && _top.layer.obj->contenedor() == NULL))
	if(_onFocus){
		NBGestorAnimadores::quitarAnimador(this);
		NBGestorEscena::quitarEscuchadorCambioPuertoVision(this->idEscena, this);
		//Update value
		{
			STNBDate date;
			date.day	= this->privListItmValueByCurPosY(ENDateBoxComp_Day);
			date.month	= this->privListItmValueByCurPosY(ENDateBoxComp_Month);
			date.year	= this->privListItmValueByCurPosY(ENDateBoxComp_Year);
			this->setDateSel(date);
		}
		//Remove from focus (from topLayer)
		if(_top.layer.obj != NULL){
			AUEscenaContenedor* parent = (AUEscenaContenedor*)_top.layer.obj->contenedor(); NBASSERT(parent != NULL)
			if(parent != NULL) parent->quitarObjetoEscena(_top.layer.obj);
		}
		_onFocus = FALSE;
	}
}

//

void AUIDateBox::tickAnimacion(float segsTranscurridos){
	//Animate
	if(_top.anim.secsDur > 0.0f){
		this->privTopAnimate(segsTranscurridos);
	}
	{
		SI32 iComp; for(iComp = 0; iComp < ENDateBoxComp_Count; iComp++){
			//Save "dirtyMoved" value and reset
			BOOL dirtyMovedBefore = _touch[iComp].scrollDirtyMoved;
			_touch[iComp].scrollDirtyMoved = FALSE;
			//Animate
			if(_itms[iComp].anim.secsDur > 0.0f){
				_itms[iComp].anim.secsCur		+= segsTranscurridos;
				{
					const float rel = (_itms[iComp].anim.secsCur / _itms[iComp].anim.secsDur);
					if(rel >= 1.0f){
						this->privScrollTopContentToPos((ENDateBoxComp)iComp, _itms[iComp].anim.yEnd, FALSE);
						//End animation
						_itms[iComp].anim.yStart	= 0.0f;
						_itms[iComp].anim.yEnd		= 0.0f;
						_itms[iComp].anim.secsDur	= 0.0f;
						_itms[iComp].anim.secsCur	= 0.0f;
					} else {
						const float ypos		= _itms[iComp].anim.yStart + ((_itms[iComp].anim.yEnd - _itms[iComp].anim.yStart) * rel /*pow(rel, 1.0f / 2.0f)*/);
						this->privScrollTopContentToPos((ENDateBoxComp)iComp, ypos, FALSE);
					}
				}
				//Nullify the drag values
				_touch[iComp].scrollSpeedY		= 0.0f;
				_touch[iComp].deltaYAccum		= 0.0f;
				_touch[iComp].deltaYAccumSecs	= 0.0f;
				//Nullify dirty move
				dirtyMovedBefore				= FALSE;
			}
			//Consume last deltaY
			{
				_touch[iComp].deltaYAccumSecs += segsTranscurridos;
				if((_touch[iComp].deltaYAccum > 0.0f && _touch[iComp].deltaYLast < 0.0f) || (_touch[iComp].deltaYAccum < 0.0f && _touch[iComp].deltaYLast > 0.0f)){
					//Direction changed
					_touch[iComp].scrollSpeedY		= 0.0f;
					_touch[iComp].deltaYAccum		= 0.0f;
					_touch[iComp].deltaYAccumSecs	= 0.0f;
				} else {
					//Acumulate this (is same direction)
					_touch[iComp].deltaYAccum		+= _touch[iComp].deltaYLast;
				}
				_touch[iComp].deltaYLast			= 0.0f;
			}
			//Scroll (only if not touching)
			if(_touch[iComp].first == NULL){
				//Apply accumulated speed
				if(_touch[iComp].deltaYAccum != 0.0f){
					NBASSERT(_touch[iComp].deltaYAccumSecs > 0.0f)
					if(_touch[iComp].deltaYAccumSecs != 0.0f){
						_touch[iComp].scrollSpeedY		+= (_touch[iComp].deltaYAccum / _touch[iComp].deltaYAccumSecs);
						//PRINTF_INFO("_touch[iComp].deltaYLast(%f); prevScrollSpeedY(%f) => _touch[iComp].scrollSpeedY(%f).\n", _touch[iComp].deltaYLast, prevScrollSpeedY, _touch[iComp].scrollSpeedY);
					}
					_touch[iComp].deltaYAccum		= 0.0f;
					_touch[iComp].deltaYAccumSecs	= 0.0f;
				}
				//Apply scroll
				if(_touch[iComp].scrollSpeedY != 0.0f){
					//PRINTF_INFO("_touch[iComp].scrollSpeedY(%f).\n", _touch[iComp].scrollSpeedY);
					this->privScrollTopContent((ENDateBoxComp)iComp, _touch[iComp].scrollSpeedY * segsTranscurridos);
					_touch[iComp].scrollSpeedY -= (_touch[iComp].scrollSpeedY * pow(segsTranscurridos, 1.0f / 3.0f));
					if(_touch[iComp].scrollSpeedY >= -5.0f && _touch[iComp].scrollSpeedY <= 5.0f){
						_touch[iComp].scrollSpeedY = 0.0f;
					}
				}
				//Animate to closest selection
				if(dirtyMovedBefore && !_touch[iComp].scrollDirtyMoved){
					const SI32 idxSel = this->privListItmIdxByCurPosY((ENDateBoxComp)iComp);
					this->privScrollTopContentToItmIdx((ENDateBoxComp)iComp, idxSel, TRUE);
				}
			}
		}
	}
}

//TOUCHES

void AUIDateBox::touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto){
	if(_bg == objeto){
		_bg->establecerMultiplicadorColor8(225, 225, 225, 255);
	} else if(_top.layer.glass == objeto){
		//Will hide
	} else if(_top.layer.opts.bg == objeto){
		//Touch safe-area (in case options not cover an area)
	} else if(_top.layer.opts.selFrame == objeto){
		//Will hide
		_top.layer.opts.selFrame->establecerMultiplicadorAlpha8(100);
	} else {
		SI32 iComp; for(iComp = 0; iComp < ENDateBoxComp_Count; iComp++){
			if(_top.layer.opts.itms[iComp].layer == objeto){
				if(_touch[iComp].first == NULL){ //Must be the first touch
					const NBPunto pos		= _top.layer.opts.itms[iComp].layer->traslacion();
					_touch[iComp].first			= touch;
					_touch[iComp].itmsStartPos.x = pos.x;
					_touch[iComp].itmsStartPos.y = pos.y;
				}
				//Stop animation
				_itms[iComp].anim.yStart	= 0.0f;
				_itms[iComp].anim.yEnd		= 0.0f;
				_itms[iComp].anim.secsDur	= 0.0f;
				_itms[iComp].anim.secsCur	= 0.0f;
				//Stop scroll
				_touch[iComp].scrollSpeedY = 0.0f;
				_touch[iComp].deltaYLast = 0.0f;
				_touch[iComp].deltaYAccum = 0.0f;
				_touch[iComp].deltaYAccumSecs = 0.0f;
			}
			{
				SI32 i; for(i = 0; i < _itms[iComp].array.use; i++){
					STIDateBoxItem* itm = NBArray_itmPtrAtIndex(&_itms[iComp].array, STIDateBoxItem, i);
					if(itm->scn.layer == objeto){
						if(i == _itms[iComp].idxSeletected){
							itm->scn.bg->establecerMultiplicadorColor8(225, 200, 200, 255);
						} else {
							itm->scn.bg->establecerMultiplicadorColor8(225, 225, 225, 255);
						}
						break;
					}
				}
			}
		}
	}
}

void AUIDateBox::touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	if(_bg == objeto){
		//nothing
	} else if(_top.layer.glass == objeto){
		//Will hide
	} else if(_top.layer.opts.selFrame == objeto || _top.layer.opts.bg == objeto){
		//Touch safe-area (in case options not cover an area)){
		NBTamano deltaScn;
		deltaScn.ancho	= posActualEscena.x - posInicialEscena.x;
		deltaScn.alto	= posActualEscena.y - posInicialEscena.y;
		const NBTamano deltaInch = NBGestorEscena::tamanoEscenaAPulgadas(this->idEscena, deltaScn);
		const float distInch = sqrtf((deltaInch.ancho * deltaInch.ancho) + (deltaInch.alto * deltaInch.alto)); NBASSERT(distInch >= 0.0f)
		if(distInch > 0.10f){
			const NBPunto localPos = objeto->coordenadaEscenaALocal(posInicialEscena.x, posInicialEscena.y);
			SI32 iComp; for(iComp = 0; iComp < ENDateBoxComp_Count; iComp++){
				const NBPunto pos = _top.layer.opts.itms[iComp].layer->traslacion();
				NBCajaAABB box = _top.layer.opts.itms[iComp].layer->cajaAABBLocal();
				if(localPos.x <= (pos.x + box.xMax) || iComp == (ENDateBoxComp_Count - 1)){
					objeto->liberarTouch(touch, posInicialEscena, posAnteriorEscena, posActualEscena, TRUE, _top.layer.opts.itms[iComp].layer);
					break;
				}
			}
		}
	} else {
		SI32 iComp; for(iComp = 0; iComp < ENDateBoxComp_Count; iComp++){
			if(_top.layer.opts.itms[iComp].layer == objeto){
				if(_touch[iComp].first == touch){ //Must be the first touch
					this->privScrollTopContentToPos((ENDateBoxComp)iComp, _touch[iComp].itmsStartPos.y + (posActualEscena.y - posInicialEscena.y), TRUE);
					_touch[iComp].deltaYLast = (posActualEscena.y - posAnteriorEscena.y);
				}
			}
			{
				SI32 i; for(i = 0; i < _itms[iComp].array.use; i++){
					STIDateBoxItem* itm = NBArray_itmPtrAtIndex(&_itms[iComp].array, STIDateBoxItem, i);
					if(itm->scn.layer == objeto){
						NBTamano deltaScn;
						deltaScn.ancho	= posActualEscena.x - posInicialEscena.x;
						deltaScn.alto	= posActualEscena.y - posInicialEscena.y;
						const NBTamano deltaInch = NBGestorEscena::tamanoEscenaAPulgadas(this->idEscena, deltaScn);
						const float distInch = sqrtf((deltaInch.ancho * deltaInch.ancho) + (deltaInch.alto * deltaInch.alto)); NBASSERT(distInch >= 0.0f)
						if(distInch > 0.10f){
							objeto->liberarTouch(touch, posInicialEscena, posAnteriorEscena, posActualEscena, TRUE, _top.layer.opts.itms[iComp].layer);
						}
						break;
					}
				}
			}
		}
	}
}

void AUIDateBox::touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	STIDateBoxItem* itmTouched = NULL; ENDateBoxComp itmTouchedCmp = ENDateBoxComp_Count; SI32 itmTouchedIdx = -1;
	if(_bg == objeto){
		_bg->establecerMultiplicadorColor8(255, 255, 255, 255);
	} else if(_top.layer.opts.bg == objeto){
		//Touch safe-area (in case options not cover an area)
	} else if(_top.layer.glass == objeto || _top.layer.opts.selFrame == objeto){
		if(_top.layer.opts.selFrame == objeto){
			_top.layer.opts.selFrame->establecerMultiplicadorAlpha8(255);
		}
		if(!touch->cancelado){
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
		}
	} else {
		SI32 iComp; for(iComp = 0; iComp < ENDateBoxComp_Count; iComp++){
			if(_top.layer.opts.itms[iComp].layer == objeto){
				if(_touch[iComp].first == touch){ //Must be the first touch
					this->privScrollTopContentToPos((ENDateBoxComp)iComp, _touch[iComp].itmsStartPos.y + (posActualEscena.y - posInicialEscena.y), TRUE);
					_touch[iComp].deltaYLast = (posActualEscena.y - posAnteriorEscena.y);
					_touch[iComp].first = NULL;
					//Animate to selection
					if(_touch[iComp].deltaYAccum == 0.0f && _touch[iComp].deltaYLast == 0.0f){
						const SI32 idxSel = this->privListItmIdxByCurPosY((ENDateBoxComp)iComp);
						this->privScrollTopContentToItmIdx((ENDateBoxComp)iComp, idxSel, TRUE);
					}
				}
			}
			{
				SI32 i; for(i = 0; i < _itms[iComp].array.use; i++){
					STIDateBoxItem* itm = NBArray_itmPtrAtIndex(&_itms[iComp].array, STIDateBoxItem, i);
					if(itm->scn.layer == objeto){
						if(i == _itms[iComp].idxSeletected){
							itm->scn.bg->establecerMultiplicadorColor8(255, 225, 225, 255);
						} else {
							itm->scn.bg->establecerMultiplicadorColor8(255, 255, 255, 255);
						}
						//PRINTF_INFO("Texto: '%s'.\n", itm->text.str);
						itmTouched		= itm;
						itmTouchedCmp	= (ENDateBoxComp)iComp;
						itmTouchedIdx	= i;
						break;
					}
				}
			}
		}
	}
	//Process
	if(!touch->cancelado){
		const NBCajaAABB cajaEscena = objeto->cajaAABBEnEscena();
		if(NBCAJAAABB_INTERSECTA_PUNTO(cajaEscena, posActualEscena.x, posActualEscena.y) && !touch->cancelado){
			if(itmTouched != NULL){
				//Select itm (animated)
				this->privScrollTopContentToItmIdx(itmTouchedCmp, itmTouchedIdx, TRUE);
				//Stop scroll
				_touch[itmTouchedCmp].scrollSpeedY = 0.0f;
				_touch[itmTouchedCmp].deltaYLast = 0.0f;
				_touch[itmTouchedCmp].deltaYAccum = 0.0f;
				_touch[itmTouchedCmp].deltaYAccumSecs = 0.0f;
				//Init out-animation
				/*{
					_top.anim.isAnimIn	= FALSE;
					_top.anim.secsCur	= 0.0f;
					_top.anim.secsDur	= AUISELECT_BOX_ANIM_DURATION;
					this->privTopAnimate(0.0f); //initial location
				}*/
			} else if(_bg == objeto){
				if(!_onFocus){
					if(this->addToTop(_top.layer.obj)){
						NBASSERT(_top.layer.obj->idEscena >= 0 && _top.layer.obj->idGrupo >= 0)
						this->privOrganizeTopContent();
						NBGestorEscena::agregarEscuchadorCambioPuertoVision(this->idEscena, this);
						NBGestorAnimadores::agregarAnimador(this, this);
						//stop scroll
						{
							SI32 iComp; for(iComp = 0; iComp < ENDateBoxComp_Count; iComp++){
								_touch[iComp].deltaYLast	= 0.0f;		//deltaY from last tick (to add to velocity)
								_touch[iComp].scrollSpeedY	= 0.0f ;	//cur scroll speed
							}
						}
						//Init in-animation
						{
							_top.anim.isAnimIn	= TRUE;
							_top.anim.secsCur	= 0.0f;
							_top.anim.secsDur	= AUISELECT_BOX_ANIM_DURATION;
							this->privTopAnimate(0.0f); //initial location
							//Set value
							this->privListSelectDate(_dateSel, FALSE);
						}
						_onFocus = TRUE;
					}
				}
			}
		}
	}
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUIDateBox, AUEscenaContenedor)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUIDateBox, "AUIDateBox", AUEscenaContenedor)
AUOBJMETODOS_CLONAR_NULL(AUIDateBox)

