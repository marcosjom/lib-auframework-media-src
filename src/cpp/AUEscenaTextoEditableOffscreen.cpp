//
//  AUEscenaTextoEditableOffscreen.cpp
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaTextoEditableOffscreen.h"
//
#include "nb/fonts/NBTextMetricsBuilder.h"
#include "NBGestorAnimadores.h"

AUEscenaTextoEditableOffscreen::AUEscenaTextoEditableOffscreen()
: AUEscenaTextoEditable()
//Behavior
, _editorType(ENTextEditorType_Literal)
, _isMultiline(false)
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTextoEditableOffscreen")
	//Visual
	NBMATRIZ_ESTABLECER_IDENTIDAD(_visualSceneMatrix)
	NBMemory_setZero(_visualScenePort);
	NBCAJAAABB_INICIALIZAR(_visualSceneBox);
}

AUEscenaTextoEditableOffscreen::AUEscenaTextoEditableOffscreen(AUFuenteRender* fuenteRender, const ENTextEditorType editorType, const BOOL allowMultiline)
: AUEscenaTextoEditable(fuenteRender)
//Behavior
, _editorType(editorType)
, _isMultiline(allowMultiline){
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTextoEditableOffscreen")
	//Visual
	NBMATRIZ_ESTABLECER_IDENTIDAD(_visualSceneMatrix)
	NBMemory_setZero(_visualScenePort);
	NBCAJAAABB_INICIALIZAR(_visualSceneBox);
}

AUEscenaTextoEditableOffscreen::AUEscenaTextoEditableOffscreen(AUEscenaTextoEditable* other)
: AUEscenaTextoEditable(other)
//Behavior
, _editorType(ENTextEditorType_Literal)
, _isMultiline(false)
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTextoEditableOffscreen")
	//Visual
	NBMATRIZ_ESTABLECER_IDENTIDAD(_visualSceneMatrix)
	NBMemory_setZero(_visualScenePort);
	NBCAJAAABB_INICIALIZAR(_visualSceneBox);
}


AUEscenaTextoEditableOffscreen::AUEscenaTextoEditableOffscreen(AUEscenaTextoEditableOffscreen* other)
: AUEscenaTextoEditable(other)
//Behavior
, _editorType(other->_editorType)
, _isMultiline(false)
//Visual
, _visualSceneMatrix(other->_visualSceneMatrix)
, _visualScenePort(other->_visualScenePort)
, _visualSceneBox(other->_visualSceneBox)
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTextoEditableOffscreen")
}

AUEscenaTextoEditableOffscreen::~AUEscenaTextoEditableOffscreen(){
	//
}

//

ENTextEditorType AUEscenaTextoEditableOffscreen::editorType() const {
	return _editorType;
}

bool AUEscenaTextoEditableOffscreen::isMultiline() const {
	return _isMultiline;
}

void AUEscenaTextoEditableOffscreen::setEditorType(const ENTextEditorType editorType){
	_editorType = editorType;
}

void AUEscenaTextoEditableOffscreen::setMultiline(const bool isMultiline){
	_isMultiline = isMultiline;
}

void AUEscenaTextoEditableOffscreen::setBehavior(const ENTextEditorType editorType, const bool isMultiline){
	_editorType		= editorType;
	_isMultiline	= isMultiline;
}

//

NBMatriz AUEscenaTextoEditableOffscreen::sceneMatrix() const {
	return _visualSceneMatrix;
}

NBRectangulo AUEscenaTextoEditableOffscreen::scenePort() const {
	return _visualScenePort;
}

NBCajaAABB AUEscenaTextoEditableOffscreen::sceneBox() const {
	return _visualSceneBox;
}

void AUEscenaTextoEditableOffscreen::setSceneProps(const NBMatriz matrix, const NBRectangulo port, const NBCajaAABB box){
	_visualSceneMatrix	= matrix;
	_visualScenePort	= port;
	_visualSceneBox		= box;
}

//

NBPunto AUEscenaTextoEditableOffscreen::privCoordSceneToPort(const float x, const float y){
	NBTamano unitsGLByPix;
	unitsGLByPix.ancho 		= ((_visualSceneBox.xMax - _visualSceneBox.xMin) / _visualScenePort.ancho);
	unitsGLByPix.alto 		= ((_visualSceneBox.yMax - _visualSceneBox.yMin) / _visualScenePort.alto);
	//transformar de coordenadas de entrada a coordenadas en escena y GUI
	NBPunto r;
	r.x = ((x - _visualSceneBox.xMin) / unitsGLByPix.ancho);
	r.y = (((_visualSceneBox.yMax - _visualSceneBox.yMin) - (y - _visualSceneBox.yMin)) / unitsGLByPix.alto);
	return r;
}

NBPunto AUEscenaTextoEditableOffscreen::privCoordPortToScene(const float x, const float y){
    NBTamano unitsGLByPix;
    unitsGLByPix.ancho         = ((_visualSceneBox.xMax - _visualSceneBox.xMin) / _visualScenePort.ancho);
    unitsGLByPix.alto         = ((_visualSceneBox.yMax - _visualSceneBox.yMin) / _visualScenePort.alto);
    //transformar de coordenadas de entrada a coordenadas en escena y GUI
    NBPunto r;
    r.x = _visualSceneBox.xMin + (x * unitsGLByPix.ancho);
    r.y = _visualSceneBox.yMin + (_visualSceneBox.yMax - _visualSceneBox.yMin) - (y * unitsGLByPix.alto);
    return r;
}

//

//TECLADO

ENTextEditorType AUEscenaTextoEditableOffscreen::entradaEditorType(BOOL* dstIsMultiline){
	if(dstIsMultiline != NULL){
		*dstIsMultiline = this->_isMultiline;
	}
	return this->_editorType;
}

bool AUEscenaTextoEditableOffscreen::entradaTieneTexto(){
	//PRINTF_INFO("TextInput, entradaTieneTexto\n");
	return (this->_textData.text.length > 0);
}

bool AUEscenaTextoEditableOffscreen::entradaPermiteMultilinea(){
	//PRINTF_INFO("TextInput, entradaPermiteMultilinea\n");
	return _isMultiline;
}

