//
//  AUIButton.cpp
//  app-refranero
//
//  Created by Marcos Ortega on 14/03/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUIButton.h"
#include "NBGestorAnimadores.h"
#include "NBGestorEscena.h"
#include "AUEscenaTexto.h"
//
#define AUIButton_FONDO_MASK_VISIBLE	1
#define AUIButton_FONDO_MASK_INVERTIR_H	2
#define AUIButton_FONDO_MASK_INVERTIR_V	4

AUIButton::AUIButton() : AUEscenaContenedor()
	, _escuchadorBoton(NULL)
	, _touchesCount(0)
	, _hoversCount(0)
	//
	, _fondoOpciones(0)
	, _estadoActual(ENIBotonItemState_Normal)
	, _estadoPresentado(ENIBotonItemState_Normal)
	//Animacion de estados
	, _maskAnimacionEstado(0)
	, _segsRetrasarOnTouch(0.20f)
	, _segsParaLongTouch(0.0f)
	, _segsAcumColorOnTouch(0.0f)
	//
	, _contenedor(NULL)
	, _fondo(NULL)
	, _items(this)
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUIButton")
	//
	NBString_init(&_refOptionId);
	_refDatos.tipo		= 0;
	_refDatos.valor		= 0;
	//
	{
		NBMemory_setZero(_margins);
		_marginI = 4.0f;
	}
	//
	_contenedor = new(this) AUEscenaContenedorLimitado(); NB_DEFINE_NOMBRE_PUNTERO(_contenedor, "AUIButton::_contenedor")
	_contenedor->establecerEscuchadorTouches(this, this);
	this->agregarObjetoEscena(_contenedor);
	//
	{
		SI32 i;
		for(i = 0; i < ENIBotonItemState_Count; i++){
			NBCOLOR_ESTABLECER(_fondoColors[i],	255, 255, 255, 255);
		}
	}
}

AUIButton::~AUIButton(){
	NBString_release(&_refOptionId);
	if((_maskAnimacionEstado & AUIButton_MASK_AGREGADO_ANIMADOR) != 0){
		NBGestorAnimadores::quitarAnimador(this);
	}
	if(_contenedor != NULL) _contenedor->liberar(NB_RETENEDOR_THIS); _contenedor = NULL;
	if(_fondo != NULL) _fondo->liberar(NB_RETENEDOR_THIS); _fondo = NULL;
	//
	{
		SI32 i; const SI32 count = _items.conteo;
		for(i = 0; i < count; i++){
			STIBotonItem* itm = (STIBotonItem*)_items.elemPtr(i);
			if(itm->obj != NULL) itm->obj->liberar(NB_RETENEDOR_THIS);
			if(itm->type == ENIBotonItemType_TextMultiline){
				NBASSERT(itm->extraData != NULL)
				if(itm->extraData != NULL){
					free(itm->extraData);
					itm->extraData = NULL;
				}
			} else {
				NBASSERT(itm->extraData == NULL)
			}
		}
		_items.vaciar();
	}
}

//

IEscuchadorIBoton* AUIButton::escuchadorBoton() const {
	return _escuchadorBoton;
}

void AUIButton::establecerEscuchadorBoton(IEscuchadorIBoton* escuchador){
	_escuchadorBoton = escuchador;
}

//

NBCajaAABB AUIButton::limitesParaContenido() const{
	NBCajaAABB r;
	//
	NBTamano tamFondo; NBTAMANO_ESTABLECER(tamFondo, 0.0f, 0.0f)
	if(_fondo != NULL){
		const AUTextura* texFondo = _fondo->textura();
		if(texFondo != NULL){
			tamFondo = texFondo->tamanoHD();
		}
	}
	//
	float altoMaximo = 0.0f;
	float anchoPorAlinea[ENIBotonItemAlign_Count];
	SI32 itemsPorAlinea[ENIBotonItemAlign_Count];
	{
		SI32 i;
		for(i = 0; i < ENIBotonItemAlign_Count; i++){
			anchoPorAlinea[i] = 0.0f;
			itemsPorAlinea[i] = 0;
		}
		const SI32 count = _items.conteo;
		for(i = 0; i < count; i++){
			STIBotonItem* itm = _items.elemPtr(i); NBASSERT(itm->obj->contenedor() == _contenedor)
			NBCajaAABB cajaAABB;
			if(itm->type == ENIBotonItemType_Icon){
				cajaAABB = itm->obj->cajaAABBLocal();
			} else if(itm->type == ENIBotonItemType_TextMonoline){
				AUFuenteRender* fuente = ((AUEscenaTexto*)itm->obj)->fuenteRender();
				cajaAABB = itm->obj->cajaAABBLocal();
				cajaAABB.yMax = fuente->ascendenteEscena();
				cajaAABB.yMin = -fuente->descendenteEscena();
			} else {
				cajaAABB = itm->obj->cajaAABBLocal();
			}
			NBASSERT(cajaAABB.xMax >= cajaAABB.xMin)
			NBASSERT(cajaAABB.yMax >= cajaAABB.yMin)
			const float ancho = cajaAABB.xMax - cajaAABB.xMin;
			const float alto = cajaAABB.yMax - cajaAABB.yMin;
			if(altoMaximo < alto){ altoMaximo = alto; }
			if(ancho > 0.0f){
				anchoPorAlinea[itm->align] += ancho;
				itemsPorAlinea[itm->align]++;
			}
		}
		//Match height with backgorund texture (if bigger)
		if(_fondo != NULL){
			if((_margins.top + altoMaximo + _margins.bottom) < tamFondo.alto){
				altoMaximo = tamFondo.alto - _margins.top - _margins.bottom;
			}
		}
	}
	//autosize
	r.yMin	= r.xMin = 0.0f;
	r.yMax	= _margins.top + altoMaximo + _margins.bottom;
	r.xMax	= _margins.left + _margins.right
			+ anchoPorAlinea[ENIBotonItemAlign_Left] + (itemsPorAlinea[ENIBotonItemAlign_Left] == 0 ? 0.0f : (itemsPorAlinea[ENIBotonItemAlign_Left] - 1) * _marginI)
			+ anchoPorAlinea[ENIBotonItemAlign_Right] + (itemsPorAlinea[ENIBotonItemAlign_Right] == 0 ? 0.0f : (itemsPorAlinea[ENIBotonItemAlign_Right] - 1) * _marginI)
			+ anchoPorAlinea[ENIBotonItemAlign_Center] + (itemsPorAlinea[ENIBotonItemAlign_Center] == 0 ? 0.0f : (itemsPorAlinea[ENIBotonItemAlign_Center] - 1) * _marginI)
			+ (((itemsPorAlinea[ENIBotonItemAlign_Left] != 0 && itemsPorAlinea[ENIBotonItemAlign_Center] != 0) || (itemsPorAlinea[ENIBotonItemAlign_Left] != 0 && itemsPorAlinea[ENIBotonItemAlign_Right] != 0)) ? _marginI : 0.0f)
			+ ((itemsPorAlinea[ENIBotonItemAlign_Center] != 0 && itemsPorAlinea[ENIBotonItemAlign_Right] != 0) ? _marginI : 0.0f);
	if(_fondo != NULL){
		if(r.xMax < tamFondo.ancho){
			r.xMax = tamFondo.ancho;
		}
	}
	//
	return r;
}

