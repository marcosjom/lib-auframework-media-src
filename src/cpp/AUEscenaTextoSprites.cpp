//
//  AUEscenaTextoSprites.cpp
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaTextoSprites.h"

AUEscenaTextoSprites::AUEscenaTextoSprites()
: AUEscenaContenedor()
, NBAnimador()
, _texto(this)
, _spritesRecicables(this)
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTextoSprites")
	_fuenteTextura				= NULL;
	_alineacionH				= ENNBTextLineAlignH_Left;
	_alineacionV				= ENNBTextAlignV_Bottom;
	_factorSeparacionCaracteres	= 1.0f;
	_factorSeparacionLineas		= 1.0f;
	NBTextMetrics_init(&_textMetrics);
	//
	this->detenerAnimacion();
}

AUEscenaTextoSprites::AUEscenaTextoSprites(AUFuenteTextura* fuenteTextura)
: AUEscenaContenedor()
, NBAnimador()
, _texto(this)
, _spritesRecicables(this)
{	//PENDIENTE, mejorar este proceso de clonado (clonar variables internas)
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTextoSprites")
	_fuenteTextura				= fuenteTextura; if(_fuenteTextura != NULL) _fuenteTextura->retener(NB_RETENEDOR_THIS);
	_alineacionH				= ENNBTextLineAlignH_Left;
	_alineacionV				= ENNBTextAlignV_Bottom;
	_factorSeparacionCaracteres	= 1.0f;
	_factorSeparacionLineas		= 1.0f;
	NBTextMetrics_init(&_textMetrics);
	//
	this->detenerAnimacion();
}

AUEscenaTextoSprites::AUEscenaTextoSprites(AUEscenaTextoSprites* otraInstancia)
: AUEscenaContenedor(otraInstancia)
, NBAnimador()
, _texto(this)
, _spritesRecicables(this)
{ //PENDIENTE, mejorar este proceso de clonado (clonar variables internas)
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTextoSprites")
	_fuenteTextura				= otraInstancia->_fuenteTextura; if(_fuenteTextura != NULL) _fuenteTextura->retener(NB_RETENEDOR_THIS);
	_alineacionH				= otraInstancia->_alineacionH;
	_alineacionV				= otraInstancia->_alineacionV;
	_factorSeparacionCaracteres	= otraInstancia->_factorSeparacionCaracteres;
	_factorSeparacionLineas		= otraInstancia->_factorSeparacionLineas;
	NBTextMetrics_init(&_textMetrics);
	//
	this->detenerAnimacion();
}

AUEscenaTextoSprites::~AUEscenaTextoSprites(){
	if(_fuenteTextura != NULL) _fuenteTextura->liberar(NB_RETENEDOR_THIS); _fuenteTextura = NULL;
	//
	UI32 i; const UI32 iConteo = _spritesRecicables.conteo;
	for(i=0; i<iConteo; i++) _spritesRecicables.elemento[i].sprite->liberar(NB_RETENEDOR_THIS);
	//
	NBTextMetrics_release(&_textMetrics);
}

//

AUFuenteTextura* AUEscenaTextoSprites::font() const {
	return _fuenteTextura;
}

ENNBTextLineAlignH AUEscenaTextoSprites::alineacionH() const {
	return (ENNBTextLineAlignH)_alineacionH;
}

ENNBTextAlignV AUEscenaTextoSprites::alineacionV() const {
	return (ENNBTextAlignV)_alineacionV;
}

float AUEscenaTextoSprites::factorSeparacionCaracteres() const {
	return _factorSeparacionCaracteres;
}

float AUEscenaTextoSprites::factorSeparacionLineas() const {
	return _factorSeparacionLineas;
}

const char* AUEscenaTextoSprites::texto() const {
	return _texto.str();
}

void AUEscenaTextoSprites::establecerAlineacionH(ENNBTextLineAlignH alineacionH){
	_alineacionH = alineacionH;
}

void AUEscenaTextoSprites::establecerAlineacionV(ENNBTextAlignV alineacionV){
	_alineacionV = alineacionV;
}

void AUEscenaTextoSprites::establecerAlineaciones(ENNBTextLineAlignH alineacionH, ENNBTextAlignV alineacionV){
	_alineacionH = alineacionH;
	_alineacionV = alineacionV;
}

void AUEscenaTextoSprites::establecerFactorSeparacionCaracteres(float factorSeparacionCaracteres){
	_factorSeparacionCaracteres	= factorSeparacionCaracteres; NBASSERT(false) //Probando si se usa
}