void AUEscenaTextoEditableOffscreen::entradaBackspace(const bool autoSelWord){
	//PRINTF_INFO("TextInput, entradaBackspace\n");
	/*const STNBRangeI newSelRng	= */ this->removeSelectedTextOrPrevChar();
	//ToDo: enable
	/*if(!autoSelWord){
		_cursorData.setRngCharsSelection(newSelRng.inicio, newSelRng.tamano, _textData.textMetrics());
	} else {
		this->privSetMarkedRangeAsWordAtPosition(newSelRng.inicio);
	}*/
	//ToDo: remove (old code)
	/*UI32 textoActualTam				= 0;
	 const char* textoActual			= _textData.texto(&textoActualTam);
	 if(textoActualTam > 0){
	 const NBRangoI rangoSel		= _cursorData.rngCharsSelection();
	 UI32 byteIniCharQuitar		= textoActualTam;
	 UI32 byteSigFinCharQuitar	= textoActualTam;
	 SI32 desplazaSeleccion		= 0;
	 if(rangoSel.tamano == 0){
	 if(rangoSel.inicio > 0){
	 //Borrar un caracter
	 byteIniCharQuitar = _charsPos.elem(rangoSel.inicio - 1);
	 if(rangoSel.inicio < _charsPos.conteo){
	 byteSigFinCharQuitar = _charsPos.elem(rangoSel.inicio);
	 }
	 desplazaSeleccion = -1;
	 }
	 } else {
	 //Borrar la seleccion
	 if(rangoSel.inicio < _charsPos.conteo){
	 byteIniCharQuitar = _charsPos.elem(rangoSel.inicio);
	 }
	 if((rangoSel.inicio + rangoSel.tamano) < _charsPos.conteo){
	 byteSigFinCharQuitar = _charsPos.elem(rangoSel.inicio + rangoSel.tamano);
	 }
	 }
	 //Rebuild string
	 //TODO: optimize this by only removing the necesary portion of the string.
	 {
	 AUCadenaMutable8* strTmp	= new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	 strTmp->agregar(textoActual, byteIniCharQuitar);
	 strTmp->agregar(&textoActual[byteSigFinCharQuitar]);
	 //
	 this->privSetString(strTmp->str());
	 //
	 const SI32 nuevoPosCursor = (SI32)rangoSel.inicio + desplazaSeleccion;
	 _rangeCharsSelected.inicio = nuevoPosCursor;
	 _rangeCharsSelected.tamano = 0;
	 //Note: do not force any behavoir than 'setNewComposingAs' value.
	 //Forcing 'ENTextRangeSet_None' when editor was 'ENTextEditorType_Compose' resulted
	 //in a writing error on Samsung Android devices 6+ with Samsung defaults keyboard.
	 if(!autoSelWord){
	 _rangeCharsComposing.inicio = nuevoPosCursor;
	 _rangeCharsComposing.tamano = 0;
	 } else {
	 this->privSetMarkedRangeAsWordAtPosition(nuevoPosCursor);
	 }
	 NBASSERT(_rangeCharsSelected.inicio <= _charsPos.conteo)
	 NBASSERT((_rangeCharsSelected.inicio + _rangeCharsSelected.tamano) <= _charsPos.conteo)
	 NBASSERT(_rangeCharsComposing.inicio <= _charsPos.conteo)
	 NBASSERT((_rangeCharsComposing.inicio + _rangeCharsComposing.tamano) <= _charsPos.conteo)
	 strTmp->liberar(NB_RETENEDOR_THIS);
	 }
	 }*/
}

void AUEscenaTextoEditableOffscreen::entradaIntroducirTexto(const char* textoIntroducir, const bool autoSelWord){
	//PRINTF_INFO("TextInput, entradaIntroducirTexto('%s')\n", textoIntroducir);
	const STNBRangeI bytesRng	= this->replaceSelectedText(textoIntroducir); NBASSERT(bytesRng.size == 0)
	this->setRngComposing(bytesRng.start, bytesRng.size);
	/*
	 NBASSERT((textoIntroducir[0] != '\r' && textoIntroducir[0] != '\n') || _isMultiline)
	 //Reemplazar texto en seleccion
	 const NBRangoI rangoSel				= _rangeCharsSelected;
	 const SI32 conteoCharsDefsAntes		= (_charsPos.conteo - 1);
	 this->privTextAtRangeReplace(rangoSel.inicio, rangoSel.tamano, textoIntroducir);
	 //Actualizar rango de seleccion y marcado
	 const SI32 conteoCharsDefsAhora		= (_charsPos.conteo - 1);
	 const SI32 conteoCharDefsQuitados	= rangoSel.tamano;
	 const SI32 conteoCharsDefsDelta		= conteoCharsDefsAhora + conteoCharDefsQuitados - conteoCharsDefsAntes;
	 NBASSERT(conteoCharsDefsDelta >= 0)
	 _rangeCharsSelected.inicio = rangoSel.inicio + conteoCharsDefsDelta;
	 _rangeCharsSelected.tamano = 0;
	 if(!autoSelWord){
	 _rangeCharsComposing.inicio = (rangoSel.inicio + conteoCharsDefsDelta);
	 _rangeCharsComposing.tamano = 0;
	 } else {
	 this->privSetMarkedRangeAsWordAtPosition(rangoSel.inicio + conteoCharsDefsDelta);
	 }
	 NBASSERT(_rangeCharsSelected.inicio <= _charsPos.conteo)
	 NBASSERT((_rangeCharsSelected.inicio + _rangeCharsSelected.tamano) <= _charsPos.conteo)
	 NBASSERT(_rangeCharsComposing.inicio <= _charsPos.conteo)
	 NBASSERT((_rangeCharsComposing.inicio + _rangeCharsComposing.tamano) <= _charsPos.conteo)
	 */
}

//TECLADO - ENTRADA

NBRangoI AUEscenaTextoEditableOffscreen::entradaRangoSeleccion() {
	//PRINTF_INFO("TextInput, entradaRangoSeleccion\n");
	const STNBRangeI rng = this->getRngSelection();
	NBRangoI r;
	r.inicio = rng.start;
	r.tamano = rng.size;
	return r;
}

NBRangoI AUEscenaTextoEditableOffscreen::entradaRangoSeleccionAbs() {
	//PRINTF_INFO("TextInput, entradaRangoSeleccion\n");
	const STNBRangeI rng = this->getRngSelectionAbs();
	NBRangoI r;
	r.inicio = rng.start;
	r.tamano = rng.size;
	return r;
}