bool AUIButton::isInternalVisualObject(AUEscenaObjeto* obj) const {
	return (_contenedor == obj);
}

AUEscenaContenedor* AUIButton::getTouchLayer(){
	return _contenedor;
}

//

AUEscenaObjeto* AUIButton::itemObjetoEscena(){
	return this;
}

const char* AUIButton::refOptionId() const {
	return _refOptionId.str;
}

STListaItemDatos AUIButton::refDatos() {
	return _refDatos;
}

void AUIButton::establecerOptionId(const char* optionId){
	NBString_set(&_refOptionId, optionId);
}

void AUIButton::establecerRefDatos(const SI32 tipo, const SI32 valor){
	_refDatos.tipo	= tipo;
	_refDatos.valor	= valor;
}

//

NBMargenes AUIButton::margenes() const {
	return _margins;
}

float AUIButton::margenI() const {
	return _marginI;
}

void AUIButton::establecerMargenes(const float mIzq, const float mDer, const float mSup, const float mInf, const float mInternal){
	_margins.left		= mIzq;
	_margins.right		= mDer;
	_margins.top		= mSup;
	_margins.bottom		= mInf;
	_marginI			= mInternal;
	//Organize
	{
		const NBCajaAABB limites = _contenedor->limites();
		this->organizarContenido((limites.xMax - limites.xMin), 0.0f, true, NULL, 0.0f);
	}
}

void AUIButton::establecerMargenes(const NBMargenes margins, const float mInternal){
	_margins	= margins;
	_marginI	= mInternal;
	//Organize
	{
		const NBCajaAABB limites = _contenedor->limites();
		this->organizarContenido((limites.xMax - limites.xMin), 0.0f, true, NULL, 0.0f);
	}
}

void AUIButton::establecerFondo(const bool mostrarFondo, AUTextura* texFondo){
	if(!mostrarFondo){
		_fondoOpciones &= ~AUIButton_FONDO_MASK_VISIBLE;
		if(_fondo != NULL){
			AUEscenaContenedor* parent = (AUEscenaContenedor*)_fondo->contenedor();
			if(parent != NULL) parent->quitarObjetoEscena(_fondo);
			_fondo->liberar(NB_RETENEDOR_THIS); _fondo = NULL;
		}
	} else {
		_fondoOpciones |= AUIButton_FONDO_MASK_VISIBLE;
		if(_fondo == NULL){
			_fondo = new(this) AUEscenaSpriteElastico();
			_contenedor->agregarObjetoEscenaEnIndice(_fondo, 0);
		}
		_fondo->establecerMultiplicadorColor8(_fondoColors[_estadoPresentado]);
		_fondo->establecerTextura(texFondo);
	}
}

void AUIButton::establecerFondoColores(const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	//
	_fondoColors[ENIBotonItemState_Normal].r = r;
	_fondoColors[ENIBotonItemState_Normal].g = g;
	_fondoColors[ENIBotonItemState_Normal].b = b;
	_fondoColors[ENIBotonItemState_Normal].a = a;
	//
	_fondoColors[ENIBotonItemState_Hover]	= _fondoColors[ENIBotonItemState_Normal];
	_fondoColors[ENIBotonItemState_Touched]	= _fondoColors[ENIBotonItemState_Normal];
	//
	if(_fondo != NULL){
		_fondo->establecerMultiplicadorColor8(_fondoColors[_estadoPresentado]);
	}
}

void AUIButton::establecerFondoColores(const NBColor8 normal, const NBColor8 hover, const NBColor8 touched){
	_fondoColors[ENIBotonItemState_Normal]	= normal;
	_fondoColors[ENIBotonItemState_Hover]	= hover;
	_fondoColors[ENIBotonItemState_Touched]	= touched;
	if(_fondo != NULL){
		_fondo->establecerMultiplicadorColor8(_fondoColors[_estadoPresentado]);
	}
}

void AUIButton::establecerFondoInvertir(const bool invertirHorizontal, const bool invertirVertical){
	if(invertirHorizontal){
		_fondoOpciones |= AUIButton_FONDO_MASK_INVERTIR_H;
	} else {
		_fondoOpciones &= ~AUIButton_FONDO_MASK_INVERTIR_H;
	}
	if(invertirVertical){
		_fondoOpciones |= AUIButton_FONDO_MASK_INVERTIR_V;
	} else {
		_fondoOpciones &= AUIButton_FONDO_MASK_INVERTIR_V;
	}
}

//

float AUIButton::segsRetrasarOnTouch() const {
	return _segsRetrasarOnTouch;
}

float AUIButton::segsParaLongTouch() const {
	return _segsParaLongTouch;
}

void AUIButton::establecerSegsRetrasarOnTouch( const float segsRetrasarOnTouch){
	_segsRetrasarOnTouch = segsRetrasarOnTouch;
}

void AUIButton::establecerSegsParaLongTouch(const float segsParaLongTouch){
	_segsParaLongTouch = segsParaLongTouch;
}

//

AUEscenaObjeto* AUIButton::subElementoPorTipo(const SI32 refTipo) const {
	AUEscenaObjeto* r = NULL;
	SI32 i; const SI32 count = _items.conteo;
	for(i = 0; i < count; i++){
		STIBotonItem* itm = _items.elemPtr(i);
		if(itm->refUserData.tipo == refTipo){
			r = itm->obj;
		}
	}
	return r;
}

//

void AUIButton::agregarObjeto(AUEscenaObjeto* obj, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor){
	this->agregarObjetoResizable(obj, NULL, NBST_P(STNBSize, 0.0f, 0.0f), align, vAlignRel, refTipo, refValor, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255);
}

void AUIButton::agregarObjeto(AUEscenaObjeto* obj, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 color){
	this->agregarObjetoResizable(obj, NULL, NBST_P(STNBSize, 0.0f, 0.0f), align, vAlignRel, refTipo, refValor, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a);
}

void AUIButton::agregarObjeto(AUEscenaObjeto* obj, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 colorNormal, const NBColor8 colorHover, const NBColor8 colorTouched){
	this->agregarObjetoResizable(obj, NULL, NBST_P(STNBSize, 0.0f, 0.0f), align, vAlignRel, refTipo, refValor, colorNormal.r, colorNormal.g, colorNormal.b, colorNormal.a, colorHover.r, colorHover.g, colorHover.b, colorHover.a, colorTouched.r, colorTouched.g, colorTouched.b, colorTouched.a);
}

void AUIButton::agregarObjeto(AUEscenaObjeto* obj, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	this->agregarObjetoResizable(obj, NULL, NBST_P(STNBSize, 0.0f, 0.0f), align, vAlignRel, refTipo, refValor, r, g, b, a, r, g, b, a, r, g, b, a);
}