void AUEscenaTextoSprites::establecerFactorSeparacionLineas(float factorSeparacionLineas){
	_factorSeparacionLineas	= factorSeparacionLineas; NBASSERT(false) //Probando si se usa
}

STTextoSpriteChar* AUEscenaTextoSprites::privRegistroSpriteDisponible(){
	STTextoSpriteChar* registro = NULL;
	UI32 i, iConteo = _spritesRecicables.conteo;
	for(i=0; i<iConteo; i++){
		if(_spritesRecicables.elemento[i].estado == ENEscenaCharEstado_Libre){
			registro = &(_spritesRecicables.elemento[i]);
			break;
		}
	}
	//Crear nuevo registro
	if(registro == NULL){
		STTextoSpriteChar nuevoRegistro;
		nuevoRegistro.estado	= ENEscenaCharEstado_Libre;
		nuevoRegistro.animacion	= ENEscenaTextoAnimacion_Ninguna;
		nuevoRegistro.sprite	= new(this) AUEscenaSprite(); NB_DEFINE_NOMBRE_PUNTERO(nuevoRegistro.sprite, "AUEscenaTextoSprites::nuevoRegistro.sprite")
		_spritesRecicables.agregarElemento(nuevoRegistro);
		registro = &(_spritesRecicables.elemento[_spritesRecicables.conteo-1]);
	}
	return registro;
}

NBCajaAABB AUEscenaTextoSprites::cajaParaTexto(const char* texto, const float anchoLimite, const float altoLimite){
	const STNBAABox box = NBText_boxForText(&this->_textData, texto, anchoLimite, altoLimite);
	NBCajaAABB r;
	r.xMin = box.xMin;
	r.xMax = box.xMax;
	r.yMin = box.yMin;
	r.yMax = box.yMax;
	return r;
	//return AUEscenaTextoSprites::cajaParaTexto(texto, _fuenteTextura, (ENNBTextLineAlignH)_alineacionH, (ENNBTextAlignV)_alineacionV, anchoLimite, altoLimite);
}

