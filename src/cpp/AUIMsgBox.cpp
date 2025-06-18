//
//  AUISelectBox.cpp
//  lib-auframework-media
//
//  Created by Marcos Ortega on 9/3/18.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUIMsgBox.h"
//
#include "nb/core/NBMemory.h"
#include "nb/core/NBString.h"
#include "NBGestorAnimadores.h"
#include "NBGestorEscena.h"
#include "AUIButton.h"

#define AUIMsgBox_BOX_ANIM_DURATION 0.30

AUIMsgBox::AUIMsgBox(AUFuenteRender* fontTitle, AUFuenteRender* fontContent, AUFuenteRender* fontBtns, AUTextura* bgTex, AUTextura* scrollTex, const STNBColor8 scrollColor8, const float marginI)
: AUEscenaObjeto() {
	_marginI			= marginI;
	_marginsInn.left	= _marginsInn.right = _marginsInn.top = _marginsInn.bottom	= 0;
	_marginsOut.left	= _marginsOut.right = _marginsOut.top = _marginsOut.bottom = 0;
	_btnsMarginI		= 0.0f;
	_btnsMarginsInn.left = _btnsMarginsInn.right = _btnsMarginsInn.top = _btnsMarginsInn.bottom = 0;
	//
	_onFocus				= FALSE;
	_onAnim					= FALSE;
	_hideWhenGlassTouched	= TRUE;
	_btnOptionIsPendNotify	= FALSE;
	_btnCloseIsPendNotify	= FALSE;
	NBString_init(&_btnOptionIdSel);
	//
	_fontTitle		= fontTitle; if(_fontTitle != NULL) _fontTitle->retener(NB_RETENEDOR_THIS);
	_fontContent	= fontContent; if(_fontContent != NULL) _fontContent->retener(NB_RETENEDOR_THIS);
	_fontBtns		= fontBtns; if(_fontBtns != NULL) _fontBtns->retener(NB_RETENEDOR_THIS);
	//
	_titleAlignH			= ENNBTextLineAlignH_Left;
	_titleIsVisible			= TRUE;
	_contentAlignH			= ENNBTextLineAlignH_Left;
	_contentTextIsVisible	= TRUE;
	//
	_titleColor		= NBST_P(STNBColor8, 0, 0, 0, 255 );
	//Items
	{
		NBMemory_setZero(_top);
		_top.layer.obj = new(this) AUEscenaContenedor();
		{
			_top.layer.glass = new(this) AUEscenaSprite();
			_top.layer.glass->establecerMultiplicadorColor8(0, 0, 0, 220);
			_top.layer.glass->establecerEscuchadorTouches(this, this);
			_top.layer.obj->agregarObjetoEscena(_top.layer.glass);
		}
		//opts
		{
			{
				_top.layer.opts.layer = new(this) AUEscenaContenedor();
				_top.layer.opts.layer->establecerEscuchadorTouches(this, this);
				_top.layer.obj->agregarObjetoEscena(_top.layer.opts.layer);
			}
			{
				_top.layer.opts.bg = new(this) AUEscenaSpriteElastico(bgTex);
				_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.bg);
			}
			{
				_top.layer.opts.textLayer = new(this) AUEscenaContenedor();
				_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.textLayer);
			}
			{
				_top.layer.opts.text = new(this) AUEscenaPanelTexto(_fontContent, scrollTex, scrollColor8, NULL);
				_top.layer.opts.text->establecerAlineaciones(ENNBTextLineAlignH_Adjust, ENNBTextAlignV_FromTop);
				_top.layer.opts.textLayer->agregarObjetoEscena(_top.layer.opts.text);
			}
			{
				_top.layer.opts.customTop = NULL;
				_top.layer.opts.customBtm = NULL;
			}
			{
				_top.layer.opts.title = new(this) AUEscenaTexto(_fontTitle);
				_top.layer.opts.title->establecerMultiplicadorColor8(0, 0, 0, 255);
				_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.title);
			}
			{
				_top.layer.opts.btnsLayer = new(this) AUEscenaContenedor();
				_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.btnsLayer);
				//
				_top.layer.opts.btns = new(this) AUArregloMutable();
			}
		}
	}
	//Top
	{
		_top.posRelFromBottom	= 0.0f;
		_top.posIn				= NBST_P(STNBPoint, 0, 0 );
		_top.posOut				= NBST_P(STNBPoint, 0, 0 );
		{
			_top.anim.isAnimIn	= FALSE;
			_top.anim.secsDur	= 0.0f;
			_top.anim.secsCur	= 0.0f;
		}
	}
	_lstnr	= NULL;
	//
	this->privOrganizar();
}

AUIMsgBox::AUIMsgBox(AUFuenteRender* fontTitle, AUFuenteRender* fontContent, AUFuenteRender* fontBtns, AUTextura* bgTex, AUTextura* scrollTex, const STNBColor8 scrollColor8, const float marginI, const NBMargenes margins)
: AUEscenaObjeto()
{
	_marginI		= marginI;
	_marginsInn		= margins;
	_marginsOut.left = _marginsOut.right = _marginsOut.top = _marginsOut.bottom = 0;
	//
	_onFocus				= FALSE;
	_onAnim					= FALSE;
	_hideWhenGlassTouched	= TRUE;
	_btnOptionIsPendNotify	= FALSE;
	_btnCloseIsPendNotify	= FALSE;
	NBString_init(&_btnOptionIdSel);
	//
	_fontTitle		= fontTitle; if(_fontTitle != NULL) _fontTitle->retener(NB_RETENEDOR_THIS);
	_fontContent	= fontContent; if(_fontContent != NULL) _fontContent->retener(NB_RETENEDOR_THIS);
	_fontBtns		= fontBtns; if(_fontBtns != NULL) _fontBtns->retener(NB_RETENEDOR_THIS);
	//
	_titleAlignH	= ENNBTextLineAlignH_Left;
	_titleIsVisible	= TRUE;
	_contentAlignH	= ENNBTextLineAlignH_Left;
	_contentTextIsVisible = TRUE;
	//
	_titleColor		= NBST_P(STNBColor8, 0, 0, 0, 255 );
	//Items
	{
		NBMemory_setZero(_top);
		_top.layer.obj = new(this) AUEscenaContenedor();
		{
			_top.layer.glass = new(this) AUEscenaSprite();
			_top.layer.glass->establecerMultiplicadorColor8(0, 0, 0, 220);
			_top.layer.glass->establecerEscuchadorTouches(this, this);
			_top.layer.obj->agregarObjetoEscena(_top.layer.glass);
		}
		//opts
		{
			{
				_top.layer.opts.layer = new(this) AUEscenaContenedor();
				_top.layer.opts.layer->establecerEscuchadorTouches(this, this);
				_top.layer.obj->agregarObjetoEscena(_top.layer.opts.layer);
			}
			{
				_top.layer.opts.bg = new(this) AUEscenaSpriteElastico(bgTex);
				_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.bg);
			}
			{
				_top.layer.opts.textLayer = new(this) AUEscenaContenedor();
				_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.textLayer);
			}
			{
				_top.layer.opts.text = new(this) AUEscenaPanelTexto(_fontContent, scrollTex, scrollColor8, NULL);
				_top.layer.opts.text->establecerAlineaciones(ENNBTextLineAlignH_Adjust, ENNBTextAlignV_FromTop);
				_top.layer.opts.textLayer->agregarObjetoEscena(_top.layer.opts.text);
			}
			{
				_top.layer.opts.customTop = NULL;
				_top.layer.opts.customBtm = NULL;
			}
			{
				_top.layer.opts.title = new(this) AUEscenaTexto(_fontTitle);
				_top.layer.opts.title->establecerMultiplicadorColor8(0, 0, 0, 255);
				_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.title);
			}
			{
				_top.layer.opts.btnsLayer = new(this) AUEscenaContenedor();
				_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.btnsLayer);
				//
				_top.layer.opts.btns = new(this) AUArregloMutable();
			}
		}
	}
	//Top
	{
		_top.posRelFromBottom	= 0.0f;
		_top.posIn				= NBST_P(STNBPoint, 0, 0 );
		_top.posOut				= NBST_P(STNBPoint, 0, 0 );
		{
			_top.anim.isAnimIn	= FALSE;
			_top.anim.secsDur	= 0.0f;
			_top.anim.secsCur	= 0.0f;
		}
	}
	_lstnr	= NULL;
	//
	this->privOrganizar();
}

