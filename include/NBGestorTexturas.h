#ifndef NBGESTORTEXTURAS_H_INCLUIDO
#define NBGESTORTEXTURAS_H_INCLUIDO

#include "NBGestorPilaLlamadas.h"
#include "NBGestorGL.h"
#include "NBGestorArchivos.h"
#include "nb/fonts/NBFontGlyphs.h"
#include "nb/fonts/NBFontLines.h"
#include "nb/fonts/NBFontBitmaps.h"
#include "nb/fonts/NBFontsGlyphsStore.h"
#include "nb/fonts/NBFontsLinesStore.h"
#include "nb/fonts/NBFontsBitmapsStore.h"
#include "AUArregloMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUCadenaMutable.h"
#include "AUAtlasMapaMutable.h"
#include "AUMapaBitsMutable.h"
#include "AUMapaBitsLienzo.h"
#include "AUTextura.h"
#include "AUFuenteTextura.h"
#include "AUFuenteRaster.h"
#include "NBColor.h"
#include "AUHilo.h"
#include "nb/crypto/NBSha1.h"
//
#include "nb/2d/NBPng.h"
#include "nb/2d/NBPngChunk.h"
#include "nb/2d/NBJpeg.h"
#include "nb/2d/NBJpegRead.h"
#include "nb/2d/NBJpegWrite.h"

#define GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS	4 //Cada ambito guarda sus propias texturas (las texturas pueden duplicarse en distintos ambitos)
#define GESTOR_TEXTURAS_ATLAS_DIMENSION_MINIMA	512
#define GESTOR_TEXTURAS_ATLAS_DIMENSION_MAXIMA	1024 //2048

#define GESTOR_TEXTURAS_SEMAFORO_INI		while(_bloqueado || _semaforo); _semaforo = true;
#define GESTOR_TEXTURAS_SEMAFORO_FIN		_semaforo = false;

//----------------------------------

enum ENGestorTexturaModo {
	ENGestorTexturaModo_cargaInmediata = 0,	//cargar los pixeles inmediatamente
	ENGestorTexturaModo_cargaSegundoPlano,	//cargar pixeles en un hilo secundario
	ENGestorTexturaModo_cargaPendiente,		//cargar los datos descriptivos pero dejar pendiente la carga de pixeles
	ENGestorTexturaModo_conteo
};

enum ENGestorTexturaOrigen {
	ENGestorTexturaOrigen_paquete = 0,	//ruta proviene de paquete (escala escena)
	ENGestorTexturaOrigen_paquetePantallaPPP,	//ruta proviene de paquete (escala pantalla)
	ENGestorTexturaOrigen_rutaFisica,	//ruta es fisica
	ENGestorTexturaOrigen_datos			//no existe ruta a archivo
};

enum ENTexturaTipoAlmacenamientoGL {
	ENTexturaTipoAlmacenamientoGL_AtlasCompartido,	//la textura compartira atlas con otras
	ENTexturaTipoAlmacenamientoGL_AtlasUnico		//la textura es cuadrada-base-2 y sera la unica en el atlas
};

enum ENTexturaTipoUsoGL {
	ENTExturaTipoUsoGL_Lectura,				//la textura solo es usada como fuente de dibujo
	ENTExturaTipoUsoGL_EscrituraLectura		//la textura sera usada como destino y fuente de dibujo
};

enum ENTexturaMipMap {
	ENTexturaMipMap_Inhabilitado	= 0,	//MipMap no-habilitado en la textura/atlas
	ENTexturaMipMap_Habilitado		= 1		//MipMap habilitado en la textura/atlas
};

//----------------------------------

//Loading states
typedef enum ENGestorTexLoadingType_ {
	ENGestorTexturasLoadingType_Png = 0,
	ENGestorTexturasLoadingType_Jpg,
	ENGestorTexturasLoadingType_Count
} ENGestorTexturasLoadingType;

