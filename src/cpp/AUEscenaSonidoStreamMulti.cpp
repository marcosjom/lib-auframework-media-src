//
//  AUEscenaSonidoStreamMulti.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 01/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaSonidoStreamMulti.h"

#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO

AUEscenaSonidoStreamMulti::AUEscenaSonidoStreamMulti(ENAudioGrupo grupoAudio, float segundosMinimoEnFila) : AUEscenaSnd(), _bufferes(this), _porcionesStream(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::AUEscenaSonidoStreamMulti")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSonidoStreamMulti");
	privInlineInicializar(grupoAudio, NULL, segundosMinimoEnFila);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSonidoStreamMulti::AUEscenaSonidoStreamMulti(ENAudioGrupo grupoAudio, AUEscenaObjeto* objetoEscena, float segundosMinimoEnFila) : AUEscenaSnd(), _bufferes(this), _porcionesStream(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::AUEscenaSonidoStreamMulti")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSonidoStreamMulti");
	privInlineInicializar(grupoAudio, objetoEscena, segundosMinimoEnFila);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSonidoStreamMulti::AUEscenaSonidoStreamMulti(AUEscenaSonidoStreamMulti* otra) : AUEscenaSnd(otra), _bufferes(this), _porcionesStream(&otra->_porcionesStream) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::AUEscenaSonidoStreamMulti")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSonidoStreamMulti");
	privInlineInicializar((ENAudioGrupo)otra->_grupoAudio, NULL, otra->_segundosMinimoEnFila);
	//Retener los stream
	UI32 iPorcion, iPorcionConteo = _porcionesStream.conteo;
	for(iPorcion=0; iPorcion<iPorcionConteo; iPorcion++){
		NBASSERT(_porcionesStream.elemento[iPorcion].streamSonido != NULL)
		_porcionesStream.elemento[iPorcion].streamSonido->retener(NB_RETENEDOR_THIS);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStreamMulti::privInlineInicializar(ENAudioGrupo grupoAudio, AUEscenaObjeto* objetoEscena, float segundosMinimoEnFila){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::privInlineInicializar")
	_grupoAudio						= grupoAudio;
	_fuenteOpenAL					= 0;
	_objetoEscena					= NULL; if(objetoEscena != NULL) this->establecerObjetoEscena(objetoEscena);
	_reproduciendoSonido			= false;
	_segundosMinimoEnFila			= segundosMinimoEnFila;
	_cantBuffersPorFila				= 0;
	_repetir						= false;
	_volumen						= 1.0f;
	//
	_iPorcionActual					= 0;
	_iMuestraActual					= 0;
	_vecesRepetidaMuestraActual		= 0;
	//
	_bufferTemporalDatosSonido		= NULL;
	_bytesBufferTemporalDatosSonido	= 0;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSonidoStreamMulti::~AUEscenaSonidoStreamMulti(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::~AUEscenaSonidoStreamMulti")
	//Advertencia, la retencion del "_objetoEscena" esta creando una retencion ciclica en las Animaciones.
	//La animacion retiene al sonido, y el sonido retiene a la animacion
	//if(_objetoEscena != NULL) _objetoEscena->liberar(NB_RETENEDOR_THIS);
	//Liberar los bufferes
	UI16 i;
	for(i=0; i<_bufferes.conteo; i++) NBGestorSonidos::bufferLiberar(_bufferes.elemento[i]); _bufferes.vaciar();
	//Liberar los stream
	UI32 iPorcion, iPorcionConteo = _porcionesStream.conteo;
	for(iPorcion=0; iPorcion<iPorcionConteo; iPorcion++){
		NBASSERT(_porcionesStream.elemento[iPorcion].streamSonido != NULL)
		_porcionesStream.elemento[iPorcion].streamSonido->liberar(NB_RETENEDOR_THIS);
	}
	_porcionesStream.vaciar();
	//
	if(_bufferTemporalDatosSonido != NULL) NBGestorMemoria::liberarMemoria(_bufferTemporalDatosSonido); _bufferTemporalDatosSonido = NULL;
	if(_fuenteOpenAL != 0) NBGestorSonidos::fuenteLiberar(_fuenteOpenAL); /*this->sndLiberarFuente(_fuenteOpenAL);*/ NBASSERT(_fuenteOpenAL == 0)
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

SI32 AUEscenaSonidoStreamMulti::indicePorcionActual(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::indicePorcionActual")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _iPorcionActual;
}

bool AUEscenaSonidoStreamMulti::agregarPorcionStream(AUSonidoStream* streamSonido, const SI16 vecesRepetir){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::agregarPorcionStream")
	NBASSERT(streamSonido != NULL)
	bool formatoCompatible = true;
	if(_porcionesStream.conteo!=0){
		STSonidoDescriptor descripcion = streamSonido->propiedades();
		STSonidoDescriptor descPrimerStream = _porcionesStream.elemento[0].streamSonido->propiedades();
		formatoCompatible = (descripcion.canales==descPrimerStream.canales && descripcion.bitsPorMuestra==descPrimerStream.bitsPorMuestra && descripcion.alineacionBloques==descPrimerStream.alineacionBloques && descripcion.muestrasPorSegundo==descPrimerStream.muestrasPorSegundo);
		if(!formatoCompatible) PRINTF_WARNING("STREAM_MULTI: se ha intentado agregar una pocion con formato no compatible con las anteriores\n");
	}
	NBASSERT(formatoCompatible)
	if(formatoCompatible){
		STStreamMultiPorcion datosPorcion;
		datosPorcion.streamSonido	= streamSonido; streamSonido->retener(NB_RETENEDOR_THIS);
		datosPorcion.vecesRepetir	= vecesRepetir;
		_porcionesStream.agregarElemento(datosPorcion);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return formatoCompatible;
}

bool AUEscenaSonidoStreamMulti::moverHaciaPorcion(const UI16 iPorcion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::moverHaciaPorcion")
	NBASSERT(iPorcion < _porcionesStream.conteo)
	bool r = false;
	if(iPorcion < _porcionesStream.conteo){
		_iPorcionActual = iPorcion;
		_iMuestraActual = 0;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

AUEscenaObjeto* AUEscenaSonidoStreamMulti::objetoEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::objetoEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _objetoEscena;
}

/*AUSonidoStream* AUEscenaSonidoStreamMulti::streamSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::streamSonido")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _streamSonido;
}*/

UI16 AUEscenaSonidoStreamMulti::fuenteAL(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::fuenteAL")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _fuenteOpenAL;
}

NBSndOrigenDatos AUEscenaSonidoStreamMulti::fuenteDatosSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::fuenteDatosSonido")
	NBASSERT(false) //Son multiples fuentes stream
	NBSndOrigenDatos sndFuente;
	sndFuente.tipo = ENSndFuente_ninguno;
	sndFuente.indice = 0;
	sndFuente.puntero = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return sndFuente;
}

bool AUEscenaSonidoStreamMulti::repetir(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::repetir")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _repetir;
}

float AUEscenaSonidoStreamMulti::volumen(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::volumen")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _volumen;
}

float AUEscenaSonidoStreamMulti::offsetSegundos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::offsetSegundos")
	NBASSERT(false) //La duracion de la pista no esta establecida
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return 0.0f;
}

void AUEscenaSonidoStreamMulti::establecerObjetoEscena(AUEscenaObjeto* objetoEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::establecerObjetoEscena")
	//Advertencia, la retencion del "_objetoEscena" esta creando una retencion ciclica en las Animaciones.
	//La animacion retiene al sonido, y el sonido retiene a la animacion
	//if(objetoEscena != NULL) objetoEscena->retener(NB_RETENEDOR_THIS);
	//if(_objetoEscena != NULL) _objetoEscena->liberar(NB_RETENEDOR_THIS);
	_objetoEscena = objetoEscena;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStreamMulti::establecerRepetir(bool repetir){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::establecerRepetir")
	//NOTA IMPORTANTE: las fuentesAL con REPEAT activado nunca liberan sus bufferes.
	//Es mejor no establecer el repeat, y repetir manualmente la lectura de archivo.
	_repetir = repetir;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStreamMulti::establecerVolumen(float volumen){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::establecerVolumen")
	NBASSERT(volumen>=0.0f && volumen<=1.0f);
	_volumen = volumen;
	if(_fuenteOpenAL!=0){
		NBGestorSonidos::fuenteEstablecerVolumen(_fuenteOpenAL, _volumen);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStreamMulti::establecerOffSetSegundos(float segundos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::establecerOffSetSegundos")
	NBASSERT(false) //No se puede determinar
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

void AUEscenaSonidoStreamMulti::privInlineAsegurarFuenteAL(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::privInlineAsegurarFuenteAL")
	if(_fuenteOpenAL==0){
		STSonidoDescriptor propsSnd = _porcionesStream.elemento[0].streamSonido->propiedades();
		const UI32 bytesPorbuffer	= NBGestorSonidos::tamanoBytesParaBufferStream(propsSnd);
		float segsPorBuffer			= NBGestorSonidos::tamanoSegundosParaBufferStream(propsSnd);
		float cantBuffers			= _segundosMinimoEnFila / segsPorBuffer; if(cantBuffers!=(float)((SI32)cantBuffers)) cantBuffers++;
		_fuenteOpenAL				= NBGestorSonidos::fuenteAsignarStream(this, false, (ENAudioGrupo)_grupoAudio, (UI16)cantBuffers);
		if(_fuenteOpenAL!=0){
			_cantBuffersPorFila = (UI16)cantBuffers;
			NBGestorSonidos::fuenteRetener(_fuenteOpenAL);
			//NOTA IMPORTANTE: las fuentesAL con REPEAT activado no liberan sus bufferes.
			//Es mejor no establecer el repeat, y repetir manualmente.
			//NBGestorSonidos::fuenteEstablecerRepetir(_fuenteOpenAL, _repetir);
			NBGestorSonidos::fuenteEstablecerVolumen(_fuenteOpenAL, _volumen);
			//Asegurar bufferes
			UI16 i;
			for(i=_bufferes.conteo; i<_cantBuffersPorFila; i++){
				const UI16 buffer = NBGestorSonidos::bufferDesdeDatos(&propsSnd, NULL, bytesPorbuffer);
				NBGestorSonidos::bufferRetener(buffer);
				_bufferes.agregarElemento(buffer);
			}
			//Cargar los buffers iniciales
			this->privLlenarCola();
		} else {
			PRINTF_ERROR("STREAM fuenteSonido no recibida (disponible?)\n");
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//para ser notificado por el NBGestorSonidos que la fuenteSonidoAL ya no me pertence
void AUEscenaSonidoStreamMulti::sndLiberarFuente(const UI16 idFuenteSonidoAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::sndLiberarFuente")
	NBASSERT(idFuenteSonidoAL == _fuenteOpenAL)
	if(_fuenteOpenAL != 0){
		//PRINTF_INFO("STREAM, fuenteSonido (%d) liberada\n", _fuenteOpenAL->idFuenteOpenAL());
		//NBGestorSonidos::fuenteLiberar(fuenteOpenAL); //No invocar (sería recursivo)
		_fuenteOpenAL = 0;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//para ser notificado por el NBGestorSonidos que debe continuarse con el llenado de stream
void AUEscenaSonidoStreamMulti::sndStreamBufferQuitadoDeCola(const UI16 cantBufferQuitados){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::sndStreamBufferQuitadoDeCola")
	if(_fuenteOpenAL!=0){
		//Cargar siguientes buffers
		this->privLlenarCola();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

bool AUEscenaSonidoStreamMulti::reproduciendoSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::reproduciendoSonido")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _reproduciendoSonido;
}

void AUEscenaSonidoStreamMulti::reproducirSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::reproducirSonido")
	_reproduciendoSonido = true;
	privInlineAsegurarFuenteAL();
	if(_fuenteOpenAL!=0){
		NBGestorSonidos::fuenteReproducir(_fuenteOpenAL);
	} else {
		PRINTF_WARNING("STREAM, no hay fuente disponible\n");
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStreamMulti::pausarSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::pausarSonido")
	_reproduciendoSonido = false;
	privInlineAsegurarFuenteAL();
	if(_fuenteOpenAL!=0){
		NBGestorSonidos::fuentePausar(_fuenteOpenAL);
		//PRINTF_INFO("Pausado sondido(%u) con bufferAL(%u)\n", (UI32)_idFuenteOpenAL, (UI32)_bufferAL->idBufferOpenAL());
	} else {
		PRINTF_WARNING("no se pudo pausar sonido porque no hay fuente disponible\n");
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStreamMulti::detenerSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::detenerSonido")
	_reproduciendoSonido = false;
	if(_fuenteOpenAL!=0){
		NBGestorSonidos::fuenteDetener(_fuenteOpenAL);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStreamMulti::vaciarCola(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::vaciarCola")
	if(_fuenteOpenAL!=0) NBGestorSonidos::fuenteVaciarCola(_fuenteOpenAL);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStreamMulti::rebobinarSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::rebobinarSonido")
	_iPorcionActual	= 0;
	_iMuestraActual = 0;
	/*privInlineAsegurarFuenteAL();
	if(_fuenteOpenAL != NULL){
		_indiceMuestraActual = 0;
		//PRINTF_INFO("Rebobinado sondido(%u) con bufferAL(%u)\n", (UI32)_idFuenteOpenAL, (UI32)_bufferAL->idBufferOpenAL());
	} else {
		PRINTF_WARNING("no se pudo rebobinar sonido porque no hay fuente disponible\n");
	}*/
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStreamMulti::privLlenarCola(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::privLlenarCola")
	if(_fuenteOpenAL!=0 && _porcionesStream.conteo!=0){
		UI32 conteoBufferesAgregados	= 0;
		bool errorLlenandoCola			= false;
		STSonidoDescriptor propiedades	= _porcionesStream.elemento[0].streamSonido->propiedades(); //Se asume que todas las porciones deben tener las mismas propiedades (y se valida de esa forma)
		while(NBGestorSonidos::fuenteTotalBuffers(_fuenteOpenAL) < _cantBuffersPorFila && !errorLlenandoCola){
			UI32 bytesBufferAL			= NBGestorSonidos::tamanoBytesParaBufferStream(propiedades);
			if(_bytesBufferTemporalDatosSonido<bytesBufferAL){
				if(_bufferTemporalDatosSonido) NBGestorMemoria::liberarMemoria(_bufferTemporalDatosSonido);
				_bufferTemporalDatosSonido		= (UI8*)NBGestorMemoria::reservarMemoria(bytesBufferAL, ENMemoriaTipo_Temporal); NB_DEFINE_NOMBRE_PUNTERO(_bufferTemporalDatosSonido, "_bufferTemporalDatosSonido");
				_bytesBufferTemporalDatosSonido	= bytesBufferAL;
				//PRINTF_INFO("STREAM, creando buffer temporal\n");
			} else {
				//PRINTF_INFO("STREAM, optimizacion reutilizando buffer temporal\n");
			}
			UI32 bytesUsadosBufferAL	= 0;
			while(bytesUsadosBufferAL < bytesBufferAL && !errorLlenandoCola){
				NBASSERT(_iPorcionActual>=0 && _iPorcionActual<(SI32)_porcionesStream.conteo)
				STStreamMultiPorcion datosPorcionActual = _porcionesStream.elemento[_iPorcionActual];
				UI32 bytesCopiados = datosPorcionActual.streamSonido->cargarDatosDeFlujoEn(_iMuestraActual, &_bufferTemporalDatosSonido[bytesUsadosBufferAL], bytesBufferAL-bytesUsadosBufferAL);
				if(bytesCopiados==0){
					_iMuestraActual = 0;
					if(datosPorcionActual.vecesRepetir<0){
						//Repetir infinito
						_vecesRepetidaMuestraActual++;
						//PRINTF_INFO("Repitiendo porcion %d de %d (infinito)\n", _iPorcionActual+1, _porcionesStream.conteo);
					} else if(_vecesRepetidaMuestraActual<datosPorcionActual.vecesRepetir){
						//Repetir porcion
						_vecesRepetidaMuestraActual++;
						//PRINTF_INFO("Repitiendo porcion %d de %d (%d de %d veces)\n", _iPorcionActual+1, _porcionesStream.conteo, _vecesRepetidaMuestraActual, datosPorcionActual.vecesRepetir);
					} else {
						//Siguiente porcion
						_vecesRepetidaMuestraActual = 0;
						_iPorcionActual++;
						if(_iPorcionActual>=(SI32)_porcionesStream.conteo){ //Era la ultima porcion
							if(_repetir){
								_iPorcionActual = 0;
								//PRINTF_INFO("Repitiendo pista completa (%d porciones)\n", _porcionesStream.conteo);
							} else {
								//PRINTF_INFO("STREAM, se llego al final del flujo stream (cero bytes copiados)\n");
								errorLlenandoCola		= true;
								_reproduciendoSonido	= false;
								//PRINTF_INFO("Fin de reproduccion de pista completa (%d porciones)\n", _porcionesStream.conteo);
							}
						} else {
							//PRINTF_INFO("Siguiente porcion (la %d de %d)\n", _iPorcionActual+1, _porcionesStream.conteo);
						}
					}
					//PRINTF_INFO("STREAM, se ha alcanzado el final del flujo\n");
				} else {
					bytesUsadosBufferAL += bytesCopiados;
					_iMuestraActual += (float)bytesCopiados / (float)propiedades.alineacionBloques;
				}
			}
			if(bytesUsadosBufferAL!=0 && !errorLlenandoCola){
				//Buscar el primer buffer disponible
				SI32 iBufferDisponible = -1; UI16 i;
				for(i=0; i<_bufferes.conteo; i++){
					if(!NBGestorSonidos::fuenteTieneBufferEnCola(_fuenteOpenAL, _bufferes.elemento[i])){
						iBufferDisponible = i; break;
					}
				} NBASSERT(iBufferDisponible!=-1) //Deberia haber algun buffer libre
				if(iBufferDisponible==-1){
					errorLlenandoCola	= true; PRINTF_ERROR("No se encontro buffer libre (deberia).\n"); NBASSERT(false)
				} else {
					if(!NBGestorSonidos::bufferEstablecerDatos(_bufferes.elemento[i], &propiedades, _bufferTemporalDatosSonido, bytesUsadosBufferAL)){
						errorLlenandoCola	= true; PRINTF_ERROR("No se pudo establecer datos de bufer.\n"); NBASSERT(false)
					} else {
						if(!NBGestorSonidos::cargarBufferStreamALEnFuente(_fuenteOpenAL, _bufferes.elemento[i])){
							errorLlenandoCola	= true; PRINTF_ERROR("No se pudo encolar bufer a fuente.\n"); NBASSERT(false)
						} else {
							conteoBufferesAgregados++;
						}
					}
				}
				/*if(!NBGestorSonidos::cargarBufferStreamALEnFuente(_fuenteOpenAL, propiedades, _bufferTemporalDatosSonido, bytesUsadosBufferAL)){
					PRINTF_INFO("STREAM error, no se pudo agregar BUFFER a STREAM\n");
					errorLlenandoCola = true;
				} else {
					conteoBufferesAgregados++;
					//PRINTF_INFO("STREAM, bufferAL agregado a la fila (%f segundos) %f segs total en fila\n", nuevoBuffer->segundosBufferAL(), segundosTotal);
				}*/
			}
		}
		if(conteoBufferesAgregados!=0 && _reproduciendoSonido){
			if(!NBGestorSonidos::fuenteReproduciendo(_fuenteOpenAL)){
				//PRINTF_INFO("STREAM ADVERTENCIA, el stream %d se habia detenido\n", _fuenteOpenAL->idFuenteOpenAL());
				NBGestorSonidos::fuenteReproducir(_fuenteOpenAL);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

bool AUEscenaSonidoStreamMulti::agregarXmlInternoEn(AUEscenaSonidoStreamMulti* sonido, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::agregarXmlInternoEn")
	guardarEn->agregarConFormato("%s  <snd>\r\n", espaciosBaseIzq);
	AUEscenaSnd::agregarXmlInternoEn(sonido, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s  </snd>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s  <rep>%d</rep>\r\n", espaciosBaseIzq, sonido->_repetir?1:0);
	guardarEn->agregarConFormato("%s  <grp>%d</grp>\r\n", espaciosBaseIzq, (SI32)sonido->_grupoAudio);
	guardarEn->agregarConFormato("%s  <vol>%f</vol>\r\n", espaciosBaseIzq, sonido->_volumen);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
	
}

bool AUEscenaSonidoStreamMulti::interpretarXmlInterno(AUEscenaSonidoStreamMulti* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::interpretarXmlInterno")
	bool exito = false;
	const sNodoXML* nodoSnd = datosXml->nodoHijo("snd", nodoXml);
	if(nodoSnd != NULL){
		if(AUEscenaSnd::interpretarXmlInterno(cargarEn, datosXml, nodoSnd)){
			exito = true;
			cargarEn->establecerRepetir(datosXml->nodoHijo("rep", true, nodoXml));
			cargarEn->_grupoAudio	= datosXml->nodoHijo("grp", (SI32)ENAudioGrupo_Efectos, nodoXml); //pendiente establecer el grupo de audio en el Gestor
			cargarEn->establecerVolumen(datosXml->nodoHijo("vol", 1.0f, nodoXml));
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaSonidoStreamMulti::agregarBitsInternosEn(AUEscenaSonidoStreamMulti* sonido, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::agregarBitsInternosEn")
	float versionDatos = 1.0f;		guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	AUEscenaSnd::agregarBitsInternosEn(sonido, guardarEn);
	bool rep = sonido->_repetir;	guardarEn->escribir(&rep, sizeof(rep), 1, guardarEn);
	UI8 grp	 = sonido->_grupoAudio;	guardarEn->escribir(&grp, sizeof(grp), 1, guardarEn);
	float vol = sonido->_volumen;	guardarEn->escribir(&vol, sizeof(vol), 1, guardarEn);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUEscenaSonidoStreamMulti::interpretarBitsInternos(AUEscenaSonidoStreamMulti* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStreamMulti::interpretarBitsInternos")
	bool exito = false;
	float versionDatos; cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		AUEscenaSnd::interpretarBitsInternos(cargarEn, cargarDe);
		bool rep; cargarDe->leer(&rep, sizeof(rep), 1, cargarDe);	cargarEn->establecerRepetir(rep);
		UI8 grp; cargarDe->leer(&grp, sizeof(grp), 1, cargarDe);	cargarEn->_grupoAudio = grp; //pendiente establecer el grupo de audio en el Gestor
		float vol; cargarDe->leer(&vol, sizeof(vol), 1, cargarDe);	cargarEn->establecerVolumen(vol);
		exito = true;
	} else {
		PRINTF_ERROR("AUEscenaSonido, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaSonidoStreamMulti, AUEscenaSnd)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaSonidoStreamMulti, "AUEscenaSonidoStreamMulti", AUEscenaSnd)
AUOBJMETODOS_CLONAR_THIS(AUEscenaSonidoStreamMulti)


#endif //#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO

