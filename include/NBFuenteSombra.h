//
//  NBFuenteSombra.h
//  AUFramework
//
//  Created by Nicaragua Binary on 05/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef NBFUENTESOMBRA_H_INCLUIDO
#define NBFUENTESOMBRA_H_INCLUIDO

#include "AUObjeto.h"
#include "AUTextura.h"
#include "NBColor.h"

struct STRangoSombra {
	NBPunto		vectorAngMax;
	NBPunto		vectorAngMin;
	//
	bool operator==(const STRangoSombra &otro) const {
		return (vectorAngMin==otro.vectorAngMin && vectorAngMax==otro.vectorAngMax);
	}
	bool operator!=(const STRangoSombra &otro) const {
		return !(vectorAngMin==otro.vectorAngMin && vectorAngMax==otro.vectorAngMax);
	}
};
	
struct NBFuenteSombra {
	AUObjeto*			objOriginaSombra;				//Objeto que origina la sombra
	AUObjeto*			objAgrupaSombras;				//Objeto que agrupa las sombras, para evitar que cuerpos unidos proyecten sus sombras entre si
	bool				sombraCubiertaPorOtra;			//Cuando esta sombra es totalmente cubierta por otra sombra de la luz
	UI8					conteoVerticesAdicionalSombra;	//En la biblioteca de vertices compartidos. Solo los que forman parte de la figura sombra.
	UI16				iPrimerVerticeAdicionalSombra;	//En la biblioteca de vertices compartidos. Solo los que forman parte de la figura sombra.
	NBPunto				vectorAnguloMax;				//Vector radial a la luz. A partir del [iPrimerVerticeAdicionalSombra].
	NBPunto				vectorAnguloMin;				//Vector radial a la luz. A partir del [iPrimerVerticeAdicionalSombra + conteoVerticesAdicionalSombra - 1].
	NBCajaAABB			cajaEscenaSombra;
	//
	bool operator==(const NBFuenteSombra &otro) const {
		return (iPrimerVerticeAdicionalSombra==otro.iPrimerVerticeAdicionalSombra && conteoVerticesAdicionalSombra==otro.conteoVerticesAdicionalSombra);
	}
	bool operator!=(const NBFuenteSombra &otro) const {
		return !(iPrimerVerticeAdicionalSombra==otro.iPrimerVerticeAdicionalSombra && conteoVerticesAdicionalSombra==otro.conteoVerticesAdicionalSombra);
	}
};

#endif
