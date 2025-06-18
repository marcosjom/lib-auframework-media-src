//
//  AUEscenaPanelTexto.h
//  AUFramework
//
//  Created by Nicaragua Binary on 10/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaPanelTexto_h_INCLUIDO
#define AUEscenaPanelTexto_h_INCLUIDO

#include "NBGestorTeclas.h"
#include "AUEscenaContenedorLimitado.h"
#include "AUEscenaSprite.h"
#include "AUEscenaSpriteElastico.h"
#include "NBAnimador.h"
#include "NBMargenes.h"
#include "AUTextoFiltroLimitado.h"
#include "AUEscenaTexto.h"
#include "AUFuenteRender.h"
#include "AUTextura.h"
#include "nb/2d/NBSize.h"

class AUEscenaPanelTexto;

class IEscuchadorPanelTexto {
	public:
		virtual ~IEscuchadorPanelTexto(){
			//
		}
		//
		virtual AUEscenaObjeto* panelTextoHerederoParaTouch(AUEscenaPanelTexto* obj, const NBPunto &posInicialEscena, const NBPunto &posActualEscena) = 0;
};

class AUEscenaPanelTexto: public AUEscenaContenedorLimitado, public IEscuchadorTouchEscenaObjeto, public NBAnimador {
public:
	AUEscenaPanelTexto(AUFuenteRender* font, AUTextura* scrollBarTex, const STNBColor8 scrollBarColor, IEscuchadorPanelTexto* lstnr);
	virtual				~AUEscenaPanelTexto();
	//
	AUFuenteRender*		getFont() const;
	const char*			getText() const;
	UI32				getTextLen() const;
	UI32				getCharDefsCount() const;
	UI32				getLinesCount() const;
	STNBSize			getCurSize() const;
	//
	NBMargenes			getMargins();
	void				setMargins(const float left, const float right, const float top, const float botm);
	void				setMargins(const NBMargenes margins);
	//
	void				establecerAlineacionH(const ENNBTextLineAlignH alineacionH);
	void				establecerAlineacionV(const ENNBTextAlignV alineacionV);
	void				establecerAlineaciones(const ENNBTextLineAlignH alineacionH, const ENNBTextAlignV alineacionV);
	//
	void				establecerTexto(const char* txt, const float anchoMax = 0.0f, const float altoMax = 0.0f);
	void				establecerTexto(const STNBText* texto, const float anchoMax = 0.0f, const float altoMax = 0.0f);
	void				organizaContenido(const float anchoMax = 0.0f, const float altoMax = 0.0f);
	//
	void				tickAnimacion(float segsTranscurridos);
	//
	void				touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto);
	void				touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
	void				touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
	void				touchScrollApply(AUEscenaObjeto* objeto, const STNBPoint posScene, const STNBSize size, const BOOL animate);
	//
	AUOBJMETODOS_CLASESID_DEFINICION
	AUOBJMETODOS_CLASESNOMBRES_DEFINICION
	AUOBJMETODOS_CLONAR_DEFINICION
protected:
	AUFuenteRender*		_font;
	STNBSize			_lastSize;
	NBMargenes			_margins;	//internal
	float				_lastWidth;
	IEscuchadorPanelTexto* _lstnr;
	struct {
		AUTextoFiltroLimitado*		txtVisualFilter;	//To limit visible area
		AUEscenaTexto*				txt;
		//
		STGTouch*					touchFirst;			//first touch
		STNBPoint					touchItmsStartPos;	//start pos of itms at first touch
		float						touchDeltaYLast;	//deltaY from last tick (to add to velocity)
		float						touchDeltaYAccum;
		float						touchDeltaYAccumSecs;
		BOOL						touchInherEval;
		struct {
			//Vertical
			struct {
				float					marginTop;			//Scroll bar empty space
				float					marginBtm;			//Scroll bar empty space
				float					objLimitMax;		//Reference for alignmet top
				float					objLimitMin;		//Reference for alignmet btm
				float					objSize;			//Object size
				float					visibleSize;		//Length for content
				float					curSpeed;			//cur scroll speed
				struct {
					UI8					maxAlpha8;			//Alpha
					float				secsForAppear;		//secs required for start showing
					float				secsFullVisible;	//visible
					float				secsForDisapr;		//secs required for start showing
					float				secsAccum;			//visible
					AUEscenaSpriteElastico*	obj;			//Object
				} bar;
			} v;
		} scroll;
	} _content;
	//
	void				privOrganizarContenido(const float ancho, const float altoMax);
	void				privScrollTopContent(const float yScroll, const BOOL showScrollbar);
	void				privScrollTopContentToPos(const float yPos, const BOOL showScrollbar);
};

#endif
