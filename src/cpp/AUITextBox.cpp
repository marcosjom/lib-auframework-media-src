//
//  AUITextBox.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 10/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUITextBox.h"
//
#include "nb/core/NBEncoding.h"
#include "NBGestorAnimadores.h"
#include "NBGestorEscena.h"
#include "AUEscenaTextoEditableOffscreen.h"

AUITextBox::AUITextBox(const ENTextEditorType editorType, AUFuenteRender* fuenteTextura, const float anchoCaja, const float lineasCaja) : AUEscenaContenedorLimitado()
	, _editorType(editorType)
	, _allowMultiline(TRUE)
	//_margenes
	, _boxWidth(anchoCaja)
	, _boxHeightInLines(lineasCaja)
	, _passChar(NULL)
	, _passCharSz(0)
	, _escuchadorCajaTxt(NULL)
	//
	, _atFocus(false)
	, _bg(NULL)
	, _value(NULL)
	, _visual(NULL)
	, _visualHelp(NULL)
	, _valueTextSeqIdCloned(0)
	, _valueCursorSeqIdCloned(0)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUITextBox::AUITextBox")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUITextBox")
	_margenes.left	= 0;
	_margenes.right	= 0;
	_margenes.top	= 0;
	_margenes.bottom= 0;
	//
	_bg		= new(this) AUEscenaSpriteElastico();
	_bg->establecerEscuchadorTouches(this, this);
	this->agregarObjetoEscena(_bg);
	//
	_font	= fuenteTextura; if(_font != NULL) _font->retener(NB_RETENEDOR_THIS);
	//Button
	{
		NBMemory_setZero(_btn);
		_btn.icon				= NULL;
		_btn.action				= ENITextboxBtnAction_None;
		_btn.touchDepth			= 0;
	}
	//
	{
		_subscn.Id				= -1;
		_subscn.Sprite			= NULL;
		_subscn.Contenedor		= new(this) AUEscenaContenedor();
		_subscn.colorBg.r		= _subscn.colorBg.g = _subscn.colorBg.b = _subscn.colorBg.a = 255;
		NBCAJAAABB_INICIALIZAR(_subscn.CajaBase)
		_subscn.Tamano.ancho	= 0;
		_subscn.Tamano.alto		= 0;
		_subscn.Scroll.ancho	= 0.0f;
		_subscn.Scroll.alto		= 0.0f;
	}
	//
	{
		_value			= new(this) AUEscenaTextoEditable(fuenteTextura);
		//
		_visual			= _value; NBASSERT(_passChar == NULL && _passCharSz == 0)
		_visual->retener(NB_RETENEDOR_THIS);
		_visual->establecerEscuchadorTouches(NULL, NULL); //disable touches
		_visual->setIsEditable(TRUE);
		_visual->setAnimatingCursor(_atFocus);
		_visual->establecerMultiplicadorColor8(0, 0, 0, 255);
		_visual->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_subscn.Contenedor->agregarObjetoEscena(_visual);
		//
		_visualHelp		= new(this) AUEscenaTexto(_value->fuenteRender());
		_visualHelp->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_visualHelp->establecerMultiplicadorColor8(155, 155, 155, 255);
		_subscn.Contenedor->agregarObjetoEscena(_visualHelp);
	}
	//
	this->privEstablecerTamanoCaja(_boxWidth, _boxHeightInLines, TRUE);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUITextBox::AUITextBox(const ENTextEditorType editorType, AUFuenteRender* fuenteTextura, const float anchoCaja, const float lineasCaja, const NBMargenes margenes) : AUEscenaContenedorLimitado()
	, _editorType(editorType)
	, _allowMultiline(TRUE)
	//_margenes
	, _boxWidth(anchoCaja)
	, _boxHeightInLines(lineasCaja)
	, _passChar(NULL)
	, _passCharSz(0)
	, _escuchadorCajaTxt(NULL)
	//
	, _atFocus(false)
	, _bg(NULL)
	, _value(NULL)
	, _visual(NULL)
	, _visualHelp(NULL)
	, _valueTextSeqIdCloned(0)
	, _valueCursorSeqIdCloned(0)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUITextBox::AUITextBox")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUITextBox")
	_margenes		= margenes;
	//
	_bg		= new(this) AUEscenaSpriteElastico();
	_bg->establecerEscuchadorTouches(this, this);
	this->agregarObjetoEscena(_bg);
	//
	_font	= fuenteTextura; if(_font != NULL) _font->retener(NB_RETENEDOR_THIS);
	//Button
	{
		NBMemory_setZero(_btn);
		_btn.icon				= NULL;
		_btn.action				= ENITextboxBtnAction_None;
		_btn.touchDepth			= 0;
	}
	//
	{
		_subscn.Id				= -1;
		_subscn.Sprite			= NULL;
		_subscn.Contenedor		= new(this) AUEscenaContenedor();
		_subscn.colorBg.r		= _subscn.colorBg.g = _subscn.colorBg.b = _subscn.colorBg.a = 255;
		NBCAJAAABB_INICIALIZAR(_subscn.CajaBase)
		_subscn.Tamano.ancho	= 0;
		_subscn.Tamano.alto		= 0;
		_subscn.Scroll.ancho	= 0.0f;
		_subscn.Scroll.alto		= 0.0f;
	}
	//
	{
		_value			= new(this) AUEscenaTextoEditable(fuenteTextura);
		//
		_visual			= _value; NBASSERT(_passChar == NULL && _passCharSz == 0)
		_visual->retener(NB_RETENEDOR_THIS);
		_visual->establecerEscuchadorTouches(NULL, NULL); //disable touches
		_visual->setIsEditable(TRUE);
		_visual->setAnimatingCursor(_atFocus);
		_visual->establecerMultiplicadorColor8(0, 0, 0, 255);
		_visual->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_subscn.Contenedor->agregarObjetoEscena(_visual);
		//
		_visualHelp		= new(this) AUEscenaTexto(_value->fuenteRender());
		_visualHelp->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_visualHelp->establecerMultiplicadorColor8(155, 155, 155, 255);
		_subscn.Contenedor->agregarObjetoEscena(_visualHelp);
	}
	//
	this->privEstablecerTamanoCaja(_boxWidth, _boxHeightInLines, TRUE);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUITextBox::AUITextBox(const ENTextEditorType editorType, AUFuenteRender* fuenteTextura, const float anchoCaja, const float lineasCaja, const float margenLeft, const float margenRight, const float margenTop, const float margenBottom) : AUEscenaContenedorLimitado()
	, _editorType(editorType)
	, _allowMultiline(TRUE)
	//, _margenes
	, _boxWidth(anchoCaja)
	, _boxHeightInLines(lineasCaja)
	, _passChar(NULL)
	, _passCharSz(0)
	, _escuchadorCajaTxt(NULL)
	//
	, _atFocus(false)
	, _bg(NULL)
	, _value(NULL)
	, _visual(NULL)
	, _visualHelp(NULL)
	, _valueTextSeqIdCloned(0)
	, _valueCursorSeqIdCloned(0)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUITextBox::AUITextBox")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUITextBox")
	_margenes.left	= margenLeft;
	_margenes.right	= margenRight;
	_margenes.top	= margenTop;
	_margenes.bottom= margenBottom;
	//
	_bg		= new(this) AUEscenaSpriteElastico();
	_bg->establecerEscuchadorTouches(this, this);
	this->agregarObjetoEscena(_bg);
	//
	_font	= fuenteTextura; if(_font != NULL) _font->retener(NB_RETENEDOR_THIS);
	//Button
	{
		NBMemory_setZero(_btn);
		_btn.icon				= NULL;
		_btn.action				= ENITextboxBtnAction_None;
		_btn.touchDepth			= 0;
	}
	//
	{
		_subscn.Id				= -1;
		_subscn.Sprite			= NULL;
		_subscn.Contenedor		= new(this) AUEscenaContenedor();
		_subscn.colorBg.r		= _subscn.colorBg.g = _subscn.colorBg.b = _subscn.colorBg.a = 255;
		NBCAJAAABB_INICIALIZAR(_subscn.CajaBase)
		_subscn.Tamano.ancho	= 0;
		_subscn.Tamano.alto		= 0;
		_subscn.Scroll.ancho	= 0.0f;
		_subscn.Scroll.alto		= 0.0f;
	}
	//
	{
		_value			= new(this) AUEscenaTextoEditable(fuenteTextura);
		//
		_visual			= _value; NBASSERT(_passChar == NULL && _passCharSz == 0)
		_visual->retener(NB_RETENEDOR_THIS);
		_visual->establecerEscuchadorTouches(NULL, NULL); //disable touches
		_visual->setIsEditable(TRUE);
		_visual->setAnimatingCursor(_atFocus);
		_visual->establecerMultiplicadorColor8(0, 0, 0, 255);
		_visual->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_subscn.Contenedor->agregarObjetoEscena(_visual);
		//
		_visualHelp		= new(this) AUEscenaTexto(_value->fuenteRender());
		_visualHelp->establecerAlineaciones(ENNBTextLineAlignH_Left, ENNBTextAlignV_FromTop);
		_visualHelp->establecerMultiplicadorColor8(155, 155, 155, 255);
		_subscn.Contenedor->agregarObjetoEscena(_visualHelp);
	}
	//
	this->privEstablecerTamanoCaja(_boxWidth, _boxHeightInLines, TRUE);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUITextBox::~AUITextBox(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUITextBox::~AUITextBox")
	//Props
	if(_passChar != NULL){
		NBMemory_free(_passChar);
		_passChar	= NULL;
		_passCharSz	= 0;
	}
	//Visual
	{
		NBASSERT(!_atFocus)
		if(_font != NULL) _font->liberar(NB_RETENEDOR_THIS); _font = NULL;
		if(_bg != NULL) _bg->liberar(NB_RETENEDOR_THIS); _bg = NULL;
		if(_value != NULL) _value->liberar(NB_RETENEDOR_THIS); _value = NULL;
		if(_visual != NULL) _visual->liberar(NB_RETENEDOR_THIS); _visual = NULL;
		if(_visualHelp != NULL) _visualHelp->liberar(NB_RETENEDOR_THIS); _visualHelp = NULL;
		_valueTextSeqIdCloned	= 0;
		_valueCursorSeqIdCloned	= 0;
	}
	//Button
	{
		if(_btn.icon != NULL) _btn.icon->liberar(NB_RETENEDOR_THIS); _btn.icon = NULL;
	}
	//Subscene
	{
		if(_subscn.Contenedor != NULL) _subscn.Contenedor->liberar(NB_RETENEDOR_THIS); _subscn.Contenedor = NULL;
		if(_subscn.Sprite != NULL) _subscn.Sprite->liberar(NB_RETENEDOR_THIS); _subscn.Sprite = NULL;
		if(_subscn.Id != -1) NBGestorEscena::liberarEscena(_subscn.Id); _subscn.Id = -1;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUITextBox::agregadoEnEscena(){
	AUEscenaContenedor::agregadoEnEscena();
	if(this->idEscena != -1){
		NBGestorEscena::habilitarEscena(_subscn.Id);
	}
}

void AUITextBox::quitandoDeEscena(){
	AUEscenaContenedor::quitandoDeEscena();
	NBGestorTeclas::escuchadorRenunciar(this);
	if(this->idEscena != -1){
		NBGestorEscena::deshabilitarEscena(_subscn.Id);
	}
}

//

bool AUITextBox::atFocus() const {
	return _atFocus;
}

void AUITextBox::focusObtain(){
	NBGestorTeclas::escuchadorEstablecer(this, this);
}

void AUITextBox::focusRelease(){
	NBGestorTeclas::escuchadorRenunciar(this);
}

//

IEscuchadorITextBox* AUITextBox::escuchadorTextBox(){
	return _escuchadorCajaTxt;
}

void AUITextBox::establecerEscuchadorTextBox(IEscuchadorITextBox* escuchador){
	_escuchadorCajaTxt = escuchador;
}

//

NBMargenes AUITextBox::margenes() const {
	return _margenes;
}

void AUITextBox::establecerMargenes(const NBMargenes margenes){
	if(_margenes.left != margenes.left || _margenes.right != margenes.right || _margenes.top != margenes.top || _margenes.bottom != margenes.bottom){
		_margenes = margenes;
		this->privEstablecerTamanoCaja(_boxWidth, _boxHeightInLines, TRUE);
	}
}

void AUITextBox::establecerMargenes(const float margenLeft, const float margenRight, const float margenTop, const float margenBottom){
	if(_margenes.left != margenLeft || _margenes.right != margenRight || _margenes.top != margenTop || _margenes.bottom != margenBottom){
		_margenes.left		= margenLeft;
		_margenes.right		= margenRight;
		_margenes.top		= margenTop;
		_margenes.bottom	= margenBottom;
		this->privEstablecerTamanoCaja(_boxWidth, _boxHeightInLines, TRUE);
	}
}

//

float AUITextBox::boxWidth() const {
	return _boxWidth;
}

float AUITextBox::boxHeightInLines() const {
	return _boxHeightInLines;
}

//

AUFuenteRender* AUITextBox::getFont() const {
	return _font;
}

//

AUTextura* AUITextBox::fondo() const {
	return _bg->textura();
}

void AUITextBox::establecerFondo(AUTextura* textura){
	_bg->establecerTextura(textura);
}

void AUITextBox::establecerFondo(AUTextura* textura, const STNBColor8 color){
	_bg->establecerTextura(textura);
	_bg->establecerMultiplicadorColor8(color.r, color.g, color.b, color.a);
}

void AUITextBox::establecerFondoColor(const STNBColor8 color){
	_bg->establecerMultiplicadorColor8(color.r, color.g, color.b, color.a);
}

void AUITextBox::establecerFondoColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	_bg->establecerMultiplicadorColor8(r, g, b, a);
}

void AUITextBox::establecerFondoBoxColor(const STNBColor8 color){
	_subscn.colorBg = color;
	//Apply color
	if(_subscn.Id >= 0){
		NBGestorEscena::establecerColorFondo(_subscn.Id, (float)_subscn.colorBg.r / 255.0f , (float)_subscn.colorBg.g / 255.0f , (float)_subscn.colorBg.b / 255.0f , (float)_subscn.colorBg.a / 255.0f);
	}
}

void AUITextBox::establecerFondoBoxColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	_subscn.colorBg.r = r;
	_subscn.colorBg.g = g;
	_subscn.colorBg.b = b;
	_subscn.colorBg.a = a;
	//Apply color
	if(_subscn.Id >= 0){
		NBGestorEscena::establecerColorFondo(_subscn.Id, (float)_subscn.colorBg.r / 255.0f , (float)_subscn.colorBg.g / 255.0f , (float)_subscn.colorBg.b / 255.0f , (float)_subscn.colorBg.a / 255.0f);
	}
}

//

bool AUITextBox::textoPermitirMultilinea(){
	return _allowMultiline;
}

void AUITextBox::establecerTextoPermitirMultilinea(const bool permitirMultilinea){
	if(_allowMultiline != permitirMultilinea){
		const STNBRect column = {0, 0, _boxWidth - _margenes.left - _margenes.right, this->altoParaLineasVisibles(_boxHeightInLines) };
		_visual->organizarTextoEnCol(column, permitirMultilinea);
		_visualHelp->organizarTextoEnCol(column, permitirMultilinea);
		if(_value != _visual){
			_value->organizarTextoEnCol(column, permitirMultilinea);
		}
		//Move content
		this->privAsegurarCursorEstaVisible();
		//
		_allowMultiline = permitirMultilinea;
	}
}

//

const char* AUITextBox::passwordChar() const {
	return _passChar;
}

void AUITextBox::setPasswordChar(const char* passChar){
	const STNBRect column = {0, 0, _boxWidth - _margenes.left - _margenes.right, this->altoParaLineasVisibles(_boxHeightInLines) };
	//Validate not empty-string
	if(passChar != NULL){
		if(passChar[0] == '\0') passChar = NULL;
	}
	//
	if(_passChar != NULL && passChar == NULL){
		//----------------------
		//- Removing pass-char
		//----------------------
		//Remove passchar
		{
			NBASSERT(_passChar != NULL && _passCharSz > 0)
			if(_passChar != NULL){
				NBMemory_free(_passChar);
				_passChar	= NULL;
				_passCharSz	= 0;
			}
		}
		//Remove visual
		{
			NBASSERT(_value != _visual)
			if(_visual != NULL){
				_subscn.Contenedor->quitarObjetoEscena(_visual);
				_visual->liberar(NB_RETENEDOR_THIS);
				_visual = NULL;
			}
		}
		//Set value as visual
		{
			NBASSERT(_value != _visual)
			NBASSERT(_value->contenedor() == NULL)
			_visual = _value;
			_visual->retener(NB_RETENEDOR_THIS);
			_visual->establecerEscuchadorTouches(NULL, NULL); //disable touches
			_visual->setIsEditable(TRUE);
			_visual->setAnimatingCursor(_atFocus);
			_subscn.Contenedor->agregarObjetoEscena(_visual);
		}
		//Move content
		this->privAsegurarCursorEstaVisible();
	} else if(_passChar == NULL && passChar != NULL){
		//----------------------
		//- Creating a new pass-char
		//----------------------
		NBASSERT((passChar[0] & 0xC0) != 0x80) //UTF8 format error
		//Create passChar
		{
			NBASSERT(_passChar == NULL && _passCharSz == 0)
			_passCharSz = NBEncoding_utf8BytesExpected(*passChar);
			_passChar	= (char*)NBMemory_alloc(_passCharSz + 1);
			NBMemory_copy(_passChar, passChar, _passCharSz);
			_passChar[_passCharSz] = '\0';
		}
		//Remove value from scene
		{
			NBASSERT(_value == _visual)
			if(_value == _visual){
				if(_visual != NULL){
					NBASSERT(_visual->contenedor() != NULL)
					_subscn.Contenedor->quitarObjetoEscena(_visual);
					_visual->liberar(NB_RETENEDOR_THIS);
					_visual = NULL;
				}
			}
		}
		//Create new visual
		{
			const UI32 unicodes = NBString_strLenUnicodes(_value->texto());
			_visual = new(this) AUEscenaTextoEditable();
			_visual->establecerFuenteRender(_value->fuenteRender());
			_visual->establecerAlineaciones(_value->alineacionH(), _value->alineacionV());
			_visual->copiarPropiedadesDe(_value);
			_visual->establecerTextoRepitiendoEnCol(_passChar, unicodes, column, _allowMultiline);
			_visual->establecerEscuchadorTouches(NULL, NULL); //disable touches
			_visual->setIsEditable(TRUE);
			_visual->setAnimatingCursor(_atFocus);
			_subscn.Contenedor->agregarObjetoEscena(_visual);
			//Sync ranges
			{
				const STNBRangeI sRng = _value->getRngSelection();
				const STNBRangeI cRng = _value->getRngComposing();
				_visual->setRngSelection(sRng);
				_visual->setRngComposing(cRng);
			}
		}
		//Move content
		this->privAsegurarCursorEstaVisible();
	} else if(_passChar != NULL && passChar != NULL){
		//----------------------
		//- Posibly chaning pass chars
		//----------------------
		NBASSERT((passChar[0] & 0xC0) != 0x80) //UTF8 format error
		NBASSERT(_passChar != NULL && _passCharSz > 0)
		if(!NBString_strIsEqualStrBytes(passChar, _passChar, _passCharSz)){
			//Change passchar
			{
				if(_passChar != NULL){
					NBMemory_free(_passChar);
					_passChar	= NULL;
					_passCharSz	= 0;
				}
				{
					NBASSERT(_passChar == NULL && _passCharSz == 0)
					_passCharSz = NBEncoding_utf8BytesExpected(*passChar);
					_passChar	= (char*)NBMemory_alloc(_passCharSz + 1);
					NBMemory_copy(_passChar, passChar, _passCharSz);
					_passChar[_passCharSz] = '\0';
				}
			}
			//Refresh visual
			{
				const UI32 unicodes = NBString_strLenUnicodes(_value->texto());
				_visual->establecerTextoRepitiendoEnCol(_passChar, unicodes, column, _allowMultiline);
				//Sync ranges
				{
					const STNBRangeI sRng = _value->getRngSelection();
					const STNBRangeI cRng = _value->getRngComposing();
					_visual->setRngSelection(sRng);
					_visual->setRngComposing(cRng);
				}
			}
		}
		//Move content
		this->privAsegurarCursorEstaVisible();
	}
}

