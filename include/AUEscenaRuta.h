//
//  AUBasura.h
//  Gameplay_iOS
//
//  Created by Marcos Ortega Morales on 27/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaRuta_H_INCLUIDO
#define AUEscenaRuta_H_INCLUIDO

#include "AUEscenaObjeto.h"

struct STRutaVertice {
	NBPunto posicion;
	float	factorVelocidad;	//Factor de velocidad en el vertice
	float	segundossEspera;	//Segundos espera en el vertice
	//
	float	radianesSegmento;		//[En secuencia: Angulo del segmento que inicia desde este vertice] [En circulo: angulo desde el primer vertice hacia este]
	float	longitudSegmento;		//[En secuencia: Longitud del segmento que inicia desde este vertice] [En circulo: longitud desde el primer vertice hacia este]
	float	longitudSegmentosAnteriores;
	NBPunto	vecUnitSegmento;
	//
	bool operator==(const STRutaVertice &otro) const {
		return (posicion == otro.posicion);
	}
	bool operator!=(const STRutaVertice &otro) const {
		return !(posicion == otro.posicion);
	}
};

class AUEscenaRuta: public AUEscenaObjeto {
	public:
		AUEscenaRuta();
		AUEscenaRuta(AUEscenaRuta* otra);
		virtual ~AUEscenaRuta();
		//
		float					longitudTotalRuta();
		float					longitudTotalRutaCalculada();
		AUArregloNativoP<STRutaVertice>* verticesRuta();
		NBPunto					posicionEnRutaDeAvance(const float avanceEnRuta);
		STRutaVertice			verticeEnAvance(const float avanceEnRuta);
		//
		void					agregarVerticeRuta(float xPosEnContenedorPadre, float yPosEnContenedorPadre, float factorVelocidad, float segundosEspera);
		void					agregarVerticeEnPuntoMedioSegmentoRuta(UI16 iVerticeSegmentoInicia);
		void					actualizarVerticeRuta(UI16 iVertice, float xPosEnContenedorPadre, float yPosEnContenedorPadre, float factorVelocidad, float segundosEspera);
		void					quitarVerticeRutaEnIndice(UI16 iVertice);
		void					vaciarVerticesRuta();
		//
		virtual	NBCajaAABB		cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL	actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		//guardar y cargar en XML
		static bool				agregarXmlInternoEn(AUEscenaRuta* ruta, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool				interpretarXmlInterno(AUEscenaRuta* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool				agregarBitsInternosEn(AUEscenaRuta* ruta, AUArchivo* guardarEn);
		static bool				interpretarBitsInternos(AUEscenaRuta* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		bool					_rutaCajaLocalSucia;
		NBCajaAABB				_rutaCajaLocal;
		float					_longitudTotalRecorrido;
		AUArregloNativoMutableP<STRutaVertice> _verticesRuta;
		//
		void					privActualizarCacheVerticesRuta();
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		bool					debugVeritificarCacheDeRuta();
		#endif
};

#endif
