//
//  AUEscenaGrupoParticulas.h
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUESCENAGRUPOPARTICULAS_H_INCLUIDO
#define AUESCENAGRUPOPARTICULAS_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "NBAnimador.h"
#include "NBGestorAnimadores.h"
#include "NBGestorFisica.h"
#include "AUEscenaGrupoParticulasDef.h"

enum ENParticulaTipo {	//El orden en esta enumeracion debe conservarse (los archivos XML y Binarios guardan el valor numerico)
	ENParticulaTipo_Libre = 0,
	ENParticulaTipo_Hojas,
	ENParticulaTipo_Gotas,
	ENParticulaTipo_Humo,
	ENParticulaTipo_Inmovil, //Particula que se coloca y no se mueve (solo desaparece, ideal para rastros)
	ENParticulaTipo_Conteo
};

#ifndef CONFIG_NB_UNSUPPORT_BOX2D
struct STParticulaColision {
	b2Fixture*	fixture;
	b2Vec2		point;
	b2Vec2		normal;
	float32		fraction;
};
#endif

struct STParticulaGrupoGL { //Para optimizar el renderizado, agrupando por textura
	SI16	idTexturaGL;
	UI16	conteoParticulas;
	AUArregloNativoMutableP<STParticulaProps>* particulas;
	//
	bool operator==(const STParticulaGrupoGL &otro) const {
		return (idTexturaGL==otro.idTexturaGL);
	}
	bool operator!=(const STParticulaGrupoGL &otro) const {
		return !(idTexturaGL==otro.idTexturaGL);
	}
};

struct STParticulaGrupoAU { //Para optimizar la piscina de retencion del renderizador, cada textura solo se retiene una vez en cada ciclo de renderizado
	AUTextura*	objTextura;
	UI16	conteoParticulas;
	bool operator==(const STParticulaGrupoAU &otro) const {
		return (objTextura==otro.objTextura);
	}
	bool operator!=(const STParticulaGrupoAU &otro) const {
		return !(objTextura==otro.objTextura);
	}
};

struct STParticulaIndice {
	SI16		iGrupoGL;		//Indice de grupoGL
	SI16		iParticula;		//Indice de las propiedades "STParticulaProps"	
	//
	bool operator==(const STParticulaIndice &otro) const {
		return (iGrupoGL==otro.iGrupoGL || iParticula==otro.iParticula);
	}
	bool operator!=(const STParticulaIndice &otro) const {
		return !(iGrupoGL==otro.iGrupoGL || iParticula==otro.iParticula);
	}
};

//Particula que no se traslada, solo desaparece
struct STParticulaAnimInmovil {
	STParticulaIndice indice;
	float		segundosViva;
	float		segundosVive;
	float		segundosDesapareciendo;
	float		segundosDesaparece;
	float		escalaInicial;
	float		escalaDesaparece;
	//
	bool operator==(const STParticulaAnimInmovil &otro) const {
		return (indice==otro.indice);
	}
	bool operator!=(const STParticulaAnimInmovil &otro) const {
		return !(indice==otro.indice);
	}
};

//Particula que rota y se traslada,
//es afectada por la gravedad,
//no detecta colision de fisica,
//desaparece en poco tiempo.
struct STParticulaAnimLibre {
	STParticulaIndice indice;
	float		segundosViva;
	float		segundosVive;
	float		segundosDesapareciendo;
	float		segundosDesaparece;
	float		velRadAngular;
	NBPunto		velLineal;
	//
	bool operator==(const STParticulaAnimLibre &otro) const {
		return (indice==otro.indice);
	}
	bool operator!=(const STParticulaAnimLibre &otro) const {
		return !(indice==otro.indice);
	}
};

//Particula que rota y se traslada,
//es afectada por la gravedad y el viento,
//no detecta colision de fisica,
//desaparece en poco tiempo.
struct STParticulaAnimHoja {
	STParticulaIndice indice;
	UI8			polarN;			//Parametro para calcular la coordenada polar
	UI8			polarD;			//Parametro para calcular la coordenada polar
	float		factorResistenciaAire;		//Freno ante la gravedad y factor de afectacion del viento
	float		factorTrayectoriaIrregular;	//Factor de radio polar
	float		radianesPolar;
	float		segundosViva;
	float		segundosVive;
	float		segundosDesapareciendo;
	float		segundosDesaparece;
	float		velRadAngular;
	NBPunto		posSinPolar;	//posicion real de la hoja (a esta se le suma un desplazamiento polar)
	NBPunto		posPolar;
	NBPunto		velLinealGravedad;
	NBPunto		velLinealViento;
	//
	bool operator==(const STParticulaAnimHoja &otro) const {
		return (indice==otro.indice);
	}
	bool operator!=(const STParticulaAnimHoja &otro) const {
		return !(indice==otro.indice);
	}
};

//Particula de lluvia rota hacia donde se dirige
//es afectada por la gravedad y el viento,
//periodicamente se detecta colision con fisica,
//cambia de textura y desparece al detectar colision.
struct STParticulaAnimGota {
	STParticulaIndice indice;
	AUTextura*	texturaImpacto; //Textura de impacto
	bool		impactada;
	float		segundosDesapareciendo;
	float		segundosDesaparece;
	NBPunto		velLineal;
	float		segsparaCalcularFisica;
	float		segsAcumSinCalcularFisica;
	//
	bool operator==(const STParticulaAnimGota &otro) const {
		return (indice==otro.indice);
	}
	bool operator!=(const STParticulaAnimGota &otro) const {
		return !(indice==otro.indice);
	}
};

