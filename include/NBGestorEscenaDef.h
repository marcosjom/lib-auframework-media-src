//
//  NBGestorEscenaDef.h
//  AUFramework
//
//  Created by Marcos Ortega on 19/12/12.
//
//

#ifndef AUFramework_NBGestorEscenaDef_h
#define AUFramework_NBGestorEscenaDef_h

#include "NBFuenteEspejo.h"
#include "AUEscenaLaserDef.h"
#include "NBPunto.h"
#include "NBColor.h"
#include "NBMatriz.h"

struct NBPropRenderizado;
struct NBPropIluminacion;
struct NBPropHeredadas;

//Enviar comandos GL
typedef void (*PTRfuncEnviarComandosGL)(UI8* punteroDatosModelo);
typedef PTRfuncEnviarComandosGL PTRfunCmdsGL;

//Generar RenderBuffer
typedef NBTamanoI (*PTRfuncGeneraRenderbuffer)(const SI32, const SI32, void*);

//Reservar verticesGL
typedef UI32 (*PTRfuncReservarVerticesGL)(const ENVerticeGlTipo tipo, const UI32 cantidad);

//Reservar indicesGL
#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
typedef UI32 (*PTRfuncReservarIndicesGL)				(const ENVerticeGlTipo tipo, const UI32 cantidad);
typedef STBloqueGL (*PTRfuncRsvIndGLTStrip4Ind)			(const ENVerticeGlTipo tipo, UI16 iPrimerVertice); //reservarIndicesParaTriangStrip4Independizado
typedef STBloqueGL (*PTRfuncRsvIndGLTStrip4IndMult)		(const ENVerticeGlTipo tipo, UI16 iPrimerVertice, UI16 cantTriangStrips4); //reservarIndicesParaTriangStrip4IndependizadoMultiples
typedef STBloqueGL (*PTRfuncRsvIndGLTStripInd)	 		(const ENVerticeGlTipo tipo, UI16 iPrimerVertice, UI16 verticesPorTriangStrip); //reservarIndicesParaTriangStripIndependizado
typedef STBloqueGL (*PTRfuncRsvIndGLTStripIndMult)		(const ENVerticeGlTipo tipo, UI16 iPrimerVertice, UI16 verticesPorTriangStrip, UI16 cantTriangStrips); //reservarIndicesParaTriangStripIndependizadoMultiples
typedef STBloqueGL (*PTRfuncRsvIndGLTStripIndTriagnFan)	(const ENVerticeGlTipo tipo, UI16 iPrimerVertice, UI16 verticesPorTriangFan); //reservarIndicesParaTriangStripIndependizadoDesdeTriangFan
#endif

//Bloque de datos de renderizado de modelo
struct STPropRenderModelo {
	SI32	indiceDatos;						//Indice del primer byte de dato de cada objeto en el ciclo de render
	PTRfuncEnviarComandosGL funcEnvComadosGL;	//ObjetosEscena en el orden que deben renderizarse (sustituye a la llamada en arbol por una secuencial)
};

typedef struct STNBSceneModelsResult_ {
	UI32	countScenes;			//Count scenes
	UI32	countScenesDirty;		//Count scenes root props need to be render.
	UI32	countModels;			//Count models (containers and objects)
	UI32	countModelsDirty;		//Count models need to be render (sceneMatris or localModel changed)
	UI32	countTexChanged;		//Count textures datat changed since last tick
} STNBSceneModelsResult;

struct NBPropRenderizado {						//Datos que describen el renderizado en escena (para un grupo, una capa y un tick)
	NBRectangulo									puertoDeVision;
	NBCajaAABB										cajaProyeccion;			//Caja que representa la escena
	UI32											idActualizacion;		//Secuencial unico para la actualizacion (ID que permite evitar actualizar dos veces el mismo modelo durante multiples llamadas)
	AUArregloNativoMutableP<STPropRenderModelo>*	renderModelos;			//ObjetosEscena en el orden que deben renderizarse (sustituye a la llamada en arbol por una secuencial)
	AUArregloNativoMutableP<UI8>*					bytesDatosModelos;		//Almacen de datos utilizados durante el ciclo de render
	AUArregloMutable*								piscinaRetecionRender;	//Piscnia de retencion de objetos en uso durante el renderizado
	//
	PTRfuncReservarVerticesGL						funcReservarVertices;
	NBVerticeGL0**									verticesGL0;	//Acceso optimizado hacia el arreglo de vertices sintexturas
	NBVerticeGL**									verticesGL1;	//Acceso optimizado hacia el arreglo de vertices monotexturas
	NBVerticeGL2**									verticesGL2;	//Acceso optimizado hacia el arreglo de vertices bitexturas
	NBVerticeGL3**									verticesGL3;	//Acceso optimizado hacia el arreglo de vertices tritexturas
#	ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
	PTRfuncReservarIndicesGL						funcReservarIndices;
	PTRfuncRsvIndGLTStrip4Ind						funcReservarIndicesParaTriangStrip4Independizado;
	PTRfuncRsvIndGLTStrip4IndMult					funcReservarIndicesParaTriangStrip4IndependizadoMultiples;
	PTRfuncRsvIndGLTStripInd						funcReservarIndicesParaTriangStripIndependizado;
	PTRfuncRsvIndGLTStripIndMult					funcReservarIndicesParaTriangStripIndependizadoMultiples;
	PTRfuncRsvIndGLTStripIndTriagnFan				funcReservarIndicesParaTriangStripIndependizadoDesdeTriangFan;
	GLushort**										indicesGL0;		//Acceso optimizado hacia el arreglo de indices sintexturas
	GLushort**										indicesGL1;		//Acceso optimizado hacia el arreglo de indices monotexturas
	GLushort**										indicesGL2;		//Acceso optimizado hacia el arreglo de indices bitexturas
	GLushort**										indicesGL3;		//Acceso optimizado hacia el arreglo de indices tritexturas
#	endif
};
	
