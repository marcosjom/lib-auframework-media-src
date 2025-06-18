//
//  AUEscenaGrupoParticulasFuego.h
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaGrupoParticulasFuego_H_INCLUIDO
#define AUEscenaGrupoParticulasFuego_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "NBAnimador.h"
#include "NBGestorFisica.h"
#include "NBGestorTexturas.h"
#include "NBGestorAnimadores.h"

//#define AUGRUPOPARTICULASFUEGO_DIBUJAR_INFO_DEBUG

struct STParticulaFuegoProps {
	UI8			iTextura;
	float		escala;
	float		radianes;
	NBPunto		posicionEscena;
	NBColor		color;
	NBPunto		esquinaMinSprite;
	NBPunto		esquinaMaxSprite;
	//
	NBPunto		vectVelocidad;		//Velocidad de movimiento
	float		velAngular;			//Velocidad de rotacion
	float		alcanceParticula;	//Alcance de la particula fuego
	float		segundosVive;		//Segundos que debe vivir
	float		segundosHaVivido;	//Segundos que ha vivido
	//
	bool operator==(const STParticulaFuegoProps &otro) const {
		return (iTextura==otro.iTextura && posicionEscena==otro.posicionEscena && radianes==otro.radianes && color==otro.color);
	}
	bool operator!=(const STParticulaFuegoProps &otro) const {
		return !(iTextura==otro.iTextura && posicionEscena==otro.posicionEscena && radianes==otro.radianes && color==otro.color);
	}
};

struct STParticulaFuegoLineaI {
	SI16 iParticulaInicial;
	SI16 iParticulaFinal;
	//
	bool operator==(const STParticulaFuegoLineaI &otro) const {
		return (iParticulaInicial==otro.iParticulaInicial && iParticulaFinal==otro.iParticulaFinal);
	}
	bool operator!=(const STParticulaFuegoLineaI &otro) const {
		return !(iParticulaInicial==otro.iParticulaInicial && iParticulaFinal==otro.iParticulaFinal);
	}
};

struct STParticulaFuegoLinea {
	bool lineaEstablecida;
	NBPunto posEscenaIni;
	NBPunto posEscenaFin;
	//
	bool operator==(const STParticulaFuegoLinea &otro) const {
		return (lineaEstablecida==otro.lineaEstablecida && posEscenaIni==otro.posEscenaIni && posEscenaFin==otro.posEscenaFin);
	}
	bool operator!=(const STParticulaFuegoLinea &otro) const {
		return !(lineaEstablecida==otro.lineaEstablecida && posEscenaIni==otro.posEscenaIni && posEscenaFin==otro.posEscenaFin);
	}
};
		
struct STGrupoParticulasFuegoRender {
	UI32		idTexturaGL[2];
	UI16		conteoParticulasTex[2];
	STBloqueGL	bloqueIndicesGL;
	#ifdef AUGRUPOPARTICULASFUEGO_DIBUJAR_INFO_DEBUG
	STBloqueGL	bloqueVerticesGLDebug;
	#endif
};
	
class AUEscenaGrupoParticulasFuego: public AUEscenaObjeto, public NBAnimador {
	public:
		AUEscenaGrupoParticulasFuego();
		virtual ~AUEscenaGrupoParticulasFuego();
		//
		float				alcanceParticulas();
		float				particulasPorSegundo();
		UI16				conteoParticulasFuegoVivas();
		void				establecerAlcanceParticulas(float alcanceParticulas);
		void				establecerParticulasPorSegundo(float particulasPorSegundo);
		void				establecerParticulasSegsVida(float segsVida);
		//
		void				activarSiguienteLineaParticulas();
		void				lineasParticulas(STParticulaFuegoLinea* guardarLinea1En, STParticulaFuegoLinea* guardarLinea2En);
		//
		virtual void		tickAnimacion(float segsTranscurridos);
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void			enviarComandosGL(UI8* punteroDatosModelo);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUTextura*			_texturasParticulas[2];
		NBTamano			_tamTexParticulas[2];
		AUArregloNativoMutableP<STParticulaFuegoProps>	_particulasFuego;	//Pendiente, agrupar las particulas por textura, para renderizarlas todas juntas en lugar de una por una.
		float				_genParticulasAcumPendientes;
		float				_genParticulasPorSegundo;
		float				_genParticulasAlcance;
		float				_genParticulasSegsVida;
		//Linea de fuego y cache
		bool				_partsCajaLocalSucia;
		UI8					_iLineaFuegoActual;
		bool				_lineaFuegoActualInicializada;
		STParticulaFuegoLineaI _lineaFuego[2];
		NBCajaAABB			_partsCajaLocal;
		//
		virtual void		agregadoEnEscena();
		virtual void		quitandoDeEscena();
};

#endif
