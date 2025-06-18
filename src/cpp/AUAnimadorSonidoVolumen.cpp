//
//  AUAnimadorSonidoVolumen.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 27/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUAnimadorSonidoVolumen.h"

#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO

AUAnimadorSonidoVolumen::AUAnimadorSonidoVolumen() : AUObjeto(), NBAnimador() {
	_segsAcumulados				= 0.0f;
	_sonidoAnimar				= NULL;
	//
	_segundoInicio				= -1.0f;
	_segundoFin					= -1.0f;
	_repeticiones				= -1;
	_puntosDeVolumen			= NULL; //AUArregloNativoMutableP<STDatosSonidoVolumen>*;
	//
	_ultimoOffsetSegundos		= 0.0f;
	_conteoRepeticiones			= 0;
	_indicePuntoVolumenActual	= 0;
	//
	this->reanudarAnimacion();
	this->establecerGrupoAnimacion(ENGestorAnimadoresGrupo_Nucleo); //Animador importante que debe ir de la mano con OpenAL
	NBGestorAnimadores::agregarAnimador(this, this);
}

AUAnimadorSonidoVolumen::AUAnimadorSonidoVolumen(AUAnimadorSonidoVolumen* otraInstancia) : NBAnimador(){
	_segsAcumulados				= 0.0f;
	_sonidoAnimar				= otraInstancia->_sonidoAnimar; if(_sonidoAnimar != NULL) _sonidoAnimar->retener(NB_RETENEDOR_THIS);
	//
	_segundoInicio				= otraInstancia->_segundoInicio;
	_segundoFin					= otraInstancia->_segundoFin;
	_repeticiones				= otraInstancia->_repeticiones;
	_puntosDeVolumen			= NULL;
	if(otraInstancia->_puntosDeVolumen != NULL) _puntosDeVolumen = new(this) AUArregloNativoMutableP<STDatosSonidoVolumen>(otraInstancia->_puntosDeVolumen);
	//
	_ultimoOffsetSegundos		= 0.0f;
	_conteoRepeticiones			= 0;
	_indicePuntoVolumenActual	= 0;
	//
	this->reanudarAnimacion();
	NBGestorAnimadores::agregarAnimador(this, this);
}

AUAnimadorSonidoVolumen::~AUAnimadorSonidoVolumen(){
	NBGestorAnimadores::quitarAnimador(this);
	if(_sonidoAnimar != NULL) _sonidoAnimar->liberar(NB_RETENEDOR_THIS);
	if(_puntosDeVolumen != NULL) _puntosDeVolumen->liberar(NB_RETENEDOR_THIS);
}

AUEscenaSnd* AUAnimadorSonidoVolumen::sonidoAnimado(){
	return _sonidoAnimar;
}

float AUAnimadorSonidoVolumen::segundoInicio(){
	return _segundoInicio;
}

float AUAnimadorSonidoVolumen::segundoFin(){
	return _segundoFin;
}

SI32 AUAnimadorSonidoVolumen::repeticiones(){
	return _repeticiones;
}

AUArregloNativoP<STDatosSonidoVolumen>* AUAnimadorSonidoVolumen::puntosDeVolumen(){
	return _puntosDeVolumen;
}

void AUAnimadorSonidoVolumen::establecerSonidoAnimar(AUEscenaSnd* sonido){
	if(sonido != NULL) sonido->retener(NB_RETENEDOR_THIS);
	if(_sonidoAnimar != NULL) _sonidoAnimar->liberar(NB_RETENEDOR_THIS);
	_sonidoAnimar = sonido;
	if(_sonidoAnimar != NULL){
		//PENDIENTE: establecer valores iniciales del sonido
	}
}

void AUAnimadorSonidoVolumen::establecerSegundoInicio(float segInicio){
	_segundoInicio	= segInicio;
	if(_sonidoAnimar != NULL){
		_sonidoAnimar->establecerOffSetSegundos(segInicio);
	}
}

void AUAnimadorSonidoVolumen::establecerSegundoFin(float segFin){
	_segundoFin		= segFin;
}

