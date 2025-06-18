//
//  NBFuenteEspejo.h
//  AUFramework
//
//  Created by Marcos Ortega on 10/12/12.
//
//

#ifndef AUFramework_NBFuenteEspejo_h
#define AUFramework_NBFuenteEspejo_h

#include "AUObjeto.h"
#include "NBPunto.h"
#include "NBCajaAABB.h"

//Datos plantilla del espejo
struct NBFuenteEspejo {
	float			factorReflejaLuz;
	NBPunto			limitesLocalesEspejo[2];
	AUObjeto*		objEscena;
	//
	bool operator==(const NBFuenteEspejo &otro) const {
		return (objEscena==otro.objEscena && factorReflejaLuz==otro.factorReflejaLuz && limitesLocalesEspejo[0]==otro.limitesLocalesEspejo[0] && limitesLocalesEspejo[1]==otro.limitesLocalesEspejo[1]);
	}
	bool operator!=(const NBFuenteEspejo &otro) const {
		return !(objEscena==otro.objEscena && factorReflejaLuz==otro.factorReflejaLuz && limitesLocalesEspejo[0]==otro.limitesLocalesEspejo[0] && limitesLocalesEspejo[1]==otro.limitesLocalesEspejo[1]);
	}
};
	
//Datos actualizados antes de enviar a consumidores
struct NBFuenteEspejoRender {
	NBCajaAABB		cajaEscenaEspejo;
	float			factorReflejaLuz;
	float			longitudEscenaEspejo;
	AUObjeto*		objEspejo;
	NBPunto			limitesEscenaEspejo[2];
	NBPunto			vecUnitEspejo;				//Vector unitario que inicia en "limitesEscenaEspejo[0]"
	NBPunto			vecEspejo;					//Vector espejo que inicia en "limitesEscenaEspejo[0]"
	float			radianesEspejo;
	//
	bool operator==(const NBFuenteEspejoRender &otro) const {
		return (factorReflejaLuz==otro.factorReflejaLuz && limitesEscenaEspejo[0]==otro.limitesEscenaEspejo[0] && limitesEscenaEspejo[1]==otro.limitesEscenaEspejo[1]);
	}
	bool operator!=(const NBFuenteEspejoRender &otro) const {
		return !(factorReflejaLuz==otro.factorReflejaLuz && limitesEscenaEspejo[0]==otro.limitesEscenaEspejo[0] && limitesEscenaEspejo[1]==otro.limitesEscenaEspejo[1]);
	}
};

#endif
