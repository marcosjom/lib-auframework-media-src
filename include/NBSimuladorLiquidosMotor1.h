//
//  NBSimuladorLiquidosMotor1.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 05/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#if !defined(Gameplay_Mac_NBSimuladorLiquidosMotor1_h) && !defined(CONFIG_NB_UNSUPPORT_BOX2D)
#define Gameplay_Mac_NBSimuladorLiquidosMotor1_h

#include "NBSimuladorLiquidosMotorI.h"
#include "NBGestorFisica.h"
#include "AUEscenaFigura.h" //Para debug

#define AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR			(50.0f/0.6f)	//multiplicador para escalar todas las gotas antes de procesar (luego se escalan de regreso)
//#define AU_MOTOR_LIQUIDOS_1_RADIO_IDEAL 		50.0f			//50.0f en demo original. Define el multiplicador de posicion y velocidad de las gotas. El sistema es mas estable al escalar.
//#define AU_MOTOR_LIQUIDOS_1_RADIO_GOTA_LIQUIDO 	0.6f			//0.6f en demo original.
//#define AU_MOTOR_LIQUIDOS_1_RADIO_GOTA_FISICO	0.05f			//0.05f en demo original.
//#define AU_MOTOR_LIQUIDOS_1_BISCOSIDAD_GOTA		0.004f			//0.004f en demo original.

struct STGotaRelacion {
	UI16 iGotaA;
	UI16 iGotaB;
	float distanciaMutua;
	bool operator==(const STGotaRelacion &otro) const {
		return (iGotaA==otro.iGotaA && iGotaB==otro.iGotaB && distanciaMutua==otro.distanciaMutua);
	}
	bool operator!=(const STGotaRelacion &otro) const {
		return !(iGotaA==otro.iGotaA && iGotaB==otro.iGotaB && distanciaMutua==otro.distanciaMutua);
	}
};

struct STLiquidoGotaProps {
	bool				registroOcupado;
	//Propiedades
	NBPunto				posLiq;				//En coordenadas de grilla de simuladorLiq (aplicado el Multiplicador)
	NBPunto				velLiq;				//En coordenadas de grilla de simuladorLiq (aplicado el Multiplicador)
	float				radioLiquido;		//Multiplicado por AU_SIMULADOR_LIQUIDOS_MUTIPLICADOR
	float				radioSolido;
	float				biscosidad;
	//Temporales de simulacion
	NBPuntoP<SI16>		coordCelda;			//Si diferente de cero entonces el registro esta ocupado
	NBPunto				cambioLiq;			//En coordenadas de grilla de simuladorLiq (aplicado el Multiplicador)
	float				p;					//Presion acumulada
	float				pCercano;			//Presion mas cercano
	//
	bool operator==(const STLiquidoGotaProps &otro) const {
		return (registroOcupado == otro.registroOcupado);
	}
	bool operator!=(const STLiquidoGotaProps &otro) const {
		return !(registroOcupado == otro.registroOcupado);
	}
};

struct STLiquidoColision {
	UI16 iGota;
	STLiquidoGotaProps*	gotaConsultando; //COnsultando colision a Box2D
	UI16		conteoColisiones;
	float		fraccionColisionMasCercana;
	NBPunto		posInicioLiq;			//En escala de simulador de liquidos
	NBPunto		posDestinoLiq;			//En escala de simulador de liquidos
	NBPunto		posProductoColision;	//En escala de simulador de liquidos
};

struct STLiquidoDebug {
	UI16 iGota;
	AUEscenaFigura* figura;
};

class NBSimuladorLiquidosMotor1: public NBSimuladorLiquidosMotorI, public b2RayCastCallback {
	public:
		NBSimuladorLiquidosMotor1(SI8 mundoFisica, AUEscenaContenedor* contenedorDebug, NBSimuladorLiquidosMotorEscuchadorI* escuchador, float x, float y, UI16 columnas, UI16 filas, float dimesionesCajas);
		virtual ~NBSimuladorLiquidosMotor1();
		//
		virtual NBPunto		posicionGota(UI16 indice);
		virtual void		gotaAgregada(UI16 indice, float xPos, float yPos, float xVel, float yVel, float radioLiquido, float radioFisico, float biscosidad);
		virtual void		gotaQuitada(UI16 indice);
		virtual void		simularFuerzasLiquidos(float segundosTranscurridos);
		//
		virtual float32		ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);
	private:
		AUEscenaContenedor*	_contenedorInfoDebug;
		AUArregloMutable*	_debugFiguras;
		STLiquidoColision	_consultaColision;
		SI8					_mundoFisica;
		UI16				_filas;
		UI16				_columnas;
		UI16				_conteoRegistrosGotasRevisar;		//Determina el tamano del bloque izquierda en el que estan todos los registros ocupados
		float				_dimensionesCajasLiq;				//En coordenadas de simulador de liq
		NBRectangulo		_areaCajasLiq;						//En coordenadas de simulador de liq
		AUArregloNativoMutableP<STLiquidoGotaProps>*	_gotas;			//Punteros de las gotas
		AUArregloNativoMutableP<STGotaRelacion>*	_tmpRelaciones;
		NBSimuladorLiquidosMotorEscuchadorI* _escuchador;
};

#endif
