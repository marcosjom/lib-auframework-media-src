//
//  AUEscenaLaserDef.h
//  AUFramework
//
//  Created by Marcos Ortega on 15/03/13.
//
//

#ifndef AUFramework_AUEscenaLaserDef_h
#define AUFramework_AUEscenaLaserDef_h

struct STEscenaLaserPos {
	NBPunto		posEscena;
	float		moduloSegmento;		//longitud de segmento
	NBPunto		vecUnitSegmento;	//vector hacia adelante
	NBPunto		vecUnitHaciaIzq;	//Determina la forma del segmento del laser
	//
	bool operator==(const STEscenaLaserPos &otro) const {
		return (posEscena==otro.posEscena && vecUnitHaciaIzq==otro.vecUnitHaciaIzq);
	}
	bool operator!=(const STEscenaLaserPos &otro) const {
		return !(posEscena==otro.posEscena && vecUnitHaciaIzq==otro.vecUnitHaciaIzq);
	}
};
	
#endif
