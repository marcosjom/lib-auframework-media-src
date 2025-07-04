//
//  AUEscenaAnimacionI.cpp
//  AUFramework
//
//  Created by Marcos Ortega Morales on 11/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaAnimacionI.h"


AUEscenaAnimacionI::AUEscenaAnimacionI(AUAnimacionBibliotecaDef* plantillaBiblioteca, AUAnimacionDef* plantillaAnimacion) : AUEscenaContenedor(), NBAnimador()
#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
, _sonidosEnEscena(this), _animadoresSonidosEscena(this)
#endif
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaAnimacionI")
	//Compartido
	_plantillaBiblioteca			= plantillaBiblioteca;		if(_plantillaBiblioteca != NULL) _plantillaBiblioteca->retener(NB_RETENEDOR_THIS);
	_plantillaAnimacion				= plantillaAnimacion;		if(_plantillaAnimacion != NULL) _plantillaAnimacion->retener(NB_RETENEDOR_THIS);
	//Reproduccion
	_mascaraProps					= AUESCENAANIMACIONI_BIT_REPRODUCIR | AUESCENAANIMACIONI_BIT_REPETIR | AUESCENAANIMACIONI_BIT_REP_SONIDOS;
	_factorVelocidad				= 1.0f;
	_segundosEsperarParaRepetir		= 0.0f;
	_segundosAcumuladosAnim			= 0.0f;
	_segundosAcumuladosParaRepetir	= 0.0f;
	_indiceFrameInicio				= 0;
	_indiceFrameActual				= 0;
	_indiceFrameUltEventosSonidosEjecutados = -1;
	//Escucha
	_escuchador						= NULL;
	//
	this->_animacionEnEjecucion		= false;
}

AUEscenaAnimacionI::AUEscenaAnimacionI(AUEscenaAnimacionI* otraInstancia) : AUEscenaContenedor(otraInstancia), NBAnimador()
#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
, _sonidosEnEscena(this), _animadoresSonidosEscena(this)
#endif
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaAnimacionI")
	//Compartido
	_plantillaBiblioteca			= otraInstancia->_plantillaBiblioteca;		if(_plantillaBiblioteca != NULL) _plantillaBiblioteca->retener(NB_RETENEDOR_THIS);
	_plantillaAnimacion				= otraInstancia->_plantillaAnimacion;		if(_plantillaAnimacion != NULL) _plantillaAnimacion->retener(NB_RETENEDOR_THIS);
	//Reproduccion
	_mascaraProps					= otraInstancia->_mascaraProps;
	_factorVelocidad				= otraInstancia->_factorVelocidad;
	_segundosEsperarParaRepetir		= otraInstancia->_segundosEsperarParaRepetir;
	_segundosAcumuladosAnim			= 0.0f;
	_segundosAcumuladosParaRepetir	= 0.0f;
	_indiceFrameInicio				= otraInstancia->_indiceFrameInicio;
	_indiceFrameActual				= 0;
	_indiceFrameUltEventosSonidosEjecutados = -1;
	//Escucha
	_escuchador						= NULL;
	//
	this->_animacionEnEjecucion		= false;
}

AUEscenaAnimacionI::~AUEscenaAnimacionI(){
	if(_plantillaBiblioteca != NULL) _plantillaBiblioteca->liberar(NB_RETENEDOR_THIS); _plantillaBiblioteca = NULL;
	if(_plantillaAnimacion != NULL) _plantillaAnimacion->liberar(NB_RETENEDOR_THIS); _plantillaAnimacion = NULL;
}

//Animacion

AUAnimacionBibliotecaDef* AUEscenaAnimacionI::plantillaBiblioteca(){
	return _plantillaBiblioteca;
}

AUAnimacionDef* AUEscenaAnimacionI::plantillaAnimacion(){
	return _plantillaAnimacion;
}

const char* AUEscenaAnimacionI::rutaArchivoOrigen(){
	return _plantillaBiblioteca->rutaRecurso.str();
}

NBCajaAABB AUEscenaAnimacionI::cajaLienzoAnimacion(){
	return _plantillaAnimacion->cajaLienzo;
}

FLOAT AUEscenaAnimacionI::fps(){
	return _plantillaBiblioteca->fps;
}

UI32 AUEscenaAnimacionI::frames(){
	return _plantillaAnimacion->frames;
}

FLOAT AUEscenaAnimacionI::segundosDuracionNormal(){
	return (float)_plantillaAnimacion->frames / _plantillaBiblioteca->fps;
}

AUArreglo* AUEscenaAnimacionI::indicesPropiedadesPorFrame(){
	return &_plantillaAnimacion->iPropiedadesPorFrame;
}

ENAnimacion_TipoEncapsulamiento AUEscenaAnimacionI::tipoEncapsulamientoHijos(){
	return _plantillaBiblioteca->tipoEncapsulamientoHijos;
}

bool AUEscenaAnimacionI::actualmenteEnUltimoFrameAnimacionSinRepetir(){
	//Determina si, al dar reanudarAnimacion no se producira movimiento (debido a que la animacion no repite y se encuetra en su ultimo frame)
	bool enUltimo = false;
	if((_mascaraProps & AUESCENAANIMACIONI_BIT_REPETIR)==0 && _plantillaAnimacion->frames>1){ //No repite y tiene mas de 1 frame
		enUltimo = (_indiceFrameActual==0 && _factorVelocidad<0.0f) || (_indiceFrameActual==(SI16)_plantillaAnimacion->frames-1 && _factorVelocidad>=0.0f);
	}
	return enUltimo;
}

//Reproduccion

float AUEscenaAnimacionI::factorVelocidadReproduccion(){
	return _factorVelocidad;
}

void AUEscenaAnimacionI::establecerFactorVelocidadReproduccion(float factorVelocidad){
	_factorVelocidad = factorVelocidad;
}

float AUEscenaAnimacionI::segundosEsperarParaRepetir(){
	return _segundosEsperarParaRepetir;
}

