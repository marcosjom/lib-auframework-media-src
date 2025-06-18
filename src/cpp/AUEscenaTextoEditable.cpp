//
//  AUEscenaTextoEditable.cpp
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaTextoEditable.h"
//
#include "nb/fonts/NBTextMetricsBuilder.h"
#include "NBGestorAnimadores.h"

typedef struct STTextoCursorRender_ {
	PTRfunCmdsGL	parentCall;
	UI32			rectsSz;
	STBloqueGL		verticesGL;
	STBloqueGL		indicesGL;
} STTextoCursorRender;

AUEscenaTextoEditable::AUEscenaTextoEditable()
: AUEscenaTexto()
, _atScene(FALSE)
, _atAnimator(FALSE)
, _isEditable(FALSE)
, _animatingCursor(FALSE)
, _visibilityCursor(FALSE)
, _visibilitySecsAcum(0.0f)
, _dragginCurId(0)
, _cursorDataTextSeqId(0)
, _cursorDataSeqId(0)
, _rectsSeqId(0)
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTextoEditable")
	NBTextCursor_init(&this->_cursorData);
	NBArray_init(&this->_rectsComposing, sizeof(STNBRect), NULL);
	NBArray_init(&this->_rectsSelection, sizeof(STNBRect), NULL);
	_rectsSelIsCursor = FALSE;
	//
	this->establecerEscuchadorTouches(this, this);
}

AUEscenaTextoEditable::AUEscenaTextoEditable(AUFuenteRender* fuenteRender)
: AUEscenaTexto(fuenteRender)
, _atScene(FALSE)
, _atAnimator(FALSE)
, _isEditable(FALSE)
, _animatingCursor(FALSE)
, _visibilityCursor(FALSE)
, _visibilitySecsAcum(0.0f)
, _dragginCurId(0)
, _cursorDataTextSeqId(0)
, _cursorDataSeqId(0)
, _rectsSeqId(0)
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTextoEditable")
	NBTextCursor_init(&this->_cursorData);
	NBArray_init(&this->_rectsComposing, sizeof(STNBRect), NULL);
	NBArray_init(&this->_rectsSelection, sizeof(STNBRect), NULL);
	_rectsSelIsCursor = FALSE;
	//
	this->establecerEscuchadorTouches(this, this);
}

AUEscenaTextoEditable::AUEscenaTextoEditable(AUEscenaTextoEditable* otraInstancia)
: AUEscenaTexto(otraInstancia)
, _atScene(FALSE)
, _atAnimator(FALSE)
, _isEditable(otraInstancia->_isEditable)
, _animatingCursor(otraInstancia->_animatingCursor)
, _visibilityCursor(FALSE)
, _visibilitySecsAcum(0.0f)
, _dragginCurId(0)
, _cursorDataTextSeqId(0)
, _cursorDataSeqId(otraInstancia->_cursorDataSeqId)
, _rectsSeqId(0)
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTextoEditable")
	NBTextCursor_initWithOther(&this->_cursorData, &otraInstancia->_cursorData);
	NBArray_init(&this->_rectsComposing, sizeof(STNBRect), NULL);
	NBArray_init(&this->_rectsSelection, sizeof(STNBRect), NULL);
	_rectsSelIsCursor = FALSE;
	//Sync visual representation
	this->privSyncVisualModels();
	//
	this->establecerEscuchadorTouches(this, this);
}

AUEscenaTextoEditable::~AUEscenaTextoEditable(){
	//Remove from animator
	if(_atAnimator){
		NBGestorAnimadores::quitarAnimador(this);
		_atAnimator = FALSE;
	}
	//
	NBTextCursor_release(&this->_cursorData);
	NBArray_release(&this->_rectsComposing);
	NBArray_release(&this->_rectsSelection);
}

//Sync

void AUEscenaTextoEditable::syncDataWithOther(const AUEscenaTextoEditable* other){
	this->syncDataWithOther(other, TRUE, TRUE);
}
	