void AUIButton::agregarObjeto(AUEscenaObjeto* obj, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 rN, const UI8 gN, const UI8 bN, const UI8 aN, const UI8 rH, const UI8 gH, const UI8 bH, const UI8 aH, const UI8 rT, const UI8 gT, const UI8 bT, const UI8 aT){
	this->agregarObjetoResizable(obj, NULL, NBST_P(STNBSize, 0.0f, 0.0f), align, vAlignRel, refTipo, refValor, rN, gN, bN, aN, rH, gH, bH, aH, rT, gT, bT, aT);
}

//

void AUIButton::agregarObjetoResizable(AUEscenaObjeto* obj, AUEscenaListaItemI* itf, const STNBSize itfRelSz, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor){
	this->agregarObjetoResizable(obj, itf, itfRelSz, align, vAlignRel, refTipo, refValor, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255);
}

void AUIButton::agregarObjetoResizable(AUEscenaObjeto* obj, AUEscenaListaItemI* itf, const STNBSize itfRelSz, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 color){
	this->agregarObjetoResizable(obj, itf, itfRelSz, align, vAlignRel, refTipo, refValor, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a);
}

void AUIButton::agregarObjetoResizable(AUEscenaObjeto* obj, AUEscenaListaItemI* itf, const STNBSize itfRelSz, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 colorNormal, const NBColor8 colorHover, const NBColor8 colorTouched){
	this->agregarObjetoResizable(obj, itf, itfRelSz, align, vAlignRel, refTipo, refValor, colorNormal.r, colorNormal.g, colorNormal.b, colorNormal.a, colorHover.r, colorHover.g, colorHover.b, colorHover.a, colorTouched.r, colorTouched.g, colorTouched.b, colorTouched.a);
}

void AUIButton::agregarObjetoResizable(AUEscenaObjeto* obj, AUEscenaListaItemI* itf, const STNBSize itfRelSz, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	this->agregarObjetoResizable(obj, itf, itfRelSz, align, vAlignRel, refTipo, refValor, r, g, b, a, r, g, b, a, r, g, b, a);
}

void AUIButton::agregarObjetoResizable(AUEscenaObjeto* obj, AUEscenaListaItemI* itf, const STNBSize itfRelSz, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 rN, const UI8 gN, const UI8 bN, const UI8 aN, const UI8 rH, const UI8 gH, const UI8 bH, const UI8 aH, const UI8 rT, const UI8 gT, const UI8 bT, const UI8 aT){
	NBASSERT(obj != NULL && align >= 0 && align < ENIBotonItemAlign_Count)
	if(obj != NULL && align >= 0 && align < ENIBotonItemAlign_Count){
		STIBotonItem itm;
		NBASSERT(ENIBotonItemState_Count == 3)
		NBCOLOR_ESTABLECER(itm.colors[ENIBotonItemState_Normal], rN, gN, bN, aN);
		NBCOLOR_ESTABLECER(itm.colors[ENIBotonItemState_Hover], rH, gH, bH, aH);
		NBCOLOR_ESTABLECER(itm.colors[ENIBotonItemState_Touched], rT, gT, bT, aT);
		itm.obj			= obj; obj->retener(NB_RETENEDOR_THIS);
		itm.obj->establecerMultiplicadorColor8(itm.colors[_estadoActual]);
		itm.type		= ENIBotonItemType_Object;
		itm.align		= align;
		itm.valign		= vAlignRel;
		//Autoresize
		itm.itf			= itf;		//itf to automatically resize object
		itm.itfRelSz	= itfRelSz;	//rel size when resizing
		//
		itm.extraData	= NULL;
		itm.refUserData.tipo = refTipo;
		itm.refUserData.valor = refValor;
		_items.agregarElemento(itm);
		_contenedor->agregarObjetoEscena(itm.obj);
		//
		const NBCajaAABB limites = _contenedor->limites();
		this->organizarContenido((limites.xMax - limites.xMin), 0.0f, false, NULL, 0.0f);
	}
}


void AUIButton::agregarIcono(AUTextura* tex, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor){
	this->agregarIcono(tex, align, vAlignRel, refTipo, refValor, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255);
}

void AUIButton::agregarIcono(AUTextura* tex, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 color){
	this->agregarIcono(tex, align, vAlignRel, refTipo, refValor, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a);
}

void AUIButton::agregarIcono(AUTextura* tex, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 colorNormal, const NBColor8 colorHover, const NBColor8 colorTouched){
	this->agregarIcono(tex, align, vAlignRel, refTipo, refValor, colorNormal.r, colorNormal.g, colorNormal.b, colorNormal.a, colorHover.r, colorHover.g, colorHover.b, colorHover.a, colorTouched.r, colorTouched.g, colorTouched.b, colorTouched.a);
}

void AUIButton::agregarIcono(AUTextura* tex, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	this->agregarIcono(tex, align, vAlignRel, refTipo, refValor, r, g, b, a, r, g, b, a, r, g, b, a);
}

void AUIButton::agregarIcono(AUTextura* tex, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 rN, const UI8 gN, const UI8 bN, const UI8 aN, const UI8 rH, const UI8 gH, const UI8 bH, const UI8 aH, const UI8 rT, const UI8 gT, const UI8 bT, const UI8 aT){
	NBASSERT(tex != NULL && align >= 0 && align < ENIBotonItemAlign_Count)
	if(tex != NULL && align >= 0 && align < ENIBotonItemAlign_Count){
		STIBotonItem itm;
		NBASSERT(ENIBotonItemState_Count == 3)
		NBCOLOR_ESTABLECER(itm.colors[ENIBotonItemState_Normal], rN, gN, bN, aN);
		NBCOLOR_ESTABLECER(itm.colors[ENIBotonItemState_Hover], rH, gH, bH, aH);
		NBCOLOR_ESTABLECER(itm.colors[ENIBotonItemState_Touched], rT, gT, bT, aT);
		itm.obj			= new(this) AUEscenaSprite(tex);
		itm.obj->establecerMultiplicadorColor8(itm.colors[_estadoActual]);
		itm.type		= ENIBotonItemType_Icon;
		itm.align		= align;
		itm.valign		= vAlignRel;
		//Autoresize
		itm.itf			= NULL;		//itf to automatically resize object
		itm.itfRelSz	= NBST_P(STNBSize, 0.0f, 0.0f);	//rel size when resizing
		//
		itm.extraData	= NULL;
		itm.refUserData.tipo = refTipo;
		itm.refUserData.valor = refValor;
		_items.agregarElemento(itm);
		_contenedor->agregarObjetoEscena(itm.obj);
		//
		const NBCajaAABB limites = _contenedor->limites();
		this->organizarContenido((limites.xMax - limites.xMin), 0.0f, false, NULL, 0.0f);
	}
}

void AUIButton::agregarTextoMonolinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor){
	this->agregarTextoMonolinea(fuente, str, align, vAlignRel, refTipo, refValor, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255);
}

