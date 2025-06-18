//
//  AUEscenaAnimacion.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 11/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUESCENAANIMACION_H_INCLUIDO
#define AUESCENAANIMACION_H_INCLUIDO

#include "AUEscenaAnimacionI.h"
#include "AUEscenaCuerpoI.h"

class AUEscenaAnimacion : public AUEscenaAnimacionI {
	public:
		AUEscenaAnimacion(AUAnimacionBibliotecaDef* plantillaBiblioteca, AUAnimacionDef* plantillaAnimacion);
		AUEscenaAnimacion(AUEscenaAnimacion* otraInstancia);
		virtual ~AUEscenaAnimacion();
		//
		virtual void                                establecerGrupoAnimacion(ENGestorAnimadoresGrupo grupoAnimacion);
		//
		virtual void*								cuerpo(); //b2Body* //generalmente el primer cuerpo en la animacion
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		void										privInicializarHijosHeredados(AUArreglo* hijosCapaLocal, AUArreglo* hijosCapaOriginal, const ENAnimacion_TipoEncapsulamiento tipoEncapsulamiento);
};


#endif
