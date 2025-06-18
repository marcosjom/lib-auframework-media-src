//
//  AUEscenaIluminacion.h
//  Gameplay_iOS
//
//  Created by Marcos Ortega on 25/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUESCENAILUMINACION_H_INCLUIDO
#define AUESCENAILUMINACION_H_INCLUIDO

#include "AUCuerpoDef.h"
#include "AUEscenaFigura.h"
#include "NBGestorEscena.h"
#include "NBCajaPolar.h"

struct STIluminacionRender {
	SI32			conteoFigurasGL;
};

struct STIluminacionFiguraRender {
	UI16			indiceVerticesGL;			//Puede ser NBVerticeGL, NBVerticeGL2 o NBVerticeGL3 en dependencia de las texturas que son -1 y NOT(-1)
	UI16			conteoVerticesGL;			//Cantidad de vertices reservados
	SI16			idTextura1;					//Textura de mascara de iluminacion
	SI16			idTextura2;					//Textura de iluminacion (determina la forma)
	SI16			idTextura3;					//Textura de patron de proyeccion de sombra
	//
	bool operator==(const STIluminacionFiguraRender &otro) const {
		return (indiceVerticesGL==otro.indiceVerticesGL && conteoVerticesGL==otro.conteoVerticesGL);
	}
	bool operator!=(const STIluminacionFiguraRender &otro) const {
		return !(indiceVerticesGL==otro.indiceVerticesGL && conteoVerticesGL==otro.conteoVerticesGL);
	}
};
	
class AUEscenaIluminacion: public AUEscenaObjeto {
	public:
		AUEscenaIluminacion(AUCuerpoDef* definicionCuerpo);
		AUEscenaIluminacion(AUEscenaIluminacion* otroCuerpo);
		virtual ~AUEscenaIluminacion();
		//
		virtual	NBCajaAABB										cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL									actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void												enviarComandosGL(UI8* punteroDatosModelo);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUCuerpoDef*											_definicionCuerpo;
		//
		virtual void											agregadoEnEscena();
		virtual void											quitandoDeEscena();
		//
		static bool												privConfigurarGestorGlParaMascaras();
};

#endif