void AUEscenaTextoEditable::syncDataWithOther(const AUEscenaTextoEditable* other, const BOOL textData, const BOOL cursorData){
	if(this != other){
		//Sync text-data (parent)
		if(textData){
			AUEscenaTexto::syncDataWithOther(other);
			NBASSERT(this->_textDataSeqId == other->_textDataSeqId)
		}
		//Sync cursor-data
		if(cursorData){
			NBTextCursor_syncDataWithOther(&this->_cursorData, &other->_cursorData);
			this->_cursorDataSeqId = other->_cursorDataSeqId;
			//Reset cursor animation
			{
				_visibilityCursor	= TRUE;
				_visibilitySecsAcum	= 0.0f;
			}
		}
	}
}

//

STNBRangeI AUEscenaTextoEditable::getRngComposing() const { //in logic-char units (not byte unit)
	return NBTextCursor_getRngComposing(&this->_cursorData);
}

STNBRangeI AUEscenaTextoEditable::getRngComposingAbs() const { //in logic-char units (not byte unit)
	return NBTextCursor_getRngComposingAbs(&this->_cursorData);
}

STNBRangeI AUEscenaTextoEditable::getRngSelection() const {	//in logic-char units (not byte unit)
	return NBTextCursor_getRngSelection(&this->_cursorData);
}

STNBRangeI AUEscenaTextoEditable::getRngSelectionAbs() const {	//in logic-char units (not byte unit)
	return NBTextCursor_getRngSelectionAbs(&this->_cursorData);
}

STNBRangeI AUEscenaTextoEditable::getRngOfLineByChar(const SI32 charDefPos) const {
	return NBText_getRngOfLineByChar(&this->_textData, charDefPos);
}

SI32 AUEscenaTextoEditable::getClosestCharIdx(const float localX, const float localY) const {
	return NBTextMetrics_closestCharIdx(&_textData.metrics, localX, -localY, NULL); //y-inverted;
}

BOOL AUEscenaTextoEditable::isEditable() const {
	return _isEditable;
}

BOOL AUEscenaTextoEditable::animatingCursor() const {
	return _animatingCursor;
}

UI32 AUEscenaTextoEditable::cursorDataSeqId() const {
	return _cursorDataSeqId;
}

//

STNBRect AUEscenaTextoEditable::rectForCursorCurrent(){
	return this->rectForCursor(_cursorData.rngSel.start + _cursorData.rngSel.size);
}

//

void AUEscenaTextoEditable::setRngComposing(const STNBRangeI rng){
	this->setRngComposing(rng.start, rng.size);
}


void AUEscenaTextoEditable::setRngComposing(const SI32 start, const SI32 size){
	const STNBRangeI rng = NBTextCursor_getRngComposing(&this->_cursorData);
	if(rng.start != start || rng.size != size){
		NBTextCursor_setRngComposing(&this->_cursorData, start, size, &this->_textData.metrics);
		_cursorDataSeqId++;
		this->setModelChangedFlag();
	}
}

void AUEscenaTextoEditable::setRngSelection(const STNBRangeI rng){
	this->setRngSelection(rng.start, rng.size);
}

void AUEscenaTextoEditable::setRngSelection(const SI32 start, const SI32 size){
	const STNBRangeI rng = NBTextCursor_getRngSelection(&this->_cursorData);
	if(rng.start != start || rng.size != size){
		NBTextCursor_setRngSelection(&this->_cursorData, start, size, &this->_textData.metrics);
		_cursorDataSeqId++;
		//Reset cursor animation
		{
			_visibilityCursor	= TRUE;
			_visibilitySecsAcum	= 0.0f;
		}
		this->setModelChangedFlag();
	}
}

void AUEscenaTextoEditable::setIsEditable(const BOOL isEditable){
	if(_isEditable != isEditable){
		_isEditable = isEditable;
		if(_atScene && _isEditable && _animatingCursor){
			if(!_atAnimator){
				NBGestorAnimadores::agregarAnimador(this, this);
				_atAnimator = TRUE;
			}
		} else {
			if(_atAnimator){
				NBGestorAnimadores::quitarAnimador(this);
				_atAnimator = FALSE;
			}
		}
		//Reset cursor animation
		if(isEditable){
			_visibilityCursor	= TRUE;
			_visibilitySecsAcum	= 0.0f;
		}
		_cursorDataSeqId++;
		this->setModelChangedFlag();
	}
}