NBCajaAABB AUEscenaTextoSprites::cajaParaTexto(const char* texto, AUFuenteTextura* objFuente, const ENNBTextLineAlignH alineacionH, const ENNBTextAlignV alineacionV, const float anchoLimite, const float altoLimite){
	NBASSERT(objFuente != NULL)
	NBCajaAABB cajaTexto; NBCAJAAABB_INICIALIZAR_CON_PUNTO(cajaTexto, 0, 0)
	STNBTextMetrics m;
	STNBTextMetricsBuilder mBldr;
	NBTextMetrics_init(&m);
	NBTextMetricsBuilder_init(&mBldr);
	//Feed formats
	{
		NBTextMetricsBuilder_pushFormat(&mBldr);
		objFuente->configureMetricsBuilder(&mBldr);
		NBTextMetricsBuilder_setFormat(&mBldr, NBST_P(STNBColor8, 0, 0, 0, 255 ), alineacionH, ENNBTextCharAlignV_Base);
		NBTextMetricsBuilder_appendBytes(<#STNBTextMetricsBuilder *obj#>, <#const char *str#>, <#const UI32 strSz#>)
		NBTextMetricsBuilder_add(&mBldr, texto);
		NBTextMetricsBuilder_popFormat(&mBldr);
	}
	//Feed columns
	{
		const STNBRect column = {
			(alineacionH == ENNBTextLineAlignH_Right ? -anchoLimite : alineacionH == ENNBTextLineAlignH_Center ? anchoLimite * -0.5f : 0.0f)
			, 0
			, anchoLimite
			, altoLimite
		};
		NBTextMetricsBuilder_feedStart(&mBldr, &m);
		NBTextMetricsBuilder_feed(&mBldr, column, &m);
		NBTextMetricsBuilder_feedEnd(&mBldr, &m);
	}
	//Calculations
	{
		SI32 iLinea; const SI32 cntLinea = m.lines.use;
		if(cntLinea > 0){
			//Alineacion vertical
			const STNBTextMetricsLine* firstLine = NBArray_itmPtrAtIndex(&m.lines, STNBTextMetricsLine, 0);
			const STNBTextMetricsLine* lastLine = NBArray_itmPtrAtIndex(&m.lines, STNBTextMetricsLine, cntLinea - 1);
			float despAlineaY   = 0.0f;
			switch (alineacionV) {
				case ENNBTextAlignV_Base:
					despAlineaY = firstLine->fontMetricsMax.ascender;
					break;
				case ENNBTextAlignV_FromBottom:
					despAlineaY = -(lastLine->yBase - lastLine->fontMetricsMax.descender); //descender is neg
					break;
				case ENNBTextAlignV_Center:
					despAlineaY = -(lastLine->yBase - lastLine->fontMetricsMax.descender) * 0.5f; //descender is neg
					break;
				default: /*ENNBTextAlignV_FromTop*/
					break;
			}
			//Lines max left/right
			float maxLeft = firstLine->visibleLeft, maxRight = firstLine->visibleRight;
			for(iLinea = 1; iLinea < cntLinea; iLinea++){
				const STNBTextMetricsLine* line = NBArray_itmPtrAtIndex(&m.lines, STNBTextMetricsLine, iLinea);
				if(maxLeft > line->visibleLeft) maxLeft = line->visibleLeft;
				if(maxRight < line->visibleRight) maxRight = line->visibleRight;
			}
			//
			float yMax	= despAlineaY + (lastLine->yBase - lastLine->fontMetricsMax.descender); //descender is negative
			float yMin	= despAlineaY;
			NBCAJAAABB_INICIALIZAR_CON_PUNTO(cajaTexto, maxLeft, yMin)
			NBCAJAAABB_ENVOLVER_PUNTO(cajaTexto, maxRight, yMax)
		}
	}
	NBTextMetrics_release(&m);
	NBTextMetricsBuilder_release(&mBldr);
	
	/*
	NBASSERT(objFuente != NULL)
	NBCajaAABB cajaTexto; NBCAJAAABB_INICIALIZAR_CON_PUNTO(cajaTexto, 0, 0)
	//
	NBRectangulo columna; NBRECTANGULO_ESTABLECER(columna, 0, 0, 9999999, 9999999)
	NBPunto posicion; NBPUNTO_ESTABLECER(posicion, 0, 0)
	STTxtOrgFormatoBloque formatoBloque; objFuente->configuraBloqueFormato(formatoBloque, 0, 9999999, ENNBTextLineAlignH_Left, ENNBTextCharAlignV_Base, 0.0f, 0.0f, 0.0f, 1.0f);
	AUArregloNativoMutableP<STTxtOrgChar>* orgDatosChars = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STTxtOrgChar>(AUCadena::tamano(texto));
	AUArregloNativoMutableP<STTxtOrgLinea>* orgDatosLineas = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STTxtOrgLinea>(16);
	NBGestorFuentes::organizacionTexto(texto, 0, columna, posicion, &formatoBloque, 1, orgDatosChars, orgDatosLineas, false/ *representar espacios* /);
	SI32 iLinea; const SI32 cntLinea = orgDatosLineas->conteo;
	if(cntLinea!=0){
		//Alineacion vertical
		STTxtOrgLinea* datLinea = &orgDatosLineas->elemento[cntLinea-1];
		float altoMax = datLinea->ySup + datLinea->ascendenteMax + datLinea->descendenteMax;
		float despAlineaY   = 0.0f;
		switch (alineacionV) {
			case ENNBTextAlignV_Base:
				despAlineaY = orgDatosLineas->elemento[0].ascendenteMax;
				break;
			case ENNBTextAlignV_Arriba:
				despAlineaY = altoMax;
				break;
			case ENNBTextAlignV_Centro:
				despAlineaY = (altoMax * 0.5f);
				break;
			default: / *ENNBTextAlignV_Bottom* /
				break;
		}
		//Alineacion Horizontal
		float anchoMax		= 0.0f; for(iLinea=0; iLinea<cntLinea; iLinea++) if(anchoMax < orgDatosLineas->elemento[iLinea].anchoContenido) anchoMax = orgDatosLineas->elemento[iLinea].anchoContenido;
		float despAlineaX	= 0.0f;
		switch (alineacionH) {
			case ENNBTextLineAlignH_Derecha:
				despAlineaX	= -anchoMax;
				break;
			case ENNBTextLineAlignH_Center:
				despAlineaX	= anchoMax * -0.5f;
				break;
			default: / *ENNBTextLineAlignH_Left ENNBTextLineAlignH_Base* /
				break;
		}
		float yMin	= despAlineaY - altoMax;
		float yMax	= despAlineaY;
		float xMin	= despAlineaX;
		float xMax	= despAlineaX + anchoMax;
		NBCAJAAABB_INICIALIZAR_CON_PUNTO(cajaTexto, xMin, yMin)
		NBCAJAAABB_ENVOLVER_PUNTO(cajaTexto, xMax, yMax)
	}
	orgDatosLineas->liberar(NB_RETENEDOR_NULL); orgDatosLineas = NULL;
	orgDatosChars->liberar(NB_RETENEDOR_NULL); orgDatosChars = NULL;
	*/
	//
	return cajaTexto;
}

void AUEscenaTextoSprites::establecerTexto(const char* texto, const ENEscenaTextoAnimacion animacion, const float anchoLimite, const float altoLimite){
	NBASSERT(_fuenteTextura != NULL)
	BOOL changed		= FALSE;
	const char* text	= NULL;
	UI32 textLen		= 0;
	//Establecer cadena actual
	_texto.establecer(texto);
	//Clear chars definition
	{
		SI32 iCar;
		for(iCar=_spritesRecicables.conteo-1; iCar>=0; iCar--){
			STTextoSpriteChar* datosSprite = &_spritesRecicables.elemento[iCar];
			if(datosSprite->estado!=ENEscenaCharEstado_Libre){
				if(animacion!=ENEscenaTextoAnimacion_Ninguna) {
					//Marcar animacion de quitado
					datosSprite->estado		= ENEscenaCharEstado_Quitando;
					datosSprite->animacion	= animacion;
				} else {
					//Quitar directamente
					datosSprite->estado		= ENEscenaCharEstado_Libre;
					datosSprite->animacion	= ENEscenaTextoAnimacion_Ninguna;
					this->quitarObjetoEscena(datosSprite->sprite);
				}
			}
		}
	}
	NBASSERT(_fuenteTextura != NULL)
	if(_fuenteTextura != NULL){
		NBTextMetrics_empty(&_textMetrics);
		//Calculate chars definition
		STNBTextMetricsBuilder mBldr;
		NBTextMetricsBuilder_init(&mBldr);
		//Feed formats
		{
			NBTextMetricsBuilder_pushFormat(&mBldr);
			_fuenteTextura->configureMetricsBuilder(&mBldr);
			NBTextMetricsBuilder_setFormat(&mBldr, NBST_P(STNBColor8, 0, 0, 0, 255 ), (ENNBTextLineAlignH)_alineacionH, ENNBTextCharAlignV_Base);
			NBTextMetricsBuilder_add(&mBldr, texto);
			NBTextMetricsBuilder_popFormat(&mBldr);
		}
		//Feed columns
		{
			const STNBRect column = {
				(_alineacionH == ENNBTextLineAlignH_Right ? -anchoLimite : _alineacionH == ENNBTextLineAlignH_Center ? anchoLimite * -0.5f : 0.0f)
				, 0
				, anchoLimite
				, altoLimite
			};
			NBTextMetricsBuilder_feedStart(&mBldr, &_textMetrics);
			NBTextMetricsBuilder_feed(&mBldr, column, &_textMetrics);
			NBTextMetricsBuilder_feedEnd(&mBldr, &_textMetrics);
		}
		//Calculations
		{
			const SI32 cntLinea = _textMetrics.lines.use;
			if(cntLinea > 0){
				//Alineacion vertical
				const STNBTextMetricsLine* firstLine = NBArray_itmPtrAtIndex(&_textMetrics.lines, STNBTextMetricsLine, 0);
				const STNBTextMetricsLine* lastLine = NBArray_itmPtrAtIndex(&_textMetrics.lines, STNBTextMetricsLine, cntLinea - 1);
				float despAlineaY   = 0.0f;
				switch(_alineacionV) {
					case ENNBTextAlignV_Base:
						despAlineaY = firstLine->fontMetricsMax.ascender;
						break;
					case ENNBTextAlignV_FromBottom:
						despAlineaY = -(lastLine->yBase - lastLine->fontMetricsMax.descender); //descender is neg
						break;
					case ENNBTextAlignV_Center:
						despAlineaY = -(lastLine->yBase - lastLine->fontMetricsMax.descender) * 0.5f; //descender is neg
						break;
					default: /*ENNBTextAlignV_FromTop*/
						break;
				}
			}
		}
		//Characters
		{
			SI32 i; const SI32 cntChars = _textMetrics.chars.use;
			for(i = 0; i < cntChars; i++){
				const STNBTextMetricsChar* c = NBArray_itmPtrAtIndex(&_textMetrics.chars, STNBTextMetricsChar, i);
				NBASSERT(_fuenteTextura == c->itfObj)
				AUFuenteTextura* fnt = (AUFuenteTextura*)c->itfObj;
				STFuenteVisualChar rc;
				NBMemory_setZeroSt(rc, STFuenteVisualChar);
				if(fnt->charMetricToRenderChar(c, &rc)){ //false when isControlChar
					STTextoSpriteChar* datosSprite = this->privRegistroSpriteDisponible();
					if(animacion != ENEscenaTextoAnimacion_Ninguna){
						datosSprite->estado		= ENEscenaCharEstado_Entering;
						datosSprite->animacion	= animacion;
						datosSprite->sprite->establecerMultiplicadorColor8(255, 255, 255, 0);
					} else {
						datosSprite->estado		= ENEscenaCharEstado_Presentando;
						datosSprite->animacion	= ENEscenaTextoAnimacion_Ninguna;
						datosSprite->sprite->establecerMultiplicadorColor8(255, 255, 255, 255);
					}
					datosSprite->sprite->establecerTextura(rc.texturaSprite); NBASSERT(rc.texturaSprite->esClase(AUTextura::idTipoClase))
					datosSprite->sprite->redimensionar(0.0f, 0.0f, rc.areaSprite.ancho, rc.areaSprite.alto);
					datosSprite->sprite->establecerTraslacion(rc.areaSprite.x, rc.areaSprite.y);
					this->agregarObjetoEscena(datosSprite->sprite);
				}
			}
		}
		//Reanudar tick de animaciones
		if(animacion != ENEscenaTextoAnimacion_Ninguna){
			this->reanudarAnimacion();
		}
		//
		NBTextMetricsBuilder_release(&mBldr);
		//
		text	= _texto.str();
		textLen	= _texto.tamano();
		changed	= TRUE;
	}
	/*if(changed){
		float defAsc = 0.0f, defDesc = 0.0f;
		if(_fuenteTextura != NULL){
			defAsc	= _fuenteTextura->ascendenteEscena();
			defDesc	= _fuenteTextura->descendenteEscena();
		}
		_lstnr->textMetricsChanged(this, &_textMetrics, text, textLen);
	}*/
}

//

void AUEscenaTextoSprites::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaTextoSprites::agregadoEnEscena")
	NBGestorAnimadores::agregarAnimador(this, this);
	AUEscenaContenedor::agregadoEnEscena();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaTextoSprites::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaTextoSprites::quitandoDeEscena")
	AUEscenaContenedor::quitandoDeEscena();
	NBGestorAnimadores::quitarAnimador(this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaTextoSprites::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaTextoSprites::quitandoDeEscena")
	if(this->_dormido){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3() }
	SI32 iCar, conteoCharAnimados = 0;
	for(iCar=_spritesRecicables.conteo-1; iCar>=0; iCar--){
		STTextoSpriteChar* datosSprite = &(_spritesRecicables.elemento[iCar]);
		if(datosSprite->estado == ENEscenaCharEstado_Entering){
			if(datosSprite->animacion == ENEscenaTextoAnimacion_Alpha){
				NBColor8 color8Actual = datosSprite->sprite->_propiedades.color8;
				NBColor colorActual; NBCOLOR_ESTABLECER_DESDE_UI8(colorActual, color8Actual)
				colorActual.a += (segsTranscurridos * 4.0f);
				if(colorActual.a > 1.0f){
					colorActual.a = 1.0f;
					datosSprite->estado = ENEscenaCharEstado_Presentando;
				}
				datosSprite->sprite->establecerMultiplicadorAlpha8(255.0f * colorActual.a);
				conteoCharAnimados++;
			} else {
				NBASSERT(false); //Animancion no valida
			}
		} else if(datosSprite->estado == ENEscenaCharEstado_Quitando){
			if(datosSprite->animacion == ENEscenaTextoAnimacion_Alpha){
				NBColor8 color8Actual = datosSprite->sprite->_propiedades.color8;
				NBColor colorActual; NBCOLOR_ESTABLECER_DESDE_UI8(colorActual, color8Actual)
				colorActual.a -= (segsTranscurridos * 4.0f);
				if(colorActual.a < 0.0f){
					colorActual.a = 0.0f;
					datosSprite->estado = ENEscenaCharEstado_Libre;
					this->quitarObjetoEscena(datosSprite->sprite);
				}
				datosSprite->sprite->establecerMultiplicadorAlpha8(255.0f * colorActual.a);
				conteoCharAnimados++;
			} else {
				NBASSERT(false); //Animancion no valida
			}
		}
	}
	if(conteoCharAnimados == 0) this->detenerAnimacion();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaTextoSprites, AUEscenaContenedor)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaTextoSprites, "AUEscenaTextoSprites", AUEscenaContenedor)
AUOBJMETODOS_CLONAR_THIS(AUEscenaTextoSprites)







