//
//  AUISelectBox.h
//  lib-auframework-media
//
//  Created by Marcos Ortega on 9/3/18.
//

#ifndef AUIMsgBox_h
#define AUIMsgBox_h

#include "nb/core/NBDate.h"
#include "NBMargenes.h"
#include "NBAnimador.h"
#include "AUEscenaContenedor.h"
#include "AUEscenaContenedorLimitado.h"
#include "AUEscenaFigura.h"
#include "AUEscenaTexto.h"
#include "AUEscenaSpriteElastico.h"
#include "AUEscenaPanelTexto.h"
#include "AUIButton.h"
#include "NBGestorEscena.h"

typedef enum ENIMsgboxBtnAction_ {
	ENIMsgboxBtnAction_AutohideAndNotify = 0,
	ENIMsgboxBtnAction_Notify
} ENIMsgboxBtnAction;

typedef enum ENIMsgboxSize_ {
	ENIMsgboxSize_Auto = 0,		//Automatically selected depending of the screen/window size
	ENIMsgboxSize_AllWindow,	//All the area of the window (including the top bars, bottom bars, left, right)
	ENIMsgboxSize_AllUsable,	//Use all the area of the window excluding the top bars, bottom bars, left, right
	ENIMsgboxSize_Modal		//Use a portion of the screen, the content behind will be visible.
} ENIMsgboxSize;

typedef enum ENIMsgboxContentMskBit_ {
	ENIMsgboxContentMskBit_None		= 0,	//
	ENIMsgboxContentMskBit_Title	= 1,	//Title
	ENIMsgboxContentMskBit_Buttons	= 2,	//Buttons on the bottom
	ENIMsgboxContentMskBit_MarginInt = 4,	//Internal margins
	ENIMsgboxContentMskBit_All		= 0xFF,	//All
} ENIMsgboxContentMskBit;

typedef enum ENIMsgboxCloseBtnLyr_ {
	ENIMsgboxCloseBtnLyr_Back = 0,		//back layer of close-icon
	ENIMsgboxCloseBtnLyr_Front,			//front layer of close-icon
	ENIMsgboxCloseBtnLyr_Count
} ENIMsgboxCloseBtnLyr;

typedef struct STIMsgboxCloseBtnLyr_ {
	AUEscenaSprite*		ico;
	float				rot;
	STNBColor8			color;
} STIMsgboxCloseBtnLyr;

class AUIMsgBox;

class IEscuchadorIMsgBox {
public:
	virtual ~IEscuchadorIMsgBox(){
		//
	}
	virtual void msgboxOptionSelected(AUIMsgBox* obj, const char* optionId) = 0;
	virtual void msgboxCloseSelected(AUIMsgBox* obj) = 0;
	virtual void msgboxFocusObtained(AUIMsgBox* obj) = 0;
	virtual void msgboxFocusLost(AUIMsgBox* obj) = 0;
};