void AUEscenaTextoEditable::setAnimatingCursor(const BOOL animCursor){
	if(_animatingCursor != animCursor){
		_animatingCursor = animCursor;
		if(_atScene && _isEditable && _animatingCursor){
			if(!_atAnimator){
				NBGestorAnimadores::agregarAnimador(this, this);
				_atAnimator = TRUE;
			}
		} else {
			if(_atAnimator){
				NBGestorAnimadores::quitarAnimador(this);
				_atAnimator = FALSE;
			}
		}
		//Reset cursor animation
		if(animCursor){
			_visibilityCursor	= TRUE;
			_visibilitySecsAcum	= 0.0f;
		}
		_cursorDataSeqId++;
		this->setModelChangedFlag();
	}
}

//

STNBRangeI AUEscenaTextoEditable::getComposingText(AUCadenaMutable8* dst) const {
	STNBRangeI bRng;
	bRng.start = bRng.size = 0;
	if(dst != NULL){
		const STNBRangeI rngAbs = NBTextCursor_getRngComposingAbs(&_cursorData);
		dst->vaciar();
		bRng = NBTextMetrics_charsRangeToBytesRange(this->textMetrics(), rngAbs.start, rngAbs.size);
		if(bRng.size > 0){
			const char* txt = this->texto();
			dst->agregar(&txt[bRng.start], bRng.size);
		}
	}
	return bRng;
}

STNBRangeI AUEscenaTextoEditable::getSelectedText(AUCadenaMutable8* dst) const {
	STNBRangeI bRng;
	bRng.start = bRng.size = 0;
	if(dst != NULL){
		const STNBRangeI rngAbs = NBTextCursor_getRngSelectionAbs(&_cursorData);
		dst->vaciar();
		bRng = NBTextMetrics_charsRangeToBytesRange(this->textMetrics(), rngAbs.start, rngAbs.size);
		if(bRng.size > 0){
			const char* txt = this->texto();
			dst->agregar(&txt[bRng.start], bRng.size);
		}
	}
	return bRng;
}

STNBRangeI AUEscenaTextoEditable::getTextAtRng(const SI32 start, const SI32 size, AUCadenaMutable8* dst) const {
	NBASSERT(start >= 0 && size >= 0)
	STNBRangeI bRng, cRng;
	bRng.start	= bRng.size = 0;
	cRng.start	= start;
	cRng.size	= size;
	bRng		= NBTextMetrics_charsRangeToBytesRange(this->textMetrics(), cRng.start, cRng.size);
	if(bRng.size > 0 && dst != NULL){
		const char* txt = this->texto();
		dst->vaciar();
		dst->agregar(&txt[bRng.start], bRng.size);
	}
	return bRng;
}

STNBRangeI AUEscenaTextoEditable::getTextBeforeSelection(const UI32 logicCharsCount, AUCadenaMutable8* dst) const {
	const STNBRangeI rngAbs = NBTextCursor_getRngSelectionAbs(&_cursorData);
	STNBRangeI bRng, cRng;
	bRng.start = bRng.size = 0;
	if(rngAbs.start < logicCharsCount){
		cRng.start	= 0;
		cRng.size	= rngAbs.start;
	} else {
		cRng.start	= rngAbs.start - logicCharsCount;
		cRng.size	= logicCharsCount;
	}
	bRng = NBTextMetrics_charsRangeToBytesRange(this->textMetrics(), cRng.start, cRng.size);
	if(bRng.size > 0 && dst != NULL){
		const char* txt = this->texto();
		dst->vaciar();
		dst->agregar(&txt[bRng.start], bRng.size);
	}
	return bRng;
}

STNBRangeI AUEscenaTextoEditable::getTextAfterSelection(const UI32 logicCharsCount, AUCadenaMutable8* dst) const {
	const STNBRangeI rngAbs = NBTextCursor_getRngSelectionAbs(&_cursorData);
	STNBRangeI bRng, cRng;
	bRng.start	= bRng.size = 0;
	cRng.start	= rngAbs.start + rngAbs.size;
	cRng.size	= logicCharsCount;
	bRng		= NBTextMetrics_charsRangeToBytesRange(this->textMetrics(), cRng.start, cRng.size);
	if(bRng.size > 0 && dst != NULL){
		const char* txt = this->texto();
		dst->vaciar();
		dst->agregar(&txt[bRng.start], bRng.size);
	}
	return bRng;
}

