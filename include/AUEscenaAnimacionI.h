//
//  AUEscenaAnimacionI.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 11/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUESCENAANIMACIONI_H_INCLUIDO
#define AUESCENAANIMACIONI_H_INCLUIDO

//#ifndef CONFIG_NB_UNSUPPORT_BOX2D
//#include "b2Body.h"
//#endif

#include "NBAnimador.h"
#include "NBGestorAnimadores.h"
#include "NBGestorTexturas.h"
#include "AUAnimacionDef.h"
#include "AUEscenaSprite.h"
#include "AUEscenaContenedor.h"
#include "AUEscenaSnd.h"
#include "AUEscenaSonido.h"
#include "AUEscenaSonidoStream.h"
#include "AUAnimadorSonidoVolumen.h"
#include "AUArregloMutable.h"

#define AUESCENAANIMACIONI_BIT_ESPERANDO_PARA_REPETIR	1 //reproducir al cargar

#define AUESCENAANIMACIONI_BIT_REPRODUCIR				1 //reproducir al cargar
#define AUESCENAANIMACIONI_BIT_REPETIR					2
#define AUESCENAANIMACIONI_BIT_SUAVIZAR					4
#define AUESCENAANIMACIONI_BIT_REP_SONIDOS				8

class AUEscenaAnimacionI;

class IEscenaAnimacionEscuchador {
	public:
		virtual ~IEscenaAnimacionEscuchador(){
			//
		}
		virtual void animacionFinalizada(AUEscenaAnimacionI* animacion) = 0;
};

class AUEscenaAnimacionI : public AUEscenaContenedor, public NBAnimador {
	public:
		AUEscenaAnimacionI(AUAnimacionBibliotecaDef* plantillaBiblioteca, AUAnimacionDef* plantillaAnimacion);
		AUEscenaAnimacionI(AUEscenaAnimacionI* otraInstancia);
		virtual ~AUEscenaAnimacionI();
		//animacion
		AUAnimacionBibliotecaDef*					plantillaBiblioteca();
		AUAnimacionDef*								plantillaAnimacion();
		const char*									rutaArchivoOrigen();
		NBCajaAABB									cajaLienzoAnimacion();
		FLOAT										fps();
		UI32										frames();
		FLOAT										segundosDuracionNormal();
		AUArreglo*									indicesPropiedadesPorFrame();
		ENAnimacion_TipoEncapsulamiento				tipoEncapsulamientoHijos();
		bool										actualmenteEnUltimoFrameAnimacionSinRepetir();
		//reproduccion
		float										factorVelocidadReproduccion();
		void										establecerFactorVelocidadReproduccion(float factorVelocidad);
		float										segundosEsperarParaRepetir();
		void										establecerSegundosEsperarParaRepetir(float segundosEsperarParaRepetir);
		bool										reproducirAlCargar();
		bool										suavizandoAnimacion();
		bool										reproduciendoSonidos();
		bool										repitiendoAnimacion();
		SI32										indiceFrameInicio();
		SI32										indiceFrameActual();
		float										indiceFrameIntermedioActual();
		void										establecerReproducirAlCargar(bool reproducirAlCargar);
		void										establecerReproducirSonidos(bool reproducirSonidos);
		void										establecerRepetirAnimacion(bool repetir);
		void										establecerSuavizarAnimacion(bool suavizar);
		void										establecerFrameInicio(UI16 frameInicio);
		bool										presentarFrame(UI16 indiceFrame);
		bool										presentarFrameIntermedio(float iFrame);
		//Escucha
		void										establecerEscuchadorAnimacion(IEscenaAnimacionEscuchador* escuchador);
		IEscenaAnimacionEscuchador*					escuchadorAnimacion();
		//
		NBCajaAABB									cajaAABBLocalEncierraAnimacion();
		//
		void										rebobinarAnimacion();
		void										reanudarSubAnimaciones();
		virtual void								reanudarAnimacion();
		virtual void								detenerAnimacion();
        virtual void                                establecerGrupoAnimacion(ENGestorAnimadoresGrupo grupoAnimacion) = 0;
		virtual void								tickAnimacion(float segsTranscurridos);
		//
		virtual void*								cuerpo() = 0; //b2Body* //generalmente el primer cuerpo en la animacion
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		UI8											_mascaraProps;
		SI16										_indiceFrameInicio;
		SI16										_indiceFrameActual;
		SI16										_indiceFrameUltEventosSonidosEjecutados;
		float										_factorVelocidad;
		float										_segundosEsperarParaRepetir;
		float										_segundosAcumuladosParaRepetir;
		float										_segundosAcumuladosAnim;
		IEscenaAnimacionEscuchador*					_escuchador;
		//
		AUAnimacionBibliotecaDef*					_plantillaBiblioteca;
		AUAnimacionDef*								_plantillaAnimacion;
#		ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
		AUArregloMutable							_sonidosEnEscena;				//arreglo de "AUEscenaSnd" en reproduccion
		AUArregloMutable							_animadoresSonidosEscena;		//arreglo de "AUAnimadorSonidoVolumen" en reproduccion
#		endif
		//
		virtual void								agregadoEnEscena();
		virtual void								quitandoDeEscena();
		inline void									privInlineActualizarEstadoSubanimacionesDeFrame(UI32 indiceFrame);
		inline void									privInlineEstablecerPropiedadesDeFrame(UI32 indiceFrame);
		inline void									privInlineEstablecerPropiedadesEntreFrames(UI32 indiceFrameIni, UI32 indiceFrameFin, float factorAvance);
		inline void									privInlineProcesarEventosSonidos(UI32 indiceFrame);
		inline void									privInlineDetenerSonidos();
		inline void									privInlineLimpiarSonidosSinUso();
#		ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
		inline void									privInlineQuitarSonidoEscenaPorIndice(AUEscenaSnd* sonidoEscena, UI32 indiceSonidoEscena);
#		endif
};


#endif
