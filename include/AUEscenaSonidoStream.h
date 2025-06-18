//
//  AUEscenaSonidoStream.h
//  AUFramework
//
//  Created by Nicaragua Binary on 01/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#if !defined(AUESCENASONIDOSTREAM_H_INCLUIDO) && !defined(CONFIG_NB_UNSUPPORT_AUDIO_IO)
#define AUESCENASONIDOSTREAM_H_INCLUIDO

#include "AUObjeto.h"
#include "AUEscenaObjeto.h"
#include "NBGestorSonidos.h"
#include "NBAnimador.h"
#include "NBGestorAnimadores.h"
#include "AUEscenaSnd.h"
#include "AUSonidoStream.h"
#include "AUArregloNativoMutableP.h"

class AUEscenaSonidoStream : public AUEscenaSnd, public IUsuarioFuentesAL {
	public:
		AUEscenaSonidoStream(ENAudioGrupo grupoAudio, float segundosMinimoEnFila);
		AUEscenaSonidoStream(ENAudioGrupo grupoAudio, AUEscenaObjeto* objetoEscena, AUSonidoStream* streamSonido, float segundosMinimoEnFila);
		AUEscenaSonidoStream(AUEscenaSonidoStream* otra);
		virtual ~AUEscenaSonidoStream();
		//
		virtual void				sndLiberarFuente(const UI16 idFuenteSonidoAL); //para ser notificado por el NBGestorSonidos que la fuenteSonidoAL ya no me pertence
		virtual void				sndStreamBufferQuitadoDeCola(const UI16 cantBufferQuitados); //para ser notificado por el NBGestorSonidos que debe continuarse con el llenado de stream
		//
		AUEscenaObjeto*				objetoEscena();
		AUSonidoStream*				streamSonido();
		virtual UI16				fuenteAL();
		virtual NBSndOrigenDatos	fuenteDatosSonido();
		virtual bool				repetir();
		virtual float				volumen();
		virtual float				offsetSegundos();
		UI32						indiceMuestraActual();
		void						establecerObjetoEscena(AUEscenaObjeto* objetoEscena);
		virtual void				establecerRepetir(bool repetir);
		virtual void				establecerVolumen(float volumen);
		virtual void				establecerOffSetSegundos(float segundos);
		//
		virtual bool				reproduciendoSonido();
		virtual void				reproducirSonido();
		virtual void				pausarSonido();
		virtual void				detenerSonido();
		virtual void				vaciarCola();
		virtual void				rebobinarSonido();
		//guardar y cargar en XML
		static bool					agregarXmlInternoEn(AUEscenaSonidoStream* sonido, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool					interpretarXmlInterno(AUEscenaSonidoStream* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool					agregarBitsInternosEn(AUEscenaSonidoStream* sonido, AUArchivo* guardarEn);
		static bool					interpretarBitsInternos(AUEscenaSonidoStream* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		bool						_repetir;
		bool						_reproduciendoSonido;
		UI8							_grupoAudio;
		UI16						_fuenteOpenAL;
		AUEscenaObjeto*				_objetoEscena;
		AUSonidoStream*				_streamSonido;
		UI32						_indiceMuestraActual;
		float						_volumen;
		float						_segundosMinimoEnFila;
		UI16						_cantBuffersPorFila;
		UI16						_cantBuffersLlenos;
		AUArregloNativoMutableP<UI16> _bufferes;
		//Optimizacion
		UI8*						_bufferTemporalDatosSonido;
		UI32						_bytesBufferTemporalDatosSonido;
	private:
		inline void					privInlineInicializar(ENAudioGrupo grupoAudio, AUEscenaObjeto* objetoEscena, AUSonidoStream* streamSonido, float segundosMinimoEnFila);
		inline void					privInlineAsegurarFuenteAL();
		void						privLlenarCola();
};

#endif