STNBRangeI AUEscenaTextoEditable::removeSelectedTextOrPrevChar(){
	STNBRangeI bytesRng		= { 0, 0 };
	const STNBRangeI selRng	= NBTextCursor_getRngSelectionAbs(&_cursorData);
	STNBRangeI newSelRng	= selRng;
	if(selRng.size > 0){
		//Delete selected text
		bytesRng = NBText_removeCharsWithoutIntegrity(&this->_textData, selRng.start, selRng.size, NULL);
		newSelRng.size = 0;
		this->setRngSelection(newSelRng.start, newSelRng.size);
		this->setRngComposing(newSelRng.start, newSelRng.size);
		this->organizarTextoEnCol(this->_defCol, this->_defAllowMultiline);
		PRINTF_INFO("removeSelectedTextOrPrevChar(selRng.size > 0) newRng(%d, +%d).\n", newSelRng.start, newSelRng.size);
	} else if(selRng.start > 0){
		//Delete previous char
		bytesRng =  NBText_removeCharsWithoutIntegrity(&this->_textData, (selRng.start - 1), 1, NULL);
		if(bytesRng.size > 0){
			newSelRng.start = selRng.start - 1;
		}
		newSelRng.size = 0;
		this->setRngSelection(newSelRng.start, newSelRng.size);
		this->setRngComposing(newSelRng.start, newSelRng.size);
		this->organizarTextoEnCol(this->_defCol, this->_defAllowMultiline);
		PRINTF_INFO("removeSelectedTextOrPrevChar(selRng.start > 0) newRng(%d, +%d).\n", newSelRng.start, newSelRng.size);
	}
	//Return
	//NBASSERT(newSelRng.start == this->_cursorData.rngSelection.start && newSelRng.size == this->_cursorData.rngSelection.size)
	return newSelRng;
}

STNBRangeI AUEscenaTextoEditable::replaceSelectedText(const char* newText){
	const SI32 newTextLen = NBString_strLenBytes(newText);
	const STNBRangeI selRng	= NBTextCursor_getRngSelectionAbs(&_cursorData);
	//Replace
	STNBRangeI charsAddedRng = { 0, 0 };
	{
		NBText_replaceCharsWithoutIntegrity(&this->_textData, selRng.start, selRng.size, newText, newTextLen, NULL, &charsAddedRng);
		this->organizarTextoEnCol(this->_defCol, this->_defAllowMultiline);
	}
	//Update seletced range
	STNBRangeI newSelRng = { charsAddedRng.start + charsAddedRng.size, 0 };
	this->setRngSelection(newSelRng.start, newSelRng.size);
	//Return
	//NBASSERT(newSelRng.start == this->_cursorData.rngSelection.start && newSelRng.size == this->_cursorData.rngSelection.size)
	return newSelRng;
}

STNBRangeI AUEscenaTextoEditable::replaceComposingText(const char* newText){ //Reutrns new composingCharsRng
	const SI32 newTextLen = NBString_strLenBytes(newText);
	const STNBRangeI cmpRng	= NBTextCursor_getRngComposingAbs(&_cursorData);
	//Replace
	PRINTF_INFO("AUEscenaTextoEditable::replaceComposingText: cmpRng(%d, +%d)-with('%s').\n", cmpRng.start, cmpRng.size, newText);
	STNBRangeI charsAddedRng = { 0, 0 };
	{
		NBText_replaceCharsWithoutIntegrity(&this->_textData, cmpRng.start, cmpRng.size, newText, newTextLen, NULL, &charsAddedRng);
		this->organizarTextoEnCol(this->_defCol, this->_defAllowMultiline);
	}
	//Update seletced range
	STNBRangeI newCmpRng = { cmpRng.start, charsAddedRng.size };
	PRINTF_INFO("AUEscenaTextoEditable::replaceComposingText: newCmpRng(%d, +%d).\n", newCmpRng.start, newCmpRng.size);
	this->setRngComposing(newCmpRng.start, newCmpRng.size);
	//Return
	return newCmpRng;
}