void AUEscenaTextoEditableOffscreen::entradaRangoSeleccionEstablecer(const SI32 primerCharDef, const SI32 conteoCharDefs){
	//PRINTF_INFO("TextInput, entradaRangoSeleccionEstablecer(%d, +%d)\n", primerCharDef, conteoCharDefs);
	this->setRngSelection(primerCharDef, conteoCharDefs);
	/*_rangeCharsSelected.inicio = primerCharDef;
	 _rangeCharsSelected.tamano = conteoCharDefs;
	 _rangeCharsComposing.inicio = (primerCharDef + conteoCharDefs);
	 _rangeCharsComposing.tamano = 0;
	 NBASSERT(_rangeCharsSelected.inicio <= _charsPos.conteo)
	 NBASSERT((_rangeCharsSelected.inicio + _rangeCharsSelected.tamano) <= _charsPos.conteo)
	 NBASSERT(_rangeCharsComposing.inicio <= _charsPos.conteo)
	 NBASSERT((_rangeCharsComposing.inicio + _rangeCharsComposing.tamano) <= _charsPos.conteo)*/
}

NBRangoI AUEscenaTextoEditableOffscreen::entradaRangoMarcado() {
	//PRINTF_INFO("TextInput, entradaRangoMarcado\n");
	const STNBRangeI rng = this->getRngComposing();
	NBRangoI r;
	r.inicio = rng.start;
	r.tamano = rng.size;
	return r;
}

NBRangoI AUEscenaTextoEditableOffscreen::entradaRangoMarcadoAbs() {
	//PRINTF_INFO("TextInput, entradaRangoMarcado\n");
	const STNBRangeI rng = this->getRngComposingAbs();
	NBRangoI r;
	r.inicio = rng.start;
	r.tamano = rng.size;
	return r;
}

void AUEscenaTextoEditableOffscreen::entradaRangoMarcadoEstablecer(const SI32 primerCharDef, const SI32 conteoCharDefs){
	//PRINTF_INFO("TextInput, entradaRangoMarcadoEstablecer(%d, +%d)\n", primerCharDef, conteoCharDefs);
	this->setRngComposing(primerCharDef, conteoCharDefs);
	/*_rangeCharsComposing.inicio = primerCharDef;
	 _rangeCharsComposing.tamano = conteoCharDefs;
	 NBASSERT(_rangeCharsComposing.inicio <= _charsPos.conteo)
	 NBASSERT((_rangeCharsComposing.inicio + _rangeCharsComposing.tamano) <= _charsPos.conteo)*/
	//PRINTF_INFO("Rango marcado establecido a (%d, +%d).\n", primerCharDef, conteoCharDefs);
}

void AUEscenaTextoEditableOffscreen::entradaRangoDesmarcar(){
	//PRINTF_INFO("TextInput, entradaRangoDesmarcar\n");
	const STNBRangeI rng = this->getRngComposingAbs();
	if(rng.size > 0){
		this->setRngComposing(rng.start + rng.size, 0);
	}
	/*const NBRangoI rangoMrcAntes		= _rangeCharsComposing;
	 _rangeCharsComposing.inicio = (rangoMrcAntes.inicio + rangoMrcAntes.tamano);
	 _rangeCharsComposing.tamano = 0;
	 NBASSERT(_rangeCharsComposing.inicio <= _charsPos.conteo)
	 NBASSERT((_rangeCharsComposing.inicio + _rangeCharsComposing.tamano) <= _charsPos.conteo)*/
	//PRINTF_INFO("Rango marcado limpiado.\n");
}

NBRangoI AUEscenaTextoEditableOffscreen::entradaRangoLineaDeCharDef(const SI32 charDefPos){
	NBRangoI r;
	STNBRangeI rr = this->getRngOfLineByChar(charDefPos);
	r.inicio	= rr.start;
	r.tamano	= rr.size;
	return r;
}

void AUEscenaTextoEditableOffscreen::entradaTexto(AUCadenaMutable8* dst){
	//PRINTF_INFO("TextInput, entradaTexto\n");
	if(dst != NULL){
		dst->establecer(this->texto());
	}
}

void AUEscenaTextoEditableOffscreen::entradaTextoMarcado(AUCadenaMutable8* dst){
	//PRINTF_INFO("TextInput, entradaTextoMarcado\n");
	this->getComposingText(dst);
}

void AUEscenaTextoEditableOffscreen::entradaTextoMarcadoReemplazar(const char* nuevoTexto, const ENTextRangeSet setNewComposingAs){
	//PRINTF_INFO("TextInput, entradaTextoMarcadoReemplazar\n");
	const STNBRangeI oldSelRng = this->getRngSelectionAbs();
	const STNBRangeI oldCmpRng = this->getRngComposingAbs();
	const STNBRangeI newCmpRng = this->replaceComposingText(nuevoTexto); NBASSERT(oldCmpRng.start == newCmpRng.start)
	switch (setNewComposingAs) {
		case ENTextRangeSet_Current:
			//PRINTF_INFO("TextInput, entradaTextoMarcadoReemplazar, ENTextRangeSet_Current\n");
			this->setRngComposing(newCmpRng.start, newCmpRng.size);
			break;
		case ENTextRangeSet_Word:
			//PRINTF_INFO("TextInput, entradaTextoMarcadoReemplazar, ENTextRangeSet_Word\n");
			break;
		case ENTextRangeSet_None:
			//PRINTF_INFO("TextInput, entradaTextoMarcadoReemplazar, ENTextRangeSet_None\n");
			this->setRngComposing(newCmpRng.start + newCmpRng.size, 0);
			break;
		default:
			break;
	}
	this->setRngSelection(newCmpRng.start + newCmpRng.size, 0);
}

void AUEscenaTextoEditableOffscreen::entradaTextoSeleccion(AUCadenaMutable8* dst){
	//PRINTF_INFO("TextInput, entradaTextoSeleccion\n");
	this->getSelectedText(dst);
}

void AUEscenaTextoEditableOffscreen::entradaTextoAntesDeSeleccion(const UI32 conteoCharDefs, AUCadenaMutable8* dst){
	//PRINTF_INFO("TextInput, entradaTextoAntesDeSeleccion(%d)\n", conteoCharDefs);
	this->getTextBeforeSelection(conteoCharDefs, dst);
	/*if(dst != NULL){
		dst->vaciar();
		const NBRangoI rng = _cursorData.rngCharsSelection();
		if(rng.inicio > 0 && conteoCharDefs > 0){
			STNBRangeI bRng = { 0, 0 };
			if(rng.inicio < conteoCharDefs){
				//All text (is shorter)
				bRng = _textData.charsRangeToBytesRange(0, rng.inicio);
			} else {
				//Some text
				bRng = _textData.charsRangeToBytesRange(rng.inicio - conteoCharDefs, conteoCharDefs);
			}
			if(bRng.size > 0){
				const char* txt = _textData.texto(NULL);
				dst->agregar(&txt[bRng.start], bRng.size);
			}
		}
		/ *if(_rangeCharsSelected.inicio > 0){
		 SI32 iPrimerElem = _rangeCharsSelected.inicio - conteoCharDefs - 1;
		 SI32 iUltimoElem = _rangeCharsSelected.inicio - 1;
		 if(iPrimerElem < 0) iPrimerElem = 0;
		 if(iUltimoElem < 0) iUltimoElem = 0;
		 const NBRangoI rangoBytesChars = this->privTextRangeOfBytes(iPrimerElem, (iUltimoElem - iPrimerElem + 1));
		 dst->agregar(&_texto.str()[rangoBytesChars.inicio], rangoBytesChars.tamano);
		 }* /
	}*/
}

void AUEscenaTextoEditableOffscreen::entradaTextoDespuesDeSeleccion(const UI32 conteoCharDefs, AUCadenaMutable8* dst){
	//PRINTF_INFO("TextInput, entradaTextoDespuesDeSeleccion(%d)\n", conteoCharDefs);
	this->getTextAfterSelection(conteoCharDefs, dst);
	/*if(dst != NULL){
		dst->vaciar();
		const STNBTextMetrics* metrics = _textData.textMetrics();
		const NBRangoI rng = _cursorData.rngCharsSelection();
		const UI32 iStart = (rng.inicio + rng.tamano);
		if(iStart < metrics->chars.use){
			const UI32 availbl = metrics->chars.use - iStart;
			const char* txt = _textData.texto(NULL);
			dst->agregar(&txt[iStart], (availbl < conteoCharDefs ? availbl : conteoCharDefs));
		}
		/ *if(_rangeCharsSelected.inicio < (_charsPos.conteo - 1)){ //includes the '\0'
		 SI32 iPrimerElem = _rangeCharsSelected.inicio + _rangeCharsSelected.tamano;
		 SI32 iUltimoElem = _rangeCharsSelected.inicio + _rangeCharsSelected.tamano + conteoCharDefs - 1;
		 if(iPrimerElem >= (_charsPos.conteo - 1)) iPrimerElem = _charsPos.conteo - 2;
		 if(iUltimoElem >= (_charsPos.conteo - 1)) iUltimoElem = _charsPos.conteo - 2;
		 const NBRangoI rangoBytesChars = this->privTextRangeOfBytes(iPrimerElem, (iUltimoElem - iPrimerElem + 1));
		 dst->agregar(&_texto.str()[rangoBytesChars.inicio], rangoBytesChars.tamano);
		 }* /
	}*/
}

void AUEscenaTextoEditableOffscreen::entradaTextoSeleccionEliminarAlrededor(const SI32 conteoCharDefsAntes, const SI32 conteoCharDefsDespues, const bool autoSelWord){
	PRINTF_INFO("TextInput, entradaTextoSeleccionEliminarAlrededor(%d, %d)\n", conteoCharDefsAntes, conteoCharDefsDespues);
	const STNBRangeI oldSelRng = this->getRngSelectionAbs();
	const SI32 limitLeft = oldSelRng.start + (conteoCharDefsAntes > 0 ? -conteoCharDefsAntes : conteoCharDefsAntes);
	const SI32 limitNext = oldSelRng.start + oldSelRng.size + (conteoCharDefsDespues > 0 ? conteoCharDefsDespues : 0);
	STNBRangeI delRng;
	delRng.start	= (limitLeft < 0 ? 0 : limitLeft);
	delRng.size		= (limitNext - delRng.start);
	//
	if(oldSelRng.start != delRng.start || (oldSelRng.start + oldSelRng.size) != (delRng.start + delRng.size)){
		if(oldSelRng.size == 0){
			//No selection, just delete
			STNBRangeI charsRng;
			/*const STNBRangeI bytesRng =*/ this->replaceTextAtRng(delRng.start, delRng.size, "", &charsRng);
			this->setRngComposing(charsRng.start, charsRng.size);
			this->setRngSelection(charsRng.start, charsRng.size);
		} else {
			//Backup selection and replace
			AUCadenaMutable8* strSel = new AUCadenaMutable8();
			this->getTextAtRng(oldSelRng.start, oldSelRng.size, strSel);
			{
				STNBRangeI charsRng;
				this->replaceTextAtRng(delRng.start, delRng.size, strSel->str(), &charsRng);
				this->setRngComposing(charsRng.start, charsRng.size);
				this->setRngSelection(charsRng.start, charsRng.size);
			}
			strSel->liberar(NB_RETENEDOR_THIS);
		}
	}
	/*const SI32 conteoCharsDefsAntes		= (_charsPos.conteo - 1);
	 const NBRangoI rangoSel				= _rangeCharsSelected;
	 const UI32 conteoCharDefsAntes		= (pConteoCharDefsAntes > rangoSel.inicio ? rangoSel.inicio : pConteoCharDefsAntes);
	 const UI32 conteoCharDefsDespues	= ((rangoSel.inicio + rangoSel.tamano + pConteoCharDefsDespues) > conteoCharsDefsAntes ? (conteoCharsDefsAntes - (rangoSel.inicio + rangoSel.tamano)) : pConteoCharDefsDespues);
	 if(conteoCharDefsAntes != 0 || conteoCharDefsDespues != 0){
	 //Delete text
	 {
	 const char* textoActual = _texto.str();
	 AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	 //Construir nuevo texto
	 //Agregar texto antes del eliminado
	 if(_rangeCharsSelected.inicio > conteoCharDefsAntes){
	 strTmp->agregar(textoActual, _charsPos.elem(_rangeCharsSelected.inicio - conteoCharDefsAntes));
	 }
	 //Agregar la seleccion actual
	 if(_rangeCharsSelected.tamano > 0){
	 const NBRangoI rangoCharBytes = this->privTextRangeOfBytes(_rangeCharsSelected.inicio, _rangeCharsSelected.tamano);
	 strTmp->agregar(&textoActual[rangoCharBytes.inicio], rangoCharBytes.tamano);
	 }
	 //Agregar texto despues del eliminado
	 if((_rangeCharsSelected.inicio + _rangeCharsSelected.tamano + conteoCharDefsDespues) < (_charsPos.conteo - 1)){
	 strTmp->agregar(&textoActual[_charsPos.elem(_rangeCharsSelected.inicio + _rangeCharsSelected.tamano + conteoCharDefsDespues)]);
	 }
	 //
	 this->privSetString(strTmp->str());
	 strTmp->liberar(NB_RETENEDOR_THIS);
	 }
	 //Actualizar rango de seleccion y marcado
	 if(conteoCharDefsAntes != 0){
	 _rangeCharsSelected.inicio = (rangoSel.inicio - conteoCharDefsAntes);
	 _rangeCharsSelected.tamano = rangoSel.tamano;
	 }
	 if(!autoSelWord){
	 _rangeCharsComposing.inicio = (rangoSel.inicio - conteoCharDefsAntes);
	 _rangeCharsComposing.tamano = 0;
	 } else {
	 this->privSetMarkedRangeAsWordAtPosition(rangoSel.inicio - conteoCharDefsAntes);
	 }
	 }*/
}