void AUEscenaAnimacionI::establecerSegundosEsperarParaRepetir(float segundosEsperarParaRepetir){
	_segundosEsperarParaRepetir = segundosEsperarParaRepetir; NBASSERT(_segundosEsperarParaRepetir>=0.0f)
}

bool AUEscenaAnimacionI::reproduciendoSonidos(){
	return (_mascaraProps & AUESCENAANIMACIONI_BIT_REP_SONIDOS)!=0;
}

void AUEscenaAnimacionI::establecerReproducirSonidos(bool reproducirSonidos){
	if(reproducirSonidos){
		_mascaraProps |= AUESCENAANIMACIONI_BIT_REP_SONIDOS;
	} else {
		_mascaraProps &= ~AUESCENAANIMACIONI_BIT_REP_SONIDOS;
	}
}

bool AUEscenaAnimacionI::reproducirAlCargar(){
	return (_mascaraProps & AUESCENAANIMACIONI_BIT_REPRODUCIR);
}

void AUEscenaAnimacionI::establecerReproducirAlCargar(bool reproducirAlCargar){
	if(reproducirAlCargar){
		_mascaraProps |= AUESCENAANIMACIONI_BIT_REPRODUCIR;
	} else {
		_mascaraProps &= ~AUESCENAANIMACIONI_BIT_REPRODUCIR;
	}
}

bool AUEscenaAnimacionI::suavizandoAnimacion(){
	return (_mascaraProps & AUESCENAANIMACIONI_BIT_SUAVIZAR)!=0;
}

void AUEscenaAnimacionI::establecerSuavizarAnimacion(bool suavizar){
	if(suavizar){
		_mascaraProps |= AUESCENAANIMACIONI_BIT_SUAVIZAR;
	} else {
		_mascaraProps &= ~AUESCENAANIMACIONI_BIT_SUAVIZAR;
	}
    //Suavizar animaciones hijas
    int iElem;
	for(iElem=0; iElem<this->_objetosEscena.conteo; iElem++){
		AUEscenaObjeto* hijo	= (AUEscenaObjeto*)this->_objetosEscena.elemento[iElem];
		if(_plantillaBiblioteca->tipoEncapsulamientoHijos==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
			hijo				= (AUEscenaObjeto*)((AUEscenaContenedor*)hijo)->hijos()->elemento[0];
		}
		if(hijo->esClase(AUEscenaAnimacionI::idTipoClase)){
			((AUEscenaAnimacionI*)hijo)->establecerSuavizarAnimacion(suavizar);
		}
	}
}

void AUEscenaAnimacionI::establecerFrameInicio(UI16 frameInicio){
	NBASSERT(frameInicio<_plantillaAnimacion->frames)
	_indiceFrameInicio = frameInicio;
}

SI32 AUEscenaAnimacionI::indiceFrameInicio(){
	return _indiceFrameInicio;
}

SI32 AUEscenaAnimacionI::indiceFrameActual(){
	return _indiceFrameActual;
}

float AUEscenaAnimacionI::indiceFrameIntermedioActual(){
	float iFrame = 0.0f;
	if(_plantillaAnimacion->frames != 0){
		iFrame = (float)_indiceFrameActual;
		iFrame += (_segundosAcumuladosAnim / (1.0f / _plantillaBiblioteca->fps));
		while (iFrame < 0.0f) iFrame += _plantillaAnimacion->frames;
		while (iFrame >= _plantillaAnimacion->frames) iFrame -= _plantillaAnimacion->frames;
	}
	return iFrame;
}

bool AUEscenaAnimacionI::presentarFrame(UI16 indiceFrame){
	bool exito = false;
	if(_plantillaAnimacion->frames!=0){
		while(indiceFrame<0.0f) indiceFrame += _plantillaAnimacion->frames;
		while(indiceFrame>=_plantillaAnimacion->frames) indiceFrame -= _plantillaAnimacion->frames;
		privInlineActualizarEstadoSubanimacionesDeFrame(indiceFrame);
		privInlineEstablecerPropiedadesDeFrame(indiceFrame);
		_indiceFrameActual = indiceFrame;
		exito = true;
	}
	return exito;
}

bool AUEscenaAnimacionI::presentarFrameIntermedio(float iFrame){
	bool exito = false;
	if(_plantillaAnimacion->frames!=0){
		while(iFrame<0.0f) iFrame += _plantillaAnimacion->frames;
		while(iFrame>=_plantillaAnimacion->frames) iFrame -= _plantillaAnimacion->frames;
		const UI16 indiceFrame = (const UI16)iFrame;
		privInlineActualizarEstadoSubanimacionesDeFrame(indiceFrame);
		privInlineEstablecerPropiedadesEntreFrames(indiceFrame, (indiceFrame + 1)<_plantillaAnimacion->frames? (indiceFrame + 1):0, iFrame - (float)((UI16)iFrame));
		//privInlineEstablecerPropiedadesDeFrame(indiceFrame);
		_indiceFrameActual = indiceFrame;
		exito = true;
	}
	return exito;
}

bool AUEscenaAnimacionI::repitiendoAnimacion(){
	return (_mascaraProps & AUESCENAANIMACIONI_BIT_REPETIR)!=0;
}

void AUEscenaAnimacionI::establecerRepetirAnimacion(bool repetir){
	if(repetir){
		_mascaraProps |= AUESCENAANIMACIONI_BIT_REPETIR;
	} else {
		_mascaraProps &= ~AUESCENAANIMACIONI_BIT_REPETIR;
	}
}