struct NBPropIluminacion {		//datos que describen la iluminacion en escena (para un grupo, una capa y un tick)
	SI32										conteoFuentesIluminacion;
	NBFuenteIluminacionRender*					fuentesIluminacion;
	AUArregloNativoMutableP<NBFuenteSombra>**	sombrasPorFuenteIluminacion;
	AUArregloNativoMutableP<NBPunto>*			sombrasVerticesIntermedios;		//Biblioteca compartida entre todas las sombras
};
	
struct NBPropHeredadasModelos {		//datos que describen las propiedades heredadas de un contenedor a sus hijos (para un objEscena y un tick)
	bool				matrizModificadaPadre;
	NBMatriz			matrizPadre;
	//
	bool operator==(const NBPropHeredadasModelos &otro) const {
		return (matrizPadre==otro.matrizPadre);
	}
	bool operator!=(const NBPropHeredadasModelos &otro) const {
		return !(matrizPadre==otro.matrizPadre);
	}
};
		
struct NBPropHeredadasRender {		//datos que describen las propiedades heredadas de un contenedor a sus hijos (para un objEscena y un tick)
	bool				iluminoDependenciaPadre;
	NBColor				colorPadre;
	bool operator==(const NBPropHeredadasRender &otro) const {
		return (colorPadre==otro.colorPadre && iluminoDependenciaPadre==otro.iluminoDependenciaPadre);
	}
	bool operator!=(const NBPropHeredadasRender &otro) const {
		return !(colorPadre==otro.colorPadre && iluminoDependenciaPadre==otro.iluminoDependenciaPadre);
	}
};

//Interfaz para los objetos que deseen recibir notificaciones de cambio del puerto de vision (util para reorganizar los elementos en escena)

typedef struct STNBViewPortSize_ {
	STNBSize		ppi;	//Pixels per inch
	STNBSize		dpi;	//Dots per inch
	STNBRectI		rect;	//sizes in pixels
} STNBViewPortSize;

class IEscuchadorCambioPuertoVision {
	public:
		virtual ~IEscuchadorCambioPuertoVision(){ /*nada*/ }
		virtual void puertoDeVisionModificado(const SI32 iEscena, const STNBViewPortSize before, const STNBViewPortSize after) = 0;
};
	
//Interfaz para objetos que consumen luces y producen sombras
class IEscenaProductorSombras {
	public:
		virtual ~IEscenaProductorSombras(){ /*nada*/ }
		virtual void producirSombras2(const NBPropIluminacion &propsIluminacion) = 0;
};
	
//Interfaz para objetos que consumen las luces (independientemente de si estan dentro o fuera de la camara)
class IEscenaConsumidorLuces {
	public:
		virtual ~IEscenaConsumidorLuces(){ /*nada*/ }
		virtual void consumirLuces(const NBPropIluminacion &propsIluminacion) = 0;
};

//Interfaz para objetos que consumen los espejos (independientemente de si estan dentro o fuera de la camara,)
class IEscenaConsumidorEspejos {
	public:
		virtual ~IEscenaConsumidorEspejos(){ /*nada*/ }
		virtual void consumirEspejos(const NBFuenteEspejoRender* arrEspejos, const UI16 conteoEspejos) = 0;
};
	
//Cuerdas y rayos
struct STCuerdaCorte {
	UI16	cuerdaISegmentoCorte;
	float	cuerdaAvanceRelSegmento;
	float	rayoAvanceRelEnRayo;
	NBPunto rayoPosEscenaColision;
	NBPunto rayoNormalColision;
};
	
class ICuerdaCortable {
	public:
		virtual ~ICuerdaCortable(){
			//
		}
		virtual bool	cuerdaAnalizarSiRayoCorta(const STEscenaLaserPos* arrPosEscena, const UI16 conteoPosEscena, STCuerdaCorte* guardarCorteEn) = 0;
		virtual void	cuerdaConsumirCorteDeRayo(const UI16 &iSegmentoCorte, const float factorCorteRayo, const float segsTranscurridos) = 0;
		virtual void	cuerdaRetenerParaAnalisisCorte() = 0;
		virtual void	cuerdaLiberarDeAnalisisCorte() = 0;
};

//Interfaz para objetos que requieren hacer calculos antes de actualizar el modeloGL (independientemente de si estan dentro o fuera de la camara,)
class IEscenaConsumidorCuerdas {
	public:
		virtual ~IEscenaConsumidorCuerdas(){ /*nada*/ }
		virtual void consumirCuerdas(ICuerdaCortable** cuerdas, const UI16 conteoCuerdas) = 0;
};
	
//Interfaz para objetos que requieren hacer calculos antes de actualizar el modeloGL (independientemente de si estan dentro o fuera de la camara,)
class IEscenaConsumidorPreModeloGL {
	public:
		virtual ~IEscenaConsumidorPreModeloGL(){ /*nada*/ }
		virtual void consumirAntesDeCrearModeloGL() = 0;
};

#endif
