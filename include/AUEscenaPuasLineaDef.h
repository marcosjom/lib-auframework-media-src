//
//  AUEscenaPuasLineaDef.h
//  AUFramework
//
//  Created by Marcos Ortega on 11/03/13.
//
//

#ifndef AUFramework_AUEscenaPuasLineaDef_h
#define AUFramework_AUEscenaPuasLineaDef_h

enum ENPuasLineaTamano {
	ENPuasLineaTamano_Pequeno = 0,
	ENPuasLineaTamano_Mediano,
	ENPuasLineaTamano_Grande,
	ENPuasLineaTamano_Gigante,
	ENPuasLineaTamano_Conteo
};

struct STPuasLineaRender {
	UI32		idTexturaGL;
	STBloqueGL	bloqueIndicesGL;
};

enum ENPuasLineaAnimEstado {
	ENPuasLineaAnimEstado_Mostradas = 0,
	ENPuasLineaAnimEstado_Ocultando,
	ENPuasLineaAnimEstado_Ocultas,
	ENPuasLineaAnimEstado_Showing,
	ENPuasLineaAnimEstado_Conteo
};

struct ENPuasLineaPropsAnim {
	UI8		animEstapaActual;	//ENPuasLineaAnimEstado
	float	factorTamanoAct;
	float	segsAcumEtapaActual;
	//
	bool operator==(const ENPuasLineaPropsAnim &otro) const {
		return (animEstapaActual==otro.animEstapaActual && factorTamanoAct==otro.factorTamanoAct && segsAcumEtapaActual==otro.segsAcumEtapaActual);
	}
	bool operator!=(const ENPuasLineaPropsAnim &otro) const {
		return !(animEstapaActual==otro.animEstapaActual && factorTamanoAct==otro.factorTamanoAct && segsAcumEtapaActual==otro.segsAcumEtapaActual);
	}
};

enum ENPuasLineaAnimIndiv {
	ENPuasLineaAnimIndiv_UnHueco = 0,	//Oculta una o varias puas consecutivas, formando un hueco-sin-peligro
	ENPuasLineaAnimIndiv_Conteo
};

#endif