void AUITextBox::setButton(AUTextura* tex, const ENITextboxBtnAction action, const STNBColor8 color){
	this->setButton(tex, action, color.r, color.g, color.b, color.a);
}
void AUITextBox::setButton(AUTextura* tex, const ENITextboxBtnAction action, const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	if(tex == NULL){
		//Remove
		if(_btn.icon != NULL){
			AUEscenaContenedor* parent = (AUEscenaContenedor*)_btn.icon->contenedor();
			if(parent != NULL) parent->quitarObjetoEscena(_btn.icon);
			_btn.icon->liberar(NB_RETENEDOR_THIS);
			_btn.icon = NULL;
		}
	} else {
		if(_btn.icon != NULL){
			_btn.icon->establecerTextura(tex);
			_btn.icon->redimensionar(tex);
		} else {
			_btn.icon = new(this) AUEscenaSprite(tex);
			_btn.icon->establecerEscuchadorTouches(this, this);
			this->agregarObjetoEscena(_btn.icon);
		}
		_btn.action	= action;
		_btn.color.r = r; _btn.color.g = g; _btn.color.b = b; _btn.color.a = a;
		_btn.icon->establecerMultiplicadorColor8(r, g, b, (((SI32)a * 3) / (_btn.touchDepth > 0 ? 4 : 3)));
	}
	this->privOrganizeBtn();
}

void AUITextBox::setButtonColor(const STNBColor8 color){
	this->setButtonColor(color.r, color.g, color.b, color.a);
}

void AUITextBox::setButtonColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	_btn.color.r = r; _btn.color.g = g; _btn.color.b = b; _btn.color.a = a;
	if(_btn.icon != NULL){
		_btn.icon->establecerMultiplicadorColor8(r, g, b, (((SI32)a * 3) / (_btn.touchDepth > 0 ? 4 : 3)));
	}
}

//

ENNBTextLineAlignH AUITextBox::alineacionH() const {
	return _value->alineacionH();
}

