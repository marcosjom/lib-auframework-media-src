//
//  AUEscenaMallaBezierAnimada.cpp
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 24/01/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaMallaBezier.h"

AUEscenaMallaBezier::AUEscenaMallaBezier(UI8 columnas, UI8 filas, AUArregloNativoP<NBPuntoColor>* verticesLocales, AUArregloNativoP<AUTextura*>* texturas, AUArregloNativoP<NBRectangulo>* areasTex) : AUEscenaMallaConHueso(columnas, filas, verticesLocales, texturas, areasTex) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaBezier::AUEscenaMallaBezier")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMallaBezier")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaMallaBezier::AUEscenaMallaBezier(AUEscenaMallaBezier* otraInstancia) : AUEscenaMallaConHueso(otraInstancia) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaBezier::AUEscenaMallaBezier")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMallaBezier")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaMallaBezier::~AUEscenaMallaBezier(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaBezier::~AUEscenaMallaBezier")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

void AUEscenaMallaBezier::moverArticulacionesHaciaBezierCuadratica(const float xIniCurva, const float yIniCurva, const float xRefCurva, const float yRefCurva, const float xFinCurva, const float yFinCurva){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaBezier::moverArticulacionesHaciaBezierCuadratica")
	float iArticulacion, conteoArticulaciones = this->datosHuesos()->conteo;
	float t, unoMenosT, ta, tb, tc;
	if(conteoArticulaciones>1){
		conteoArticulaciones--; //Para evitar restar 1 constantemente
		for(iArticulacion=0.0f; iArticulacion<=conteoArticulaciones; iArticulacion++){
			t			= iArticulacion / conteoArticulaciones;
			unoMenosT	= 1.0f - t;
			ta			= unoMenosT * unoMenosT;
			tb			= 2.0f * unoMenosT * t;
			tc			= t * t;
			this->moverArticulacion(iArticulacion
									, (ta * xIniCurva) + (tb * xRefCurva) + (tc * xFinCurva)
									, (ta * yIniCurva) + (tb * yRefCurva) + (tc * yFinCurva));
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaBezier::moverArticulacionesHaciaBezierCubica(const float xIniCurva, const float yIniCurva, const float xRefIniCurva, float yRefIniCurva, const float xRefFinCurva, float yRefFinCurva, const float xFinCurva, const float yFinCurva){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaBezier::moverArticulacionesHaciaBezierCubica")
	float iArticulacion, conteoArticulaciones = this->datosHuesos()->conteo;
	float t, unoMenosT, ta, tb, tc, td;
	if(conteoArticulaciones>1){
		conteoArticulaciones--; //Para evitar restar 1 constantemente
		for(iArticulacion=0.0f; iArticulacion<=conteoArticulaciones; iArticulacion++){
			t			= iArticulacion / conteoArticulaciones;
			unoMenosT	= 1.0f - t;
			ta			= unoMenosT * unoMenosT * unoMenosT;
			tb			= 3.0f * t * unoMenosT * unoMenosT;
			tc			= 3.0f * t * t * unoMenosT;
			td			= t * t * t;
			this->moverArticulacion(iArticulacion
									, (ta * xIniCurva) + (tb * xRefIniCurva) + (tc * xRefFinCurva) + (td * xFinCurva)
									, (ta * yIniCurva) + (tb * yRefIniCurva) + (tc * yRefFinCurva) + (td * yFinCurva));
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

AUEscenaMallaBezier* AUEscenaMallaBezier::crearMallaBezierSprite(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaSprite, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaBezier::crearMallaBezierSprite")
	AUArregloNativoMutableP<NBPuntoColor>* vertices	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBPuntoColor>();
	AUArregloNativoMutableP<AUTextura*>* texturas	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<AUTextura*>();
	AUArregloNativoMutableP<NBRectangulo>* areasTex	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBRectangulo>();
	NB_DEFINE_NOMBRE_PUNTERO(vertices, "AUEscenaMalla::vertices");
	NB_DEFINE_NOMBRE_PUNTERO(texturas, "AUEscenaMalla::texturas");
	NB_DEFINE_NOMBRE_PUNTERO(areasTex, "AUEscenaMalla::areasTex");

	//
	AUEscenaMalla::llenarParametrosMallaSprite(texturaSprite, columnas, filas, vertices, texturas, areasTex, alineacionH, alineacionV);
	AUEscenaMallaBezier* nuevaMalla = new(tipoMemoriaResidencia) AUEscenaMallaBezier(columnas, filas, vertices, texturas, areasTex);
	nuevaMalla->autoLiberar(NB_RETENEDOR_NULL);
	//
	vertices->liberar(NB_RETENEDOR_NULL);
	texturas->liberar(NB_RETENEDOR_NULL);
	areasTex->liberar(NB_RETENEDOR_NULL);
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return nuevaMalla;
}

AUEscenaMallaBezier* AUEscenaMallaBezier::crearMallaBezierPatron(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaRepetir, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaBezier::crearMallaBezierPatron")
	AUArregloNativoMutableP<NBPuntoColor>* vertices	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBPuntoColor>();
	AUArregloNativoMutableP<AUTextura*>* texturas	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<AUTextura*>();
	AUArregloNativoMutableP<NBRectangulo>* areasTex	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBRectangulo>();
	NB_DEFINE_NOMBRE_PUNTERO(vertices, "AUEscenaMalla::vertices");
	NB_DEFINE_NOMBRE_PUNTERO(texturas, "AUEscenaMalla::texturas");
	NB_DEFINE_NOMBRE_PUNTERO(areasTex, "AUEscenaMalla::areasTex");
	//
	AUEscenaMalla::llenarParametrosMallaPatron(texturaRepetir, columnas, filas, vertices, texturas, areasTex, alineacionH, alineacionV);
	AUEscenaMallaBezier* nuevaMalla = new(tipoMemoriaResidencia) AUEscenaMallaBezier(columnas, filas, vertices, texturas, areasTex);
	nuevaMalla->autoLiberar(NB_RETENEDOR_NULL);
	//
	vertices->liberar(NB_RETENEDOR_NULL);
	texturas->liberar(NB_RETENEDOR_NULL);
	areasTex->liberar(NB_RETENEDOR_NULL);
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return nuevaMalla;
}

AUEscenaMallaBezier* AUEscenaMallaBezier::crearMallaBezierDesdeArchivoXml(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaArchivoMalla, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaBezier::crearMallaBezierDesdeArchivoXml")
	AUArregloNativoMutableP<NBPuntoColor>* vertices	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBPuntoColor>();
	AUArregloNativoMutableP<AUTextura*>* texturas	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<AUTextura*>();
	AUArregloNativoMutableP<NBRectangulo>* areasTex	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBRectangulo>();
	NB_DEFINE_NOMBRE_PUNTERO(vertices, "AUEscenaMalla::vertices");
	NB_DEFINE_NOMBRE_PUNTERO(texturas, "AUEscenaMalla::texturas");
	NB_DEFINE_NOMBRE_PUNTERO(areasTex, "AUEscenaMalla::areasTex");
	//
	UI8 columnas, filas;
	AUEscenaMalla::llenarParametrosMallaDesdeArchivoXML(rutaArchivoMalla, &columnas, &filas, vertices, texturas, areasTex, alineacionH, alineacionV);
	AUEscenaMallaBezier* nuevaMalla = new(tipoMemoriaResidencia) AUEscenaMallaBezier(columnas, filas, vertices, texturas, areasTex);
	nuevaMalla->autoLiberar(NB_RETENEDOR_NULL);
	//
	vertices->liberar(NB_RETENEDOR_NULL);
	texturas->liberar(NB_RETENEDOR_NULL);
	areasTex->liberar(NB_RETENEDOR_NULL);
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return nuevaMalla;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaMallaBezier, AUEscenaMallaConHueso)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaMallaBezier, "AUEscenaMallaBezier", AUEscenaMallaConHueso)
AUOBJMETODOS_CLONAR_THIS(AUEscenaMallaBezier)






