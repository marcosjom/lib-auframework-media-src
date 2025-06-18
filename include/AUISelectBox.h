//
//  AUISelectBox.h
//  lib-auframework-media
//
//  Created by Marcos Ortega on 9/3/18.
//

#ifndef AUISelectBox_h
#define AUISelectBox_h

#include "nb/core/NBDate.h"
#include "NBMargenes.h"
#include "NBAnimador.h"
#include "AUEscenaContenedor.h"
#include "AUEscenaContenedorLimitado.h"
#include "AUEscenaFigura.h"
#include "AUEscenaTexto.h"
#include "AUEscenaSpriteElastico.h"

class AUISelectBox;

typedef struct STISelectBoxItem_ {
	STNBString					text;
	AUTextura*					icoLeft;
	AUTextura*					icoRight;
	struct {
		float					yTop;		//Natural position in itms layer
		AUEscenaContenedorLimitado* layer;	//layer limiting the area
		AUEscenaSprite*			bg;			//solid bg
		AUFuenteRender*			fontCustom;	//Custom font (optional)
		AUEscenaTexto*			text;		//text
		AUEscenaSprite*			icoLeft;	//icon at the left (nullable)
		AUEscenaSprite*			icoRight;	//icon at the right (nullable)
		AUEscenaFigura*			line;		//line at the bottom (separation)
	} scn;
} STISelectBoxItem;

class IEscuchadorISelectBox {
public:
	virtual ~IEscuchadorISelectBox(){
		//
	}
	//
	virtual BOOL selectboxFocusObtaining(AUISelectBox* obj) = 0;
	virtual void selectboxFocusObtained(AUISelectBox* obj) = 0;
	virtual void selectboxFocusLost(AUISelectBox* obj) = 0;
	virtual bool selectboxMantainFocusInsteadTouchConsumer(AUISelectBox* obj, AUEscenaObjeto* touchConsumer) = 0;
	virtual void selectboxItemSelected(AUISelectBox* obj, const SI32 idx, const char* value) = 0;
};