void AUEscenaTextoEditableOffscreen::entradaTextoEnCharDefsContenido(const UI32 primerCharDef, const UI32 conteoCharDefs, AUCadenaMutable8* dst){
	//PRINTF_INFO("TextInput, entradaTextoEnCharDefsContenido(%d, +%d)\n", primerCharDef, conteoCharDefs);
	this->getTextAtRng(primerCharDef, conteoCharDefs, dst);
	/*if(dst != NULL){
		dst->vaciar();
		const STNBRangeI bRng = _textData.charsRangeToBytesRange(primerCharDef, conteoCharDefs);
		if(bRng.size > 0){
			const char* txt = _textData.texto(NULL);
			dst->agregar(&txt[bRng.start], bRng.size);
		}
		/ *
		 const NBRangoI rangoBytesChars = this->privTextRangeOfBytes(primerCharDef, conteoCharDefs);
		 if(rangoBytesChars.tamano > 0){
		 dst->agregar(&_texto.str()[rangoBytesChars.inicio], rangoBytesChars.tamano);
		 }* /
	}*/
}

void AUEscenaTextoEditableOffscreen::entradaTextoEnCharDefsReemplazar(const UI32 primerCharDef, const UI32 conteoCharDefs, const char* nuevoTexto, const ENTextRangeSet setNewComposingAs){
	//Reemplazar texto en seleccion
	STNBRangeI charsRng;
	/*const STNBRangeI bytesRng =*/ this->replaceTextAtRng(primerCharDef, conteoCharDefs, nuevoTexto, &charsRng);
	switch (setNewComposingAs) {
		case ENTextRangeSet_Current:
			//PRINTF_INFO("TextInput, entradaTextoEnCharDefsReemplazar, ENTextRangeSet_Current\n");
			this->setRngComposing(charsRng.start, charsRng.size);
			break;
		case ENTextRangeSet_Word:
			//PRINTF_INFO("TextInput, entradaTextoEnCharDefsReemplazar, ENTextRangeSet_Word\n");
			this->setRngComposing(charsRng.start + charsRng.size, 0);
			break;
		case ENTextRangeSet_None:
			//PRINTF_INFO("TextInput, entradaTextoEnCharDefsReemplazar, ENTextRangeSet_None\n");
			this->setRngComposing(charsRng.start + charsRng.size, 0);
			break;
		default:
			break;
	}
	this->setRngSelection(charsRng.start + charsRng.size, 0);
	//
	/*
	const SI32 qCharsDefsBefore	= (_charsPos.conteo - 1);
	this->privTextAtRangeReplace(primerCharDef, conteoCharDefs, nuevoTexto);
	//Actualizar rango de seleccion y marcado
	const SI32 qCharsDefsNow	= (_charsPos.conteo - 1);
	const SI32 qCharsDefsDelta	= qCharsDefsNow - qCharsDefsBefore;
	////PRINTF_INFO("TextInput, entradaTextoEnCharDefsReemplazar(%d, +%d, '%s') ... charsDefsBefore(%d) now(%d) delta(%d)\n", primerCharDef, conteoCharDefs, nuevoTexto, qCharsDefsBefore, qCharsDefsNow, qCharsDefsDelta);
	////PRINTF_INFO("TextInput, entradaTextoEnCharDefsReemplazar(%d, +%d, '%s') ... charsDefsBefore(%d) now(%d) delta(%d)\n", primerCharDef, conteoCharDefs, nuevoTexto, qCharsDefsBefore, qCharsDefsNow, qCharsDefsDelta);
	_rangeCharsSelected.inicio = (primerCharDef + conteoCharDefs + qCharsDefsDelta);
	_rangeCharsSelected.tamano = 0;
	//Note: do not force any behavoir than 'setNewComposingAs' value.
	//Forcing 'ENTextRangeSet_None' when editor was 'ENTextEditorType_Compose' resulted
	//in a writing error on Samsung Android devices 6+ with Samsung defaults keyboard.
	if(setNewComposingAs == ENTextRangeSet_None){
		_rangeCharsComposing.inicio = (primerCharDef + conteoCharDefs + qCharsDefsDelta);
		_rangeCharsComposing.tamano = 0;
	} else if(setNewComposingAs == ENTextRangeSet_Current){
		_rangeCharsComposing.inicio = primerCharDef;
		_rangeCharsComposing.tamano = (conteoCharDefs + qCharsDefsDelta);
	} else { //ENTextRangeSet_Word
		this->privSetMarkedRangeAsWordAtPosition(primerCharDef + conteoCharDefs + qCharsDefsDelta);
	}
	NBASSERT(_rangeCharsSelected.inicio <= _charsPos.conteo)
	NBASSERT((_rangeCharsSelected.inicio + _rangeCharsSelected.tamano) <= _charsPos.conteo)
	NBASSERT(_rangeCharsComposing.inicio <= _charsPos.conteo)
	NBASSERT((_rangeCharsComposing.inicio + _rangeCharsComposing.tamano) <= _charsPos.conteo)
	*/
}