void AUEscenaAnimacionI::privInlineActualizarEstadoSubanimacionesDeFrame(UI32 indiceFrame){
	UI32 iElem, conteoElemsAnimados = ((AUArregloNativoMutableP<STAnimacionIndicesPropiedades>*)_plantillaAnimacion->iPropiedadesPorFrame.elemento[indiceFrame])->conteo;
	NBASSERT(conteoElemsAnimados<=_objetosEscena.conteo)
	if(_plantillaBiblioteca->tipoEncapsulamientoHijos==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
		for(iElem=0; iElem<conteoElemsAnimados; iElem++){
			AUEscenaObjeto* hijo					= (AUEscenaObjeto*)((AUEscenaContenedor*)_objetosEscena.elemento[iElem])->hijos()->elemento[0];
			STAnimacionIndicesPropiedades indicesPropiedades = ((AUArregloNativoMutableP<STAnimacionIndicesPropiedades>*)_plantillaAnimacion->iPropiedadesPorFrame.elemento[indiceFrame])->elemento[iElem];
			bool estadoVisible						= !(indicesPropiedades.iTraslacion==-1 || indicesPropiedades.iColor==-1);
			if(!hijo->visible() && estadoVisible){
				//Rebobinar animaciones que se estan presentando
				if(hijo->esClase(AUEscenaAnimacionI::idTipoClase)){
					((AUEscenaAnimacionI*)hijo)->reanudarAnimacion();
					((AUEscenaAnimacionI*)hijo)->rebobinarAnimacion();
				}
			} else if(hijo->visible() && !estadoVisible){
				//Detener subanimaciones
				if(hijo->esClase(AUEscenaAnimacionI::idTipoClase)){
					((AUEscenaAnimacionI*)hijo)->detenerAnimacion();
				}
			}
		}
	} else { //_tipoEncapsulamientoHijos==ENAnimacion_TipoEncapsulamiento_UnContenedorNormal
		for(iElem=0; iElem<conteoElemsAnimados; iElem++){
			AUEscenaObjeto* hijo = (AUEscenaObjeto*)_objetosEscena.elemento[iElem];
			STAnimacionIndicesPropiedades indicesPropiedades = ((AUArregloNativoMutableP<STAnimacionIndicesPropiedades>*)_plantillaAnimacion->iPropiedadesPorFrame.elemento[indiceFrame])->elemento[iElem];
			bool estadoVisible						= !(indicesPropiedades.iTraslacion==-1 || indicesPropiedades.iColor==-1);
			if(!hijo->visible() && estadoVisible){
				//Rebobinar animaciones que se estan presentando
				if(hijo->esClase(AUEscenaAnimacionI::idTipoClase)){
					((AUEscenaAnimacionI*)hijo)->reanudarAnimacion();
					((AUEscenaAnimacionI*)hijo)->rebobinarAnimacion();
				}
			} else if(hijo->visible() && !estadoVisible){
				//Detener subanimaciones
				if(hijo->esClase(AUEscenaAnimacionI::idTipoClase)){
					((AUEscenaAnimacionI*)hijo)->detenerAnimacion();
				}
			}
		}
	}
}

void AUEscenaAnimacionI::privInlineEstablecerPropiedadesDeFrame(UI32 indiceFrame){
	NBASSERT(indiceFrame<_plantillaAnimacion->frames)
	int iElem, conteoElemsAnimados = ((AUArregloNativoMutableP<STAnimacionIndicesPropiedades>*)_plantillaAnimacion->iPropiedadesPorFrame.elemento[indiceFrame])->conteo;
	NBASSERT(conteoElemsAnimados<=_objetosEscena.conteo)
	if(_plantillaBiblioteca->tipoEncapsulamientoHijos==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
		for(iElem=0; iElem<conteoElemsAnimados; iElem++){
			AUEscenaContenedor* encapsulador		= (AUEscenaContenedor*)_objetosEscena.elemento[iElem];
			AUEscenaObjeto* hijo					= (AUEscenaObjeto*)encapsulador->hijos()->elemento[0];
			STAnimacionIndicesPropiedades indicesPropiedades = ((AUArregloNativoMutableP<STAnimacionIndicesPropiedades>*)_plantillaAnimacion->iPropiedadesPorFrame.elemento[indiceFrame])->elemento[iElem];
			if(indicesPropiedades.iTraslacion==-1 || indicesPropiedades.iColor==-1){
				hijo->establecerVisible(false);
			} else {
				hijo->establecerVisible(true);
				hijo->establecerTraslacion(_plantillaBiblioteca->traslaciones.elemento[indicesPropiedades.iTraslacion]);
				hijo->establecerEscalacion(_plantillaBiblioteca->escalaciones.elemento[indicesPropiedades.iEscalacion]);
				hijo->establecerRotacion(_plantillaBiblioteca->rotaciones.elemento[indicesPropiedades.iRotacion]);
				hijo->establecerMultiplicadorColor8(_plantillaBiblioteca->colores.elemento[indicesPropiedades.iColor]);
			}
		}
	} else { //_tipoEncapsulamientoHijos==ENAnimacion_TipoEncapsulamiento_UnContenedorNormal
		for(iElem=0; iElem<conteoElemsAnimados; iElem++){
			AUEscenaObjeto* hijo = (AUEscenaObjeto*)_objetosEscena.elemento[iElem];
			STAnimacionIndicesPropiedades indicesPropiedades = ((AUArregloNativoMutableP<STAnimacionIndicesPropiedades>*)_plantillaAnimacion->iPropiedadesPorFrame.elemento[indiceFrame])->elemento[iElem];
			if(indicesPropiedades.iTraslacion==-1 || indicesPropiedades.iColor==-1){
				hijo->establecerVisible(false);
			} else {
				hijo->establecerVisible(true);
				hijo->establecerTraslacion(_plantillaBiblioteca->traslaciones.elemento[indicesPropiedades.iTraslacion]);
				hijo->establecerEscalacion(_plantillaBiblioteca->escalaciones.elemento[indicesPropiedades.iEscalacion]);
				hijo->establecerRotacion(_plantillaBiblioteca->rotaciones.elemento[indicesPropiedades.iRotacion]);
				hijo->establecerMultiplicadorColor8(_plantillaBiblioteca->colores.elemento[indicesPropiedades.iColor]);
			}
		}
	}
}

