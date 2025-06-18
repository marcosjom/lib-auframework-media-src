//
//  Header.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 25/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUCUERPODEF_H_INLCUIDO
#define AUCUERPODEF_H_INLCUIDO

//#ifndef CONFIG_NB_UNSUPPORT_BOX2D
//#include "b2Body.h"
//#include "b2Joint.h"
//#include "b2WeldJoint.h"
//#include "b2RevoluteJoint.h"
//#include "b2DistanceJoint.h"
//#include "b2RopeJoint.h"
//#endif
//
#include "NBPropiedadesFijacion.h"
#include "NBGestorSonidos.h"
#include "AUEscenaSonido.h"
#include "AUEscenaContenedor.h"
#include "AUEscenaCuerda.h"
#include "AUEscenaGeneradorParticulas.h"
//
#include "AUBateria.h"
#include "AUPanelSolar.h"
#include "AUBombillo.h"
#include "AULaser.h"
#include "AULanzallamas.h"
#include "AUElectronodo.h"

struct STParticulaDef {
	SI32		indiceTextura;
	AUTextura*	textura;
	NBPunto		posicion;
	bool		estaDentroZonaGeneracion;
	bool operator==(const STParticulaDef &otro) const {
		return (indiceTextura==otro.indiceTextura && textura==otro.textura && posicion==otro.posicion && estaDentroZonaGeneracion==otro.estaDentroZonaGeneracion);
	}
	bool operator!=(const STParticulaDef &otro) const {
		return !(indiceTextura==otro.indiceTextura && textura==otro.textura && posicion==otro.posicion && estaDentroZonaGeneracion==otro.estaDentroZonaGeneracion);
	}
};

struct STIlumMascara {
	AUTextura*		texturaMascara;
	NBRectangulo	limites;
	NBPunto			posicion;
	NBTamano		escalacion;
	float			rotacion;
	//
	NBPunto			cacheVerticesCuerpo[4];				//vertices ya transformados a coordenadas del cuerpo/contenedor (en orden contrareloj)
	//
	bool operator==(const STIlumMascara &otro) const {
		return (texturaMascara==otro.texturaMascara && limites==otro.limites && posicion==otro.posicion && escalacion==otro.escalacion && rotacion==otro.rotacion);
	}
	bool operator!=(const STIlumMascara &otro) const {
		return !(texturaMascara==otro.texturaMascara && limites==otro.limites && posicion==otro.posicion && escalacion==otro.escalacion && rotacion==otro.rotacion);
	}
};

#define AUCUERPODEF_MASCARA_ACTIVO				1
#define AUCUERPODEF_MASCARA_PERMITIR_DORMIR		2
#define AUCUERPODEF_MASCARA_ROTACION_FIJA		4
#define AUCUERPODEF_MASCARA_ES_BALA				8

struct STFijacionDefinicion {
	SI32					tipoFijacion;
	NBPropiedadesFijacion	propiedadesFijacion;
	AUArregloNativoMutableP<NBPunto>* vertices;
	bool operator==(const STFijacionDefinicion &otro) const {
		return (vertices==otro.vertices);
	}
	bool operator!=(const STFijacionDefinicion &otro) const {
		return !(vertices==otro.vertices);
	}
};

enum ENUnionCuerpoTipo {
	ENUnionCuerpoTipo_NoDefinido = -1,
	ENUnionCuerpoTipo_Rotacion = 0,
	ENUnionCuerpoTipo_Distancia,
	ENUnionCuerpoTipo_Soldadura,
	ENUnionCuerpoTipo_Cuerda,
	ENUnionCuerpoTipo_Prismatica,
	ENUnionCuerpoTipo_Friccion,
	ENUnionCuerpoTipo_Conteo
};

struct STUnionCuerposDef {
	ENUnionCuerpoTipo	tipoUnion;
	SI32				indiceCuerpoA;
	SI32				indiceCuerpoB;
	//AUTextura*		textura0;
	//AUTextura*		textura1;
	//AUTextura*		textura2;
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	void*				b2Definicion; //b2JointDef*
	void*				estructuraAdicional;
#	endif
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	bool operator==(const STUnionCuerposDef &otro) const {
		return (b2Definicion==otro.b2Definicion);
	}
	bool operator!=(const STUnionCuerposDef &otro) const {
		return !(b2Definicion==otro.b2Definicion);
	}
#	endif
};

struct STUnionCuerposCuerdaDef {
	ENCuerdaTipo	tipoCuerda;
	float			masaSegmentos;
	float			factorEstiramiento;
	float			superposicionSegmentos;
	AUTextura*		texturaCuerda;
};

//Sonidos de impactos
struct STEscenaSonidoImpactoDef {
	UI16	bufferAL;
	float	segsDuracion;
	bool	esSonidoDestruccion;
	float	velImpactoMin;
	float	velImpactoMax;
	bool operator==(const STEscenaSonidoImpactoDef &otro) const {
		return (bufferAL==otro.bufferAL && velImpactoMin==otro.velImpactoMin && velImpactoMax==otro.velImpactoMax);
	}
	bool operator!=(const STEscenaSonidoImpactoDef &otro) const {
		return !(bufferAL==otro.bufferAL && velImpactoMin==otro.velImpactoMin && velImpactoMax==otro.velImpactoMax);
	}
};