void AUIButton::agregarTextoMonolinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 color){
	this->agregarTextoMonolinea(fuente, str, align, vAlignRel, refTipo, refValor, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a);
}

void AUIButton::agregarTextoMonolinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 colorNormal, const NBColor8 colorHover, const NBColor8 colorTouched){
	this->agregarTextoMonolinea(fuente, str, align, vAlignRel, refTipo, refValor, colorNormal.r, colorNormal.g, colorNormal.b, colorNormal.a, colorHover.r, colorHover.g, colorHover.b, colorHover.a, colorTouched.r, colorTouched.g, colorTouched.b, colorTouched.a);
}

void AUIButton::agregarTextoMonolinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	this->agregarTextoMonolinea(fuente, str, align, vAlignRel, refTipo, refValor, r, g, b, a, r, g, b, a, r, g, b, a);
}

void AUIButton::agregarTextoMonolinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 rN, const UI8 gN, const UI8 bN, const UI8 aN, const UI8 rH, const UI8 gH, const UI8 bH, const UI8 aH, const UI8 rT, const UI8 gT, const UI8 bT, const UI8 aT){
	NBASSERT(fuente != NULL && align >= 0 && align < ENIBotonItemAlign_Count)
	if(fuente != NULL && align >= 0 && align < ENIBotonItemAlign_Count){
		AUEscenaTexto* escTxt = new(this) AUEscenaTexto(fuente);
		escTxt->establecerAlineaciones(ENNBTextLineAlignH_Base, ENNBTextAlignV_Base);
		escTxt->establecerTexto(str);
		//
		STIBotonItem itm;
		NBASSERT(ENIBotonItemState_Count == 3)
		NBCOLOR_ESTABLECER(itm.colors[ENIBotonItemState_Normal], rN, gN, bN, aN);
		NBCOLOR_ESTABLECER(itm.colors[ENIBotonItemState_Hover], rH, gH, bH, aH);
		NBCOLOR_ESTABLECER(itm.colors[ENIBotonItemState_Touched], rT, gT, bT, aT);
		itm.obj			= escTxt;
		itm.obj->establecerMultiplicadorColor8(itm.colors[_estadoActual]);
		itm.type		= ENIBotonItemType_TextMonoline;
		itm.align		= align;
		itm.valign		= vAlignRel;
		//Autoresize
		itm.itf			= NULL;		//itf to automatically resize object
		itm.itfRelSz	= NBST_P(STNBSize, 0.0f, 0.0f);	//rel size when resizing
		//
		itm.extraData	= NULL;
		itm.refUserData.tipo = refTipo;
		itm.refUserData.valor = refValor;
		_items.agregarElemento(itm);
		_contenedor->agregarObjetoEscena(itm.obj);
		//
		const NBCajaAABB limites = _contenedor->limites();
		this->organizarContenido((limites.xMax - limites.xMin), 0.0f, false, NULL, 0.0f);
	}
}

void AUIButton::agregarTextoMultilinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor){
	this->agregarTextoMultilinea(fuente, str, align, vAlignRel, refTipo, refValor, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255);
}

void AUIButton::agregarTextoMultilinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 color){
	this->agregarTextoMultilinea(fuente, str, align, vAlignRel, refTipo, refValor, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a, color.r, color.g, color.b, color.a);
}

void AUIButton::agregarTextoMultilinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 colorNormal, const NBColor8 colorHover, const NBColor8 colorTouched){
	this->agregarTextoMultilinea(fuente, str, align, vAlignRel, refTipo, refValor, colorNormal.r, colorNormal.g, colorNormal.b, colorNormal.a, colorHover.r, colorHover.g, colorHover.b, colorHover.a, colorTouched.r, colorTouched.g, colorTouched.b, colorTouched.a);
}

void AUIButton::agregarTextoMultilinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	this->agregarTextoMultilinea(fuente, str, align, vAlignRel, refTipo, refValor, r, g, b, a, r, g, b, a, r, g, b, a);
}

void AUIButton::agregarTextoMultilinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 rN, const UI8 gN, const UI8 bN, const UI8 aN, const UI8 rH, const UI8 gH, const UI8 bH, const UI8 aH, const UI8 rT, const UI8 gT, const UI8 bT, const UI8 aT){
	NBASSERT(fuente != NULL && align >= 0 && align < ENIBotonItemAlign_Count)
	if(fuente != NULL && align >= 0 && align < ENIBotonItemAlign_Count){
		AUEscenaTexto* escTxt = new(this) AUEscenaTexto(fuente);
		escTxt->establecerAlineaciones(ENNBTextLineAlignH_Base, ENNBTextAlignV_Base);
		escTxt->establecerTexto(str);
		//Extra data
		float* anchoUltimo = (float*)malloc(sizeof(float));
		*anchoUltimo	= -1.0f;
		//
		STIBotonItem itm;
		NBASSERT(ENIBotonItemState_Count == 3)
		NBCOLOR_ESTABLECER(itm.colors[ENIBotonItemState_Normal], rN, gN, bN, aN);
		NBCOLOR_ESTABLECER(itm.colors[ENIBotonItemState_Hover], rH, gH, bH, aH);
		NBCOLOR_ESTABLECER(itm.colors[ENIBotonItemState_Touched], rT, gT, bT, aT);
		itm.obj			= escTxt;
		itm.obj->establecerMultiplicadorColor8(itm.colors[_estadoActual]);
		itm.type		= ENIBotonItemType_TextMultiline;
		itm.align		= align;
		itm.valign		= vAlignRel;
		//Autoresize
		itm.itf			= NULL;		//itf to automatically resize object
		itm.itfRelSz	= NBST_P(STNBSize, 0.0f, 0.0f);	//rel size when resizing
		//
		itm.extraData	= anchoUltimo;
		itm.refUserData.tipo = refTipo;
		itm.refUserData.valor = refValor;
		_items.agregarElemento(itm);
		_contenedor->agregarObjetoEscena(itm.obj);
		//
		const NBCajaAABB limites = _contenedor->limites();
		this->organizarContenido((limites.xMax - limites.xMin), 0.0f, false, NULL, 0.0f);
	}
}


//

void AUIButton::actualizarObjetoPorTipo(const SI32 refTipo, AUEscenaObjeto* obj){
	SI32 i; const SI32 count = _items.conteo;
	for(i = 0; i < count; i++){
		STIBotonItem* itm = _items.elemPtr(i);
		if(itm->type == ENIBotonItemType_Object && itm->refUserData.tipo == refTipo){
			if(itm->obj != obj){
				if(obj != NULL){
					_contenedor->agregarObjetoEscena(obj);
					obj->retener(NB_RETENEDOR_THIS);
				}
				if(itm->obj != NULL){
					AUEscenaContenedor* parent = (AUEscenaContenedor*)itm->obj->contenedor();
					if(parent != NULL) parent->quitarObjetoEscena(itm->obj);
					itm->obj->liberar(NB_RETENEDOR_THIS);
					itm->obj = NULL;
				}
				itm->obj	= obj;
				itm->itf	= NULL;
			}
		}
	}
}