AUIMsgBox::AUIMsgBox(AUFuenteRender* fontTitle, AUFuenteRender* fontContent, AUFuenteRender* fontBtns, AUTextura* bgTex, AUTextura* scrollTex, const STNBColor8 scrollColor8, const float marginI, const float mLeft, const float mRight, const float mTop, const float mBottom)
: AUEscenaObjeto()
{
	_marginI		= marginI;
	_marginsInn.left	= mLeft;
	_marginsInn.right	= mRight;
	_marginsInn.top	= mTop;
	_marginsInn.bottom	= mBottom;
	_marginsOut.left = _marginsOut.right = _marginsOut.top = _marginsOut.bottom = 0;
	//
	_onFocus		= FALSE;
	_onAnim			= FALSE;
	_hideWhenGlassTouched = TRUE;
	NBString_init(&_btnOptionIdSel);
	//
	_fontTitle		= fontTitle; if(_fontTitle != NULL) _fontTitle->retener(NB_RETENEDOR_THIS);
	_fontContent	= fontContent; if(_fontContent != NULL) _fontContent->retener(NB_RETENEDOR_THIS);
	_fontBtns		= fontBtns; if(_fontBtns != NULL) _fontBtns->retener(NB_RETENEDOR_THIS);
	//
	_titleAlignH			= ENNBTextLineAlignH_Left;
	_titleIsVisible			= TRUE;
	_contentAlignH			= ENNBTextLineAlignH_Left;
	_contentTextIsVisible	= TRUE;
	//
	_titleColor		= NBST_P(STNBColor8, 0, 0, 0, 255 );
	//Items
	{
		NBMemory_setZero(_top);
		_top.layer.obj = new(this) AUEscenaContenedor();
		{
			_top.layer.glass = new(this) AUEscenaSprite();
			_top.layer.glass->establecerMultiplicadorColor8(0, 0, 0, 220);
			_top.layer.glass->establecerEscuchadorTouches(this, this);
			_top.layer.obj->agregarObjetoEscena(_top.layer.glass);
		}
		//opts
		{
			{
				_top.layer.opts.layer = new(this) AUEscenaContenedor();
				_top.layer.opts.layer->establecerEscuchadorTouches(this, this);
				_top.layer.obj->agregarObjetoEscena(_top.layer.opts.layer);
			}
			{
				_top.layer.opts.bg = new(this) AUEscenaSpriteElastico(bgTex);
				_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.bg);
			}
			{
				_top.layer.opts.textLayer = new(this) AUEscenaContenedor();
				_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.textLayer);
			}
			{
				_top.layer.opts.text = new(this) AUEscenaPanelTexto(_fontContent, scrollTex, scrollColor8, NULL);
				_top.layer.opts.text->establecerAlineaciones(ENNBTextLineAlignH_Adjust, ENNBTextAlignV_FromTop);
				_top.layer.opts.textLayer->agregarObjetoEscena(_top.layer.opts.text);
			}
			{
				_top.layer.opts.customTop = NULL;
				_top.layer.opts.customBtm = NULL;
			}
			{
				_top.layer.opts.title = new(this) AUEscenaTexto(_fontTitle);
				_top.layer.opts.title->establecerMultiplicadorColor8(0, 0, 0, 255);
				_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.title);
			}
			{
				_top.layer.opts.btnsLayer = new(this) AUEscenaContenedor();
				_top.layer.opts.layer->agregarObjetoEscena(_top.layer.opts.btnsLayer);
				//
				_top.layer.opts.btns = new(this) AUArregloMutable();
			}
		}
	}
	//Top
	{
		_top.posRelFromBottom	= 0.0f;
		_top.posIn				= NBST_P(STNBPoint, 0, 0 );
		_top.posOut				= NBST_P(STNBPoint, 0, 0 );
		{
			_top.anim.isAnimIn	= FALSE;
			_top.anim.secsDur	= 0.0f;
			_top.anim.secsCur	= 0.0f;
		}
	}
	_lstnr	= NULL;
	//
	this->privOrganizar();
}

AUIMsgBox::~AUIMsgBox(){
	_marginI	= 0.0f;
	_marginsInn.left = _marginsInn.right = _marginsInn.top = _marginsInn.bottom	= 0.0f;
	//Remove from focus
	this->privRemoveFromTopPriv();
	NBASSERT(!_onFocus)
	NBASSERT(!_onAnim)
	//Items
	{
		if(_top.layer.obj != NULL) _top.layer.obj->liberar(NB_RETENEDOR_THIS); _top.layer.obj = NULL;
		if(_top.layer.glass != NULL) _top.layer.glass->liberar(NB_RETENEDOR_THIS); _top.layer.glass = NULL;
		//opts
		{
			if(_top.layer.opts.layer != NULL) _top.layer.opts.layer->liberar(NB_RETENEDOR_THIS); _top.layer.opts.layer = NULL;
			if(_top.layer.opts.bg != NULL) _top.layer.opts.bg->liberar(NB_RETENEDOR_THIS); _top.layer.opts.bg = NULL;
			if(_top.layer.opts.title != NULL) _top.layer.opts.title->liberar(NB_RETENEDOR_THIS); _top.layer.opts.title = NULL;
			if(_top.layer.opts.customTop != NULL){
				AUEscenaContenedor* parent = (AUEscenaContenedor*)_top.layer.opts.customTop->contenedor();
				if(parent != NULL) parent->quitarObjetoEscena(_top.layer.opts.customTop);
				_top.layer.opts.customTop->liberar(NB_RETENEDOR_THIS);
				_top.layer.opts.customTop = NULL;
			}
			if(_top.layer.opts.customBtm != NULL){
				AUEscenaContenedor* parent = (AUEscenaContenedor*)_top.layer.opts.customBtm->contenedor();
				if(parent != NULL) parent->quitarObjetoEscena(_top.layer.opts.customBtm);
				_top.layer.opts.customBtm->liberar(NB_RETENEDOR_THIS);
				_top.layer.opts.customBtm = NULL;
			}
			if(_top.layer.opts.textLayer != NULL) _top.layer.opts.textLayer->liberar(NB_RETENEDOR_THIS); _top.layer.opts.textLayer = NULL;
			if(_top.layer.opts.text != NULL) _top.layer.opts.text->liberar(NB_RETENEDOR_THIS); _top.layer.opts.text = NULL;
			if(_top.layer.opts.btnsLayer != NULL) _top.layer.opts.btnsLayer->liberar(NB_RETENEDOR_THIS); _top.layer.opts.btnsLayer = NULL;
			if(_top.layer.opts.btns != NULL) _top.layer.opts.btns->liberar(NB_RETENEDOR_THIS); _top.layer.opts.btns = NULL;
		}
		//close
		{
			if(_top.layer.close.lyr != NULL) _top.layer.close.lyr->liberar(); _top.layer.close.lyr = NULL;
			if(_top.layer.close.dummy[0] != NULL) _top.layer.close.dummy[0]->liberar(); _top.layer.close.dummy[0] = NULL;
			if(_top.layer.close.dummy[1] != NULL) _top.layer.close.dummy[1]->liberar(); _top.layer.close.dummy[1] = NULL;
			{
				SI32 i; for(i = 0; i < ENIMsgboxCloseBtnLyr_Count; i++){
					STIMsgboxCloseBtnLyr* ico = &_top.layer.close.icos[i];
					if(ico->ico != NULL) ico->ico->liberar(); ico->ico = NULL;
				}
			}
		}
	}
	if(_fontTitle != NULL) _fontTitle->liberar(NB_RETENEDOR_THIS); _fontTitle = NULL;
	if(_fontContent != NULL) _fontContent->liberar(NB_RETENEDOR_THIS); _fontContent = NULL;
	if(_fontBtns != NULL) _fontBtns->liberar(NB_RETENEDOR_THIS); _fontBtns = NULL;
	//
	_btnOptionIsPendNotify	= FALSE;
	_btnCloseIsPendNotify	= FALSE;
	NBString_release(&_btnOptionIdSel);
	_lstnr	= NULL;
}

