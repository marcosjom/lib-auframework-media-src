//
//  AUBasura.h
//  Gameplay_iOS
//
//  Created by Marcos Ortega Morales on 27/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaRutaAnimador_H_INCLUIDO
#define AUEscenaRutaAnimador_H_INCLUIDO

#include "AUEscenaRuta.h"
#include "NBAnimador.h"
#include "NBGestorAnimadores.h"
#include "AUEscenaContenedor.h"

#define AUESCENARUTAANIMADOR_INVERTIR_BIT_EN_MASCARA(VAR_MASCARA, BIT_INVERTIR) if((VAR_MASCARA & BIT_INVERTIR)!=0) VAR_MASCARA &= ~BIT_INVERTIR; else VAR_MASCARA |= BIT_INVERTIR;
//
//#define AUESCENARUTAANIMADOR_BIT_ANIMANDO_RUTA			1
//#define AUESCENARUTAANIMADOR_BIT_INICIAR_ANIMANDO_RUTA	2
//
#define AUESCENARUTAANIMADOR_BIT_ANIMANDO_ORDEN_INVERSO		1
#define AUESCENARUTAANIMADOR_BIT_ANIMANDO_SEGMNT_CICLO		2	//Animando el segmento que va del vIni al vFin de la ruta (o viceversa)
#define AUESCENARUTAANIMADOR_BIT_ANIMANDO_CICLO_HACIA_FIN	4	//La animacion de cilo se dirige hacia vertice final (caso contrario se derige al primer vertice)

enum ENRutaAnimModo {
	ENRutaAnimModo_Ida = 0,
	ENRutaAnimModo_IdaVuelta,
	ENRutaAnimModo_Ciclo,
	ENRutaAnimModo_Conteo
};

enum ENRutaAnimModoTick {
	ENRutaAnimModoTick_Animando = 0,
	ENRutaAnimModoTick_Desapareciendo,
	ENRutaAnimModoTick_Apareciendo
};

enum ENRutaAnimAvance {
	ENRutaAnimAvance_Ninguno = 0,
	ENRutaAnimAvance_Directo,
	ENRutaAnimAvance_Reverso
};

struct STRutaAnimEstado {
	public:
		STRutaAnimEstado(){
			animandoRuta				= false;
			repetirRuta					= false;
			NBPUNTO_ESTABLECER(posLocalEnAnimacion, 0.0f, 0.0f)
			velocidadRecorreRuta		= 128.0f;
			modoRutaAnimacion			= ENRutaAnimModo_IdaVuelta;
			mascPropsAnimacion			= 0;
			indiceSegmentoActual		= 0;
			avanceSobreSementoActual	= 0.0f;
			avanceSobreRutaTotal		= 0.0f;
			segsDuraSegmentoActual		= 0.0f;
			segsAcumEnSegmentoActual	= 0.0f;
			segsEsperaPendientes		= 0.0f;
			//
			consumirMovimientoBrusco	= false;
		}
		NBPunto					posLocalEnAnimacion;
		float					velocidadRecorreRuta;
		//Repetir pendiente
		bool					consumirMovimientoBrusco;	//Cuando se traslada al objeto desde el ultimo punto al primer punto (o viceversa) para repetir la animacion. Esta variable sirve para prevenir movimientos bruscos de la fisica de los cuerpos kinematicos.
		//
		bool					animandoRuta;
		bool					repetirRuta;
		UI8						modoRutaAnimacion;
		UI8						mascPropsAnimacion;
		UI8						indiceSegmentoActual;
		float					avanceSobreSementoActual;
		float					avanceSobreRutaTotal;
		float					segsDuraSegmentoActual;		//Segundos que dura el recorrido del segmento actual (con base alos parametros)
		float					segsAcumEnSegmentoActual;	//Segundos acumulados en el segmento actual
		float					segsEsperaPendientes;
		//Animacion de ciclo
		NBPunto					cicloPosIni;
		NBPunto					cicloVectorUnit;
		float					cicloLongitudTotal;
		float					cicloLongitudRecorrida;
		float					cicloVelActual;
		float					cicloVelFinal;
};

