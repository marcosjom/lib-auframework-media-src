//
//  AUEscenaSonidoStream.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 01/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaSonidoStream.h"

#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO

AUEscenaSonidoStream::AUEscenaSonidoStream(ENAudioGrupo grupoAudio, float segundosMinimoEnFila) : AUEscenaSnd(), _bufferes(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::AUEscenaSonidoStream")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSonidoStream");
	privInlineInicializar(grupoAudio, NULL, NULL, segundosMinimoEnFila);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSonidoStream::AUEscenaSonidoStream(ENAudioGrupo grupoAudio, AUEscenaObjeto* objetoEscena, AUSonidoStream* streamSonido, float segundosMinimoEnFila) : AUEscenaSnd(), _bufferes(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::AUEscenaSonidoStream")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSonidoStream");
	privInlineInicializar(grupoAudio, objetoEscena, streamSonido, segundosMinimoEnFila);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSonidoStream::AUEscenaSonidoStream(AUEscenaSonidoStream* otra) : AUEscenaSnd(otra), _bufferes(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::AUEscenaSonidoStream")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSonidoStream");
	privInlineInicializar((ENAudioGrupo)otra->_grupoAudio, NULL, otra->_streamSonido, otra->_segundosMinimoEnFila);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStream::privInlineInicializar(ENAudioGrupo grupoAudio, AUEscenaObjeto* objetoEscena, AUSonidoStream* streamSonido, float segundosMinimoEnFila){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::privInlineInicializar")
	_grupoAudio						= grupoAudio;
	_fuenteOpenAL					= 0;
	_streamSonido					= streamSonido; if(_streamSonido != NULL) _streamSonido->retener(NB_RETENEDOR_THIS);
	_objetoEscena					= NULL; if(objetoEscena != NULL) this->establecerObjetoEscena(objetoEscena);
	_reproduciendoSonido			= true;
	_cantBuffersPorFila				= 0;
	_cantBuffersLlenos				= 0;
	_segundosMinimoEnFila			= segundosMinimoEnFila;
	_indiceMuestraActual			= 0;
	_repetir						= false;
	_volumen						= 1.0f;
	//
	_bufferTemporalDatosSonido		= NULL;
	_bytesBufferTemporalDatosSonido	= 0;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSonidoStream::~AUEscenaSonidoStream(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::~AUEscenaSonidoStream")
	//Advertencia, la retencion del "_objetoEscena" esta creando una retencion ciclica en las Animaciones.
	//La animacion retiene al sonido, y el sonido retiene a la animacion
	//if(_objetoEscena != NULL) _objetoEscena->liberar(NB_RETENEDOR_THIS);
	//Liberar los bufferes
	{
		UI16 i;
		for(i=0; i<_bufferes.conteo; i++){
			NBGestorSonidos::bufferLiberar(_bufferes.elemento[i]);
		}
		_bufferes.vaciar();
	}
	if(_streamSonido != NULL) _streamSonido->liberar(NB_RETENEDOR_THIS);
	if(_bufferTemporalDatosSonido != NULL) NBGestorMemoria::liberarMemoria(_bufferTemporalDatosSonido); _bufferTemporalDatosSonido = NULL;
	if(_fuenteOpenAL != 0) NBGestorSonidos::fuenteLiberar(_fuenteOpenAL); /*this->sndLiberarFuente(_fuenteOpenAL);*/ NBASSERT(_fuenteOpenAL == 0)
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

AUEscenaObjeto* AUEscenaSonidoStream::objetoEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::objetoEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _objetoEscena;
}

AUSonidoStream* AUEscenaSonidoStream::streamSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::streamSonido")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _streamSonido;
}

UI16 AUEscenaSonidoStream::fuenteAL(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::fuenteAL")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _fuenteOpenAL;
}

