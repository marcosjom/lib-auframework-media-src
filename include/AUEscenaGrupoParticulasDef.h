//
//  AUEscenaGrupoParticulasDef.h
//  AUFramework
//
//  Created by Marcos Ortega on 05/12/12.
//
//

#ifndef AUFramework_AUEscenaGrupoParticulasDef_h
#define AUFramework_AUEscenaGrupoParticulasDef_h

#include "NBPunto.h"
#include "AUTextura.h"

struct STParticulaProps {
	bool		texRegistrada;		//la textura esta cargada y registrada en su grupo (false si la textura esta en precarga)
	SI8			texAlineacionH;
	SI8			texAlineacionV;
	AUTextura*	objTextura;
	float		escala;
	float		alpha;
	float		radianes;
	NBPunto		posicion;
	bool operator==(const STParticulaProps &otro) const {
		return (objTextura==otro.objTextura && posicion==otro.posicion && radianes==otro.radianes && alpha==otro.alpha);
	}
	bool operator!=(const STParticulaProps &otro) const {
		return !(objTextura==otro.objTextura && posicion==otro.posicion && radianes==otro.radianes && alpha==otro.alpha);
	}
};

#endif
