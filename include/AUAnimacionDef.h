//
//  AUAnimacionDef.h
//  AUFramework
//
//  Created by Nicaragua Binary on 25/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUFramework_AUAnimacionDef_h
#define AUFramework_AUAnimacionDef_h

#include "NBPunto.h"
#include "NBTamano.h"
#include "NBColor.h"
#include "AUEscenaObjeto.h"
#include "AUEscenaContenedor.h"
#include "AUCadena.h"
#include "AUArregloNativoMutable.h"
#include "AUAnimadorSonidoVolumen.h"
#include "AUSonidoStream.h"
#include "NBGestorSonidos.h"

//Metodo de encapsulamiento de animacion
enum ENAnimacion_TipoEncapsulamiento {
	ENAnimacion_TipoEncapsulamiento_UnContenedorNormal,		//Normal, la animacion es el contenedor padre y los hijos dentro de ella
	ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo		//Cada elemento de la animacion es encapsulada en un contenedor (permite modificar las propiedades de cada componente de la animacion)
};

//
//Biblioteca de animacion (compartida con las subanimaciones)
//
class AUAnimacionBibliotecaDef : public AUObjeto {
	public:
		AUAnimacionBibliotecaDef() : AUObjeto(), rutaRecurso(this), traslaciones(this), escalaciones(this), rotaciones(this), colores(this) {
			scaleBase2 = 4;	//By default, animations are designed at scaleBase2 = 4.
			plantillaAnimacion = NULL;
		}
		virtual ~AUAnimacionBibliotecaDef(){
			if(plantillaAnimacion != NULL) plantillaAnimacion->liberar(NB_RETENEDOR_THIS);
		}
		//
		UI32											scaleBase2;
		UI32											identificador;
		ENAnimacion_TipoEncapsulamiento					tipoEncapsulamientoHijos;
		FLOAT											fps;
		AUEscenaContenedor*								plantillaAnimacion;			//incluye subanimaciones que comparten esta plantilla (biblioteca de transformaciones y colores)
		AUCadenaMutable8								rutaRecurso;
		AUArregloNativoMutableP<NBPunto>				traslaciones;
		AUArregloNativoMutableP<NBTamano>				escalaciones;
		AUArregloNativoMutableP<float>					rotaciones;
		AUArregloNativoMutableP<NBColor8>				colores;
		//
		void establecerObjetoEscena(AUEscenaContenedor* contenedor) {
			if(contenedor != NULL) contenedor->retener(NB_RETENEDOR_THIS);
			if(plantillaAnimacion != NULL) plantillaAnimacion->liberar(NB_RETENEDOR_THIS);
			plantillaAnimacion = contenedor;
		}
		//
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_UNICLASE(AUAnimacionBibliotecaDef)
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_UNICLASE(AUAnimacionBibliotecaDef, "AUAnimacionBibliotecaDef")
		AUOBJMETODOS_CLONAR_DEFINICIONCOMPLETA_NULL
};

struct STAnimacionEventoSonido {
	UI32			iFrame;
#	ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
	UI16			bufferSonido;		//Si el sonido es corto (el buffer completo se carga en memoria)
	AUSonidoStream*	streamSonido;		//Si el sonido es largo (el buffer se cargara por partes desde el archivo)
	AUAnimadorSonidoVolumen* animadorSonido;
#	endif
	bool			detener;
	bool			noMultiple;
	bool			repetir;
	SI32			repeticiones;
	AUCadena8*		rutaSonido;
	//
	bool operator==(const STAnimacionEventoSonido &otro) const {
		return (iFrame==otro.iFrame
#				ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
				&& bufferSonido==otro.bufferSonido
#				endif
				&& detener==otro.detener && noMultiple==otro.noMultiple);
	}
	bool operator!=(const STAnimacionEventoSonido &otro) const {
		return !(iFrame==otro.iFrame
#				ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
				 && bufferSonido==otro.bufferSonido
#				endif
				 && detener==otro.detener && noMultiple==otro.noMultiple);
	}
};

struct STAnimacionIndicesPropiedades {
	SI16 iTraslacion;
	SI16 iEscalacion;
	SI16 iRotacion;
	SI16 iColor;
	//
	bool operator==(const STAnimacionIndicesPropiedades &otro) const {
		return (iTraslacion==otro.iTraslacion && iEscalacion==otro.iEscalacion && iRotacion==otro.iRotacion && iColor==otro.iColor);
	}
	bool operator!=(const STAnimacionIndicesPropiedades &otro) const {
		return !(iTraslacion==otro.iTraslacion && iEscalacion==otro.iEscalacion && iRotacion==otro.iRotacion && iColor==otro.iColor);
	}
};