typedef struct STGestoTexLoadingBuffer_ {
	BYTE*	data;			//Buffer data
	SI32	size;			//Total size in bytes
	SI32	use;			//Used size in bytes
	SI32	linesLoaded;	//Lines of the image loaded
} STGestoTexLoadingBuffer;

typedef struct STGestoTexLoadingBase_ {
	ENGestorTexturasLoadingType type;
	//
	AUArchivo*			archivo;
	//
	ENGestorTexturaOrigen origenTextura;
	UI8					escalaBase2;
	float				escalaParaHD;
	AUCadena8*			enListarConNombre;
	SI32				nomTamano;
	ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento;
	ENTexturaTipoUsoGL	tipoUso;
	ENTexturaModoPintado modoPintado;
	ENTexturaMipMap		habilitarMipMap;
	ENTexturaOrdenV		ordenVTextura;
	//
	GLenum				formatoGlLocal;
	GLenum				tpoDatoGlLocal;
	//Loading buffers
	bool				linesLoadingBuffersInited;
	bool				linesLoadingEnded;
	SI32				linesAlreadyPasted;		//lines already pasted to textureGL
	SI32				linesAlreadyLoaded;		//lines already loaded from resource
	NBHILO_MUTEX_CLASE	linesBuffersMutex;
	AUArregloNativoMutableP<STGestoTexLoadingBuffer>* linesBuffers;
} STGestoTexLoadingBase;

typedef struct STGestoTexLoadingPNG_ : public STGestoTexLoadingBase {
	STNBPngLoadState	load;
	STNBArray			extraChunks;	//STNBPngChunk
} STGestoTexLoadingPNG;

typedef struct STGestoTexLoadingJPEG_ : public STGestoTexLoadingBase {
	void*	estado;
	BYTE*	buff;
	UI32	buffSz;
} STGestoTexLoadingJPEG;

//Textures

enum ENTexturaOrigenFormato {
	ENTexturaOrigenFormato_Png = 0,
	ENTexturaOrigenFormato_Jpg,
	ENTexturaOrigenFormato_Conteo
};

struct STGestorTexTextura {
	SI32							retainCount;	//For the multithread textures loading system (avoid releasing objects while producer/consumer is using them)
	//
	AUTextura*						textura;
	UI32							codigoTextura;
	ENGestorTexturaOrigen			origenTextura;
	ENTexturaOrigenFormato			origenFormato;
	UI8								escalaBase2;
	float							escalaParaHD;
	AUCadena8*						nombreTextura;	//Redundat with name-at-atlas but necesary for preloaded textures
	//Subtexturas
	AUAtlasMapaMutable*				subTexturasMapa;
	AUArregloMutable*				subTexturas;
	//
	ENTexturaTipoAlmacenamientoGL	tipoAlmacenamientoGL;
	ENTexturaTipoUsoGL				tipoUsoGL;
	ENTexturaModoPintado			tipoPintado;
	ENTexturaMipMap					mipMapHabiltado;
	//
	STGestoTexLoadingBase*			loadingState;	//Loading state, NULL if exture was completly loaded
	//
	bool operator == (const STGestorTexTextura &otro) const {
		return (textura == otro.textura);
	}
	bool operator!=(const STGestorTexTextura &otro) const {
		return !(textura == otro.textura);
	}
};

struct STGestorTexAtlas {
	SI32							retainCount;	//For the multithread textures loading system (avoid releasing objects while producer/consumer is using them)
	//
	STGestorGLTextura				propsTexturaGL;					//Propiedades de la texturaGL (incluyendo el idGL)
	MapaBitsColor					colorFormato;					//Formato de almacenamiento de mapas de bits (actualmente acepta: COLOR_GRIS8, COLOR_GRISALPHA8, COLOR_RGB8, COLOR_RGBA8 y COLOR_BGRA8)
	ENTexturaTipoAlmacenamientoGL	tipoAlmacenamientoGL;
	ENTexturaTipoUsoGL				tipoUsoGL;
	ENTexturaModoPintado			modoPintado;
	AUAtlasMapaMutable*				mapaAtlas;						//Mapa de los rectangulos en el atlas, NULL for preloaded atlas.
	//NBHILO_MUTEX_CLASE			texturasEnMapaMutex;
	AUArregloNativoMutableP<STGestorTexTextura*>* texturasEnMapa;	//Instancias AUTextura para cada rectangulo en el atlas (compartida a todos los objEscena).
	//
	bool operator == (const STGestorTexAtlas &otro) const {
		return (propsTexturaGL.idTexturaGL == otro.propsTexturaGL.idTexturaGL && mapaAtlas == otro.mapaAtlas && texturasEnMapa == otro.texturasEnMapa);
	}
	bool operator!=(const STGestorTexAtlas &otro) const {
		return !(propsTexturaGL.idTexturaGL == otro.propsTexturaGL.idTexturaGL && mapaAtlas == otro.mapaAtlas && texturasEnMapa == otro.texturasEnMapa);
	}
};

//Texture reference
typedef struct STGestorTexTexturaRef_ {
	SI32 iAmb;
	STGestorTexAtlas* atlas;
	STGestorTexTextura* texture;
	//
	bool operator == (const STGestorTexTexturaRef_ &otro) const {
		return (iAmb == otro.iAmb && atlas == otro.atlas && texture == otro.texture);
	}
	bool operator!=(const STGestorTexTexturaRef_ &otro) const {
		return !(iAmb == otro.iAmb && atlas == otro.atlas && texture == otro.texture);
	}
} STGestorTexTexturaRef;

//----------------------------------

struct STGestorTexFuenteTex {
	UI32				version;
	SI32				iAtlasEnFuente;			//Indice de atlas en FuenteMapaBits
	STGestorGLTextura	propsTexturaGL;			//Propiedades de la texturaGL (incluyendo el idGL)
	MapaBitsColor		colorFormato;			//Formato de almacenamiento de mapas de bits (actualmente acepta: COLOR_GRIS8, COLOR_GRISALPHA8, COLOR_RGB8 y COLOR_RGBA8)
	//AUAtlasMapaMutable*	mapaAtlas;			//Mapa de los rectangulos en el atlas
};

struct STGestorTexFuente {
	UI32				version;
	STNBFontBitmaps*	fontBitmaps;
	AUFuenteTextura* 	texturaFuente;			//Datos de la fuente
	AUArregloNativoMutableP<STGestorTexFuenteTex>* texturas;
	//
	bool operator == (const STGestorTexFuente &otro) const {
		return (texturaFuente == otro.texturaFuente);
	}
	bool operator!=(const STGestorTexFuente &otro) const {
		return !(texturaFuente == otro.texturaFuente);
	}
};

struct STGestorTexFuenteVAO {
	STNBFontLines*		fuenteLineas;		//Definicion
	float				tamano;				//Tamano
	UI32				versionEnVao;		//Version de los datos enviados al VAO (los datos se actualizan cuando difiere de la version de la fuenteLineas)
	SI32				iVao;				//Identificados del VAO
	bool operator == (const STGestorTexFuenteVAO &otro) const { return (fuenteLineas == otro.fuenteLineas && tamano == otro.tamano); }
	bool operator!=(const STGestorTexFuenteVAO &otro) const { return (fuenteLineas != otro.fuenteLineas || tamano != otro.tamano); }
	bool operator<(const STGestorTexFuenteVAO &otro) const { return (fuenteLineas < otro.fuenteLineas || (fuenteLineas == otro.fuenteLineas && tamano < otro.tamano)); }
	bool operator<=(const STGestorTexFuenteVAO &otro) const { return (fuenteLineas < otro.fuenteLineas || (fuenteLineas == otro.fuenteLineas && tamano <= otro.tamano)); }
	bool operator>(const STGestorTexFuenteVAO &otro) const { return (fuenteLineas > otro.fuenteLineas || (fuenteLineas == otro.fuenteLineas && tamano > otro.tamano)); }
	bool operator>=(const STGestorTexFuenteVAO &otro) const { return (fuenteLineas > otro.fuenteLineas || (fuenteLineas == otro.fuenteLineas && tamano >= otro.tamano)); }
};

struct STGestorTexFuenteR {
	STNBFontLines*		fontLines;
	AUFuenteRaster* 	fuenteRaster;			//Datos de la fuente
	//
	bool operator == (const STGestorTexFuenteR &otro) const {
		return (fuenteRaster == otro.fuenteRaster);
	}
	bool operator!=(const STGestorTexFuenteR &otro) const {
		return !(fuenteRaster == otro.fuenteRaster);
	}
};

//Ambito de carga de
struct STGestorTexAmbito {
	bool											ambitoReservado;
	UI16											tamanosTexturasPrincipales; //Texturas con forma RGBA y GRIS
	UI16											tamanosTexturasSecundarias; //Texturas con forma RGB, GRIS+ALPHA y ALPHA
	//NBHILO_MUTEX_CLASE							atlasesTexturasMutex;
	AUArregloNativoMutableP<STGestorTexAtlas*>*		atlasesTexturas;
};

//----------------------------------

typedef enum ENTexturaModoHilo_ {
	ENTexturaModoHilo_MonoHilo = 0,
	ENTexturaModoHilo_MultiHilo,
	ENTexturaModoHilo_Conteo
} ENTexturaModoHilo;

//-----------------------
// Textures props per resolution
//-----------------------

typedef enum ENTexturaResolucion_ {
	ENTexturaResolucion_Escena = 0,
	ENTexturaResolucion_Pantalla,
	ENTexturaResolucion_Conteo
} ENTexturaResolucion;

typedef enum ENTexturaPropsSetStatus_ {
	ENTexturaPropsSetStatus_Unset = 0,
	ENTexturaPropsSetStatus_Implicit,
	ENTexturaPropsSetStatus_Explicit,
	ENTexturaPropsSetStatus_Count
} ENTexturaPropsSetStatus;

typedef struct STTexturaResProps_ {
	ENTexturaPropsSetStatus setStatus;
	AUCadenaMutable8*	pathPrefix;	//prefijo de rutas para imagenes PNG (ej: "./PNG/X4/", "./PNG/X2/", "./PNG/X1/")
	UI16				scaleBase2;			//32, 64, 128, 256 ... escala que determina el divisor/multiplicador por cada atlas o malla cargado (escala de diseno vs escala de presentacion)
	float				scaleToHD;			//escala a aplicar a las PNG para ajustar a HighDef (ejem: "./PNG/X4/" = 1, "./PNG/X2/" = 1, "./PNG/X1/" = 2)
} STTexturaResProps;

//