STNBRangeI AUEscenaTextoEditable::replaceTextAtRng(const SI32 start, const SI32 size, const char* newText, STNBRangeI* dstAddedCharsRng){ //Reutrns the range replaced
	const UI32 newTextLen = NBString_strLenBytes(newText);
	const STNBRangeI bytesRng = NBText_replaceCharsWithoutIntegrity(&this->_textData, start, size, newText, newTextLen, NULL, dstAddedCharsRng);
	this->organizarTextoEnCol(this->_defCol, this->_defAllowMultiline);
	return bytesRng;
}

//

void AUEscenaTextoEditable::agregadoEnEscena(){
	NBASSERT(!_atScene)
	if(!_atScene){
		if(_isEditable && _animatingCursor && !_atAnimator){
			NBGestorAnimadores::agregarAnimador(this, this);
			_atAnimator = TRUE;
		}
		_atScene		= TRUE;
		//Reset cursor animation
		{
			_visibilityCursor	= TRUE;
			_visibilitySecsAcum	= 0.0f;
		}
	}
	//Call parent method
	AUEscenaTexto::agregadoEnEscena();
}

void AUEscenaTextoEditable::quitandoDeEscena(){
	NBASSERT(_atScene)
	if(_atScene){
		if(_atAnimator){
			NBGestorAnimadores::quitarAnimador(this);
			_atAnimator = FALSE;
		}
		_atScene = FALSE;
	}
	//Call parent method
	AUEscenaTexto::quitandoDeEscena();
}

void AUEscenaTextoEditable::tickAnimacion(float segsTranscurridos){
	//Cursor animation
	if(_atScene && _atAnimator && _isEditable && _animatingCursor){
		_visibilitySecsAcum += segsTranscurridos;
		if(_visibilitySecsAcum > 0.75f){
			_visibilityCursor	= !_visibilityCursor;
			_visibilitySecsAcum	= 0.0f;
			this->setModelChangedFlag();
		}
	}
}

//Draggin

void AUEscenaTextoEditable::dragginStart(const UI64 dragId, const float localX, const float localY){
	//Only one draggin will be processed
	if(_dragginCurId == 0 && dragId != 0){
		_dragginStartedChar = NBTextMetrics_closestCharIdx(&_textData.metrics, localX, -localY, &_dragginStartedLine); //y-inverted
		NBASSERT(_dragginStartedChar >= 0)
		this->setRngComposing(_dragginStartedChar, 0);
		this->setRngSelection(_dragginStartedChar, 0);
		PRINTF_INFO("First char: %u.\n", _dragginStartedChar);
		//Set touch data
		_dragginStartedPos.x = localX;
		_dragginStartedPos.y = localY;
		_dragginCurId = dragId;
	}
}

