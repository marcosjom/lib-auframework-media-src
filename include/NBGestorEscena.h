//
//  NBGestorEscena.h
//  AUFramework
//
//  Created by Nicaragua Binary on 22/10/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBGESTORESCENA_H_INCLUIDO
#define NBGESTORESCENA_H_INCLUIDO

#include "NBGestorPilaLlamadas.h"
#include "NBGestorGL.h"
#include "NBGestorTexturas.h"
#include "nb/fonts/NBFontLines.h"
#include "nb/fonts/NBFontBitmaps.h"
#include "AURenderBuffer.h"
#include "AUTextura.h"
#include "NBMargenes.h"
#include "NBColor.h"
#include "NBRectangulo.h"
#include "AUAtlasMapaMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUArregloMutable.h"
#include "AUFuenteLineas.h"
#include "AUFuenteMapaBits.h"
#include "AUEscenaContenedor.h"
#include "AUEscenaContenedorRaiz.h"
#include "AUEscenaSprite.h"
#include "NBGestorTouches.h"
#include "AUCacheNativaMutableP.h"
#include "AUEscenaGrupoParticulas.h"
#include "NBSegmentadorFiguras.h"
#include "NBFuenteIluminacion.h"
#include "NBFuenteEspejo.h"
#include "AUEscenaTopItf.h"
//
#include "nb/scene/NBScnRender.h"

#define NBGESTORESCENA_MAX_TEXTURAS_RENDER		64 //16	//cantidad de texturas que pueden crearse (para uso temporal de luces individuales, luces combinadas o capas)
#define NBGESTORESCENA_MAX_ESCENAS				64 //16	//cantidad de escenas que pueden crearse

typedef enum ENGestorEscenaDestinoGl_ {
	ENGestorEscenaDestinoGl_Ninguno			= -1,
	ENGestorEscenaDestinoGl_Heredado		= 0,	//Render buffer generado fuera del motor "enlazable mediante bind(0)"
	ENGestorEscenaDestinoGl_RenderBuffer	= 1,	//Render buffer generado por este motor
	ENGestorEscenaDestinoGl_Textura			= 2		//Textura generada por este motor
} ENGestorEscenaDestinoGl;

typedef enum ENGestorEscenaFondoModo_ {
	ENGestorEscenaFondoModo_Opaco				= 0,	//El resultado en el destino de la escena se asegura sin alpha
	ENGestorEscenaFondoModo_Transparente,				//El resultado en el destino de la escena permite alpha
	ENGestorEscenaFondoModo_Conteo
} ENGestorEscenaFondoModo;

typedef enum ENGestorEscenaBufferEstado_ {
	ENGestorEscenaBufferEstado_Vacio = 0,	//El buffer no contiene datos
	ENGestorEscenaBufferEstado_Lleno,		//El buffer contiene datos que deben procesarse
	ENGestorEscenaBufferEstado_Discarted,	//Nothing to render, keep old buffer content
	ENGestorEscenaBufferEstado_Conteo
} ENGestorEscenaBufferEstado;

typedef enum ENGestorEscenaGrupo_ {
	ENGestorEscenaGrupo_Fondo = 0,			//Estatico segun tamano de pantalla
	ENGestorEscenaGrupo_Escena,				//Dinamico segun logica de programa
	ENGestorEscenaGrupo_GUI,				//Estatico segun tamano de pantalla
	ENGestorEscenaGrupo_Cortina,			//Estatico segun tamano de pantalla (para contenido de transiciones entre escenas)
	ENGestorEscenaGrupo_DebugFrontal,		//Estatico segun tamano de pantalla (para informacion de depuracion)
	ENGestorEscenaGrupo_Conteo
} ENGestorEscenaGrupo;

typedef enum ENGestorEscenaCambioDef_ {
	ENGestorEscenaCambioDef_Inicio = 0,
	ENGestorEscenaCambioDef_MostrarMensaje,
	ENGestorEscenaCambioDef_AplicarCambio,
	ENGestorEscenaCambioDef_Conteo
} ENGestorEscenaCambioDef;

struct STGestorEscenaFrameBuffer_;			//Bufferes compartidos entre escenas con destinos del mismo tamano
struct STGestorEscenaEscena_;				//Escenas autorenderizada (destino RenderBuffers o Textures)
struct STGestorEscenaCapa_;					//Capas de las escenas autorenderizadas

//Nota: Inicialmente implementaba un FrameBuffer para todas las escenas. 
//El rendimiento en dispositivos iOS se reduce cosiderablemente (a mitad) cuando se intercambian adjuntos de diferentes tamanos.
//Bufferes compartidos entre escenas con destinos del mismo tamano

typedef struct STGestorEscenaFrameBuffer_ {
	bool						registroOcupado;
	ENGestorEscenaDestinoGl		tipoDestinoGL;
	GLuint						idFrameBufferGl;
	GLuint						idDepthBufferGl;			//idDepthBufferGl y idStencilBufferGl pueden ser identicos, si la implementacion limita a depth+stencil combinados (iOS 4.0+)
	GLuint						idStencilBufferGl;			//idDepthBufferGl y idStencilBufferGl pueden ser identicos, si la implementacion limita a depth+stencil combinados (iOS 4.0+)
	UI32						anchoFrameBufferGl;
	UI32						altoFrameBufferGl;
	MapaBitsColor				colorFrameBufferGl;
	//Destinos actualmente ligados
	AURenderBuffer*				renderbufferLigado;			//Renderbuffer destino actual
	AUTextura*					texturaLigada;				//Textura destino actual
	//Destinos originales del FrameBuffer
	PTRfuncGeneraRenderbuffer	funcionGeneraRenderBuffer;
	void*						funcionGeneraRenderBufferParams;
	AURenderBuffer*				renderbufferOriginal;		//Renderbuffer destino original
	AUTextura*					texturaOriginal;			//Textura destino original
	//
	bool operator==(const struct STGestorEscenaFrameBuffer_ &otro) const {
		return (idFrameBufferGl == otro.idFrameBufferGl);
	}
	bool operator!=(const struct STGestorEscenaFrameBuffer_ &otro) const {
		return !(idFrameBufferGl == otro.idFrameBufferGl);
	}
} STGestorEscenaFrameBuffer;

typedef struct STGestorEscenaTextura_ {
	bool			registroOcupado;
	bool			texturaOcupada;
	UI8				texturaModoPintado;		//ENTexturaModo
	SI32			iEscenaPropietaria;		//Escena a la que pertenece la textura
	GLuint			idFrameBufferGlPropio;
	UI32			anchoFrameBufferGl;
	UI32			altoFrameBufferGl;
	MapaBitsColor	colorFrameBufferGl;
	AUTextura*		objTexturaAtlas;		//Textura que cubre todo el area del atlas
	//
	bool operator==(const struct STGestorEscenaTextura_ &otro) const {
		return (idFrameBufferGlPropio == otro.idFrameBufferGlPropio && iEscenaPropietaria == otro.iEscenaPropietaria);
	}
	bool operator!=(const struct STGestorEscenaTextura_ &otro) const {
		return !(idFrameBufferGlPropio == otro.idFrameBufferGlPropio && iEscenaPropietaria == otro.iEscenaPropietaria);
	}
} STGestorEscenaTextura;