class AUEscenaRutaAnimador: public AUEscenaRuta, public NBAnimador {
	public:
		AUEscenaRutaAnimador();
		AUEscenaRutaAnimador(AUEscenaRutaAnimador* otra);
		virtual ~AUEscenaRutaAnimador();
		//
		bool					animandoRuta();
		bool					iniciarAnimandoRuta();
		bool					repetirRuta();
		float					posicionRelativaInicial();
		float					velRecorreRuta();
		ENRutaAnimModo			modoRutaAnimacion();
		AUEscenaObjeto*			objAnimandoEnRuta();
		//
		void					actualizarObjetosSegunCopia(AUEscenaRutaAnimador* objOriginal, AUArreglo* capaObjetosViejos, AUArreglo* capaObjetosNuevos);
		//
		void					moverHaciaAvanceRelativo(float avanceRelativo);
		void					moverHaciaAvanceRelativo(float avanceRelativo, STRutaAnimEstado &estadoAnim);
		void					establecerPosicionRelativaInicial(float posicionRelativaInicial);
		void					establecerVelRecorreRuta(float velRecorreRuta);
		void					establecerModoRutaAnimacion(ENRutaAnimModo modoRutaAnim);
		void					reanudarAnimacionRuta();
		void					detenerAnimacionRuta();
		void					establecerIniciarAnimandoRuta(bool iniciarAnimandoRuta);
		void					establecerRepetirRuta(bool repetirRuta);
		void					establecerObjAnimarEnRuta(AUEscenaObjeto* objAnimarEnRuta);
		//
		virtual void			tickAnimacion(float segsTranscurridos);
		void					tickAnimacionRuta(float segsTranscurridos, STRutaAnimEstado &estadoAnim);
		//
		//guardar y cargar en XML
		static bool				agregarXmlInternoEn(AUEscenaRutaAnimador* rutaAnim, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloObjetosParaID);
		static bool				interpretarXmlInterno(AUEscenaRutaAnimador* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos, AUArregloNativoP<SI32>* arregloIDs);
		//guardar y cargar en binario
		static bool				agregarBitsInternosEn(AUEscenaRutaAnimador* rutaAnim, AUArchivo* guardarEn, AUArreglo* arregloObjetosParaID);
		static bool				interpretarBitsInternos(AUEscenaRutaAnimador* cargarEn, AUArchivo* cargarDe, AUArreglo* arregloObjetosParaID);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		bool					_iniciarAnimandoRuta;
		UI8						_modoTickAnimacionRuta;
		float					_posicionRelInicial;
		AUEscenaObjeto*			_objAnimandoEnRuta;
		STRutaAnimEstado		_estadoAnimacion;
		//Para repetir ciclo de forma suave
		float					_segsTomaDesaparecer;
		float					_segsTomaReaparecer;
		float					_segsAcumDesaparecer;
		float					_segsAcumReaparecer;
		//
		virtual void			agregadoEnEscena();
		virtual void			agregadoEnEscenaConHijosNotificados();
		virtual void			quitandoDeEscena();
		// Formulas de la cinematica
		// Movimiento rectilineo uniforme
		//	x1 = x0 + v * t;
		// Movimiento rectilineo uniformemente acelerado
		//	x1 = x0 + (v0 * t) + (1/2 * a * t * t)
		//	v1 = v0 + a * t
		//	v1 * v1 = v0 * v0 + 2 * a * (x1 - x0)
		inline float			privInlineSegundosDuracionSegmento(const float longitudVector, const float velInicial, const float velFinal){
			float segundos = 0.0f;
			if(velInicial==velFinal){
				if(velInicial!=0.0f) segundos = longitudVector / velInicial;
			} else if(longitudVector!=0.0f){
				segundos			= (velFinal - velInicial) / ((velFinal * velFinal - velInicial * velInicial) / (2.0f * longitudVector)); //aceleracion = (velFinal * velFinal - velInicial * velInicial) / (2.0f * longitudVector)
			}
			NBASSERT(segundos==segundos) //Si falla es Nan
			return segundos;
		}
		inline float			privInlineAvanceEnSegmentoSegunTiempo(const float longitudVector, const float velInicial, const float velFinal, const float segsAcumulados){
			float avance			= 0.0f;
			if(longitudVector!=0.0f) avance = velInicial * segsAcumulados + (((velFinal * velFinal - velInicial * velInicial) / (2.0f * longitudVector)) * segsAcumulados * segsAcumulados * 0.5f); //aceleracion = (velFinal * velFinal - velInicial * velInicial) / (2.0f * longitudVector)
			NBASSERT(avance==avance) //Si falla es Nan
			return avance;
		}
};

#endif