ENNBTextAlignV AUITextBox::alineacionV() const {
	return _value->alineacionV();
}

void AUITextBox::establecerAlineacionH(ENNBTextLineAlignH alineacionH){
	_visualHelp->establecerAlineacionH(alineacionH);
	_value->establecerAlineacionH(alineacionH);
	if(_value != _visual){
		_visual->establecerAlineacionH(alineacionH);
	}
}

void AUITextBox::establecerAlineacionV(ENNBTextAlignV alineacionV){
	_visualHelp->establecerAlineacionV(alineacionV);
	_value->establecerAlineacionV(alineacionV);
	if(_value != _visual){
		_visual->establecerAlineacionV(alineacionV);
	}
}

void AUITextBox::establecerAlineaciones(ENNBTextLineAlignH alineacionH, ENNBTextAlignV alineacionV){
	_visualHelp->establecerAlineaciones(alineacionH, alineacionV);
	_value->establecerAlineaciones(alineacionH, alineacionV);
	if(_value != _visual){
		_visual->establecerAlineaciones(alineacionH, alineacionV);
	}
}

//

const char* AUITextBox::texto() const {
	return _value->texto();
}

UI32 AUITextBox::textoLen() const {
	return _value->textoLen();
}

const STNBText* AUITextBox::getVisualTextDef() const {
	return _visual->getVisualTextDef();
}

UI32 AUITextBox::getVisualCharDefsCount() const {
	return _visual->getCharDefsCount();
}

UI32 AUITextBox::getVisualLinesCount() const {
	return _visual->getLinesCount();
}

void AUITextBox::establecerTexto(const char* valor){
	const STNBRect column = {0, 0, _boxWidth - _margenes.left - _margenes.right, this->altoParaLineasVisibles(_boxHeightInLines) };
	_value->establecerTextoEnCol(valor, column, _allowMultiline);
	{
		const UI32 charsLen = NBString_strLenUnicodes(valor);
		_value->setRngSelection(charsLen, 0);
		_value->setRngComposing(charsLen, 0);
	}
	this->privValueUpdated(TRUE, TRUE);
}

void AUITextBox::privValueUpdated(const BOOL valueChanged, const BOOL cursrChanged){
	const char* v = _value->texto();
	//Update visual if it's different
	if(_value != _visual){
		NBASSERT(_visual != NULL)
		NBASSERT(_passChar != NULL && _passCharSz > 0)
		if(_passChar != NULL && _passCharSz > 0){
			if(valueChanged){
				//Create visual text (repeating passChar)
				const UI32 unicodes		= NBString_strLenUnicodes(v);
				const STNBRect column	= {0, 0, _boxWidth - _margenes.left - _margenes.right, this->altoParaLineasVisibles(_boxHeightInLines) };
				_visual->establecerTextoRepitiendoEnCol(_passChar, unicodes, column, _allowMultiline);
			}
			//Sync ranges
			if(valueChanged || cursrChanged){
				const STNBRangeI sRng = _value->getRngSelection();
				const STNBRangeI cRng = _value->getRngComposing();
				_visual->setRngSelection(sRng);
				_visual->setRngComposing(cRng);
			}
		}
	}
	//Show/hide help text
	{
		BOOL isEmpty = TRUE;
		if(v != NULL){
			if(v[0] != '\0'){
				isEmpty = FALSE;
			}
		}
		_visualHelp->setVisibleAndAwake(isEmpty && !_atFocus);
	}
	//Listener
	if(valueChanged && _escuchadorCajaTxt != NULL){
		_escuchadorCajaTxt->textboxContenidoModificado(this, _value->texto());
	}
	//Update visual state
	this->setModelChangedFlag();
	//Move content
	this->privAsegurarCursorEstaVisible();
}

void AUITextBox::establecerTextoColor(const NBColor8 color){
	_visual->establecerMultiplicadorColor8(color);
}

void AUITextBox::establecerTextoColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	_visual->establecerMultiplicadorColor8(r, g, b, a);
}

void AUITextBox::establecerTextoAyuda(const char* valor){
	const STNBRect column = {0, 0, _boxWidth - _margenes.left - _margenes.right, this->altoParaLineasVisibles(_boxHeightInLines) };
	_visualHelp->establecerTextoEnCol(valor, column, _allowMultiline);
}

void AUITextBox::establecerTextoAyudaColor(const NBColor8 color){
	_visualHelp->establecerMultiplicadorColor8(color);
}

void AUITextBox::establecerTextoAyudaColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	_visualHelp->establecerMultiplicadorColor8(r, g, b, a);
}

float AUITextBox::altoParaLineasVisibles(const float lineasVisibles) const {
	return AUITextBox::altoParaLineasVisibles(lineasVisibles, _value->fuenteRender());
}

