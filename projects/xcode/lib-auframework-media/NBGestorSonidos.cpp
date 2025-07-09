//
//  NBGestorSonidos.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 21/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "NBGestorSonidos.h"

#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO

//Configuracion de importacion de NIXTLA
#define NIX_MALLOC(POINTER_DEST, POINTER_TYPE, SIZE_BYTES, STR_HINT) POINTER_DEST = (POINTER_TYPE*)NBGestorMemoria::reservarMemoria(SIZE_BYTES, ENMemoriaTipo_General); NB_DEFINE_NOMBRE_PUNTERO(POINTER_DEST, STR_HINT)
#define NIX_FREE(POINTER) NBGestorMemoria::liberarMemoria(POINTER);
#define NIX_SOURCES_GROWTH	32
#define NIX_BUFFERS_GROWTH	32
#define NIX_AUDIO_GROUPS_SIZE 2
//
#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	#define NIX_ASSERTS_ACTIVATED
#endif
//
#ifdef CONFIG_NB_DESHABILITAR_IMPRESIONES_PRINTF
	#define NIX_SILENT_MODE
#endif
//
//#define NIX_VERBOSE_MODE

#include "nixtla-audio.h"
#include "nixtla-audio.c"

//

bool											NBGestorSonidos::_gestorInicializado = false;

UI8												NBGestorSonidos::_maximasFuentesGenerar = 0;	//cantidad maxima de fuentesAL a generar. La cantidad maxima posible depende de cada implementacion de OpenAL (algunas dependen e Hardware y otras de software)
UI8												NBGestorSonidos::_ciclosPorSegundoEnStreams = 0;//cantidad de segmentos por segundo para los streamings de audio
AUCadenaMutable8*								NBGestorSonidos::_prefijoRutas[ENAudioTipo_Conteo];			//prefijo de rutas para sonidos
AUCadenaMutable8*								NBGestorSonidos::_prefijoRutasCache[ENAudioTipo_Conteo];	//prefijo de rutas para cache sonidos

AUArregloNativoMutableP<IEscuchadorGestionSonidos*>* NBGestorSonidos::_escuchadores = NULL;

AUArregloNativoMutableP<STDatosFuentesAL>*		NBGestorSonidos::_fuentesAL = NULL;				//fuentes AL creados (general, ningun otro objeto deberia crear)
AUArregloNativoMutableP<STDatosBufferAL>*		NBGestorSonidos::_bufferesAL = NULL;			//bufferes cargados y pendientes de cargar
AUArregloNativoMutableP<STDatosBufferStream>*	NBGestorSonidos::_bufferesStream = NULL;		//bufferes de stream
UI32											NBGestorSonidos::_bufferesPendCargar = 0;		//cantidad de bufferes pendientes de cargar
AUArregloNativoMutableP<STDatosStream>*			NBGestorSonidos::_streamsAL = NULL;				//streams para la carga de bufferes

ENGestorSonidoModo								NBGestorSonidos::_modoDeCarga = ENGestorSonidoModo_cargaInmediata;

//Nixtla-audio
static STNix_Engine								nbGestorSonidos_nixtlaMotor;

//Capacidades
bool											NBGestorSonidos::_soportaCapturaSonido = false;
bool											NBGestorSonidos::_soportaBufferEstaticos = false;
bool											NBGestorSonidos::_soportaDesplazamientoEnFuentes = false;

bool											NBGestorSonidos::_bloqueado = false;

#define NBGSONIDOS_GET_NIXFUENTE_INI(NOMVAR_INDICE, NOMVAR_PTRSTRUCT) \
	NBASSERT(NOMVAR_INDICE!=0) \
	if(NOMVAR_INDICE!=0){ \
		NBASSERT(NOMVAR_INDICE < _fuentesAL->conteo) \
		if(NOMVAR_INDICE < _fuentesAL->conteo){ \
			STDatosFuentesAL* NOMVAR_PTRSTRUCT = &(_fuentesAL->elemento[NOMVAR_INDICE]); \
			NBASSERT(NOMVAR_PTRSTRUCT->regOcupado) \
			if(NOMVAR_PTRSTRUCT->regOcupado){ \
				NBASSERT(NOMVAR_PTRSTRUCT->fuenteAL!=0) \
				if(NOMVAR_PTRSTRUCT->fuenteAL!=0){
					
#define NBGSONIDOS_GET_NIXFUENTE_FIN \
				} \
			} \
		} \
	} \

#define NBGSONIDOS_GET_NIXBUFFER_INI(NOMVAR_INDICE, NOMVAR_PTRSTRUCT) \
	NBASSERT(NOMVAR_INDICE!=0) \
	if(NOMVAR_INDICE!=0){ \
		NBASSERT(NOMVAR_INDICE < _bufferesAL->conteo) \
		if(NOMVAR_INDICE < _bufferesAL->conteo){ \
			STDatosBufferAL* NOMVAR_PTRSTRUCT = &(_bufferesAL->elemento[NOMVAR_INDICE]); \
			NBASSERT(NOMVAR_PTRSTRUCT->regOcupado) \
			if(NOMVAR_PTRSTRUCT->regOcupado){ \
				NBASSERT(NOMVAR_PTRSTRUCT->bufferAL!=0) \
				if(NOMVAR_PTRSTRUCT->bufferAL!=0){

#define NBGSONIDOS_GET_NIXBUFFER_FIN \
				} \
			} \
		} \
	} \