#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
struct STEscenaSonidoImpacto {
	STEscenaSonidoImpactoDef*	definicion;
	AUEscenaSonido*				sonido;
	float						segundoVirtualFinalizariaReproduccion;
	bool operator==(const STEscenaSonidoImpacto &otro) const {
		return (sonido==otro.sonido);
	}
	bool operator!=(const STEscenaSonidoImpacto &otro) const {
		return !(sonido==otro.sonido);
	}
};
#endif
/*
 AUCadenaMutable8											rutaRecurso;					//Ruta con que fue cargado el recurso
 //Capa de presentacion
 AUEscenaContenedor*											plantillaPresentacion;			//ObjsEscena de presentacion
 AUArregloNativoMutableP<SI32>								indicesObjEscenaAleatorios;
 AUArregloNativoMutableP<SI32>								indicesObjEscenaDesprendibles;
 //Capa de iluminacion
 NBCajaAABB													cajaLocalMascarasIlum;
 NBColor														colorIluminacion;				//ilumnacion hacia este cuerpo
 NBColor														colorSombra;					//sombra que proyecta este cuerpo
 AUTextura*													texturaIluminacion;				//patron de iluminacion hacia este cuerpo
 AUTextura*													texturaSombra;					//patron de sombra que proyecta este cuerpo
 AUArregloNativoMutableP<STIlumMascara>						mascarasIluminacion;
 //Capa de sombra
 NBCajaAABB													cajaLocalSombras;				//Cajaque encierra a todos los vertices de las figuras que producen sombra
 AUArregloMutable											ptsFigurasSombraLocal;			//contiene arreglos: "AUArregloNativoMutableP<NBPunto>"
 //Capa de fisica
 AUArregloNativoMutableP<STFijacionDefinicion>				fijaciones;
 AUArregloNativoMutableP<STUnionCuerposDef>					defUnionesCuerpos;
 //Capa de sonidos
 AUArregloNativoMutableP<STEscenaSonidoImpactoDef>			defSonidosDeImpacto;
 //Capa de particulas
 AUArregloMutable											texturasParticulas;
 AUArregloNativoMutableP<STParticulaDef>						defParticulas;
 AUArregloNativoMutableP<SI32>								indicesParticulasEnZonasGen;
 AUArregloMutable											ptsZonasLocalGenParticulas;		//contiene arreglos: "AUArregloNativoMutableP<NBPunto>"
 float														multDanoProduce;
 float														multDanoRecibe;
 */
