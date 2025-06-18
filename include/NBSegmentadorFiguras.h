//
//  NBSegmentadorIlumnacion.h
//  AUFramework
//
//  Created by Marcos Ortega on 22/11/12.
//
//

#ifndef AUFramework_NBSegmentadorFiguras_h
#define AUFramework_NBSegmentadorFiguras_h

#include "NBLinea.h"
#include "NBTriangulo.h"
#include "NBCajaAABB.h"
#include "NBFuenteSombra.h"
#include "AUCadenaMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUEscenaObjeto.h"
#include "AUTextura.h"
#include "AUEscenaGrupoParticulasDef.h"
#include "NBGestorGLDef.h"
#include "AUEscenaCuerdaDef.h"

#define NBSEGMENTADORFIGURAS_CACHE_FIGURAS_INICIALES		4
#define NBSEGMENTADORFIGURAS_CACHE_VERTICES_INICIALES		64
//
#define NBSEGMENTADORFIGURAS_MAX_VERTICES_SOMBRAS			16
#define NBSEGMENTADORFIGURAS_MAX_INTERSECCONES_POR_SOMBRA	12	//El maximo encontrado deberia ser 6 (+1 olgura antes, +3 olgura despues)

struct NBSegFigInterseccionSombra {
	bool		precisaEnSombra;		//si true, entonces la interseccion con el segmento sombra esta entre 0.0f y 1.0f, sino lo mas cercano posible
	SI16		iVertSombra;			//desde -2 hasta (conteoVerticesSombra + 1)
	float		avanceEnSegSombra;		//si precisa entonces entre 0.0f y 1.0f inclusives, sino la mas cercana a 0.0f o 1.0f
	float		avanceEnSegFigura;		//obligatorimente esta entre 0.0f y 1.0f inclusives
	//
	bool operator==(const NBSegFigInterseccionSombra &otro) const {
		return (iVertSombra==otro.iVertSombra && avanceEnSegSombra==otro.avanceEnSegSombra);
	}
	bool operator!=(const NBSegFigInterseccionSombra &otro) const {
		return !(iVertSombra==otro.iVertSombra && avanceEnSegSombra==otro.avanceEnSegSombra);
	}
};

struct NBSegFigInterseccionSimple {
	NBPunto		posicion;
	SI16		iVertFigura;
	//
	bool operator==(const NBSegFigInterseccionSimple &otro) const {
		return (posicion==otro.posicion);
	}
	bool operator!=(const NBSegFigInterseccionSimple &otro) const {
		return !(posicion==otro.posicion);
	}
};
	