void AUEscenaTextoEditable::dragginContinue(const UI64 dragId, const float localX, const float localY){
	//Only one draggin will be processed
	if(_dragginCurId == dragId){
		const float coverRequired = 0.33f; //factor to cover chars
		UI32 iLine = _dragginStartedLine;
		const SI32 iChar = NBTextMetrics_closestCharIdx(&_textData.metrics, localX, -localY, &iLine); //y-inverted
		if(_dragginStartedChar == iChar){
			SI32 rngLen = 0;
			//Still into same char (process relative to char)
			if(_dragginStartedChar < _textData.metrics.chars.use){
				const STNBTextMetricsChar* c = NBArray_itmPtrAtIndex(&_textData.metrics.chars, STNBTextMetricsChar, _dragginStartedChar);
				const float cWidth	= (c->extendsRight + c->extendsLeft);
				if(cWidth != 0.0f){ //chars with zero-width are incomplete definitions
					const float relStart = (_dragginStartedPos.x - (c->pos.x - c->extendsLeft)) / cWidth;
					const float relCurnt = (localX - (c->pos.x - c->extendsLeft)) / cWidth;
					const float relDelta = (relCurnt - relStart);
					rngLen = (relDelta < -coverRequired || relDelta > coverRequired ? 1 : 0);
					//PRINTF_INFO("Draggin into same char: delta %d%% from %d%% to %d%%.\n", (SI32)(relDelta * 100.0f), (SI32)(relStart * 100.0f), (SI32)(relCurnt * 100.0f));
				}
			}
			this->setRngComposing(iChar, 0);
			this->setRngSelection(iChar, rngLen);
		} else {
			if(iLine < _dragginStartedLine || (iLine == _dragginStartedLine && localX <= _dragginStartedPos.x)){
				//Selection is moving back
				NBASSERT(iChar <= _dragginStartedChar)
				if(iChar <= _dragginStartedChar){
					SI32 rngStart = iChar;
					SI32 rngLen = (_dragginStartedChar - iChar);
					//Cover the start-char (at right) if is covered enough
					if(_dragginStartedChar < _textData.metrics.chars.use){
						const STNBTextMetricsChar* c = NBArray_itmPtrAtIndex(&_textData.metrics.chars, STNBTextMetricsChar, _dragginStartedChar);
						const float cWidth	= (c->extendsRight + c->extendsLeft);
						if(cWidth != 0.0f){ //chars with zero-width are incomplete definitions
							const float cRel	= (_dragginStartedPos.x - (c->pos.x - c->extendsLeft)) / cWidth;
							if(cRel > coverRequired){
								rngLen++;
							}
						}
					}
					//Conver the left char only if it is covered enough
					if(iChar > 0 && iChar < _textData.metrics.chars.use){
						const STNBTextMetricsChar* c = NBArray_itmPtrAtIndex(&_textData.metrics.chars, STNBTextMetricsChar, iChar);
						const float cWidth	= (c->extendsRight + c->extendsLeft);
						if(cWidth != 0.0f){ //chars with zero-width are incomplete definitions
							const float cRel	= (localX - (c->pos.x - c->extendsLeft)) / cWidth;
							NBASSERT(rngLen > 0)
							//PRINTF_INFO("Rel left %d%%.\n", (SI32)(cRel * 100.0f));
							if(cRel >= (1.0f - coverRequired) && rngLen > 0){
								rngStart++; //Do not cover the current char
								rngLen--;
							}
						}
					}
					this->setRngComposing(rngStart + rngLen, 0);
					this->setRngSelection(rngStart + rngLen, -rngLen);
					PRINTF_INFO("Char sel: %u (+%d) -> %u (+%d).\n", iChar, (rngStart - iChar), _dragginStartedChar, (rngLen - (_dragginStartedChar - iChar)));
				}
			} else {
				//Selection is moving forward
				NBASSERT(_dragginStartedChar <= iChar)
				if(_dragginStartedChar <= iChar){
					SI32 rngLen = (iChar - _dragginStartedChar);
					//Cover the current char if a portion is already covered
					if(iChar < _textData.metrics.chars.use){
						const STNBTextMetricsChar* c = NBArray_itmPtrAtIndex(&_textData.metrics.chars, STNBTextMetricsChar, iChar);
						const float cWidth	= (c->extendsRight + c->extendsLeft);
						if(cWidth != 0.0f){ //chars with zero-width are incomplete definitions
							const float cRel	= (localX - (c->pos.x - c->extendsLeft)) / cWidth;
							//PRINTF_INFO("Rel right %d%%.\n", (SI32)(cRel * 100.0f));
							if(cRel > coverRequired){
								rngLen++; //Cover the current char
							}
						}
					}
					this->setRngComposing(_dragginStartedChar, 0);
					this->setRngSelection(_dragginStartedChar, rngLen);
					//PRINTF_INFO("Char sel: %u -> %u (+%d).\n", _dragginStartedChar, iChar, (SI32)(rngLen - (iChar - _dragginStartedChar)));
				}
			}
		}
	}
}

void AUEscenaTextoEditable::dragginEnd(const UI64 dragId, const float localX, const float localY, const bool canceled){
	//Only one draggin will be processed
	if(_dragginCurId == dragId){
		//Disable current selection
		_dragginCurId = 0;
	}
}

//TOUCHES
void AUEscenaTextoEditable::touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto){
	const NBPunto local = this->coordenadaEscenaALocal(posTouchEscena.x, posTouchEscena.y);
	this->dragginStart((UI64)touch, local.x, local.y);
}

void AUEscenaTextoEditable::touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	const NBPunto local = this->coordenadaEscenaALocal(posActualEscena.x, posActualEscena.y);
	this->dragginContinue((UI64)touch, local.x, local.y);
}