//Plantilla de cuerpo
class AUCuerpoDef : public AUObjeto {
	public:
		AUCuerpoDef() : AUObjeto(), 
						rutaRecurso(this), 
						indicesObjEscenaAleatorios(this), 
						indicesObjEscenaDesprendibles(this),
						mascarasIluminacion(this),
						ptsFigurasSombraLocal(this),
						fijaciones(this),
						defUnionesCuerpos(this),
						defSonidosDeImpacto(this),
						texturasParticulas(this),
						defParticulas(this),
						indicesParticulasEnZonasGen(this),
						ptsZonasLocalGenParticulas(this)
		{
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCuerpoDef")
			//rutaRecurso					= new(this) AUCadenaMutable8();
			multDanoProduce					= 1.0f;
			multDanoRecibe					= 1.0f;
			plantillaPresentacion			= new(this) AUEscenaContenedor();
			texturaIluminacion				= NULL;
			NBCOLOR_ESTABLECER(colorIluminacion, 1.0f, 1.0f, 1.0f, 1.0f);
			NBCOLOR_ESTABLECER(colorSombra, 1.0f, 1.0f, 1.0f, 1.0f);
			texturaSombra					= NULL;
			//indicesObjEscenaAleatorios		= new(this) AUArregloNativoMutableP<SI32>();
			//indicesObjEscenaDesprendibles	= new(this) AUArregloNativoMutableP<SI32>();
			//ptsFigurasSombraLocal			= new(this) AUArregloMutable();
			//texturasParticulas				= new(this) AUArregloMutable();
			//defParticulas					= new(this) AUArregloNativoMutableP<STParticulaDef>();
			//indicesParticulasEnZonasGen		= new(this) AUArregloNativoMutableP<SI32>();
			//ptsZonasLocalGenParticulas		= new(this) AUArregloMutable();
			NBCAJAAABB_INICIALIZAR(cajaLocalMascarasIlum);
			NBCAJAAABB_INICIALIZAR(cajaLocalSombras);
			//
			//mascarasIluminacion				= new(this) AUArregloNativoMutableP<STIlumMascara>();
			//fijaciones						= new(this) AUArregloNativoMutableP<STFijacionDefinicion>();
			//defUnionesCuerpos				= new(this) AUArregloNativoMutableP<STUnionCuerposDef>();
			//defSonidosDeImpacto				= new(this) AUArregloNativoMutableP<STEscenaSonidoImpactoDef>();
		}
		virtual ~AUCuerpoDef(){
			//Liberar mascaras de iluminacion
			//NBASSERT(_mascarasIluminacion->conteoReferencias() == 1); //Si falla, alguien aun utiliza/retiene los datos
			UI32 iMasc, iMascConteo = mascarasIluminacion.conteo;
			for(iMasc=0; iMasc<iMascConteo; iMasc++){
				STIlumMascara datosMascara =  mascarasIluminacion.elemento[iMasc];
				if(datosMascara.texturaMascara != NULL) datosMascara.texturaMascara->liberar(NB_RETENEDOR_THIS);
			}
			//Liberar fijaciones
			//NBASSERT(_fijaciones->conteoReferencias() == 1); //Si falla, alguien aun utiliza/retiene los datos
			UI32 f, fConteo = fijaciones.conteo;
			for(f=0; f<fConteo; f++){
				STFijacionDefinicion* fijacion = &(fijaciones.elemento[f]);
				fijacion->vertices->liberar(NB_RETENEDOR_THIS);
			}
			//Liberar def uniones
			//NBASSERT(_defUnionesCuerpos->conteoReferencias() == 1); //Si falla, alguien aun utiliza/retiene los datos
			UI32 u, uConteo = defUnionesCuerpos.conteo;
			for(u=0; u<uConteo; u++){
				if(defUnionesCuerpos.elemento[u].tipoUnion == ENUnionCuerpoTipo_Cuerda){
#					ifndef CONFIG_NB_UNSUPPORT_BOX2D
					STUnionCuerposCuerdaDef* defCuerda = (STUnionCuerposCuerdaDef*)defUnionesCuerpos.elemento[u].estructuraAdicional;
					if(defCuerda->texturaCuerda != NULL) defCuerda->texturaCuerda->liberar(NB_RETENEDOR_THIS);
					delete defCuerda;
#					endif
				}
#				ifndef CONFIG_NB_UNSUPPORT_BOX2D
				delete defUnionesCuerpos.elemento[u].b2Definicion;
#				endif
			}
			//Liberar def sonidos
			//NBASSERT(_defSonidosDeImpacto->conteoReferencias() == 1); //Si falla, alguien aun utiliza/retiene los datos
			UI32 iSnd, iSndConteo = defSonidosDeImpacto.conteo;
			for(iSnd=0; iSnd<iSndConteo; iSnd++){
#				ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
				if(defSonidosDeImpacto.elemento[iSnd].bufferAL!=0) NBGestorSonidos::bufferLiberar(defSonidosDeImpacto.elemento[iSnd].bufferAL);
#				endif
			}
			//
			plantillaPresentacion->liberar(NB_RETENEDOR_THIS);
			if(texturaIluminacion != NULL) texturaIluminacion->liberar(NB_RETENEDOR_THIS);
			if(texturaSombra != NULL) texturaSombra->liberar(NB_RETENEDOR_THIS);
		}
		//
		AUCadenaMutable8											rutaRecurso;					//Ruta con que fue cargado el recurso
		//Capa de presentacion
		AUEscenaContenedor*											plantillaPresentacion;			//ObjsEscena de presentacion
		AUArregloNativoMutableP<SI32>								indicesObjEscenaAleatorios;
		AUArregloNativoMutableP<SI32>								indicesObjEscenaDesprendibles;
		//Capa de iluminacion
		NBCajaAABB													cajaLocalMascarasIlum;
		NBColor														colorIluminacion;				//ilumnacion hacia este cuerpo
		NBColor														colorSombra;					//sombra que proyecta este cuerpo
		AUTextura*													texturaIluminacion;				//patron de iluminacion hacia este cuerpo
		AUTextura*													texturaSombra;					//patron de sombra que proyecta este cuerpo
		AUArregloNativoMutableP<STIlumMascara>						mascarasIluminacion;
		//Capa de sombra
		NBCajaAABB													cajaLocalSombras;				//Cajaque encierra a todos los vertices de las figuras que producen sombra
		AUArregloMutable											ptsFigurasSombraLocal;			//contiene arreglos: "AUArregloNativoMutableP<NBPunto>"
		//Capa de fisica
		AUArregloNativoMutableP<STFijacionDefinicion>				fijaciones;
		AUArregloNativoMutableP<STUnionCuerposDef>					defUnionesCuerpos;
		//Capa de sonidos
		AUArregloNativoMutableP<STEscenaSonidoImpactoDef>			defSonidosDeImpacto;
		//Capa de particulas
		AUArregloMutable											texturasParticulas;
		AUArregloNativoMutableP<STParticulaDef>						defParticulas;
		AUArregloNativoMutableP<SI32>								indicesParticulasEnZonasGen;
		AUArregloMutable											ptsZonasLocalGenParticulas;		//contiene arreglos: "AUArregloNativoMutableP<NBPunto>"
		float														multDanoProduce;
		float														multDanoRecibe;
		//
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_UNICLASE(AUCuerpoDef)
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_UNICLASE(AUCuerpoDef, "AUCuerpoDef")
		AUOBJMETODOS_CLONAR_DEFINICIONCOMPLETA_NULL
};

#endif
