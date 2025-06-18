//
//  AUEscenaMallaBezierAnimada.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 24/01/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaMallaBezier_H_INCLUIDO
#define AUEscenaMallaBezier_H_INCLUIDO

#include "AUEscenaMallaConHueso.h"

class AUEscenaMallaBezier: public AUEscenaMallaConHueso {
	public:
		AUEscenaMallaBezier(UI8 columnas, UI8 filas, AUArregloNativoP<NBPuntoColor>* verticesLocales, AUArregloNativoP<AUTextura*>* texturas, AUArregloNativoP<NBRectangulo>* areasTex);
		AUEscenaMallaBezier(AUEscenaMallaBezier* otraInstancia);
		virtual ~AUEscenaMallaBezier();
		//
		void								moverArticulacionesHaciaBezierCuadratica(const float xIniCurva, const float yIniCurva, const float xRefCurva, float yRefCurva, const float xFinCurva, const float yFinCurva);
		void								moverArticulacionesHaciaBezierCubica(const float xIniCurva, const float yIniCurva, const float xRefIniCurva, float yRefIniCurva, const float xRefFinCurva, float yRefFinCurva, const float xFinCurva, const float yFinCurva);
		//
		static AUEscenaMallaBezier*			crearMallaBezierSprite(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaSprite, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH=ENEscenaMallaAlineacionH_Centro, ENEscenaMallaAlineacionV alineacionV=ENEscenaMallaAlineacionV_Centro);
		static AUEscenaMallaBezier*			crearMallaBezierPatron(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaRepetir, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH=ENEscenaMallaAlineacionH_Centro, ENEscenaMallaAlineacionV alineacionV=ENEscenaMallaAlineacionV_Centro);
		static AUEscenaMallaBezier*			crearMallaBezierDesdeArchivoXml(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaArchivoMalla, ENEscenaMallaAlineacionH alineacionH=ENEscenaMallaAlineacionH_Centro, ENEscenaMallaAlineacionV alineacionV=ENEscenaMallaAlineacionV_Centro);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
};

#endif
