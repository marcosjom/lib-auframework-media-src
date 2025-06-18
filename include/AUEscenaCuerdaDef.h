//
//  AUEscenaCuerdaDef.h
//  AUFramework
//
//  Created by Marcos Ortega on 07/12/12.
//
//

#ifndef AUFramework_AUEscenaCuerdaDef_h
#define AUFramework_AUEscenaCuerdaDef_h

#include "NBPunto.h"
//#ifndef CONFIG_NB_UNSUPPORT_BOX2D
//#include "b2Math.h" //Para acceder a b2Vec2
//#endif

enum ENCuerdaTipo {
	ENCuerdaTipo_Continua,		//segmentos unidos sin separaciones
	ENCuerdaTipo_Cadena,		//segmentos conseparaciones permitidas
	ENCuerdaTipo_CintaDoblable,	//segmentos continuos que solo se desplazan a lo horizontal
	ENCuerdaTipo_Conteo
};

struct STCuerdaVertice {
	NBPunto	ps;		//Posicion (b2Vec2)
	NBPunto	p0s;	//Posicion anterior (b2Vec2)
	NBPunto	vs;		//Velocidad (b2Vec2)
	float	ims;	//Masa invertida (1.0f/masa)
	//
	bool operator==(const STCuerdaVertice &otro) const {
		return (ps==otro.ps);
	}
	bool operator!=(const STCuerdaVertice &otro) const {
		return !(ps==otro.ps);
	}
};
	
struct STCuerdaSegmento {
	float	Ls;		//longitud
	float	factorDanoPorCorte;	//dano recibido por corte de laser
	bool operator==(const STCuerdaSegmento &otro) const {
		return (Ls==otro.Ls);
	}
	bool operator!=(const STCuerdaSegmento &otro) const {
		return !(Ls==otro.Ls);
	}
};
		
#endif
