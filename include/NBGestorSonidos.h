//
//  NBGestorSonidos.h
//  AUFramework
//
//  Created by Nicaragua Binary on 21/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#if !defined(NBGESTORSONIDOS_H_INCLUIDO) && !defined(CONFIG_NB_UNSUPPORT_AUDIO_IO)
#define NBGESTORSONIDOS_H_INCLUIDO

#include "NBGestorPilaLlamadas.h"
#include "NBGestorArchivos.h"

#include "AUArregloMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUCadenaMutable.h"
#include "AUSonido.h"
#include "AUSonidoStream.h"
#include "AUSonidoMutable.h"
#include "../src/ext/nixtla-audio/include/nixtla-audio.h"

enum ENAudioTipo {
	ENAudioTipo_Sonidos = 0,
	ENAudioTipo_SonidosAud,
	ENAudioTipo_SonidosOgg,
	ENAudioTipo_Musica,
	ENAudioTipo_MusicaAud,
	ENAudioTipo_MusicaOgg,
	ENAudioTipo_Conteo
};

enum ENAudioFormato {
	ENAudioFormato_DESCONOCIDO = 0,
	ENAudioFormato_WAV,
	ENAudioFormato_AUD,
	ENAudioFormato_Conteo
};


#define STR_ERROR_AL(idError) alGetString(idError) //(idError==AL_INVALID_NAME?"AL_INVALID_NAME":idError==AL_INVALID_ENUM?"AL_INVALID_ENUM":idError==AL_INVALID_VALUE?"AL_INVALID_VALUE":idError==AL_INVALID_OPERATION?"AL_INVALID_OPERATION":idError==AL_OUT_OF_MEMORY?"AL_OUT_OF_MEMORY":idError==AL_NO_ERROR?"AL_NO_ERROR":"AL_ERROR_DESCONOCIDO")

enum ENAudioGrupo {				//si se modifica esta enumeracion se debe actualizar la MACRO "NBAUDIO_CONTEO_GRUPOS_AUDIO"
	ENAudioGrupo_Efectos	= 0,
	ENAudioGrupo_Musica,
	ENAudioGrupo_Conteo
};

class IEscuchadorGestionSonidos {
	public:
		virtual ~IEscuchadorGestionSonidos(){
			//
		}
		virtual void	sndGrupoCambioHabilitado(const ENAudioGrupo grupo, const bool nuevoHabilitado) = 0;
};


class IUsuarioFuentesAL {
	public:
		virtual ~IUsuarioFuentesAL(){
			//
		}
		virtual void	sndLiberarFuente(const UI16 fuenteSonidoAL) = 0;
		virtual void	sndStreamBufferQuitadoDeCola(const UI16 cantBufferQuitados) = 0;
};

enum ENGestorSonidoModo {
	ENGestorSonidoModo_cargaInmediata = 0,	//cargar los datos de sonido inmediatamente
	ENGestorSonidoModo_cargaPendiente		//cargar los datos descriptivos pero deja pendiente la carga de datos de sonido
};

enum ENBufferALEstado {
	ENBufferALEstado_SinDatos = 0,	//el buffer fue creado pero no cuenta con datos
	ENBufferALEstado_Cargado		//el buffer cuenta con datos para reproducir
};

struct STDatosSndGroup {
    bool                isDisabled;
    float               volume;
};

struct STDatosFuentesAL {
	bool				regOcupado;
    UI8                 iGroup; //ENAudioGrupo
    STNixSourceRef      source;
    float               volume;
	UI32				conteoRetenciones;
	IUsuarioFuentesAL*	actualUsuario;
	//
	bool operator==(const STDatosFuentesAL &otro) const {
		return (NixSource_isSame(source, otro.source) && actualUsuario==otro.actualUsuario);
	}
	bool operator!=(const STDatosFuentesAL &otro) const {
		return !(NixSource_isSame(source, otro.source) && actualUsuario==otro.actualUsuario);
	}
};