//

void AUIMsgBox::agregadoEnEscena(){
	AUEscenaObjeto::agregadoEnEscena();
	NBASSERT(!_onFocus) //cant have the focus
}

void AUIMsgBox::quitandoDeEscena(){
	AUEscenaObjeto::quitandoDeEscena();
	//Remove from focus (from topLayer)
	{
		NBASSERT((_onFocus && _top.layer.obj->contenedor() != NULL) || (!_onFocus && _top.layer.obj->contenedor() == NULL))
		this->hide();
	}
}

//

float AUIMsgBox::getPreferedRelPosVertical(const SI32 iScene, const float myPreferedPos){
	float r = myPreferedPos;
	if(iScene >= 0){
		//iPhoneSE: 4" (3.48 x 1.96)
		//iPhone6S: 4.7" (4.1 x 2.3)
		//iPhone6SPlus: 5.5" (4.79 x 2.7)
		//iPadMini: 7.9" (6.28 x 4.7)
		const NBTamano szInch = NBGestorEscena::tamanoPulgadasPantalla(iScene);
		const float szDiagInch2 = (szInch.ancho * szInch.ancho) + (szInch.alto * szInch.alto);
		if(szDiagInch2 >= (7.0f * 7.0f)){
			r = 0.5f;
		}
	}
	return r;
}

//

IEscuchadorIMsgBox* AUIMsgBox::getMsgBoxListener(){
	return _lstnr;
}

void AUIMsgBox::setMsgBoxListener(IEscuchadorIMsgBox* lstnr){
	_lstnr = lstnr;
}

STNBSize AUIMsgBox::getContentSizeForSceneLyr(const SI32 iScene, const ENGestorEscenaGrupo iGrp, STNBSize* dstBoxSz, STNBPoint* dstRelPos, NBMargenes* orgAndDstMarginIn, NBMargenes* orgAndDstMarginOut, NBMargenes* dstUnusableInnerContentSz, const UI32 maskContentExclude, const ENIMsgboxSize sizeH, const ENIMsgboxSize sizeV){
	STNBSize r; r.width = r.height = 0;
	if(iScene >= 0 && iGrp >= 0){
		const NBMargenes marginsScn = NBGestorEscena::getSceneMarginsScn(iScene, iGrp);
		const float relHeight		= (sizeV == ENIMsgboxSize_AllWindow ? 1.0f : sizeV == ENIMsgboxSize_AllUsable ? 0.85f : 0.75f);
		const NBCajaAABB sceneBox	= NBGestorEscena::cajaProyeccionGrupo(iScene, iGrp);
		NBTamano sceneSz;			NBCAJAAABB_TAMANO(sceneSz, sceneBox)
		const NBTamano sceneSzInch	= NBGestorEscena::tamanoEscenaAPulgadas(iScene, sceneSz.ancho, sceneSz.alto);
		/* Tamanos de pantallas en pulgadas
		Android Young:			1.87 x 2.75 pulgadas
		iPodTouch / iPhone:	2.00 x 3.00 pulgadas
		iPhone 5:				2.00 x 3.50 pulgadas
		iPad:					5.81 x 7.75 pulgadas
		iPad Mini:				4.70 x 6.28 pulgadas
		iPhone 11 Pro Max:		6.22 x 3.06 x 0.32 in (body)*/
		if(sceneSzInch.ancho < 3.1f){
			//Align bottom (small device)
			BOOL isTopAlign		= FALSE;
			STNBSize topBgSzScn;
			topBgSzScn.width	= sceneSz.ancho;
			topBgSzScn.height	= sceneSz.alto;
			//Set values
			if(_top.posRelFromBottom >= 0.0f && _top.posRelFromBottom <= 1.0f){
				isTopAlign	= (_top.posRelFromBottom > 0.5f);
			}
			if(dstRelPos != NULL){
				dstRelPos->x	= 0.5f;
				dstRelPos->y	= (isTopAlign ? 1.0f : 0.0f);
			}
			//Define modified margins
			{
				//Outter
				if(orgAndDstMarginOut != NULL){
					if(isTopAlign){
						//Do not use top marginOut
						orgAndDstMarginOut->top = 0.0f;
					} else {
						//Do not use btm marginOut
						orgAndDstMarginOut->bottom = 0.0f;
					}
					orgAndDstMarginOut->left	= 0.0f;
					orgAndDstMarginOut->right	= 0.0f;
				}
				//Innner
				if(orgAndDstMarginIn != NULL){
					if(isTopAlign){
						orgAndDstMarginIn->top	+= marginsScn.top;
					} else {
						orgAndDstMarginIn->bottom += marginsScn.bottom;
					}
					orgAndDstMarginIn->left		+= marginsScn.left;
					orgAndDstMarginIn->right	+= marginsScn.right;
				}
				//Apply texture extras
				if(_top.layer.opts.bg != NULL){
					AUTextura* tex = _top.layer.opts.bg->textura();
					if(tex != NULL){
						AUMallaMapa* map = tex->mallaMapa();
						if(map != NULL){
							const float first = tex->meshFirstPortionSzHD(ENMallaCorteSentido_Horizontal);
							const float last = tex->meshLastPortionSzHD(ENMallaCorteSentido_Horizontal);
							if(orgAndDstMarginOut != NULL){
								if(isTopAlign){
									orgAndDstMarginOut->top		= -first;
								} else {
									orgAndDstMarginOut->bottom	= -last;
								}
							}
							if(orgAndDstMarginIn != NULL){
								if(isTopAlign){
									orgAndDstMarginIn->top		+= first;
								} else {
									orgAndDstMarginIn->bottom	+= last;
								}
							}
						}
					}
				}
			}
			//Set outBox
			{
				if(orgAndDstMarginOut != NULL){
					topBgSzScn.width	-= (orgAndDstMarginOut->left + orgAndDstMarginOut->right);
					topBgSzScn.height	-= (orgAndDstMarginOut->top + orgAndDstMarginOut->bottom);
				}
				if(dstBoxSz != NULL){
					*dstBoxSz = topBgSzScn;
				}
			}
			//Set innerBox
			{
				r.width		= topBgSzScn.width;
				r.height	= topBgSzScn.height;
				if(orgAndDstMarginIn != NULL){
					r.width		-= (orgAndDstMarginIn->left + orgAndDstMarginIn->right);
					r.height	-= (orgAndDstMarginIn->top + orgAndDstMarginIn->bottom);
				}
			}
			//PRINTF_INFO("MsgBox-sz(%f, %f)-outter(%d%%, %d%%)-inner(%d%%, %d%%) scn-mrgn-top(%d%%)-btm(%d%%).\n", sceneSz.ancho, sceneSz.alto, (SI32)(topBgSzScn.width * 100.0f / sceneSz.ancho), (SI32)(topBgSzScn.height * 100.0f / sceneSz.alto), (SI32)(r.width * 100.0f / sceneSz.ancho), (SI32)(r.height * 100.0f / sceneSz.alto), (SI32)(marginsScn.top * 100.0f / sceneSz.alto), (SI32)(marginsScn.bottom * 100.0f / sceneSz.alto));
		} else {
			//Window mode
			NBTamano topBgSzMaxScn;
			topBgSzMaxScn.ancho				= (sceneBox.xMax - sceneBox.xMin);
			topBgSzMaxScn.alto				= (sceneBox.yMax - sceneBox.yMin);
			if(orgAndDstMarginOut != NULL){
				topBgSzMaxScn.ancho		-= (orgAndDstMarginOut->left + orgAndDstMarginOut->right);
				topBgSzMaxScn.alto		-= (orgAndDstMarginOut->top + orgAndDstMarginOut->bottom);
			}
			const NBTamano topBgSzMaxInch	= NBGestorEscena::tamanoEscenaAPulgadas(iScene, topBgSzMaxScn);
			STNBSize topBgSzScn; float relY = 0.5f;
			if(topBgSzMaxInch.ancho <= 3.6f){
				//Align bottom (small device)
				topBgSzScn.width	= topBgSzMaxScn.ancho;
				topBgSzScn.height	= topBgSzMaxScn.alto * relHeight;
				relY				= 0.0f; //bottom
			} else {
				//Align center (big device)
				if(sizeH == ENIMsgboxSize_AllWindow || sizeH == ENIMsgboxSize_AllUsable){
					topBgSzScn.width = topBgSzMaxScn.ancho; //3 inches
				} else {
					//ENIMsgboxSize_Auto, ENIMsgboxSize_Modal
					topBgSzScn.width = (topBgSzMaxScn.ancho * 3.5f) / topBgSzMaxInch.ancho; //3 inches
				}
				topBgSzScn.height	= topBgSzMaxScn.alto * relHeight;
				relY				= 0.5f; //center
			}
			if(_top.posRelFromBottom >= 0.0f && _top.posRelFromBottom <= 1.0f){
				relY = _top.posRelFromBottom;
			}
			if(dstBoxSz != NULL){
				*dstBoxSz = topBgSzScn;
			}
			if(dstRelPos != NULL){
				dstRelPos->x	= 0.5f;
				dstRelPos->y	= relY;
			}
			r.width		= topBgSzScn.width;
			r.height	= topBgSzScn.height;
			if(orgAndDstMarginIn != NULL){
				r.width		-= (orgAndDstMarginIn->left + orgAndDstMarginIn->right);
				r.height	-= (orgAndDstMarginIn->top + orgAndDstMarginIn->bottom);
			}
		}
		//Exclude
		{
			//Title
			if((maskContentExclude & ENIMsgboxContentMskBit_Title) != 0){
				if(_titleIsVisible){
					const char* txt			= _top.layer.opts.title->texto();
					const NBCajaAABB box	= _top.layer.opts.title->cajaParaTexto(txt, r.width);
					const float height		= (box.yMax - box.yMin);
					if(height != 0.0f){
						r.height -= height + _marginI;
						if(dstUnusableInnerContentSz != NULL){
							dstUnusableInnerContentSz->top += height + _marginI;
						}
					}
				}
			}
			//Buttons
			if((maskContentExclude & ENIMsgboxContentMskBit_Buttons) != 0){
				float btnsHeight = 0.0f;
				SI32 i, addedCount = 0; const SI32 count = _top.layer.opts.btns->conteo;
				for(i = 0 ; i < count; i++){
					AUIButton* btn			= (AUIButton*)_top.layer.opts.btns->elem(i);
					const NBCajaAABB box	= btn->cajaAABBLocalCalculada();
					const float height		= (box.yMax - box.yMin);
					PRINTF_INFO("MsgBox-btn-precalc-height: %f.\n", height);
					if(height != 0.0f){
						if(addedCount != 0) btnsHeight += _marginI;
						btnsHeight += height;
						addedCount++;
					}
				}
				if(btnsHeight != 0.0f){
					r.height -= btnsHeight + _marginI;
					if(dstUnusableInnerContentSz != NULL){
						dstUnusableInnerContentSz->bottom += btnsHeight + _marginI;
					}
				}
			}
		}
		//
		if(dstUnusableInnerContentSz != NULL && orgAndDstMarginIn != NULL){
			dstUnusableInnerContentSz->left		+= orgAndDstMarginIn->left;
			dstUnusableInnerContentSz->right	+= orgAndDstMarginIn->right;
			dstUnusableInnerContentSz->top		+= orgAndDstMarginIn->top;
			dstUnusableInnerContentSz->bottom	+= orgAndDstMarginIn->bottom;
		}
	}
	return r;
}

NBMargenes AUIMsgBox::getMarginsInner() const {
	return _marginsInn;
}

NBMargenes AUIMsgBox::getMarginsOuter() const {
	return _marginsOut;
}

float AUIMsgBox::getDefaultButtonsMarginI() const {
	return _btnsMarginI;
}

NBMargenes AUIMsgBox::getDefaultButtonsMargins() const {
	return _btnsMarginsInn;
}

BOOL AUIMsgBox::isTitleVisible() const {
	return _titleIsVisible;
}

BOOL AUIMsgBox::isContentTextVisible() const {
	return _contentTextIsVisible;
}

//

void AUIMsgBox::setMarginsInner(const NBMargenes margins){
	_marginsInn	= margins;
}

void AUIMsgBox::setMarginsInner(const float mLeft, const float mRight, const float mTop, const float mBottom){
	_marginsInn.left	= mLeft;
	_marginsInn.right	= mRight;
	_marginsInn.top		= mTop;
	_marginsInn.bottom	= mBottom;
}

void AUIMsgBox::setMarginsOuter(const NBMargenes margins){
	_marginsOut			= margins;
}

void AUIMsgBox::setMarginsOuter(const float mLeft, const float mRight, const float mTop, const float mBottom){
	_marginsOut.left	= mLeft;
	_marginsOut.right	= mRight;
	_marginsOut.top		= mTop;
	_marginsOut.bottom	= mBottom;
}

void AUIMsgBox::setDefaultButtonsMargins(const NBMargenes margins, const float marginI){
	_btnsMarginsInn	= margins;
	_btnsMarginI	= marginI;
}

void AUIMsgBox::setDefaultButtonsMargins(const float mLeft, const float mRight, const float mTop, const float mBottom, const float marginI){
	_btnsMarginsInn.left	= mLeft;
	_btnsMarginsInn.right	= mRight;
	_btnsMarginsInn.top		= mTop;
	_btnsMarginsInn.bottom	= mBottom;
	_btnsMarginI			= marginI;
}

void AUIMsgBox::setTitleVisible(const BOOL titleIsVisible){
	_titleIsVisible = titleIsVisible;
}

void AUIMsgBox::setContentTextVisible(const BOOL contentTextIsVisible){
	_contentTextIsVisible = contentTextIsVisible;
}

//

void AUIMsgBox::setHideWhenGlassTouched(const BOOL hideWhenGlassTouched){
	_hideWhenGlassTouched = hideWhenGlassTouched;
}

void AUIMsgBox::setTitleAlignment(const ENNBTextLineAlignH align){
	_titleAlignH = align;
}

void AUIMsgBox::setContentAlignment(const ENNBTextLineAlignH align){
	_contentAlignH = align;
}

void AUIMsgBox::setTitleColor(const NBColor8 color){
	this->setTitleColor(color.r, color.g, color.b, color.a);
}

void AUIMsgBox::setTitleColor(const STNBColor8 color){
	this->setTitleColor(color.r, color.g, color.b, color.a);
}

void AUIMsgBox::setTitleColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	_titleColor.r	= r;
	_titleColor.g	= g;
	_titleColor.b	= b;
	_titleColor.a	= a;
	_top.layer.opts.title->establecerMultiplicadorColor8(_titleColor.r, _titleColor.g, _titleColor.b, _titleColor.a);
}

//

void AUIMsgBox::privOrganizar(){
	//const float lineHeight = AUIMsgBox::altoParaLineasVisibles(1.0f, _fontTitle);
	//const float itmHeight = _marginsInn.top + lineHeight + _marginsInn.bottom;
}

void AUIMsgBox::privOrganizeTopContent(){
	NBMargenes marginIn = _marginsInn;
	NBMargenes marginOut = _marginsOut;
	STNBSize outterSz = NBST_P(STNBSize, 0, 0 );
	STNBPoint relPos = NBST_P(STNBPoint, 0, 0 );
	const STNBSize innerSz = this->getContentSizeForSceneLyr(_top.layer.obj->idEscena, (ENGestorEscenaGrupo)_top.layer.obj->idGrupo, &outterSz, &relPos, &marginIn, &marginOut, NULL, ENIMsgboxContentMskBit_None);
	if(innerSz.width > 0 && innerSz.height > 0 && outterSz.width > 0 && outterSz.height > 0){
		const NBCajaAABB sceneBox	= NBGestorEscena::cajaProyeccionGrupo(_top.layer.obj->idEscena, (ENGestorEscenaGrupo)_top.layer.obj->idGrupo);
		//Set bg
		{
			STNBAABox boxOuter;
			boxOuter.xMin = 0.0f;
			boxOuter.xMax = outterSz.width;
			boxOuter.yMin = -outterSz.height;
			boxOuter.yMax = 0.0f;
			//Set itms
			{
				STNBAABox boxInner;
				boxInner.xMin = boxOuter.xMin + marginIn.left;
				boxInner.xMax = boxOuter.xMax - marginIn.right;
				boxInner.yMin = boxOuter.yMin + marginIn.bottom;
				boxInner.yMax = boxOuter.yMax - marginIn.top;
				const float yMaxIni = boxInner.yMax;
				float heightForBtns = 0.0f;
				//Calculate buttons height
				{
					const float yTopBtns = boxInner.yMax;
					SI32 i; const SI32 count = _top.layer.opts.btns->conteo;
					for(i = 0 ; i < count; i++){
						AUIButton* btn = (AUIButton*)_top.layer.opts.btns->elem(i);
						btn->organizarContenido((boxInner.xMax - boxInner.xMin), 0.0f, false, NULL, 0.0f);
						{
							const NBCajaAABB box = btn->cajaAABBLocalCalculada();
							const float height = (box.yMax - box.yMin);
							if(height > 0.0f){
								//PRINTF_INFO("MsgBox-btn-organize-height: %f.\n", height);
								if(boxInner.yMax != yMaxIni) boxInner.yMax -= _marginI;
								boxInner.yMax -= (box.yMax - box.yMin);
							}
						}
					}
					heightForBtns = (yTopBtns - boxInner.yMax);
					boxInner.yMax = yTopBtns;
				}
				{
					float yTopCloseBtn = boxInner.yMax;
					float yTopTitle = boxInner.yMax;
					STNBSize closeBtnSz = NBST_P(STNBSize, 0.0f, 0.0f);
					//Calculate close btn size
					if(_top.layer.close.lyr != NULL){
						const NBCajaAABB box = _top.layer.close.lyr->cajaAABBLocal();
						closeBtnSz.width	= (box.xMax - box.xMin);
						closeBtnSz.height	= (box.yMax - box.yMin);
						_top.layer.close.lyr->establecerTraslacion(boxInner.xMax - box.xMax, boxInner.yMax - box.yMax);
						yTopCloseBtn		-= closeBtnSz.height;
					}
					//Tittle
					if(!_titleIsVisible || _top.layer.opts.title->textoLen() <= 0){
						_top.layer.opts.title->setVisibleAndAwake(FALSE);
					} else {
						float xDer = boxInner.xMax - closeBtnSz.width;
						const float relAlignH = (_titleAlignH == ENNBTextLineAlignH_Right ? 1.0f : _titleAlignH == ENNBTextLineAlignH_Center ? 0.5f : 0.0f);
						if(yTopTitle != yMaxIni) yTopTitle -= _marginI;
						//
						_top.layer.opts.title->setVisibleAndAwake(TRUE);
						_top.layer.opts.title->organizarTexto((boxInner.xMax - boxInner.xMin));
						const NBCajaAABB box	= _top.layer.opts.title->cajaAABBLocal();
						const float height		= (box.yMax - box.yMin);
						const STNBPoint pos		= NBST_P(STNBPoint, boxInner.xMin - box.xMin + (((xDer - boxInner.xMin) - (box.xMax - box.xMin)) * relAlignH), yTopTitle - box.yMax - (height < closeBtnSz.height ? ((closeBtnSz.height - height) * 0.5f) : 0.0f));
						_top.layer.opts.title->establecerTraslacion(pos.x, pos.y);
						yTopTitle				-= (box.yMax - box.yMin);
					}
					//Set bottom
					boxInner.yMax = (yTopCloseBtn < yTopTitle ? yTopCloseBtn : yTopTitle);
				}
				//CustomContent-Top
				if(_top.layer.opts.customTop != NULL){
					const NBTamano scale = _top.layer.opts.customTop->escalacion();
					NBCajaAABB box = _top.layer.opts.customTop->cajaAABBLocal();
					if(boxInner.yMax != yMaxIni && box.yMin < box.yMax) boxInner.yMax -= _marginI;
					box.xMin *= scale.ancho;
					box.xMax *= scale.ancho;
					box.yMin *= scale.alto;
					box.yMax *= scale.alto;
					const float relAlignH = (_contentAlignH == ENNBTextLineAlignH_Right ? 1.0f : _contentAlignH == ENNBTextLineAlignH_Center ? 0.5f : 0.0f);
					_top.layer.opts.customTop->establecerTraslacion(boxInner.xMin - box.xMin + (((boxInner.xMax - boxInner.xMin) - (box.xMax - box.xMin)) * relAlignH), boxInner.yMax - box.yMax);
					boxInner.yMax -= (box.yMax - box.yMin);
				}
				//Content
				if(!_contentTextIsVisible || _top.layer.opts.text->getTextLen() <= 0){
					_top.layer.opts.text->setVisibleAndAwake(FALSE);
				} else {
					_top.layer.opts.text->setVisibleAndAwake(TRUE);
					_top.layer.opts.text->setMargins(0.0f, 0.0f, _marginI, _marginI);
					_top.layer.opts.text->establecerAlineacionH(_contentAlignH);
					_top.layer.opts.text->organizaContenido((boxInner.xMax - boxInner.xMin), (boxInner.yMax - boxInner.yMin) - heightForBtns);
					{
						const NBCajaAABB box = _top.layer.opts.text->cajaAABBLocalCalculada();
						const float relAlignH = (_contentAlignH == ENNBTextLineAlignH_Right ? 1.0f : _contentAlignH == ENNBTextLineAlignH_Center ? 0.5f : 0.0f);
						_top.layer.opts.text->establecerTraslacion(boxInner.xMin - box.xMin + (((boxInner.xMax - boxInner.xMin) - (box.xMax - box.xMin)) * relAlignH), boxInner.yMax - box.yMax);
						boxInner.yMax -= (box.yMax - box.yMin);
					}
				}
				//CustomContent-Btm
				if(_top.layer.opts.customBtm != NULL){
					const NBTamano scale = _top.layer.opts.customBtm->escalacion();
					NBCajaAABB box = _top.layer.opts.customBtm->cajaAABBLocal();
					if(boxInner.yMax != yMaxIni && box.yMin < box.yMax) boxInner.yMax -= _marginI;
					box.xMin *= scale.ancho;
					box.xMax *= scale.ancho;
					box.yMin *= scale.alto;
					box.yMax *= scale.alto;
					const float relAlignH = (_contentAlignH == ENNBTextLineAlignH_Right ? 1.0f : _contentAlignH == ENNBTextLineAlignH_Center ? 0.5f : 0.0f);
					_top.layer.opts.customBtm->establecerTraslacion(boxInner.xMin - box.xMin + (((boxInner.xMax - boxInner.xMin) - (box.xMax - box.xMin)) * relAlignH), boxInner.yMax - box.yMax);
					boxInner.yMax -= (box.yMax - box.yMin);
				}
				//Buttons
				{
					SI32 i; const SI32 count = _top.layer.opts.btns->conteo;
					for(i = 0 ; i < count; i++){
						AUIButton* btn = (AUIButton*)_top.layer.opts.btns->elem(i);
						const NBCajaAABB box = btn->cajaAABBLocalCalculada();
						//PRINTF_INFO("MsgBox-btn-organize-height: %f.\n", height);
						if(boxInner.yMax != yMaxIni && box.yMin < box.yMax) boxInner.yMax -= _marginI;
						btn->establecerTraslacion(boxInner.xMin - box.xMin, boxInner.yMax - box.yMax);
						boxInner.yMax -= (box.yMax - box.yMin);
					}
				}
				boxOuter.yMin = boxInner.yMax - marginIn.bottom;
				NBASSERT(boxOuter.yMin <= boxOuter.yMax)
			}
			//PRINTF_INFO("relY(%f).\n", relPos.y);
			_top.layer.opts.bg->redimensionar(boxOuter.xMin, boxOuter.yMin, (boxOuter.xMax - boxOuter.xMin), (boxOuter.yMax - boxOuter.yMin));
			_top.posIn	= NBST_P(STNBPoint, sceneBox.xMin - boxOuter.xMin + (((sceneBox.xMax - sceneBox.xMin) - (boxOuter.xMax - boxOuter.xMin)) * 0.5f), sceneBox.yMin - boxOuter.yMin + marginOut.bottom + (((sceneBox.yMax - sceneBox.yMin - marginOut.bottom - marginOut.top) - (boxOuter.yMax - boxOuter.yMin)) * relPos.y) );
			if(relPos.y <= 0.5f){
				_top.posOut	= NBST_P(STNBPoint, _top.posIn.x, sceneBox.yMin - boxOuter.yMax - 1.0f);
			} else {
				_top.posOut	= NBST_P(STNBPoint, _top.posIn.x, sceneBox.yMax - boxOuter.yMin + 1.0f);
			}
			//Change to horizontal-in instead of vertical-in
			if(_top.layer.obj->idEscena >= 0){
				//iPhoneSE: 4" (3.48 x 1.96)
				//iPhone6S: 4.7" (4.1 x 2.3)
				//iPhone6SPlus: 5.5" (4.79 x 2.7)
				//iPadMini: 7.9" (6.28 x 4.7)
				//iPhone 11 Pro Max: 6.22 x 3.06 x 0.32 in (body)
				const NBTamano szInch	= NBGestorEscena::tamanoPulgadasPantalla(_top.layer.obj->idEscena);
				const float szDiagInch2	= (szInch.ancho * szInch.ancho) + (szInch.alto * szInch.alto);
				if(szDiagInch2 >= (7.0f * 7.0f)){
					if(szInch.ancho < szInch.alto){
						_top.posOut.x = sceneBox.xMin - boxOuter.xMax - 1.0f;
						_top.posOut.y = _top.posIn.y;
					}
				}
			}
		}
		//Glass
		_top.layer.glass->redimensionar(sceneBox.xMin - 1.0f, sceneBox.yMin - 1.0f, (sceneBox.xMax - sceneBox.xMin) + 2.0f, (sceneBox.yMax - sceneBox.yMin) + 2.0f);
	}
}