void AUAnimadorSonidoVolumen::establecerRepeticiones(SI32 repeticiones){
	_repeticiones	= repeticiones;
}

void AUAnimadorSonidoVolumen::agregarPuntoDeVolumen(float segundo, float volumen){
	if(_puntosDeVolumen == NULL){
		_puntosDeVolumen = new(this) AUArregloNativoMutableP<STDatosSonidoVolumen>();
		NB_DEFINE_NOMBRE_PUNTERO(_puntosDeVolumen, "AUAnimadorSonidoVolumen::_puntosDeVolumen");
	}
	STDatosSonidoVolumen nuevoPuntoVolumen;
	nuevoPuntoVolumen.segundo	= segundo;
	nuevoPuntoVolumen.volumen	= volumen;
	_puntosDeVolumen->agregarElemento(nuevoPuntoVolumen);
}

//

void AUAnimadorSonidoVolumen::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAnimadorSonidoVolumen::tickAnimacion")
	if(_sonidoAnimar != NULL){
		//if(_sonidoAnimar->reproduciendoSonido()){
			_segsAcumulados += segsTranscurridos;
			//
			const UI16 fuenteAL			= _sonidoAnimar->fuenteAL();
			if(fuenteAL!=0){
				float offSetSegundo		= _sonidoAnimar->offsetSegundos();
				//Animar offset de fin
				if(_segundoFin>0.0f && offSetSegundo>=_segundoFin){
					if(_segundoInicio>=0.0f){
						_sonidoAnimar->establecerOffSetSegundos(_segundoInicio);
						offSetSegundo = _segundoInicio;
					} else {
						_sonidoAnimar->rebobinarSonido();
						offSetSegundo = 0.0f;
					}
					PRINTF_INFO("Animacion alcanzo offsetFinal (%f)\n", _segundoFin);
				}
				//Animar offset de inicio
				if(_segundoInicio>=0.0f && offSetSegundo<_segundoInicio){
					PRINTF_INFO("Animacion estaba antes de offsetInicial (%f)\n", _segundoInicio);
					_sonidoAnimar->establecerOffSetSegundos(_segundoInicio);
				}
				//Animar repeticiones
				if(_repeticiones>0){
					if(offSetSegundo<_ultimoOffsetSegundos){
						_conteoRepeticiones++;
						PRINTF_INFO("Animacion repeticion (%d)\n", _conteoRepeticiones);
					}
					if(_conteoRepeticiones>=_repeticiones){
						PRINTF_INFO("Animacion alcanzo maximo de repeticiones (%d)\n", _repeticiones);
						_sonidoAnimar->detenerSonido();
						this->detenerAnimacion();
					}
				}
				//Animar volumen
				if(_puntosDeVolumen != NULL){
					if(_indicePuntoVolumenActual<(SI32)_puntosDeVolumen->conteo){
						float segPuntoVolumenAnterior	= 0.0f;
						float volumenPuntoAnterior		= 1.0f;
						if(_indicePuntoVolumenActual>0){
							segPuntoVolumenAnterior		= _puntosDeVolumen->elemento[_indicePuntoVolumenActual-1].segundo;
							volumenPuntoAnterior		= _puntosDeVolumen->elemento[_indicePuntoVolumenActual-1].volumen;
						}
						float segActual					= _segsAcumulados;
						float segPuntoVolumenSiguiente	= _puntosDeVolumen->elemento[_indicePuntoVolumenActual].segundo; if(segActual>segPuntoVolumenSiguiente) segActual = segPuntoVolumenSiguiente;
						float volumenPuntoSiguiente		= _puntosDeVolumen->elemento[_indicePuntoVolumenActual].volumen;
						//
						float volumenActual				= volumenPuntoSiguiente;
						if(segPuntoVolumenAnterior!=segPuntoVolumenSiguiente){
							float relacionAvanceActual	= (segActual - segPuntoVolumenAnterior) / (segPuntoVolumenSiguiente - segPuntoVolumenAnterior);
							volumenActual				= volumenPuntoAnterior + ((volumenPuntoSiguiente - volumenPuntoAnterior) * relacionAvanceActual);
						}
						_sonidoAnimar->establecerVolumen(volumenActual);
						PRINTF_INFO("Volumen actual: %f, indiceEnvelope (%d), segs(%f) segs(%f) PuntosVolumen(%d)\n", volumenActual, _indicePuntoVolumenActual, segActual, _segsAcumulados, (SI32)_puntosDeVolumen->conteo);
						if(segActual>=segPuntoVolumenSiguiente){
							_indicePuntoVolumenActual++;
						}
					}
				}
				//
				_ultimoOffsetSegundos = offSetSegundo;
			}
		//}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//Guardar y cargar en binario

bool AUAnimadorSonidoVolumen::agregarBitsInternosEn(AUAnimadorSonidoVolumen* animadorSonido, AUArchivo* guardarEn){
	guardarEn->escribir(&animadorSonido->_segundoInicio, sizeof(animadorSonido->_segundoInicio), 1, guardarEn);
	guardarEn->escribir(&animadorSonido->_segundoFin, sizeof(animadorSonido->_segundoFin), 1, guardarEn);
	guardarEn->escribir(&animadorSonido->_repeticiones, sizeof(animadorSonido->_repeticiones), 1, guardarEn);
	SI32 conteoPuntosVolumen = 0; if(animadorSonido->_puntosDeVolumen != NULL) conteoPuntosVolumen = animadorSonido->_puntosDeVolumen->conteo;
	guardarEn->escribir(&conteoPuntosVolumen, sizeof(conteoPuntosVolumen), 1, guardarEn);
	if(conteoPuntosVolumen>0){
		guardarEn->escribir(animadorSonido->_puntosDeVolumen->elemento, sizeof(STDatosSonidoVolumen), conteoPuntosVolumen, guardarEn);
	}
	return true;
}

bool AUAnimadorSonidoVolumen::interpretarBitsInternos(AUAnimadorSonidoVolumen* cargarEn, AUArchivo* cargarDe){
	cargarDe->leer(&cargarEn->_segundoInicio, sizeof(cargarEn->_segundoInicio), 1, cargarDe);
	cargarDe->leer(&cargarEn->_segundoFin, sizeof(cargarEn->_segundoFin), 1, cargarDe);
	cargarDe->leer(&cargarEn->_repeticiones, sizeof(cargarEn->_repeticiones), 1, cargarDe);
	UI32 conteoPuntosVolumen = 0; cargarDe->leer(&conteoPuntosVolumen, sizeof(conteoPuntosVolumen), 1, cargarDe);
	//PRINTF_INFO("Conteo de puntos de volumen: %d\n", conteoPuntosVolumen);
	if(conteoPuntosVolumen>0){
		//Asegurar arreglo y tamano
		if(cargarEn->_puntosDeVolumen == NULL){
			cargarEn->_puntosDeVolumen = new(cargarEn) AUArregloNativoMutableP<STDatosSonidoVolumen>();
			NB_DEFINE_NOMBRE_PUNTERO(cargarEn->_puntosDeVolumen, "AUAnimadorSonidoVolumen::cargarEn->_puntosDeVolumen");
		}
		cargarEn->_puntosDeVolumen->vaciar();
		//Crear espacios
		STDatosSonidoVolumen plantillaPuntoVolumen;
		plantillaPuntoVolumen.segundo = 0.0f;
		plantillaPuntoVolumen.volumen = 0.0f;
		while(cargarEn->_puntosDeVolumen->conteo < conteoPuntosVolumen) cargarEn->_puntosDeVolumen->agregarElemento(plantillaPuntoVolumen);
		//
		cargarDe->leer(cargarEn->_puntosDeVolumen->elemento, sizeof(STDatosSonidoVolumen), conteoPuntosVolumen, cargarDe);
	}
	return true;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUAnimadorSonidoVolumen)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUAnimadorSonidoVolumen, "AUAnimadorSonidoVolumen")
AUOBJMETODOS_CLONAR_THIS(AUAnimadorSonidoVolumen)



#endif //#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO


