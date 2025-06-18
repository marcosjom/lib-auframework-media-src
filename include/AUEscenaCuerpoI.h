//
//  AUEscenaCuerpoI.h
//  Gameplay_iOS
//
//  Created by Marcos Ortega on 25/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUESCENACUERPOI_H_INCLUIDO
#define AUESCENACUERPOI_H_INCLUIDO

#include "AUEscenaCuerda.h"
#include "AUCuerpoDef.h"
#include "AUEscenaFigura.h"
#include "AUEscenaMallaGelatina.h"
#include "AUEscenaMallaConHueso.h"
#include "AUEscenaIluminacion.h"
#include "AUEscenaLuzSombra.h"
#include "AUEscenaLuzRadial.h"
#include "NBGestorFisica.h"
#include "NBGestorEscena.h"
#include "AUEscenaGrupoParticulas.h"
//#include "AUEscenaAnimacion.h"
//#include "NBGestorAnimaciones.h"

enum ENCuerpoTipo {
	ENCuerpoTipo_Estatico	= 0,	//Mismo valor que b2_staticBody
	ENCuerpoTipo_Kinematico,		//Mismo valor que b2_kineaticBody
	ENCuerpoTipo_Dinamico,			//Mismo valor que b2_dynamicBody
	ENCuerpoTipo_Ninguno,			//Valor no existe en Box2D
	ENCuerpoTipo_Conteo
};

struct STCuerpoFisicaDef {
	SI8			tipoCuerpo;		//ENCuerpoTipo
	SI8			tipoAnimacion;	//ENAnimFisicaTipo
	UI8			mascaraProps;
	SI8			filtroCuerpos;
	float		factorGravedad;
	void*		datoUsuario;
};

struct STCuerpoMultDef {
	float		multDensidad;
	float		multFriccion;
	float		multRestitucion;
};

enum ENCuerpoSonidoTipo {
	ENCuerpoSonidoTipo_Impacto = 0,
	ENCuerpoSonidoTipo_Destructivo
};

struct NBParticula {
	bool				activa;
	AUEscenaObjeto*		objEscena;
	float				tiempoViva;
	NBPunto				direccion;
	float				rotacion;
	//
	bool operator==(const NBParticula &otro) const {
		return (objEscena == otro.objEscena && tiempoViva == otro.tiempoViva && direccion==otro.direccion && rotacion==otro.rotacion);
	}
	bool operator!=(const NBParticula &otro) const {
		return !(objEscena == otro.objEscena && tiempoViva == otro.tiempoViva && direccion==otro.direccion && rotacion==otro.rotacion);
	}
};

class AUEscenaCuerpoI: public AUEscenaContenedor {
	public:
		AUEscenaCuerpoI();
		AUEscenaCuerpoI(AUEscenaContenedor* contendorPlantilla);
		virtual ~AUEscenaCuerpoI();
		//
		virtual const char*							rutaArchivoOrigen() = 0;
		virtual float								multiplicadorDanoProduce() = 0;
		virtual float								multiplicadorDanoRecibe() = 0;
		virtual void								agregarParticulasObjetosDesprendiblesHaciaCapa(AUArregloNativoMutableP<NBParticula>* arregloCrearPiezaDesprendibleEn, AUEscenaContenedor* agregarEnCapa, const NBPunto &vectorImpacto) = 0;
		virtual NBCajaAABB							cajaLocalSombra() = 0;
		virtual AUArregloNativoP<STParticulaDef>*	definicionesParticulas() = 0;
		virtual const STParticulaDef*				cualquierParticulaEnZonaGeneracion() = 0;
		virtual void*								cuerpo() = 0; //b2Body*
		virtual void								procesarImpacto(float velImpacto, ENCuerpoSonidoTipo tipoSonido, AUArregloMutable* arregloSonidosRecicables) = 0;
		STCuerpoMultDef								multiplicadoresPropsCuerpo();
		float										multiplicadorDensidad();
		float										multiplicadorFriccion();
		float										multiplicadorRestitucion();
		//
		virtual void								establecerGrupoAnimacion(ENGestorAnimadoresGrupo grupoAnimacion) = 0;
		//
		STCuerpoFisicaDef							definicionCreacionCuerpo();
		virtual void								establecerCuerpoTipo(ENCuerpoTipo tipoCuerpo, ENAnimFisicaTipo tipoAnimacion);
		virtual void								establecerCreacionCuerpoTipo(ENCuerpoTipo tipoCuerpo, ENAnimFisicaTipo tipoAnimacion);
		virtual void								establecerCreacionCuerpoMascara(UI8 mascaraProps);
		virtual void								establecerCreacionCuerpoFiltro(SI16 filtroCuerpo);
		virtual void								establecerCreacionCuerpoFactorGravedad(float factorGravedad);
		virtual void								establecerCreacionCuerpoDatoUsuario(void* datoUsuario);
		void										establecerMultiplicadoresPropsCuerpo(const STCuerpoMultDef &multiplicadores);
		void										establecerMultiplicadorDensidad(const float multiplicador);
		void										establecerMultiplicadorFriccion(const float multiplicador);
		void										establecerMultiplicadorRestitucion(const float multiplicador);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		STCuerpoFisicaDef							_propsCreacionCuerpo;
		STCuerpoMultDef								_propsMultiplicadores;
		static bool									privGenerarParticula(AUTextura* textura, const NBPunto &posEscena, const NBPunto &vectorImpacto, AUEscenaGrupoParticulas* grupoParticulasDestino);
};

#endif