bool AUEscenaTextoEditableOffscreen::entradaTextoEnCharDefsPuedeReemplazarse(const UI32 primerCharDef, const UI32 conteoCharDefs, const char* nuevoTexto){
	//PRINTF_INFO("TextInput, entradaTextoEnCharDefsPuedeReemplazarse\n");
	return true;
}

//Geometria de texto

NBRectangulo AUEscenaTextoEditableOffscreen::entradaRectanguloParaCursor(const UI32 charDefPos){
	//PRINTF_INFO("TextInput, entradaRectanguloParaCursor(%d)\n", charDefPos);
	NBRectangulo r; NBMemory_setZeroSt(r, NBRectangulo);
	const STNBRect localRect = NBTextMetrics_rectForCursor(&this->_textData.metrics, charDefPos);
	NBASSERT(localRect.width > 0 && localRect.height > 0)
	if(localRect.width > 0 && localRect.height > 0){
		//Scene coords
		NBPunto posEIni; NBMATRIZ_MULTIPLICAR_PUNTO(posEIni, _visualSceneMatrix, localRect.x, localRect.y);
		NBPunto posEFin; NBMATRIZ_MULTIPLICAR_PUNTO(posEFin, _visualSceneMatrix, localRect.x + localRect.width, localRect.y + localRect.height);
		//Viewport coords
		const NBPunto posPIni = this->privCoordSceneToPort(posEIni.x, posEIni.y);
		const NBPunto posPFin = this->privCoordSceneToPort(posEFin.x, posEFin.y);
		if(posPIni.x < posPFin.x){
			r.x		= posPIni.x;
			r.ancho	= (posPFin.x - posPIni.x);
		} else {
			r.x		= posPFin.x;
			r.ancho	= (posPIni.x - posPFin.x);
		}
		if(posPIni.y < posPFin.y){
			r.y		= posPIni.y;
			r.alto	= (posPFin.y - posPIni.y);
		} else {
			r.y		= posPFin.y;
			r.alto	= (posPIni.y - posPFin.y);
		}
	}
	/*
	 //NBRectangulo r; r.x = r.y = r.ancho = r.alto = 0;
	 const NBRangoI rngBytes = this->rangeCharDefToBytes(charDefPos, 0);
	 const NBRangoI rngVisual = this->rangeBytesToVisualCharDef(rngBytes.inicio, rngBytes.tamano);
	 NBRectangulo r = this->rectanguloParaCursor(rngVisual.inicio);
	 NBASSERT(r.ancho >= 0.0f && r.alto >= 0.0f)
	 //if(this->idEscena >= 0){
	 NBPunto posEIni; NBMATRIZ_MULTIPLICAR_PUNTO(posEIni, _visualSceneMatrix, r.x, r.y);
	 NBPunto posEFin; NBMATRIZ_MULTIPLICAR_PUNTO(posEFin, _visualSceneMatrix, r.x + r.ancho, r.y + r.alto);
	 / *if(_subescenaId != -1){
	 const NBCajaAABB cajaSubEscena = NBGestorEscena::cajaProyeccionGrupo(_subescenaId, ENGestorEscenaGrupo_Escena);
	 NBTamano tamSubEscena; NBCAJAAABB_TAMANO(tamSubEscena, cajaSubEscena);
	 const NBCajaAABB cajaSprite = _subescenaSprite->cajaAABBLocal();
	 NBTamano tamSprite; NBCAJAAABB_TAMANO(tamSprite, cajaSprite);
	 //
	 NBPunto posEIniSRel; NBPUNTO_ESTABLECER(posEIniSRel, (posEIni.x - cajaSubEscena.xMin) / tamSubEscena.ancho, (posEIni.y - cajaSubEscena.yMin) / tamSubEscena.alto);
	 NBPunto posEFinSRel; NBPUNTO_ESTABLECER(posEFinSRel, (posEFin.x - cajaSubEscena.xMin) / tamSubEscena.ancho, (posEFin.y - cajaSubEscena.yMin) / tamSubEscena.alto);
	 posEIni = _subescenaSprite->coordenadaLocalAEscena(cajaSprite.xMin + (posEIniSRel.x * tamSprite.ancho), cajaSprite.yMin + (posEIniSRel.y * tamSprite.alto));
	 posEFin = _subescenaSprite->coordenadaLocalAEscena(cajaSprite.xMin + (posEFinSRel.x * tamSprite.ancho), cajaSprite.yMin + (posEFinSRel.y * tamSprite.alto));
	 }* /
	 const NBPunto posPIni = this->privCoordSceneToPort(posEIni.x, posEIni.y);
	 const NBPunto posPFin = this->privCoordSceneToPort(posEFin.x, posEFin.y);
	 if(posPIni.x < posPFin.x){ r.x = posPIni.x; r.ancho = (posPFin.x - posPIni.x); } else { r.x = posPFin.x; r.ancho = (posPIni.x - posPFin.x); }
	 if(posPIni.y < posPFin.y){ r.y = posPIni.y; r.alto = (posPFin.y - posPIni.y); } else { r.y = posPFin.y; r.alto = (posPIni.y - posPFin.y); }
	 //}
	 */
	return r;
}