bool NBGestorSonidos::inicializar(UI8 maximasFuentesEstaticasGenerar, UI8 ciclosPorSegundoEnStreams){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::inicializar")
	bool exito = false;
	//
	_maximasFuentesGenerar			= maximasFuentesEstaticasGenerar;
	_ciclosPorSegundoEnStreams		= ciclosPorSegundoEnStreams;
	_modoDeCarga					= ENGestorSonidoModo_cargaInmediata;
	UI16 i;
	for(i = 0; i < ENAudioTipo_Conteo; i++){
		_prefijoRutas[i]			= new(ENMemoriaTipo_General) AUCadenaMutable8();							NB_DEFINE_NOMBRE_PUNTERO(_prefijoRutas, "NBGestorSonidos::_prefijoRutas");
		_prefijoRutasCache[i]		= new(ENMemoriaTipo_General) AUCadenaMutable8();							NB_DEFINE_NOMBRE_PUNTERO(_prefijoRutasCache, "NBGestorSonidos::_prefijoRutasCache");
	}
	_fuentesAL						= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STDatosFuentesAL>();	NB_DEFINE_NOMBRE_PUNTERO(_fuentesAL, "NBGestorSonidos::_fuentesAL");
	_bufferesAL						= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STDatosBufferAL>();	NB_DEFINE_NOMBRE_PUNTERO(_bufferesAL, "NBGestorSonidos::_bufferesAL");
	_streamsAL						= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STDatosStream>();		NB_DEFINE_NOMBRE_PUNTERO(_streamsAL, "NBGestorSonidos::_streamsAL");
	_bufferesStream					= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STDatosBufferStream>(); NB_DEFINE_NOMBRE_PUNTERO(_bufferesStream, "NBGestorSonidos::_bufferesStream");
	_escuchadores					= new(ENMemoriaTipo_General) AUArregloNativoMutableP<IEscuchadorGestionSonidos*>(); NB_DEFINE_NOMBRE_PUNTERO(_escuchadores, "NBGestorSonidos::_escuchadores");
	_bufferesPendCargar				= 0;
	STDatosFuentesAL fuenteCero;	_fuentesAL->agregarElemento(fuenteCero);		//El indice cero esta reservado
	STDatosBufferAL bufferCero;		_bufferesAL->agregarElemento(bufferCero);		//El indice cero esta reservado
	STDatosBufferStream buffStCero;	_bufferesStream->agregarElemento(buffStCero);	//El indice cero esta reservado
	STDatosStream streamCero;		_streamsAL->agregarElemento(streamCero);		//El indice cero esta reservado
	//
	_soportaCapturaSonido			= false;
	_soportaBufferEstaticos			= false;
	_soportaDesplazamientoEnFuentes	= false;
	//
	_bloqueado						= false;
	//
	if(!nixInit(&nbGestorSonidos_nixtlaMotor, _maximasFuentesGenerar)){
		PRINTF_ERROR("no se pudo inicializar nixtla-audio.\n");
	} else {
		UI32 mascaraCaps				= nixCapabilities(&nbGestorSonidos_nixtlaMotor);
		_soportaCapturaSonido			= ((mascaraCaps & NIX_CAP_AUDIO_CAPTURE) != 0);
		_soportaBufferEstaticos			= ((mascaraCaps & NIX_CAP_AUDIO_STATIC_BUFFERS) != 0);
		_soportaDesplazamientoEnFuentes	= ((mascaraCaps & NIX_CAP_AUDIO_SOURCE_OFFSETS) != 0);
		//nixSrcGroupSetEnabled(&nbGestorSonidos_nixtlaMotor, ENAudioGrupo_Efectos, NIX_TRUE);
		//nixSrcGroupSetEnabled(&nbGestorSonidos_nixtlaMotor, ENAudioGrupo_Musica, NIX_TRUE);
		//nixSrcGroupSetVolume(&nbGestorSonidos_nixtlaMotor, ENAudioGrupo_Efectos, 0.5f);
		//nixSrcGroupSetVolume(&nbGestorSonidos_nixtlaMotor, ENAudioGrupo_Musica, 0.9f);
		exito							= true;
		_gestorInicializado				= true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return exito;
}

void NBGestorSonidos::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::finalizar")
	UI16 i;
	for(i = 0; i < ENAudioTipo_Conteo; i++){
		if(_prefijoRutas[i] != NULL) _prefijoRutas[i]->liberar(NB_RETENEDOR_NULL); _prefijoRutas[i] = NULL;
		if(_prefijoRutasCache[i] != NULL) _prefijoRutasCache[i]->liberar(NB_RETENEDOR_NULL); _prefijoRutasCache[i] = NULL;
	}
	if(_fuentesAL != NULL){
		UI32 i; const UI32 uso = _fuentesAL->conteo;
		for(i=1; i<uso; i++){ //Indice cero esta reservado
			STDatosFuentesAL* datos = &(_fuentesAL->elemento[i]);
			if(datos->regOcupado){
				//Notificar al usuario
				if(datos->actualUsuario != NULL){
					datos->actualUsuario->sndLiberarFuente(datos->fuenteAL);
					datos->actualUsuario = NULL;
				}
				//Liberar la fuente
				nixSourceRelease(&nbGestorSonidos_nixtlaMotor, datos->fuenteAL);
				datos->regOcupado = false;
			}
		}
		_fuentesAL->vaciar();
		_fuentesAL->liberar(NB_RETENEDOR_NULL);
		_fuentesAL = NULL;
	}
	if(_bufferesAL != NULL){
		UI32 i; const UI32 uso = _bufferesAL->conteo;
		for(i=1; i<uso; i++){ //Indice cero esta reservado
			STDatosBufferAL* datos = &(_bufferesAL->elemento[i]);
			if(datos->regOcupado){
				if(datos->bufferAL != 0){
					nixBufferRelease(&nbGestorSonidos_nixtlaMotor, datos->bufferAL);
				} else {
					NBASSERT(_bufferesPendCargar!=0); //Si falla, hay un problema con el manejor de este contador cache
					_bufferesPendCargar--;
				}
				datos->nombreRecurso->liberar(NB_RETENEDOR_NULL);
				datos->regOcupado = false;
			}
		}
		_bufferesAL->vaciar();
		_bufferesAL->liberar(NB_RETENEDOR_NULL);
		_bufferesAL = NULL;
	}
	if(_bufferesStream != NULL){
		UI32 i; const UI32 uso = _bufferesStream->conteo;
		for(i=1; i<uso; i++){
			STDatosBufferStream* datos = &(_bufferesStream->elemento[i]);
			nixBufferRelease(&nbGestorSonidos_nixtlaMotor, datos->buffer);
		}
		_bufferesStream->vaciar();
		_bufferesStream->liberar(NB_RETENEDOR_NULL);
		_bufferesStream = NULL;
	}
	if(_streamsAL != NULL){
		UI32 i; const UI32 uso = _streamsAL->conteo;
		for(i=1; i<uso; i++){ //Indice cero esta reservado
			STDatosStream* datos = &(_streamsAL->elemento[i]);
			if(datos->regOcupado){
				datos->nombreRecurso->liberar(NB_RETENEDOR_NULL);
				datos->stream->liberar(NB_RETENEDOR_NULL);
			}
		}
		_streamsAL->vaciar();
		_streamsAL->liberar(NB_RETENEDOR_NULL);
		_streamsAL = NULL;
	}
	if(_escuchadores != NULL) _escuchadores->liberar(NB_RETENEDOR_NULL); _escuchadores = NULL;
	NBASSERT(_bufferesPendCargar==0) //Si falla, hay un problema con el manejor de este contador cache
	nixFinalize(&nbGestorSonidos_nixtlaMotor);
	_gestorInicializado = false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorSonidos::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _gestorInicializado;
}

//