void AUIButton::actualizarObjetoResizablePorTipo(const SI32 refTipo, AUEscenaObjeto* obj, AUEscenaListaItemI* itf){
	SI32 i; const SI32 count = _items.conteo;
	for(i = 0; i < count; i++){
		STIBotonItem* itm = _items.elemPtr(i);
		if(itm->type == ENIBotonItemType_Object && itm->refUserData.tipo == refTipo){
			if(itm->obj != obj){
				if(obj != NULL){
					_contenedor->agregarObjetoEscena(obj);
					obj->retener(NB_RETENEDOR_THIS);
				}
				if(itm->obj != NULL){
					AUEscenaContenedor* parent = (AUEscenaContenedor*)itm->obj->contenedor();
					if(parent != NULL) parent->quitarObjetoEscena(itm->obj);
					itm->obj->liberar(NB_RETENEDOR_THIS);
					itm->obj = NULL;
				}
				itm->obj	= obj;
				itm->itf	= itf;
			}
		}
	}
}

void AUIButton::actualizarIconosPorTipo(const SI32 refTipo, AUTextura* tex){
	SI32 i; const SI32 count = _items.conteo;
	for(i = 0; i < count; i++){
		STIBotonItem* itm = _items.elemPtr(i);
		if(itm->type == ENIBotonItemType_Icon && itm->refUserData.tipo == refTipo){
			AUEscenaSprite* ico = (AUEscenaSprite*)itm->obj;
			ico->establecerTextura(tex);
			ico->redimensionar(tex);
		}
	}
}

void AUIButton::actualizarTextosMonolineaPorTipo(const SI32 refTipo, const char* strTexto){
	SI32 i; const SI32 count = _items.conteo;
	for(i = 0; i < count; i++){
		STIBotonItem* itm = _items.elemPtr(i);
		if(itm->type == ENIBotonItemType_TextMonoline && itm->refUserData.tipo == refTipo){
			AUEscenaTexto* txt = (AUEscenaTexto*)itm->obj;
			txt->establecerTexto(strTexto);
		}
	}
}

void AUIButton::actualizarTextosMultilineaPorTipo(const SI32 refTipo, const char* strTexto){
	SI32 i; const SI32 count = _items.conteo;
	for(i = 0; i < count; i++){
		STIBotonItem* itm = _items.elemPtr(i);
		if(itm->type == ENIBotonItemType_TextMultiline && itm->refUserData.tipo == refTipo){
			NBASSERT(itm->extraData != NULL)
			const float anchoUlt = *((float*)itm->extraData);
			AUEscenaTexto* txt = (AUEscenaTexto*)itm->obj;
			txt->establecerTextoEnCol(strTexto, NBST_P(STNBRect,  0, 0, anchoUlt, 0), TRUE);
		}
	}
}

//

void AUIButton::establecerColoresParaItemsTipo(const SI32 refTipo, const NBColor8 &normal, const NBColor8 &hover, const NBColor8 &touched){
	SI32 i; const SI32 count = _items.conteo;
	for(i = 0; i < count; i++){
		STIBotonItem* itm = _items.elemPtr(i);
		if(itm->refUserData.tipo == refTipo){
			itm->colors[ENIBotonItemState_Normal]	= normal;
			itm->colors[ENIBotonItemState_Hover]	= hover;
			itm->colors[ENIBotonItemState_Touched]	= touched;
			itm->obj->establecerMultiplicadorColor8(itm->colors[_estadoActual]);
		}
	}
}

//

void AUIButton::organizarContenido(const bool notificarCambioAltura){
	const NBCajaAABB cajaCont = this->limitesParaContenido();
	this->organizarContenido((cajaCont.xMax - cajaCont.xMin), 0.0f, notificarCambioAltura, NULL, 0.0f);
}

