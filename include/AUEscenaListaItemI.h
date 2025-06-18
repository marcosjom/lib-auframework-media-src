//
//  AUGpMenuFinPartida.h
//  game-Refranero
//
//  Created by Marcos Ortega on 14/03/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#ifndef App_AUEscenaListaItemI_h
#define App_AUEscenaListaItemI_h

#include "AUEscenaObjeto.h"
#include "AUAnimadorObjetoEscena.h"

enum ENListaItemMargen {
	ENListaItemMargen_Izq = 0,
	ENListaItemMargen_Der,
	ENListaItemMargen_Arr,
	ENListaItemMargen_Aba,
	ENListaItemMargen_Conteo
};

struct STListaItemDatos {
	SI32 tipo;
	SI32 valor;
};

class AUEscenaListaItemI {
	public:
		virtual ~AUEscenaListaItemI(){
			//
		}
		//
		virtual AUEscenaObjeto*		itemObjetoEscena() = 0;
		virtual void				organizarContenido(const float anchoParaContenido, const float altoVisible, const bool notificarCambioAltura, AUAnimadorObjetoEscena* animator, const float secsAnim) = 0;
		//
		virtual STListaItemDatos	refDatos() = 0;
		virtual void				establecerRefDatos(const SI32 tipo, const SI32 valor) = 0;
};

#endif