float AUITextBox::altoParaLineasVisibles(const float lineasVisibles, AUFuenteRender* fuenteRender) {
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

void AUITextBox::establecerTamanoCaja(const float anchoCaja, const float lineasVisibles){
	this->privEstablecerTamanoCaja(anchoCaja, lineasVisibles, FALSE);
}

void AUITextBox::privEstablecerTamanoCaja(const float anchoCaja, const float lineasVisibles, const BOOL force){
	if(_boxWidth != anchoCaja || _boxHeightInLines != lineasVisibles || force){
		NBASSERT(lineasVisibles > 0.0f)
		AUFuenteRender* fuenteRender	= _value->fuenteRender();
		const float altoParaTexto		= AUITextBox::altoParaLineasVisibles(lineasVisibles, fuenteRender);
		//Redimensionar subescena (si es necesario)
		NBTamanoI tamSubEscena;
		tamSubEscena.ancho	= (anchoCaja - _margenes.left - _margenes.right);
		tamSubEscena.alto	= altoParaTexto;
		if(tamSubEscena.ancho <= 0 || tamSubEscena.alto <= 0){
			if(_subscn.Sprite != NULL){
				AUEscenaContenedor* cont = (AUEscenaContenedor*)_subscn.Sprite->contenedor();
				if(cont != NULL) cont->quitarObjetoEscena(_subscn.Sprite);
				_subscn.Sprite->liberar(NB_RETENEDOR_THIS);
				_subscn.Sprite = NULL;
			}
		} else {
			if(_subscn.Id == -1){
				_subscn.Id = NBGestorEscena::crearEscena(tamSubEscena.ancho, tamSubEscena.alto, 1.0f, 1.0f, 72.0f, 72.0f, 72.0f, 72.0f, COLOR_RGBA8, ENGestorEscenaDestinoGl_Textura);
				if(this->idEscena == -1){
					NBGestorEscena::deshabilitarEscena(_subscn.Id);
				} else {
					NBGestorEscena::habilitarEscena(_subscn.Id);
				}
				NBColor8 colorAmb; NBCOLOR_ESTABLECER(colorAmb, 255, 255, 255, 255)
				NBGestorEscena::establecerColorFondo(_subscn.Id, (float)_subscn.colorBg.r / 255.0f , (float)_subscn.colorBg.g / 255.0f , (float)_subscn.colorBg.b / 255.0f , (float)_subscn.colorBg.a / 255.0f);
				NBGestorEscena::establecerFondoModo(_subscn.Id, ENGestorEscenaFondoModo_Opaco); //ENGestorEscenaFondoModo_Transparente
				NBGestorEscena::establecerEscenaLimpiaColorBuffer(_subscn.Id, true);
				NBGestorEscena::agregarObjetoCapa(_subscn.Id, ENGestorEscenaGrupo_Escena, _subscn.Contenedor, colorAmb);
				_subscn.Tamano			= tamSubEscena;
				_subscn.CajaBase.xMin	= 0;
				_subscn.CajaBase.xMax	= tamSubEscena.ancho;
				_subscn.CajaBase.yMin	= /*ascendente*/ -tamSubEscena.alto;
				_subscn.CajaBase.yMax	= 0 /*ascendente*/;
				NBGestorEscena::establecerCajaProyeccion(_subscn.Id, _subscn.CajaBase.xMin + _subscn.Scroll.ancho, _subscn.CajaBase.xMax + _subscn.Scroll.ancho, _subscn.CajaBase.yMin + _subscn.Scroll.alto, _subscn.CajaBase.yMax + _subscn.Scroll.alto); //float xMin, float xMax, float yMin, float yMax
				//PRINTF_INFO("Texbox (%llu) visual(%llu), caja proy escena: x(%f, %f)-y(%f, %f).\n", (UI64)this, (UI64)this->_visual, _subscn.CajaBase.xMin, _subscn.CajaBase.xMax, _subscn.CajaBase.yMin, _subscn.CajaBase.yMax);
			} else {
				const STGestorEscenaEscena props = NBGestorEscena::propiedadesEscena(_subscn.Id);
				if(props.puertoDeVision.ancho != tamSubEscena.ancho || props.puertoDeVision.alto != tamSubEscena.alto){
					NBTamano sameDpi; sameDpi.ancho = sameDpi.alto = 0;
					NBGestorEscena::redimensionarEscena(_subscn.Id, tamSubEscena.ancho, tamSubEscena.alto, 1.0f, sameDpi, sameDpi, 1.0f);
					_subscn.Tamano			= tamSubEscena;
					_subscn.CajaBase.xMin	= 0;
					_subscn.CajaBase.xMax	= tamSubEscena.ancho;
					_subscn.CajaBase.yMin	= /*ascendente */ -tamSubEscena.alto;
					_subscn.CajaBase.yMax	= 0/*ascendente*/;
					NBGestorEscena::establecerCajaProyeccion(_subscn.Id, _subscn.CajaBase.xMin + _subscn.Scroll.ancho, _subscn.CajaBase.xMax + _subscn.Scroll.ancho, _subscn.CajaBase.yMin + _subscn.Scroll.alto, _subscn.CajaBase.yMax + _subscn.Scroll.alto); //float xMin, float xMax, float yMin, float yMax
					//PRINTF_INFO("Texbox (%llu)visual(%llu), caja proy escena: x(%f, %f)-y(%f, %f).\n", (UI64)this, (UI64)this->_visual, _subscn.CajaBase.xMin, _subscn.CajaBase.xMax, _subscn.CajaBase.yMin, _subscn.CajaBase.yMax);
				}
			}
			const STGestorEscenaEscena propsEscena = NBGestorEscena::propiedadesEscena(_subscn.Id); NBASSERT(propsEscena.texturaEscena != NULL)
			if(_subscn.Sprite == NULL){
				_subscn.Sprite = new(this) AUEscenaSprite();
				this->agregarObjetoEscena(_subscn.Sprite);
			}
			_subscn.Sprite->establecerTextura(propsEscena.texturaEscena);
			_subscn.Sprite->redimensionar(_margenes.left, -(_margenes.top + tamSubEscena.alto), tamSubEscena.ancho, tamSubEscena.alto);
		}
		//Bg and limits
		{
			const STNBRect column	= { 0.0f, 0.0f, (float)tamSubEscena.ancho, (float)tamSubEscena.alto };
			const float bgWidth		= (_margenes.left + tamSubEscena.ancho + _margenes.right);
			const float bgHeight	= (_margenes.top + tamSubEscena.alto + _margenes.bottom);
			_bg->redimensionar(0, 0, bgWidth, -bgHeight);
			_visual->organizarTextoEnCol(column, _allowMultiline);
			_visualHelp->organizarTextoEnCol(column, _allowMultiline);
			if(_value != _visual){
				_value->organizarTextoEnCol(column, _allowMultiline);
			}
			this->establecerLimites(0.0f, bgWidth, -bgHeight, 0.0f);
		}
		//
		_boxWidth = anchoCaja;
		_boxHeightInLines = lineasVisibles;
		//NBASSERT(lineasVisibles < 2.0f)
		this->privOrganizeBtn();
	}
}

void AUITextBox::privOrganizeBtn(){
	if(_btn.icon != NULL){
		//Move
		if(_subscn.Sprite != NULL){
			const NBPunto scnPos = _subscn.Sprite->traslacion();
			const NBCajaAABB scnBox = _subscn.Sprite->cajaAABBLocal();
			const NBCajaAABB icoBox = _btn.icon->cajaAABBLocal();
			_btn.icon->establecerTraslacion(scnPos.x + scnBox.xMax - icoBox.xMax, scnPos.y + scnBox.yMax - icoBox.yMax - (((scnBox.yMax - scnBox.yMin) - (icoBox.yMax - icoBox.yMin)) * 0.5f));
		}
		//Visibility
		if(_btn.action == ENITextboxBtnAction_ResignFocus){
			_btn.icon->establecerVisible(_atFocus);
		} else {
			_btn.icon->establecerVisible(_atFocus);
		}
		//Set alpha (by touch)
		_btn.icon->establecerMultiplicadorAlpha8((((SI32)_btn.color.a * 3) / (_btn.touchDepth > 0 ? 4 : 3)));
	}
}

//

void AUITextBox::textEditorCloneInterfaceObjectsRetained(AUObjeto** dstVisualObj, ITextInputListener** dstVisualItf, AUObjeto** dstValueObj, ITextInputListener** dstValueItf){
	//
	if(dstVisualObj != NULL) *dstVisualObj = NULL;
	if(dstVisualItf != NULL) *dstVisualItf = NULL;
	if(dstValueObj != NULL) *dstValueObj = NULL;
	if(dstValueItf != NULL) *dstValueItf = NULL;
	//
	if(dstVisualObj != NULL || dstVisualItf != NULL){
		AUEscenaTextoEditableOffscreen* visualClone = new(this) AUEscenaTextoEditableOffscreen(_visual); //Return retained
		visualClone->setBehavior(_editorType, _allowMultiline);
		//Update scene props
		if(this->idEscena != -1){
			const STGestorEscenaEscena sceneProps = NBGestorEscena::propiedadesEscena(this->idEscena);
			visualClone->setSceneProps(this->_cacheObjEscena.matrizEscena, sceneProps.puertoDeVision, sceneProps.gruposCapas[this->idGrupo].cajaProyeccion);
		}
		if(dstVisualObj != NULL) *dstVisualObj = visualClone;
		if(dstVisualItf != NULL) *dstVisualItf = visualClone;
		//
		if(_value == _visual){
			visualClone->retener(NB_RETENEDOR_THIS); //Return both retained
			if(dstValueObj != NULL) *dstValueObj = visualClone;
			if(dstValueItf != NULL) *dstValueItf = visualClone;
		} else {
			AUEscenaTextoEditableOffscreen* valueClone = new(this) AUEscenaTextoEditableOffscreen(_value);
			valueClone->setBehavior(_editorType, _allowMultiline);
			//Update scene props
			if(this->idEscena != -1){
				const STGestorEscenaEscena sceneProps = NBGestorEscena::propiedadesEscena(this->idEscena);
				valueClone->setSceneProps(this->_cacheObjEscena.matrizEscena, sceneProps.puertoDeVision, sceneProps.gruposCapas[this->idGrupo].cajaProyeccion);
			}
			if(dstValueObj != NULL) *dstValueObj = valueClone;
			if(dstValueItf != NULL) *dstValueItf = valueClone;
		}
	}
	_valueTextSeqIdCloned	= _value->textoDataSeqId();
	_valueCursorSeqIdCloned	= _value->cursorDataSeqId();
}

STTextEditorSyncResult AUITextBox::privTextEditorCompareClones_(const AUITextBox* obj, const AUObjeto* pVisualClone, const AUObjeto* pValueClone){
	STTextEditorSyncResult r;
	NBMemory_setZeroSt(r, STTextEditorSyncResult);
	r.synced = ENTextEditorSyncResult_None;
	if(pVisualClone != NULL && pValueClone != NULL){
		NBASSERT(pVisualClone->esClase(AUEscenaTextoEditableOffscreen::idTipoClase))
		NBASSERT(pValueClone->esClase(AUEscenaTextoEditableOffscreen::idTipoClase))
		if(pVisualClone->esClase(AUEscenaTextoEditableOffscreen::idTipoClase) && pValueClone->esClase(AUEscenaTextoEditableOffscreen::idTipoClase)){
			//const AUEscenaTextoEditableOffscreen* visualClone = (const AUEscenaTextoEditableOffscreen*)pVisualClone;
			const AUEscenaTextoEditableOffscreen* valueClone = (const AUEscenaTextoEditableOffscreen*)pValueClone;
			BOOL valueChanged = FALSE; //from off to on-screen
			BOOL cursrChanged = FALSE; //from off to on-screen
			//-----------------------
			//-- Apply current state to clone
			//-----------------------
			if(r.synced == ENTextEditorSyncResult_None){
				const BOOL valueChanged = (obj->_valueTextSeqIdCloned != obj->_value->textoDataSeqId() ? TRUE : FALSE);
				const BOOL cursrChanged = (obj->_valueCursorSeqIdCloned != obj->_value->cursorDataSeqId() ? TRUE : FALSE);
				if(valueChanged || cursrChanged){
					/*PRINTF_INFO("Syncing from onscreen -> offscreen.\n");
					valueClone->syncDataWithOther(_value, valueChanged, (valueChanged || cursrChanged));
					if(visualClone != NULL && visualClone != valueClone){
						NBASSERT(_visual != _value)
						visualClone->syncDataWithOther(_visual, valueChanged, (valueChanged || cursrChanged));
					}
					_valueTextSeqIdCloned		= _value->textoDataSeqId();
					_valueCursorSeqIdCloned		= _value->cursorDataSeqId();*/
					r.synced					= ENTextEditorSyncResult_CloneSynced;
					r.valueChanged				= valueChanged;
					r.cursorChanged				= cursrChanged;
					//NBASSERT(valueClone->textoDataSeqId() == _value->textoDataSeqId() && valueClone->cursorDataSeqId() == _value->cursorDataSeqId())
				}
			}
			//-----------------------
			//-- Apply clone state as current
			//-----------------------
			if(r.synced == ENTextEditorSyncResult_None){
				valueChanged = (valueClone->textoDataSeqId() != obj->_value->textoDataSeqId());
				cursrChanged = (valueClone->cursorDataSeqId() != obj->_value->cursorDataSeqId());
				if(valueChanged || cursrChanged){
					//PRINTF_INFO("Syncing valueData from offscreen -> onscreen.\n");
					/*_value->syncDataWithOther(valueClone, valueChanged, (valueChanged || cursrChanged));
					_valueTextSeqIdCloned		= _value->textoDataSeqId();
					_valueCursorSeqIdCloned		= _value->cursorDataSeqId();*/
					r.synced					= ENTextEditorSyncResult_ListenerSynced;
					r.valueChanged				= valueChanged;
					r.cursorChanged				= cursrChanged;
					//NBASSERT(valueClone->textoDataSeqId() == _value->textoDataSeqId() && valueClone->cursorDataSeqId() == _value->cursorDataSeqId())
				}
			}
			/*
			//---------------------
			//-- Set new keyboard props
			//---------------------
			if(r.synced == ENTextEditorSyncResult_None || r.synced == ENTextEditorSyncResult_CloneSynced){
				//Sync multiline
				{
					if(valueClone->isMultiline() != _allowMultiline){
						valueClone->setMultiline(_allowMultiline);
						r.synced = ENTextEditorSyncResult_CloneSynced;
					}
					//Sync visual (if different)
					if(visualClone != NULL && visualClone != valueClone){
						if(visualClone->isMultiline() != _allowMultiline){
							visualClone->setMultiline(_allowMultiline);
						}
					}
				}
				//Sync editortype
				{
					if(valueClone->editorType() != _editorType){
						valueClone->setEditorType(_editorType);
						r.synced = ENTextEditorSyncResult_CloneSynced;
					}
					if(visualClone != NULL && visualClone != valueClone){
						if(visualClone->editorType() != _editorType){
							visualClone->setEditorType(_editorType);
						}
					}
				}
			}
			//-----------------------
			//-- Update scene props (always)
			//-----------------------
			if(this->idEscena != -1){
				NBMatriz sceneMatrix = this->_cacheObjEscena.matrizEscena;
				if(_subscn.Id != -1){
					const NBCajaAABB box = NBGestorEscena::cajaProyeccionGrupo(this->idEscena, ENGestorEscenaGrupo_Escena);
					NBMATRIZ_TRASLADAR(sceneMatrix, box.xMin, -box.yMax);
				}
				const STGestorEscenaEscena sceneProps = NBGestorEscena::propiedadesEscena(this->idEscena);
				valueClone->setSceneProps(sceneMatrix, sceneProps.puertoDeVision, sceneProps.gruposCapas[this->idGrupo].cajaProyeccion);
				//Sync visual (if different)
				if(visualClone != NULL && visualClone != valueClone){
					visualClone->setSceneProps(sceneMatrix, sceneProps.puertoDeVision, sceneProps.gruposCapas[this->idGrupo].cajaProyeccion);
				}
			}
			//Update visuals and notify (at end)
			if(valueChanged || cursrChanged){
				this->privValueUpdated(valueChanged, (valueChanged || cursrChanged));
				//Sync visual (if different)
				if(visualClone != NULL && visualClone != valueClone){
					//PRINTF_INFO("Syncing visualData from onscreen -> offscreen.\n");
					visualClone->syncDataWithOther(_visual, valueChanged, (valueChanged || cursrChanged));
				}
			}
			*/
		}
	}
	return r;
}
	
STTextEditorSyncResult AUITextBox::textEditorCompareClones(AUObjeto* pVisualClone, AUObjeto* pValueClone){
	return AUITextBox::privTextEditorCompareClones_(this, pVisualClone, pValueClone);
}

STTextEditorSyncResult AUITextBox::textEditorSyncClones(AUObjeto* pVisualClone, AUObjeto* pValueClone){
	STTextEditorSyncResult r;
	NBMemory_setZeroSt(r, STTextEditorSyncResult);
	r.synced = ENTextEditorSyncResult_None;
	if(pVisualClone != NULL && pValueClone != NULL){
		NBASSERT(pVisualClone->esClase(AUEscenaTextoEditableOffscreen::idTipoClase))
		NBASSERT(pValueClone->esClase(AUEscenaTextoEditableOffscreen::idTipoClase))
		if(pVisualClone->esClase(AUEscenaTextoEditableOffscreen::idTipoClase) && pValueClone->esClase(AUEscenaTextoEditableOffscreen::idTipoClase)){
			AUEscenaTextoEditableOffscreen* visualClone = (AUEscenaTextoEditableOffscreen*)pVisualClone;
			AUEscenaTextoEditableOffscreen* valueClone = (AUEscenaTextoEditableOffscreen*)pValueClone;
			BOOL valueChanged = FALSE; //from off to on-screen
			BOOL cursrChanged = FALSE; //from off to on-screen
			//-----------------------
			//-- Apply current state to clone
			//-----------------------
			if(r.synced == ENTextEditorSyncResult_None){
				const BOOL valueChanged = (_valueTextSeqIdCloned != _value->textoDataSeqId() ? TRUE : FALSE);
				const BOOL cursrChanged = (_valueCursorSeqIdCloned != _value->cursorDataSeqId() ? TRUE : FALSE);
				if(valueChanged || cursrChanged){
					PRINTF_INFO("Syncing from onscreen -> offscreen.\n");
					valueClone->syncDataWithOther(_value, valueChanged, (valueChanged || cursrChanged));
					if(visualClone != NULL && visualClone != valueClone){
						NBASSERT(_visual != _value)
						visualClone->syncDataWithOther(_visual, valueChanged, (valueChanged || cursrChanged));
					}
					_valueTextSeqIdCloned		= _value->textoDataSeqId();
					_valueCursorSeqIdCloned		= _value->cursorDataSeqId();
					r.synced					= ENTextEditorSyncResult_CloneSynced;
					r.valueChanged				= valueChanged;
					r.cursorChanged				= cursrChanged;
					//NBASSERT(valueClone->textoDataSeqId() == _value->textoDataSeqId() && valueClone->cursorDataSeqId() == _value->cursorDataSeqId())
				}
			}
			//-----------------------
			//-- Apply clone state as current
			//-----------------------
			if(r.synced == ENTextEditorSyncResult_None){
				valueChanged = (valueClone->textoDataSeqId() != _value->textoDataSeqId());
				cursrChanged = (valueClone->cursorDataSeqId() != _value->cursorDataSeqId());
				if(valueChanged || cursrChanged){
					//PRINTF_INFO("Syncing valueData from offscreen -> onscreen.\n");
					_value->syncDataWithOther(valueClone, valueChanged, (valueChanged || cursrChanged));
					_valueTextSeqIdCloned		= _value->textoDataSeqId();
					_valueCursorSeqIdCloned		= _value->cursorDataSeqId();
					r.synced					= ENTextEditorSyncResult_ListenerSynced;
					r.valueChanged				= valueChanged;
					r.cursorChanged				= cursrChanged;
					//NBASSERT(valueClone->textoDataSeqId() == _value->textoDataSeqId() && valueClone->cursorDataSeqId() == _value->cursorDataSeqId())
				}
			}
			//---------------------
			//-- Set new keyboard props
			//---------------------
			if(r.synced == ENTextEditorSyncResult_None || r.synced == ENTextEditorSyncResult_CloneSynced){
				//Sync multiline
				{
					if(valueClone->isMultiline() != _allowMultiline){
						valueClone->setMultiline(_allowMultiline);
						r.synced = ENTextEditorSyncResult_CloneSynced;
					}
					//Sync visual (if different)
					if(visualClone != NULL && visualClone != valueClone){
						if(visualClone->isMultiline() != _allowMultiline){
							visualClone->setMultiline(_allowMultiline);
						}
					}
				}
				//Sync editortype
				{
					if(valueClone->editorType() != _editorType){
						valueClone->setEditorType(_editorType);
						r.synced = ENTextEditorSyncResult_CloneSynced;
					}
					if(visualClone != NULL && visualClone != valueClone){
						if(visualClone->editorType() != _editorType){
							visualClone->setEditorType(_editorType);
						}
					}
				}
			}
			//-----------------------
			//-- Update scene props (always)
			//-----------------------
			if(this->idEscena != -1){
				NBMatriz sceneMatrix = this->_cacheObjEscena.matrizEscena;
				if(_subscn.Id != -1){
					const NBCajaAABB box = NBGestorEscena::cajaProyeccionGrupo(this->idEscena, ENGestorEscenaGrupo_Escena);
					NBMATRIZ_TRASLADAR(sceneMatrix, box.xMin, -box.yMax);
				}
				const STGestorEscenaEscena sceneProps = NBGestorEscena::propiedadesEscena(this->idEscena);
				valueClone->setSceneProps(sceneMatrix, sceneProps.puertoDeVision, sceneProps.gruposCapas[this->idGrupo].cajaProyeccion);
				//Sync visual (if different)
				if(visualClone != NULL && visualClone != valueClone){
					visualClone->setSceneProps(sceneMatrix, sceneProps.puertoDeVision, sceneProps.gruposCapas[this->idGrupo].cajaProyeccion);
				}
			}
			//Update visuals and notify (at end)
			if(valueChanged || cursrChanged){
				this->privValueUpdated(valueChanged, (valueChanged || cursrChanged));
				//Sync visual (if different)
				if(visualClone != NULL && visualClone != valueClone){
					//PRINTF_INFO("Syncing visualData from onscreen -> offscreen.\n");
					visualClone->syncDataWithOther(_visual, valueChanged, (valueChanged || cursrChanged));
				}
			}
		}
	}
	return r;
}

//

void AUITextBox::textEditorFocusGained(){
	NBASSERT(!_atFocus)
	_atFocus = true;
	_visual->setAnimatingCursor(_atFocus);
	_visualHelp->setVisibleAndAwake(NBString_strIsEmpty(_value->texto()) && !_atFocus);
	//
	if(_btn.icon != NULL){
		this->privOrganizeBtn();
	}
	//
	if(_escuchadorCajaTxt != NULL){
		_escuchadorCajaTxt->textboxFocoObtenido(this);
	}
}

void AUITextBox::textEditorFocusLost(const ENNBKeybFocusLostType actionType){
	NBASSERT(_atFocus)
	_atFocus = false;
	_visual->setAnimatingCursor(_atFocus);
	_visualHelp->setVisibleAndAwake(NBString_strIsEmpty(_value->texto()) && !_atFocus);
	//
	if(_btn.icon != NULL){
		this->privOrganizeBtn();
	}
	//
	if(_escuchadorCajaTxt != NULL){
		_escuchadorCajaTxt->textboxFocoPerdido(this, actionType);
	}
}

bool AUITextBox::textEditorMantainBeforeTouchConsumer(AUEscenaObjeto* consumidorTouch){
	bool r = (consumidorTouch != NULL && (consumidorTouch == this || consumidorTouch == _bg || consumidorTouch == _btn.icon));
	if(!r && _escuchadorCajaTxt != NULL){
		r = _escuchadorCajaTxt->textboxMantenerFocoAnteConsumidorDeTouch(this, consumidorTouch);
	}
	return r;
}


//TECLADO

void AUITextBox::privAsegurarCursorEstaVisible(){
	BOOL scrollAdjusted		= FALSE;
	const STNBRect rCursor	= _visual->rectForCursorCurrent();
	//PRINTF_INFO("Asegurando cursor rect(%f, %f)-(+%f, +%f).\n", rCursor.x, rCursor.y, rCursor.width, rCursor.height);
	NBASSERT(rCursor.width >= 0.0f && rCursor.height >= 0.0f)
	if((rCursor.x + rCursor.width) > (_subscn.CajaBase.xMax + _subscn.Scroll.ancho)){
		_subscn.Scroll.ancho	= (rCursor.x + rCursor.width) - _subscn.CajaBase.xMax;
		scrollAdjusted			= TRUE;
	} else if(rCursor.x < (_subscn.CajaBase.xMin + _subscn.Scroll.ancho)){
		_subscn.Scroll.ancho	= rCursor.x - _subscn.CajaBase.xMin;
		scrollAdjusted			= TRUE;
	}
	if((rCursor.y + rCursor.height) > (_subscn.CajaBase.yMax + _subscn.Scroll.alto)){
		_subscn.Scroll.alto	= (rCursor.y + rCursor.height) - _subscn.CajaBase.yMax;
		scrollAdjusted			= TRUE;
	} else if(rCursor.y < (_subscn.CajaBase.yMin + _subscn.Scroll.alto)){
		_subscn.Scroll.alto	= rCursor.y - _subscn.CajaBase.yMin;
		scrollAdjusted			= TRUE;
	}
	if(scrollAdjusted && _subscn.Id != -1){
		NBGestorEscena::establecerCajaProyeccion(_subscn.Id, _subscn.CajaBase.xMin + _subscn.Scroll.ancho, _subscn.CajaBase.xMax + _subscn.Scroll.ancho, _subscn.CajaBase.yMin + _subscn.Scroll.alto, _subscn.CajaBase.yMax + _subscn.Scroll.alto); //float xMin, float xMax, float yMin, float yMax
	}
}

//TOUCHES

void AUITextBox::touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto){
	if(objeto == _btn.icon){
		_btn.touchDepth++;
		_btn.icon->establecerMultiplicadorAlpha8((((SI32)_btn.color.a * 3) / (_btn.touchDepth > 0 ? 4 : 3)));
	} else if(objeto == _bg){
		if(_atFocus && _subscn.Sprite != NULL){
			NBASSERT(_subscn.Id != -1)
			const NBCajaAABB box	= _subscn.Sprite->cajaAABBLocal();
			const NBPunto posLocal	= _subscn.Sprite->coordenadaEscenaALocal(posTouchEscena.x, posTouchEscena.y);
			const float xSubport	= (posLocal.x - box.xMin);
			const float ySubport	= (box.yMax - posLocal.y);
			const NBPunto posSubscn	= NBGestorEscena::coordenadaPuertoAGrupo(_subscn.Id, ENGestorEscenaGrupo_Escena, xSubport, ySubport);
			const NBPunto visualPos	= _visual->coordenadaEscenaALocal(posSubscn.x, posSubscn.y);
			_visual->dragginStart((const UI64)touch, visualPos.x, visualPos.y);
			//Update ranges (posibly updated by touch)
			if(_visual != _value){
				const STNBRangeI sRng = _visual->getRngSelection();
				const STNBRangeI cRng = _visual->getRngComposing();
				_value->setRngSelection(sRng);
				_value->setRngComposing(cRng);
			}
		}
		if(!_atFocus){
			//Update selection (at touched char)
			{
				const NBCajaAABB box	= _subscn.Sprite->cajaAABBLocal();
				const NBPunto posLocal	= _subscn.Sprite->coordenadaEscenaALocal(posTouchEscena.x, posTouchEscena.y);
				const float xSubport	= (posLocal.x - box.xMin);
				const float ySubport	= (box.yMax - posLocal.y);
				const NBPunto posSubscn	= NBGestorEscena::coordenadaPuertoAGrupo(_subscn.Id, ENGestorEscenaGrupo_Escena, xSubport, ySubport);
				const NBPunto visualPos	= _visual->coordenadaEscenaALocal(posSubscn.x, posSubscn.y);
				const SI32 iChar		= _visual->getClosestCharIdx(visualPos.x, visualPos.y);
				_visual->setRngSelection(iChar, 0);
				_visual->setRngComposing(iChar, 0);
				if(_value != _visual){
					_value->setRngSelection(iChar, 0);
					_value->setRngComposing(iChar, 0);
				}
				//Move content
				this->privAsegurarCursorEstaVisible();
			}
			//Obtain focus
			NBGestorTeclas::escuchadorEstablecer(this, this);
		}
	}
}