class AUISelectBox : public AUEscenaContenedor, public IEscuchadorTouchEscenaObjeto, public IEscuchadorCambioPuertoVision, public NBAnimador {
public:
	AUISelectBox(AUFuenteRender* font, AUFuenteRender* fontSels, AUTextura* icoDropMore, const float boxWidth, const float marginI);
	AUISelectBox(AUFuenteRender* font, AUFuenteRender* fontSels, AUTextura* icoDropMore, const float boxWidth, const float marginI, const NBMargenes margins);
	AUISelectBox(AUFuenteRender* font, AUFuenteRender* fontSels, AUTextura* icoDropMore, const float boxWidth, const float marginI, const float mLeft, const float mRight, const float mTop, const float mBottom);
	virtual ~AUISelectBox();
	//
	IEscuchadorISelectBox* getTextBoxListener();
	void				setTextBoxListener(IEscuchadorISelectBox* lstnr);
	//Selection props
	NBMargenes			getMargins() const;
	float				getBoxWidth() const;
	AUTextura*			getBoxBgTexture() const;
	void				setMargins(const NBMargenes margins);
	void				setMargins(const float mLeft, const float mRight, const float mTop, const float mBottom);
	void				setBoxWidth(const float width);
	void				setBoxBgTexture(AUTextura* textura);
	//Enabled
	BOOL				isEnabled() const;
	void				setEnabled(const BOOL enabled);
	//Value
	void				establecerTextoColor(const NBColor8 color);
	void				establecerTextoColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a);
	void				establecerTextoAyuda(const char* valor);
	void				establecerTextoAyudaColor(const NBColor8 color);
	void				establecerTextoAyudaColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a);
	//List
	AUTextura*			getOptionsBgTexture() const;			//options bg textye
	NBMargenes			getOptionsMarginIn() const;				//options inner margin
	NBMargenes			getOptionsMarginOut() const;			//options outer margin
	void				setOptionsBgTexture(AUTextura* tex);	//options bg textye
	void				setOptionsMarginsIn(const NBMargenes margins); //options inner margin
	void				setOptionsMarginsIn(const float mLeft, const float mRight, const float mTop, const float mBottom); //options inner margin
	void				setOptionsMarginsOut(const NBMargenes margins); //options outer margin
	void				setOptionsMarginsOut(const float mLeft, const float mRight, const float mTop, const float mBottom); //options outer margin
	//List
	void				empty();
	UI32				getItemsCount() const;
	BOOL				addItm(const char* text, AUTextura* icoLeft, AUTextura* icoRight);
	BOOL				addItm(const char* text, AUTextura* icoLeft, AUTextura* icoRight, AUFuenteRender* font);
	SI32				selectedIdx() const;
	SI32				setSelectedIdx(const SI32 idx);
	//
	BOOL				atFocus();
	void				focusObtain();
	void				focusRelease();
	//
	void				setTextColor(const NBColor8 color);
	void				setTextColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a);
	float				altoParaLineasVisibles(const float lineasVisibles) const;
	static float		altoParaLineasVisibles(const float lineasVisibles, AUFuenteRender* fuenteRender);
	//ViewPort
	void				puertoDeVisionModificado(const SI32 iEscena, const STNBViewPortSize before, const STNBViewPortSize after);
	//
	void				tickAnimacion(float segsTranscurridos);
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
	NBMargenes					_margins;
	BOOL						_isEnabled;
	BOOL						_onFocus;
	float						_boxWidth;
	AUFuenteRender*				_font;				//selection
	AUFuenteRender*				_fontSels;			//list-options
	//
	STNBColor8					_textColor;
	//
	AUTextura*					_optsBgTex;
	NBMargenes					_optsMarginIn;	//options inner margin
	NBMargenes					_optsMarginOut;	//options outer margin
	//
	STNBArray					_itms;				//STISelectBoxItem
	SI32						_itmOrganizedIdx;	//For incremental-organization (to avoid ordering all the list when adding items)
	float						_itmOrganizedTopY;	//For incremental-organization (to avoid ordering all the list when adding items)
	float						_itmWidth;			//Per itm
	SI32						_itmIdxSeletected;	//Currently selected
	struct {
		struct {
			AUEscenaContenedor*	obj;			//All items layer
			AUEscenaSprite*		glass;			//Glass
			struct {
				AUEscenaContenedor*		layer;	//All items layer
				AUEscenaSpriteElastico*	bg;		//Bg of options
				struct {
					AUEscenaContenedorLimitado*	layer;			//All items bgs (touchables)
					AUEscenaContenedor*			valuesLayer;	//All items values (text and icons)
					AUEscenaContenedor*			linesLayer;		//All items lines
					STNBAABox					boxOuter;		//Outer box (nothing outside shoulb be visible)
					STNBAABox					boxInner;		//Inner box (where content can be focus)
				} itms;
			} opts;
		} layer;
		STNBPoint				posIn;			//pos in scene
		STNBPoint				posOut;			//pos out scene
		STGTouch*				touchFirst;		//first touch
		STNBPoint				touchItmsStartPos;	//start pos of itms at first touch
		float					touchDeltaYLast;	//deltaY from last tick (to add to velocity)
		float					touchDeltaYAccum;
		float					touchDeltaYAccumSecs;
		float					scrollSpeedY;		//cur scroll speed
		struct {
			BOOL				isAnimIn;
			float				secsDur;	//total time
			float				secsCur;	//current progress
		} anim;
	} _top;
	//
	AUEscenaSpriteElastico*		_bg;
	AUEscenaTexto*				_visual;
	AUEscenaSprite*				_visualIcoLeft;
	AUEscenaSprite*				_visualIcoRight;
	AUEscenaSprite*				_visualIcoMore;
	AUEscenaTexto*				_visualHelp;
	IEscuchadorISelectBox*		_lstnr;
	//
	virtual void				agregadoEnEscena();
	virtual void				quitandoDeEscena();
	//
	void						privOrganizar();
	void						privOrganizeItmsList(const SI32 startIdx, const float itmWidth);
	void						privOrganizeTopContent();
	void						privScrollTopContent(const float yScroll);
	void						privScrollTopContentToItmIdx(const SI32 itmIdx);
	void						privScrollTopContentToPos(const float yPos);
	void						privTopAnimate(const float secs);
	void						privStartAddToTop();
	void						privRemoveFromTopPriv();
};

#endif /* AUISelectBox_h */
