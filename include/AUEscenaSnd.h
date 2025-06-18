//
//  AUEscenaSnd.h
//  AUFramework
//
//  Created by Nicaragua Binary on 28/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#if !defined(AUESCENASND_H_INCLUIDO) && !defined(CONFIG_NB_UNSUPPORT_AUDIO_IO)
#define AUESCENASND_H_INCLUIDO

#include "AUEscenaObjeto.h"

enum ENSndFuente {
	ENSndFuente_ninguno = 0,
	ENSndFuente_buffer,
	ENSndFuente_stream
};

struct NBSndOrigenDatos {
	ENSndFuente	tipo;
	//
	UI16		indice;		//Si es tipo bufer
	void*		puntero;	//Si es tipo stream
};

class AUEscenaSnd : public AUEscenaObjeto {
	public:
		AUEscenaSnd();
		AUEscenaSnd(AUEscenaSnd* otraInstancia);
		virtual ~AUEscenaSnd();
		//
		bool					iniciarReproduciendo();
		bool					esEspacial();
		void					establecerIniciarReproduciendo(bool iniciarReproduciendo);
		void					establecerEsEspacial(bool esEspacial);
		//
		virtual UI16			fuenteAL() = 0;
		virtual NBSndOrigenDatos fuenteDatosSonido() = 0;
		virtual bool			repetir() = 0;
		virtual float			volumen() = 0;
		virtual float			offsetSegundos() = 0;
		virtual void			establecerRepetir(bool repetir) = 0;
		virtual void			establecerVolumen(float volumen) = 0;
		virtual void			establecerOffSetSegundos(float segundos) = 0;
		//
		virtual bool			reproduciendoSonido() = 0;
		virtual void			reproducirSonido() = 0;
		virtual void			pausarSonido() = 0;
		virtual void			detenerSonido() = 0;
		virtual void			vaciarCola() = 0;
		virtual void			rebobinarSonido() = 0;
		//
		virtual	NBCajaAABB		cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL	actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		//guardar y cargar en XML
		static bool				agregarXmlInternoEn(AUEscenaSnd* sonido, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool				interpretarXmlInterno(AUEscenaSnd* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool				agregarBitsInternosEn(AUEscenaSnd* sonido, AUArchivo* guardarEn);
		static bool				interpretarBitsInternos(AUEscenaSnd* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		bool					_iniciarReproduciendo;
		bool					_esEspacial;
		//
		virtual void			agregadoEnEscena();
		virtual void			quitandoDeEscena();
};

#endif