void AUEscenaAnimacionI::privInlineEstablecerPropiedadesEntreFrames(UI32 indiceFrameIni, UI32 indiceFrameFin, float factorAvance){
	NBASSERT(indiceFrameIni!=indiceFrameFin)
	NBASSERT(indiceFrameIni<_plantillaAnimacion->frames)
	NBASSERT(indiceFrameFin<_plantillaAnimacion->frames)
	SI32 iElem, conteoElemsAnimados = ((AUArregloNativoMutableP<STAnimacionIndicesPropiedades>*)_plantillaAnimacion->iPropiedadesPorFrame.elemento[indiceFrameIni])->conteo;
	NBASSERT(conteoElemsAnimados<=_objetosEscena.conteo)
	for(iElem=0; iElem<conteoElemsAnimados; iElem++){
		AUEscenaObjeto* hijo;
		if(_plantillaBiblioteca->tipoEncapsulamientoHijos==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
			hijo								= (AUEscenaObjeto*)((AUEscenaContenedor*)_objetosEscena.elemento[iElem])->hijos()->elemento[0];
		} else {
			hijo								= (AUEscenaObjeto*)_objetosEscena.elemento[iElem];
		}
        bool visibleAntes = false, visibleDespues = false;
		NBPropiedadesEnEscena propiedadesAntes;
		NBPropiedadesEnEscena propiedadesDespues;
		STAnimacionIndicesPropiedades indicesPropiedadesAntes = ((AUArregloNativoMutableP<STAnimacionIndicesPropiedades>*)_plantillaAnimacion->iPropiedadesPorFrame.elemento[indiceFrameIni])->elemento[iElem];
		STAnimacionIndicesPropiedades indicesPropiedadesDespues = ((AUArregloNativoMutableP<STAnimacionIndicesPropiedades>*)_plantillaAnimacion->iPropiedadesPorFrame.elemento[indiceFrameFin])->elemento[iElem];
		if(indicesPropiedadesAntes.iTraslacion==-1 || indicesPropiedadesAntes.iColor==-1){
			visibleAntes					= false;
		} else {
			visibleAntes					= true;
			propiedadesAntes.transformaciones.trasladoX	= _plantillaBiblioteca->traslaciones.elemento[indicesPropiedadesAntes.iTraslacion].x;
			propiedadesAntes.transformaciones.trasladoY	= _plantillaBiblioteca->traslaciones.elemento[indicesPropiedadesAntes.iTraslacion].y;
			propiedadesAntes.transformaciones.escalaX	= _plantillaBiblioteca->escalaciones.elemento[indicesPropiedadesAntes.iEscalacion].ancho;
			propiedadesAntes.transformaciones.escalaY	= _plantillaBiblioteca->escalaciones.elemento[indicesPropiedadesAntes.iEscalacion].alto;
			propiedadesAntes.transformaciones.rotacion	= _plantillaBiblioteca->rotaciones.elemento[indicesPropiedadesAntes.iRotacion];
			propiedadesAntes.color8						= _plantillaBiblioteca->colores.elemento[indicesPropiedadesAntes.iColor];
		}
		if(indicesPropiedadesDespues.iTraslacion==-1 || indicesPropiedadesDespues.iColor==-1){
			visibleDespues						= false;
		} else {
			visibleDespues						= true;
			propiedadesDespues.transformaciones.trasladoX	= _plantillaBiblioteca->traslaciones.elemento[indicesPropiedadesDespues.iTraslacion].x;
			propiedadesDespues.transformaciones.trasladoY	= _plantillaBiblioteca->traslaciones.elemento[indicesPropiedadesDespues.iTraslacion].y;
			propiedadesDespues.transformaciones.escalaX		= _plantillaBiblioteca->escalaciones.elemento[indicesPropiedadesDespues.iEscalacion].ancho;
			propiedadesDespues.transformaciones.escalaY		= _plantillaBiblioteca->escalaciones.elemento[indicesPropiedadesDespues.iEscalacion].alto;
			propiedadesDespues.transformaciones.rotacion	= _plantillaBiblioteca->rotaciones.elemento[indicesPropiedadesDespues.iRotacion];
			propiedadesDespues.color8			= _plantillaBiblioteca->colores.elemento[indicesPropiedadesDespues.iColor];
		}
		if(visibleAntes || visibleDespues){
            bool visibleIntermedio = false;
			NBPropiedadesEnEscena propiedadesIntermedias;
			if(!visibleAntes){
				propiedadesAntes.transformaciones	= propiedadesDespues.transformaciones;
				propiedadesAntes.color8				= propiedadesDespues.color8;
				propiedadesAntes.color8.a			= 0;
			} else if(!visibleDespues){
				propiedadesDespues.transformaciones	= propiedadesAntes.transformaciones;
				propiedadesDespues.color8			= propiedadesAntes.color8;
				propiedadesDespues.color8.a			= 0;
			} else if((propiedadesAntes.transformaciones.escalaX<0.0f && propiedadesAntes.transformaciones.escalaY<0.0f && propiedadesDespues.transformaciones.escalaX>0.0f && propiedadesDespues.transformaciones.escalaY>0.0f) || (propiedadesAntes.transformaciones.escalaX>0.0f && propiedadesAntes.transformaciones.escalaY>0.0f && propiedadesDespues.transformaciones.escalaX<0.0f && propiedadesDespues.transformaciones.escalaY<0.0f)){
				//Nota: al interpretar las transformaciones de flash, cada 180 grados que un objeto rota, este es invertido en ambas escalas (x) y (y) y rotado +180 grados.
				//Caso 1: Valores anteriores escala(~-1, ~-1) rotacion(~-90) valores posteriores escala(~+1, ~+1) rotacion(~+90)
				//Caso 2: Valores anteriores escala(~+1, ~+1) rotacion(~-90) valores posteriores escala(~-1, ~-1) rotacion(~+90)
				if(propiedadesAntes.transformaciones.rotacion<0.0f && propiedadesDespues.transformaciones.rotacion>0.0f){
					propiedadesDespues.transformaciones.rotacion -= 180.0f;
					propiedadesDespues.transformaciones.escalaX = -propiedadesDespues.transformaciones.escalaX;
					propiedadesDespues.transformaciones.escalaY = -propiedadesDespues.transformaciones.escalaY;
				} else if(propiedadesAntes.transformaciones.rotacion>0.0f && propiedadesDespues.transformaciones.rotacion<0.0f){
					propiedadesDespues.transformaciones.rotacion += 180.0f;
					propiedadesDespues.transformaciones.escalaX = -propiedadesDespues.transformaciones.escalaX;
					propiedadesDespues.transformaciones.escalaY = -propiedadesDespues.transformaciones.escalaY;
				}
			}
			float difRotacion									= (propiedadesDespues.transformaciones.rotacion-propiedadesAntes.transformaciones.rotacion);
			if(difRotacion<-180.0f) difRotacion					+= 360.0f;
			else if(difRotacion>180.0f) difRotacion				-= 360.0f;
			NBASSERT(difRotacion>=-180.0f && difRotacion<=180.0f);
            visibleIntermedio						            = true;
			propiedadesIntermedias.transformaciones.trasladoX	= propiedadesAntes.transformaciones.trasladoX+((propiedadesDespues.transformaciones.trasladoX-propiedadesAntes.transformaciones.trasladoX)*factorAvance);
			propiedadesIntermedias.transformaciones.trasladoY	= propiedadesAntes.transformaciones.trasladoY+((propiedadesDespues.transformaciones.trasladoY-propiedadesAntes.transformaciones.trasladoY)*factorAvance);
			propiedadesIntermedias.transformaciones.rotacion	= propiedadesAntes.transformaciones.rotacion+(difRotacion*factorAvance);
			propiedadesIntermedias.transformaciones.escalaX		= propiedadesAntes.transformaciones.escalaX+((propiedadesDespues.transformaciones.escalaX-propiedadesAntes.transformaciones.escalaX)*factorAvance);
			propiedadesIntermedias.transformaciones.escalaY		= propiedadesAntes.transformaciones.escalaY+((propiedadesDespues.transformaciones.escalaY-propiedadesAntes.transformaciones.escalaY)*factorAvance);
			propiedadesIntermedias.color8.r						= (SI16)propiedadesAntes.color8.r+(((SI16)propiedadesDespues.color8.r-(SI16)propiedadesAntes.color8.r)*factorAvance);
			propiedadesIntermedias.color8.g						= (SI16)propiedadesAntes.color8.g+(((SI16)propiedadesDespues.color8.g-(SI16)propiedadesAntes.color8.g)*factorAvance);
			propiedadesIntermedias.color8.b						= (SI16)propiedadesAntes.color8.b+(((SI16)propiedadesDespues.color8.b-(SI16)propiedadesAntes.color8.b)*factorAvance);
			propiedadesIntermedias.color8.a						= (SI16)propiedadesAntes.color8.a+(((SI16)propiedadesDespues.color8.a-(SI16)propiedadesAntes.color8.a)*factorAvance);
			//PRINTF_INFO("Escala antes %d (%f, %f) escala despues %d (%f, %f): en factor (%f) (%f, %f) ROTACION ANTES(%f) DESPUES(%f)\n", indiceFrameIni, propiedadesAntes.transformaciones.escalaX, propiedadesAntes.transformaciones.escalaY, indiceFrameFin, propiedadesDespues.transformaciones.escalaX, propiedadesDespues.transformaciones.escalaY, factorAvance, propiedadesIntermedias.transformaciones.escalaX, propiedadesIntermedias.transformaciones.escalaY, propiedadesAntes.transformaciones.rotacion, propiedadesDespues.transformaciones.rotacion);
			hijo->establecerPropiedades(visibleIntermedio, propiedadesIntermedias);
		} else {
			hijo->establecerVisible(false);
		}
	}
}