class NBGestorTexturas {
	public:
		static bool					inicializar(const ENTexturaModoHilo modoOperacion, UI16 tamanosTexturasPrincipales, UI16 tamanosTexturasSecundarias);
		static void					finalizar();
		static bool					gestorInicializado();
		static bool					reiniciarContextoGrafico(const bool elimObjetosAnteriores);
		//Prefijos de rutas
		static const char*			pefijoRutas(const ENTexturaResolucion res = ENTexturaResolucion_Escena);
		static void					establecerPrefijoRutas(const char* prefijoStr, const UI16 escalaTexturasBase2, const float escalaTexturasHaciaHD, const ENTexturaResolucion res);
		static SI32					escalaTexturasBase2(const ENTexturaResolucion res = ENTexturaResolucion_Escena);
		static float				escalaTexturasHaciaHD(const ENTexturaResolucion res = ENTexturaResolucion_Escena);
		//Modo de operacion
		static ENGestorTexturaModo	modoCargaTexturas();
		static void					modoCargaTexturasPush(const ENGestorTexturaModo nuevoModo);
		static void					modoCargaTexturasPop();
		//Unorganized textures (preloaded textures)
		static SI32					texPendienteOrganizarConteo();
		static void					texPendienteOrganizarProcesar(const SI32 maxProcesar);
		//Font and other textures sync
		static void					texSyncAny();
		static UI32					texsChangesCount(const BOOL resetCount);
		//Background loading textures
		static SI32					texCargandoSegundoPlanoConteo();
		static SI32					texCargandoSegundoPlanoProcesarBufferes();
		//Volver a cargar texturas
		static void					recargarTexturas(); //Util para cambios de resolucion, cambios peferencias de video o al restaurar app (android)
		//Carga de texturas
		static bool					trabajadorSegundoPlanoIniciar();
		static void					trabajadorSegundoPlanoDetenerYEsperar();
		static void					funcHiloCargaDatosTexturas(void* param);
		//Ambito de texturas
		static UI8					indiceAmbitoTexturasActual();
		static bool					activarAmbitoTexturas(UI8 iAmbito);
		static void					establecerTamanosTexturasDeAmbito(UI8 iAmbito, UI16 tamanosTexturasPrincipales, UI16 tamanosTexturasSecundarias);
		static UI8					reservarAmbitoTexturas(UI16 tamTexturasPrincipales, UI16 tamTexturasSecundarias);
		static void					liberarAmbitoTexturas(UI8 iAmbitoTexturas);
		//Texturas
		static AUTextura*			texturaBlanca; //required on some systems where texture-0 will not be drawn
		static void					rutaArchivoTextura(AUCadenaMutable8* dst, UI8* dstEscalaBase2, float* dstEscalaParaHD, const char* nombre, const ENGestorTexturaOrigen origenTextura = ENGestorTexturaOrigen_paquete);
		static AUTextura*			texturaDesdeArchivo(const char* nombre, const ENGestorTexturaOrigen origenTextura = ENGestorTexturaOrigen_paquete, const UI8 escalaBase2 = 1, const float escalaParaHD = 1.0f);
		static AUTextura*			texturaDesdeArchivoPNG(const char* nombrePNG, const ENGestorTexturaOrigen origenTextura = ENGestorTexturaOrigen_paquete, const UI8 escalaBase2 = 1, const float escalaParaHD = 1.0f);
		static AUTextura*			texturaDesdeArchivoJPEG(const char* nombreJPEG, const ENGestorTexturaOrigen origenTextura = ENGestorTexturaOrigen_paquete, const UI8 escalaBase2 = 1, const float escalaParaHD = 1.0f);
		static AUTextura*			texturaDesdeMapaBits(AUMapaBits* mapaBitsACargar, const char* guardarConNombre, const ENTexturaModoPintado modoPintado, const ENTexturaMipMap habilitarMipMap, const ENTexturaOrdenV ordenVTextura, const UI8 escalaBase2, const float escalaParaHD);
		static AUTextura*			texturaDesdeDatos(const MapaDeBitsDesciptor* props, const void* data, const ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, const ENTexturaTipoUsoGL tipoUso, const ENTexturaModoPintado modoPintado, const ENTexturaMipMap habilitarMipMap, const ENTexturaOrdenV ordenVTextura, const UI8 escalaBase2, const float escalaParaHD);
		static AUTextura*			texturaDesdeAreaVacia(const SI32 ancho, const SI32 alto, const MapaBitsColor color, const ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, const ENTexturaTipoUsoGL tipoUso, const ENTexturaModoPintado modoPintado, const ENTexturaMipMap habilitarMipMap, const ENTexturaOrdenV ordenVTextura, const UI8 escalaBase2, const float escalaParaHD);
		//
		static AUTextura*			texturaPatronDesdeArchivoPNG(const char* nombrePNG, const ENGestorTexturaOrigen origenTextura = ENGestorTexturaOrigen_paquete, const UI8 escalaBase2 = 1, const float escalaParaHD = 1.0f);
		static AUTextura*			texturaPatronDesdeArchivoJPEG(const char* nombrePNG, const ENGestorTexturaOrigen origenTextura = ENGestorTexturaOrigen_paquete, const UI8 escalaBase2 = 1, const float escalaParaHD = 1.0f);
		static AUTextura*			texturaPatronDesdeMapaBits(AUMapaBits* mapaBitsACargar, const char* guardarConNombre, ENTexturaOrdenV ordenVTextura, const UI8 escalaBase2, const float escalaParaHD);
		//
		static bool					texturaEstablecerContenido(const AUTextura* textura, const AUMapaBits* mapaBits);
		static bool					texturaEstablecerContenidoData(const AUTextura* textura, const MapaDeBitsDesciptor* props, const void* data);
		//
		static void					liberarEspacioTexturaSiSoloTieneUnaReferenciaExterna(AUTextura* textura);
		static UI16					liberarTexturasSinReferenciasEnAmbito(UI8 iAmb);
		static UI16					liberarTexturasSinReferencias();
		//Fuentes
		//static STNBFontLines*		fuenteLineas(const char* nomFuente, const bool negrilla, const bool cursiva);
		//static STNBFontBitmaps*	fuenteBitmaps(const char* nomFuente, const float fontSz, const bool negrilla, const bool cursiva);
		static AUFuenteTextura*		fuenteTextura(const char* nombreFuente, const float tamanoFuente, const char* subfamily, const float escalaParaHD = 1.0f);
		static AUFuenteTextura*		fuenteTextura(const char* nombreFuente, const float tamanoFuente, const bool negrilla, const bool cursiva, const float escalaParaHD = 1.0f);
		static AUFuenteRaster*		fuenteRaster(const char* nombreFuente, const float tamanoFuente, const char* subfamily);
		static AUFuenteRaster*		fuenteRaster(const char* nombreFuente, const float tamanoFuente, const bool negrilla, const bool cursiva);
		//static BOOL				updateFontRasterToSize(AUFuenteRaster* font, const float newSize);
		//ToDo: remove
		//static void				fuentesResetCurrent();
		//
		static void					bufferObtenerDatos(void* param, const SI32 iVao, STVerticesDatos* guardarDatosEn);
		//Consultas
		static const char*			nombreRecursoTextura(AUTextura* textura);
		//MipMaps
		static void					generarMipMapsDeTexturas();
		static void					generarMipMapsDeTexturas(UI16 iAmbitoTexturas);
		//
		static void					volcarAltasHaciaArchivos();
		static void					estadoTexturas(UI32* guardarBytesReservadosEn, UI32* guardarBytesUsadosEn, UI32* guardarAtlasEn);
		//
		static void					bloquear();
		static void					desbloquear();
		static bool					bloqueado();
		//
		static GLenum				formatoGlLocal(MapaBitsColor colorDestino);
		static GLenum				formatoGlDriver(MapaBitsColor colorDestino);
		static GLenum				tipoDatoGlLocal(MapaBitsColor colorDestino);
		static GLenum				tipoDatoGlDriver(MapaBitsColor colorDestino);
		static UI8					bytesPorPixel(MapaBitsColor colorDestino);
		static bool					dimensionTexturaEsMultiploDe2Valido(int tamanoTexeles);
		static bool					tamanoTexturaEsCuadradaMultiploDe2Valido(int anchoTexeles, int altoTexeles);
		//
		#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
		static UI32					debugBytesTotalTexturas();
		static UI32					debugBytesTotalTexturasDelColor(MapaBitsColor colorBuscado, SI32* guardarConteoEn);
		#endif
	private:
		//PROPIEDADES
		static bool					_gestorInicializado;
		static bool					_bloqueado;
		static UI32					_texsChangesCount; //Textures with new data
		static AUArregloNativoMutableP<ENGestorTexturaModo>* _pilaModosCargaTexturas;
		static NBHILO_MUTEX_CLASE	_pilaModosCargaTexturasMutex;
		//RUTAS
		static STTexturaResProps	_propsByResolution[ENTexturaResolucion_Conteo];
		//ATLASES
		static UI8											_indiceAmbitoActivo;
		static STGestorTexAmbito							_ambitosAtlases[GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS]; //atlas creados (general), agrupados por ambitos
		static AUArregloNativoMutableP<STGestorTexFuente>*	_fuentesTexturas;	//Fuentes texturas (usa texturasGL)
		static AUArregloNativoMutableP<STGestorTexFuenteR>*	_fuentesRaster;	//Fuentes rasters (usa VAOs y VBOs cuando es posible)
		static AUArregloNativoOrdenadoMutableP<STGestorTexFuenteVAO>* _fuentesRasterVAOs;
		//CARGA DE TEXTURAS EN SEGUNDO PLANO
		static ENTexturaModoHilo								_texLoadThreadMode;
		static AUHilo*											_texLoadThread;
		static bool												_texLoadThreadWorking;
		static bool												_texLoadThreadStopFlag;
		static NBHILO_MUTEX_CLASE								_texLoadingRefsMutex;
		static AUArregloNativoMutableP<STGestorTexTexturaRef*>*	_texLoadingRefs;
		//Unorganized textures (preloaded textures)
		static NBHILO_MUTEX_CLASE								_texUnorganizedRefsMutex;
		static AUArregloNativoMutableP<STGestorTexTexturaRef>*	_texUnorganizedRefs;
		//
		static AUFuenteTextura*		privFuenteTextura(const char* nombreFuente, const float tamanoFuente, const char* optSubfamily, const bool negrilla, const bool cursiva, const float escalaParaHD);
		static AUFuenteRaster*		privFuenteRaster(const char* nombreFuente, const float tamanoFuente, const char* optSubfamily, const bool negrilla, const bool cursiva);
		//
		static STGestorTexFuenteVAO* privFuenteRasterVao(STNBFontLines* fuenteDef, const float tamano, const bool crearSiNoExiste);
		static void					privRecargarTexturas(const bool elimTexturasGLAnteriores);
		static void					privRegenerarAtlasesTexturasEscritura(const bool elimTexturasGLAnteriores);
		static void					privRegenerarAtlasesFuentesTexturas(const bool elimTexturasGLAnteriores);
		//
		static STGestorTexTextura*	privTexturaPorNombre(UI8 iAmbitoTexturas, const char* filepath, ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento);
		//ToDo: remove
		//static STGestorTexTextura* privTexturaPorFirmaSHA1(UI8 iAmbitoTexturas, const STNBSha1Hash &firmaSHA1, ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento);
		static bool					privReservaAreaEnAtlas(UI8 iAmbitoTexturas, const char* nombreTextura, const SI32 nomTamano, UI32 ancho, UI32 alto, MapaBitsColor colorDestino, ENTexturaTipoUsoGL tipoUsoTextura, ENTexturaModoPintado modoPintado, SI32* guardarIndiceAtlasEn, NBRectanguloUI16* guardarAreaEn);
		static bool					privReservaAreaEnAtlasDePrecarga(UI8 iAmbitoTexturas, SI32* guardarIndiceAtlasEn);
		//Textura desde archivos
		static const char*			privRutaArchivoTextura(STNBString* buff, UI8* dstEscalaBase2, float* dstEscalaParaHD, const char* nombre, const SI32 nomTamano, const ENGestorTexturaOrigen origenTextura);
		static STGestorTexTextura*	privTexturaDesdeArchivoPNG(UI8 iAmbitoTexturas, const ENGestorTexturaOrigen origenTextura, const UI8 escalaBase2, const float escalaParaHD, const char* filepath, bool verificarEnListas, ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, ENTexturaTipoUsoGL tipoUso, ENTexturaModoPintado modoPintado, ENTexturaMipMap habilitarMipMap, STGestorTexTextura* inOptPreloadedTex);
		static STGestorTexTextura*	privTexturaDesdeArchivoJPEG(UI8 iAmbitoTexturas, const ENGestorTexturaOrigen origenTextura, const UI8 escalaBase2, const float escalaParaHD, const char* filepath, bool verificarEnListas, ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, ENTexturaTipoUsoGL tipoUso, ENTexturaModoPintado modoPintado, ENTexturaMipMap habilitarMipMap, STGestorTexTextura* inOptPreloadedTex);
		static STGestorTexTextura*	privTexturaRegistrar(UI8 iAmbitoTexturas, const bool reservarEnAtlasGL, const bool inicializarEstadoCargaLineas, STGestoTexLoadingBase* state, const ENTexturaOrigenFormato formato, const MapaDeBitsDesciptor propsMapaBits, STNBArray* extraChuncks, STGestorTexAtlas** outAtlasTex, STGestorTexTextura* inOptPreloadedTex);
		static bool					privTexturaCargarDatosStreamLocked(STGestorTexAtlas* atlasTex, STGestorTexTextura* texture, STGestoTexLoadingBase* state);
		//Textura desde datos
		static STGestorTexTextura*	privTexturaDesdeMapaBits(UI8 iAmbitoTexturas, AUMapaBits* mapaBitsACargar, const char* enListarConNombre, ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, ENTexturaTipoUsoGL tipoUso, ENTexturaModoPintado modoPintado, ENTexturaMipMap habilitarMipMap, ENTexturaOrdenV ordenVTextura, STGestorTexTextura* inOptPreloadedTex, const UI8 escalaBase2, const float escalaParaHD);
		static STGestorTexTextura*	privTexturaDesdeDatos(UI8 iAmbitoTexturas, SI32 anchoMapaBits, SI32 altoMapaBits, const void* datosMapaBits, MapaBitsColor colorMapaBits, const ENGestorTexturaOrigen origenTextura, const UI8 escalaBase2, const float escalaParaHD, const char* enListarConNombre, ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, ENTexturaTipoUsoGL tipoUso, ENTexturaModoPintado modoPintado, ENTexturaMipMap habilitarMipMap, ENTexturaOrdenV ordenVTextura, STGestorTexTextura* inOptPreloadedTex);
		static SI32					privCrearAtlasTexturaGL(UI8 iAmbitoTexturas, SI32 anchoAtlas, SI32 altoAtlas, MapaBitsColor colorDestino, ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, ENTexturaTipoUsoGL tipoUso, ENTexturaModoPintado modoPintado, ENTexturaMipMap habilitarMipMap);
		//STGestorTexAtlas
		static void					privSTGestorTexAtlasRetain(STGestorTexAtlas* obj);
		static void					privSTGestorTexAtlasRelease(STGestorTexAtlas* obj);
		//STGestorTexTextura
		static void					privSTGestorTexTexturaRetain(STGestorTexTextura* obj);
		static void					privSTGestorTexTexturaRelease(STGestorTexTextura* obj);
		//Loading states
		static void					privLoadingStateInitBase(STGestoTexLoadingBase* state, AUArchivo* archivo, const ENGestorTexturaOrigen origenTextura, const UI8 escalaBase2, const float escalaParaHD, const char* enListarConNombre, const SI32 nomTamano, const ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, const ENTexturaTipoUsoGL tipoUso, const ENTexturaModoPintado modoPintado, const ENTexturaMipMap habilitarMipMap, const ENTexturaOrdenV ordenVTextura);
		static void					privLoadingStateInitPNG(STGestoTexLoadingPNG* state, AUArchivo* archivo, const ENGestorTexturaOrigen origenTextura, const UI8 escalaBase2, const float escalaParaHD, const char* enListarConNombre, const SI32 nomTamano, const ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, const ENTexturaTipoUsoGL tipoUso, const ENTexturaModoPintado modoPintado, const ENTexturaMipMap habilitarMipMap, const ENTexturaOrdenV ordenVTextura);
		static void					privLoadingStateInitJPEG(STGestoTexLoadingJPEG* state, AUArchivo* archivo, const ENGestorTexturaOrigen origenTextura, const UI8 escalaBase2, const float escalaParaHD, const char* enListarConNombre, const SI32 nomTamano, const ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, const ENTexturaTipoUsoGL tipoUso, const ENTexturaModoPintado modoPintado, const ENTexturaMipMap habilitarMipMap, const ENTexturaOrdenV ordenVTextura);
		static void					privLoadingStateRelease(STGestoTexLoadingBase* state);
		//
		static UI32					privCreateFontTextureAtlases(STGestorTexFuente* record, STNBFontBitmaps* fontDef);
};

#endif