struct STDatosBufferAL {
	bool				regOcupado;
	UI8					tipoAudio;	//ENAudioTipo
	UI8					formatoArchivo; //ENAudioFormato
    STNixBufferRef      buffer;
    STNixAudioDesc      desc;
    UI32                dataUse;
	UI32				conteoRetenciones;
	AUCadenaMutable8*	nombreRecurso;
	//
	bool operator==(const STDatosBufferAL &otro) const {
		return (NixBuffer_isSame(buffer, otro.buffer) && nombreRecurso==otro.nombreRecurso);
	}
	bool operator!=(const STDatosBufferAL &otro) const {
		return !(NixBuffer_isSame(buffer, otro.buffer) && nombreRecurso==otro.nombreRecurso);
	}
};

struct STDatosBufferStream {
    STNixSourceRef      stream;
    STNixBufferRef      buffer;
	//
	bool operator==(const STDatosBufferStream &otro) const {
		return (NixSource_isSame(stream, otro.stream) && NixBuffer_isSame(buffer, otro.buffer));
	}
	bool operator!=(const STDatosBufferStream &otro) const {
		return !(NixSource_isSame(stream, otro.stream) && NixBuffer_isSame(buffer, otro.buffer));
	}
};

struct STDatosStream {
	bool				regOcupado;
	UI8					tipoAudio;	//ENAudioTipo
	AUSonidoStream*		stream;
	AUCadenaMutable8*	nombreRecurso;
	//
	bool operator==(const STDatosStream &otro) const {
		return (stream==otro.stream && nombreRecurso==otro.nombreRecurso);
	}
	bool operator!=(const STDatosStream &otro) const {
		return !(stream==otro.stream && nombreRecurso==otro.nombreRecurso);
	}
};

typedef struct STGestorSonidosEstado_ {
	//Sources
	UI16	countSources;			//All sources
	UI16	countSourcesReusable;	//Only reusable sources. Not-reusable = (countSources - countSourcesReusable);
	UI16	countSourcesAssigned;	//Only sources retained by ussers. Not-assigned = (countSources - countSourcesAssigned);
	UI16	countSourcesStatic;		//Only static sounds sources.
	UI16	countSourcesStream;		//Only stream sounds sources. Undefined-sources = (countSources - countSourcesStatic - countSourcesStream);
	//PLay buffers
	UI16	countPlayBuffers;		//All play-buffers
	UI32	sizePlayBuffers;		//Bytes of all play-buffers
	UI32	sizePlayBuffersAtSW;	//Only bytes of play-buffers that resides in nixtla's memory. sizeBuffersAtExternal = (sizeBuffers - sizeBuffersAtSW); //this includes Hardware-buffers
	//Record buffers
	UI16	countRecBuffers;		//All rec-buffers
	UI32	sizeRecBuffers;			//Bytes of all rec-buffers
	UI32	sizeRecBuffersAtSW;		//Only bytes of rec-buffers that resides in nixtla's memory. sizeBuffersAtExternal = (sizeBuffers - sizeBuffersAtSW); //this includes Hardware-buffers
} STGestorSonidosEstado;