NBSndOrigenDatos AUEscenaSonidoStream::fuenteDatosSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::fuenteDatosSonido")
	NBSndOrigenDatos sndFuente;
	sndFuente.tipo = ENSndFuente_stream;
	sndFuente.indice = 0;
	sndFuente.puntero = _streamSonido;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return sndFuente; //_streamSonido; PENDIENTE
}


bool AUEscenaSonidoStream::repetir(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::repetir")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _repetir;
}

float AUEscenaSonidoStream::volumen(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::volumen")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _volumen;
}

float AUEscenaSonidoStream::offsetSegundos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::offsetSegundos")
	float offsetSegs = 0.0f;
	if(_streamSonido != NULL){
		offsetSegs = (float)_indiceMuestraActual / (float)_streamSonido->propiedades().muestrasPorSegundo;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return offsetSegs;
}

UI32 AUEscenaSonidoStream::indiceMuestraActual(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::indiceMuestraActual")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _indiceMuestraActual;
}

void AUEscenaSonidoStream::establecerObjetoEscena(AUEscenaObjeto* objetoEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::establecerObjetoEscena")
	//Advertencia, la retencion del "_objetoEscena" esta creando una retencion ciclica en las Animaciones.
	//La animacion retiene al sonido, y el sonido retiene a la animacion
	//if(objetoEscena != NULL) objetoEscena->retener(NB_RETENEDOR_THIS);
	//if(_objetoEscena != NULL) _objetoEscena->liberar(NB_RETENEDOR_THIS);
	_objetoEscena = objetoEscena;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStream::establecerRepetir(bool repetir){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::establecerRepetir")
	//NOTA IMPORTANTE: las fuentesAL con REPEAT activado nunca liberan sus bufferes.
	//Es mejor no establecer el repeat, y repetir manualmente la lectura de archivo.
	_repetir = repetir;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStream::establecerVolumen(float volumen){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::establecerVolumen")
	NBASSERT(volumen>=0.0f && volumen<=1.0f);
	_volumen = volumen;
	if(_fuenteOpenAL!=0){
		NBGestorSonidos::fuenteEstablecerVolumen(_fuenteOpenAL, _volumen);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStream::establecerOffSetSegundos(float segundos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::establecerOffSetSegundos")
	NBASSERT(_streamSonido != NULL)
	if(_streamSonido != NULL){
		if(segundos<0.0f) segundos = 0.0f;
		_indiceMuestraActual = (float)segundos * (float)_streamSonido->propiedades().muestrasPorSegundo;
	}
	if(_reproduciendoSonido && _cantBuffersLlenos == 0){
		this->privLlenarCola();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

void AUEscenaSonidoStream::privInlineAsegurarFuenteAL(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::privInlineAsegurarFuenteAL")
	//Asegurar fuenteAL
	if(_fuenteOpenAL == 0){
		STSonidoDescriptor propsSnd	= _streamSonido->propiedades();
		const UI32 bytesPorbuffer	= NBGestorSonidos::tamanoBytesParaBufferStream(propsSnd);
		float segsPorBuffer			= NBGestorSonidos::tamanoSegundosParaBufferStream(propsSnd);
		float cantBuffers			= _segundosMinimoEnFila / segsPorBuffer; if(cantBuffers!=(float)((SI32)cantBuffers)) cantBuffers++;
		_fuenteOpenAL				= NBGestorSonidos::fuenteAsignarStream(this, false, (ENAudioGrupo)_grupoAudio, (UI16)cantBuffers);
		if(_fuenteOpenAL == 0){
			PRINTF_ERROR("STREAM fuenteSonido no recibida (disponible?)\n");
		} else {
			_cantBuffersPorFila		= (UI16)cantBuffers;
			NBGestorSonidos::fuenteRetener(_fuenteOpenAL);
			//NOTA IMPORTANTE: las fuentesAL con REPEAT activado nunca liberan sus bufferes.
			//Es mejor no establecer el repeat, y repetir manualmente.
			//NBGestorSonidos::fuenteEstablecerRepetir(_fuenteOpenAL, _repetir);
			NBGestorSonidos::fuenteEstablecerVolumen(_fuenteOpenAL, _volumen);
		}
	}
	//Asegurar bufferes
	if(_fuenteOpenAL != 0 && _bufferes.conteo == 0){
		STSonidoDescriptor propsSnd		= _streamSonido->propiedades();
		const UI32 bytesPorbuffer		= NBGestorSonidos::tamanoBytesParaBufferStream(propsSnd);
		//Asegurar bufferes
		UI16 i; const UI16 buffersCountBefore = _bufferes.conteo;
		for(i = _bufferes.conteo; i < _cantBuffersPorFila; i++){
			const UI16 buffer = NBGestorSonidos::bufferDesdeDatos(&propsSnd, NULL, bytesPorbuffer);
			NBGestorSonidos::bufferRetener(buffer);
			_bufferes.agregarElemento(buffer);
		}
		//PRINTF_INFO("STREAM, %d nuevos bufferes reservados.\n", (_bufferes.conteo - buffersCountBefore));
		//Cargar los buffers iniciales
		this->privLlenarCola();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//para ser notificado por el NBGestorSonidos que la fuenteSonidoAL ya no me pertence
void AUEscenaSonidoStream::sndLiberarFuente(const UI16 idFuenteSonidoAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::sndLiberarFuente")
	NBASSERT(idFuenteSonidoAL == _fuenteOpenAL)
	if(_fuenteOpenAL != 0){
		PRINTF_INFO("STREAM, fuenteSonido (%d) liberada\n", _fuenteOpenAL);
		//NBGestorSonidos::fuenteLiberar(_fuenteOpenAL); //No invocar (sería recursivo)
		_cantBuffersLlenos = 0;
		_fuenteOpenAL = 0;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//para ser notificado por el NBGestorSonidos que debe continuarse con el llenado de stream
void AUEscenaSonidoStream::sndStreamBufferQuitadoDeCola(const UI16 cantBufferQuitados){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::sndStreamBufferQuitadoDeCola")
	if(_fuenteOpenAL!=0){
		//PRINTF_INFO("STREAM, %d buffers eliminados de cola\n", (SI32)cantBufferQuitados);
		NBASSERT(_cantBuffersLlenos >= cantBufferQuitados)
		_cantBuffersLlenos -= cantBufferQuitados;
		if(_reproduciendoSonido){
			//Cargar siguientes buffers
			this->privLlenarCola();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

bool AUEscenaSonidoStream::reproduciendoSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::reproduciendoSonido")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _reproduciendoSonido;
}

void AUEscenaSonidoStream::reproducirSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::reproducirSonido")
	_reproduciendoSonido = true;
	privInlineAsegurarFuenteAL();
	if(_fuenteOpenAL != 0){
		NBGestorSonidos::fuenteReproducir(_fuenteOpenAL);
	} else {
		PRINTF_WARNING("STREAM, no hay fuente disponible\n");
	}
	if(_reproduciendoSonido && _cantBuffersLlenos == 0){
		this->privLlenarCola();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStream::pausarSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::pausarSonido")
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

void AUEscenaSonidoStream::detenerSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::detenerSonido")
	_reproduciendoSonido = false;
	if(_fuenteOpenAL!=0){
		NBGestorSonidos::fuenteDetener(_fuenteOpenAL);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStream::vaciarCola(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::vaciarCola")
	if(_fuenteOpenAL!=0){
		//PRINTF_INFO("STREAM, cola vaciada (%d buffers deberian estar)\n", (SI32)_cantBuffersLlenos);
		NBGestorSonidos::fuenteVaciarCola(_fuenteOpenAL);
		_cantBuffersLlenos = 0;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStream::rebobinarSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::rebobinarSonido")
	_indiceMuestraActual = 0;
	/*privInlineAsegurarFuenteAL();
	if(_fuenteOpenAL != NULL){
		_indiceMuestraActual = 0;
		//PRINTF_INFO("Rebobinado sondido(%u) con bufferAL(%u)\n", (UI32)_idFuenteOpenAL, (UI32)_bufferAL->idBufferOpenAL());
	} else {
		PRINTF_WARNING("no se pudo rebobinar sonido porque no hay fuente disponible\n");
	}*/
	if(_reproduciendoSonido && _cantBuffersLlenos == 0){
		this->privLlenarCola();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonidoStream::privLlenarCola(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::privLlenarCola")
	if(_fuenteOpenAL!=0 && _streamSonido != NULL){
		bool errorLlenandoCola			= false;
		UI32 conteoBufferesAgregados	= 0;
		STSonidoDescriptor propiedades	= _streamSonido->propiedades();
		while(NBGestorSonidos::fuenteTotalBuffers(_fuenteOpenAL) < _cantBuffersPorFila && !errorLlenandoCola){
			UI32 bytesBufferAL			= NBGestorSonidos::tamanoBytesParaBufferStream(_streamSonido->propiedades());
			if(_bytesBufferTemporalDatosSonido < bytesBufferAL){
				if(_bufferTemporalDatosSonido) NBGestorMemoria::liberarMemoria(_bufferTemporalDatosSonido);
				_bufferTemporalDatosSonido		= (UI8*)NBGestorMemoria::reservarMemoria(bytesBufferAL, ENMemoriaTipo_Temporal); NB_DEFINE_NOMBRE_PUNTERO(_bufferTemporalDatosSonido, "_bufferTemporalDatosSonido");
				_bytesBufferTemporalDatosSonido	= bytesBufferAL;
				//PRINTF_INFO("STREAM, creando buffer temporal\n");
			} else {
				//PRINTF_INFO("STREAM, optimizacion reutilizando buffer temporal\n");
			}
			//
			UI32 bytesCopiados = _streamSonido->cargarDatosDeFlujoEn(_indiceMuestraActual, _bufferTemporalDatosSonido, bytesBufferAL);
			if(bytesCopiados == 0){
				_indiceMuestraActual = 0;
				if(!_repetir){
					//PRINTF_INFO("STREAM, se llego al final del flujo stream (cero bytes copiados)\n");
					_reproduciendoSonido	= false;
					errorLlenandoCola		= true;
				}
				//PRINTF_INFO("STREAM, se ha alcanzado el final del flujo\n");
			} else {
				//PRINTF_INFO("STREAM, datos cargados desde muestra: %d.\n", _indiceMuestraActual);
#				ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				SI32 dbgCantBuffersLlenos = 0;
#				endif
				//Buscar el primer buffer disponible
				SI32 iBufferDisponible = -1; SI32 i;
				for(i = ((SI32)_bufferes.conteo - 1); i >= 0; i--){
					if(!NBGestorSonidos::fuenteTieneBufferEnCola(_fuenteOpenAL, _bufferes.elemento[i])){
						iBufferDisponible = i;
#						ifndef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
						break;
#						endif
#					ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
					} else {
						dbgCantBuffersLlenos++;
#					endif
					}
				}
				NBASSERT(dbgCantBuffersLlenos == _cantBuffersLlenos)
				NBASSERT(iBufferDisponible!=-1) //Deberia haber algun buffer libre
				if(iBufferDisponible == -1){
					errorLlenandoCola	= true; PRINTF_ERROR("No se encontro buffer libre (deberia).\n"); NBASSERT(false)
				} else {
					if(!NBGestorSonidos::bufferEstablecerDatos(_bufferes.elemento[iBufferDisponible], &propiedades, _bufferTemporalDatosSonido, bytesCopiados)){
						errorLlenandoCola	= true; PRINTF_ERROR("No se pudo establecer datos de bufer (desde muestra %d).\n", _indiceMuestraActual); NBASSERT(false)
					} else {
						if(!NBGestorSonidos::cargarBufferStreamALEnFuente(_fuenteOpenAL, _bufferes.elemento[iBufferDisponible])){
							errorLlenandoCola	= true; PRINTF_ERROR("No se pudo encolar bufer a fuente (desde muestra %d).\n", _indiceMuestraActual); NBASSERT(false)
						} else {
							//PRINTF_INFO("STREAM, buffer encolado (desde muestra %d, %d buffers deberian estar encolados).\n", _indiceMuestraActual, (_cantBuffersLlenos + 1));
							_indiceMuestraActual += (float)bytesCopiados / (float)propiedades.alineacionBloques;
							_cantBuffersLlenos++;
							conteoBufferesAgregados++;
						}
					}
				}
				/*if(NBGestorSonidos::cargarBufferStreamALEnFuente(_fuenteOpenAL, propiedades, _bufferTemporalDatosSonido, bytesCopiados)){
					_indiceMuestraActual += (float)bytesCopiados / (float)propiedades.alineacionBloques;
					conteoBufferesAgregados++;
					//PRINTF_INFO("STREAM, bufferAL agregado a la fila (%f segundos) %f segs total en fila\n", nuevoBuffer->segundosBufferAL(), segundosTotal);
				} else {
					PRINTF_ERROR("STREAM error, no se pudo agregar BUFFER a STREAM\n");
					errorLlenandoCola	= true;
					_cantBuffersLlenos++;
				}*/
			}
		}
		if(conteoBufferesAgregados!=0 && _reproduciendoSonido){
			if(!NBGestorSonidos::fuenteReproduciendo(_fuenteOpenAL)){
				PRINTF_WARNING("STREAM ADVERTENCIA, el stream %d se habia detenido\n", _fuenteOpenAL);
				NBGestorSonidos::fuenteReproducir(_fuenteOpenAL);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

bool AUEscenaSonidoStream::agregarXmlInternoEn(AUEscenaSonidoStream* sonido, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::agregarXmlInternoEn")
	guardarEn->agregarConFormato("%s  <snd>\r\n", espaciosBaseIzq);
	AUEscenaSnd::agregarXmlInternoEn(sonido, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s  </snd>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s  <rep>%d</rep>\r\n", espaciosBaseIzq, sonido->_repetir?1:0);
	guardarEn->agregarConFormato("%s  <grp>%d</grp>\r\n", espaciosBaseIzq, (SI32)sonido->_grupoAudio);
	guardarEn->agregarConFormato("%s  <vol>%f</vol>\r\n", espaciosBaseIzq, sonido->_volumen);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
	
}

bool AUEscenaSonidoStream::interpretarXmlInterno(AUEscenaSonidoStream* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::interpretarXmlInterno")
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

bool AUEscenaSonidoStream::agregarBitsInternosEn(AUEscenaSonidoStream* sonido, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::agregarBitsInternosEn")
	float versionDatos = 1.0f;		guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	AUEscenaSnd::agregarBitsInternosEn(sonido, guardarEn);
	bool rep = sonido->_repetir;	guardarEn->escribir(&rep, sizeof(rep), 1, guardarEn);
	UI8 grp	 = sonido->_grupoAudio;	guardarEn->escribir(&grp, sizeof(grp), 1, guardarEn);
	float vol = sonido->_volumen;	guardarEn->escribir(&vol, sizeof(vol), 1, guardarEn);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUEscenaSonidoStream::interpretarBitsInternos(AUEscenaSonidoStream* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonidoStream::interpretarBitsInternos")
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
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaSonidoStream, AUEscenaSnd)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaSonidoStream, "AUEscenaSonidoStream", AUEscenaSnd)
AUOBJMETODOS_CLONAR_THIS(AUEscenaSonidoStream)


#endif //#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO


