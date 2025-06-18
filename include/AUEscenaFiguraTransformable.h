//
//  AUFijacion.h
//  EditorCuerpos
//
//  Created by Nicaragua Binary on 30/10/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AU_ESCENAFIGURATRANSFORMABLE_H_INCLUIDO
#define AU_ESCENAFIGURATRANSFORMABLE_H_INCLUIDO

#define AU_FIJACION_TAMANO_CAJA_PUNTERO		8.0f

#include "NBPunto.h"
#include "AUEscenaFigura.h"
#include "NBGestorEscena.h"

struct STFiguraTTRender {
	ENEscenaFiguraTipo	tipo;
	bool				dibujarFiguraBase;
	bool				dibujarEscalaH;
	bool				dibujarEscalaV;
	bool				dibujarReferenciasInternas;
	bool				dibujarAngulo;
	bool				dibujarCentro;
	bool				dibujarVertices;
	SI32				cantidadCoordenadasLocales;
	STBloqueGL			bloqueVerticesGL;
};

class AUEscenaFiguraTransformable : public AUEscenaFigura {
	public:
		AUEscenaFiguraTransformable(ENEscenaFiguraTipo tipo);
		AUEscenaFiguraTransformable(AUEscenaFiguraTransformable* otro);
		virtual ~AUEscenaFiguraTransformable();
		//
		void					sobreIndicadorEscala(float xEscena, float yEscena, bool* guardarEnEscalaHorizontalEn, bool* guardarEnEscalaVerticalEn);
		void					resaltarCentro(bool resaltado);
		bool					centroResaltado();
		void					resaltarAngulo(bool resaltado);
		bool					anguloResaltado();
		void					resaltarEscalaHorizontal(bool resaltada);
		bool					escalaHorizontalResaltada();
		void					resaltarEscalaVertical(bool resaltada);
		bool					escalaVerticalResaltada();
		void					resaltarVertice(int indice);
		int						indiceVerticeResaltado();
		void					seleccionarVertice(SI32 indiceSelecionar);
		int						indiceVerticeSelecionado();
		void					establecerColorVertices(UI8 r, UI8 g, UI8 b, UI8 a);
		void					establecerColorVerticesResaltado(UI8 r, UI8 g, UI8 b, UI8 a);
		void					establecerColorVerticesSeleccionado(UI8 r, UI8 g, UI8 b, UI8 a);
		void					dibujarFiguraBase(bool dibujar);
		void					dibujarEscalaH(bool dibujar);
		void					dibujarEscalaV(bool dibujar);
		void					dibujarAngulo(bool dibujar);
		void					dibujarCentro(bool dibujar);
		void					dibujarReferenciasInternas(bool dibujar);
		void					dibujarVertices(bool dibujar);
		bool					dibujandoFiguraBase();
		bool					dibujandoEscalaH();
		bool					dibujandoEscalaV();
		bool					dibujandoAngulo();
		bool					dibujandoCentro();
		bool					dibujandoReferenciasInternas();
		bool					dibujandoVertices();
		//
		virtual PTRfunCmdsGL	actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void				enviarComandosGL(UI8* punteroDatosModelo);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		//
		SI16					_indiceVerticeSel;
		NBColor					_colorVertices;
		NBColor					_colorVerticesReslatado;
		NBColor					_colorVerticeSeleccionado;
		NBColor					_colorTransformaciones;
		NBColor					_colorRefInternas;
		bool					_dibujarFiguraBase;		//Solo las partes de la FiguraTransformables o tambien las heredadas
		bool					_dibujarReferenciasInternas;
		bool					_dibujarVertices;
		bool					_dibujarEscalaH;
		bool					_dibujarEscalaV;
		bool					_dibujarAngulo;
		bool					_dibujarCentro;
		bool					_centroResaltado;
		bool					_anguloResaltado;
		bool					_escalaHorizontalResaltada;
		bool					_escalaVerticalResaltada;
		int						_indiceVerticeReslatado;
	private:
		void					inicializarVariables();
		int						inlineVerticesGLNecesarios();
		void					inlineActualizarModelo(const NBPropRenderizado &propsRenderizado, const STBloqueGL &bloqueVerticesGL);
		void					inlineAplicarColoresEspecificosModelo(NBVerticeGL* verticesGL);
		int						inlineEstablecerColoresVertices(NBVerticeGL* verticesGL, int indicePrimero, int cantidad, const NBColor &colorDestino);
		int						estableceVerticesCaja(NBVerticeGL* arregloVertsGL, float centroGLX, float centroGLY, float anchoGL, float altoGL);
		int						estableceVerticesCruz(NBVerticeGL* arregloVertsGL, float centroGLX, float centroGLY, float anchoGL, float altoGL);
		int						establecerVerticesFlecha(NBVerticeGL* arregloVertsGL, float centroGLX, float centroGLY, float anchoGL, float altoGL, float angulo);
};

typedef AUEscenaFiguraTransformable AUEscenaFiguraTT;


#endif