//Particula que flota, rota y se disipa
//es afectada por la gravedad (invertida) y el viento.
//no detecta colision de fisica,
struct STParticulaAnimHumo {
	STParticulaIndice indice;
	float		escalaInicial;				//Escala inicial a mostrar la textura
	float		escalaParaDisipacion;		//Multiplicador escala inicial, en el que la particula desaparece
	float		factorResistenciaAire;		//Freno ante la gravedad y factor de afectacion del viento
	float		segundosViva;
	float		segundosVive;
	float		velRadAngular;
	NBPunto		velLinealGravedad;
	NBPunto		velLinealViento;
	//
	bool operator==(const STParticulaAnimHumo &otro) const {
		return (indice==otro.indice);
	}
	bool operator!=(const STParticulaAnimHumo &otro) const {
		return !(indice==otro.indice);
	}
};

struct STGrupoParticulasRender {
	SI32		cantGruposGL;
	STBloqueGL	bloqueIndicesGL;
};

struct STGrupoParticulasRenderGrupo {
	UI32		idTexturaGL;
	UI32		cantModelos;
};

/*
 Pruebas de rendimiento para diferente tamano de caja en la grilla:
 Ciclo a: 60FPS, 750 gotas, 0.6 radio liquido, 0.05 radio solido, 0.004 biscosidad
 tamCaja = (AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO * 2.0f): consumen 80% del tiempo, comportamiento realista (OK)
 tamCaja = (AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO / 1.0f): consumen 62% del tiempo, comportamiento realista (OK)
 tamCaja = (AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO / 1.33f): consumen 55% del tiempo, comportamiento realista (OK - IDEAL)
 tamCaja = (AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO / 1.5f): consumen 52% del tiempo, comportamiento realista (un poco tembloroso irregular)
 tamCaja = (AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO / 2.0f): consumen 48% del tiempo, comportamiento no-realista (muy tembloroso irregular)
 */

class AUEscenaGrupoParticulas: public AUEscenaObjeto, public NBAnimador
#ifndef CONFIG_NB_UNSUPPORT_BOX2D
, public b2RayCastCallback
#endif
{
	public:
		AUEscenaGrupoParticulas(float xMin, float yMin, float xMax, float yMax);
		virtual ~AUEscenaGrupoParticulas();
		//
		UI16				conteoParticulasInmovil();
		UI16				conteoParticulasLibres();
		UI16				conteoParticulasHoja();
		UI16				conteoParticulasGotas();
		UI16				conteoParticulasHumo();
		//
		void				agregarParticulaInmovil(AUTextura* textura, float segundosVive, float segundosDesaparece, float posX, float posY, float escala, float radianes, float escalaDesaparece);
		void				agregarParticulaLibre(AUTextura* textura, float segundosVive, float segundosDesaparece, float posX, float posY, float escala, float radianes, float velX, float velY, float velradianes);
		void				agregarParticulaHoja(AUTextura* textura, float segundosVive, float segundosDesaparece, float posX, float posY, float escala, float radianes, float velX, float velY, float velradianes, float factorResistenciaAire, float factorTrayectoriaIrregular);
		void				agregarParticulaGota(AUTextura* texturaGota, AUTextura* texturaImpacto, float segundosDesaparece, float posX, float posY, float escala, float velX, float velY, float segsParaCalcularFisica);
		void				agregarParticulaHumo(AUTextura* textura, float segundosDesaparece, float posX, float posY, float escala, float radianes, float velX, float velY, float velradianes, float escalarParaDisipar, float factorResistenciaAire);
		//
		virtual void		tickAnimacion(float segsTranscurridos);
#		ifndef CONFIG_NB_UNSUPPORT_BOX2D
		virtual float32		ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);
#		endif
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void			enviarComandosGL(UI8* punteroDatosModelo);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		virtual void		agregadoEnEscena();
		virtual void		quitandoDeEscena();
	private:
		bool											_partsCajaLocalSucia;
		UI16											_conteoParticulas;
		NBCajaAABB										_partsCajaLocal;
		NBCajaAABB										_cajaPersistenciaParticulas;
		AUArregloNativoMutableP<STParticulaGrupoGL>		_gruposPorTexturaGL;
		AUArregloNativoMutableP<STParticulaGrupoAU>		_gruposPorTexturaAU;
		AUArregloNativoMutableP<STParticulaAnimInmovil>	_datosAnimInmovil;
		AUArregloNativoMutableP<STParticulaAnimLibre>	_datosAnimLibre;
		AUArregloNativoMutableP<STParticulaAnimHoja>	_datosAnimHoja;
		AUArregloNativoMutableP<STParticulaAnimGota>	_datosAnimGota;
		AUArregloNativoMutableP<STParticulaAnimHumo>	_datosAnimHumo;
#		ifndef CONFIG_NB_UNSUPPORT_BOX2D
		STParticulaColision								_resultadosColision;
#		endif
		//
		inline STParticulaIndice privInlineRegistrarParticula( AUTextura* textura, SI8 alineaTexH, SI8 alineaTexV, float posX, float posY, float escala, float radianes, float alpha);
		inline void			privInlineLiberarParticula(SI16 iGrupoGL, SI16 iParticula, SI16 idTexturaGL); //Se pasa explicito el idTexturaGL, para poder eliminar del grupo de precarga (indice 0, idTexturaGL 0) las texturas que fueron cargadas posteriormente
		inline SI16			privInlineRegistrarTextura(AUTextura* textura);
		inline void			privInlineLiberarTextura(AUTextura* textura, SI16 idTexturaGL); 
};

#endif
