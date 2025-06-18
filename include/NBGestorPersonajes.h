//
//  NBGestorPersonajes.h
//  DodoTown
//
//  Created by Marcos Ortega on 23/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBGESTORPERSONAJES_H_INCLUIDO
#define NBGESTORPERSONAJES_H_INCLUIDO

#include "NBCajaAABB.h"
#include "NBGestorPilaLlamadas.h"
#include "AUArregloNativoMutableP.h"

struct STPersonajeDano {			//Objeto que estara sobre/alrededor de la cabeza del Aracno
	float		factorDanoRecibido;
	NBPunto		posEscenaOrigenDano;
	//
	bool operator==(const STPersonajeDano &otro) const {
		return (factorDanoRecibido==otro.factorDanoRecibido && posEscenaOrigenDano==otro.posEscenaOrigenDano);
	}
	bool operator!=(const STPersonajeDano &otro) const {
		return !(factorDanoRecibido==otro.factorDanoRecibido && posEscenaOrigenDano==otro.posEscenaOrigenDano);
	}
};

	
class IPersonajeVictima {
	public:
		virtual ~IPersonajeVictima(){ }
		virtual NBCajaAABB	personajeCajaAABBEscenaRelevante() = 0;
};

class IPersonajeVictimario {
	public:
		virtual ~IPersonajeVictimario(){ }
		virtual bool	personajeDanoHaciaVictima(const NBCajaAABB &cajaEscenaVictima, STPersonajeDano* guardarDanoEn) = 0;
};

struct STPersonajeVictima {			//Objeto que estara sobre/alrededor de la cabeza del Aracno
	IPersonajeVictima*	objVictima;
	SI32				datoUsuario;
	//
	bool operator==(const STPersonajeVictima &otro) const {
		return (objVictima==otro.objVictima);
	}
	bool operator!=(const STPersonajeVictima &otro) const {
		return !(objVictima==otro.objVictima);
	}
};

struct STPersonajeVictimario {			//Objeto que estara sobre/alrededor de la cabeza del Aracno
		IPersonajeVictimario*	objVictimario;
		SI32					datoUsuario;
		//
		bool operator==(const STPersonajeVictimario &otro) const {
			return (objVictimario==otro.objVictimario);
		}
		bool operator!=(const STPersonajeVictimario &otro) const {
			return !(objVictimario==otro.objVictimario);
		}
};
		
class NBGestorPersonajes {
	public:
		static bool										inicializar();
		static void										finalizar();
		static bool										gestorInicializado();
		//Victimas
		static AUArregloNativoP<STPersonajeVictima>*	victimas();
		static void										agregarVictima(IPersonajeVictima* objVictima, SI32 datoUsuario);
		static void										quitarVictima(IPersonajeVictima* objVictima);
		//Victimarios
		static AUArregloNativoP<STPersonajeVictimario>*	victimarios();
		static void										agregarVictimario(IPersonajeVictimario* objVictimario, SI32 datoUsuario);
		static void										quitarVictimario(IPersonajeVictimario* objVictimario);
	private:
		static bool										_gestorInicializado;
		static AUArregloNativoMutableP<STPersonajeVictima>*		_victimas;		//quienes reciben dano
		static AUArregloNativoMutableP<STPersonajeVictimario>*	_victimarios;	//quienes realizan dano
};

#endif