bool NBGestorSonidos::contextIsActive(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::contextIsActive")
	NBASSERT(_gestorInicializado)
	bool r = false;
	r = (nixContextIsActive(&nbGestorSonidos_nixtlaMotor) == NIX_TRUE);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

bool NBGestorSonidos::contextActivate(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::contextActivate")
	NBASSERT(_gestorInicializado)
	bool r = false;
	r = (nixContextActivate(&nbGestorSonidos_nixtlaMotor) == NIX_TRUE);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

bool NBGestorSonidos::contextDeactivate(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::contextDeactivate")
	NBASSERT(_gestorInicializado)
	bool r = false;
	r = (nixContextDeactivate(&nbGestorSonidos_nixtlaMotor) == NIX_TRUE);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

/*void NBGestorSonidos::dameContexto(void* destino){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::dameContexto")
	NBASSERT(_gestorInicializado)
	nixGetContext(&nbGestorSonidos_nixtlaMotor, destino);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}*/

//

bool NBGestorSonidos::grupoAudioHabilitado(ENAudioGrupo grupoAudio){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::grupoAudioHabilitado")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return nixSrcGroupIsEnabled(&nbGestorSonidos_nixtlaMotor, grupoAudio);
}

float NBGestorSonidos::grupoAudioMultiplicadorVolumen(ENAudioGrupo grupoAudio){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::grupoAudioMultiplicadorVolumen")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return nixSrcGroupGetVolume(&nbGestorSonidos_nixtlaMotor, grupoAudio);
}

void NBGestorSonidos::establecerGrupoAudioHabilitado(ENAudioGrupo grupoAudio, bool habilitado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::establecerGrupoAudioHabilitado")
	NBASSERT(_gestorInicializado)
	const bool habilitadoAnt = nixSrcGroupIsEnabled(&nbGestorSonidos_nixtlaMotor, grupoAudio);
	if(habilitadoAnt != habilitado){
		nixSrcGroupSetEnabled(&nbGestorSonidos_nixtlaMotor, grupoAudio, habilitado ? NIX_TRUE : NIX_FALSE);
		const bool habilitadoNvo = nixSrcGroupIsEnabled(&nbGestorSonidos_nixtlaMotor, grupoAudio);
		if(habilitadoAnt != habilitadoNvo){
			UI16 i; const UI16 uso = _escuchadores->conteo;
			for(i=0; i<uso; i++){
				_escuchadores->elemento[i]->sndGrupoCambioHabilitado(grupoAudio, habilitadoNvo);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorSonidos::establecerGrupoAudioMultiplicadorVolumen(ENAudioGrupo grupoAudio, float multiplicadorVolumen){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::establecerGrupoAudioMultiplicadorVolumen")
	NBASSERT(_gestorInicializado)
	nixSrcGroupSetVolume(&nbGestorSonidos_nixtlaMotor, grupoAudio, multiplicadorVolumen);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorSonidos::agregarEscuchador(IEscuchadorGestionSonidos* escuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::agregarEscuchador")
	NBASSERT(_gestorInicializado)
	_escuchadores->agregarElemento(escuchador);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorSonidos::quitarEscuchador(IEscuchadorGestionSonidos* escuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::quitarEscuchador")
	NBASSERT(_gestorInicializado)
	_escuchadores->quitarElemento(escuchador);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

STGestorSonidosEstado NBGestorSonidos::estadoGestor(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::tickSonido")
	NBASSERT(_gestorInicializado)
	STGestorSonidosEstado estado;
	STNix_StatusDesc nixEstado; nixGetStatusDesc(&nbGestorSonidos_nixtlaMotor, &nixEstado);
	//Sources
	estado.countSources = nixEstado.countSources;
	estado.countSourcesReusable = nixEstado.countSourcesReusable;
	estado.countSourcesAssigned = nixEstado.countSourcesAssigned;
	estado.countSourcesStatic = nixEstado.countSourcesStatic;
	estado.countSourcesStream = nixEstado.countSourcesStream;
	//PLay buffers
	estado.countPlayBuffers = nixEstado.countPlayBuffers;
	estado.sizePlayBuffers = nixEstado.sizePlayBuffers;
	estado.sizePlayBuffersAtSW = nixEstado.sizePlayBuffersAtSW;
	//Record buffers
	estado.countRecBuffers = nixEstado.countRecBuffers;
	estado.sizeRecBuffers = nixEstado.sizeRecBuffers;
	estado.sizeRecBuffersAtSW = nixEstado.sizeRecBuffersAtSW;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return estado;
}

void NBGestorSonidos::tickSonido(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::tickSonido")
	NBASSERT(_gestorInicializado)
	if(nixContextIsActive(&nbGestorSonidos_nixtlaMotor) == NIX_TRUE){
		nixTick(&nbGestorSonidos_nixtlaMotor);
	}
	NBGestorSonidos::liberarBufferesSinReferencias();
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

UI16 NBGestorSonidos::liberarBufferesSinReferencias(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::liberarBufferesSinReferencias")
	NBASSERT(_gestorInicializado)
	UI16 conteoLiberados = 0;
	//liberar los bufferes cargados
	SI32 iBufferAL, iBufferALConteo = _bufferesAL->conteo;
	for(iBufferAL=1; iBufferAL<iBufferALConteo; iBufferAL++){ //El indice cero esta reservado
		STDatosBufferAL* datosBufferAL = &(_bufferesAL->elemento[iBufferAL]);
		if(datosBufferAL->regOcupado && datosBufferAL->bufferAL!=0){
			if(datosBufferAL->conteoRetenciones == 0){
				nixBufferRelease(&nbGestorSonidos_nixtlaMotor, datosBufferAL->bufferAL);
				datosBufferAL->nombreRecurso->liberar(NB_RETENEDOR_NULL);
				datosBufferAL->nombreRecurso	= NULL;
				datosBufferAL->bufferAL			= 0;
				datosBufferAL->regOcupado		= false;
				conteoLiberados++;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return conteoLiberados;
}

//

bool NBGestorSonidos::soportaBufferesEstaticos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::soportaBufferesEstaticos")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _soportaBufferEstaticos;
}

bool NBGestorSonidos::soportaDesplazamientoEnFuentes(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::soportaDesplazamientoEnFuentes")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _soportaDesplazamientoEnFuentes;
}

ENGestorSonidoModo NBGestorSonidos::modoDeCarga(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::modoDeCarga")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _modoDeCarga;
}

void NBGestorSonidos::establecerModoDeCarga(ENGestorSonidoModo modoDeCarga){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::establecerModoDeCarga")
	NBASSERT(_gestorInicializado)
	_modoDeCarga = modoDeCarga;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

SI32 NBGestorSonidos::conteoBufferesPendientesDeCargar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::conteoBufferesPendientesDeCargar")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _bufferesPendCargar;
}

void NBGestorSonidos::cargarBufferesPendientesDeCargar(SI32 cantidadCargar){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::cargarBufferesPendientesDeCargar")
	NBASSERT(_gestorInicializado)
	if(_bufferesPendCargar!=0){
		SI32 iBufferAL, conteoCargados = 0;
		for(iBufferAL = 1; iBufferAL < _bufferesAL->conteo; iBufferAL++){
			STDatosBufferAL* datosBuffer = &(_bufferesAL->elemento[iBufferAL]);
			if(datosBuffer->regOcupado && datosBuffer->bufferAL==0){
				//Cargar contenido desde archivo
				AUCadenaMutable8* rutaCompleta = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(rutaCompleta, "NBGestorSonidos::rutaCompleta")
				if(datosBuffer->tipoAudio < ENAudioTipo_Conteo){
					AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(strTmp, "NBGestorSonidos::strTmp")
					strTmp->establecer(_prefijoRutas[datosBuffer->tipoAudio]->str());
					strTmp->agregar(datosBuffer->nombreRecurso->str());
					rutaCompleta->establecer(NBGestorArchivos::rutaHaciaRecursoEnPaquete(datosBuffer->nombreRecurso->str()));
					strTmp->liberar(NB_RETENEDOR_NULL);
				} else {
					rutaCompleta->establecer(datosBuffer->nombreRecurso->str());
				}
				AUSonidoMutable* sonido = new(ENMemoriaTipo_Temporal) AUSonidoMutable(); NB_DEFINE_NOMBRE_PUNTERO(sonido, "NBGestorSonidos::sonido")
				bool contenidoAudioCargado = false;
				if(datosBuffer->formatoArchivo == ENAudioFormato_WAV){
					if(sonido->cargarDesdeArchivoWav(rutaCompleta->str(), true, NULL, 0)){
						contenidoAudioCargado = true;
					}
				} else if(datosBuffer->formatoArchivo == ENAudioFormato_AUD){
					if(sonido->cargarDesdeArchivoDeltas(rutaCompleta->str(), true)){
						contenidoAudioCargado = true;
					}
				}
				//Cargar hacia buffer
				if(contenidoAudioCargado){
					STSonidoDescriptor propiedadesSonido = sonido->propiedades();
					STNix_audioDesc audioDes;
					audioDes.samplesFormat	= ENNix_sampleFormat_int;
					audioDes.channels		= propiedadesSonido.canales;
					audioDes.samplerate		= propiedadesSonido.muestrasPorSegundo;
					audioDes.bitsPerSample	= propiedadesSonido.bitsPorMuestra;
					audioDes.blockAlign		= propiedadesSonido.alineacionBloques;
					const NixUI16 idBuffer		= nixBufferWithData(&nbGestorSonidos_nixtlaMotor, &audioDes, sonido->datos(), propiedadesSonido.bytesDatosPCM);
					if(idBuffer!=0){
						datosBuffer->bufferAL	= idBuffer;
						_bufferesPendCargar--;
						conteoCargados++;
						//PRINTF_INFO("Sonido precargado ha sido cargado\n");
					}
				}
				sonido->liberar(NB_RETENEDOR_NULL);
				rutaCompleta->liberar(NB_RETENEDOR_NULL);
				if(conteoCargados==cantidadCargar) break;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

UI16 NBGestorSonidos::fuenteAsignarEstatica(IUsuarioFuentesAL* asignarA, bool buscarEntreRecicables, ENAudioGrupo grupoAudio){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteAsignarEstatica")
	NBASSERT(_gestorInicializado)
	UI16 iFuenteAsig = 0;
	UI16 idFuente = nixSourceAssignStatic(&nbGestorSonidos_nixtlaMotor, buscarEntreRecicables?NIX_TRUE:NIX_FALSE, grupoAudio, NULL /*PTRNIX_SourceReleaseCallback*/, NULL/*void *releaseCallBackUserData*/);
	if(idFuente!=0){
		iFuenteAsig = NBGestorSonidos::privFuenteAsignar(idFuente, asignarA, buscarEntreRecicables, grupoAudio);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return iFuenteAsig;
}

static void NBGestorSonidos_privStreamBufferUnqueuedCallback(STNix_Engine* engAbs, void* userdata, const NixUI32 sourceIndex, const NixUI16 buffersUnqueuedCount){
	NBGestorSonidos::streamBufferUnqueuedCallback(sourceIndex, buffersUnqueuedCount);
}

void NBGestorSonidos::streamBufferUnqueuedCallback(const UI32 sourceIndex, const UI16 buffersUnqueuedCount){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::privStreamBufferUnqueuedCallback")
	NBASSERT(_gestorInicializado)
	SI32 i;
	//Liberar buferres
	SI32 conteoQuitados = 0; SI32 usoB = _bufferesStream->conteo;
	for(i=1; i<usoB; i++){
		STDatosBufferStream* datbuff = &_bufferesStream->elemento[i];
		if(datbuff->stream==sourceIndex){
			nixBufferRelease(&nbGestorSonidos_nixtlaMotor, datbuff->buffer);
			_bufferesStream->quitarElementoEnIndice(i--); usoB--;
			//
			conteoQuitados++;
			if(conteoQuitados==buffersUnqueuedCount) break;
		}
	}
	NBASSERT(conteoQuitados == buffersUnqueuedCount) //Si falla, no fue encontrado.
	//Notificar a usuario
	const SI32 usoF = _fuentesAL->conteo;
	for(i=1; i<usoF; i++){
		STDatosFuentesAL* fuente = &_fuentesAL->elemento[i];
		if(fuente->regOcupado && fuente->fuenteAL==sourceIndex){
			//nixBufferRelease(&nbGestorSonidos_nixtlaMotor, idBuffer);
			if(fuente->actualUsuario != NULL){
				fuente->actualUsuario->sndStreamBufferQuitadoDeCola(buffersUnqueuedCount);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

UI16 NBGestorSonidos::fuenteAsignarStream(IUsuarioFuentesAL* asignarA, bool buscarEntreRecicables, ENAudioGrupo grupoAudio, const UI8 tamColaBuffers){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteAsignarStream")
	NBASSERT(_gestorInicializado)
	NBASSERT(tamColaBuffers>0)
	UI16 iFuenteAsig = 0;
	UI16 idFuente = nixSourceAssignStream(&nbGestorSonidos_nixtlaMotor, buscarEntreRecicables?NIX_TRUE:NIX_FALSE, grupoAudio, NULL /*PTRNIX_SourceReleaseCallback*/, NULL/*void *releaseCallBackUserData*/, tamColaBuffers, NBGestorSonidos_privStreamBufferUnqueuedCallback /*PTRNIX_StreamBufferUnqueuedCallback bufferUnqueueCallback*/, NULL /*void* bufferUnqueueCallbackData*/);
	if(idFuente!=0){
		iFuenteAsig = NBGestorSonidos::privFuenteAsignar(idFuente, asignarA, buscarEntreRecicables, grupoAudio);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return iFuenteAsig;
}

UI16 NBGestorSonidos::privFuenteAsignar(UI16 nixFuente, IUsuarioFuentesAL* asignarA, bool buscarEntreRecicables, ENAudioGrupo grupoAudio){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::privFuenteAsignar")
	NBASSERT(_gestorInicializado)
	UI16 iFuenteAsig = 0;
	STDatosFuentesAL datosFuente;
	datosFuente.regOcupado			= true;
	datosFuente.fuenteAL			= nixFuente;
	datosFuente.conteoRetenciones	= 0;
	datosFuente.actualUsuario		= asignarA;
	//Reutilizar registro libre
	UI16 i; const UI16 uso = _fuentesAL->conteo;
	for(i=1; i<uso; i++){ //El indice cero esta reservado
		if(!_fuentesAL->elemento[i].regOcupado){
			_fuentesAL->elemento[i]		= datosFuente;
			iFuenteAsig = i;
			break;
		}
	}
	//Agregar nuevo registro
	if(iFuenteAsig==0){
		iFuenteAsig = _fuentesAL->conteo;
		_fuentesAL->agregarElemento(datosFuente);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return iFuenteAsig;
}


void NBGestorSonidos::fuenteRetener(const UI16 fuenteAL){
	NBASSERT(_gestorInicializado)
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	datosFuenteAL->conteoRetenciones++;
	//PRINTF_INFO("fuenteRetener antes(%d) ahora(%d)\n", datosFuenteAL->conteoRetenciones-1, datosFuenteAL->conteoRetenciones);
	NBGSONIDOS_GET_NIXFUENTE_FIN
}

void NBGestorSonidos::fuenteLiberar(const UI16 fuenteAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteLiberar")
	NBASSERT(_gestorInicializado)
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	//PRINTF_INFO("fuenteLiberar antes(%d)\n", datosFuenteAL->conteoRetenciones);
	NBASSERT(datosFuenteAL->conteoRetenciones != 0)
	if(datosFuenteAL->conteoRetenciones != 0){
		datosFuenteAL->conteoRetenciones--;
		if(datosFuenteAL->conteoRetenciones == 0){
			//Notificar al usuario actual
			if(datosFuenteAL->actualUsuario != NULL){
				datosFuenteAL->actualUsuario->sndLiberarFuente(fuenteAL);
				datosFuenteAL->actualUsuario = NULL;
			}
			//Liberar fuente
			nixSourceRelease(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL);
			datosFuenteAL->fuenteAL		= 0;
			datosFuenteAL->regOcupado	= false;
		}
	}
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

UI32 NBGestorSonidos::tamanoBytesParaBufferStream(const STSonidoDescriptor &propiedadesSonido) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::tamanoBytesParaBufferStream")
	NBASSERT(_gestorInicializado)
	UI32 tamanoBytes = ((UI32)propiedadesSonido.canales * (UI32)(propiedadesSonido.bitsPorMuestra / 8) * ((UI32)propiedadesSonido.muestrasPorSegundo / (UI32)_ciclosPorSegundoEnStreams));
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return tamanoBytes;
}

float NBGestorSonidos::tamanoSegundosParaBufferStream(const STSonidoDescriptor &propiedadesSonido) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::tamanoSegundosParaBufferStream")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return (1.0f / (float)_ciclosPorSegundoEnStreams);
}

bool NBGestorSonidos::cargarBufferStreamALEnFuente(const UI16 fuenteAL, const STSonidoDescriptor &propiedadesSonido, UI8* datosSonido, UI32 conteoBytesDatos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::cargarBufferStreamALEnFuente")
	NBASSERT(_gestorInicializado)
	bool exito = false;
	if((propiedadesSonido.canales!=1 && propiedadesSonido.canales!=2) || (propiedadesSonido.bitsPorMuestra!=8 && propiedadesSonido.bitsPorMuestra!=16 && propiedadesSonido.bitsPorMuestra!=32) || (propiedadesSonido.muestrasPorSegundo!=44100 && propiedadesSonido.muestrasPorSegundo!=22050 && propiedadesSonido.muestrasPorSegundo!=11025)){
		PRINTF_ERROR("NBGestorSonidos, no se puede crear buffer para stream con los parametros indicados canales(%d), bitsPorMuestra(%d) frecuecia(%d)\n", propiedadesSonido.canales, propiedadesSonido.bitsPorMuestra, propiedadesSonido.muestrasPorSegundo);
		NBASSERT(false /*Propiedades sonido no validas: cargarBufferStreamALEnFuente*/)
	} else {
		NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
		STNix_audioDesc audioDesc;
		audioDesc.samplesFormat	= ENNix_sampleFormat_int;
		audioDesc.channels		= propiedadesSonido.canales;
		audioDesc.samplerate	= propiedadesSonido.muestrasPorSegundo;
		audioDesc.bitsPerSample	= propiedadesSonido.bitsPorMuestra;
		audioDesc.blockAlign	= propiedadesSonido.alineacionBloques;
		const NixUI16 idBuffer	= nixBufferWithData(&nbGestorSonidos_nixtlaMotor, &audioDesc, datosSonido, conteoBytesDatos);
		if(idBuffer!=0){
			if(!nixSourceStreamAppendBuffer(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL, idBuffer)){
				nixBufferRelease(&nbGestorSonidos_nixtlaMotor, idBuffer);
			} else {
				STDatosBufferStream datStream;
				datStream.stream		= datosFuenteAL->fuenteAL;
				datStream.buffer		= idBuffer;
				_bufferesStream->agregarElemento(datStream);
				exito = true;
			}
		}
		NBGSONIDOS_GET_NIXFUENTE_FIN
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return exito;
}

bool NBGestorSonidos::cargarBufferStreamALEnFuente(const UI16 fuenteAL, const UI16 bufferAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::cargarBufferStreamALEnFuente")
	NBASSERT(_gestorInicializado)
	bool exito = false;
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	NBGSONIDOS_GET_NIXBUFFER_INI(bufferAL, datosBufferAL)
	if(nixSourceStreamAppendBuffer(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL, datosBufferAL->bufferAL)){
		STDatosBufferStream datStream;
		datStream.stream		= datosFuenteAL->fuenteAL;
		datStream.buffer		= datosBufferAL->bufferAL; nixBufferRetain(&nbGestorSonidos_nixtlaMotor, datosBufferAL->bufferAL);
		_bufferesStream->agregarElemento(datStream);
		exito = true;
	}
	NBGSONIDOS_GET_NIXBUFFER_FIN
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return exito;
}

void NBGestorSonidos::fuenteEstablecerBuffer(const UI16 fuenteAL, const UI16 bufferAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteEstablecerBuffer")
	NBASSERT(_gestorInicializado)
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	if(bufferAL==0){
		nixSourceSetBuffer(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL, 0);
	} else {
		NBGSONIDOS_GET_NIXBUFFER_INI(bufferAL, datosBufferAL)
		nixSourceSetBuffer(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL, datosBufferAL->bufferAL);
		NBGSONIDOS_GET_NIXBUFFER_FIN
	}
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorSonidos::fuenteEstablecerRepetir(const UI16 fuenteAL, bool repetir){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteEstablecerRepetir")
	NBASSERT(_gestorInicializado)
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	nixSourceSetRepeat(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL, repetir ? NIX_TRUE : NIX_FALSE);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorSonidos::fuenteEstablecerVolumen(const UI16 fuenteAL, float volumen){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteEstablecerVolumen")
	NBASSERT(_gestorInicializado)
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	nixSourceSetVolume(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL, volumen);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorSonidos::fuenteReproducir(const UI16 fuenteAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteReproducir")
	NBASSERT(_gestorInicializado)
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	nixSourcePlay(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorSonidos::fuentePausar(const UI16 fuenteAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuentePausar")
	NBASSERT(_gestorInicializado)
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	nixSourcePause(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorSonidos::fuenteDetener(const UI16 fuenteAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteDetener")
	NBASSERT(_gestorInicializado)
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	nixSourceStop(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorSonidos::fuenteVaciarCola(const UI16 fuenteAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteVaciarCola")
	NBASSERT(_gestorInicializado)
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	nixSourceEmptyQueue(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorSonidos::fuenteRebobinar(const UI16 fuenteAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteRebobinar")
	NBASSERT(_gestorInicializado)
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	nixSourceRewind(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

float NBGestorSonidos::fuenteVolumen(const UI16 fuenteAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteVolumen")
	NBASSERT(_gestorInicializado)
	float r = 0.0f;
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	r = nixSourceGetVoume(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

bool NBGestorSonidos::fuenteReproduciendo(const UI16 fuenteAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteReproduciendo")
	NBASSERT(_gestorInicializado)
	NixBOOL r = NIX_FALSE;
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	r = nixSourceIsPlaying(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

UI32 NBGestorSonidos::fuenteOffsetMuestras(const UI16 fuenteAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteOffsetMuestras")
	NBASSERT(_gestorInicializado)
	UI32 r = 0;
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	r = nixSourceGetOffsetSamples(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

UI32 NBGestorSonidos::fuenteOffsetBytes(const UI16 fuenteAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteOffsetBytes")
	NBASSERT(_gestorInicializado)
	UI32 r = 0;
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	r = nixSourceGetOffsetBytes(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

void NBGestorSonidos::fuenteEstablecerOffsetMuestra(const UI16 fuenteAL, const UI32 offsetMuestra){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteEstablecerOffsetMuestra")
	NBASSERT(_gestorInicializado)
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	nixSourceSetOffsetSamples(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL, offsetMuestra);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

UI16 NBGestorSonidos::fuenteTotalBuffers(const UI16 fuenteAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteTotalBuffers")
	NBASSERT(_gestorInicializado)
	UI16 r = 0;
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	r = nixSourceGetBuffersCount(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}


UI32 NBGestorSonidos::fuenteTotalBytes(const UI16 fuenteAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteEstablecerOffsetMuestra")
	NBASSERT(_gestorInicializado)
	UI32 r = 0;
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	r = nixSourceGetBytes(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

UI32 NBGestorSonidos::fuenteTotalMuestras(const UI16 fuenteAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteEstablecerOffsetMuestra")
	NBASSERT(_gestorInicializado)
	UI32 r = 0;
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	r = nixSourceGetSamples(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

float NBGestorSonidos::fuenteTotalSegundos(const UI16 fuenteAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteEstablecerOffsetMuestra")
	NBASSERT(_gestorInicializado)
	float r = 0.0f;
	NBGSONIDOS_GET_NIXFUENTE_INI(fuenteAL, datosFuenteAL)
	r = nixSourceGetSeconds(&nbGestorSonidos_nixtlaMotor, datosFuenteAL->fuenteAL);
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

bool NBGestorSonidos::fuenteTieneBufferEnCola(const UI16 fuente, const UI16 buffer){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::fuenteTieneBufferEnCola")
	NBASSERT(_gestorInicializado)
	bool r = false;
	NBGSONIDOS_GET_NIXFUENTE_INI(fuente, datosFuente)
	NBGSONIDOS_GET_NIXBUFFER_INI(buffer, datosBuffer)
	r = (nixSourceHaveBuffer(&nbGestorSonidos_nixtlaMotor, datosFuente->fuenteAL, datosBuffer->bufferAL) == NIX_TRUE);
	NBGSONIDOS_GET_NIXBUFFER_FIN
	NBGSONIDOS_GET_NIXFUENTE_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

// Bloqueo

void NBGestorSonidos::bloquear(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::bloquear")
	NBASSERT(_gestorInicializado)
	_bloqueado = true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorSonidos::desbloquear(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::desbloquear")
	NBASSERT(_gestorInicializado)
	_bloqueado = false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorSonidos::bloqueado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::bloqueado")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _bloqueado;
}

//Prefijos de rutas
const char*	NBGestorSonidos::pefijoRutas(const ENAudioTipo tipo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::pefijoRutas")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _prefijoRutas[tipo]->str();
}

const char*	NBGestorSonidos::pefijoRutasCache(const ENAudioTipo tipo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::pefijoRutasCache")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _prefijoRutasCache[tipo]->str();
}

void NBGestorSonidos::establecerPrefijoRutas(const char* prefijoStr, const ENAudioTipo tipo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::establecerPrefijoRutas")
	NBASSERT(_gestorInicializado)
	_prefijoRutas[tipo]->establecer(prefijoStr);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorSonidos::establecerPrefijoRutasCache(const char* prefijoCacheStr, const ENAudioTipo tipo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::establecerPrefijoRutasCache")
	NBASSERT(_gestorInicializado)
	_prefijoRutasCache[tipo]->establecer(prefijoCacheStr);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}
//

bool NBGestorSonidos::soportaCapturaSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::soportaCapturaSonido")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _soportaCapturaSonido;
}

bool NBGestorSonidos::abrirDispositivoCapturaSonido(ENSonidoFrecuencia frecuencia, float segundosEnBuffer, const char* opcionalNombreDispositivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::abrirDispositivoCapturaSonido")
	NBASSERT(_gestorInicializado)
	STNix_audioDesc audioDesc;
	audioDesc.channels			= 1;
	audioDesc.samplerate		= frecuencia;
	audioDesc.bitsPerSample		= 16;
	audioDesc.blockAlign		= (audioDesc.bitsPerSample / 8) * audioDesc.channels;
	audioDesc.samplesFormat		= ENNix_sampleFormat_int;
	NixBOOL	exito				= nixCaptureInit(&nbGestorSonidos_nixtlaMotor, &audioDesc, 2, audioDesc.samplerate / 2, NULL /*PTRNIX_CaptureBufferFilledCallback*/, NULL /*bufferCaptureCallbackUserData*/);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return exito;
}

void NBGestorSonidos::cerrarDispositivoCapturaSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::cerrarDispositivoCapturaSonido")
	NBASSERT(_gestorInicializado)
	nixCaptureFinalize(&nbGestorSonidos_nixtlaMotor);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorSonidos::capturandoSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::capturandoSonido")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return nixCaptureIsOnProgress(&nbGestorSonidos_nixtlaMotor);
}

bool NBGestorSonidos::iniciarCapturaSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::iniciarCapturaSonido")
	NBASSERT(_gestorInicializado)
	NixBOOL	exito = nixCaptureStart(&nbGestorSonidos_nixtlaMotor);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return exito;
}

void NBGestorSonidos::detenerCapturaSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::detenerCapturaSonido")
	NBASSERT(_gestorInicializado)
	nixCaptureStop(&nbGestorSonidos_nixtlaMotor);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

SI32 NBGestorSonidos::conteoMuestrasEnBufferCaptura(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::conteoMuestrasEnBufferCaptura")
	NBASSERT(_gestorInicializado)
	NixUI32	r = nixCaptureFilledBuffersSamples(&nbGestorSonidos_nixtlaMotor);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

float NBGestorSonidos::segundosEnBufferCaptura(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::segundosEnBufferCaptura")
	NBASSERT(_gestorInicializado)
	float r = nixCaptureFilledBuffersSeconds(&nbGestorSonidos_nixtlaMotor);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

AUSonido* NBGestorSonidos::sonidoEnBufferCaptura(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::sonidoEnBufferCaptura")
	NBASSERT(_gestorInicializado)
	AUSonidoMutable* sonidoCapturado = NULL;
	/*const NixUI32 buffersLlenos = nixCaptureFilledBuffersCount(&nbGestorSonidos_nixtlaMotor);
	if(_dispositivoCapturaOpenAL != NULL){
		ALCint muestrasCapturadas	= 0;
		alcGetIntegerv(_dispositivoCapturaOpenAL, ALC_CAPTURE_SAMPLES, 1, &muestrasCapturadas);
		VERIFICA_ERROR_AL("alcGetIntegerv(ALC_CAPTURE_SAMPLES)")
		if(muestrasCapturadas>0){
			sonidoCapturado = new(ENMemoriaTipo_General) AUSonidoMutable(); NB_DEFINE_NOMBRE_PUNTERO(sonidoCapturado, "NBGestorSonidos::sonidoCapturado")
			STSonidoDescriptor propiedades;
			propiedades.canales					= 1;
			propiedades.bitsPorMuestra			= 16;
			propiedades.alineacionBloques		= (propiedades.canales * (propiedades.bitsPorMuestra/8));
			propiedades.muestrasPorSegundo		= samplerateCapturaActual;
			propiedades.bytesPromedioPorSegundo = (propiedades.muestrasPorSegundo * propiedades.canales * (propiedades.bitsPorMuestra/8));
			propiedades.bytesDatosPCM			= (muestrasCapturadas * propiedades.canales * (propiedades.bitsPorMuestra/8));
			sonidoCapturado->establecerPropiedades(propiedades);
			alcCaptureSamples(_dispositivoCapturaOpenAL, sonidoCapturado->_datosPCM, muestrasCapturadas);
			sonidoCapturado->autoLiberar(NB_RETENEDOR_NULL);
		}
	}*/
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return sonidoCapturado;
}

//Obtener sonidos
AUSonido* NBGestorSonidos::sonidoSinDatosDesdeArchivoWav(const char* nombreWav, const ENAudioTipo tipo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::sonidoSinDatosDesdeArchivoWav")
	NBASSERT(_gestorInicializado)
	AUSonidoMutable* sonidoSinDatos = new(ENMemoriaTipo_Nucleo) AUSonidoMutable(); NB_DEFINE_NOMBRE_PUNTERO(sonidoSinDatos, "NBGestorSonidos::sonidoSinDatos")
	AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(strTmp, "NBGestorSonidos::strTmp")
	strTmp->establecer(_prefijoRutas[tipo]->str());
	strTmp->agregar(nombreWav);
	AUCadenaMutable8* rutaCompleta = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(NBGestorArchivos::rutaHaciaRecursoEnPaquete(strTmp->str())); NB_DEFINE_NOMBRE_PUNTERO(rutaCompleta, "NBGestorSonidos::rutaCompleta")
	if(!sonidoSinDatos->cargarDesdeArchivoWav(rutaCompleta->str(), false, NULL, 0)){
		PRINTF_ERROR("NBGestorSonidos, no se pudo cargar el archivo WAV: '%s'\n", rutaCompleta->str());
		sonidoSinDatos->liberar(NB_RETENEDOR_NULL);
		sonidoSinDatos = NULL;
	} else {
		sonidoSinDatos->autoLiberar(NB_RETENEDOR_NULL);
	}
	rutaCompleta->liberar(NB_RETENEDOR_NULL);
	strTmp->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return sonidoSinDatos;
}

//Obtener sonidos

void NBGestorSonidos::bufferRetener(const UI16 buffer){
	NBASSERT(_gestorInicializado)
	NBGSONIDOS_GET_NIXBUFFER_INI(buffer, datosBufferAL)
	datosBufferAL->conteoRetenciones++;
	NBGSONIDOS_GET_NIXBUFFER_FIN
}

void NBGestorSonidos::bufferLiberar(const UI16 buffer){
	NBASSERT(_gestorInicializado)
	NBGSONIDOS_GET_NIXBUFFER_INI(buffer, datosBufferAL)
	NBASSERT(datosBufferAL->conteoRetenciones!=0)
	if(datosBufferAL->conteoRetenciones != 0){
		datosBufferAL->conteoRetenciones--;
	}
	NBGSONIDOS_GET_NIXBUFFER_FIN
}

UI16 NBGestorSonidos::bufferFrecuencia(const UI16 buffer){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::bufferFrecuencia")
	NBASSERT(_gestorInicializado)
	UI16 r = 0;
	NBGSONIDOS_GET_NIXBUFFER_INI(buffer, datosBufferAL)
	STNix_audioDesc audioDesc = nixBufferAudioDesc(&nbGestorSonidos_nixtlaMotor, datosBufferAL->bufferAL);
	r = audioDesc.samplerate;
	NBGSONIDOS_GET_NIXBUFFER_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

ENBufferALEstado NBGestorSonidos::bufferEstado(const UI16 buffer){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::bufferFrecuencia")
	NBASSERT(_gestorInicializado)
	ENBufferALEstado r = ENBufferALEstado_SinDatos;
	NBGSONIDOS_GET_NIXBUFFER_INI(buffer, datosBufferAL)
	r = (datosBufferAL->bufferAL!=0 ? ENBufferALEstado_Cargado : ENBufferALEstado_SinDatos);
	NBGSONIDOS_GET_NIXBUFFER_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

float NBGestorSonidos::bufferSegundos(const UI16 buffer){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::bufferFrecuencia")
	NBASSERT(_gestorInicializado)
	float r = 0.0f;
	NBGSONIDOS_GET_NIXBUFFER_INI(buffer, datosBufferAL)
	r = nixBufferSeconds(&nbGestorSonidos_nixtlaMotor, datosBufferAL->bufferAL);
	NBGSONIDOS_GET_NIXBUFFER_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

UI16 NBGestorSonidos::bufferDesdeArchivoWav(const char* nombreWavAud, const ENAudioTipo tipo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::bufferDesdeArchivoWav")
	NBASSERT(_gestorInicializado)
	UI16 bufferAL = 0;
	//Buscar entre los bufferes ya cargados
	if(nombreWavAud[0] != '\0'){
		UI32 i; const UI32 uso = _bufferesAL->conteo;
		for(i=1; i<uso; i++){ //El indice cero esta reservado
			STDatosBufferAL* datosBufferAL = &(_bufferesAL->elemento[i]);
			if(datosBufferAL->regOcupado){
				if(datosBufferAL->tipoAudio == tipo && datosBufferAL->nombreRecurso->esIgual(nombreWavAud)){
					bufferAL = i;
					break;
				}
			}
		}
	}
	//Intentar cargar desde el archivo Wav
	if(bufferAL == 0){
		const SI32 tamNombre = AUCadena8::tamano(nombreWavAud);
		if(tamNombre > 4){
			AUSonidoMutable* sonido			= new(ENMemoriaTipo_Temporal) AUSonidoMutable();	NB_DEFINE_NOMBRE_PUNTERO(sonido, "NBGestorSonidos::sonido")
			AUCadenaMutable8* rutaCompleta	= new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(rutaCompleta, "NBGestorSonidos::rutaCompleta")
			if(tipo < ENAudioTipo_Conteo){
				AUCadenaMutable8* strTmp	= new(ENMemoriaTipo_Temporal) AUCadenaMutable8();	NB_DEFINE_NOMBRE_PUNTERO(strTmp, "NBGestorSonidos::strTmp")
				strTmp->agregar(_prefijoRutas[tipo]->str());
				strTmp->agregar(nombreWavAud);
				rutaCompleta->establecer(NBGestorArchivos::rutaHaciaRecursoEnPaquete(strTmp->str()));
				strTmp->liberar(NB_RETENEDOR_NULL);
			} else {
				rutaCompleta->establecer(nombreWavAud);
			}
			//
			const char* strExt4 = &nombreWavAud[tamNombre - 4];
			if(strExt4[0] == '.' && (strExt4[1] == 'W' || strExt4[1] == 'w') && (strExt4[2] == 'A' || strExt4[2] == 'a') && (strExt4[3] == 'V' || strExt4[3] == 'v')){
				if(!sonido->cargarDesdeArchivoWav(rutaCompleta->str(), (_modoDeCarga==ENGestorSonidoModo_cargaInmediata), NULL, 0)){
					PRINTF_ERROR("NBGestorSonidos, no se pudo cargar el archivo WAV: '%s'\n", rutaCompleta->str());
				} else {
					//sonido->debugAnalizarDatosPCMParaCompresion();
					bufferAL = NBGestorSonidos::privBufferDesdeSonido(sonido, ENAudioFormato_WAV, tipo, nombreWavAud, (_modoDeCarga==ENGestorSonidoModo_cargaInmediata));
				}
			} else if(strExt4[0] == '.' && (strExt4[1] == 'A' || strExt4[1] == 'a') && (strExt4[2] == 'U' || strExt4[2] == 'u') && (strExt4[3] == 'D' || strExt4[3] == 'd')){
				if(!sonido->cargarDesdeArchivoDeltas(rutaCompleta->str(), (_modoDeCarga==ENGestorSonidoModo_cargaInmediata))){
					PRINTF_ERROR("NBGestorSonidos, no se pudo cargar el archivo AUD: '%s'\n", rutaCompleta->str());
				} else {
					//sonido->debugAnalizarDatosPCMParaCompresion();
					bufferAL = NBGestorSonidos::privBufferDesdeSonido(sonido, ENAudioFormato_AUD, tipo, nombreWavAud, (_modoDeCarga==ENGestorSonidoModo_cargaInmediata));
				}
#			ifndef CONFIG_NB_UNSUPPORT_OGG_FORMAT
			} else if(strExt4[0] == '.' && (strExt4[1] == 'O' || strExt4[1] == 'o') && (strExt4[2] == 'G' || strExt4[2] == 'g') && (strExt4[3] == 'G' || strExt4[3] == 'g')){
				if(!sonido->cargarDesdeArchivoOgg(rutaCompleta->str(), (_modoDeCarga==ENGestorSonidoModo_cargaInmediata))){
					PRINTF_ERROR("NBGestorSonidos, no se pudo cargar el archivo OGG: '%s'\n", rutaCompleta->str());
				} else {
					//sonido->debugAnalizarDatosPCMParaCompresion();
					bufferAL = NBGestorSonidos::privBufferDesdeSonido(sonido, ENAudioFormato_AUD, tipo, nombreWavAud, (_modoDeCarga==ENGestorSonidoModo_cargaInmediata));
				}
#			endif
			}
			NBASSERT(bufferAL != 0)
			rutaCompleta->liberar(NB_RETENEDOR_NULL);
			sonido->liberar(NB_RETENEDOR_NULL);
		}
	}
	//
	NBASSERT(bufferAL != 0)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return bufferAL;
}

UI16 NBGestorSonidos::bufferDesdeSonido(AUSonido* sonido, const char* enlistarConNombre){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::bufferDesdeSonido")
	NBASSERT(_gestorInicializado)
	UI16 r = NBGestorSonidos::privBufferDesdeSonido(sonido, ENAudioFormato_DESCONOCIDO, ENAudioTipo_Sonidos, enlistarConNombre, (_modoDeCarga==ENGestorSonidoModo_cargaInmediata)/*false*/);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

UI16 NBGestorSonidos::privBufferDesdeSonido(AUSonido* sonido, const ENAudioFormato formatoArchAudio, const ENAudioTipo tipoAudio, const char* enlistarConNombre, bool cargarDatosInmediatamente){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::privBufferDesdeSonido")
	NBASSERT(_gestorInicializado)
	UI16 bufferAL = 0;
	if(sonido != NULL){
		//PENDIENTE: respetar el 'cargarDatosInmediatamente'
		const STSonidoDescriptor propiedadesSonido = sonido->propiedades();
		bufferAL = NBGestorSonidos::privBufferDesdeDatos(&propiedadesSonido, sonido->datos(), propiedadesSonido.bytesDatosPCM, formatoArchAudio, tipoAudio, enlistarConNombre);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return bufferAL;
}

UI16 NBGestorSonidos::bufferDesdeDatos(const STSonidoDescriptor* propiedadesSonido, const UI8* datosSonido, const UI32 conteoBytesDatos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::bufferDesdeDatos")
	NBASSERT(_gestorInicializado)
	UI16 bufferAL = NBGestorSonidos::privBufferDesdeDatos(propiedadesSonido, datosSonido, conteoBytesDatos, ENAudioFormato_DESCONOCIDO, ENAudioTipo_Sonidos, "");
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return bufferAL;
}

UI16 NBGestorSonidos::privBufferDesdeDatos(const STSonidoDescriptor* propiedadesSonido, const UI8* datosSonido, const UI32 conteoBytesDatos, const ENAudioFormato formatoArchAudio, const ENAudioTipo tipoAudio, const char* enlistarConNombre){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::privBufferDesdeDatos")
	NBASSERT(_gestorInicializado)
	UI16 bufferAL = 0;
	STNix_audioDesc audioDesc;
	audioDesc.samplesFormat		= ENNix_sampleFormat_int;
	audioDesc.channels			= propiedadesSonido->canales;
	audioDesc.bitsPerSample		= propiedadesSonido->bitsPorMuestra;
	audioDesc.samplerate		= propiedadesSonido->muestrasPorSegundo;
	audioDesc.blockAlign		= propiedadesSonido->alineacionBloques;
	const UI16 idBuffer			= nixBufferWithData(&nbGestorSonidos_nixtlaMotor, &audioDesc, datosSonido, conteoBytesDatos);
	if(idBuffer==0){
		PRINTF_ERROR("NBGestorSonidos, nixBufferWithData returned '0'.\n");
		NBASSERT(false)
	} else {
		STDatosBufferAL datosBufferAL;
		datosBufferAL.regOcupado		= true;
		datosBufferAL.bufferAL			= idBuffer;
		datosBufferAL.conteoRetenciones	= 0;
		datosBufferAL.formatoArchivo	= formatoArchAudio;
		datosBufferAL.tipoAudio			= tipoAudio;
		datosBufferAL.nombreRecurso		= new(ENMemoriaTipo_Nucleo) AUCadenaMutable8(enlistarConNombre); NB_DEFINE_NOMBRE_PUNTERO(datosBufferAL.nombreRecurso, "NBGestorSonidos::datosBufferAL.nombreRecurso")
		//Buscar un registro libre
		UI32 i; const UI32 uso = _bufferesAL->conteo;
		for(i=1; i<uso; i++){ //El indice cero esta reservado
			if(!_bufferesAL->elemento[i].regOcupado){
				_bufferesAL->elemento[i] = datosBufferAL;
				bufferAL = i;
				break;
			}
		}
		//Agregar nuevo registro
		if(bufferAL==0){
			bufferAL = _bufferesAL->conteo;
			_bufferesAL->agregarElemento(datosBufferAL);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return bufferAL;
}

bool NBGestorSonidos::bufferEstablecerDatos(const UI16 buffer, const STSonidoDescriptor* propiedadesSonido, const UI8* datosSonido, const UI32 conteoBytesDatos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::bufferEstablecerDatos")
	NBASSERT(_gestorInicializado)
	bool r = false;
	NBASSERT(buffer > 0 && buffer < _bufferesAL->conteo)
	NBASSERT(conteoBytesDatos > 0)
	if(buffer > 0 && buffer < _bufferesAL->conteo && conteoBytesDatos > 0){
		NBASSERT(_bufferesAL->elemento[buffer].regOcupado)
		STNix_audioDesc audioDesc;
		audioDesc.samplesFormat		= ENNix_sampleFormat_int;
		audioDesc.channels			= propiedadesSonido->canales; NBASSERT(audioDesc.channels == 1 || audioDesc.channels == 2)
		audioDesc.bitsPerSample		= propiedadesSonido->bitsPorMuestra; NBASSERT(audioDesc.bitsPerSample == 8 || audioDesc.bitsPerSample == 16 || audioDesc.bitsPerSample == 32)
		audioDesc.samplerate		= propiedadesSonido->muestrasPorSegundo; NBASSERT(audioDesc.samplerate > 0)
		audioDesc.blockAlign		= propiedadesSonido->alineacionBloques; NBASSERT(audioDesc.blockAlign > 0)
		r = (nixBufferSetData(&nbGestorSonidos_nixtlaMotor, _bufferesAL->elemento[buffer].bufferAL, &audioDesc, datosSonido, conteoBytesDatos) == NIX_TRUE);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}
//static bool					bufferEstablecerDatos(const UI16 buffer, const STSonidoDescriptor* propiedadesSonido, const UI8* datosSonido, const UI32 conteoBytesDatos);

AUSonidoStream* NBGestorSonidos::streamDesdeArchivo(const char* nombreWavAud, const ENAudioTipo tipo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::streamDesdeArchivo")
	NBASSERT(_gestorInicializado)
	AUSonidoStream* r = NULL;
	AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(strTmp, "NBGestorSonidos::strTmp")
	//Buscar entre los stream ya cargados
	if(nombreWavAud[0] != '\0'){
		UI32 i; const UI32 uso = _streamsAL->conteo;
		for(i=1; i<uso; i++){
			STDatosStream* datosStreamAL = &(_streamsAL->elemento[i]);
			if(datosStreamAL->tipoAudio == tipo && datosStreamAL->nombreRecurso->esIgual(nombreWavAud)){
				r = datosStreamAL->stream;
				//PRINTF_INFO("STREAM reutilizado en gestor\n");
				break;
			}
		}
	}
	//Intentar cargar desde el archivo de audio
	if(r == NULL){
		PRINTF_INFO("Cargando stream desde '%s'.\n", nombreWavAud);
		const SI32 tamNombre = AUCadena8::tamano(nombreWavAud);
		if(tamNombre > 3){
			AUSonidoStream* stream				= new(ENMemoriaTipo_Nucleo) AUSonidoStream(); NB_DEFINE_NOMBRE_PUNTERO(stream, "NBGestorSonidos::stream")
			AUCadenaMutable8* rutaCompleta		= new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(rutaCompleta, "NBGestorSonidos::rutaCompleta")
			if(tipo < ENAudioTipo_Conteo){
				strTmp->establecer(_prefijoRutas[tipo]->str());
				strTmp->agregar(nombreWavAud);
				rutaCompleta->agregar(NBGestorArchivos::rutaHaciaRecursoEnPaquete(strTmp->str()));
			} else {
				rutaCompleta->establecer(nombreWavAud);
			}
			//
			const char* strExt4 = &nombreWavAud[tamNombre - 4];
			if(strExt4[0] == '.' && (strExt4[1] == 'W' || strExt4[1] == 'w') && (strExt4[2] == 'A' || strExt4[2] == 'a') && (strExt4[3] == 'V' || strExt4[3] == 'v')){
				if(!stream->abrirFlujoWav(rutaCompleta->str(), NULL, 0)){
					PRINTF_ERROR("NBGestorSonidos, no se pudo abrir el flujo/stream al archivo WAV: '%s'\n", rutaCompleta->str());
				} else {
					//Agregar buffer a arreglo
					STDatosStream datosStreamAL;
					datosStreamAL.stream		= stream; stream->retener(NB_RETENEDOR_NULL);
					datosStreamAL.tipoAudio		= tipo;
					datosStreamAL.nombreRecurso	= new(ENMemoriaTipo_Nucleo) AUCadenaMutable8(nombreWavAud); NB_DEFINE_NOMBRE_PUNTERO(datosStreamAL.nombreRecurso, "NBGestorSonidos::datosStreamAL.nombreRecurso")
					_streamsAL->agregarElemento(datosStreamAL);
					r							= stream;
				}
			} else if(strExt4[0] == '.' && (strExt4[1] == 'A' || strExt4[1] == 'a') && (strExt4[2] == 'U' || strExt4[2] == 'u') && (strExt4[3] == 'D' || strExt4[3] == 'd')){
				if(!stream->abrirFlujoDeltas(rutaCompleta->str())){
					PRINTF_ERROR("NBGestorSonidos, no se pudo abrir el flujo/stream al archivo AUD: '%s'\n", rutaCompleta->str());
				} else {
					//Agregar buffer a arreglo
					STDatosStream datosStreamAL;
					datosStreamAL.stream		= stream; stream->retener(NB_RETENEDOR_NULL);
					datosStreamAL.tipoAudio		= tipo;
					datosStreamAL.nombreRecurso	= new(ENMemoriaTipo_Nucleo) AUCadenaMutable8(nombreWavAud); NB_DEFINE_NOMBRE_PUNTERO(datosStreamAL.nombreRecurso, "NBGestorSonidos::datosStreamAL.nombreRecurso")
					_streamsAL->agregarElemento(datosStreamAL);
					r							= stream;
				}
#			ifndef CONFIG_NB_UNSUPPORT_OGG_FORMAT
			} else if(strExt4[0] == '.' && (strExt4[1] == 'O' || strExt4[1] == 'o') && (strExt4[2] == 'G' || strExt4[2] == 'g') && (strExt4[3] == 'G' || strExt4[3] == 'g')){
				if(!stream->abrirFlujoOgg(rutaCompleta->str())){
					PRINTF_ERROR("NBGestorSonidos, no se pudo abrir el flujo/stream al archivo OGG: '%s'\n", rutaCompleta->str());
				} else {
					//Agregar buffer a arreglo
					STDatosStream datosStreamAL;
					datosStreamAL.stream		= stream; stream->retener(NB_RETENEDOR_NULL);
					datosStreamAL.tipoAudio		= tipo;
					datosStreamAL.nombreRecurso	= new(ENMemoriaTipo_Nucleo) AUCadenaMutable8(nombreWavAud); NB_DEFINE_NOMBRE_PUNTERO(datosStreamAL.nombreRecurso, "NBGestorSonidos::datosStreamAL.nombreRecurso")
					_streamsAL->agregarElemento(datosStreamAL);
					r							= stream;
				}
#			endif
			}
			rutaCompleta->liberar(NB_RETENEDOR_NULL);
			stream->liberar(NB_RETENEDOR_NULL);
		}
	}
	strTmp->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

//Consultas
const char* NBGestorSonidos::nombreSonido(const UI16 indBuffer){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::nombreSonido")
	NBASSERT(_gestorInicializado)
	const char* nombre = NULL;
	NBGSONIDOS_GET_NIXBUFFER_INI(indBuffer, datosBufferAL)
	nombre = datosBufferAL->nombreRecurso->str();
	NBGSONIDOS_GET_NIXBUFFER_FIN
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return nombre;
}

const char* NBGestorSonidos::nombreSonido(const AUSonidoStream* stream){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::nombreSonido")
	NBASSERT(_gestorInicializado)
	const char* nombre = NULL;
	UI32 i; const UI32 uso = _streamsAL->conteo;
	for(i=1; i<uso; i++){
		STDatosStream* dStream = &(_streamsAL->elemento[i]);
		if(dStream->regOcupado && dStream->stream==stream){
			nombre = dStream->nombreRecurso->str();
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return nombre;
}

void NBGestorSonidos::estadoSonidos(UI32* guardarBytesReservadosEn, UI32* guardarBytesUsadosEn, UI32* guardarConteoBufferesEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::estadoSonidos")
	NBASSERT(_gestorInicializado)
	STNix_StatusDesc nixEstado; nixGetStatusDesc(&nbGestorSonidos_nixtlaMotor, &nixEstado);
	if(guardarBytesReservadosEn != NULL)	*guardarBytesReservadosEn	= nixEstado.sizePlayBuffers + nixEstado.sizeRecBuffers;
	if(guardarBytesUsadosEn != NULL)		*guardarBytesUsadosEn		= nixEstado.sizePlayBuffers + nixEstado.sizeRecBuffers;
	if(guardarConteoBufferesEn != NULL)	*guardarConteoBufferesEn	= nixEstado.countPlayBuffers;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorSonidos::debugPrintSourcesState(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::nombreSonido")
	NBASSERT(_gestorInicializado)
	nixDbgPrintSourcesStatus(&nbGestorSonidos_nixtlaMotor);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
UI32 NBGestorSonidos::debugBytesTotalBufferes(SI32* guardarConteoFuentesEnUsoEn, SI32* guardarConteoBufferesEn, SI32* guardarConteoBufferesStreamEn, SI32* guardarConteoStreamsEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorSonidos::debugBytesTotalBufferes")
	NBASSERT(_gestorInicializado)
	STNix_StatusDesc nixEstado; nixGetStatusDesc(&nbGestorSonidos_nixtlaMotor, &nixEstado);
	if(guardarConteoFuentesEnUsoEn != NULL) *guardarConteoFuentesEnUsoEn		= nixEstado.countSourcesAssigned;
	if(guardarConteoBufferesEn != NULL) *guardarConteoBufferesEn				= nixEstado.countPlayBuffers;
	if(guardarConteoBufferesStreamEn != NULL) *guardarConteoBufferesStreamEn	= 0;
	if(guardarConteoStreamsEn != NULL) *guardarConteoStreamsEn				= nixEstado.countSourcesStream;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return nixEstado.sizePlayBuffers + nixEstado.sizeRecBuffers;
}
#endif


#endif