//

BOOL AUIMsgBox::isCustomContentAny() const {
	return (_top.layer.opts.customTop != NULL || _top.layer.opts.customBtm != NULL ? TRUE : FALSE);
}

BOOL AUIMsgBox::isCustomContentTop() const {
	return (_top.layer.opts.customTop != NULL ? TRUE : FALSE);
}

BOOL AUIMsgBox::isCustomContentBtm() const {
	return (_top.layer.opts.customBtm != NULL ? TRUE : FALSE);
}

void AUIMsgBox::setCustomContentTop(AUEscenaObjeto* custom){
	if(_top.layer.opts.customTop != custom){
		if(custom != NULL){
			custom->retener(NB_RETENEDOR_THIS);
			_top.layer.opts.textLayer->agregarObjetoEscena(custom);
		}
		if(_top.layer.opts.customTop != NULL){
			AUEscenaContenedor* parent = (AUEscenaContenedor*)_top.layer.opts.customTop->contenedor();
			if(parent != NULL) parent->quitarObjetoEscena(_top.layer.opts.customTop);
			_top.layer.opts.customTop->liberar(NB_RETENEDOR_THIS);
		}
		_top.layer.opts.customTop = custom;
	}
}

void AUIMsgBox::setCustomContentBtm(AUEscenaObjeto* custom){
	if(_top.layer.opts.customBtm != custom){
		if(custom != NULL){
			custom->retener(NB_RETENEDOR_THIS);
			_top.layer.opts.textLayer->agregarObjetoEscena(custom);
		}
		if(_top.layer.opts.customBtm != NULL){
			AUEscenaContenedor* parent = (AUEscenaContenedor*)_top.layer.opts.customBtm->contenedor();
			if(parent != NULL) parent->quitarObjetoEscena(_top.layer.opts.customBtm);
			_top.layer.opts.customBtm->liberar(NB_RETENEDOR_THIS);
		}
		_top.layer.opts.customBtm = custom;
	}
}

//close

void AUIMsgBox::setCloseIco(const ENIMsgboxCloseBtnLyr iLyr, AUTextura* tex, const STNBColor8 color, const float rot, const STNBSize scale){
	this->privSetCloseIco(iLyr, tex, color, rot, scale);
}

void AUIMsgBox::privSetCloseIco(const ENIMsgboxCloseBtnLyr iLyr, AUTextura* tex, const STNBColor8 color, const float rot, const STNBSize scale){
	if(iLyr >= 0 && iLyr < ENIMsgboxCloseBtnLyr_Count){
		if(tex == NULL){ 
			STIMsgboxCloseBtnLyr* ico = &_top.layer.close.icos[iLyr];
			if(ico->ico != NULL){
				AUEscenaContenedor* prnt = (AUEscenaContenedor*)ico->ico->contenedor();
				if(prnt != NULL) prnt->quitarObjetoEscena(ico->ico);
				ico->ico->liberar();
				ico->ico = NULL;
			}
			//Remove layer
			{
				BOOL lyrsRemain = FALSE;
				SI32 i; for(i = 0; i < ENIMsgboxCloseBtnLyr_Count; i++){
					STIMsgboxCloseBtnLyr* ico = &_top.layer.close.icos[i];
					if(ico->ico != NULL){
						lyrsRemain = TRUE;
						break;
					}
				}
				if(!lyrsRemain){
					if(_top.layer.close.lyr != NULL) _top.layer.close.lyr->liberar(); _top.layer.close.lyr = NULL;
					if(_top.layer.close.dummy[0] != NULL) _top.layer.close.dummy[0]->liberar(); _top.layer.close.dummy[0] = NULL;
					if(_top.layer.close.dummy[1] != NULL) _top.layer.close.dummy[1]->liberar(); _top.layer.close.dummy[1] = NULL;
					{
						SI32 i; for(i = 0; i < ENIMsgboxCloseBtnLyr_Count; i++){
							STIMsgboxCloseBtnLyr* ico = &_top.layer.close.icos[i];
							if(ico->ico != NULL){
								AUEscenaContenedor* prnt = (AUEscenaContenedor*)ico->ico->contenedor();
								if(prnt != NULL) prnt->quitarObjetoEscena(ico->ico);
								ico->ico->liberar();
								ico->ico = NULL;
							}
						}
					}
				}
			}
		} else {
			//Create layer (if necesary)
			if(_top.layer.close.lyr == NULL){
				_top.layer.close.lyr = new(this) AUEscenaContenedorLimitado();
				_top.layer.close.lyr->establecerEscuchadorTouches(this, this);
				_top.layer.opts.layer->agregarObjetoEscena(_top.layer.close.lyr);
				//
				_top.layer.close.dummy[0] = new(this) AUEscenaSprite(2.0f, 2.0f);
				_top.layer.close.dummy[0]->establecerMultiplicadorAlpha8(0);
				_top.layer.close.lyr->agregarObjetoEscena(_top.layer.close.dummy[0]);
				//
				_top.layer.close.dummy[1] = new(this) AUEscenaSprite(2.0f, 2.0f);
				_top.layer.close.dummy[1]->establecerMultiplicadorAlpha8(0);
				_top.layer.close.lyr->agregarObjetoEscena(_top.layer.close.dummy[1]);
			}
			//Add-or-set
			{
				STIMsgboxCloseBtnLyr* ico = &_top.layer.close.icos[iLyr];
				if(ico->ico == NULL){
					ico->ico = new(this) AUEscenaSprite(tex);
					_top.layer.close.lyr->agregarObjetoEscenaEnIndice(ico->ico, iLyr);
				} else {
					ico->ico->establecerTextura(tex);
				}
				{
					const NBTamano texSz = tex->tamanoHD();
					ico->ico->redimensionar(texSz.ancho * scale.width, texSz.alto * scale.height);
				}
				ico->ico->establecerRotacion(rot);
				ico->ico->establecerMultiplicadorColor8(color.r, color.g, color.b, color.a);
				ico->rot	= 0.0f;
				ico->color	= color;
			}
		}
	}
}