class NBSegmentadorFiguras {
	public:
		static void											inicializar();
		static void											finalizar();
		//Cache
		static AUArregloNativoMutableP<NBPunto>**			cacheFiguras();
		static UI16											cacheConteoFiguras();
		//Producir sombra
		static void											producirSombraDosVertices(NBFuenteIluminacionRender* arrLuces, const UI16 conteoLues, AUArregloNativoMutableP<NBFuenteSombra>** arregloSombrasPorLuces, AUArregloNativoMutableP<NBPunto>* bibVerticesIntermediosSombras, const NBPunto verticesEscenaIzq, const NBPunto &verticeEscenaDer, AUEscenaObjeto* objOriginaSombra, AUTextura* texturaSombra);
		static void											producirSombraMultiVertices(NBFuenteIluminacionRender* arrLuces, const UI16 conteoLues, AUArregloNativoMutableP<NBFuenteSombra>** arregloSombrasPorLuces, AUArregloNativoMutableP<NBPunto>* bibVerticesIntermediosSombras, const NBPunto* verticesEscenaSombra, const UI16 conteoVerticesEscenaSombra, AUEscenaObjeto* objOriginaSombra, const bool verticesSombraSonContrareloj, AUTextura* texturaSombra);
		static bool											sombraEsCubiertaPorOtraYCubrirOtrasDosVertices(NBFuenteSombra* arrOtrasSombras, const UI16 conteoOtrasSombras, const NBPunto* bibVerticesIntermediosSombras, const NBPunto &vectorMasIzquierda, const NBPunto &vectorMasDerecha);
		static bool											sombraEsCubiertaPorOtraYCubrirOtrasMultiVertices(NBFuenteSombra* arrOtrasSombras, const UI16 conteoOtrasSombras, const NBPunto* bibVerticesIntermediosSombras, const NBPunto &vectorMasIzquierda, const NBPunto &vectorMasDerecha, const NBPunto* verticesNuevaSombra, const UI16 conteoVerticesNuevaSombra);
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		static bool											debugValidarSombra(const NBPunto* verticesEscenaSombra, const SI16 conteoVerticesEscenaSombra, const NBPunto &vectorAngMin, const NBPunto &vectorAngMax);
		#endif
		//Segmentar figura
		static AUArregloNativoMutableP<NBPunto>*			figuraActual();
		static void											cargarFiguraActual(const NBPunto* vertices4FiguraConvexaContrareloj);
		static void											limitarCajaSegunVectoresAngulos(const NBPunto &vectorAngMin, const NBPunto &vectorAngMax);
		static void											extraerPorcionesSombrasAFigura(const NBFuenteSombra* arregloSombras, const UI16 conteoSombras, const NBPunto* bibVertsSombras, const UI16 tamBibVertsSombras);
		static void											extraerProcionesRangosSombrasAFigura(const STRangoSombra* arregloSombras, const UI16 conteoSombras);
		static void											dividirEnPorcionesDerechaLinea(const NBPunto &posVector, const NBPunto &vectUnitCorte);
		//Color ante luces y sombras
		static NBColor										calcularColoresAnteLucesConsiderandoSombras(NBFuenteIluminacionRender* fuentesLuz, AUArregloNativoMutableP<NBFuenteSombra>** sombrasPorFuenteLuz, const UI16 conteoIlum, const NBPunto* arrVerticesSombras, const AUEscenaObjeto* objAgrupadorSombras, const NBPunto &punto, const NBCajaAABB &cajaVerticesGL);
		//
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		static bool											figuraEsConvexaContrareloj(const NBPunto* vertices, const UI16 conteoVertices);
		static bool											figuraNoRepiteVertices(const NBPunto* vertices, const UI16 conteoVertices);
		#endif
		//
		static AUArregloNativoMutableP<NBPunto>*			_figuras[2];	//Figura que se esta segmentando "_indiceFiguraActual"
		static AUArregloMutable*							_cacheFiguras;	//Para calculo de segmentacion (Arreglos de "AUArregloNativoMutableP<NBPunto>*")
		static UI16											_indiceFiguraActual;
		static UI16											_cacheConteoFigurasUsadas;
		static AUArregloNativoMutableP<NBSegFigInterseccionSimple>* _cacheIntersecciones; //Para cuando se esta cortando la figura ante un vector que no pasa por el centro
		#ifdef CONFIG_NB_SEGMENTADOR_FIGURAS_ESCRIBIR_LOG
		static AUCadenaLargaMutable8*						_dbgStr;
		#endif
	private:
		static void											privCacheVaciar();
		static AUArregloNativoMutableP<NBPunto>*			privCacheReservarNuevaFigura();
		static void											privAgregarOrejasNoConvexasDeFigura(AUArregloNativoMutableP<NBPunto>* figura);
		static SI16											privIndiceVerticeCompletamenteFueraDeSombraDosVertices(const NBPunto* arrVerticesFig, const UI16 conteoVerticesFig, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra);
		static SI16											privIndiceVerticeCompletamenteFueraDeSombraMultiVertices(const NBPunto* arrVerticesFig, const UI16 conteoVerticesFig, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra, const NBPunto* arrVertSmb, const NBPunto* vertSmbSigUlt);
		static bool											privVerticeEstaCompletamenteFueraDeSombraDosVertices(const NBPunto &vectorVertice, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra);
		static bool											privVerticeEstaCompletamenteFueraDeSombraMultiVertices(const NBPunto &vectorVertice, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra, const NBPunto* arrVertSmb, const NBPunto* vertSmbSigUlt);
		//Busqueda de intersecciones utilizando el segmento preciso
		static UI16											privBuscarUnaInterseccionConSombraDosVerticesOrdenInversoVsSegmento(NBSegFigInterseccionSombra* guardarEn, const NBPunto &iniSegFig, const NBPunto &finSegFig, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra, const NBPunto &posFinVectorSmbAngMin, const NBPunto &posFinVectorSmbAngMax);
		static UI16											privBuscarUnaInterseccionConSombraMultiVerticesOrdenInversoVsSegmento(NBSegFigInterseccionSombra* guardarEn, const NBPunto &iniSegFig, const NBPunto &finSegFig, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra, const NBPunto &posFinVectorSmbAngMin, const NBPunto &posFinVectorSmbAngMax, const NBPunto* arrVertSmb, const SI16 conteoVertSmb);
		static UI16											privBuscarHastaDosInterseccionesConSombraDosVerticesOrdenInversoVsSegmento(NBSegFigInterseccionSombra* guardarEn, const NBPunto &iniSegFig, const NBPunto &finSegFig, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra, const NBPunto &posFinVectorSmbAngMin, const NBPunto &posFinVectorSmbAngMax);
		static UI16											privBuscarHastaDosInterseccionesConSombraMultiVerticesOrdenInversoVsSegmento(NBSegFigInterseccionSombra* guardarEn, const NBPunto &iniSegFig, const NBPunto &finSegFig, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra, const NBPunto &posFinVectorSmbAngMin, const NBPunto &posFinVectorSmbAngMax, const NBPunto* arrVertSmb, const SI16 conteoVertSmb);
};
	
#endif