void AUIButton::organizarContenido(const float anchoParaContenido, const float altoVisible, const bool notificarCambioAltura, AUAnimadorObjetoEscena* animator, const float secsAnim){
	//
	// Calculate items sizes
	//
	NBTamano tamFondo; NBTAMANO_ESTABLECER(tamFondo, 0.0f, 0.0f)
	if(_fondo != NULL){
		const AUTextura* texFondo = _fondo->textura();
		if(texFondo != NULL){
			tamFondo = texFondo->tamanoHD();
		}
	}
	//
	float txtMonoAscSup = 0.0f;
	float txtMonoDscInf = 0.0f;
	float altoMaximo = 0.0f;
	float anchoPorAlinea[ENIBotonItemAlign_Count];
	float anchoRigidoPorAlinea[ENIBotonItemAlign_Count];
	float anchoFlexiblePorAlinea[ENIBotonItemAlign_Count];
	float anchoFlexibleTotal = 0.0f;
	SI32 itemsPorAlinea[ENIBotonItemAlign_Count];
	SI32 itemsFlexiblesPorAlinea[ENIBotonItemAlign_Count];
	SI32 itemsFlexiblesTotal = 0;
	{
		{
			SI32 i; for(i = 0; i < ENIBotonItemAlign_Count; i++){
				anchoPorAlinea[i]			= 0.0f;
				anchoRigidoPorAlinea[i]		= 0.0f;
				anchoFlexiblePorAlinea[i]	= 0.0f;
				itemsPorAlinea[i]			= 0;
				itemsFlexiblesPorAlinea[i]	= 0;
			}
		}
		{
			SI32 i; const SI32 count = _items.conteo;
			for(i = 0; i < count; i++){
				STIBotonItem* itm = _items.elemPtr(i); NBASSERT(itm->obj->contenedor() == _contenedor)
				if(itm->itf != NULL && anchoParaContenido > 0.0f){
					const float width = (anchoParaContenido - _margins.left - _margins.right) * itm->itfRelSz.width;
					float height = 0.0f;
					if(altoVisible > 0.0f){
						height = (altoVisible - _margins.top - _margins.bottom) * itm->itfRelSz.height;
					}
					if(width >= 0.0f && height >= 0.0f){
						itm->itf->organizarContenido(width, height, TRUE, NULL, 0.0f);
					}
				}
				const NBCajaAABB cajaAABB = itm->obj->cajaAABBLocal();
				NBASSERT(cajaAABB.xMax >= cajaAABB.xMin)
				NBASSERT(cajaAABB.yMax >= cajaAABB.yMin)
				const bool esFlexible = (itm->type == ENIBotonItemType_TextMultiline);
				//Analizar ancho
				const float ancho = cajaAABB.xMax - cajaAABB.xMin;
				if(ancho > 0.0f){
					if(esFlexible){
						anchoFlexibleTotal += ancho; NBASSERT(anchoFlexibleTotal >= 0.0f)
						anchoFlexiblePorAlinea[itm->align] += ancho; NBASSERT(anchoFlexiblePorAlinea[itm->align] >= 0.0f)
						itemsFlexiblesPorAlinea[itm->align]++; NBASSERT(itemsFlexiblesPorAlinea[itm->align] > 0)
						itemsFlexiblesTotal++;
						//Nota: los anchosFlexibles se agregan a "anchoPorAlinea" despues de ser procesados.
					} else {
						anchoRigidoPorAlinea[itm->align] += ancho; NBASSERT(anchoRigidoPorAlinea[itm->align] >= 0.0f)
						anchoPorAlinea[itm->align] += ancho; NBASSERT(anchoPorAlinea[itm->align] >= 0)
					}
					itemsPorAlinea[itm->align]++; NBASSERT(itemsPorAlinea[itm->align] > 0)
				}
				//Analizar alto
				if(!esFlexible){
					if(itm->type == ENIBotonItemType_TextMonoline){
						if(txtMonoAscSup < cajaAABB.yMax) txtMonoAscSup = cajaAABB.yMax; //yMax es el acendente-real.
						if(txtMonoDscInf < cajaAABB.yMin) txtMonoDscInf = cajaAABB.yMin; //yMin es el descendente-real.
					} else {
						const float alto = cajaAABB.yMax - cajaAABB.yMin;
						if(altoMaximo < alto){ altoMaximo = alto; }
					}
				}
			}
		}
		//Considerar el alto de los textos
		const float maxAltoTxtMono = (txtMonoAscSup - txtMonoDscInf);
		if(altoMaximo < maxAltoTxtMono){ altoMaximo = maxAltoTxtMono; }
		//Match height with backgorund texture (if bigger)
		if(_fondo != NULL){
			if((_margins.top + altoMaximo + _margins.bottom) < tamFondo.alto){
				altoMaximo = tamFondo.alto - _margins.top - _margins.bottom;
			}
		}
	}
	NBCajaAABB limites;
	limites.yMin = 0.0f; limites.yMax = _margins.top + altoMaximo + _margins.bottom;
	if(anchoParaContenido > 0.0f){
		limites.xMin = 0.0f; limites.xMax = anchoParaContenido;
		//Ajustar los anchos flexibles y actualizar el alto
		if(itemsFlexiblesTotal != 0 && anchoFlexibleTotal > 0.0f){
			const float anchoSinFlexibles = _margins.left + _margins.right
			+ anchoRigidoPorAlinea[ENIBotonItemAlign_Left] + (itemsPorAlinea[ENIBotonItemAlign_Left] == 0 ? 0.0f : (itemsPorAlinea[ENIBotonItemAlign_Left] - 1) * _marginI)
			+ anchoRigidoPorAlinea[ENIBotonItemAlign_Right] + (itemsPorAlinea[ENIBotonItemAlign_Right] == 0 ? 0.0f : (itemsPorAlinea[ENIBotonItemAlign_Right] - 1) * _marginI)
			+ anchoRigidoPorAlinea[ENIBotonItemAlign_Center] + (itemsPorAlinea[ENIBotonItemAlign_Center] == 0 ? 0.0f : (itemsPorAlinea[ENIBotonItemAlign_Center] - 1) * _marginI)
			+ (((itemsPorAlinea[ENIBotonItemAlign_Left] != 0 && itemsPorAlinea[ENIBotonItemAlign_Center] != 0) || (itemsPorAlinea[ENIBotonItemAlign_Left] != 0 && itemsPorAlinea[ENIBotonItemAlign_Right] != 0)) ? _marginI : 0.0f)
			+ ((itemsPorAlinea[ENIBotonItemAlign_Center] != 0 && itemsPorAlinea[ENIBotonItemAlign_Right] != 0) ? _marginI : 0.0f);
			const float sobranteParaFlexibles = anchoParaContenido - anchoSinFlexibles;
			if(sobranteParaFlexibles > 0.0f){
				SI32 i; const SI32 count = _items.conteo;
				for(i = 0; i < count; i++){
					STIBotonItem* itm = _items.elemPtr(i); NBASSERT(itm->obj->contenedor() == _contenedor)
					const bool esFlexible = (itm->type == ENIBotonItemType_TextMultiline);
					if(esFlexible){
						NBCajaAABB cajaAABB = itm->obj->cajaAABBLocal();
						NBASSERT(cajaAABB.xMax >= cajaAABB.xMin)
						NBASSERT(cajaAABB.yMax >= cajaAABB.yMin)
						const float ancho = cajaAABB.xMax - cajaAABB.xMin;
						const float anchoUsar = sobranteParaFlexibles * (ancho / anchoFlexibleTotal);
						if(itm->type == ENIBotonItemType_TextMultiline){
							AUEscenaTexto* objTxt = (AUEscenaTexto*)itm->obj;
							float* anchoUltimo = (float*)itm->extraData; NBASSERT(anchoUltimo != NULL)
							if(*anchoUltimo != anchoUsar){
								objTxt->organizarTexto(anchoUsar);
								*anchoUltimo = anchoUsar;
							}
							cajaAABB = itm->obj->cajaAABBLocalCalculada(); //Nueva caja
							anchoPorAlinea[itm->align] += (cajaAABB.xMax - cajaAABB.xMin);
						} else {
							NBASSERT(false)
						}
						//Analizar alto
						{
							const float alto = cajaAABB.yMax - cajaAABB.yMin;
							if(altoMaximo < alto){ altoMaximo = alto; }
						}
					}
				}
				//Actualizar limites verticales
				limites.yMin = 0.0f; limites.yMax = _margins.top + altoMaximo + _margins.bottom;
			}
		}
	} else {
		//autosize
		limites.xMin = 0.0f;
		limites.xMax = _margins.left + _margins.right
		+ anchoRigidoPorAlinea[ENIBotonItemAlign_Left] + anchoFlexiblePorAlinea[ENIBotonItemAlign_Left] + (itemsPorAlinea[ENIBotonItemAlign_Left] == 0 ? 0.0f : (itemsPorAlinea[ENIBotonItemAlign_Left] - 1) * _marginI)
		+ anchoRigidoPorAlinea[ENIBotonItemAlign_Right] + anchoFlexiblePorAlinea[ENIBotonItemAlign_Right] + (itemsPorAlinea[ENIBotonItemAlign_Right] == 0 ? 0.0f : (itemsPorAlinea[ENIBotonItemAlign_Right] - 1) * _marginI)
		+ anchoRigidoPorAlinea[ENIBotonItemAlign_Center] + anchoFlexiblePorAlinea[ENIBotonItemAlign_Center] + (itemsPorAlinea[ENIBotonItemAlign_Center] == 0 ? 0.0f : (itemsPorAlinea[ENIBotonItemAlign_Center] - 1) * _marginI)
		+ (((itemsPorAlinea[ENIBotonItemAlign_Left] != 0 && itemsPorAlinea[ENIBotonItemAlign_Center] != 0) || (itemsPorAlinea[ENIBotonItemAlign_Left] != 0 && itemsPorAlinea[ENIBotonItemAlign_Right] != 0)) ? _marginI : 0.0f)
		+ ((itemsPorAlinea[ENIBotonItemAlign_Center] != 0 && itemsPorAlinea[ENIBotonItemAlign_Right] != 0) ? _marginI : 0.0f);
		if(_fondo != NULL){
			if(limites.xMax < tamFondo.ancho){
				limites.xMax = tamFondo.ancho;
			}
		}
	}
	_contenedor->establecerLimites(limites.xMin, limites.xMax, limites.yMin, limites.yMax);
	if(_fondo != NULL){
		;
		_fondo->redimensionar((_fondoOpciones & AUIButton_FONDO_MASK_INVERTIR_H) == 0 ? limites.xMin : limites.xMax
							  , (_fondoOpciones & AUIButton_FONDO_MASK_INVERTIR_V) == 0 ? limites.yMax : limites.yMin
							  , (limites.xMax - limites.xMin) * ((_fondoOpciones & AUIButton_FONDO_MASK_INVERTIR_H) == 0 ? 1 : -1)
							  , limites.yMin - limites.yMax * ((_fondoOpciones & AUIButton_FONDO_MASK_INVERTIR_V) == 0 ? 1 : -1));
	}
	//
	// Organize items
	//
	{
		//Left-aligned
		if(itemsPorAlinea[ENIBotonItemAlign_Left] != 0){
			//PRINTF_INFO("Left-aligned(%d items, %f width).\n", itemsPorAlinea[ENIBotonItemAlign_Left], anchoPorAlinea[ENIBotonItemAlign_Left]);
			float xIzq = _margins.left;
			SI32 i; const SI32 count = _items.conteo;
			for(i = 0; i < count; i++){
				STIBotonItem* itm = _items.elemPtr(i);
				if(itm->align == ENIBotonItemAlign_Left){
					NBCajaAABB cajaAABB = itm->obj->cajaAABBLocal();
					if(itm->type == ENIBotonItemType_TextMonoline){
						cajaAABB.yMax = txtMonoAscSup;
						cajaAABB.yMin = txtMonoDscInf;
					}
					NBASSERT(cajaAABB.xMax >= cajaAABB.xMin)
					NBASSERT(cajaAABB.yMax >= cajaAABB.yMin)
					const float ancho = cajaAABB.xMax - cajaAABB.xMin;
					const float alto = cajaAABB.yMax - cajaAABB.yMin;
					if(ancho <= 0.0f){
						itm->obj->establecerVisible(false);
					} else {
						itm->obj->establecerVisible(true);
						itm->obj->establecerTraslacion(xIzq - cajaAABB.xMin, _margins.bottom - cajaAABB.yMin + ((altoMaximo - alto) * itm->valign));
						xIzq += ancho + _marginI;
					}
				}
			}
		}
		//Right-aligned
		if(itemsPorAlinea[ENIBotonItemAlign_Right] != 0){
			//PRINTF_INFO("Right-aligned(%d items, %f width).\n", itemsPorAlinea[ENIBotonItemAlign_Right], anchoPorAlinea[ENIBotonItemAlign_Right]);
			float xDer = limites.xMax - _margins.right;
			SI32 i; const SI32 count = _items.conteo;
			for(i = 0; i < count; i++){
				STIBotonItem* itm = _items.elemPtr(i);
				if(itm->align == ENIBotonItemAlign_Right){
					NBCajaAABB cajaAABB = itm->obj->cajaAABBLocal();
					if(itm->type == ENIBotonItemType_TextMonoline){
						cajaAABB.yMax = txtMonoAscSup;
						cajaAABB.yMin = txtMonoDscInf;
					}
					NBASSERT(cajaAABB.xMax >= cajaAABB.xMin)
					NBASSERT(cajaAABB.yMax >= cajaAABB.yMin)
					const float ancho = cajaAABB.xMax - cajaAABB.xMin;
					const float alto = cajaAABB.yMax - cajaAABB.yMin;
					if(ancho <= 0.0f){
						itm->obj->establecerVisible(false);
					} else {
						itm->obj->establecerVisible(true);
						itm->obj->establecerTraslacion(xDer - cajaAABB.xMax, _margins.bottom - cajaAABB.yMin + ((altoMaximo - alto) * itm->valign));
						xDer -= ancho + _marginI;
					}
				}
			}
		}
		//Center-aligned
		if(itemsPorAlinea[ENIBotonItemAlign_Center] != 0){
			//PRINTF_INFO("Center-aligned(%d items, %f width).\n", itemsPorAlinea[ENIBotonItemAlign_Center], anchoPorAlinea[ENIBotonItemAlign_Center]);
			const float limIzq = _margins.left + anchoPorAlinea[ENIBotonItemAlign_Left] + (itemsPorAlinea[ENIBotonItemAlign_Left] == 0 ? 0.0f : (itemsPorAlinea[ENIBotonItemAlign_Left] - 1) * _marginI);
			const float limDer = limites.xMax - (_margins.right + anchoPorAlinea[ENIBotonItemAlign_Right] + (itemsPorAlinea[ENIBotonItemAlign_Right] == 0 ? 0.0f : (itemsPorAlinea[ENIBotonItemAlign_Right] - 1) * _marginI));
			float xIzq;
			if(limIzq > limDer){
				if(anchoPorAlinea[ENIBotonItemAlign_Left] != 0 || anchoPorAlinea[ENIBotonItemAlign_Right] != 0){
					xIzq = limites.xMin + ((limites.xMax - limites.xMin) * (anchoPorAlinea[ENIBotonItemAlign_Left] / (anchoPorAlinea[ENIBotonItemAlign_Left] + anchoPorAlinea[ENIBotonItemAlign_Right])));
				} else {
					xIzq = limites.xMin + ((limites.xMax - limites.xMin) * 0.5f);
				}
			} else {
				xIzq = limIzq + (((limDer - limIzq) - (anchoPorAlinea[ENIBotonItemAlign_Center] + (itemsPorAlinea[ENIBotonItemAlign_Center] == 0 ? 0.0f : (itemsPorAlinea[ENIBotonItemAlign_Center] - 1) * _marginI))) * 0.5f);
			}
			SI32 i; const SI32 count = _items.conteo;
			for(i = 0; i < count; i++){
				STIBotonItem* itm = _items.elemPtr(i);
				if(itm->align == ENIBotonItemAlign_Center){
					NBCajaAABB cajaAABB = itm->obj->cajaAABBLocal();
					if(itm->type == ENIBotonItemType_TextMonoline){
						cajaAABB.yMax = txtMonoAscSup;
						cajaAABB.yMin = txtMonoDscInf;
					}
					NBASSERT(cajaAABB.xMax >= cajaAABB.xMin)
					NBASSERT(cajaAABB.yMax >= cajaAABB.yMin)
					const float ancho = cajaAABB.xMax - cajaAABB.xMin;
					const float alto = cajaAABB.yMax - cajaAABB.yMin;
					if(ancho <= 0.0f){
						itm->obj->establecerVisible(false);
					} else {
						itm->obj->establecerVisible(true);
						itm->obj->establecerTraslacion(xIzq - cajaAABB.xMin, _margins.bottom - cajaAABB.yMin + ((altoMaximo - alto) * itm->valign));
						xIzq += ancho + _marginI;
					}
				}
			}
		}
	}
}

void AUIButton::privAnalizarEstado(){
	ENIBotonItemState estadoDst = ENIBotonItemState_Normal;
	NBASSERT(_touchesCount >= 0 && _hoversCount >= 0)
	//
	if(_touchesCount > 0){
		estadoDst = ENIBotonItemState_Touched;
	} else if(_hoversCount > 0){
		estadoDst = ENIBotonItemState_Hover;
	} else {
		estadoDst = ENIBotonItemState_Normal;
	}
	//
	if(_estadoActual != estadoDst){
		_estadoActual = estadoDst;
		if((_maskAnimacionEstado & AUIButton_MASK_AGREGADO_ANIMADOR) == 0){
			if(_estadoActual == ENIBotonItemState_Touched && (_segsRetrasarOnTouch > 0.0f || _segsParaLongTouch > 0.0f)){
				//Animar espera hacia siguiente estado
				_segsAcumColorOnTouch = 0.0f;
				NBGestorAnimadores::agregarAnimador(this, this);
				_maskAnimacionEstado |= AUIButton_MASK_AGREGADO_ANIMADOR;
			} else {
				//Aplicar estado de inmediato
				this->privActualizarObjetos((ENIBotonItemState)_estadoActual);
			}
		}
	}
}

void AUIButton::privActualizarObjetos(const ENIBotonItemState estado){
	if(_fondo != NULL){
		_fondo->establecerMultiplicadorColor8(_fondoColors[estado]);
	}
	{
		SI32 i; const SI32 count = _items.conteo;
		for(i = 0; i < count; i++){
			STIBotonItem* itm = _items.elemPtr(i);
			itm->obj->establecerMultiplicadorColor8(itm->colors[_estadoActual]);
		}
	}
	_estadoPresentado = estado;
}