void AUEscenaAnimacionI::privInlineDetenerSonidos(){
#	ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
	while(_sonidosEnEscena.conteo>0){
		AUEscenaSnd* sonido = (AUEscenaSnd*)_sonidosEnEscena.elemento[0];
		sonido->establecerRepetir(false);
		sonido->detenerSonido();
		//PRINTF_INFO("Limpiando sonido que ya no esta en reproduccion indice(%d)\n", iSonidoVivo);
		privInlineQuitarSonidoEscenaPorIndice(sonido, 0);
	}
#		endif
}

void AUEscenaAnimacionI::privInlineLimpiarSonidosSinUso(){
#	ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
	SI32 iSonidoVivo;
	for(iSonidoVivo=0; iSonidoVivo<_sonidosEnEscena.conteo; iSonidoVivo++){
		AUEscenaSnd* sonido = (AUEscenaSnd*)_sonidosEnEscena.elemento[iSonidoVivo];
		if(!sonido->reproduciendoSonido()){
			//PRINTF_INFO("Limpiando sonido que ya no esta en reproduccion indice(%d)\n", iSonidoVivo);
			privInlineQuitarSonidoEscenaPorIndice(sonido, iSonidoVivo);
			iSonidoVivo--;
		}
	}
#	endif
}

void AUEscenaAnimacionI::privInlineProcesarEventosSonidos(UI32 indiceFrame){
	NBASSERT(indiceFrame>=0 && indiceFrame<_plantillaAnimacion->frames)
	if(_plantillaAnimacion->framesConEventoSonido.elemento[indiceFrame]==true && (_mascaraProps & AUESCENAANIMACIONI_BIT_REP_SONIDOS)){
		privInlineLimpiarSonidosSinUso();
		//Ejecutar evento
#		ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
		UI32 iEvento, iEventoConteo = _plantillaAnimacion->eventosDeSonido.conteo;
		for(iEvento=0; iEvento<iEventoConteo; iEvento++){
			if(_plantillaAnimacion->eventosDeSonido.elemento[iEvento].iFrame == indiceFrame){
				STAnimacionEventoSonido* eventoSonido = &(_plantillaAnimacion->eventosDeSonido.elemento[iEvento]);
				if(eventoSonido->detener){
					//Detener todos los sonidos del mismo buffer
					SI32 iSonidoVivo;
					for(iSonidoVivo=0; iSonidoVivo<_sonidosEnEscena.conteo; iSonidoVivo++){
						AUEscenaSnd* sonido = (AUEscenaSnd*)_sonidosEnEscena.elemento[iSonidoVivo];
						const NBSndOrigenDatos fuenteDatosSonido = sonido->fuenteDatosSonido();
						if((fuenteDatosSonido.tipo==ENSndFuente_buffer && fuenteDatosSonido.indice==eventoSonido->bufferSonido) || (fuenteDatosSonido.tipo==ENSndFuente_stream && fuenteDatosSonido.puntero==eventoSonido->streamSonido)){
							sonido->detenerSonido();
							privInlineQuitarSonidoEscenaPorIndice(sonido, iSonidoVivo);
							iSonidoVivo--;
						}
					}
				} else {
					bool reproducir = true;
					if(eventoSonido->noMultiple){
						SI32 iSonidoVivo;
						for(iSonidoVivo=0; iSonidoVivo<_sonidosEnEscena.conteo; iSonidoVivo++){
							AUEscenaSnd* sonido = (AUEscenaSnd*)_sonidosEnEscena.elemento[iSonidoVivo];
							const NBSndOrigenDatos fuenteDatosSonido = sonido->fuenteDatosSonido();
							if((fuenteDatosSonido.tipo==ENSndFuente_buffer && fuenteDatosSonido.indice==eventoSonido->bufferSonido) || (fuenteDatosSonido.tipo==ENSndFuente_stream && fuenteDatosSonido.puntero==eventoSonido->streamSonido)){
								reproducir = false;
							}
						}
					}
					if(reproducir){
						//PRINTF_INFO("RECORDATORIO, sonido en animaciones esta deshabilitado\n");
						AUEscenaSnd* sonido = NULL;
						if(eventoSonido->bufferSonido!=0)			sonido =  new(this) AUEscenaSonido(ENAudioGrupo_Efectos, this, eventoSonido->bufferSonido);
						else if(eventoSonido->streamSonido != NULL)	sonido = new(this) AUEscenaSonidoStream(ENAudioGrupo_Efectos, this, eventoSonido->streamSonido, 0.5f);
						if(sonido != NULL){
							if(eventoSonido->animadorSonido != NULL){
								//PRINTF_INFO("RECORDATORIO, animadorSonido esta deshabilitado\n");
								PRINTF_INFO("Animador de sonido agregado\n");
								AUAnimadorSonidoVolumen* animadorSonido = new(this) AUAnimadorSonidoVolumen(eventoSonido->animadorSonido);
								animadorSonido->establecerSonidoAnimar(sonido);
								_animadoresSonidosEscena.agregarElemento(animadorSonido);
								animadorSonido->liberar(NB_RETENEDOR_THIS);
							}
							sonido->establecerRepetir(eventoSonido->repetir);
							sonido->reproducirSonido();
							_sonidosEnEscena.agregarElemento(sonido);
							sonido->liberar(NB_RETENEDOR_THIS);
						}
					}
				}
			}
		}
#		endif
	}
	_indiceFrameUltEventosSonidosEjecutados = indiceFrame;
}