NBRectangulo AUEscenaTextoEditableOffscreen::entradaPrimerRectanguloParaCharDefs(const UI32 primerCharDef, const UI32 conteoCharDefs){
	//PRINTF_INFO("TextInput, entradaPrimerRectanguloParaCharDefs(%d, +%d)\n", primerCharDef, conteoCharDefs);
	NBRectangulo r; NBMemory_setZeroSt(r, NBRectangulo);
	const STNBRect localRect = NBTextMetrics_firstRectForCharsRange(&this->_textData.metrics, primerCharDef, conteoCharDefs);
	NBASSERT(localRect.width > 0 && localRect.height > 0)
	if(localRect.width > 0 && localRect.height > 0){
		//Scene coords
		NBPunto posEIni; NBMATRIZ_MULTIPLICAR_PUNTO(posEIni, _visualSceneMatrix, localRect.x, localRect.y);
		NBPunto posEFin; NBMATRIZ_MULTIPLICAR_PUNTO(posEFin, _visualSceneMatrix, localRect.x + localRect.width, localRect.y + localRect.height);
		//Viewport coords
		const NBPunto posPIni = this->privCoordSceneToPort(posEIni.x, posEIni.y);
		const NBPunto posPFin = this->privCoordSceneToPort(posEFin.x, posEFin.y);
		if(posPIni.x < posPFin.x){
			r.x = posPIni.x;
			r.ancho = (posPFin.x - posPIni.x);
		} else {
			r.x = posPFin.x;
			r.ancho = (posPIni.x - posPFin.x);
		}
		if(posPIni.y < posPFin.y){
			r.y = posPIni.y;
			r.alto = (posPFin.y - posPIni.y);
		} else {
			r.y = posPFin.y;
			r.alto = (posPIni.y - posPFin.y);
		}
	}
	/*//NBRectangulo r; r.x = r.y = r.ancho = r.alto = 0;
	 const NBRangoI rngBytes = this->rangeCharDefToBytes(primerCharDef, conteoCharDefs);
	 const NBRangoI rngVisual = this->rangeBytesToVisualCharDef(rngBytes.inicio, rngBytes.tamano);
	 NBRectangulo r = this->primerRectanguloParaCharDefs(rngVisual.inicio, rngVisual.tamano);
	 //if(this->idEscena >= 0){
	 NBPunto posEIni; NBMATRIZ_MULTIPLICAR_PUNTO(posEIni, _visualSceneMatrix, r.x, r.y);
	 NBPunto posEFin; NBMATRIZ_MULTIPLICAR_PUNTO(posEFin, _visualSceneMatrix, r.x + r.ancho, r.y + r.alto);
	 / *if(_subescenaId != -1){
	 const NBCajaAABB cajaSubEscena = NBGestorEscena::cajaProyeccionGrupo(_subescenaId, ENGestorEscenaGrupo_Escena);
	 NBTamano tamSubEscena; NBCAJAAABB_TAMANO(tamSubEscena, cajaSubEscena);
	 const NBCajaAABB cajaSprite = _subescenaSprite->cajaAABBLocal();
	 NBTamano tamSprite; NBCAJAAABB_TAMANO(tamSprite, cajaSprite);
	 //
	 NBPunto posEIniSRel; NBPUNTO_ESTABLECER(posEIniSRel, (posEIni.x - cajaSubEscena.xMin) / tamSubEscena.ancho, (posEIni.y - cajaSubEscena.yMin) / tamSubEscena.alto);
	 NBPunto posEFinSRel; NBPUNTO_ESTABLECER(posEFinSRel, (posEFin.x - cajaSubEscena.xMin) / tamSubEscena.ancho, (posEFin.y - cajaSubEscena.yMin) / tamSubEscena.alto);
	 posEIni = _subescenaSprite->coordenadaLocalAEscena(cajaSprite.xMin + (posEIniSRel.x * tamSprite.ancho), cajaSprite.yMin + (posEIniSRel.y * tamSprite.alto));
	 posEFin = _subescenaSprite->coordenadaLocalAEscena(cajaSprite.xMin + (posEFinSRel.x * tamSprite.ancho), cajaSprite.yMin + (posEFinSRel.y * tamSprite.alto));
	 }* /
	 const NBPunto posPIni = this->privCoordSceneToPort(posEIni.x, posEIni.y);
	 const NBPunto posPFin = this->privCoordSceneToPort(posEFin.x, posEFin.y);
	 if(posPIni.x < posPFin.x){ r.x = posPIni.x; r.ancho = (posPFin.x - posPIni.x); } else { r.x = posPFin.x; r.ancho = (posPIni.x - posPFin.x); }
	 if(posPIni.y < posPFin.y){ r.y = posPIni.y; r.alto = (posPFin.y - posPIni.y); } else { r.y = posPFin.y; r.alto = (posPIni.y - posPFin.y); }
	 //}*/
	return r;
}

