//
//  AUFijacion.h
//  EditorCuerpos
//
//  Created by Nicaragua Binary on 30/10/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AU_ESCENAFIGURA_H_INCLUIDO
#define AU_ESCENAFIGURA_H_INCLUIDO

#define AU_FIJACION_VERTICES_MAXIMO				16		//El maximo posible, Box2D soporta por defecto maximo 8 vertices
#define AU_FIJACION_TAMANO_CAJA_PUNTERO			8.0f

#include "AUEscenaObjeto.h"
#include "AUTextura.h"
#include "NBGestorTexturas.h"
#include "NBPuntoColor.h"

#define AUESCENAFIGURA_VALOR_VERIFICACION_BINARIO	419

#define AUESCENAFIGURA_ES_PATRON_GLOBAL				1
#define AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL			2
#define AUESCENAFIGURA_ENSUCIAR_MODELO				_mascaraPropiedades |= (AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL);
#define AUESCENAFIGURA_ENSUCIAR_MODELO_DE(PTR_FIG)	PTR_FIG->_mascaraPropiedades |= (AUESCENAFIGURA_BIT_SUCIO_CAJA_LOCAL);

enum ENEscenaFiguraTipo {
	ENEscenaFiguraTipo_Linea			= 0,
	ENEscenaFiguraTipo_Circulo			= 1,
	ENEscenaFiguraTipo_PoligonoCerrado	= 2,
	ENEscenaFiguraTipo_SecuenciaLineas	= 3,
};

enum ENEscenaFiguraModo {
	ENEscenaFiguraModo_Borde,
	ENEscenaFiguraModo_Relleno
};

struct STFiguraRender {
	ENEscenaFiguraTipo	tipo;
	ENEscenaFiguraModo	modo;
	UI16				conteoVertices;
	UI32				idTexturaGL;
	STBloqueGL			bloqueVerticesGL;
	STBloqueGL			bloqueIndicesGL;
};

class AUEscenaFigura : public AUEscenaObjeto {
	public:
		AUEscenaFigura(const ENEscenaFiguraTipo tipo);
		AUEscenaFigura(AUEscenaFigura* otro);
		virtual ~AUEscenaFigura();
		//
		AUTextura*							texturaPatron();
		bool								coordenadasTexturasSonGlobal();
		void								establecerTexturaPatron(AUTextura* nuevaTextura, const bool basadaEnCoordsGlobal/*, const NBMatriz &matrizTransformaciones*/);
		ENEscenaFiguraTipo					tipo();
		ENEscenaFiguraModo					modo();
		AUArregloNativoP<NBPuntoColor>*		puntosLocales();
		bool								esPoligonoConvexoContraReloj();
		//
		void								establecerTipo(const ENEscenaFiguraTipo tipo);
		void								establecerModo(const ENEscenaFiguraModo modo);
		void								establecerMaximoVertices(const UI8 maxVertices);
		bool								agregarCoordenadaLocal(const float xLocal, const float yLocal);
		bool								agregarCoordenadaLocal(const float xLocal, const float yLocal, const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		bool								agregarCoordenadaLocal(const float xLocal, const float yLocal, const NBColor8 color);
		bool								agregarCoordenadaLocal(const NBPuntoColor &puntoColor);
		bool								agregarCoordenadaLocalEnIndice(const UI16 indice, const NBPuntoColor &puntoColor);
		bool								agregarCoordenadaLocalEnIndice(const UI16 indice, const float xLocal, const float yLocal, const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		bool								agregarCoordenadaLocalEnIndice(const UI16 indice, const float xLocal, const float yLocal, const NBColor8 color);
		bool								agregarCoordenadaEnPuntoMedioDeSegmento(const UI32 indiceVerticeIniSegmento);
		void								quitarCoordenadaLocalEnIndice(const UI32 indice);
		void								establecerCoordenadasComoCaja(const float centroLocalX, const float centroLocalY, const float ancho, const float alto);
		void								escalarCoordenadasDesdeCentroLocal(const float centroLocalX, const float centroLocalY, const float escalaAncho, const float escalaAlto);
		void								moverVerticeHacia(const UI32 indice, const float xLocal, const float yLocal);
		void								colorearVertice(const UI32 indice, const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		SI32								indiceVerticeMasCercano(const float xLocal, const float yLocal, float distanciaAbsMaxima);
		//
		virtual	NBCajaAABB					cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL				actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void							enviarComandosGL(UI8* punteroDatosModelo);
		//guardar y cargar en XML
		static bool							agregarXmlInternoEn(AUEscenaFigura* figura, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool							interpretarXmlInterno(AUEscenaFigura* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool							agregarBitsInternosEn(AUEscenaFigura* figura, AUArchivo* guardarEn);
		static bool							interpretarBitsInternos(AUEscenaFigura* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		UI8									_mascaraPropiedades;
		UI8									_tipo;
		UI8									_modo;
		UI8									_maxVertices;
		AUTextura*							_texturaPatron;	
		NBCajaAABB							_cacheCajaLocalCalculada;
		AUArregloNativoMutableP<NBPuntoColor> _coordenadasLocales;
		virtual void						agregadoEnEscena();
		virtual void						quitandoDeEscena();
	private:
		void								inicializarVariables();
		BOOL								privValidarVerticesCircunferencia();
};


#endif
