//
//  AUIDateBox.h
//  lib-auframework-media
//
//  Created by Marcos Ortega on 9/3/18.
//

#ifndef AUIDateBox_h
#define AUIDateBox_h

#include "nb/core/NBDate.h"
#include "NBMargenes.h"
#include "NBAnimador.h"
#include "AUEscenaContenedor.h"
#include "AUEscenaContenedorLimitado.h"
#include "AUEscenaFigura.h"
#include "AUEscenaTexto.h"
#include "AUEscenaSpriteElastico.h"

typedef enum ENDateBoxComp_ {
	ENDateBoxComp_Day = 0,
	ENDateBoxComp_Month,
	ENDateBoxComp_Year,
	ENDateBoxComp_Count
} ENDateBoxComp;

class AUIDateBox;

typedef struct STIDateBoxItem_ {
	STNBString					text;
	SI32						value;
	struct {
		float					yTop;		//Natural position in itms layer
		AUEscenaContenedorLimitado* layer;	//layer limiting the area
		AUEscenaSprite*			bg;			//solid bg
		AUEscenaTexto*			text;		//text
		AUEscenaFigura*			line;		//line at the bottom (separation)
		UI8						color;		//color by closest at selection
	} scn;
} STIDateBoxItem;

class IEscuchadorIDateBox {
public:
	virtual ~IEscuchadorIDateBox(){
		//
	}
	//
	virtual void dateboxFocusObtained(AUIDateBox* obj) = 0;
	virtual void dateboxFocusLost(AUIDateBox* obj) = 0;
	virtual bool dateboxMantainFocusInsteadTouchConsumer(AUIDateBox* obj, AUEscenaObjeto* touchConsumer) = 0;
	virtual void dateboxDateSelected(AUIDateBox* obj, const STNBDate date) = 0;
};

