//
//  AUSimuladorLiquidosMotorP.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 05/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Gameplay_Mac_AUSimuladorLiquidosMotorI_h
#define Gameplay_Mac_AUSimuladorLiquidosMotorI_h

#include "NBPunto.h"

class NBSimuladorLiquidosMotorEscuchadorI {
	public:
		virtual ~NBSimuladorLiquidosMotorEscuchadorI(){
			//
		};
		//
		virtual void		gotaQuitadaDeMotorPorSalirDeLimites(UI16 indice) = 0;
};

class NBSimuladorLiquidosMotorI {
	public:
		virtual ~NBSimuladorLiquidosMotorI(){
			//
		};
		//
		virtual NBPunto		posicionGota(UI16 indice) = 0;
		virtual void		gotaAgregada(UI16 indice, float xPos, float yPos, float xVel, float yVel, float radioLiquido, float radioFisico, float biscosidad) = 0;
		virtual void		gotaQuitada(UI16 indice) = 0;
		virtual void		simularFuerzasLiquidos(float segundosTranscurridos) = 0;
};

#endif