class NBGestorSonidos {
	public:
		static bool					inicializar(UI8 maximasFuentesGenerar, UI8 ciclosPorSegundoEnStreams);
		static void					finalizar();
		static bool					gestorInicializado();
		static bool					contextIsActive();
		static bool					contextActivate();
		static bool					contextDeactivate();
		//static void				dameContexto(void* destino); //Destino debe ser un "ALCcontext**" (en OpenAL) o "SLEngineItf*" (en OpenSL)
		//
		static void					agregarEscuchador(IEscuchadorGestionSonidos* escuchador);
		static void					quitarEscuchador(IEscuchadorGestionSonidos* escuchador);
		//
		static STGestorSonidosEstado estadoGestor();
		static void					tickSonido(float segsTranscurridos);
		//
		static bool					grupoAudioHabilitado(ENAudioGrupo grupoAudio);
		static float				grupoAudioMultiplicadorVolumen(ENAudioGrupo grupoAudio);
		static void					establecerGrupoAudioHabilitado(ENAudioGrupo grupoAudio, bool habilitado);
		static void					establecerGrupoAudioMultiplicadorVolumen(ENAudioGrupo grupoAudio, float multiplicadorVolumen);
		//
		static ENGestorSonidoModo	modoDeCarga();
		static void					establecerModoDeCarga(ENGestorSonidoModo modoDeCarga);
		static SI32					conteoBufferesPendientesDeCargar();
		static void					cargarBufferesPendientesDeCargar(SI32 cantidadCargar);
		static bool					soportaBufferesEstaticos();
		static bool					soportaDesplazamientoEnFuentes();
		//
		static UI32					tamanoBytesParaBufferStream(const STSonidoDescriptor &propiedadesSonido);
		static float				tamanoSegundosParaBufferStream(const STSonidoDescriptor &propiedadesSonido);
		static bool					cargarBufferStreamALEnFuente(const UI16 fuenteAL, const STSonidoDescriptor &propiedadesSonido, UI8* datosSonido, UI32 conteoBytesDatos);
		static bool					cargarBufferStreamALEnFuente(const UI16 fuenteAL, const UI16 bufferAL);
		//Bufferes
		static UI16					bufferDesdeArchivoWav(const char* nombreWavAud, const ENAudioTipo tipo = ENAudioTipo_Sonidos);
		static UI16					bufferDesdeSonido(AUSonido* sonido, const char* enlistarConNombre);
		static UI16					bufferDesdeDatos(const STSonidoDescriptor* propiedadesSonido, const UI8* datosSonido, const UI32 conteoBytesDatos);
		static bool					bufferEstablecerDatos(const UI16 buffer, const STSonidoDescriptor* propiedadesSonido, const UI8* datosSonido, const UI32 conteoBytesDatos);
		static void					bufferRetener(const UI16 buffer);
		static void					bufferLiberar(const UI16 buffer);
		static UI16					bufferFrecuencia(const UI16 buffer);
		static ENBufferALEstado     bufferEstado(const UI16 buffer);
		static float				bufferSegundos(const UI16 buffer);
		//Fuentes (general)
		static UI16					fuenteAsignarEstatica(IUsuarioFuentesAL* asignarA, bool buscarEntreRecicables, ENAudioGrupo grupoAudio);
		static UI16					fuenteAsignarStream(IUsuarioFuentesAL* asignarA, bool buscarEntreRecicables, ENAudioGrupo grupoAudio, const UI8 tamColaBuffers);
		static void					fuenteRetener(const UI16 fuenteAL);
		static void					fuenteLiberar(const UI16 fuenteAL);
		static void					fuenteEstablecerBuffer(const UI16 fuenteAL, const UI16 bufferAL);
		static void					fuenteEstablecerRepetir(const UI16 fuenteAL, bool repetir);
		static void					fuenteEstablecerVolumen(const UI16 fuenteAL, float volumen);
		static void					fuenteReproducir(const UI16 fuenteAL);
		static void					fuentePausar(const UI16 fuenteAL);
		static void					fuenteVaciarCola(const UI16 fuenteAL);
		static void					fuenteDetener(const UI16 fuenteAL);
		static void					fuenteRebobinar(const UI16 fuenteAL);
		static float				fuenteVolumen(const UI16 fuenteAL);
		static bool					fuenteReproduciendo(const UI16 fuenteAL);
		static UI32					fuenteOffsetMuestras(const UI16 fuenteAL);
		static UI32					fuenteOffsetBytes(const UI16 fuenteAL);
		static void					fuenteEstablecerOffsetMuestra(const UI16 fuenteAL, const UI32 offsetMuestra);
		static UI16					fuenteTotalBuffers(const UI16 fuenteAL);
		static UI32					fuenteTotalBytes(const UI16 fuenteAL);
		static UI32					fuenteTotalMuestras(const UI16 fuenteAL);
		static float				fuenteTotalSegundos(const UI16 fuenteAL);
		static bool					fuenteTieneBufferEnCola(const UI16 fuente, const UI16 buffer);
		//
		static void					streamBufferUnqueuedCallback(STNixSourceRef src, const UI16 buffersUnqueuedCount);
		//Bloqueo
		static void					bloquear();
		static void					desbloquear();
		static bool					bloqueado();
		//Prefijos de rutas
		static const char*			pefijoRutas(const ENAudioTipo tipo = ENAudioTipo_Sonidos);
		static const char*			pefijoRutasCache(const ENAudioTipo tipo = ENAudioTipo_Sonidos);
		static void					establecerPrefijoRutas(const char* prefijoStr, const ENAudioTipo tipo = ENAudioTipo_Sonidos);
		static void					establecerPrefijoRutasCache(const char* prefijoCacheStr, const ENAudioTipo tipo = ENAudioTipo_Sonidos);
		//Captura de audio
		static bool					soportaCapturaSonido();
		static bool					capturandoSonido();
		static bool					abrirDispositivoCapturaSonido(ENSonidoFrecuencia frecuencia, float segundosEnBuffer, const char* opcionalNombreDispositivo=NULL);
		static void					cerrarDispositivoCapturaSonido();
		static bool					iniciarCapturaSonido();
		static void					detenerCapturaSonido();
		static SI32					conteoMuestrasEnBufferCaptura();
		static float				segundosEnBufferCaptura();
		static AUSonido*			sonidoEnBufferCaptura();
		//Obtener sonidos
		static AUSonido*			sonidoSinDatosDesdeArchivoWav(const char* nombreWav, const ENAudioTipo tipo = ENAudioTipo_Sonidos);
		static AUSonidoStream*		streamDesdeArchivo(const char* nombreWavAud, const ENAudioTipo tipo = ENAudioTipo_Sonidos);
		//
		static UI16					liberarBufferesSinReferencias();
		//Consultas
		static const char*			nombreSonido(const UI16 indBuffer);
		static const char*			nombreSonido(const AUSonidoStream* stream);
        //2025-07-24: removed
		//static void				estadoSonidos(UI32* guardarBytesReservadosEn, UI32* guardarBytesUsadosEn, UI32* guardarConteoBufferesEn);
		//
        //2025-07-24: removed
		//static void				debugPrintSourcesState();
		#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
		static UI32					debugBytesTotalBufferes(SI32* guardarConteoFuentesEnUsoEn, SI32* guardarConteoBufferesEn, SI32* guardarConteoBufferesStreamEn, SI32* guardarConteoStreamsEn);
		#endif
	private:
		static bool											_gestorInicializado;
		//Reproduccion de sonido
		static ENGestorSonidoModo							_modoDeCarga;
		//Captura de sonido
		static bool											_soportaCapturaSonido;
		static bool											_soportaBufferEstaticos;
		static bool											_soportaDesplazamientoEnFuentes;
		//
		static UI8											_maximasFuentesGenerar;
		static UI8											_ciclosPorSegundoEnStreams;
		static AUCadenaMutable8*							_prefijoRutas[ENAudioTipo_Conteo];			//prefijo de rutas para sonidos
		static AUCadenaMutable8*							_prefijoRutasCache[ENAudioTipo_Conteo];		//prefijo de rutas para cache sonidos
        static STDatosSndGroup                              _grps[ENAudioTipo_Conteo];
		static AUArregloNativoMutableP<STDatosFuentesAL>*	_fuentesAL;				//fuentes AL creados (general, ningun otro objeto deberia crear)
		static AUArregloNativoMutableP<STDatosBufferAL>*	_bufferesAL;			//bufferes AL cargado y precargados
		static AUArregloNativoMutableP<STDatosStream>*		_streamsAL;				//streams para la carga de bufferes
		static AUArregloNativoMutableP<STDatosBufferStream>* _bufferesStream;		//Registro de bufferes de stream
		static AUArregloNativoMutableP<IEscuchadorGestionSonidos*>* _escuchadores;
		static UI32											_bufferesPendCargar;	//Cantidad de bufferes pendientes de cargar
		static bool											_bloqueado;
		//
		static UI16											privFuenteAsignar(STNixSourceRef src, IUsuarioFuentesAL* asignarA, bool buscarEntreRecicables, ENAudioGrupo grupoAudio);
		static UI16											privBufferDesdeSonido(AUSonido* sonido, const ENAudioFormato formatoArchAudio, const ENAudioTipo tipoAudio, const char* enlistarConNombre, bool cargarDatosInmediatamente);
		static UI16											privBufferDesdeDatos(const STSonidoDescriptor* propiedadesSonido, const UI8* datosSonido, const UI32 conteoBytesDatos, const ENAudioFormato formatoArchAudio, const ENAudioTipo tipoAudio, const char* enlistarConNombre);
};

#endif
