//
//  AUEscenaPolea.h
//  AUFramework
//
//  Created by Nicaragua Binary on 10/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaPolea_h_INCLUIDO
#define AUEscenaPolea_h_INCLUIDO

//#ifndef CONFIG_NB_UNSUPPORT_BOX2D
//#include "b2PulleyJoint.h"
//#endif

#include "NBAnimador.h"
#include "AUEscenaContenedor.h"
#include "AUEscenaSprite.h"
#include "AUEscenaCuerda.h"
#include "NBGestorAnimadores.h"

class AUEscenaPolea;

class IEscuchadorPolea {
	public:
		virtual ~IEscuchadorPolea(){
			//
		}
		virtual void poleaCortada(AUEscenaPolea* objPolea, AUEscenaCuerda* objCuerda, UI16 iSegmentoCorte) = 0;
};

class AUEscenaPolea : public AUEscenaContenedor, public NBAnimador, public IEscuchadorCuerda {
	public:
		AUEscenaPolea(AUTextura* texPoleaA, AUTextura* texPoleaB, AUTextura* texSegmentosCuerda, /*b2PulleyJoint*/void* unionPolea, ENCuerdaTipo tipoCuerda, float superposicionSegmentos, float masaSegmentos, float factorEstiramiento, float k2, float k3, float factorLongitud, float segsCortaCuerda);
		virtual ~AUEscenaPolea();
		//
		void*						unionPolea(); //b2PulleyJoint*
		IEscuchadorPolea*			escuchadorPolea();
		//
		void						establecerUnionPolea(/*b2PulleyJoint*/void* unionPolea);
		void						establecerEscuchadorPolea(IEscuchadorPolea* escuchador);
		//
		void						procesarCuerdaCortada(AUEscenaCuerda* objCuerda, UI16 iSegmentoCorte, AUEscenaContenedor* cuerpoA, AUEscenaContenedor* cuerpoB, AUArregloMutable* arregloParaCuerdasHuerfanas);
		virtual void				cuerdaCortada(AUEscenaCuerda* objCuerda, UI16 iSegmentoCorte);
		//
		virtual void				tickAnimacion(float segsTranscurridos);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		bool						_cuerdasInicializadas;
		void*						_unionPolea; //b2PulleyJoint*
		AUEscenaSprite*				_poleaA;
		AUEscenaSprite*				_poleaB;
		AUEscenaCuerda*				_cuerdaPoleaA;
		AUEscenaCuerda*				_cuerdaPoleaB;
		AUEscenaCuerda*				_cuerdaEntrePoleas;
		IEscuchadorPolea*			_escuchadorPolea;
		float						_radioPoleaA;		//Radio en el que circula la cuerda
		float						_radioPoleaB;		//Radio en el que circula la cuerda
		float						_longitudPoleaA;	//El perimetro/longitud del circulo exterior
		float						_longitudPoleaB;	//El perimetro/longitud del circulo exterior
		float						_factorLongitud;
		float						_segsCortaCuerda;
		//
		virtual void				agregadoEnEscena();
		virtual void				quitandoDeEscena();
		void						privOrganizarObjetosSegunUnion();
};

#endif