//btns

UI32 AUIMsgBox::getButtonsCount() const {
	return (UI32) _top.layer.opts.btns->conteo;
}

void AUIMsgBox::addButton(const char* optionId, const ENIMsgboxBtnAction action, const char* text, AUTextura* icon, AUTextura* bgTex, const STNBColor8 bgColor, const STNBColor8 fgColor){
	this->privAddButton(optionId, action, text, icon, bgTex, bgColor, fgColor, FALSE);
}

void AUIMsgBox::addButtonAtTop(const char* optionId, const ENIMsgboxBtnAction action, const char* text, AUTextura* icon, AUTextura* bgTex, const STNBColor8 bgColor, const STNBColor8 fgColor){
	this->privAddButton(optionId, action, text, icon, bgTex, bgColor, fgColor, TRUE);
}

void AUIMsgBox::privAddButton(const char* optionId, const ENIMsgboxBtnAction action, const char* text, AUTextura* icon, AUTextura* bgTex, const STNBColor8 bgColor, const STNBColor8 fgColor, const BOOL atTop){
	NBColor8 bgs[3], fgs[3];
	NBCOLOR_ESTABLECER(bgs[0], bgColor.r, bgColor.g, bgColor.b, bgColor.a)
	NBCOLOR_ESTABLECER(bgs[1], ((SI32)bgColor.r * 200 / 255), ((SI32)bgColor.g * 200 / 255), ((SI32)bgColor.b * 200 / 255), bgColor.a)
	NBCOLOR_ESTABLECER(bgs[2], ((SI32)bgColor.r * 150 / 255), ((SI32)bgColor.g * 150 / 255), ((SI32)bgColor.b * 150 / 255), bgColor.a)
	NBCOLOR_ESTABLECER(fgs[0], fgColor.r, fgColor.g, fgColor.b, fgColor.a)
	NBCOLOR_ESTABLECER(fgs[1], ((SI32)fgColor.r * 200 / 255), ((SI32)fgColor.g * 200 / 255), ((SI32)fgColor.b * 200 / 255), fgColor.a)
	NBCOLOR_ESTABLECER(fgs[2], ((SI32)fgColor.r * 150 / 255), ((SI32)fgColor.g * 150 / 255), ((SI32)fgColor.b * 150 / 255), fgColor.a)
	AUIButton* obj = new(this) AUIButton();
	if(icon != NULL){
		obj->agregarIcono(icon, ENIBotonItemAlign_Center, 0.5f, 0, 0, fgs[0], fgs[1], fgs[2]);
	}
	if(_fontBtns != NULL && text != NULL){
		if(text[0] != '\0'){
			obj->agregarTextoMultilinea(_fontBtns, text, ENIBotonItemAlign_Center, 0.5f, 0, 0, fgs[0], fgs[1], fgs[2]);
		}
	}
	obj->establecerFondo(true, bgTex);
	obj->establecerFondoColores(bgs[0], bgs[1], bgs[2]);
	obj->establecerMargenes(_btnsMarginsInn.left, _btnsMarginsInn.right, _btnsMarginsInn.top, _btnsMarginsInn.bottom, _btnsMarginI);
	obj->establecerSegsRetrasarOnTouch(0.0f);
	obj->establecerOptionId(optionId);
	obj->establecerRefDatos(0, action);
	obj->establecerEscuchadorBoton(this);
	obj->organizarContenido(false);
	if(atTop){
		_top.layer.opts.btns->agregarElementoEnIndice(obj, 0);
		_top.layer.opts.btnsLayer->agregarObjetoEscenaEnIndice(obj, 0);
	} else {
		_top.layer.opts.btns->agregarElemento(obj);
		_top.layer.opts.btnsLayer->agregarObjetoEscena(obj);
	}
	obj->liberar(NB_RETENEDOR_THIS);
}

void AUIMsgBox::updateButtonText(const char* optionId, const char* text){
	SI32 i; const SI32 count = _top.layer.opts.btns->conteo;
	for(i = 0 ; i < count; i++){
		AUIButton* btn = (AUIButton*)_top.layer.opts.btns->elem(i);
		if(NBString_strIsEqual(optionId, btn->refOptionId())){
			btn->actualizarTextosMultilineaPorTipo(0, text);
			btn->organizarContenido(false);
		}
	}
}

void AUIMsgBox::show(){
	float relV = 0.0f;
	if(this->idEscena >= 0){
		relV = AUIMsgBox::getPreferedRelPosVertical(this->idEscena, relV);
	}
	this->show(relV);
}

void AUIMsgBox::show(const float posRelFromBottom){
	if(!_onFocus){
		if(this->addToTop(_top.layer.obj)){
			NBASSERT(_top.layer.obj->idEscena >= 0 && _top.layer.obj->idGrupo >= 0)
			//
			_top.posRelFromBottom = posRelFromBottom;
			//
			this->privOrganizeTopContent();
			if(!_onAnim){
				NBGestorEscena::agregarEscuchadorCambioPuertoVision(this->idEscena, this);
				NBGestorAnimadores::agregarAnimador(this, this);
				_onAnim = TRUE;
			}
			//Init in-animation
			{
				_top.anim.isAnimIn	= TRUE;
				_top.anim.secsCur	= 0.0f;
				_top.anim.secsDur	= AUIMsgBox_BOX_ANIM_DURATION;
				this->privTopAnimate(0.0f); //initial location
			}
			_onFocus = TRUE;
			//Notify
			if(_lstnr != NULL){
				_lstnr->msgboxFocusObtained(this);
			}
		}
	}
}

void AUIMsgBox::show(const ENNBTextLineAlignH alignH, const char* title, const char* content){
	float relV = 0.0f;
	if(this->idEscena >= 0){
		relV = AUIMsgBox::getPreferedRelPosVertical(this->idEscena, relV);
	}
	this->show(alignH, title, content, relV);
}

void AUIMsgBox::show(const ENNBTextLineAlignH alignH, const char* title, const char* content, const float posRelFromBottom){
	if(!_onFocus){
		_contentAlignH = alignH;
		//_top.layer.opts.title->establecerAlineacionH((alignH == ENNBTextLineAlignH_Adjust ? ENNBTextLineAlignH_Left : alignH));
		_top.layer.opts.title->establecerTexto(title);
		//_top.layer.opts.text->establecerAlineacionH(alignH);
		_top.layer.opts.text->establecerTexto(content);
		this->show(posRelFromBottom);
	}
}

void AUIMsgBox::updateContent(){
	if(_onFocus){
		this->privOrganizeTopContent();
		if(_top.anim.secsDur == 0.0f){
			_top.layer.opts.layer->establecerTraslacion(_top.posIn.x, _top.posIn.y);
		}
	}
	
}

