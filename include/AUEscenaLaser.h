#ifndef AUESCENASLASER_H_INCLUIDO
#define AUESCENASLASER_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "NBRectangulo.h"
#include "AUTextura.h"
#include "NBGestorGL.h"
#include "NBGestorTexturas.h"
#include "NBColor.h"
#include "NBMatriz.h"
#include "AUArchivo.h"
#include "NBGestorEscena.h"
#include "AUEscenaCuerda.h"
#include "AUEscenaLaserDef.h"

//Para renderizar un sprite no-segmentado
struct STEscenaLaserRender {
	UI32		idTexturaGL;
	UI32		conteoSegmentos;
	STBloqueGL	bloqueIndicesGL;
};

struct STEscenaLaserColFisica {
	float		fraccion;
	void*		cuerpo; //b2Body*
	NBPunto		posFisica;
	NBPunto		normal;
};

class AUEscenaLaser;

class IEscenaLaserEscuchador {
	public:
		IEscenaLaserEscuchador() { }
		virtual ~IEscenaLaserEscuchador() { }
		//
		virtual void		laserEscenaTickObstruccionPorFisica(AUEscenaLaser* objEscenaLaser, const float segsTranscurridos, /*b2Body*/ void* objCuerpo, const NBPunto &posEscena, const NBPunto &vecNormal) = 0;
		virtual void		laserEscenaTickObstruccionPorCuerda(AUEscenaLaser* objEscenaLaser, const float segsTranscurridos, ICuerdaCortable* objCuerda, const NBPunto &posEscena, const STCuerdaCorte &propsColision) = 0;
};

class AUEscenaLaser : public AUEscenaObjeto, public IEscenaConsumidorEspejos, public IEscenaConsumidorCuerdas, public IEscenaConsumidorPreModeloGL, public NBAnimador
#ifndef CONFIG_NB_UNSUPPORT_BOX2D
, public b2RayCastCallback
#endif
{
	public:
		AUEscenaLaser(const float alcanceLaser, const float anchoLaser);
		AUEscenaLaser(AUEscenaLaser* otra);
		virtual ~AUEscenaLaser();
		//
		float				laserAlcance() const;
		float				laserAncho() const;
		NBPunto				laserVelDesplazaTex() const;
		bool				laserColisionIgnorarPrimera() const;
		bool				laserColisionaConFisica() const;
		bool				laserColisionaConCuerdas() const;
		//
		void				establecerLaserEscuchador(IEscenaLaserEscuchador* _laserEscuchador);
		void				establecerLaserAlcance(const float alcance);
		void				establecerLaserAncho(const float ancho);
		void				establecerLaserVelDesplazaTex(const float velX, const float velY);
		void				establecerLaserVelDesplazaTex(const NBPunto &velDesplazaTex);
		void				establecerLaserColisionIgnorarPrimera(const bool ignorarPrimeraColision);
		void				establecerLaserColisionaConFisica(const bool colisionaFisica);
		void				establecerLaserColisionaConCuerdas(const bool colisionaCuerdas);
		//
		virtual void		consumirEspejos(const NBFuenteEspejoRender* arrEspejos, const UI16 conteoEspejos);
		virtual void		consumirCuerdas(ICuerdaCortable** cuerdas, const UI16 conteoCuerdas);
		virtual void		consumirAntesDeCrearModeloGL();
#		ifndef CONFIG_NB_UNSUPPORT_BOX2D
		virtual float32		ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction);
#		endif
		virtual void		tickAnimacion(float segsTranscurridos);
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		//Para renderizar
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
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		bool				_dbgLaserRegistradoComoRayo;
		#endif
		bool				_laserColisionIgnorarPrimera;
		bool				_laserColisionaConFisica;
		bool				_laserColisionaConCuerdas;
		float				_laserAlcance;
		float				_laseAncho;
		AUTextura*			_laserTexturaPatron;
		NBPunto				_laserTexDesplazaVel;
		float				_laserObstruccionFraccion;
		IEscenaLaserEscuchador* _laserEscuchador;
		//Deteccion de colision con cuerdas
		ICuerdaCortable*	_colisionCuerdaConsumir;
		STCuerdaCorte		_colisionCuerdaDatos;
		//Deteccion de colision con fisica
		float				_colisionFraccionBase;
		STEscenaLaserColFisica _colisionFisicaDatos[2];
		//
		NBPunto				_laserTexDesplaza;
		AUArregloNativoMutableP<STEscenaLaserPos> _laserTrayectoriaEscena;
		
};

#endif