class AUIMsgBox : public AUEscenaObjeto, public IEscuchadorTouchEscenaObjeto, public IEscuchadorCambioPuertoVision, public NBAnimador, public IEscuchadorIBoton {
public:
	AUIMsgBox(AUFuenteRender* fontTitle, AUFuenteRender* fontContent, AUFuenteRender* fontBtns, AUTextura* bgTex, AUTextura* scrollTex, const STNBColor8 scrollColor8, const float marginI);
	AUIMsgBox(AUFuenteRender* fontTitle, AUFuenteRender* fontContent, AUFuenteRender* fontBtns, AUTextura* bgTex, AUTextura* scrollTex, const STNBColor8 scrollColor8, const float marginI, const NBMargenes margins);
	AUIMsgBox(AUFuenteRender* fontTitle, AUFuenteRender* fontContent, AUFuenteRender* fontBtns, AUTextura* bgTex, AUTextura* scrollTex, const STNBColor8 scrollColor8, const float marginI, const float mLeft, const float mRight, const float mTop, const float mBottom);
	virtual ~AUIMsgBox();
	//
	static float		getPreferedRelPosVertical(const SI32 iScene, const float myPreferedPos);
	//
	IEscuchadorIMsgBox* getMsgBoxListener();
	void				setMsgBoxListener(IEscuchadorIMsgBox* lstnr);
	//Selection props
	STNBSize			getContentSizeForSceneLyr(const SI32 iScene, const ENGestorEscenaGrupo iGrp, STNBSize* dstBoxSz, STNBPoint* dstRelPos, NBMargenes* orgAndDstMarginIn, NBMargenes* orgAndDstMarginOut, NBMargenes* dstUnusableInnerContentSz, const UI32 maskContentExclude, const ENIMsgboxSize sizeH = ENIMsgboxSize_Auto, const ENIMsgboxSize sizeV = ENIMsgboxSize_Auto);
	NBMargenes			getMarginsInner() const;
	NBMargenes			getMarginsOuter() const;
	float				getDefaultButtonsMarginI() const;
	NBMargenes			getDefaultButtonsMargins() const;
	BOOL				isTitleVisible() const;
	BOOL				isContentTextVisible() const;
	void				setMarginsInner(const NBMargenes margins);
	void				setMarginsInner(const float mLeft, const float mRight, const float mTop, const float mBottom);
	void				setMarginsOuter(const NBMargenes margins);
	void				setMarginsOuter(const float mLeft, const float mRight, const float mTop, const float mBottom);
	void				setDefaultButtonsMargins(const NBMargenes margins, const float marginI);
	void				setDefaultButtonsMargins(const float mLeft, const float mRight, const float mTop, const float mBottom, const float marginI);
	void				setTitleVisible(const BOOL titleIsVisible);
	void				setContentTextVisible(const BOOL contentTextIsVisible);
	//
	void				setHideWhenGlassTouched(const BOOL hideWhenGlassTouched);
	void				setTitleAlignment(const ENNBTextLineAlignH align);
	void				setContentAlignment(const ENNBTextLineAlignH align);
	void				setTitleColor(const NBColor8 color);
	void				setTitleColor(const STNBColor8 color);
	void				setTitleColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a);
	//
	BOOL				isCustomContentAny() const;
	BOOL				isCustomContentTop() const;
	BOOL				isCustomContentBtm() const;
	void				setCustomContentTop(AUEscenaObjeto* custom);
	void				setCustomContentBtm(AUEscenaObjeto* custom);
	//close
	void				setCloseIco(const ENIMsgboxCloseBtnLyr iLyr, AUTextura* tex, const STNBColor8 color, const float rot, const STNBSize scale);
	//btns
	UI32				getButtonsCount() const;
	void				addButton(const char* optionId, const ENIMsgboxBtnAction action, const char* text, AUTextura* icon, AUTextura* bgTex, const STNBColor8 bgColor, const STNBColor8 fgColor);
	void				addButtonAtTop(const char* optionId, const ENIMsgboxBtnAction action, const char* text, AUTextura* icon, AUTextura* bgTex, const STNBColor8 bgColor, const STNBColor8 fgColor);
	void				updateButtonText(const char* optionId, const char* text);
	//
	void				show();
	void				show(const float posRelFromBottom);
	void				show(const ENNBTextLineAlignH alignH, const char* title, const char* content);
	void				show(const ENNBTextLineAlignH alignH, const char* title, const char* content, const float posRelFromBottom);
	void				updateContent();
	void				updateContent(const ENNBTextLineAlignH alignH,const char* title, const char* content);
	void				hide();
	//
	//ViewPort
	void				puertoDeVisionModificado(const SI32 iEscena, const STNBViewPortSize before, const STNBViewPortSize after);
	//
	NBCajaAABB			cajaAABBLocalCalculada();
	PTRfunCmdsGL		actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
	//
	void				tickAnimacion(float segsTranscurridos);
	//
	void				botonPresionado(AUIButton* obj);
	void				botonAccionado(AUIButton* obj);
	AUEscenaObjeto*		botonHerederoParaTouch(AUIButton* obj, const NBPunto &posInicialEscena, const NBPunto &posActualEscena);
	//TOUCHES
	void				touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto);
	void				touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
	void				touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
	//
	AUOBJMETODOS_CLASESID_DEFINICION
	AUOBJMETODOS_CLASESNOMBRES_DEFINICION
	AUOBJMETODOS_CLONAR_DEFINICION
protected:
	float						_marginI;
	NBMargenes					_marginsInn;
	NBMargenes					_marginsOut;
	ENNBTextLineAlignH			_titleAlignH;
	BOOL						_titleIsVisible;
	ENNBTextLineAlignH			_contentAlignH;
	BOOL						_contentTextIsVisible;
	AUFuenteRender*				_fontTitle;
	AUFuenteRender*				_fontContent;
	AUFuenteRender*				_fontBtns;
	float						_btnsMarginI;
	NBMargenes					_btnsMarginsInn;
	//
	STNBColor8					_titleColor;
	//top
	struct {
		struct {
			AUEscenaContenedor*	obj;			//All items layer
			AUEscenaSprite*		glass;			//Glass
			//opts
			struct {
				AUEscenaContenedor*		layer;	//All items layer
				AUEscenaSpriteElastico*	bg;		//Bg of options
				AUEscenaTexto*			title;
				AUEscenaContenedor*		textLayer;	//All buttons layer
				AUEscenaPanelTexto*		text;
				AUEscenaObjeto*			customTop;	//custom content
				AUEscenaObjeto*			customBtm;	//custom content
				AUEscenaContenedor*		btnsLayer;	//All buttons layer
				AUArregloMutable*		btns;
			} opts;
			//close btn
			struct {
				AUEscenaContenedorLimitado* lyr;
				STIMsgboxCloseBtnLyr	icos[ENIMsgboxCloseBtnLyr_Count];
				AUEscenaSprite*			dummy[2]; //to inflate layer
			} close;
		} layer;
		float					posRelFromBottom;
		STNBPoint				posIn;			//pos in scene
		STNBPoint				posOut;			//pos out scene
		struct {
			BOOL				isAnimIn;
			float				secsDur;	//total time
			float				secsCur;	//current progress
		} anim;
	} _top;
	//
	BOOL						_onFocus;
	BOOL						_onAnim;
	BOOL						_hideWhenGlassTouched;
	BOOL						_btnOptionIsPendNotify;
	BOOL						_btnCloseIsPendNotify;
	STNBString					_btnOptionIdSel;
	IEscuchadorIMsgBox*			_lstnr;
	//
	virtual void				agregadoEnEscena();
	virtual void				quitandoDeEscena();
	//
	void						privSetCloseIco(const ENIMsgboxCloseBtnLyr iLyr, AUTextura* tex, const STNBColor8 color, const float rot, const STNBSize scale);
	void						privAddButton(const char* optionId, const ENIMsgboxBtnAction action, const char* text, AUTextura* icon, AUTextura* bgTex, const STNBColor8 bgColor, const STNBColor8 fgColor, const BOOL atTop);
	void						privOrganizar();
	void						privOrganizeTopContent();
	void						privTopAnimate(const float secs);
	void						privRemoveFromTopPriv();
};

#endif /* AUISelectBox_h */