//
// Datos de animacion (una para para cada animacion principal y otra para cada subanimacion)
//
class AUAnimacionDef : public AUObjeto {
	public:
		AUAnimacionDef(UI32 pFrames, NBCajaAABB pCajaLienzo) : AUObjeto(), iPropiedadesPorFrame(this, pFrames), eventosDeSonido(this), framesConEventoSonido(this) {
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUAnimacionDef")
			frames		= pFrames;
			cajaLienzo	= pCajaLienzo;
			UI32 iFrame;
			for(iFrame=0; iFrame<frames; iFrame++){
				AUArregloNativoMutableP<STAnimacionIndicesPropiedades>* iPropiedades = new(this) AUArregloNativoMutableP<STAnimacionIndicesPropiedades>(); NB_DEFINE_NOMBRE_PUNTERO(iPropiedades, "AUEscenaAnimacion::iPropiedades");
				iPropiedadesPorFrame.agregarElemento(iPropiedades);
				iPropiedades->liberar(NB_RETENEDOR_THIS);
			}
		}
		virtual ~AUAnimacionDef(){
			SI32 iEvSonido;
			for(iEvSonido=eventosDeSonido.conteo-1; iEvSonido>=0; iEvSonido--){
				STAnimacionEventoSonido* evento = &(eventosDeSonido.elemento[iEvSonido]);
#				ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
				if(evento->streamSonido != NULL) evento->streamSonido->liberar(NB_RETENEDOR_NULL);
				if(evento->bufferSonido!=0) NBGestorSonidos::bufferLiberar(evento->bufferSonido);
				if(evento->animadorSonido != NULL) evento->animadorSonido->liberar(NB_RETENEDOR_NULL);
#				endif
				if(evento->rutaSonido != NULL) evento->rutaSonido->liberar(NB_RETENEDOR_NULL);
			}
		}
		//
		UI32											frames;
		NBCajaAABB										cajaLienzo;
		AUArregloMutable								iPropiedadesPorFrame; 	//arreglo de arreglos "AUArregloNativoMutableP<STAnimacionIndicesPropiedades>"
		AUArregloNativoMutableP<STAnimacionEventoSonido> eventosDeSonido;
		AUArregloNativoMutableP<bool>					framesConEventoSonido;
		//
		void establecerIndicePropiedadesEnFrame(UI32 indiceFrame, UI32 indiceObjEscenaHijo, SI32 indiceTraslacion, SI32 indiceEscalacion, SI32 indiceRotacion, SI32 indiceColor){
			NBASSERT(indiceFrame<frames)
			NBASSERT(indiceTraslacion>=-1 && indiceTraslacion<32767) //32767 es el rango maximo de un SI16
			NBASSERT(indiceEscalacion>=-1 && indiceEscalacion<32767) //32767 es el rango maximo de un SI16
			NBASSERT(indiceRotacion>=-1 && indiceRotacion<32767) //32767 es el rango maximo de un SI16
			NBASSERT(indiceColor>=-1 && indiceColor<32767) //32767 es el rango maximo de un SI16
			STAnimacionIndicesPropiedades datosIndices;
			datosIndices.iTraslacion = indiceTraslacion;
			datosIndices.iEscalacion = indiceEscalacion;
			datosIndices.iRotacion = indiceRotacion;
			datosIndices.iColor = indiceColor;
			//Almacenar indices de transformaciones y color
			AUArregloNativoMutableP<STAnimacionIndicesPropiedades>* indicesPropsDeFrame = (AUArregloNativoMutableP<STAnimacionIndicesPropiedades>*)iPropiedadesPorFrame.elemento[indiceFrame];
			while(indicesPropsDeFrame->conteo <= indiceObjEscenaHijo){
				indicesPropsDeFrame->agregarElemento(datosIndices); //Rellenar huecos
			}
			indicesPropsDeFrame->elemento[indiceObjEscenaHijo] = datosIndices;
		}
		//
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_UNICLASE(AUAnimacionDef)
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_UNICLASE(AUAnimacionDef, "AUAnimacionDef")
		AUOBJMETODOS_CLONAR_DEFINICIONCOMPLETA_NULL
};

#endif
