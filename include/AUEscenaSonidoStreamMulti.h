//
//  AUEscenaSonidoStreamMulti.h
//  AUFramework
//
//  Created by Nicaragua Binary on 01/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#if !defined(AUEscenaSonidoStreamMulti_H_INCLUIDO) && !defined(CONFIG_NB_UNSUPPORT_AUDIO_IO)
#define AUEscenaSonidoStreamMulti_H_INCLUIDO

#include "AUObjeto.h"
#include "AUEscenaObjeto.h"
#include "NBGestorSonidos.h"
#include "NBAnimador.h"
#include "NBGestorAnimadores.h"
#include "AUEscenaSnd.h"
#include "AUSonidoStream.h"
#include "AUArregloNativoMutableP.h"

struct STStreamMultiPorcion {
	AUSonidoStream*		streamSonido;		//stream de la pocion
	SI16				vecesRepetir;		//veces que se debe repetir la porcion
	//
	bool operator==(const STStreamMultiPorcion &otro) const {
		return (streamSonido==otro.streamSonido);
	}
	bool operator!=(const STStreamMultiPorcion &otro) const {
		return !(streamSonido==otro.streamSonido);
	}
};
	
class AUEscenaSonidoStreamMulti : public AUEscenaSnd, public IUsuarioFuentesAL {
	public:
		AUEscenaSonidoStreamMulti(ENAudioGrupo grupoAudio, float segundosMinimoEnFila);
		AUEscenaSonidoStreamMulti(ENAudioGrupo grupoAudio, AUEscenaObjeto* objetoEscena, float segundosMinimoEnFila);
		AUEscenaSonidoStreamMulti(AUEscenaSonidoStreamMulti* otra);
		virtual ~AUEscenaSonidoStreamMulti();
		//
		SI32						indicePorcionActual();
		bool						agregarPorcionStream(AUSonidoStream* streamSonido, const SI16 vecesRepetir); //vecesRepetir -1 para infinito
		bool						moverHaciaPorcion(const UI16 iPorcion);
		//
		virtual void				sndLiberarFuente(const UI16 idFuenteSonidoAL);		//para ser notificado por el NBGestorSonidos que la fuenteSonidoAL ya no me pertence
		virtual void				sndStreamBufferQuitadoDeCola(const UI16 cantBufferQuitados);	//para ser notificado por el NBGestorSonidos que debe continuarse con el llenado de stream
		//
		AUEscenaObjeto*				objetoEscena();
		//AUSonidoStream*			streamSonido();
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
		static bool					agregarXmlInternoEn(AUEscenaSonidoStreamMulti* sonido, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool					interpretarXmlInterno(AUEscenaSonidoStreamMulti* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool					agregarBitsInternosEn(AUEscenaSonidoStreamMulti* sonido, AUArchivo* guardarEn);
		static bool					interpretarBitsInternos(AUEscenaSonidoStreamMulti* cargarEn, AUArchivo* cargarDe);
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
		float						_volumen;
		float						_segundosMinimoEnFila;
		UI16						_cantBuffersPorFila;
		AUArregloNativoMutableP<UI16> _bufferes;
		//
		SI32						_iPorcionActual;
		SI32						_iMuestraActual;
		SI32						_vecesRepetidaMuestraActual;
		AUArregloNativoMutableP<STStreamMultiPorcion> _porcionesStream;
		//Optimizacion
		UI8*						_bufferTemporalDatosSonido;
		UI32						_bytesBufferTemporalDatosSonido;
	private:
		inline void					privInlineInicializar(ENAudioGrupo grupoAudio, AUEscenaObjeto* objetoEscena, float segundosMinimoEnFila);
		inline void					privInlineAsegurarFuenteAL();
		void						privLlenarCola();
}; 
	
#endif