void AUEscenaTextoEditable::touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	const NBPunto local = this->coordenadaEscenaALocal(posActualEscena.x, posActualEscena.y);
	this->dragginEnd((UI64)touch, local.x, local.y, touch->cancelado);
}

//Sync metrics with visual representations

void AUEscenaTextoEditable::privSyncVisualModels(){
	//Empty
	NBArray_empty(&this->_rectsComposing);
	NBArray_empty(&this->_rectsSelection);
	_rectsSelIsCursor = FALSE;
	//Generate rects
	{
		{
			const STNBRangeI rngCmp = NBTextCursor_getRngComposingAbs(&_cursorData);
			if(rngCmp.size > 0){
				NBTextMetrics_rectsForCharsRange(&this->_textData.metrics, rngCmp.start, rngCmp.size, &this->_rectsComposing);
			}
		}
		{
			const STNBRangeI rngSel = NBTextCursor_getRngSelectionAbs(&_cursorData);
			if(rngSel.size > 0){
				NBTextMetrics_rectsForCharsRange(&this->_textData.metrics, rngSel.start, rngSel.size, &this->_rectsSelection);
				_rectsSelIsCursor = FALSE;
			} else {
				const STNBRect curRect = NBTextMetrics_rectForCursor(&this->_textData.metrics, rngSel.start);
				NBArray_addValue(&this->_rectsSelection, curRect);
				_rectsSelIsCursor = TRUE;
			}
		}
	}
	//
	_cursorDataTextSeqId	= this->_textDataSeqId;
	_rectsSeqId				= _cursorDataSeqId;
}

//

NBCajaAABB AUEscenaTextoEditable::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTextoEditable::cajaAABBLocalCalculada")
	const NBCajaAABB r = this->boxForCurrentText(TRUE); //include cursor
	NBASSERT(r.xMin < r.xMax)	//Must include at least the cursor
	NBASSERT(r.yMin < r.yMax)	//Must include at least the cursor
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

//