void AUIMsgBox::updateContent(const ENNBTextLineAlignH alignH, const char* title, const char* content){
	_contentAlignH = alignH;
	//_top.layer.opts.title->establecerAlineacionH((alignH == ENNBTextLineAlignH_Adjust ? ENNBTextLineAlignH_Left : alignH));
	_top.layer.opts.title->establecerTexto(title);
	//_top.layer.opts.text->establecerAlineacionH(alignH);
	_top.layer.opts.text->establecerTexto(content);
	if(_onFocus){
		this->privOrganizeTopContent();
		if(_top.anim.secsDur == 0.0f){
			_top.layer.opts.layer->establecerTraslacion(_top.posIn.x, _top.posIn.y);
		}
	}
}

void AUIMsgBox::hide(){
	if(_onFocus){
		_top.anim.isAnimIn	= FALSE;
		_top.anim.secsCur	= 0.0f;
		_top.anim.secsDur	= AUIMsgBox_BOX_ANIM_DURATION;
		this->privTopAnimate(0.0f); //initial location
	}
}

//

void AUIMsgBox::puertoDeVisionModificado(const SI32 iEscena, const STNBViewPortSize before, const STNBViewPortSize after){
	if(_onFocus){
		this->privOrganizeTopContent();
	}
}

//

void AUIMsgBox::privTopAnimate(const float secs){
	if(_top.anim.secsDur > 0.0f){
		_top.anim.secsCur += secs;
		{
			const float glassAlpha8 = 150.0f;
			float rel			= (_top.anim.secsCur / _top.anim.secsDur);
			if(_top.anim.secsCur >= _top.anim.secsDur){
				rel				= 1.0f;
				_top.anim.secsCur = 0.0f;
				_top.anim.secsDur = 0.0f;
			} NBASSERT(rel >= 0.0f && rel <= 1.0f)
			//Apply
			if(_top.anim.isAnimIn){
				//Deacelerated
				const float rel1 = (1.0f - rel);
				const float rel2 = rel1 * rel1 * rel1;
				const STNBPoint pos = {
					_top.posIn.x + ((_top.posOut.x - _top.posIn.x) * rel2)
					, _top.posIn.y + ((_top.posOut.y - _top.posIn.y) * rel2)
				};
				_top.layer.opts.layer->establecerMultiplicadorAlpha8(255);
				_top.layer.opts.layer->establecerTraslacion(pos.x, pos.y);
				_top.layer.glass->establecerMultiplicadorAlpha8(glassAlpha8 * rel);
				//PRINTF_INFO("Anim(%.2f) pos(%f, %f).\n", rel, pos.x, pos.y);
			} else {
				//Accelerated
				const float rel2 = (rel * rel * rel);
				const STNBPoint pos = {
					_top.posIn.x + ((_top.posOut.x - _top.posIn.x) * rel2)
					, _top.posIn.y + ((_top.posOut.y - _top.posIn.y) * rel2)
				};
				_top.layer.opts.layer->establecerMultiplicadorAlpha8(255);
				_top.layer.opts.layer->establecerTraslacion(pos.x, pos.y);
				_top.layer.glass->establecerMultiplicadorAlpha8(glassAlpha8 * (1.0f - rel));
				//PRINTF_INFO("Anim(%.2f) pos(%f, %f).\n", rel, pos.x, pos.y);
				//Remove from top
				if(rel >= 1.0f){
					this->privRemoveFromTopPriv();
				}
			}
		}
	}
}

void AUIMsgBox::privRemoveFromTopPriv(){
	NBASSERT((_onFocus && _top.layer.obj->contenedor() != NULL) || (!_onFocus && _top.layer.obj->contenedor() == NULL))
	if(_onFocus){
		if(_onAnim){
			NBGestorAnimadores::quitarAnimador(this);
			NBGestorEscena::quitarEscuchadorCambioPuertoVision(this->idEscena, this);
			_onAnim = FALSE;
		}
		//Remove from focus (from topLayer)
		if(_top.layer.obj != NULL){
			AUEscenaContenedor* parent = (AUEscenaContenedor*)_top.layer.obj->contenedor(); NBASSERT(parent != NULL)
			if(parent != NULL) parent->quitarObjetoEscena(_top.layer.obj);
		}
		_onFocus = FALSE;
		//Notify
		if(_lstnr != NULL){
			//Notify selection
			if(_btnOptionIsPendNotify){
				char* optId = NBString_strNewBuffer(_btnOptionIdSel.str);
				//Reset (before notifying)
				{
					_btnOptionIsPendNotify = FALSE;
					NBString_empty(&_btnOptionIdSel);
				}
				//Notify
				{
					_lstnr->msgboxOptionSelected(this, optId);
				}
				NBMemory_free(optId);
			}
			//notify close
			if(_btnCloseIsPendNotify){
				//Reset before notifying
				{
					_btnCloseIsPendNotify = FALSE;
				}
				//Notify
				{
					_lstnr->msgboxCloseSelected(this);
				}
			}
			//Notify hidden
			_lstnr->msgboxFocusLost(this);
		}
	}
}

//

NBCajaAABB AUIMsgBox::cajaAABBLocalCalculada(){
	NBCajaAABB r;
	r.xMin = r.xMax = r.yMin = r.yMax = 0.0f;
	return r;
}

PTRfunCmdsGL AUIMsgBox::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	return NULL;
}

//

void AUIMsgBox::tickAnimacion(float segsTranscurridos){
	//Animate
	if(_top.anim.secsDur > 0.0f){
		this->privTopAnimate(segsTranscurridos);
	}
}

//

void AUIMsgBox::botonPresionado(AUIButton* obj){
	//
}

void AUIMsgBox::botonAccionado(AUIButton* obj){
	const STListaItemDatos data = obj->refDatos();
	if(data.valor == ENIMsgboxBtnAction_AutohideAndNotify){
		//Autohide
		_btnOptionIsPendNotify = TRUE;
		NBString_set(&_btnOptionIdSel, obj->refOptionId());
		this->hide();
	} else {
		_btnOptionIsPendNotify = FALSE;
		NBString_empty(&_btnOptionIdSel);
		if(_lstnr != NULL){
			_lstnr->msgboxOptionSelected(this, obj->refOptionId());
		}
	}
}

AUEscenaObjeto* AUIMsgBox::botonHerederoParaTouch(AUIButton* obj, const NBPunto &posInicialEscena, const NBPunto &posActualEscena){
	return NULL;
}

//TOUCHES

void AUIMsgBox::touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto){
	if(_top.layer.glass == objeto){
		//Will hide
	} else if(_top.layer.close.lyr == objeto){
		objeto->establecerMultiplicadorAlpha8(200);
	}
}

void AUIMsgBox::touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	if(_top.layer.glass == objeto){
		//Will hide
	}
}

void AUIMsgBox::touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	if(_top.layer.glass == objeto || _top.layer.close.lyr == objeto){
		//
		if(_top.layer.close.lyr == objeto){
			objeto->establecerMultiplicadorAlpha8(255);
			_btnCloseIsPendNotify = TRUE;
		}
		//
		if(_hideWhenGlassTouched || _top.layer.close.lyr == objeto || _top.layer.glass != objeto){
			if(_top.anim.secsDur == 0){
				this->hide();
			} else if(_top.anim.isAnimIn){
				//Invert current animation
				NBASSERT(_top.anim.secsCur < _top.anim.secsDur)
				_top.anim.isAnimIn	= FALSE;
				_top.anim.secsCur	= (_top.anim.secsDur - _top.anim.secsCur);
			}
		}
	}
	//Process
	if(!touch->cancelado){
		const NBCajaAABB cajaEscena = objeto->cajaAABBEnEscena();
		if(NBCAJAAABB_INTERSECTA_PUNTO(cajaEscena, posActualEscena.x, posActualEscena.y) && !touch->cancelado){
			//
		}
	}
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUIMsgBox, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUIMsgBox, "AUIMsgBox", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_NULL(AUIMsgBox)


