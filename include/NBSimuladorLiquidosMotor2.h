//
//  NBSimuladorLiquidosMotor2.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 05/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Gameplay_Mac_NBSimuladorLiquidosMotor2_h
#define Gameplay_Mac_NBSimuladorLiquidosMotor2_h

#include "NBRectangulo.h"
#include "AUArregloNativoMutable.h"
#include "NBSimuladorLiquidosMotorI.h"

#define IX(i,j) ((i)+(N+2)*(j))
#define SWAP(x0,x) {float *tmp=x0;x0=x;x=tmp;}

struct STLiquidoGotaDensidad {
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
	bool operator==(const STLiquidoGotaDensidad &otro) const {
		return (registroOcupado == otro.registroOcupado);
	}
	bool operator!=(const STLiquidoGotaDensidad &otro) const {
		return !(registroOcupado == otro.registroOcupado);
	}
};

class NBSimuladorLiquidosMotor2: public NBSimuladorLiquidosMotorI {
	public:
		NBSimuladorLiquidosMotor2(NBSimuladorLiquidosMotorEscuchadorI* escuchador, float x, float y, UI16 columnas, UI16 filas, float dimesionesCajas);
		virtual ~NBSimuladorLiquidosMotor2();
		//
		virtual NBPunto		posicionGota(UI16 indice);
		virtual void		gotaAgregada(UI16 indice, float xPos, float yPos, float xVel, float yVel, float radioLiquido, float radioFisico, float biscosidad);
		virtual void		gotaQuitada(UI16 indice);
		virtual void		simularFuerzasLiquidos(float segundosTranscurridos);
		NBRectangulo		areaGrillaDensidades();
		SI32				dimGrillaDensidad();
		float				densidadEnCelda(SI32 x, SI32 y);
	private:
		UI16				_filas;
		UI16				_columnas;
		UI16				_conteoRegistrosGotasRevisar;			//Determina el tamano del bloque izquierda en el que estan todos los registros ocupados
		float				_dimensionesCajasLiq;					//En coordenadas de simulador de liq
		NBRectangulo		_areaCajasLiq;							//En coordenadas de simulador de liq
		AUArregloNativoMutableP<STLiquidoGotaDensidad>*	_gotas;		//Punteros de las gotas
		NBSimuladorLiquidosMotorEscuchadorI* _escuchador;
		//
		SI32				_tamGrillaDensidad;
		float*				_u;			//fuerza
		float*				_v;			//velocidad
		float*				_dens;		//densidad
		float*				_u_prev;	//fuerza previa
		float*				_v_prev;	//velocidad previa
		float*				_dens_prev;	//densidad previa
		//
		void				add_source ( int N, float * x, float * s, float dt );
		void				diffuse ( int N, int b, float * x, float * x0, float diff, float dt );
		void				advect ( int N, int b, float * d, float * d0, float * u, float * v, float dt );
		void				dens_step ( int N, float * x, float * x0,  float * u, float * v, float diff, float dt );
		void				vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt );
		void				project(int N, float* u, float* v, float* p, float* div);
		void				set_bnd ( int N, int b, float * x );
};

#endif
