//
//  AUEscenaMalla.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 24/01/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaMalla_H_INCLUIDO
#define AUEscenaMalla_H_INCLUIDO

#define AUESCENAMALLA_BIT_SUCIO_MODELO_LOCAL		1
#define AUESCENAMALLA_BIT_SUCIO_CAJA_LOCAL			2
#define AUESCENAMALLA_ENSUCIAR_MODELO				_mascaraCacheSuciasMalla = 0xFF;
#define AUESCENAMALLA_ENSUCIAR_MODELO_DE(PTR_MALLA)	PTR_MALLA->_mascaraCacheSuciasMalla = 0xFF;

#include "NBRectangulo.h"
#include "NBPuntoColor.h"
#include "AUEscenaObjeto.h"
#include "AUArregloNativoMutable.h"
#include "NBGestorTexturas.h"

enum ENEscenaMallaAlineacionH {
	ENEscenaMallaAlineacionH_Izquierda,
	ENEscenaMallaAlineacionH_Centro,
	ENEscenaMallaAlineacionH_Derecha
};

enum ENEscenaMallaAlineacionV {
	ENEscenaMallaAlineacionV_Abajo,
	ENEscenaMallaAlineacionV_Centro,
	ENEscenaMallaAlineacionV_Arriba
};

struct STMallaRender {
	bool		esMascaraAlpha;
	UI8			conteoCeldas;
	UI16		conteoTexturas;
	STBloqueGL	bloqueIndicesGL;
};

struct STMallaRenderTextura {
	UI32		idTexturaGL;
};

class AUEscenaMalla: public AUEscenaObjeto {
	public:
		AUEscenaMalla();
		AUEscenaMalla(UI8 columnas, UI8 filas, AUArregloNativoP<NBPuntoColor>* verticesLocales, AUArregloNativoP<AUTextura*>* texturas, AUArregloNativoP<NBRectangulo>* areasTex);
		AUEscenaMalla(UI8 columnas, UI8 filas, NBPuntoColor* verticesLocales, UI16 conteoVertices, AUTextura** texturas, UI16 conteoTexturas, NBRectangulo* areasTex, UI16 conteoAreas);
		AUEscenaMalla(AUEscenaMalla* otraInstancia);
		virtual ~AUEscenaMalla();
		//
		UI8										columnas() const;
		UI8										filas() const;
		AUArregloNativoP<NBPuntoColor>*			verticesLocales() /*const*/;
		AUArregloNativoP<AUTextura*>*			texturas() /*const*/;
		AUArregloNativoP<NBRectangulo>*			areasTexturas() /*const*/;
		bool									esMascaraAlpha() const;
		void									establecerVerticesLocales(AUArregloNativoP<NBPunto>* nuevosVerticesLocales);
		void									establecerVerticesLocales(AUArregloNativoP<NBPuntoColor>* nuevosVerticesLocales);
		void									establecerVerticesLocales(const NBPunto* nuevosVerticesLocales, UI32 cantidadVerticesLocales);
		void									establecerVerticesLocales(const NBPuntoColor* nuevosVerticesLocales, UI32 cantidadVerticesLocales);
		void									establecerEsMascaraAlpha(bool esMascaraAlpha);
		void									establecerColorVertice(UI32 iVertice, UI8 r, UI8 g, UI8 b, UI8 a);
		//
		static AUEscenaMalla*					crearMallaSprite(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaSprite, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH=ENEscenaMallaAlineacionH_Centro, ENEscenaMallaAlineacionV alineacionV=ENEscenaMallaAlineacionV_Centro);
		static AUEscenaMalla*					crearMallaPatron(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaRepetir, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH=ENEscenaMallaAlineacionH_Centro, ENEscenaMallaAlineacionV alineacionV=ENEscenaMallaAlineacionV_Centro);
		static AUEscenaMalla*					crearMallaDesdeArchivoXml(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaArchivoMalla, ENEscenaMallaAlineacionH alineacionH=ENEscenaMallaAlineacionH_Centro, ENEscenaMallaAlineacionV alineacionV=ENEscenaMallaAlineacionV_Centro);
		static void								llenarParametrosMallaSprite(AUTextura* texturaSprite, UI8 columnas, UI8 filas, AUArregloNativoMutableP<NBPuntoColor>* guardarVerticesEn, AUArregloNativoMutableP<AUTextura*>* guardarTexturasEn, AUArregloNativoMutableP<NBRectangulo>* guardarAreasTexturasEn, ENEscenaMallaAlineacionH alineacionH=ENEscenaMallaAlineacionH_Centro, ENEscenaMallaAlineacionV alineacionV=ENEscenaMallaAlineacionV_Centro);
		static void								llenarParametrosMallaPatron(AUTextura* texturaRepetir, UI8 columnas, UI8 filas, AUArregloNativoMutableP<NBPuntoColor>* guardarVerticesEn, AUArregloNativoMutableP<AUTextura*>* guardarTexturasEn, AUArregloNativoMutableP<NBRectangulo>* guardarAreasTexturasEn, ENEscenaMallaAlineacionH alineacionH=ENEscenaMallaAlineacionH_Centro, ENEscenaMallaAlineacionV alineacionV=ENEscenaMallaAlineacionV_Centro);
		static void								llenarParametrosMallaDesdeArchivoXML(const char* rutaArchivoMalla, UI8* guardarColumnasEn, UI8* guardarFilasEn, AUArregloNativoMutableP<NBPuntoColor>* guardarVerticesEn, AUArregloNativoMutableP<AUTextura*>* guardarTexturasEn, AUArregloNativoMutableP<NBRectangulo>* guardarAreasTexturasEn, ENEscenaMallaAlineacionH alineacionH=ENEscenaMallaAlineacionH_Centro, ENEscenaMallaAlineacionV alineacionV=ENEscenaMallaAlineacionV_Centro);
		//
		virtual	NBCajaAABB						cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL					actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void								enviarComandosGL(UI8* punteroDatosModelo);
		//guardar y cargar en XML
		static bool								agregarXmlInternoEn(AUEscenaMalla* malla, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool								interpretarXmlInterno(AUEscenaMalla* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool								agregarBitsInternosEn(AUEscenaMalla* malla, AUArchivo* guardarEn);
		static bool								interpretarBitsInternos(AUEscenaMalla* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		UI8										_columnas;
		UI8										_filas;
		UI8										_mascaraCacheSuciasMalla;
		bool									_esMascaraAlpha;
		//
		AUArregloNativoMutableP<NBPuntoColor>	_verticesLocales;
		AUArregloNativoMutableP<AUTextura*> 	_texturas;
		AUArregloNativoMutableP<NBRectangulo>	_areasTexturas;
		NBCajaAABB								_cacheCajaLocalCalculada;
		//
		virtual void							agregadoEnEscena();
		virtual void							quitandoDeEscena();
		//
		//inline void							privInlineReconstruirMalla(float anchoTotal, float altoTotal, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV);
};

#endif