typedef struct STGestorEscenaFiguraRender_ {
	STBloqueGL		bloqueIndicesGL;
	bool			esTriangFan;		//Si no es triangFan, es TrianStripN
	UI32			idTextura0;
	UI32			idTextura1;
	//
	bool operator==(const struct STGestorEscenaFiguraRender_ &otro) const {
		return (bloqueIndicesGL == otro.bloqueIndicesGL && idTextura0 == otro.idTextura0 && idTextura1 == otro.idTextura1);
	}
	bool operator!=(const struct STGestorEscenaFiguraRender_ &otro) const {
		return !(bloqueIndicesGL == otro.bloqueIndicesGL && idTextura0 == otro.idTextura0 && idTextura1 == otro.idTextura1);
	}
} STGestorEscenaFiguraRender;

typedef struct STGestorEscenaTexturaLuz_ { //Render-to-Textura
	UI16			iTexRender;					//indice de la textura recicable donde se renderiza
	NBRectanguloI	areaReservada;				//area reservada en iTexRender
	STBloqueGL		indicesRenderMascLuz;		//Datos para renderizar la mascaraIndividual hacia la mascaraCombinada o escena
	//
	static void		inicializar(struct STGestorEscenaTexturaLuz_* st){
		st->iTexRender = st->areaReservada.x = st->areaReservada.y = st->areaReservada.ancho = st->areaReservada.alto = 0;
		st->indicesRenderMascLuz.primerElemento = st->indicesRenderMascLuz.cantidadElementos = 0;
	}
	//
	bool operator==(const struct STGestorEscenaTexturaLuz_ &otro) const {
		return (iTexRender == otro.iTexRender && areaReservada == otro.areaReservada && indicesRenderMascLuz == otro.indicesRenderMascLuz);
	}
	bool operator!=(const struct STGestorEscenaTexturaLuz_ &otro) const {
		return !(iTexRender == otro.iTexRender && areaReservada == otro.areaReservada && indicesRenderMascLuz == otro.indicesRenderMascLuz);
	}
} STGestorEscenaTexturaLuz;

typedef  struct STGestorEscenaLuzRender_ {
	UI16		primerFiguraLuz;			//Indice de primer "STGestorEscenaFiguraRender" en el arreglo compartido "renderMascLuzBloquesGL" a usar para renderizar la luz
	UI16		conteoFigurasLuz;
	UI16		primerFiguraDecae;			//Indice de primer "STGestorEscenaFiguraRender" en el arreglo compartido "renderMascLuzBloquesGL" a usar para renderizar la luz
	UI16		conteoFigurasDecae;
	//
	bool operator==(const struct STGestorEscenaLuzRender_ &otro) const {
		return (primerFiguraLuz == otro.primerFiguraLuz && conteoFigurasLuz == otro.conteoFigurasLuz);
	}
	bool operator!=(const struct STGestorEscenaLuzRender_ &otro) const {
		return !(primerFiguraLuz == otro.primerFiguraLuz && conteoFigurasLuz == otro.conteoFigurasLuz);
	}
} STGestorEscenaLuzRender;

typedef struct STGestorEscenaCapaRender_ {
	bool												tmpImplementarMascaraLuces;
	AUEscenaContenedor*									objetoEscenaCapa;
	NBCajaAABB											renderCajaProyeccion;				//Copia de variable para renderizado (permite seguir modificando la variable original mientras se utiliza esta copia para renderizar)
	NBMatriz											renderMatrizCapa;					//Copia de variable para renderizado (permite seguir modificando la variable original mientras se utiliza esta copia para renderizar)
	NBColor												renderColorLuzAmbiente;
	//Datos de modelos y retencion temporal
	AUArregloNativoMutableP<STPropRenderModelo>*		renderModelos;						//Arreglo de punteros a funciones que envian los comandosGL a partir de los datosModelos ordenados por indicesDatos
	AUArregloNativoMutableP<UI8>*						renderModelosDatos;					//Buffer de datos de los modelos en la secuencia
	AUArregloMutable*									renderPiscinaObjetosRetener;		//Arreglo de "AUObjetos" retenidos hasta que se renderice la escena, principalmente AUTextura. (Para evitar que el gestor libere los recursos que se utilizaran).
	//Datos de iluminacion
	AUArregloNativoMutableP<NBFuenteIluminacionRender>*	renderIluminacion;					//Fuentes de luz
	AUArregloNativoMutableP<NBFuenteEspejoRender>*		renderEspejos;						//Espejos
	AUArregloNativoMutableP<IEscenaProductorSombras*>*	renderProductoresSombras;			//Productores de sombras
	AUArregloNativoMutableP<ICuerdaCortable*>*			renderDescriptoresCuerdas;			//Descriptores de cuerdas
	AUArregloNativoMutableP<IEscenaConsumidorLuces*>*	renderConsumidoresLuces;			//Consumidores de luces
	AUArregloNativoMutableP<IEscenaConsumidorEspejos*>*	renderConsumidoresEspejos;			//Consumidores de espejos
	AUArregloNativoMutableP<IEscenaConsumidorCuerdas*>*	renderConsumidoresCuerdas;			//Consumidores de cuerdas
	AUArregloNativoMutableP<IEscenaConsumidorPreModeloGL*>*	renderConsumidoresPreModeloGL;	//Consumidores antes de crear el ModeloGL
	AUArregloMutable*									renderSombrasPorIluminacion;		//Sombras generadas por los objetos dentro de esta capa (arreglo de AUArregloNativoMutableP<NBFuenteSombra>*)
	AUArregloMutable*									renderSombrasRangosPorIluminacion;	//Rangos por los objetos dentro de esta capa (arreglo de AUArregloNativoMutableP<STRangoSombra>*)
	AUArregloNativoMutableP<NBPunto>*					renderSombrasVerticesIntermedios;	//Sombras, biblioteca compartida de vertices
	//Mascaras de iluminacion
	UI16												renderConteoLucesAfectanMascara;	//Determina si es necesario procesar la mascara de iluminacion para la capa
	UI16												renderMascLuzSumaFigurasTotal;		//Cantidad de figuras (luz + decae), debe corresponder con las contenidas en el arreglo "renderMascLuz"
	STGestorEscenaTexturaLuz							renderMascLucesCombinadas;			//Bloque de verticesGL que renderizan la mascara en la escena (la textura es retenida en la renderPiscinaObjetosRetener)
	AUArregloNativoMutableP<STGestorEscenaLuzRender>*	renderMascLuz;						//Descriptores de cada mascara de luz individual
	AUArregloNativoMutableP<STGestorEscenaFiguraRender>* renderMascLuzBloquesGL;			//Biblioteca compartida de bloquesGL
	AUArregloNativoMutableP<UI16>*						renderMascLuzLucesConIntensidad;	//Indice de luces que aplican intensidad
	//
	bool operator==(const struct STGestorEscenaCapaRender_ &otro) const {
		return (renderMatrizCapa == otro.renderMatrizCapa && objetoEscenaCapa == otro.objetoEscenaCapa
				&& renderModelos == otro.renderModelos && renderModelosDatos == otro.renderModelosDatos
				&& renderPiscinaObjetosRetener == otro.renderPiscinaObjetosRetener
				);
	}
	bool operator!=(const struct STGestorEscenaCapaRender_ &otro) const {
		return !(renderMatrizCapa == otro.renderMatrizCapa && objetoEscenaCapa == otro.objetoEscenaCapa
				 && renderModelos == otro.renderModelos && renderModelosDatos == otro.renderModelosDatos
				 && renderPiscinaObjetosRetener == otro.renderPiscinaObjetosRetener
				 );
	}
} STGestorEscenaCapaRender;

//Capas de las escenas autorenderizadas
typedef struct STGestorEscenaCapa_ {
	bool												habilitada;
	bool												tmpImplementarMascaraLuces;
	SI32												idCapa;						//identificador unico de la capa
	NBMatriz											matrizCapa;					//Matriz que modifica el puertoDeVision para esta capa
	NBColor8											colorLuzAmbiente;
	AUEscenaContenedorRaiz*								root;
	AUEscenaContenedor*									objEscena;					//objeto raiz de la capa
	AUArregloNativoMutableP<NBFuenteIluminacion>*		fuentesIluminacion;			//fuentes de iluminacion dentro de la capa
	AUArregloNativoMutableP<NBFuenteEspejo>*			fuentesEspejo;				//fuentes de espejo dentro de la capa
	AUArregloNativoMutableP<IEscenaProductorSombras*>*	productoresSombras;
	AUArregloNativoMutableP<ICuerdaCortable*>*			descriptoresCuerdas;
	AUArregloNativoMutableP<IEscenaConsumidorLuces*>*	consumidoresLuces;
	AUArregloNativoMutableP<IEscenaConsumidorEspejos*>*	consumidoresEspejos;
	AUArregloNativoMutableP<IEscenaConsumidorCuerdas*>*	consumidoresCuerdas;
	AUArregloNativoMutableP<IEscenaConsumidorPreModeloGL*>*	consumidoresPreModeloGL;
	//
	bool operator==(const struct STGestorEscenaCapa_ &otro) const {
		return (idCapa == otro.idCapa && objEscena == otro.objEscena && fuentesIluminacion == otro.fuentesIluminacion);
	}
	bool operator!=(const struct STGestorEscenaCapa_ &otro) const {
		return !(idCapa == otro.idCapa && objEscena == otro.objEscena && fuentesIluminacion == otro.fuentesIluminacion);
	}
} STGestorEscenaCapa;

//Grupos de capas
typedef struct STGestorEscenaGrupo_ {
	NBCajaAABB										cajaProyeccion;		//Caja de proyeccion del grupo
	AUArregloNativoMutableP<STGestorEscenaCapa>*	capas;
	//
	bool operator==(const struct STGestorEscenaGrupo_ &otro) const {
		return (cajaProyeccion == otro.cajaProyeccion && capas == otro.capas);
	}
	bool operator!=(const struct STGestorEscenaGrupo_ &otro) const {
		return !(cajaProyeccion == otro.cajaProyeccion && capas == otro.capas);
	}
} STGestorEscenaGrupo;

typedef struct STGestorEscenaEnlaceCapa_ { //Capa de otra escena que sera renderizada en una
	SI16			iEscena;
	SI16			idCapa;
	//
	bool operator==(const struct STGestorEscenaEnlaceCapa_ &otro) const {
		return (iEscena == otro.iEscena && idCapa == otro.idCapa);
	}
	bool operator!=(const struct STGestorEscenaEnlaceCapa_ &otro) const {
		return !(iEscena == otro.iEscena && idCapa == otro.idCapa);
	}
} STGestorEscenaEnlaceCapa;

//Escenas (destino RenderBuffers o Textures)
typedef struct STGestorEscenaEscena_ {
	bool						registroOcupado;			//Los registros de escenas son reciclables
	bool						escenaHabilitada;
	bool						pendienteRenderizar;
	bool						limpiarColorBuffer;			//Determina si durante un renderizado se debe llamar a GL::clear();
	//
	UI16						sceneMask;					//AUOBJETOESCENA_BIT_*
	SI16						iFramebufferEscena;			//Indice de framebuffer
	AUTextura*					texturaEscena;				//Textura de escena (si su destino es textura, render-to-texture)
	//
	float						escalaParaMascarasIlum;		//Escala para las texturas destinos de las mascaras de iluminacion
	float						escalaParaHD;				//Escala para determinar el tamano en unidadesEscena
	NBTamano					pixelsPorPulgadaPantalla;	//Pixels por pulgada pantalla
	NBTamano					puntosPorPulgada;			//Puntos por pulgada metricas
	NBTamano					tamanoEscenaGL;				//Tamano escena en unidadesGL
	NBTamano					tamanoEscenaEscn;			//Tamano escena en unidadesEscena
	NBMargenes					marginsPxs;					//Size of areas that should not be used for content (because are used by the device's top bar or bottom area)
	NBRectanguloI				areaOcupadaDestino;			//Recuadro que la escena ocupa en el destino
	NBRectangulo				puertoDeVision;				//Puerto de vision de la escena
	ENGestorEscenaFondoModo		fondoModo;
	NBColor						colorFondo;					//Color de fondo de la escena
	UI32						contadorIdUnicosCapas;		//
	bool						capaDebugHabilitada;
	AUEscenaTopItf*				topItf;						//Itf to the top manager (the one that presents quickTips, selectBoxes or content that must allways be on top of the screen)
	STGestorEscenaGrupo			gruposCapas[ENGestorEscenaGrupo_Conteo];
	AUArregloMutable*			agrupadoresParticulas;
	AUArregloNativoMutableP<IEscuchadorCambioPuertoVision*>* escuchadoresCambioPuertoVision;
	AUArregloNativoMutableP<STGestorEscenaEnlaceCapa>* capasEnlazadas;
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	bool						debugBloqueadoActualizandoModelos;
	#endif
	//Copia de datos para el Render
	NBRectanguloI				renderAreaOcupadaDestino;
	NBRectangulo				renderPuertoDeVision;
	NBColor						renderColorFondo;
	AUArregloNativoMutableP<STGestorEscenaCapaRender>* renderCapas;				//Biblioteca de capas render a producir. El arreglo retiene a los objetos para evitar que se eliminen durante el proce paralelo de "renderizado" y "animacion".
	AUArregloNativoMutableP<STGestorEscenaCapaRender*>* renderCapasProducir;	//Punteros de capas render a producir (toda capa solo debe ser producida su escena padre)
	AUArregloNativoMutableP<STGestorEscenaCapaRender*>* renderCapasConsumir;	//Punteros a otras capas render a consumir (las propias y las enlazadas)
	//
	bool operator==(const struct STGestorEscenaEscena_ &otro) const {
		return (registroOcupado == otro.registroOcupado && iFramebufferEscena == otro.iFramebufferEscena && areaOcupadaDestino == otro.areaOcupadaDestino);
	}
	bool operator!=(const struct STGestorEscenaEscena_ &otro) const {
		return !(registroOcupado == otro.registroOcupado && iFramebufferEscena == otro.iFramebufferEscena && areaOcupadaDestino == otro.areaOcupadaDestino);
	}
} STGestorEscenaEscena;

