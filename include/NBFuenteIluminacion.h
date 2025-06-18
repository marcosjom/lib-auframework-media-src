//
//  NBFuenteIluminacion.h
//  AUFramework
//
//  Created by Marcos Ortega on 05/12/12.
//
//

#ifndef AUFramework_NBFuenteIluminacion_h
#define AUFramework_NBFuenteIluminacion_h

#include "AUObjeto.h"
#include "NBPunto.h"
#include "NBCajaAABB.h"
#include "NBColor.h"

enum ENFuenteIlumTipo {
	ENFuenteIlumTipo_SoloMascara = 0,		//Luz que solamente es mascara de color
	ENFuenteIlumTipo_MascaraMasBrillo,		//Luz que se aplica como mascara y como brillo al resultado
	ENFuenteIlumTipo_Conteo
};

struct NBFuenteIluminacion {
	bool			produceSombras;
	bool			limitesAngulosActivos;		//determina si la luz tiene angulos limites
	UI8				tipoIluminacion;			//ENFuenteIlumTipo
	//
	AUObjeto*		objEscena;
	AUObjeto*		objAgrupaSombras;
	AUTextura*		texturaLuz;
	float			alcanceLuzzz;				//en unidades GL
	//Si "limitesAngulosActivos"
	float			radianesMin;				//solo si "limitesAngulosActivos"
	float			radianesApertura;			//solo si "limitesAngulosActivos" (debe ser menor o igual a PI para que la validacion pos segmento luz funcione)
	//
	bool operator==(const NBFuenteIluminacion &otro) const {
		return (objEscena==otro.objEscena);
	}
	bool operator!=(const NBFuenteIluminacion &otro) const {
		return !(objEscena==otro.objEscena);
	}
};

//Datos actualizados antes de enviar a consumidores
struct NBFuenteIluminacionRender {
	bool			produceSombras;
	bool			obstruida;					//determina si esta obstruida por un objeto que produce sombra encima de ella
	bool			limitesAngulosActivos;		//determina si la luz tiene angulos limites
	bool			limitesFormaActivos;		//determina si la luz tiene una segmento limite
	bool			cajaFiguraLuzEsContrareloj;
	UI8				tipoIluminacion;			//ENFuenteIlumTipo
	//
	AUObjeto*		objetoOrigenLuz;			//Objeto que origina la luz (no se aseguraque este objeto exista)
	AUObjeto*		objetoOrigenRefleja;		//Si es una luz reflejada, entonces este es el puntero del objeto espejo. (no se aseguraque este objeto exista)
	AUObjeto*		objetoAgrupaSombras;		//Objeto que apgrupa las sombras (si concuerda se omiten sombras para esta luz) (no se aseguraque este objeto exista)
	AUTextura*		objetoTexturaLuz;			//Textura que proyecta la luz (mascara, intensidad)
	//
	NBPunto			posicionEscenaLuz;
	NBCajaAABB		cajaEscenaLuz;
	NBPunto			cajaFiguraLuzLocal[4];		//Relativos a posicion de luz
	NBColor			colorLuzF;
	//Para determinar si un puntoEscena esta dentro de la luz
	NBPunto			posLocalVectorAvanceX;
	NBPunto			posLocalVectorAvanceY;
	NBPunto			vecUnitAvanceX;
	NBPunto			vecUnitAvanceY;
	float			maxAvanceX;
	float			maxAvanceY;
	//si "limitesAngulosActivos"
	NBPunto			vecUnitAnguloMinDesdeLuz;	//vector unitario centroLuz->anguloMin (para determinar si un punto esta dentro o no de la luz)
	NBPunto			vecUnitAnguloMaxDesdeLuz;	//vector unitario centroLuz->anguloMax (para determinar si un punto esta dentro o no de la luz)
	//si "limitesFormaActivos" (el vectorLimite siempre tiene a su izquierda a la luz)
	NBPunto			posIniVecLimiteForma;		//Posicion de inicio del vector limite de forma de luz
	NBPunto			posFinVecLimiteForma;		//Posicion de finl del vector limite de forma de luz
	NBPunto			vecUnitLimiteForma;			//Vector unitario que determina el limite de forma de la luz (lo que esta a la izquierda esta iluminado, a la derecha no)
	//
	bool operator==(const NBFuenteIluminacionRender &otro) const {
		return (cajaEscenaLuz==otro.cajaEscenaLuz);
	}
	bool operator!=(const NBFuenteIluminacionRender &otro) const {
		return !(cajaEscenaLuz==otro.cajaEscenaLuz);
	}
};
		
#endif