void AUEscenaTextoEditableOffscreen::entradaRectangulosParaCharDefs(const UI32 primerCharDef, const UI32 conteoCharDefs, AUArregloNativoMutableP<NBRectangulo>* dst){
	//PRINTF_INFO("TextInput, entradaRectangulosParaCharDefs(%d, +%d)\n", primerCharDef, conteoCharDefs);
	if(dst != NULL){
		dst->vaciar();
		{
			STNBArray rects;
			NBArray_init(&rects, sizeof(STNBRect), NULL);
			NBTextMetrics_rectsForCharsRange(&this->_textData.metrics, primerCharDef, conteoCharDefs, &rects);
			UI32 i; for(i = 0 ; i < rects.use; i++){
				const STNBRect* localRect = NBArray_itmPtrAtIndex(&rects, STNBRect, i);
				NBASSERT(localRect->width > 0 && localRect->height > 0)
				if(localRect->width > 0 && localRect->height > 0){
					//Scene coords
					NBPunto posEIni; NBMATRIZ_MULTIPLICAR_PUNTO(posEIni, _visualSceneMatrix, localRect->x, localRect->y);
					NBPunto posEFin; NBMATRIZ_MULTIPLICAR_PUNTO(posEFin, _visualSceneMatrix, localRect->x + localRect->width, localRect->y + localRect->height);
					//Viewport coords
					const NBPunto posPIni = this->privCoordSceneToPort(posEIni.x, posEIni.y);
					const NBPunto posPFin = this->privCoordSceneToPort(posEFin.x, posEFin.y);
					NBRectangulo r;
					if(posPIni.x < posPFin.x){
						r.x = posPIni.x;
						r.ancho = (posPFin.x - posPIni.x);
					} else {
						r.x = posPFin.x;
						r.ancho = (posPIni.x - posPFin.x);
					}
					if(posPIni.y < posPFin.y){
						r.y = posPIni.y;
						r.alto = (posPFin.y - posPIni.y);
					} else {
						r.y = posPFin.y;
						r.alto = (posPIni.y - posPFin.y);
					}
					dst->agregarElemento(r);
				}
			}
			NBArray_release(&rects);
		}
		/*const NBRangoI rngBytes = this->rangeCharDefToBytes(primerCharDef, conteoCharDefs);
		 const NBRangoI rngVisual = this->rangeBytesToVisualCharDef(rngBytes.inicio, rngBytes.tamano);
		 this->rectangulosParaCharDefs(rngVisual.inicio, rngVisual.tamano, dst);
		 //if(this->idEscena >= 0){
		 //Convertir nuevos rectangulos
		 SI32 i; const SI32 conteo = dst->conteo;
		 for(i = 0; i < conteo; i++){
		 NBRectangulo* r = dst->elemPtr(i);
		 NBPunto posEIni; NBMATRIZ_MULTIPLICAR_PUNTO(posEIni, _visualSceneMatrix, r->x, r->y);
		 NBPunto posEFin; NBMATRIZ_MULTIPLICAR_PUNTO(posEFin, _visualSceneMatrix, r->x + r->ancho, r->y + r->alto);
		 / *if(_subescenaId != -1){
		 const NBCajaAABB cajaSubEscena = NBGestorEscena::cajaProyeccionGrupo(_subescenaId, ENGestorEscenaGrupo_Escena);
		 NBTamano tamSubEscena; NBCAJAAABB_TAMANO(tamSubEscena, cajaSubEscena);
		 const NBCajaAABB cajaSprite = _subescenaSprite->cajaAABBLocal();
		 NBTamano tamSprite; NBCAJAAABB_TAMANO(tamSprite, cajaSprite);
		 //
		 NBPunto posEIniSRel; NBPUNTO_ESTABLECER(posEIniSRel, (posEIni.x - cajaSubEscena.xMin) / tamSubEscena.ancho, (posEIni.y - cajaSubEscena.yMin) / tamSubEscena.alto);
		 NBPunto posEFinSRel; NBPUNTO_ESTABLECER(posEFinSRel, (posEFin.x - cajaSubEscena.xMin) / tamSubEscena.ancho, (posEFin.y - cajaSubEscena.yMin) / tamSubEscena.alto);
		 posEIni = _subescenaSprite->coordenadaLocalAEscena(cajaSprite.xMin + (posEIniSRel.x * tamSprite.ancho), cajaSprite.yMin + (posEIniSRel.y * tamSprite.alto));
		 posEFin = _subescenaSprite->coordenadaLocalAEscena(cajaSprite.xMin + (posEFinSRel.x * tamSprite.ancho), cajaSprite.yMin + (posEFinSRel.y * tamSprite.alto));
		 }* /
		 const NBPunto posPIni = this->privCoordSceneToPort(posEIni.x, posEIni.y);
		 const NBPunto posPFin = this->privCoordSceneToPort(posEFin.x, posEFin.y);
		 if(posPIni.x < posPFin.x){ r->x = posPIni.x; r->ancho = (posPFin.x - posPIni.x); } else { r->x = posPFin.x; r->ancho = (posPIni.x - posPFin.x); }
		 if(posPIni.y < posPFin.y){ r->y = posPIni.y; r->alto = (posPFin.y - posPIni.y); } else { r->y = posPFin.y; r->alto = (posPIni.y - posPFin.y); }
		 }
		 //}*/
	}
}

UI32 AUEscenaTextoEditableOffscreen::entradaIndiceCharDefMasCercano(const float xViewPort, const float yViewPort){
    //Scene coords
    const NBPunto posScn = this->privCoordPortToScene(xViewPort, yViewPort);
    //Local coord
    NBMatriz invMatrix; NBPunto poslcl;
    NBMATRIZ_INVERSA(invMatrix, _visualSceneMatrix);
    NBMATRIZ_MULTIPLICAR_PUNTO(poslcl, invMatrix, posScn.x, posScn.y);
	//PRINTF_INFO("TextInput, entradaIndiceCharDefMasCercano(%f, %f)\n", xLocalTexto, yLocalTexto);
    return NBTextMetrics_closestCharIdx(&this->_textData.metrics, poslcl.x, poslcl.y, NULL);
}

UI32 AUEscenaTextoEditableOffscreen::entradaIndiceCharDefMasCercanoEnCharDefs(const float xLocalTexto, const float yLocalTexto, const UI32 primerCharDef, const UI32 conteoCharDefs){
	//PRINTF_INFO("TextInput, entradaIndiceCharDefMasCercano(%f, %f)(%d, +%d)\n", xLocalTexto, yLocalTexto, primerCharDef, conteoCharDefs);
	PRINTF_ERROR("Implement entradaIndiceCharDefMasCercanoEnCharDefs.\n");
	return 0; //_textoExpuesto->indiceCharDefMasCercanoEnCharDefs(ENTextoSeleccionLado_Izq, xLocalTexto, yLocalTexto, primerCharDef, conteoCharDefs);
}

UI32 AUEscenaTextoEditableOffscreen::entradaIndiceCharDefPrimero(){
	//PRINTF_INFO("TextInput, entradaIndiceCharDefPrimero\n");
	return 0;
}

UI32 AUEscenaTextoEditableOffscreen::entradaIndiceCharDefUltimo(){
	//PRINTF_INFO("TextInput, entradaIndiceCharDefUltimo\n");
	UI32 r = 0;
	if(_textData.metrics.chars.use > 0){
		r = (_textData.metrics.chars.use - 1);
	}
	/*if(_charsPos.conteo > 1){
	 r = (_charsPos.conteo - 2);
	 }*/
	return r;
}

SI32 AUEscenaTextoEditableOffscreen::entradaIndiceCharDefDesdePosicion(const UI32 charDefPos, const SI32 movimiento, const ENTextoDireccion direccion){
	//PRINTF_INFO("TextInput, entradaIndiceCharDefDesdePosicion(%d, %d, %d).\n", charDefPos, movimiento, direccion);
	return -1;
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaTextoEditableOffscreen, AUEscenaTextoEditable)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaTextoEditableOffscreen, "AUEscenaTextoEditableOffscreen", AUEscenaTextoEditable)
AUOBJMETODOS_CLONAR_THIS(AUEscenaTextoEditableOffscreen)