typedef struct STBufferVerticesGL_ {
	//Arreglo de vertices
	UI32		bytesPorRegistro; //Este tipo de dato puede reducirse hasta UI8 si conviene
	UI32		usoArregloVertices;
	UI32		tamanoArregloVertices;
	BYTE*		arregloVertices;
	//Arreglo de indices
	#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
	UI32		usoArregloIndices;
	UI32		tamanoArregloIndices;
	GLushort*	arregloIndices;
	#endif
} STBufferVerticesGL;

class NBGestorEscena {
	public:
		static bool							inicializar(const float pantallaFrecuencia);
		static void							finalizar();
		static bool							gestorInicializado();
		//
		static bool							reiniciarContextoGrafico(const bool elimObjetosAnteriores);
		//
		static float						pantallaFrecuenciaNativa();
		static float						pantallaFrecuenciaPintando();
		static bool							pantallaPermitidoRotar();
		static void							pantallaEstablecerPermitidoRotar(bool permitidoRotarPantalla);
		//
		static void							cambioDefinicionListarEscalaHaciaHD(float cambiarHacia);
		static ENGestorEscenaCambioDef		cambioDefinicionEstado();
		static void							cambioDefinicionMoverHaciaEstadoSiguiente();
		static float						cambioDefinicionEscalaHaciaHD();
        //Render
        static STNBScnRenderRef             getScnRender(void);
		//Bufferes
		static bool							bufferDatosEnLecturaMoverHaciaSiguienteNoBloqueado();
		static void							bufferDatosEnEscrituraMoverHaciaSiguiente();
		static bool							bufferDatosEnEscrituraBloqueado();
		static bool							bufferDatosEnEscrituraBloquear();
		static bool							bufferDatosEnEscrituraDesbloquear();
		static bool							bufferDatosEnEscrituraLlenar();
		static bool							bufferDatosEnEscrituraDescartar();
		//
		static bool							bufferDatosEnEscrituraMoverHaciaSiguienteNoBloqueado();
		static void							bufferDatosEnLecturaMoverHaciaSiguiente();
		static bool							bufferDatosEnLecturaBloqueado();
		static bool							bufferDatosEnLecturaBloquear();
		static bool							bufferDatosEnLecturaDesbloquear();
		static ENGestorEscenaBufferEstado	bufferDatosEnLecturaEstado();
		static bool							bufferDatosEnEscrituraVaciar();
		static bool							bufferDatosEnLecturaVaciar();
		static SI32							bufferDatosConteoBloqueados();
		static SI32							bufferDatosConteoLlenos();
		//
		static void							bufferObtenerDatos(void* param, const SI32 iVao, STVerticesDatos* guardarDatosEn);
		static SI32							debugImprimirEstadoBufferes();
		//Arreglos de vertices GL
		static NBVerticeGL0*				verticesGL0;	//Acceso optimizado hacia el arreglo de vertices sintexturas
		static NBVerticeGL*					verticesGL1;	//Acceso optimizado hacia el arreglo de vertices monotexturas
		static NBVerticeGL2*				verticesGL2;	//Acceso optimizado hacia el arreglo de vertices bitexturas
		static NBVerticeGL3*				verticesGL3;	//Acceso optimizado hacia el arreglo de vertices tritexturas
#		ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
		static GLushort*					indicesGL0;		//Acceso optimizado hacia el arreglo de indices sintexturas
		static GLushort*					indicesGL1;		//Acceso optimizado hacia el arreglo de indices monotexturas
		static GLushort*					indicesGL2;		//Acceso optimizado hacia el arreglo de indices bitexturas
		static GLushort*					indicesGL3;		//Acceso optimizado hacia el arreglo de indices tritexturas
#		endif
		static void							resetearVerticesGL();
		static UI32							reservarVerticesGL(const ENVerticeGlTipo tipo, const UI32 cantidadVerticesReservar);
		static bool							punteroVerticeGlEsValido(const ENVerticeGlTipo tipo, const void* puntero);
#		ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
		static UI32							reservarIndicesGL(const ENVerticeGlTipo tipo, const UI32 cantidadIndicesReservar);
		static STBloqueGL					reservarIndicesParaTriangStrip4Independizado(const ENVerticeGlTipo tipo, UI16 iPrimerVertice);
		static STBloqueGL					reservarIndicesParaTriangStrip4IndependizadoMultiples(const ENVerticeGlTipo tipo, UI16 iPrimerVertice, UI16 cantTriangStrips4);
		static STBloqueGL					reservarIndicesParaTriangStripIndependizado(const ENVerticeGlTipo tipo, UI16 iPrimerVertice, UI16 verticesPorTriangStrip);
		static STBloqueGL					reservarIndicesParaTriangStripIndependizadoMultiples(const ENVerticeGlTipo tipo, UI16 iPrimerVertice, UI16 verticesPorTriangStrip, UI16 cantTriangStrips);
		static STBloqueGL					reservarIndicesParaTriangStripIndependizadoDesdeTriangFan(const ENVerticeGlTipo tipo, UI16 iPrimerVertice, UI16 verticesPorTriangFan);
#		endif
		static SI32							conteoVerticesGLUsados();
		static SI32							conteoVerticesGLEnviadosHaciaBufferGL();
#		ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
		static SI32							conteoIndicesGLUsados();
#		endif
		//
		static float						cuadrosPorSegundo();
		static void							establecerCuadrosPorSegundo(float cuadrosPorSegundo);
		static SI32							ticksAcumulados();
		static void							resetearTicksAcumulados();
		//Size of areas that should not be used for content (because are used by the device's top bar or bottom area)
		static NBMargenes					getSceneMarginsScn(const SI32 iEscena, const ENGestorEscenaGrupo idGrupo);
		static NBMargenes					getSceneMarginsPx(const SI32 iEscena);
		static void							setSceneMarginsPx(const SI32 iEscena, const NBMargenes margins);
		//
		static void							agregarObjetoCapa(SI32 iEscena, ENGestorEscenaGrupo idGrupo, AUEscenaContenedor* objetoEscena, const NBColor8 &colorLuzAmbiente, bool tmpImplementarMascaraLuces=false);
		static void							quitarObjetoCapa(SI32 iEscena, AUEscenaContenedor* objetoEscena);
		static void							habilitarObjetoCapa(SI32 iEscena, AUEscenaContenedor* objetoEscena);
		static void							deshabilitarObjetoCapa(SI32 iEscena, AUEscenaContenedor* objetoEscena);
		static const STGestorEscenaCapa*	propiedadesDeCapa(SI32 iEscena, AUEscenaContenedor* objetoEscena);
		static const STGestorEscenaCapa*	propiedadesDeCapa(SI32 iEscena, SI32 idCapa);
		static NBMatriz						matrizDeCapa(const SI16 iEscena, const AUEscenaContenedor* objetoEscena);
		static NBMatriz						matrizDeCapa(const SI16 iEscena, const SI16 idCapa);
		static NBColor8						colorLuzAmbienteDeCapa(const SI16 iEscena, const AUEscenaContenedor* objetoEscena);
		static NBColor8						colorLuzAmbienteDeCapa(const SI16 iEscena, const SI16 idCapa);
		static void							establecerMatrizDeCapa(SI32 iEscena, AUEscenaContenedor* objetoEscena, NBMatriz matrizCapa);
		static void							establecerMatrizDeCapa(SI32 iEscena, SI32 idCapa, NBMatriz matrizCapa);
		static void							establecerColorLuzAmbienteCapa(const SI16 iEscena, const AUEscenaContenedor* objetoEscena, const NBColor8 &colorLuzAmbiente);
		static void							establecerColorLuzAmbienteCapa(const SI16 iEscena, const AUEscenaContenedor* objetoEscena, const UI8 colorLuzAmbienteR, const UI8 colorLuzAmbienteG, const UI8 colorLuzAmbienteB);
		static void							establecerColorLuzAmbienteCapa(const SI16 iEscena, const SI16 idCapa, const NBColor8 &colorLuzAmbiente);
		static void							establecerColorLuzAmbienteCapa(const SI16 iEscena, const SI16 idCapa, const UI8 colorLuzAmbienteR, const UI8 colorLuzAmbienteG, const UI8 colorLuzAmbienteB);
		//Enlaces de capas
		static void							enlaceCapaAgregar(SI16 iEscena, SI16 iEscenaCopiar, SI16 idCapaCopiar);
		static void							enlaceCapaQuitar(SI16 iEscena, SI16 iEscenaCopiar, SI16 idCapaCopiar);
		static void							enlaceCapaVaciar(SI16 iEscena);
		//
		static void							moverCapasHaciaEscena(SI32 iEscenaOrigen, SI32 iEscenaDestino, SI32 filtroGrupo=-1);
		static void							copiarCajasDeGrupos(SI32 iEscenaOrigen, SI32 iEscenaDestino, SI32 filtroGrupo=-1);
		static void							vaciarGrupo(SI32 iEscena, ENGestorEscenaGrupo idGrupo);
		static void							vaciarGrupos(SI32 iEscena);
		//Puertos de vision
		static NBRectangulo					puertoDeVision(SI32 iEscena);
		static NBTamano						escalaCajaProyeccionOrtogonal(SI32 iEscena, ENGestorEscenaGrupo idGrupo);
		static float						escalaEscenaParaHD(const SI32 iEscena);
		static NBPunto						coordenadaPuertoAGrupo(SI32 iEscena, ENGestorEscenaGrupo idGrupo, float xPuertoVision, float yPuertoVision);
		static NBPunto						coordenadaGrupoAPuerto(SI32 iEscena, ENGestorEscenaGrupo idGrupo, float xGrupo, float yGrupo);
		static float						anchoGrupoAPuerto(const SI32 iEscena, const ENGestorEscenaGrupo idGrupo, const float ancho);
		static float						altoGrupoAPuerto(const SI32 iEscena, const ENGestorEscenaGrupo idGrupo, const float alto);
		static float						anchoPuertoAGrupo(const SI32 iEscena, const ENGestorEscenaGrupo idGrupo, const float ancho);
		static float						altoPuertoAGrupo(const SI32 iEscena, const ENGestorEscenaGrupo idGrupo, const float alto);
		static ENGestorEscenaFondoModo		fondoModo(const SI16 iEscena);
		static NBColor						colorFondo(SI32 iEscena);
		static void							establecerColorFondo(SI32 iEscena, float r, float g, float b, float a);
		static void							establecerColorFondo(SI32 iEscena, NBColor color);
		static void							establecerFondoModo(const SI16 iEscena, const ENGestorEscenaFondoModo fondoModo);
		//Conversion de dimensiones (pantalla)
		static NBTamano						tamanoPulgadasPantalla(const SI32 iEscena);
		static NBTamano						pixelsPorPulgadaPantalla(const SI32 iEscena);
		static NBTamano						tamanoPantallaAPulgadas(const SI32 iEscena, const NBTamano tamPantalla);
		static NBTamano						tamanoPantallaAPulgadas(const SI32 iEscena, const float anchoPantalla, const float altoPantalla);
		static float						anchoPantallaAPulgadas(const SI32 iEscena, const float anchoPantalla);
		static float						altoPantallaAPulgadas(const SI32 iEscena, const float altoPantalla);
		static NBTamano						tamanoPulgadasAPantalla(const SI32 iEscena, const NBTamano tamPulgadas);
		static NBTamano						tamanoPulgadasAPantalla(const SI32 iEscena, const float anchoPulgadas, const float altoPulgadas);
		static float						anchoPulgadasAPantalla(const SI32 iEscena, const float anchoPulgadas);
		static float						altoPulgadasAPantalla(const SI32 iEscena, const float altoPulgadas);
		//Conversion de dimensiones (metricas escena)
		static NBTamano						puntosPorPulgada(const SI32 iEscena);
		static float						anchoEscenaAPulgadas(const SI32 iEscena, const float anchoEscena);
		static float						altoEscenaAPulgadas(const SI32 iEscena, const float altoEscena);
		static NBTamano						tamanoEscenaAPulgadas(const SI32 iEscena, const NBTamano tamEscena);
		static NBTamano						tamanoEscenaAPulgadas(const SI32 iEscena, const float anchoEscena, const float altoEscena);
		static NBTamano						tamanoPulgadasAEscena(const SI32 iEscena, const NBTamano tamPulgadas);
		static NBTamano						tamanoPulgadasAEscena(const SI32 iEscena, const float anchoPulgadas, const float altoPulgadas);
		static float						anchoPulgadasAEscena(const SI32 iEscena, const float anchoPulgadas);
		static float						altoPulgadasAEscena(const SI32 iEscena, const float altoPulgadas);
		//Fuentes
		static float						anchoEscenaAPuntos(const SI32 iEscena, const float anchoEscena);
		static float						altoEscenaAPuntos(const SI32 iEscena, const float altoEscena);
		static NBTamano						tamanoEscenaAPuntos(const SI32 iEscena, const NBTamano tamEscena);
		static float						anchoPuntosAEscena(const SI32 iEscena, const float tamEnPuntos72);
		static float						altoPuntosAEscena(const SI32 iEscena, const float tamEnPuntos72);
		static NBTamano						tamanoPuntosAEscena(const SI32 iEscena, const NBTamano tamEnPuntos72);
		static NBTamano						tamanoPuntosAEscena(const SI32 iEscena, const float anchoEnPuntos72, const float altoEnPuntos72);
		static AUFuenteLineas*				fuenteLineas(const SI32 iEscena, const char* nomFuente, const bool negrilla, const bool cursiva);
		static AUFuenteMapaBits*			fuenteBitmapsEnPuntos(const SI32 iEscena, const char* nomFuente, const float tamEnPuntos72, const bool negrilla, const bool cursiva);
		static AUFuenteTextura*				fuenteTexturaEnPuntos(const SI32 iEscena, const char* nomFuente, const float tamEnPuntos72, const bool negrilla, const bool cursiva);
		static AUFuenteRaster*				fuenteRasterEnPuntos(const SI32 iEscena, const char* nomFuente, const float tamEnPuntos72, const bool negrilla, const bool cursiva);
		//Cajas de proyeccion
		static NBCajaAABB					cajaProyeccionGrupo(SI32 iEscena, ENGestorEscenaGrupo idGrupo);
		static NBTamano						escalaCajaProyeccionGrupo(SI32 iEscena, ENGestorEscenaGrupo idGrupo);
		static void							normalizaCajaProyeccionEscena(SI32 iEscena);
		static void							normalizaMatricesCapasEscena(SI32 iEscena);
		static void							establecerCajaProyeccion(SI32 iEscena, float xMin, float xMax, float yMin, float yMax);
		//Ayudas para pantallas pequenas
		static bool							ayudasParaPantallaPequena();
		static void							establecerAyudasParaPantallasPequenas(bool ayudasActivas);
		//Iluminacion
		static bool							lucesSombrasActivos();
		static void							establecerLucesSombrasActivos(bool lucesSombrasActivos);
		static void							agregarFuenteIluminacion(SI32 iEscena, AUEscenaObjeto* objEscena, AUObjeto* objAgrupaSombras, AUTextura* texrutaLuz, float alcanceLuz, ENFuenteIlumTipo tipoLuz, bool produceSombras, bool limitesAngulosActivos, float radianesMin, float radianesApertura);
		static void							actualizarFuenteIluminacion(SI32 iEscena, AUEscenaObjeto* objEscena, AUObjeto* objAgrupaSombras, AUTextura* texrutaLuz, float alcanceLuz, ENFuenteIlumTipo tipoLuz, bool produceSombras, bool limitesAngulosActivos, float radianesMin, float radianesApertura);
		static void							quitarFuenteIluminacion(SI32 iEscena, AUEscenaObjeto* objEscena);
		//Iluminacion, productores de sombras
		static void							agregarProductorSombras(SI32 iEscena, SI32 iCapa, IEscenaProductorSombras* productor);
		static void							quitarProductorSombras(SI32 iEscena, SI32 iCapa, IEscenaProductorSombras* productor);
		//Cuerdas cortables
		static void							agregarDescriptorCuerda(SI32 iEscena, SI32 iCapa, ICuerdaCortable* cuerdaCortable);
		static void							quitarDescriptorCuerda(SI32 iEscena, SI32 iCapa, ICuerdaCortable* cuerdaCortable);
		//Iluminacion, consumidores de luces
		static void							agregarConsumidorLuces(SI32 iEscena, SI32 iCapa, IEscenaConsumidorLuces* consumidor);
		static void							quitarConsumidorLuces(SI32 iEscena, SI32 iCapa, IEscenaConsumidorLuces* consumidor);
		//Iluminacion, consumidores de espejos
		static void							agregarConsumidorEspejos(SI32 iEscena, SI32 iCapa, IEscenaConsumidorEspejos* consumidor);
		static void							quitarConsumidorEspejos(SI32 iEscena, SI32 iCapa, IEscenaConsumidorEspejos* consumidor);
		//Cuerdas, consumidores de cuerdas
		static void							agregarConsumidorCuerdas(SI32 iEscena, SI32 iCapa, IEscenaConsumidorCuerdas* consumidor);
		static void							quitarConsumidorCuerdas(SI32 iEscena, SI32 iCapa, IEscenaConsumidorCuerdas* consumidor);
		//Consumidores que requieren ejecutar calculos antes de generar el modeloGL
		static void							agregarConsumidorPreModeloGL(SI32 iEscena, SI32 iCapa, IEscenaConsumidorPreModeloGL* consumidor);
		static void							quitarConsumidorPreModeloGL(SI32 iEscena, SI32 iCapa, IEscenaConsumidorPreModeloGL* consumidor);
		//Iluminacion, espejos de luz
		static void							agregarFuenteEspejo(SI32 iEscena, AUEscenaObjeto* objEscena, float factorReflejoLuz, NBPunto limiteLocalIni, NBPunto limiteLocalFin);
		static void							actualizarFuenteEspejo(SI32 iEscena, AUEscenaObjeto* objEscena, float factorReflejoLuz, NBPunto limiteLocalIni, NBPunto limiteLocalFin);
		static void							quitarFuenteEspejo(SI32 iEscena, AUEscenaObjeto* objEscena);
		static STGestorEscenaTextura*		tmpTexturasAtlasRender();
		//Frame buffers (render-to-texture)
		static SI32							crearEscena(UI16 anchoEscenaGL, UI16 altoEscenaGL, float escalaParaHD, float escalaParaMascarasIlum, const float pixelsPorPulgadaPantallaAncho, const float pixelsPorPulgadaPantallaAlto, float puntosPorPulgadaEscenaAncho, float puntosPorPulgadaEscenaAlto, MapaBitsColor color, ENGestorEscenaDestinoGl tipoDestino, PTRfuncGeneraRenderbuffer opcionalFuncionQueGeneraDatosDeRenderBuffer = NULL, void* datoUsuario = NULL); //idGlFrameBuffer, anchoRender, altoRender, datoUsuario
		static void							setTopItf(SI32 iEscena, AUEscenaTopItf* topItf);
		static bool							registroEscenaOcupado(SI32 iEscena);
		static void							liberarEscena(SI32 iEscena);
		static void							liberarRecursosCacheRenderEscenas();
		static void							habilitarEscena(SI32 iEscena);
		static void							deshabilitarEscena(SI32 iEscena);
		static bool							escenaLimpiaColorBuffer(const SI32 iEscena);
		static void							establecerEscenaLimpiaColorBuffer(const SI32 iEscena, const bool limpiaBuffer);
		static UI32							idGlFrameBufferParaEscena(SI32 iEscena);
		static STGestorEscenaFrameBuffer	propiedadesFramebuffer(SI32 iFramebuffer);
		static STGestorEscenaEscena 		propiedadesEscena(SI32 iEscena);
		static AUTextura*					texturaEscena(SI32 iEscena);
		static void							redimensionarEscena(SI32 iEscena, UI16 anchoEscena, UI16 altoEscena, float escalaParaHD, const NBTamano ppiScreen, const NBTamano dpiScene, float escalaParaMascarasIlum, bool notificar=true, PTRfuncGeneraRenderbuffer opcionalFuncionQueGeneraDatosDeRenderBuffer = NULL, void* datoUsuario = NULL); //idGlFrameBuffer, anchoRender, altoRender, datoUsuario
		static void							notificarObjetosTamanoEscena(SI32 iEscena, const STNBViewPortSize before, const STNBViewPortSize after);
		//
		static AUArreglo*					agrupadoresParticulas(SI32 iEscena);
		static void							agregarAgrupadorParticulas(SI32 iEscena, AUEscenaGrupoParticulas* grupoParticulas);
		static void							quitarAgrupadorParticulas(SI32 iEscena, AUEscenaGrupoParticulas* grupoParticulas);
		//
		static void							agregarEscuchadorCambioPuertoVision(SI32 iEscena, IEscuchadorCambioPuertoVision* _escuchador);
		static void							quitarEscuchadorCambioPuertoVision(SI32 iEscena, IEscuchadorCambioPuertoVision* _escuchador);
		static bool							capaDebugHabilitada(SI32 iEscena);
		static void							establecerCapaDebugHabilitada(SI32 iEscena, bool habilitada);
		//
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		static void							debugMarcarBloqueoModelosEscena(const bool bloqueados);
		#endif
		static UI32							secuencialActualizacionesModelos();
		static UI32							secuencialActualizacionesModelosGL();
		static UI32							secuencialRenderizadasModelosGL();
		static void							actualizarMatricesYModelos(STNBSceneModelsResult* dst);
		static void							consumirModelosYProducirBufferRender();
		static void							consumirBufferRenderYEnviarComandosDibujo();
		//Touches
		static void							touchesProcesar();
		static void							touchesOwnerWillBeRemovedFromScene(AUEscenaObjeto* obj, const SI32 touchesOwnedCount, void* param);
		static AUEscenaObjeto* 				touchObjetoQueConsume(const SI32 posXPuerto, const SI32 posYPuerto, const UI8 touchFilterMask, NBPunto* outPosEscena, AUObjeto** outConsumidorEscuchadorObj, void** outConsumidorEscuchadorInterfaz);
		static AUEscenaObjeto* 				touchObjetoQueConsumeSegunArreglo(const SI32 posXPuerto, const SI32 posYPuerto, const UI8 touchFilterMask, const AUArreglo* arregloFiltroObjetosEscena);
		//
		#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
		static UI32							debugBytesDeRenderBuffers();
		#endif
		//static void						debug_verificaNivelEnEscena();
		static UI32							cantidadLucesRenderizadas();
		static UI32							cantidadEscenasRenderizadas();
		static UI32							cantidadCambiosFrameBuffer();
	private:
		static bool							_gestorInicializado;
		static bool							_lucesSombrasActivos;
		static bool							_ayudasParaPantallaPequena;
		static float						_pantallaFrecuenciaNativa;
		static float						_pantallaFrecuenciaPintando;
		static ENGestorEscenaCambioDef		_cambioDefinicioEstado;
		static float						_cambioDefinicionEscalaHaciaHD;
		static float						_cuadrosPorSegundo;
		static SI32							_ticksAcumulados;
		static UI32							_secuencialActualizacionesModelos;
		static UI32							_secuencialActualizacionesModelosGL;
		static UI32							_secuencialRenderizadasModelosGL;
		static bool							_pantallaPermitidoRotar;
		static STGestorEscenaTextura		_texturasParaRenderizadoGL[NBGESTORESCENA_MAX_TEXTURAS_RENDER];
		static STGestorEscenaFrameBuffer	_frameBuffersGL[NBGESTORESCENA_MAX_ESCENAS];
		static STGestorEscenaEscena			_escenas[NBGESTORESCENA_MAX_ESCENAS];
		//
		static AUArregloNativoMutableP<STRangoSombra>* _cacheSombrasFusionadas;
		//Bufferes de datos
		static ENGestorEscenaBufferEstado	_bufferDatosEstado;
		static bool							_bufferDatosBloqueado;
		static STBufferVerticesGL			_buffersVertices[ENVerticeGlTipo_Conteo];
        static STNBScnRenderRef             _scnRender;
		//
		static void							privIncializarConfigurarGL();
		//
		static SI32							privCrearEscena(SI32 iEscenaLibre, bool formatearRegistro, UI16 anchoEscenaGL, UI16 altoEscenaGL, float escalaParaHD, float escalaParaMascarasIlum, const float pixelsPorPulgadaPantallaAncho, const float pixelsPorPulgadaPantallaAlto, float puntosPorPulgadaAncho, float puntosPorPulgadaAlto, MapaBitsColor color, ENGestorEscenaDestinoGl tipoDestino, PTRfuncGeneraRenderbuffer opcionalFuncionQueGeneraDatosDeRenderBuffer, void* datoUsuario);
		static void							privLiberarEscena(SI32 iEscena, bool formatearRegistro);
		static void							privActualizarMatricesYModelosDeEscena(const SI32 iEscena, STNBSceneModelsResult* dst);
		static void							privInicializarCacheRender(AUArregloNativoMutableP<STGestorEscenaCapaRender>* capasRender, const STGestorEscenaCapa* propiedadesDeCapa, const NBCajaAABB &cajaCapa, const UI16 indiceRegistroUsar);
		static void							privInicializarCacheIluminacion(STGestorEscenaCapaRender* renderCapa, UI16 iIlum);
		static void							privActualizarCajaEscenaLuz(NBFuenteIluminacionRender &datosRender);
		//
		static bool							privBufferDatosVaciar(void);
		//Producir luces
		static void							privProducirSombrasYLucesReflejadas();
		static void							privProducirSombrasEnCapa(STGestorEscenaCapaRender* renderCapa, const UI16 iPrimeraLuz);
		static void							privProducirLucesPorReflejoEnCapa(STGestorEscenaCapaRender* renderCapa, const UI16 iPrimeraLuz);
		static void							privConsumirCuerdasEnCapa(STGestorEscenaCapaRender* renderCapa);
		static void							privConsumirEspejosEnCapa(STGestorEscenaCapaRender* renderCapa);
		static void							privConsumirPreModeloGLEnCapa(STGestorEscenaCapaRender* renderCapa);
		static void							privConsumirLucesEnCapa(STGestorEscenaCapaRender* renderCapa);
		static void							privProducirMascarasIluminacion();
		static void							privProducirMascaraIluminacionDeCapa(SI32 iEscena, STGestorEscenaCapaRender* renderCapa, const NBCajaAABB &cajaEscena);
		static void							privIlumAgregarFiguraLuzDecaeNormalizadaNoLimitada(STGestorEscenaCapaRender* renderCapa, const NBFuenteIluminacionRender* datosLuz, const NBPunto* vertsContrarelojLuz, const NBPunto* verticeFig, const UI16 conteoVertsFig, const NBPunto &texPosCero, const NBPunto &texVecUnitCreceX, const NBPunto &texVecUnitCreceY, const float texMaxCrecimientoX, const float texMaxCrecimientoY);
		static void							privIlumAgregarFiguraLuzDecaeNormalizadaLimitada(STGestorEscenaCapaRender* renderCapa, const NBFuenteIluminacionRender* datosLuz, const NBPunto* vertsContrarelojLuz, const NBPunto* verticeFig, const UI16 conteoVertsFig, const NBPunto &posVectLimite, const NBPunto &vecLimite, const NBPunto &texPosCero, const NBPunto &texVecUnitCreceX, const NBPunto &texVecUnitCreceY, const float texMaxCrecimientoX, const float texMaxCrecimientoY);
		//static void						privIlumAgregarFiguraLuzDecaeSimpleNoLimitada(STGestorEscenaCapaRender* renderCapa, const NBFuenteIluminacionRender* datosLuz, const NBPunto* vertsContrarelojLuz, const NBPunto* verticeFig, const UI16 conteoVertsFig, const NBPunto &texPosCero, const NBPunto &texVecUnitCreceX, const NBPunto &texVecUnitCreceY, const float texMaxCrecimientoX, const float texMaxCrecimientoY);
		//static void						privIlumAgregarFiguraLuzDecaeSimpleLimitada(STGestorEscenaCapaRender* renderCapa, const NBFuenteIluminacionRender* datosLuz, const NBPunto* vertsContrarelojLuz, const NBPunto* verticeFig, const UI16 conteoVertsFig, const NBPunto &posVectLimite, const NBPunto &vecLimite, const NBPunto &texPosCero, const NBPunto &texVecUnitCreceX, const NBPunto &texVecUnitCreceY, const float texMaxCrecimientoX, const float texMaxCrecimientoY);
		static void							privIlumAgregarFiguraLuzDesdeCroquisUnico(STGestorEscenaCapaRender* renderCapa, const NBFuenteIluminacionRender* datosLuz, const NBPunto* vertices, const UI16 cantidadVertices, const NBPunto &texPosCero, const NBPunto &texVecUnitCreceX, const NBPunto &texVecUnitCreceY, const float texMaxCrecimientoX, const float texMaxCrecimientoY);
		static void							privIlumAcumularSombraEnCache(const NBPunto &vectorAngMin, const NBPunto &vectorAngMax);
		//static bool						privIlumSombraIntersectaFigura(const NBPunto* verticesSombra, const UI16 conteoVerticesSombra, const NBPunto* vertices4Figura);
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		static bool							privDebugFiguraEsConvexaContrareloj(const NBPunto* vertices, const SI32 conteoVertices);
		#endif
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		static void							privDebugMarcarBloqueoModelosEscena(const SI32 iEscena, const bool bloqueados);
		#endif
		//Dibujar GL
		static void							privConsumirModelosYProducirBufferRenderDeEscena(const SI32 iEscena);
		static void							privConsumirBufferRenderYEnviarComandosDibujoDeEscenaEnOrdenDirecto(const SI32 iEscena, bool &frameBufferLimpiado, NBColor &colorFondoLimpiado);
		static void							privConsumirBufferRenderYEnviarComandosDibujoDeEscenaPriorizandoUnaCapaConLuz(const SI32 iEscena, bool &frameBufferLimpiado, NBColor &colorFondoLimpiado);
		static void							privConsumirBufferRenderYEnviarComandosDibujoDeCapa(const STGestorEscenaFrameBuffer* datosFB, const STGestorEscenaEscena* escena, const STGestorEscenaCapaRender* renderCapa, const bool enOrdenDirecto, const NBColor colorMultiplicador, bool &frameBufferLimpiado, NBColor &colorFondoLimpiado, bool &escenaInicializada);
		static void							privAsegurarFrameBufferInicializadoParaEscena(const STGestorEscenaFrameBuffer* datosFB, const STGestorEscenaEscena* escena, bool &frameBufferLimpiado, NBColor &colorFondoLimpiado, bool &escenaInicializada);
		static void							privEnviarComandosDibujarMascarasIluminacion();
		//Texturas de mascaras de luz
		static SI32							privTexturaRenderReservarEspacioSinCrearDestino(const SI32 iEscenaPropietaria, const MapaBitsColor colorFB, const ENTexturaModoPintado modoPintadoTextura, const UI16 anchoNecesario, const UI16 altoNecesario, NBRectanguloI* guardarAreaReservadaEn);
		static void							privTexturaRenderAsegurarDestinosCreados(const SI32 iEscenaPropietaria);
		static void							privTexturaRenderVaciarReservasDeBuffer(void);
		static void							privTexturaRenderLiberarReservasDeEscena(const SI32 iEscenaPropietaria);
		static void							privTexturaRenderLiberarReservasTodas();
		//Framebuffers
		static void							privFrameBufferEnlazarTextura(SI32 iFrameBuffer, AUTextura* texRender);
		static void							privFrameBufferEnlazarRenderBuffer(SI32 iFrameBuffer, AURenderBuffer* renderBuffer);
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_FRAMEBUFFERS_COMPLETOS
		static bool							privFrameBufferCheckStatusEXT();
		#endif
		//
		static STGestorEscenaCapa*			privPropiedadesCapaPorID(SI32 iEscena, int idCapa);
		static STGestorEscenaCapa*			privPropiedadesCapaPorObjetoEscena(SI32 iEscena, AUEscenaContenedor* objEscena);
};

#endif