class AUIDateBox : public AUEscenaContenedor, public IEscuchadorTouchEscenaObjeto, public IEscuchadorCambioPuertoVision, public NBAnimador {
public:
	AUIDateBox(AUFuenteRender* font, AUFuenteRender* fontSels, AUTextura* selFrameTex, AUTextura* icoDropMore, AUTextura* icoRightSelect, const float boxWidth, const float marginI, const UI32 firstYear, const UI16 ammYears, const BOOL includeEmptyOpts);
	AUIDateBox(AUFuenteRender* font, AUFuenteRender* fontSels, AUTextura* selFrameTex, AUTextura* icoDropMore, AUTextura* icoRightSelect, const float boxWidth, const float marginI, const NBMargenes margins, const UI32 firstYear, const UI16 ammYears, const BOOL includeEmptyOpts);
	AUIDateBox(AUFuenteRender* font, AUFuenteRender* fontSels, AUTextura* selFrameTex, AUTextura* icoDropMore, AUTextura* icoRightSelect, const float boxWidth, const float marginI, const float mLeft, const float mRight, const float mTop, const float mBottom, const UI32 firstYear, const UI16 ammYears, const BOOL includeEmptyOpts);
	virtual ~AUIDateBox();
	//
	IEscuchadorIDateBox* getDateBoxListener();
	void				setDateBoxListener(IEscuchadorIDateBox* lstnr);
	//
	STNBDate			getDateSel() const;
	void				setDateSel(const STNBDate date);
	//
	//Selection props
	NBMargenes			getMargins() const;
	float				getBoxWidth() const;
	AUTextura*			getBoxBgTexture() const;
	void				setMargins(const NBMargenes margins);
	void				setMargins(const float mLeft, const float mRight, const float mTop, const float mBottom);
	void				setBoxWidth(const float width);
	void				setBoxBgTexture(AUTextura* textura);
	//Value
	void				establecerTextoColor(const NBColor8 color);
	void				establecerTextoColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a);
	void				establecerTextoAyuda(const char* valor);
	void				establecerTextoAyudaColor(const NBColor8 color);
	void				establecerTextoAyudaColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a);
	void				setSelFrameColor(const NBColor8 color);
	void				setSelFrameColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a);
	//List
	AUTextura*			getOptionsBgTexture() const;			//options bg textye
	NBMargenes			getOptionsMarginIn() const;				//options inner margin
	NBMargenes			getOptionsMarginOut() const;			//options outer margin
	void				setOptionsBgTexture(AUTextura* tex);	//options bg textye
	void				setOptionsMarginsIn(const NBMargenes margins); //options inner margin
	void				setOptionsMarginsIn(const float mLeft, const float mRight, const float mTop, const float mBottom); //options inner margin
	void				setOptionsMarginsOut(const NBMargenes margins); //options outer margin
	void				setOptionsMarginsOut(const float mLeft, const float mRight, const float mTop, const float mBottom); //options outer margin
	//
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
	float						_boxWidth;
	AUFuenteRender*				_font;				//selection
	AUFuenteRender*				_fontSels;			//list-options
	//
	STNBDate					_dateSel;
	STNBColor8					_textColor;
	//
	AUEscenaSprite*				_optsIcoRight;
	AUTextura*					_optsBgTex;
	NBMargenes					_optsMarginIn;		//options inner margin
	NBMargenes					_optsMarginOut;		//options outer margin
	//
	struct {
		STNBArray				array;			//STIDateBoxItem
		float					width;			//Per itm
		SI32					idxSeletected;	//Currently selected
		struct {
			float				yStart;
			float				yEnd;
			float				secsDur;	//total time
			float				secsCur;	//current progress
		} anim;
	} _itms[ENDateBoxComp_Count];
	struct {
		struct {
			AUEscenaContenedor*	obj;			//All items layer
			AUEscenaSprite*		glass;			//Glass
			struct {
				AUEscenaContenedor*		layer;		//All items layer
				AUEscenaSpriteElastico*	bg;			//Bg of options
				AUEscenaSpriteElastico*	selFrame;	//Sel of options
				struct {
					AUEscenaContenedorLimitado*	layer;			//All items bgs (touchables)
					AUEscenaContenedor*			valuesLayer;	//All items values (text and icons)
					AUEscenaContenedor*			linesLayer;		//All items lines
				} itms[ENDateBoxComp_Count];
				STNBAABox					boxOuter;		//Outer box (nothing outside shoulb be visible)
				STNBAABox					boxInner;		//Inner box (where content can be focus)
				STNBAABox					boxSelect;		//Selection box (where select content is)
			} opts;
		} layer;
		STNBPoint				posIn;			//pos in scene
		STNBPoint				posOut;			//pos out scene
		struct {
			BOOL				isAnimIn;
			float				secsDur;	//total time
			float				secsCur;	//current progress
		} anim;
	} _top;
	//
	struct {
		STGTouch*				first;				//first touch
		STNBPoint				itmsStartPos;		//start pos of itms at first touch
		float					deltaYLast;			//deltaY from last tick (to add to velocity)
		float					deltaYAccum;
		float					deltaYAccumSecs;
		float					scrollSpeedY;		//cur scroll speed
		BOOL					scrollDirtyMoved;	//Last time moved (helps to detect autoscroll adjustment)
	} _touch[ENDateBoxComp_Count];
	//
	BOOL						_onFocus;
	AUEscenaSpriteElastico*		_bg;
	AUEscenaTexto*				_visual;
	AUEscenaSprite*				_visualIcoMore;
	AUEscenaTexto*				_visualHelp;
	IEscuchadorIDateBox*		_lstnr;
	//
	virtual void				agregadoEnEscena();
	virtual void				quitandoDeEscena();
	//List
	void						privListsInit(const UI32 firstYear, const UI16 ammYears, const BOOL includeEmptyOpts);
	void						privListsEmpty();
	BOOL						privListAddItm(const ENDateBoxComp iComp, const char* text, const SI32 value);
	void						privListsCalculateWidths(const float totalWidth);
	SI32						privListItmIdxByValue(const ENDateBoxComp iComp, const SI32 value);
	SI32						privListItmIdxByCurPosY(const ENDateBoxComp iComp);
	SI32						privListItmValueByCurPosY(const ENDateBoxComp iComp);
	void						privListSelectDate(const STNBDate value, const BOOL animate);
	//
	void						privOrganizar();
	void						privOrganizeItmsList(const ENDateBoxComp iComp);
	void						privOrganizeTopContent();
	void						privScrollTopContent(const ENDateBoxComp iComp, const float yScroll);
	void						privScrollTopContentToItmIdx(const ENDateBoxComp iComp, const SI32 itmIdx, const BOOL animate);
	void						privScrollTopContentToPos(const ENDateBoxComp iComp, const float yPos, const BOOL setAsDirtyMoved);
	void						privTopAnimate(const float secs);
	void						privRemoveFromTopPriv();
};

#endif /* AUIDateBox_h */