//Escuchador

void AUEscenaAnimacionI::establecerEscuchadorAnimacion(IEscenaAnimacionEscuchador* escuchador){
	_escuchador = escuchador;
}

IEscenaAnimacionEscuchador* AUEscenaAnimacionI::escuchadorAnimacion(){
	return _escuchador;
}

//

NBCajaAABB AUEscenaAnimacionI::cajaAABBLocalEncierraAnimacion(){
	NBCajaAABB cajaAnimacion; NBCAJAAABB_INICIALIZAR(cajaAnimacion);
	//envolver todos los frames de la animacion
	UI32 iFrame, iFrameConteo = _plantillaAnimacion->frames;
	for(iFrame=0; iFrame<iFrameConteo; iFrame++){
		privInlineActualizarEstadoSubanimacionesDeFrame(iFrame);
		privInlineEstablecerPropiedadesDeFrame(iFrame);
		NBCajaAABB cajaLocal = this->cajaAABBLocal();
		NBCAJAAABB_ENVOLVER_CAJA(cajaAnimacion, cajaLocal);
		//Evaluar cajas que encierran a las subaniamciones en sus transformaciones actuales
		int iElem;
		for(iElem=0; iElem<this->_objetosEscena.conteo; iElem++){
			if(this->_objetosEscena.elemento[iElem]->esClase(AUEscenaAnimacionI::idTipoClase)){
				AUEscenaAnimacionI* subanimacion = ((AUEscenaAnimacionI*)this->_objetosEscena.elemento[iElem]);
				NBMatriz matrizLocalSubAnimacion; NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_ESCENA_OBJETO(matrizLocalSubAnimacion, subanimacion->_propiedades.transformaciones)
				NBCajaAABB cajaLocalSubAnimacion = subanimacion->cajaAABBLocalEncierraAnimacion();
				NBMATRIZ_MULTIPLICAR_CAJAAABB(cajaLocalSubAnimacion, matrizLocalSubAnimacion, cajaLocalSubAnimacion);
				NBCAJAAABB_ENVOLVER_CAJA(cajaAnimacion, cajaLocalSubAnimacion);
			}
		}
	}
	//retornar al frame actual
	if(_indiceFrameActual>=0 && _indiceFrameActual<(SI32)_plantillaAnimacion->frames){
		privInlineActualizarEstadoSubanimacionesDeFrame(_indiceFrameActual);
		privInlineEstablecerPropiedadesDeFrame(_indiceFrameActual);
	}
	//
	return cajaAnimacion;
}

