//
//  AUEscenaTextoEditable.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaTextoEditable_H_INCLUIDO
#define AUEscenaTextoEditable_H_INCLUIDO

#include "AUEscenaTexto.h"
#include "nb/fonts/NBTextCursor.h"
#include "NBAnimador.h"

class AUEscenaTextoEditable : public AUEscenaTexto, public IEscuchadorTouchEscenaObjeto, public NBAnimador {
	public:
		AUEscenaTextoEditable();
		AUEscenaTextoEditable(AUFuenteRender* fuenteRender);
		AUEscenaTextoEditable(AUEscenaTextoEditable* otraInstancia);
		virtual ~AUEscenaTextoEditable();
		//Sync
		virtual void	syncDataWithOther(const AUEscenaTextoEditable* other);
		void			syncDataWithOther(const AUEscenaTextoEditable* other, const BOOL textData, const BOOL cursorData);
		//
		STNBRangeI		getRngComposing() const;	//in logic-char units (not byte unit)
		STNBRangeI		getRngComposingAbs() const;	//in logic-char units (not byte unit)
		STNBRangeI		getRngSelection() const;	//in logic-char units (not byte unit)
		STNBRangeI		getRngSelectionAbs() const;	//in logic-char units (not byte unit)
		STNBRangeI		getRngOfLineByChar(const SI32 charDefPos) const; //in logic-char units (not byte unit)
		SI32			getClosestCharIdx(const float localX, const float localY) const;
		BOOL			isEditable() const;
		BOOL			animatingCursor() const;
		UI32			cursorDataSeqId() const;
		//
		STNBRect		rectForCursorCurrent();
		//
		void			setRngComposing(const STNBRangeI rng); //in logic-char units (not byte unit)
		void			setRngComposing(const SI32 start, const SI32 size); //in logic-char units (not byte unit)
		void			setRngSelection(const STNBRangeI rng); //in logic-char units (not byte unit)
		void			setRngSelection(const SI32 start, const SI32 size); //in logic-char units (not byte unit)
		void			setIsEditable(const BOOL isEditable);
		void			setAnimatingCursor(const BOOL animCursor);
		//
		STNBRangeI		getComposingText(AUCadenaMutable8* dst) const;	//Returns bytes-range
		STNBRangeI		getSelectedText(AUCadenaMutable8* dst) const;	//Returns bytes-range
		STNBRangeI		getTextAtRng(const SI32 start, const SI32 size, AUCadenaMutable8* dst) const;	//Returns bytes-range
		STNBRangeI		getTextBeforeSelection(const UI32 logicCharsCount, AUCadenaMutable8* dst) const; //Returns bytes-range
		STNBRangeI		getTextAfterSelection(const UI32 logicCharsCount, AUCadenaMutable8* dst) const; //Returns bytes-range
		STNBRangeI		removeSelectedTextOrPrevChar();					//Reutrns new selectedCharsRng
		STNBRangeI		replaceSelectedText(const char* newText);		//Reutrns new selectedCharsRng
		STNBRangeI		replaceComposingText(const char* newText);		//Reutrns new composingCharsRng
		STNBRangeI		replaceTextAtRng(const SI32 start, const SI32 size, const char* newText, STNBRangeI* dstAddedCharsRng); //Reutrns the range replaced
		//
		void			tickAnimacion(float segsTranscurridos);
		//Draggin
		void			dragginStart(const UI64 dragId, const float localX, const float localY);
		void			dragginContinue(const UI64 dragId, const float localX, const float localY);
		void			dragginEnd(const UI64 dragId, const float localX, const float localY, const bool canceled);
		//TOUCHES
		void			touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto);
		void			touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
		void			touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
		//
		virtual	NBCajaAABB		cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL	actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void				enviarComandosGL(UI8* punteroDatosModelo);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		BOOL			_atScene;
		BOOL			_atAnimator;
		BOOL			_isEditable;
		BOOL			_animatingCursor;
		BOOL			_visibilityCursor;
		float			_visibilitySecsAcum;
		UI64			_dragginCurId;			//Only one draggin will be processed
		SI32			_dragginStartedChar;	//StartChar of drag
		UI32			_dragginStartedLine;	//StartLine of drag
		NBPunto			_dragginStartedPos;		//StartPos of drag
		UI32			_cursorDataTextSeqId;	//compared to parent's "_textDataSeqId"
		UI32			_cursorDataSeqId;		//compared to parent's "_textDataSeqId"
		STNBTextCursor	_cursorData;
		UI32			_rectsSeqId;
		STNBArray		_rectsComposing;		//STNBRect, y-inverted (y+ is down)
		STNBArray		_rectsSelection;		//STNBRect, y-inverted (y+ is down)
		BOOL			_rectsSelIsCursor;
		//
		void			agregadoEnEscena();
		void			quitandoDeEscena();
		//
		void			privSyncVisualModels();
	
};

#endif
