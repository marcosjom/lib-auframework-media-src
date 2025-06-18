#ifndef AUESCENALUZSENSOR_H_INCLUIDO
#define AUESCENALUZSENSOR_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "NBRectangulo.h"
#include "AUTextura.h"
#include "NBGestorGL.h"
#include "NBGestorTexturas.h"
#include "NBColor.h"
#include "NBMatriz.h"
#include "AUArchivo.h"
#include "NBSegmentadorFiguras.h"
#include "NBGestorEscena.h"

class AUEscenaLuzSensor : public AUEscenaObjeto, public IEscenaConsumidorLuces {
	public:
		AUEscenaLuzSensor();
		AUEscenaLuzSensor(AUEscenaLuzSensor* otra);
		virtual ~AUEscenaLuzSensor();
		//
		NBColor				ultimoColorPorLuz();
		float				ultimoPromedioRGBPorLuz();
		//
		virtual void		consumirLuces(const NBPropIluminacion &propsIluminacion);
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		//Para renderizar sprites no-segmentadas
		virtual PTRfunCmdsGL actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		NBColor				_ultColorAcum;
		float				_ultPromedioIluminado;	//Promedio de componentes R-G-B del ultimo color iluminado
		//
		virtual void		agregadoEnEscena();
		virtual void		quitandoDeEscena();
};

#endif