void AUEscenaAnimacionI::rebobinarAnimacion(){
	this->presentarFrame(_factorVelocidad>=0.0f?0:_plantillaAnimacion->frames-1);
	//Detener sonidos
	privInlineDetenerSonidos();
	//Rebobinar hijos
	int iElem; 
	for(iElem=0; iElem<_objetosEscena.conteo; iElem++){
		AUEscenaObjeto* hijo	= (AUEscenaObjeto*)_objetosEscena.elemento[iElem];
		if(_plantillaBiblioteca->tipoEncapsulamientoHijos==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
			hijo				= (AUEscenaObjeto*)((AUEscenaContenedor*)hijo)->hijos()->elemento[0];
		}
		if(hijo->esClase(AUEscenaAnimacionI::idTipoClase)){
			((AUEscenaAnimacionI*)hijo)->rebobinarAnimacion();
			if(hijo->visible() && this->animando()){
				((AUEscenaAnimacionI*)hijo)->reanudarAnimacion();
			} else {
				((AUEscenaAnimacionI*)hijo)->detenerAnimacion();
			}
		}
	}
}

void AUEscenaAnimacionI::detenerAnimacion(){
	NBAnimador::detenerAnimacion();
	//Detener sonidos
	privInlineDetenerSonidos();
	//Detener hijos
	int iElem; 
	for(iElem=0; iElem<_objetosEscena.conteo; iElem++){
		AUEscenaObjeto* hijo	= (AUEscenaObjeto*)_objetosEscena.elemento[iElem];
		if(_plantillaBiblioteca->tipoEncapsulamientoHijos==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
			hijo				= (AUEscenaObjeto*)((AUEscenaContenedor*)hijo)->hijos()->elemento[0];
		}
		if(hijo->esClase(AUEscenaAnimacionI::idTipoClase)){
			((AUEscenaAnimacionI*)hijo)->detenerAnimacion();
		}
	}
}

void AUEscenaAnimacionI::reanudarSubAnimaciones(){
	//Reanudar hijos visibles
	int iElem; 
	for(iElem=0; iElem<_objetosEscena.conteo; iElem++){
		AUEscenaObjeto* hijo	= (AUEscenaObjeto*)_objetosEscena.elemento[iElem];
		if(_plantillaBiblioteca->tipoEncapsulamientoHijos==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
			hijo				= (AUEscenaObjeto*)((AUEscenaContenedor*)hijo)->hijos()->elemento[0];
		}
		if(hijo->esClase(AUEscenaAnimacionI::idTipoClase) && hijo->visible()){
			((AUEscenaAnimacionI*)hijo)->reanudarAnimacion();
		}
	}
}

void AUEscenaAnimacionI::reanudarAnimacion(){
	NBAnimador::reanudarAnimacion();
	privInlineActualizarEstadoSubanimacionesDeFrame(_indiceFrameActual);
	privInlineEstablecerPropiedadesDeFrame(_indiceFrameActual);
	if(_indiceFrameActual!=_indiceFrameUltEventosSonidosEjecutados){
		privInlineProcesarEventosSonidos(_indiceFrameActual);
	}
	//Reanudar hijos visibles
	int iElem; 
	for(iElem=0; iElem<_objetosEscena.conteo; iElem++){
		AUEscenaObjeto* hijo	= (AUEscenaObjeto*)_objetosEscena.elemento[iElem];
		if(_plantillaBiblioteca->tipoEncapsulamientoHijos==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
			hijo				= (AUEscenaObjeto*)((AUEscenaContenedor*)hijo)->hijos()->elemento[0];
		}
		if(hijo->esClase(AUEscenaAnimacionI::idTipoClase) && hijo->visible()){
			((AUEscenaAnimacionI*)hijo)->reanudarAnimacion();
		}
	}
}

#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
void AUEscenaAnimacionI::privInlineQuitarSonidoEscenaPorIndice(AUEscenaSnd* sonidoEscena, UI32 indiceSonidoEscena){
	_sonidosEnEscena.quitarElementoEnIndice(indiceSonidoEscena);
	SI32 iAnimadorSonido;
	for(iAnimadorSonido=0; iAnimadorSonido<_animadoresSonidosEscena.conteo; iAnimadorSonido++){
		AUAnimadorSonidoVolumen* animaSonido = (AUAnimadorSonidoVolumen*)_animadoresSonidosEscena.elemento[iAnimadorSonido];
		if(animaSonido->sonidoAnimado()==sonidoEscena){
			PRINTF_INFO("Animador de sonido quitado\n");
			_animadoresSonidosEscena.quitarElementoEnIndice(iAnimadorSonido--);
		}
	}
}
#endif

