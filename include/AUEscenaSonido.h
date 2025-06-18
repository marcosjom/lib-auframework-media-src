//
//  AUEscenaSonido.h
//  AUFramework
//
//  Created by Nicaragua Binary on 21/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#if !defined(AUESCENASONIDO_H_INCLUIDO) && !defined(CONFIG_NB_UNSUPPORT_AUDIO_IO)
#define AUESCENASONIDO_H_INCLUIDO

#include "AUObjeto.h"
#include "AUEscenaObjeto.h"
#include "NBGestorSonidos.h"
#include "AUEscenaSnd.h"

class AUEscenaSonido : public AUEscenaSnd, public IUsuarioFuentesAL {
	public:
		AUEscenaSonido(ENAudioGrupo grupoAudio);
		AUEscenaSonido(ENAudioGrupo grupoAudio, AUEscenaObjeto* objetoEscena, const UI16 idBufferSonido);
		AUEscenaSonido(AUEscenaSonido* otra);
		virtual ~AUEscenaSonido();
		//
		virtual void				sndLiberarFuente(const UI16 idFuenteSonidoAL); //para ser notificado por el NBGestorSonidos que la fuenteSonidoAL ya no me pertence
		virtual void				sndStreamBufferQuitadoDeCola(const UI16 cantBufferQuitados); //para ser notificado por el NBGestorSonidos que debe continuarse con el llenado de stream
		//
		AUEscenaObjeto*				objetoEscena();
		UI16						bufferAL();
		virtual UI16				fuenteAL();
		virtual NBSndOrigenDatos	fuenteDatosSonido();
		virtual bool				repetir();
		virtual float				volumen();
		virtual float				offsetSegundos();
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
		static bool					agregarXmlInternoEn(AUEscenaSonido* sonido, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool					interpretarXmlInterno(AUEscenaSonido* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool					agregarBitsInternosEn(AUEscenaSonido* sonido, AUArchivo* guardarEn);
		static bool					interpretarBitsInternos(AUEscenaSonido* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		bool						_repetir;
		UI8							_grupoAudio;
		float						_volumen;
		UI16						_fuenteOpenAL;
		UI16						_bufferAL;
		AUEscenaObjeto*				_objetoEscena;
		//
		//NBPunto					_ultimaPosicionEscenaObjeto;
	private:
		inline void					privInlineInicializar(ENAudioGrupo grupoAudio, AUEscenaObjeto* objetoEscena, const UI16 bufferAL, bool repetir, float volumen);
		inline void					privInlineAsegurarFuenteAL();
};

#endif