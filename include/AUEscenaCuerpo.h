//
//  AUEscenaCuerpo.h
//  Gameplay_iOS
//
//  Created by Marcos Ortega on 25/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUESCENACUERPO_H_INCLUIDO
#define AUESCENACUERPO_H_INCLUIDO

#include "AUEscenaCuerpoI.h"
#include "AUEscenaAnimacionI.h"

class AUEscenaCuerpo: public AUEscenaCuerpoI {
	public:
		AUEscenaCuerpo(AUCuerpoDef* definicionCuerpo);
		AUEscenaCuerpo(AUEscenaCuerpo* otroCuerpo);
		virtual ~AUEscenaCuerpo();
		//
		virtual const char*										rutaArchivoOrigen();
		virtual float											multiplicadorDanoProduce();
		virtual float											multiplicadorDanoRecibe();
		virtual void											agregarParticulasObjetosDesprendiblesHaciaCapa(AUArregloNativoMutableP<NBParticula>* arregloCrearPiezaDesprendibleEn, AUEscenaContenedor* agregarEnCapa, const NBPunto &vectorImpacto);
		virtual NBCajaAABB										cajaLocalSombra();
		virtual AUArregloNativoP<STParticulaDef>*				definicionesParticulas();
		virtual const STParticulaDef*							cualquierParticulaEnZonaGeneracion();
		virtual void*											cuerpo(); //b2Body*
		virtual void											procesarImpacto(float velImpacto, ENCuerpoSonidoTipo tipoSonido, AUArregloMutable* arregloSonidosRecicables);
		virtual void											establecerGrupoAnimacion(ENGestorAnimadoresGrupo grupoAnimacion);
		//
		void													crearCuerpo();			//Para manejo manual (los cuerpos se crean y destruyen automaticamente)
		void													crearUnionesCuerpo();	//Para manejo manual (los cuerpos se crean y destruyen automaticamente)
		void													destruirCuerpo();		//Para manejo manual (los cuerpos se crean y destruyen automaticamente)
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		void*													_cuerpo; //b2Body*
		AUCuerpoDef*											_definicionCuerpo;
#		ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
		AUArregloNativoMutableP<STEscenaSonidoImpacto>			_sonidosDeImpacto;
#		endif
		//
		void													privInicializarHijosHeredados(AUArreglo* hijosCapaLocal, AUArreglo* hijosCapaOriginal);
		virtual void											agregadoEnEscena();
		virtual void											agregadoEnEscenaConHijosNotificados();
		virtual void											quitandoDeEscena();
};

#endif


