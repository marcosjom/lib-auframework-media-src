//
//  AUEscenaMallaConHuesoAnimada.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 24/01/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaMallaConHueso_H_INCLUIDO
#define AUEscenaMallaConHueso_H_INCLUIDO

#include "AUEscenaMalla.h"

//Ver el documento (Skeletal Deformation): http://www.cs.sfu.ca/~torsten/Teaching/Cmpt466/LectureNotes/PDF/07_deformations.pdf

struct STMallaConHuesoVertice {
	NBPunto verticeNormal;					//Posicion del vertice en estado normal
	//Datos de deformacion
	SI32	indiceHueso;					//Si es cero o negativo el vertice no tiene hueso asociado.
	float	factorAvancePolilinea;			//Entre cero y uno.
	float	distanciaBisectorIni;			//
	float	distanciaBisectorFin;			//
	float	anguloAperturaBisectorIni;		//
	float	anguloAperturaBisectorFin;		//
	bool operator==(const STMallaConHuesoVertice &otro) const {
		return (verticeNormal==otro.verticeNormal);
	}
	bool operator!=(const STMallaConHuesoVertice &otro) const {
		return !(verticeNormal==otro.verticeNormal);
	}
};

struct STMallaConHuesoArticulacion {
	NBPunto posicionReposo;		//Coordenada local de la articulacion en reposo (natural)
	NBPunto posicionActual;		//Coordenada local de la articulacion actual (forzada)
	//
	bool operator==(const STMallaConHuesoArticulacion &otro) const {
		return (posicionReposo==otro.posicionReposo && posicionActual==otro.posicionActual);
	}
	bool operator!=(const STMallaConHuesoArticulacion &otro) const {
		return !(posicionReposo==otro.posicionReposo && posicionActual==otro.posicionActual);
	}
};

//Si defined la malla se compartara como un AUEscenaContenedor y agregará hijos que repreentan informacion DEBUG
//#define AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG

class AUEscenaMallaConHueso: public AUEscenaMalla {
	public:
		AUEscenaMallaConHueso();
		AUEscenaMallaConHueso(UI8 columnas, UI8 filas, AUArregloNativoP<NBPuntoColor>* verticesLocales, AUArregloNativoP<AUTextura*>* texturas, AUArregloNativoP<NBRectangulo>* areasTex);
		AUEscenaMallaConHueso(AUEscenaMallaConHueso* otraInstancia);
		virtual ~AUEscenaMallaConHueso();
		//
		void								agregarArticulacion(float posX, float posY);
		void								agregarArticulacionEnIndice(UI32 indice, float posX, float posY);
		void								agregarArticulacionEnPuntoMedioDeHueso(UI32 indiceArticulacionIniHueso);
		void								agregarHuesosEnLinea(float xIniLinea, float yIniLinea, float xFinLinea, float yFinLinea, UI8 cantidadHuesos);
		NBPunto								verticeNormal(UI32 indice);
		void								moverVerticeNormal(UI32 indice, float posX, float posY);
		void								moverArticulacionReposo(UI32 indice, float posX, float posY);
		void								moverArticulacion(UI32 indice, float posX, float posY);
		void								restablecerPosicionArticulaciones();
		void								quitarArticulacionEnIndice(UI32 indice);
		void								vaciarArticulaciones();
		AUArregloNativoP<STMallaConHuesoVertice>*	datosVertices();
		AUArregloNativoP<STMallaConHuesoArticulacion>* datosHuesos();
		//
		static AUEscenaMallaConHueso*		crearMallaConHuesoSprite(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaSprite, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH=ENEscenaMallaAlineacionH_Centro, ENEscenaMallaAlineacionV alineacionV=ENEscenaMallaAlineacionV_Centro);
		static AUEscenaMallaConHueso*		crearMallaConHuesoPatron(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaRepetir, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH=ENEscenaMallaAlineacionH_Centro, ENEscenaMallaAlineacionV alineacionV=ENEscenaMallaAlineacionV_Centro);
		static AUEscenaMallaConHueso*		crearMallaConHuesoDesdeArchivoXml(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaArchivoMalla, ENEscenaMallaAlineacionH alineacionH=ENEscenaMallaAlineacionH_Centro, ENEscenaMallaAlineacionV alineacionV=ENEscenaMallaAlineacionV_Centro);
		//
		virtual PTRfunCmdsGL				actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		//guardar y cargar en XML
		static bool							agregarXmlInternoEn(AUEscenaMallaConHueso* malla, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool							interpretarXmlInterno(AUEscenaMallaConHueso* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool							agregarBitsInternosEn(AUEscenaMallaConHueso* malla, AUArchivo* guardarEn);
		static bool							interpretarBitsInternos(AUEscenaMallaConHueso* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		bool													_cacheDefinicionHuesosSucia;
		bool													_cacheHuesosMovidos;
		AUArregloNativoMutableP<STMallaConHuesoVertice>			_verticesConHuesos;
		AUArregloNativoMutableP<STMallaConHuesoArticulacion>	_datosHuesos;
		#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
		AUArregloMutable*					_debugVerticesReposo;
		AUArregloMutable*					_debugVerticesActuales;
		AUArregloMutable*					_debugVerticesInterPre;
		AUArregloMutable*					_debugVerticesInterPost;
		AUArregloMutable*					_debugUnionesVerticesArtics;
		AUArregloMutable*					_debugLineasBisectoras;
		#endif
		//
		inline void							privInlineCalcularDatosVerticesAnteHuesos();
		inline void							privInlineActualizarMallaSegunHuesos();
		#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
		inline void							privInlineAgregarFigurasDebugParaVertice();
		inline void							privInlineAgregarFigurasDebugParaArticulacion();
		#endif
};

#endif
