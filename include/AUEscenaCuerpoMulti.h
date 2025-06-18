//
//  AUEscenaCuerpoMulti.h
//  Gameplay_iOS
//
//  Created by Marcos Ortega on 25/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaCuerpoMulti_H_INCLUIDO
#define AUEscenaCuerpoMulti_H_INCLUIDO

#include "AUEscenaCuerpoI.h"
#include "AUEscenaCuerpo.h"

class AUEscenaCuerpoMulti: public AUEscenaCuerpoI {
	public:
		AUEscenaCuerpoMulti();
		AUEscenaCuerpoMulti(AUEscenaCuerpoMulti* otroCuerpo);
		virtual ~AUEscenaCuerpoMulti();
		//
		SI16													indiceCuerpoActual();
		float													vida();
		void													reducirVida(float reduccionPositiva);
		float													reducirVida(float velImpacto, float retitucionImpacto, float masaOtroCuerpo, float multiplicadorDanoProduceOtroCuerpo, NBPunto miVelocidad, NBPunto velOtro, AUArregloNativoMutableP<NBParticula>* arregloCrearPiezaDesprendibleEn, AUEscenaContenedor* agregarEnCapa, AUArregloMutable* arregloSonidosRecicables);
		void													generarParticulasAleatoriasEn(AUEscenaGrupoParticulas* agregarParticulasEn, const NBPunto &vectorImpacto, SI32 cantidadParticulas);
		void													generarParticulasTodasEn(AUEscenaGrupoParticulas* agregarParticulasEn, const NBPunto &vectorImpacto);
		//
		virtual void											establecerCuerpoTipo(ENCuerpoTipo tipoCuerpo, ENAnimFisicaTipo tipoAnimacion);
		virtual void											establecerCreacionCuerpoTipo(ENCuerpoTipo tipoCuerpo, ENAnimFisicaTipo tipoAnimacion);
		virtual void											establecerCreacionCuerpoMascara(UI8 mascaraProps);
		virtual void											establecerCreacionCuerpoFiltro(SI16 filtroCuerpo);
		virtual void											establecerCreacionCuerpoFactorGravedad(float factorGravedad);
		virtual void											establecerCreacionCuerpoDatoUsuario(void* datoUsuario);
		//
		virtual const char*										rutaArchivoOrigen();
		virtual float											multiplicadorDanoProduce();
		virtual float											multiplicadorDanoRecibe();
		virtual void											agregarParticulasObjetosDesprendiblesHaciaCapa(AUArregloNativoMutableP<NBParticula>* arregloCrearPiezaDesprendibleEn, AUEscenaContenedor* agregarEnCapa, const NBPunto &vectorImpacto);
		virtual NBCajaAABB										cajaLocalSombra();
		virtual AUArregloNativoP<STParticulaDef>*				definicionesParticulas();
		virtual const STParticulaDef*							cualquierParticulaEnZonaGeneracion();
		virtual void											procesarImpacto(float velImpacto, ENCuerpoSonidoTipo tipoSonido, AUArregloMutable* arregloSonidosRecicables);
		//
		virtual void											establecerGrupoAnimacion(ENGestorAnimadoresGrupo grupoAnimacion);
		//
		virtual void*											cuerpo(); //b2Body*
		void													crearCuerpo();			//Para manejo manual (los cuerpos se crean y destruyen automaticamente)
		void													crearUnionesCuerpo();	//Para manejo manual (los cuerpos se crean y destruyen automaticamente)
		void													destruirCuerpo();		//Para manejo manual (los cuerpos se crean y destruyen automaticamente)
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		SI16													_iCuerpoActual;
		//
		virtual void											agregadoEnEscenaConHijosNotificados();
		virtual void											quitandoDeEscena();
		//
		void													privAgregarCuerpoEstado(AUEscenaCuerpo* cuerpoParaEstado);
		SI16													privIndicePresentacionSegunVida(float vida);
		void													privCambiarCuerpoSiNecesario();
	private:
		float													_vida;
		AUArregloMutable										_cuerposPorEstado;
};

#endif