void AUITextBox::touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	if(objeto == _bg){
		if(_atFocus && _subscn.Sprite != NULL){
			NBASSERT(_subscn.Id != -1)
			const NBCajaAABB box	= _subscn.Sprite->cajaAABBLocal();
			const NBPunto posLocal	= _subscn.Sprite->coordenadaEscenaALocal(posActualEscena.x, posActualEscena.y);
			const float xSubport	= (posLocal.x - box.xMin);
			const float ySubport	= (box.yMax - posLocal.y);
			const NBPunto posSubscn	= NBGestorEscena::coordenadaPuertoAGrupo(_subscn.Id, ENGestorEscenaGrupo_Escena, xSubport, ySubport);
			const NBPunto visualPos	= _visual->coordenadaEscenaALocal(posSubscn.x, posSubscn.y);
			_visual->dragginContinue((const UI64)touch, visualPos.x, visualPos.y);
			//Update ranges (posibly updated by touch)
			if(_visual != _value){
				const STNBRangeI sRng = _visual->getRngSelection();
				const STNBRangeI cRng = _visual->getRngComposing();
				_value->setRngSelection(sRng);
				_value->setRngComposing(cRng);
			}
			//Move content
			this->privAsegurarCursorEstaVisible();
		}
	}
}

void AUITextBox::touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	if(objeto == _btn.icon){
		_btn.touchDepth--; if(_btn.touchDepth < 0) _btn.touchDepth = 0;
		_btn.icon->establecerMultiplicadorAlpha8((((SI32)_btn.color.a * 3) / (_btn.touchDepth > 0 ? 4 : 3)));
	} else if(objeto == _bg){
		if(_atFocus && _subscn.Sprite != NULL){
			NBASSERT(_subscn.Id != -1)
			const NBCajaAABB box	= _subscn.Sprite->cajaAABBLocal();
			const NBPunto posLocal	= _subscn.Sprite->coordenadaEscenaALocal(posActualEscena.x, posActualEscena.y);
			const float xSubport	= (posLocal.x - box.xMin);
			const float ySubport	= (box.yMax - posLocal.y);
			const NBPunto posSubscn	= NBGestorEscena::coordenadaPuertoAGrupo(_subscn.Id, ENGestorEscenaGrupo_Escena, xSubport, ySubport);
			const NBPunto visualPos	= _visual->coordenadaEscenaALocal(posSubscn.x, posSubscn.y);
			_visual->dragginEnd((const UI64)touch, visualPos.x, visualPos.y, touch->cancelado);
			//Update ranges (posibly updated by touch)
			if(_visual != _value){
				const STNBRangeI sRng = _visual->getRngSelection();
				const STNBRangeI cRng = _visual->getRngComposing();
				_value->setRngSelection(sRng);
				_value->setRngComposing(cRng);
			}
			//Move content
			this->privAsegurarCursorEstaVisible();
		}
	}
	//
	const NBCajaAABB cajaEscena = objeto->cajaAABBEnEscena();
	if(NBCAJAAABB_INTERSECTA_PUNTO(cajaEscena, posActualEscena.x, posActualEscena.y)){
		if(objeto == _btn.icon){
			switch(_btn.action) {
				case ENITextboxBtnAction_Clear:
					if(_value->textoLen() > 0){
						this->establecerTexto("");
					} else {
						this->focusRelease();
					}
					break;
				case ENITextboxBtnAction_ResignFocus:
					this->focusRelease();
					break;
				default:
					break;
			}
		} else if(objeto == _bg){
			if(!_atFocus){
				//
			}
		}
	}
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUITextBox, AUEscenaContenedor)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUITextBox, "AUITextBox", AUEscenaContenedor)
AUOBJMETODOS_CLONAR_NULL(AUITextBox)





