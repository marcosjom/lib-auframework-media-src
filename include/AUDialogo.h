#ifndef AUDialogo_H_INCLUIDO
#define AUDialogo_H_INCLUIDO

#include "NBColor.h"
#include "AUObjeto.h"
#include "AUCadenaMutable.h"
#include "AUArregloMutable.h"
#include "AUArregloNativoMutable.h"
#include "NBGestorTexturas.h"
//
#include "AUFuenteTextura.h"
#include "AUEscenaSprite.h"
#include "AUEscenaAnimacion.h"

enum ENDialogoElemTipo {
	ENDialogoElemTipo_SeparadorBloque = 0,	//Separa un bloque de contenido
	ENDialogoElemTipo_SeparadorLinea,		//Nueva linea
	ENDialogoElemTipo_Texto,
	ENDialogoElemTipo_Imagen,
	ENDialogoElemTipo_Animacion,
	ENDialogoElemTipo_Conteo
};

enum ENDialogoAlineacionH {
	ENDialogoAlineacionH_Izquierda = 0,		//El contenido forma parte de la linea, y toda la linea se alinea a la izquierda
	ENDialogoAlineacionH_Centro,			//El contenido forma parte de la linea, y toda la linea se alinea al centro
	ENDialogoAlineacionH_Derecha,			//El contenido forma parte de la linea, y toda la linea se alinea a la derecha
	ENDialogoAlineacionH_IzquierdaLibre,	//El contenido no forma parte de la linea, se alinea a la izquierda y ocupa ese espacio (ENDialogoAlineacionV siempre serra Arriba)
	ENDialogoAlineacionH_DerechaLibre,		//El contenido no forma parte de la linea, se alinea a la derecha y ocupa ese espacio ENDialogoAlineacionV siempre serra Arriba)
	ENDialogoAlineacionH_Conteo
};

enum ENDialogoAlineacionV {
	ENDialogoAlineacionV_Arriba = 0,		//El contenido forma parte de la linea, y se alinea arriba en relacion al alto de toda la linea
	ENDialogoAlineacionV_Centro,			//El contenido forma parte de la linea, y se alinea centro en relacion al alto de toda la linea
	ENDialogoAlineacionV_Abajo,				//El contenido forma parte de la linea, y se alinea abajo en relacion al alto de toda la linea
	ENDialogoAlineacionV_Conteo
};

struct STDialogoElem {
	ENDialogoElemTipo	tipoElemento;	//Tipo de elemento
	SI32				iElem;			//Indice del elemento en el arreglo de su tipo
	//
	ENDialogoAlineacionH alineacionH;	//Alineacion horizontal del contenido
	ENDialogoAlineacionV alineacionV;	//Alineacion vertical del contenido en su linea
	SI32				iStrID;			//Indice del id del objeto en la cadena compartida
	bool				esPrimeroDeID;	//Un elemento con una ID puede ser subdividido subElementos que comparten el mismo ID (se marcan el primero y el ultimo)
	bool				esUltimoDeID;	//Un elemento con una ID puede ser subdividido subElementos que comparten el mismo ID (se marcan el primero y el ultimo)
	bool				colorExplicito;	//Determina si el color ha sido determinado de forma explicita
	NBColor8			color8;			//Color explicito (si colorExplicito es true
	//
	bool operator==(const STDialogoElem &otro) const {
		return (tipoElemento==otro.tipoElemento && iElem==otro.iElem && iStrID==otro.iStrID && colorExplicito==otro.colorExplicito && color8==otro.color8);
	}
	bool operator!=(const STDialogoElem &otro) const {
		return !(tipoElemento==otro.tipoElemento && iElem==otro.iElem && iStrID==otro.iStrID && colorExplicito==otro.colorExplicito && color8==otro.color8);
	}
};

struct STDialogoTexto {
	UI32				iTexto;		//Indice del texto en la cadena compartida
	AUFuenteTextura*	texFuente;	//Textura fuente
	bool				resaltar;	//Resaltar el texto
	//
	bool operator==(const STDialogoTexto &otro) const {
		return (iTexto==otro.iTexto && texFuente==otro.texFuente && resaltar==otro.resaltar);
	}
	bool operator!=(const STDialogoTexto &otro) const {
		return !(iTexto==otro.iTexto && texFuente==otro.texFuente && resaltar==otro.resaltar);
	}
};

struct STDialogoImagen {
	UI32			iURLRecurso;		//Indice del URL del recurso en la cadena compartida
	AUEscenaSprite*	objPlantilla;		//Asegura que la textura es cargada en tiempo de carga, y retenida mientras exista el dialogo
	//
	bool operator==(const STDialogoImagen &otro) const {
		return (iURLRecurso==otro.iURLRecurso);
	}
	bool operator!=(const STDialogoImagen &otro) const {
		return !(iURLRecurso==otro.iURLRecurso);
	}
};

struct STDialogoAnimacion {
	UI32				iURLRecurso;					//Indice del URL del recurso en la cadena compartida
	AUEscenaAnimacion*	objPlantilla;		//Asegura que la animacion es cargada en tiempo de carga, y retenida mientras exista el dialogo
	bool				repetir;
	bool				suavizar;
	float				factorVelocidad;
	float				angulo;
	//
	bool operator==(const STDialogoAnimacion &otro) const {
		return (iURLRecurso==otro.iURLRecurso);
	}
	bool operator!=(const STDialogoAnimacion &otro) const {
		return !(iURLRecurso==otro.iURLRecurso);
	}
};
		
class AUDialogo : public AUObjeto {
	public:
		//constructores y destructores
		AUDialogo();
		virtual ~AUDialogo();
		//funciones de instancia
		SI32				conteoElementos();
		const char*			cadenaEnIndice(UI32 iPrimerChar);
		STDialogoElem		elemento(UI32 indice);
		SI32				indicePrimerElementoConID(const char* strID);
		//
		STDialogoTexto		elementoTexto(UI32 indice);
		STDialogoImagen		elementoImagen(UI32 indice);
		STDialogoAnimacion	elementoAnimacion(UI32 indice);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		//propiedades
		AUCadenaLargaMutable8						_strCompartida;
		AUArregloNativoMutableP<STDialogoElem>		_arregloElementos;
		//
        AUArregloNativoMutableP<STDialogoTexto>		_arregloTextos;
		AUArregloNativoMutableP<STDialogoImagen>	_arregloImagenes;
		AUArregloNativoMutableP<STDialogoAnimacion>	_arregloAnimaciones;
		//
		AUArregloMutable							_bibliotecaFuentes; //Optimizacion, para no solicitar repetitivamente las fuentes al Gestor
		//
		void				privVaciar();
};

#endif
