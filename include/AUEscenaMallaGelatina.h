//
//  AUEscenaMallaGelatinaAnimada.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 24/01/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaMallaGelatina_H_INCLUIDO
#define AUEscenaMallaGelatina_H_INCLUIDO

#include "AUEscenaMalla.h"
#include "NBAnimador.h"
#include "NBGestorAnimadores.h"

enum ENMallaGelatinaVertices {
	ENMallaGelatinaVertices_Todos,			//Todos los vertices de la malla
	ENMallaGelatinaVertices_Internos,		//Todos excepto los bordes
	ENMallaGelatinaVertices_Bordes,			//Solamente los bordes
	ENMallaGelatinaVertices_BordesSuperior,	//Solamente los bordes superiores (arriba)
	ENMallaGelatinaVertices_Esquinas		//Solamente las 4 esquinas
};

enum ENMallaGelatinaCalculo {
	ENMallaGelatinaCalculo_Uniforme = 0,
	ENMallaGelatinaCalculo_CentroEsMaximo,
	ENMallaGelatinaCalculo_CentroEsCero
};

struct STMallaGelatinaVertice {
	NBPunto coordNormal;
	NBPunto coordDestino;
	float	factorMovimiento;
	//
	bool operator==(const STMallaGelatinaVertice &otro) const {
		return (coordNormal==otro.coordNormal && coordDestino==otro.coordDestino && factorMovimiento==otro.factorMovimiento);
	}
	bool operator!=(const STMallaGelatinaVertice &otro) const {
		return !(coordNormal==otro.coordNormal && coordDestino==otro.coordDestino && factorMovimiento==otro.factorMovimiento);
	}
};

class AUEscenaMallaGelatina: public AUEscenaMalla, NBAnimador {
	public:
		AUEscenaMallaGelatina();
		AUEscenaMallaGelatina(UI8 columnas, UI8 filas, AUArregloNativoP<NBPuntoColor>* verticesLocales, AUArregloNativoP<AUTextura*>* texturas, AUArregloNativoP<NBRectangulo>* areasTex);
		AUEscenaMallaGelatina(AUEscenaMallaGelatina* otraInstancia);
		virtual ~AUEscenaMallaGelatina();
		//
		void								establecerVerticesAnimar(ENMallaGelatinaVertices verticesAnimar);
		void								establecerVelocidadMaxima(float xVelMax, float yVelMax);
		void								establecerDesplazamientoMaximo(float anchoDesplazamientoMax, float altoDesplazamientoMax);
		void								establecerMetodoCalculoDesplazamiento(ENMallaGelatinaCalculo metodoCalculoDesplazamiento);
		void								establecerCentroCalculoDesplazamiento(float centroX, float centroY);
		void								establecerRadioCalculoDesplazamiento(float radio);
		void								establecerCalculoDesplazamiento(ENMallaGelatinaCalculo metodoCalculoDesplazamiento, float centroX, float centroY, float radio);
		void								establecerDesplazamientoMaximoSegunFactor(float factorX, float factorY);
		void								establecerVelocidadMaximaSegunFactor(float factorX, float factorY);
		void								moverVerticeReposo(UI32 iVertice, float posX, float posY);
		NBPunto								verticeReposo(UI32 iVertice) const;
		AUArregloNativoP<STMallaGelatinaVertice>* verticesGelatina();
		ENMallaGelatinaVertices				verticesAnimados() const;
		NBTamano							velocidadMaxima() const;
		NBTamano							desplazamientoMaximo() const;
		NBPunto								centroCalculoDesplazamiento() const;
		float								radioCalculoDesplazamiento() const;
		ENMallaGelatinaCalculo				metodoCalculoDesplazamiento() const;
		//
		void								desplazarCoordenadasVertices(SI32 desplazamientoFilas, SI32 desplazamientoColumnas);
		//
		static AUEscenaMallaGelatina*		crearMallaGelatinaSprite(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaSprite, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH=ENEscenaMallaAlineacionH_Centro, ENEscenaMallaAlineacionV alineacionV=ENEscenaMallaAlineacionV_Centro);
		static AUEscenaMallaGelatina*		crearMallaGelatinaPatron(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaRepetir, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH=ENEscenaMallaAlineacionH_Centro, ENEscenaMallaAlineacionV alineacionV=ENEscenaMallaAlineacionV_Centro);
		static AUEscenaMallaGelatina*		crearMallaGelatinaDesdeArchivoXml(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaArchivoMalla, ENEscenaMallaAlineacionH alineacionH=ENEscenaMallaAlineacionH_Centro, ENEscenaMallaAlineacionV alineacionV=ENEscenaMallaAlineacionV_Centro);
		//
		virtual void						tickAnimacion(float segsTranscurridos);
		//guardar y cargar en XML
		static bool							agregarXmlInternoEn(AUEscenaMallaGelatina* malla, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool							interpretarXmlInterno(AUEscenaMallaGelatina* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool							agregarBitsInternosEn(AUEscenaMallaGelatina* malla, AUArchivo* guardarEn);
		static bool							interpretarBitsInternos(AUEscenaMallaGelatina* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		//Propiedades de animacion
		UI8									_verticesAnimar;		//Determina cuales vertices animar
		UI8									_metodoCalculoDesplazamiento;
		NBTamano							_velocidadMaxima;		//Determina la velocidad de desplazamiento
		NBTamano							_desplazamientoMaximo;	//Determina el desplzamiento maximo de cada vertice
		NBPunto								_centroCalculoDesplazamiento;
		float								_radioCalculoDesplazamiento;
		AUArregloNativoMutableP<STMallaGelatinaVertice>	_verticesGelatina;
		//
		virtual void						agregadoEnEscena();
		virtual void						quitandoDeEscena();
		inline void							privInlineActualizaFactoresMovimientosVertices();
	
		
};

#endif
