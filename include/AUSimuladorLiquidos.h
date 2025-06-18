//
//  AUSimuladorLiquidos.h
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#if !defined(AU_SIMULADORLIQUIDOS_H_INCLUIDO) && !defined(CONFIG_NB_UNSUPPORT_BOX2D)
#define AU_SIMULADORLIQUIDOS_H_INCLUIDO

//temporal?
#include "NBGestorTexturas.h"
#include "AUEscenaContenedor.h"
#include "AUEscenaSprite.h"
#include "NBGestorFisica.h"

#include "AUObjeto.h"
#include "AUArregloMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUArregloNativoOrdenadoMutable.h"

#include "Box2D.h"
#include "IAnimadorFisicaAdicional.h"
#include "NBSimuladorLiquidosMotor1.h"
#include "NBSimuladorLiquidosMotor2.h"

enum ENTuberiaLiquidoTipo {
	ENTuberiaLiquidoTipo_AguaPura,
	ENTuberiaLiquidoTipo_LiqVerde,
	ENTuberiaLiquidoTipo_LiqNegro,
	ENTuberiaLiquidoTipo_Conteo
};

struct STLiquidoGota {
	bool				registroOcupado;
	//Propiedades
	UI8					tipoLiq;
	float				radioLiquido;
	//Temporales de presentacion
	NBPunto				vectEscenaEstela;	//Para la presentacion de la estela de la gota
	NBPunto				posEscenaAnterior;	//Para la presentacion de la estela de la gota
	//
	bool operator==(const STLiquidoGota &otro) const {
		return (registroOcupado == otro.registroOcupado);
	}
	bool operator!=(const STLiquidoGota &otro) const {
		return !(registroOcupado == otro.registroOcupado);
	}
};

struct STLiquidoRender {
	SI32		cantModelos;
	UI32		idTexturaGota;
	UI32		idTexturaGotaMed;
	UI32		idTexturaGotaPeq;
	STBloqueGL	bloqueIndicesGL;
};

/*
 Pruebas de rendimiento para diferente tamano de caja en la grilla:
 Ciclo a: 60FPS, 750 gotas, 0.6 radio liquido, 0.05 radio solido, 0.004 biscosidad
 tamCaja = (AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO * 2.0f): consumen 80% del tiempo, comportamiento realista (OK)
 tamCaja = (AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO / 1.0f): consumen 62% del tiempo, comportamiento realista (OK)
 tamCaja = (AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO / 1.33f): consumen 55% del tiempo, comportamiento realista (OK - IDEAL)
 tamCaja = (AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO / 1.5f): consumen 52% del tiempo, comportamiento realista (un poco tembloroso irregular)
 tamCaja = (AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO / 2.0f): consumen 48% del tiempo, comportamiento no-realista (muy tembloroso irregular)
 */

class AUSimuladorLiquidos: public AUEscenaObjeto, public IAnimadorFisicaAdicional, public NBSimuladorLiquidosMotorEscuchadorI {
	public:
		AUSimuladorLiquidos(float x, float y, float ancho, float alto, float dimesionesCajas, AUEscenaContenedor* contenedorDebug);
		virtual ~AUSimuladorLiquidos();
		//
		UI16				conteoGotas();
		UI16				creaGota(float posX, float posY, float velX, float velY, ENTuberiaLiquidoTipo tipoLiquido, float radioLiquido, float radioSolido, float biscosidad);
		//UI16				gotaEnCoordenada(float posX, float posY);
		void				eliminarGota(UI16 iGota);
		virtual void		simularFisicaCuerpos(float segundosTranscurridos);
		virtual void		gotaQuitadaDeMotorPorSalirDeLimites(UI16 indice);
		//
		NBPunto				posicionGotaEnMundoFisica(UI16 iGota);
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void			enviarComandosGL(UI8* punteroDatosModelo);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		virtual void		agregadoEnEscena();
		virtual void		quitandoDeEscena();
	private:
		UI16				_filas;
		UI16				_columnas;
		UI16				_conteoRegistrosGotasRevisar;	//Determina el tamano del bloque izquierda en el que estan todos los registros ocupados
		float				_dimensionesCajasLiq;			//En coordenadas de simulador de liq
		NBRectangulo		_areaCajasLiq;					//En coordenadas de simulador de liq
		AUTextura*			_texGota;
		AUTextura*			_texGotaMed;
		AUTextura*			_texGotaPeq;
		AUArregloNativoMutableP<STLiquidoGota>	_gotas;			//Punteros de las gotas
		AUArregloNativoMutableP<STGotaRelacion>	_tmpRelaciones;
		NBSimuladorLiquidosMotorI*	_motorSumilacion;
		//
		void				privSimularFisicaLiquidosPrecacheandoCercanas(float segundosTranscurridos);
};

#endif