void AUIButton::tickAnimacion(float segsTranscurridos){
	BOOL animRequired = FALSE;
	if((_maskAnimacionEstado & AUIButton_MASK_BOTON_ACCIONADO) != 0){
		//El boton fue accionado, debe animarse el color de fondo a touched (si no se ha hecho)
		if(_estadoPresentado != ENIBotonItemState_Touched){
			this->privActualizarObjetos(ENIBotonItemState_Touched);
			animRequired = TRUE;
		} else {
			this->privActualizarObjetos((ENIBotonItemState)_estadoActual);
			_maskAnimacionEstado &= ~AUIButton_MASK_BOTON_ACCIONADO;
		}
	} else {
		if(_estadoActual == ENIBotonItemState_Touched){
			const float segsAcumPrev = _segsAcumColorOnTouch; 
			_segsAcumColorOnTouch += segsTranscurridos;
			if(_segsAcumColorOnTouch <= _segsRetrasarOnTouch){
				animRequired = TRUE;
			} else {
				this->privActualizarObjetos(ENIBotonItemState_Touched);
				_maskAnimacionEstado &= ~AUIButton_MASK_BOTON_ACCIONADO;
			}
			if(_segsParaLongTouch > 0.0f){
				if(_segsAcumColorOnTouch < _segsParaLongTouch){
					animRequired = TRUE;
				} else if(segsAcumPrev < _segsParaLongTouch && _escuchadorBoton != NULL){
					BOOL consumeAsLongTouch = FALSE;
					_escuchadorBoton->botonLongTouch(this, &consumeAsLongTouch);
					if(consumeAsLongTouch){
						_touchesAsLongTouch = TRUE;
					}
				}
			}
		} else {
			this->privActualizarObjetos((ENIBotonItemState)_estadoActual);
			_maskAnimacionEstado &= ~AUIButton_MASK_BOTON_ACCIONADO;
		}
	}
	if(!animRequired){
		NBGestorAnimadores::quitarAnimador(this);
		_maskAnimacionEstado &= ~AUIButton_MASK_AGREGADO_ANIMADOR;
	}
}

//TOUCHES
void AUIButton::touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto){
	if(objeto == _contenedor){
		_touchesCount++;
		this->privAnalizarEstado();
		if(_touchesCount == 1){
			_touchesAsLongTouch = FALSE;
			if(_escuchadorBoton != NULL){
				_escuchadorBoton->botonPresionado(this);
			}
		}
	}
}

void AUIButton::touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	if(objeto == _contenedor){
		if(_escuchadorBoton != NULL){
			if(this->idEscena >= 0){
				const NBTamano maxDrag	= NBGestorEscena::tamanoPulgadasAPantalla(this->idEscena, 0.02f, 0.02f);
				const STNBSize curDrag	= NBST_P(STNBSize, (posActualEscena.x - posInicialEscena.x), (posActualEscena.y - posInicialEscena.y));
				const float curDist2	= ((curDrag.width * curDrag.width) + (curDrag.height * curDrag.height));
				const float maxDist2	= ((maxDrag.ancho * maxDrag.ancho) + (maxDrag.alto * maxDrag.alto)); //Originaly fixed to: dist(6) or dist2(26)
				//PRINTF_INFO("curDist2(%f) of maxDist2(%f) maxDist(%f).\n", curDist2, maxDist2, sqrtf(maxDist2));
				if(curDist2 > maxDist2){
					AUEscenaObjeto* heredero = _escuchadorBoton->botonHerederoParaTouch(this, posInicialEscena, posActualEscena);
					if(heredero != NULL){
						objeto->liberarTouch(touch, posInicialEscena, posAnteriorEscena, posActualEscena, true, heredero);
					}
				}
			}
		}
	}
}

void AUIButton::touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	if(objeto == _contenedor){
		_touchesCount--;
		this->privAnalizarEstado();
		if(_touchesCount == 0){
			const NBCajaAABB cajaEscena = objeto->cajaAABBEnEscena();
			if(NBCAJAAABB_INTERSECTA_PUNTO(cajaEscena, posActualEscena.x, posActualEscena.y) && !touch->cancelado){
				if((_maskAnimacionEstado & AUIButton_MASK_AGREGADO_ANIMADOR) != 0){
					_maskAnimacionEstado |= AUIButton_MASK_BOTON_ACCIONADO;
				}
				if(!_touchesAsLongTouch && _escuchadorBoton != NULL){
					_escuchadorBoton->botonAccionado(this);
				}
			}
		}
	}
}

void AUIButton::hoverIniciado(AUEscenaObjeto* objeto){
	if(objeto == _contenedor){
		_hoversCount++;
		this->privAnalizarEstado();
	}
}

void AUIButton::hoverMovido(const NBPunto &posActualEscena, AUEscenaObjeto* objeto){

}

void AUIButton::hoverFinalizado(AUEscenaObjeto* objeto){
	if(objeto == _contenedor){
		_hoversCount--;
		this->privAnalizarEstado();
	}
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUIButton, AUEscenaContenedor)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUIButton, "AUIButton", AUEscenaContenedor)
AUOBJMETODOS_CLONAR_NULL(AUIButton)


