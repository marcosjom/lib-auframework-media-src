//
//  AUEscenaPanel.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 10/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaPanel.h"
#include "NBGestorAnimadores.h"
#include "NBGestorEscena.h"

AUEscenaPanel::AUEscenaPanel() : AUEscenaContenedor()
	, _escuchadorPanel(NULL)
	, _subescenaObj(NULL)
	, _subescenaObjUlt(NULL)
	//
	, _subescenaId(-1)
	, _subescenaSprite(NULL)
	, _subescenaContenedor(NULL)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPanel::AUEscenaPanel")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaPanel")
	_panelTamano.ancho		= 0;
	_panelTamano.alto		= 0;
	_colorFondo.r			= 255;
	_colorFondo.g			= 255;
	_colorFondo.b			= 255;
	_colorFondo.a			= 255;
	_margenes.left			= 0;
	_margenes.right			= 0;
	_margenes.top			= 0;
	_margenes.bottom		= 0;
	//
	_subescenaObjCajaUlt.xMin = 0;
	_subescenaObjCajaUlt.xMax = 0;
	_subescenaObjCajaUlt.yMin = 0;
	_subescenaObjCajaUlt.yMax = 0;
	_subescenaScrollAutoTouches = 0;
	_subescenaScrollAuto.ancho = 0;
	_subescenaScrollAuto.alto = 0;
	//
	_subescenaContenedor	= new(this) AUEscenaContenedor();
	NBCAJAAABB_INICIALIZAR(_subescenaCajaBase)
	_subescenaTamano.ancho	= 0;
	_subescenaTamano.alto	= 0;
	_subescenaScroll.ancho	= 0.0f;
	_subescenaScroll.alto	= 0.0f;
	//
	this->establecerTamanoPanel(0, 0);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaPanel::AUEscenaPanel(const float ancho, const float alto, const NBMargenes margenes) : AUEscenaContenedor()
	, _escuchadorPanel(NULL)
	, _subescenaObj(NULL)
	, _subescenaObjUlt(NULL)
	//
	, _subescenaId(-1)
	, _subescenaSprite(NULL)
	, _subescenaContenedor(NULL)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPanel::AUEscenaPanel")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaPanel")
	_panelTamano.ancho		= 0;
	_panelTamano.alto		= 0;
	_colorFondo.r			= 255;
	_colorFondo.g			= 255;
	_colorFondo.b			= 255;
	_colorFondo.a			= 255;
	_margenes				= margenes;
	//
	_subescenaObjCajaUlt.xMin = 0;
	_subescenaObjCajaUlt.xMax = 0;
	_subescenaObjCajaUlt.yMin = 0;
	_subescenaObjCajaUlt.yMax = 0;
	_subescenaScrollAutoTouches = 0;
	_subescenaScrollAuto.ancho = 0;
	_subescenaScrollAuto.alto = 0;
	//
	_subescenaContenedor	= new(this) AUEscenaContenedor();
	NBCAJAAABB_INICIALIZAR(_subescenaCajaBase)
	_subescenaTamano.ancho	= 0;
	_subescenaTamano.alto	= 0;
	_subescenaScroll.ancho	= 0.0f;
	_subescenaScroll.alto	= 0.0f;
	//
	this->establecerTamanoPanel(ancho, alto);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaPanel::AUEscenaPanel(const float ancho, const float alto, const float margenLeft, const float margenRight, const float margenTop, const float margenBottom) : AUEscenaContenedor()
	, _escuchadorPanel(NULL)
	, _subescenaObj(NULL)
	, _subescenaObjUlt(NULL)
	//
	, _subescenaId(-1)
	, _subescenaSprite(NULL)
	, _subescenaContenedor(NULL)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPanel::AUEscenaPanel")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaPanel")
	_panelTamano.ancho		= 0;
	_panelTamano.alto		= 0;
	_colorFondo.r			= 255;
	_colorFondo.g			= 255;
	_colorFondo.b			= 255;
	_colorFondo.a			= 255;
	_margenes.left			= margenLeft;
	_margenes.right			= margenRight;
	_margenes.top			= margenTop;
	_margenes.bottom		= margenBottom;
	//
	_subescenaObjCajaUlt.xMin = 0;
	_subescenaObjCajaUlt.xMax = 0;
	_subescenaObjCajaUlt.yMin = 0;
	_subescenaObjCajaUlt.yMax = 0;
	_subescenaScrollAutoTouches = 0;
	_subescenaScrollAuto.ancho = 0;
	_subescenaScrollAuto.alto = 0;
	//
	_subescenaContenedor	= new(this) AUEscenaContenedor();
	NBCAJAAABB_INICIALIZAR(_subescenaCajaBase)
	_subescenaTamano.ancho	= 0;
	_subescenaTamano.alto	= 0;
	_subescenaScroll.ancho	= 0.0f;
	_subescenaScroll.alto	= 0.0f;
	//
	this->establecerTamanoPanel(ancho, alto);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaPanel::~AUEscenaPanel(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaPanel::~AUEscenaPanel")
	if(_subescenaObj != NULL) _subescenaObj->liberar(NB_RETENEDOR_THIS); _subescenaObj = NULL;
	if(_subescenaContenedor != NULL) _subescenaContenedor->liberar(NB_RETENEDOR_THIS); _subescenaContenedor = NULL;
	if(_subescenaSprite != NULL) _subescenaSprite->liberar(NB_RETENEDOR_THIS); _subescenaSprite = NULL;
	if(_subescenaId != -1){ NBGestorEscena::liberarEscena(_subescenaId); _subescenaId = -1; }
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaPanel::agregadoEnEscena(){
	AUEscenaContenedor::agregadoEnEscena();
	NBGestorAnimadores::agregarAnimador(this, this);
	if(_subescenaId != -1){
		NBGestorEscena::habilitarEscena(_subescenaId);
	}
}

void AUEscenaPanel::quitandoDeEscena(){
	NBGestorAnimadores::quitarAnimador(this);
	AUEscenaContenedor::quitandoDeEscena();
	if(_subescenaId != -1){
		NBGestorEscena::deshabilitarEscena(_subescenaId);
	}
}

//

IEscuchadorEscenaPanel* AUEscenaPanel::escuchadorPanel(){
	return _escuchadorPanel;
}

void AUEscenaPanel::establecerEscuchadorPanel(IEscuchadorEscenaPanel* escuchador){
	_escuchadorPanel = escuchador;
}

//

NBColor8 AUEscenaPanel::colorFondo() const {
	return _colorFondo;
}

void AUEscenaPanel::establecerColorFondo(const NBColor8 color){
	_colorFondo = color;
	if(_subescenaId != -1){
		NBGestorEscena::establecerColorFondo(_subescenaId, ((float)_colorFondo.r / 255.0f), ((float)_colorFondo.g / 255.0f), ((float)_colorFondo.b / 255.0f), ((float)_colorFondo.a / 255.0f));
		NBGestorEscena::establecerFondoModo(_subescenaId, (_colorFondo.a == 255 ? ENGestorEscenaFondoModo_Opaco : ENGestorEscenaFondoModo_Transparente));
	}
}

void AUEscenaPanel::establecerColorFondo(const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	_colorFondo.r	= r;
	_colorFondo.g	= g;
	_colorFondo.b	= b;
	_colorFondo.a	= a;
	if(_subescenaId != -1){
		NBGestorEscena::establecerColorFondo(_subescenaId, ((float)_colorFondo.r / 255.0f), ((float)_colorFondo.g / 255.0f), ((float)_colorFondo.b / 255.0f), ((float)_colorFondo.a / 255.0f));
		NBGestorEscena::establecerFondoModo(_subescenaId, (_colorFondo.a == 255 ? ENGestorEscenaFondoModo_Opaco : ENGestorEscenaFondoModo_Transparente));
	}
}

NBMargenes AUEscenaPanel::margenes() const {
	return _margenes;
}

void AUEscenaPanel::establecerMargenes(const NBMargenes margenes){
	_margenes = margenes;
	this->establecerTamanoPanel(_panelTamano.ancho, _panelTamano.alto);
}

void AUEscenaPanel::establecerMargenes(const float margenLeft, const float margenRight, const float margenTop, const float margenBottom){
	_margenes.left		= margenLeft;
	_margenes.right		= margenRight;
	_margenes.top		= margenTop;
	_margenes.bottom	= margenBottom;
	this->establecerTamanoPanel(_panelTamano.ancho, _panelTamano.alto);
}

//

NBTamano AUEscenaPanel::tamanoPanel() const {
	return _panelTamano;
}

void AUEscenaPanel::establecerTamanoPanel(const NBTamano tam){
	this->establecerTamanoPanel(tam.ancho, tam.alto);
}

void AUEscenaPanel::establecerTamanoPanel(const float anchoCaja, const float altoCaja){
	if(anchoCaja > 0 && altoCaja > 0 && (_panelTamano.ancho != anchoCaja || _panelTamano.alto != altoCaja)){
		//Redimensionar subescena (si es necesario)
		NBTamanoI tamSubEscena;
		tamSubEscena.ancho	= (anchoCaja - _margenes.left - _margenes.right);
		tamSubEscena.alto	= (altoCaja - _margenes.top - _margenes.bottom);
		if(tamSubEscena.ancho <= 0 || tamSubEscena.alto <= 0){
			if(_subescenaSprite != NULL){
				AUEscenaContenedor* cont = (AUEscenaContenedor*)_subescenaSprite->contenedor();
				if(cont != NULL) cont->quitarObjetoEscena(_subescenaSprite);
				_subescenaSprite->liberar(NB_RETENEDOR_THIS);
				_subescenaSprite = NULL;
			}
		} else {
			if(_subescenaId == -1){
				_subescenaId = NBGestorEscena::crearEscena(tamSubEscena.ancho, tamSubEscena.alto, 1.0f, 1.0f, 72.0f, 72.0f, 72.0f, 72.0f, COLOR_RGBA8, ENGestorEscenaDestinoGl_Textura);
				if(this->idEscena == -1){
					NBGestorEscena::deshabilitarEscena(_subescenaId);
				} else {
					NBGestorEscena::habilitarEscena(_subescenaId);
				}
				//
				NBGestorEscena::establecerColorFondo(_subescenaId, ((float)_colorFondo.r / 255.0f), ((float)_colorFondo.g / 255.0f), ((float)_colorFondo.b / 255.0f), ((float)_colorFondo.a / 255.0f));
				NBGestorEscena::establecerFondoModo(_subescenaId, (_colorFondo.a == 255 ? ENGestorEscenaFondoModo_Opaco : ENGestorEscenaFondoModo_Transparente));
				NBGestorEscena::establecerEscenaLimpiaColorBuffer(_subescenaId, true);
				//
				NBColor8 colorAmb; NBCOLOR_ESTABLECER(colorAmb, 255, 255, 255, 255)
				NBGestorEscena::agregarObjetoCapa(_subescenaId, ENGestorEscenaGrupo_Escena, _subescenaContenedor, colorAmb);
				_subescenaTamano = tamSubEscena;
				_subescenaCajaBase.xMin = 0;
				_subescenaCajaBase.xMax = tamSubEscena.ancho;
				_subescenaCajaBase.yMin = -tamSubEscena.alto;
				_subescenaCajaBase.yMax = 0.0f;
				NBGestorEscena::establecerCajaProyeccion(_subescenaId, _subescenaCajaBase.xMin + _subescenaScroll.ancho, _subescenaCajaBase.xMax + _subescenaScroll.ancho, _subescenaCajaBase.yMin + _subescenaScroll.alto, _subescenaCajaBase.yMax + _subescenaScroll.alto); //float xMin, float xMax, float yMin, float yMax
			} else {
				const STGestorEscenaEscena props = NBGestorEscena::propiedadesEscena(_subescenaId);
				if(props.puertoDeVision.ancho != tamSubEscena.ancho || props.puertoDeVision.alto != tamSubEscena.alto){
					NBTamano sameDpi; sameDpi.ancho = sameDpi.alto = 0;
					NBGestorEscena::redimensionarEscena(_subescenaId, tamSubEscena.ancho, tamSubEscena.alto, 1.0f, sameDpi, sameDpi, 1.0f);
					_subescenaTamano = tamSubEscena;
					_subescenaCajaBase.xMin = 0;
					_subescenaCajaBase.xMax = tamSubEscena.ancho;
					_subescenaCajaBase.yMin = -tamSubEscena.alto;
					_subescenaCajaBase.yMax = 0.0f;
					NBGestorEscena::establecerCajaProyeccion(_subescenaId, _subescenaCajaBase.xMin + _subescenaScroll.ancho, _subescenaCajaBase.xMax + _subescenaScroll.ancho, _subescenaCajaBase.yMin + _subescenaScroll.alto, _subescenaCajaBase.yMax + _subescenaScroll.alto); //float xMin, float xMax, float yMin, float yMax
				}
			}
			const STGestorEscenaEscena propsEscena = NBGestorEscena::propiedadesEscena(_subescenaId); NBASSERT(propsEscena.texturaEscena != NULL)
			if(_subescenaSprite == NULL){
				_subescenaSprite = new(this) AUEscenaSprite();
				_subescenaSprite->establecerEscuchadorTouches(this, this);
				this->agregarObjetoEscena(_subescenaSprite);
			}
			_subescenaSprite->establecerTextura(propsEscena.texturaEscena);
			_subescenaSprite->redimensionar(0, -tamSubEscena.alto, tamSubEscena.ancho, tamSubEscena.alto);
		}
		//
		_panelTamano.ancho = anchoCaja;
		_panelTamano.alto = altoCaja;
	}
}

//

const AUEscenaObjeto* AUEscenaPanel::objetoContenido() const {
	return _subescenaObj;
}

void AUEscenaPanel::establecerObjetoContenido(AUEscenaObjeto* obj){
	if(obj != NULL){
		obj->establecerTraslacion(0.0f, 0.0f);
		obj->establecerRotacion(0.0f);
		obj->establecerEscalacion(1.0f, 1.0f);
		_subescenaContenedor->agregarObjetoEscena(obj);
		obj->retener(NB_RETENEDOR_THIS);
	}
	if(_subescenaObj != NULL){
		AUEscenaContenedor* parent = (AUEscenaContenedor*)_subescenaObj->contenedor();
		if(parent != NULL){
			parent->quitarObjetoEscena(_subescenaObj);
		}
		_subescenaObj->liberar(NB_RETENEDOR_THIS);
	}
	_subescenaObj = obj;
	//Show left top
	if(_subescenaObj != NULL){
		const NBCajaAABB contentBox = _subescenaObj->cajaAABBLocal();
		_subescenaScroll.ancho = (contentBox.xMin - _subescenaCajaBase.xMin);
		_subescenaScroll.alto = (_subescenaCajaBase.yMax - contentBox.yMax);
	} else {
		_subescenaScroll.ancho = 0;
		_subescenaScroll.alto = 0;
	}
	//Stop auto scrolling
	_subescenaScrollAuto.ancho	= 0.0f;
	_subescenaScrollAuto.alto	= 0.0f;
}

void AUEscenaPanel::scrollToLeftTop(){
	if(_subescenaObj != NULL){
		const NBCajaAABB contentBox = _subescenaObj->cajaAABBLocal();
		_subescenaScroll.ancho = (contentBox.xMin - _subescenaCajaBase.xMin);
		_subescenaScroll.alto = (_subescenaCajaBase.yMax - contentBox.yMax);
	} else {
		_subescenaScroll.ancho = 0;
		_subescenaScroll.alto = 0;
	}
}

//

void AUEscenaPanel::privAsegurarAreaEstaVisible(const NBRectangulo area){
	bool scrollAjustado = false;
	NBASSERT(area.ancho >= 0.0f && area.alto >= 0.0f)
	if((area.x + area.ancho) > (_subescenaCajaBase.xMax + _subescenaScroll.ancho)){
		_subescenaScroll.ancho	= (area.x + area.ancho) - _subescenaCajaBase.xMax;
		scrollAjustado			= true;
	} else if(area.x < (_subescenaCajaBase.xMin + _subescenaScroll.ancho)){
		_subescenaScroll.ancho	= area.x - _subescenaCajaBase.xMin;
		scrollAjustado			= true;
	}
	if((area.y + area.alto) > (_subescenaCajaBase.yMax + _subescenaScroll.alto)){
		_subescenaScroll.alto	= (area.y + area.alto) - _subescenaCajaBase.yMax;
		scrollAjustado			= true;
	} else if(area.y < (_subescenaCajaBase.yMin + _subescenaScroll.alto)){
		_subescenaScroll.alto	= area.y - _subescenaCajaBase.yMin;
		scrollAjustado			= true;
	}
	if(scrollAjustado && _subescenaId != -1){
		NBGestorEscena::establecerCajaProyeccion(_subescenaId, _subescenaCajaBase.xMin + _subescenaScroll.ancho, _subescenaCajaBase.xMax + _subescenaScroll.ancho, _subescenaCajaBase.yMin + _subescenaScroll.alto, _subescenaCajaBase.yMax + _subescenaScroll.alto); //float xMin, float xMax, float yMin, float yMax
	}
}

void AUEscenaPanel::privScrollSubEscena(const float deltaX, const float deltaY, const bool forceLeftTop){
	if(_subescenaObj != NULL && _subescenaId != -1){
		//Aplicar scroll
		_subescenaScroll.ancho	+= deltaX;
		_subescenaScroll.alto	+= deltaY;
		//Validar limites
		{
			NBCajaAABB viewBox;
			viewBox.xMin = _subescenaCajaBase.xMin + _subescenaScroll.ancho;
			viewBox.xMax = _subescenaCajaBase.xMax + _subescenaScroll.ancho;
			viewBox.yMin = _subescenaCajaBase.yMin + _subescenaScroll.alto;
			viewBox.yMax = _subescenaCajaBase.yMax + _subescenaScroll.alto;
			const NBCajaAABB contentBox = _subescenaObj->cajaAABBLocal();
			if(viewBox.xMin < contentBox.xMin || forceLeftTop || (contentBox.xMax - contentBox.xMin) <= (_subescenaCajaBase.xMax - _subescenaCajaBase.xMin)){
				_subescenaScroll.ancho = (contentBox.xMin - _subescenaCajaBase.xMin);
			} else if(viewBox.xMax > contentBox.xMax){
				_subescenaScroll.ancho = (contentBox.xMax - _subescenaCajaBase.xMax);
			}
			if(viewBox.yMax > contentBox.yMax || forceLeftTop || (contentBox.yMax - contentBox.yMin) <= (_subescenaCajaBase.yMax - _subescenaCajaBase.yMin)){
				_subescenaScroll.alto = (_subescenaCajaBase.yMax - contentBox.yMax);
			} else if(viewBox.yMin < contentBox.yMin){
				_subescenaScroll.alto = (contentBox.yMin - _subescenaCajaBase.yMin);
			}
		}
		//Aplicar nueva area visible
		NBGestorEscena::establecerCajaProyeccion(_subescenaId, _subescenaCajaBase.xMin + _subescenaScroll.ancho, _subescenaCajaBase.xMax + _subescenaScroll.ancho, _subescenaCajaBase.yMin + _subescenaScroll.alto, _subescenaCajaBase.yMax + _subescenaScroll.alto); //float xMin, float xMax, float yMin, float yMax
	}
}

//

void AUEscenaPanel::tickAnimacion(float segsTranscurridos){
	//Auto-dectectar cambio en el contenido y ajustar para que siempre sea visible
	{
		bool ajustarContenido = false;
		if(_subescenaObjUlt != _subescenaObj){
			ajustarContenido = true;
		}
		if(_subescenaObj != NULL){
			const NBCajaAABB contentBox = _subescenaObj->cajaAABBLocal();
			if(_subescenaObjCajaUlt != contentBox){
				ajustarContenido = true;
				_subescenaObjCajaUlt = contentBox;
			}
		}
		if(ajustarContenido){
			this->privScrollSubEscena(0.0f, 0.0f, false);
		}
		_subescenaObjUlt = _subescenaObj;
	}
	//Aplicar autoScroll
	if(_subescenaScrollAutoTouches == 0){
		if(_subescenaScrollAuto.ancho < -0.1f || _subescenaScrollAuto.ancho > 0.1f || _subescenaScrollAuto.alto < -0.1f || _subescenaScrollAuto.alto > 0.1f){
			_subescenaScrollAuto.ancho	*= 0.95f;
			_subescenaScrollAuto.alto	*= 0.95f;
			this->privScrollSubEscena(_subescenaScrollAuto.ancho, _subescenaScrollAuto.alto, false);
		}
	}
}

//TOUCHES

void AUEscenaPanel::touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto){
	if(objeto == _subescenaSprite){
		NBASSERT(_subescenaScrollAutoTouches >= 0)
		_subescenaScrollAuto.ancho	= 0.0f;
		_subescenaScrollAuto.alto	= 0.0f;
		_subescenaScrollAutoTouches++;
	}
}

void AUEscenaPanel::touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	if(objeto == _subescenaSprite){
		NBTamano scrollDelta;
		scrollDelta.ancho = (posAnteriorEscena.x - posActualEscena.x); //Inverted x-y
		scrollDelta.alto = (posAnteriorEscena.y - posActualEscena.y); //Inverted x-y
		_subescenaScrollAuto = scrollDelta;
		this->privScrollSubEscena(scrollDelta.ancho, scrollDelta.alto, false);
	}
}

void AUEscenaPanel::touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	if(objeto == _subescenaSprite){
		NBASSERT(_subescenaScrollAutoTouches > 0)
		_subescenaScrollAutoTouches--;
	}
	//
	const NBCajaAABB cajaEscena = objeto->cajaAABBEnEscena();
	if(NBCAJAAABB_INTERSECTA_PUNTO(cajaEscena, posActualEscena.x, posActualEscena.y)){
		if(objeto == _subescenaSprite){
			//
		}
	}
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaPanel, AUEscenaContenedor)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaPanel, "AUEscenaPanel", AUEscenaContenedor)
AUOBJMETODOS_CLONAR_NULL(AUEscenaPanel)