PTRfunCmdsGL AUEscenaTextoEditable::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTextoEditable::actualizarModeloGL")
	//Sync model
	if(_cursorDataTextSeqId	!= this->_textDataSeqId || _rectsSeqId != _cursorDataSeqId){
		this->privSyncVisualModels();
		_cursorDataTextSeqId	= this->_textDataSeqId;
		_rectsSeqId				= _cursorDataSeqId;
		//PRINTF_INFO("Updated cursor model (%d rects in total, %d chars sel, %d chars cmp).\n", (_rectsSelection.use + _rectsComposing.use), _cursorData.rngSelection.size, _cursorData.rngComposing.size);
	}
	//Build render model
	{
		SI32 i; NBVerticeGL0* verticesGL = NULL;
		STTextoCursorRender render;
		NBMemory_setZeroSt(render, STTextoCursorRender);
		//
		const STNBRangeI rngSel		= NBTextCursor_getRngSelectionAbs(&_cursorData);
		const BOOL showCursor		= (_visibilityCursor && _atScene && _isEditable && _animatingCursor);
		const SI32 selRectsVisible	= (showCursor || rngSel.size > 0) ? _rectsSelection.use : 0;
		//
		if(selRectsVisible > 0 || _rectsComposing.use > 0){
			render.rectsSz = selRectsVisible + _rectsComposing.use;
			render.verticesGL.cantidadElementos = render.rectsSz * 4;
			render.verticesGL.primerElemento	= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_SinTextura, render.verticesGL.cantidadElementos);
			verticesGL							= &((*propsRenderizado.verticesGL0)[render.verticesGL.primerElemento]);
		}
		//---------------------------
		//-- Genera modelosGL de seleccion
		//---------------------------
		if(selRectsVisible > 0){
			UI8 r, g, b, a;
			if(_rectsSelIsCursor){
                r = 0; g = 0; b = 200; a = 255;
			} else {
				r = 0; g = 0; b = 200; a = 100;
			}
			for(i = 0; i < selRectsVisible; i++){
				const STNBRect* rect = NBArray_itmPtrAtIndex(&_rectsSelection, STNBRect, i);
				NBPunto pMin; pMin.x = rect->x; pMin.y = -rect->y - rect->height; //y-inverted
				NBPunto pMax; pMax.x = rect->x + rect->width; pMax.y = -rect->y; //y-inverted
				NBASSERT(pMin.x <= pMax.x)
				//NBASSERT(pMin.y <= pMax.y) //Todo: enable
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], _cacheObjEscena.matrizEscena, pMin.x, pMax.y);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[1], _cacheObjEscena.matrizEscena, pMax.x, pMax.y);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[2], _cacheObjEscena.matrizEscena, pMin.x, pMin.y);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[3], _cacheObjEscena.matrizEscena, pMax.x, pMin.y);
				NBCOLOR_ESTABLECER(verticesGL[0], r, g, b, a)
				NBCOLOR_ESTABLECER(verticesGL[1], r, g, b, a)
				NBCOLOR_ESTABLECER(verticesGL[2], r, g, b, a)
				NBCOLOR_ESTABLECER(verticesGL[3], r, g, b, a)
				verticesGL += 4;
			}
		}
		//---------------------------
		//-- Genera modelosGL de marcado
		//---------------------------
		if(_rectsComposing.use > 0){
			const UI8 r = 255, g = 0, b = 0, a = 100;
			for(i = 0; i < _rectsComposing.use; i++){
				const STNBRect* rect = NBArray_itmPtrAtIndex(&_rectsComposing, STNBRect, i);
				NBPunto pMin; pMin.x = rect->x; pMin.y = -rect->y - rect->height; //y-inverted
				NBPunto pMax; pMax.x = rect->x + rect->width; pMax.y = -rect->y; //y-inverted
				NBASSERT(pMin.x <= pMax.x)
				//NBASSERT(pMin.y <= pMax.y) //Todo: enable
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], _cacheObjEscena.matrizEscena, pMin.x, pMax.y);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[1], _cacheObjEscena.matrizEscena, pMax.x, pMax.y);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[2], _cacheObjEscena.matrizEscena, pMin.x, pMin.y);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[3], _cacheObjEscena.matrizEscena, pMax.x, pMin.y);
				NBCOLOR_ESTABLECER(verticesGL[0], r, g, b, a)
				NBCOLOR_ESTABLECER(verticesGL[1], r, g, b, a)
				NBCOLOR_ESTABLECER(verticesGL[2], r, g, b, a)
				NBCOLOR_ESTABLECER(verticesGL[3], r, g, b, a)
				verticesGL += 4;
			}
		}
		if(render.rectsSz > 0){
			NBASSERT(verticesGL == &((*propsRenderizado.verticesGL0)[render.verticesGL.primerElemento + (render.rectsSz * 4)]))
			render.indicesGL = NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_4(ENVerticeGlTipo_SinTextura, render.verticesGL, render.rectsSz);
		}
		//Save model to render context
		{
			const SI32 indiceDatosRenderSel		= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STTextoCursorRender));
			//Build parent
			render.parentCall					= AUEscenaTexto::actualizarModeloGL(propsRenderizado, propsIluminacion, propsHeredadas);
			STTextoCursorRender* datosRenderSel	= (STTextoCursorRender*)&(propsRenderizado.bytesDatosModelos->elemento[indiceDatosRenderSel]);
			*datosRenderSel = render;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return AUEscenaTextoEditable::enviarComandosGL;
}

void AUEscenaTextoEditable::enviarComandosGL(UI8* punteroDatosModelo){
	STTextoCursorRender* datosRender	= (STTextoCursorRender*)punteroDatosModelo;
	//Render parent first
	if(datosRender->parentCall != NULL){
		(*datosRender->parentCall)(punteroDatosModelo + sizeof(STTextoCursorRender));
	}
	//Render cursor rects
	if(datosRender->rectsSz != 0){
		NBASSERT(datosRender->indicesGL.cantidadElementos > 0)
		NBGestorGL::activarVerticesGL(ENVerticeGlTipo_SinTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaTextoCursorConSel")
		NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(datosRender->indicesGL.primerElemento, datosRender->rectsSz * NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaTextoCursorConSel")
	}
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaTextoEditable, AUEscenaTexto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaTextoEditable, "AUEscenaTextoEditable", AUEscenaTexto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaTextoEditable)