void AUEscenaAnimacionI::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaAnimacionI::agregadoEnEscena")
	NBGestorAnimadores::agregarAnimador(this, this);
	AUEscenaContenedor::agregadoEnEscena();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaAnimacionI::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaAnimacionI::quitandoDeEscena")
	AUEscenaContenedor::quitandoDeEscena();
	NBGestorAnimadores::quitarAnimador(this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaAnimacionI::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaAnimacionI::tickAnimacion")
	NBASSERT(this->idEscena!=-1) //2025-07-04: temporarily commented
	NBASSERT(this->_animacionEnEjecucion)
	if(!this->_dormido){
		if(_plantillaAnimacion->frames != 0 && _plantillaBiblioteca->fps != 0.0f){
			const float segsAcumular	= segsTranscurridos * _factorVelocidad;
			//Es permitido repetir || (No es la frame final en sentido directo) || (No es la frame final en sentido inverso)
			if((_mascaraProps & AUESCENAANIMACIONI_BIT_REPETIR) || (segsAcumular >= 0.0f && _indiceFrameActual < (_plantillaAnimacion->frames - 1)) || (segsAcumular < 0.0f && _indiceFrameActual > 0)){
				_segundosAcumuladosAnim		+= segsAcumular;
				if(_segundosAcumuladosAnim >= 0.0f){
					//Procesar frames transcurridas (HACIA ADELANTE)
					bool avanceEnReproduccion	= (_segundosAcumuladosParaRepetir == 0.0f);
					float segsPorFrame			= 1.0f / _plantillaBiblioteca->fps;
					SI32 indiceFrameInicioTick	= _indiceFrameActual;
					while(_segundosAcumuladosAnim > segsPorFrame){
						_segundosAcumuladosAnim	-= segsPorFrame;
						if(_segundosAcumuladosParaRepetir!=0.0f){
							//Espera para repetir
							_segundosAcumuladosParaRepetir -= segsPorFrame;
							if(_segundosAcumuladosParaRepetir < 0.0f){
								_segundosAcumuladosAnim -= _segundosAcumuladosParaRepetir; //se devuelve el restante al acumulado
								_segundosAcumuladosParaRepetir = 0.0f;
							}
						} else {
							//Ejecutar animacion
							SI32 indiceFrameAnterior	= _indiceFrameActual;
							_indiceFrameActual			= indiceFrameAnterior + 1;
							avanceEnReproduccion		= true;
							if(_indiceFrameActual >= (SI32)_plantillaAnimacion->frames){
								NBASSERT(_indiceFrameActual == (SI32)_plantillaAnimacion->frames)
								if(_segundosEsperarParaRepetir != 0.0f && (_mascaraProps & AUESCENAANIMACIONI_BIT_REPETIR)){
									_indiceFrameActual	= 0;
									_segundosAcumuladosParaRepetir = _segundosEsperarParaRepetir;
								} else {
									_indiceFrameActual	= (_mascaraProps & AUESCENAANIMACIONI_BIT_REPETIR) ? 0 : _plantillaAnimacion->frames - 1;
								}
								if(!(_mascaraProps & AUESCENAANIMACIONI_BIT_REPETIR)) _segundosAcumuladosAnim = 0.0f;
							}
							//
							if(_indiceFrameActual != indiceFrameAnterior){
								privInlineActualizarEstadoSubanimacionesDeFrame(_indiceFrameActual);
								privInlineProcesarEventosSonidos(_indiceFrameActual);
								if(_indiceFrameActual==(_plantillaAnimacion->frames-1)){
									if(_escuchador != NULL) _escuchador->animacionFinalizada(this);
								}
							}
						}
					}
					//Actualizar presentacion al frame final
					if(avanceEnReproduccion){ //para evitar un "temblor" en las animaciones suavizadas
						if((_mascaraProps & AUESCENAANIMACIONI_BIT_SUAVIZAR) && _plantillaAnimacion->frames!=1 && (_indiceFrameActual+1<(SI32)_plantillaAnimacion->frames || (_mascaraProps & AUESCENAANIMACIONI_BIT_REPETIR))){
							privInlineEstablecerPropiedadesEntreFrames(_indiceFrameActual, _indiceFrameActual+1<(SI32)_plantillaAnimacion->frames?_indiceFrameActual+1:0, _segundosAcumuladosAnim/segsPorFrame);
						} else if(indiceFrameInicioTick!=_indiceFrameActual){
							privInlineEstablecerPropiedadesDeFrame(_indiceFrameActual);
						}
					}
				} else {
					//Procesar frames transcurridas (HACIA ATRAS, no se reproducen sonidos)
					bool avanceEnReproduccion	= (_segundosAcumuladosParaRepetir==0.0f); //para evitar un "temblor" en las animaciones suavizadas
					float segsPorFrame			= -1.0f / _plantillaBiblioteca->fps;
					SI32 indiceFrameInicioTick	= _indiceFrameActual;
					while(_segundosAcumuladosAnim < segsPorFrame){
						_segundosAcumuladosAnim	-= segsPorFrame;
						if(_segundosAcumuladosParaRepetir!=0.0f){
							//Espera para repetir
							_segundosAcumuladosParaRepetir -= segsPorFrame;
							if(_segundosAcumuladosParaRepetir>0.0f){
								_segundosAcumuladosAnim -= _segundosAcumuladosParaRepetir; //se devuelve el restante al acumulado
								_segundosAcumuladosParaRepetir = 0.0f;
							}
						} else {
							//Ejecutar animacion
							int indiceFrameAnterior	= _indiceFrameActual;
							_indiceFrameActual		= indiceFrameAnterior - 1;
							avanceEnReproduccion	= true;
							if(_indiceFrameActual < 0){
								NBASSERT(_indiceFrameActual==-1)
								if(_segundosEsperarParaRepetir!=0.0f && (_mascaraProps & AUESCENAANIMACIONI_BIT_REPETIR)){
									_indiceFrameActual	= _plantillaAnimacion->frames-1;
									_segundosAcumuladosParaRepetir = -_segundosEsperarParaRepetir;
								} else {
									_indiceFrameActual	= ((_mascaraProps & AUESCENAANIMACIONI_BIT_REPETIR)?_plantillaAnimacion->frames-1:0);
								}
								if(!(_mascaraProps & AUESCENAANIMACIONI_BIT_REPETIR)) _segundosAcumuladosAnim = 0.0f;
							}
							//
							if(_indiceFrameActual!=indiceFrameAnterior){
								privInlineActualizarEstadoSubanimacionesDeFrame(_indiceFrameActual);
								//privInlineProcesarEventosSonidos(_indiceFrameActual); //No se deben procesar los eventos de sonido a la inversa
								if(_indiceFrameActual==0){
									if(_escuchador != NULL) _escuchador->animacionFinalizada(this);
								}
							}
						}
					}
					//Actualizar presentacion al frame final
					if(avanceEnReproduccion){ //para evitar un "temblor" en las animaciones suavizadas
						if((_mascaraProps & AUESCENAANIMACIONI_BIT_SUAVIZAR) && _plantillaAnimacion->frames!=1 && (_indiceFrameActual>0 || (_mascaraProps & AUESCENAANIMACIONI_BIT_REPETIR))){
							privInlineEstablecerPropiedadesEntreFrames(_indiceFrameActual, _indiceFrameActual!=0?_indiceFrameActual-1:_plantillaAnimacion->frames-1, _segundosAcumuladosAnim/segsPorFrame);
						} else if(indiceFrameInicioTick!=_indiceFrameActual){
							privInlineEstablecerPropiedadesDeFrame(_indiceFrameActual);
						}
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaAnimacionI, AUEscenaContenedor)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaAnimacionI, "AUEscenaAnimacionI", AUEscenaContenedor)
AUOBJMETODOS_CLONAR_NULL(AUEscenaAnimacionI)





