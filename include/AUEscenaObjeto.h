#ifndef AUESCENAOBJETO_H_INCLUIDO
#define AUESCENAOBJETO_H_INCLUIDO

#include "nb/2d/NBPoint.h"
#include "nb/2d/NBRect.h"
#include "nb/2d/NBAABox.h"
//
#include "AUObjeto.h"
#include "NBPunto.h"
#include "NBTamano.h"
#include "NBColor.h"
#include "AUPilaNativaMutable.h"
#include "NBMatriz.h"
#include "NBCajaAABB.h"
#include "NBPropiedadesEnEscena.h"
#include "NBGestorGL.h"
#include "AUDatosCSV.h"
#include "AUDatosXML.h"
#include "AUTextura.h"
#include "NBGestorTouches.h"
#include "AUArreglo.h"
#include "NBFuenteSombra.h"
#include "NBFuenteIluminacion.h"
#include "AUArchivo.h"
#include "AUArregloMutable.h"
#include "NBGestorEscenaDef.h"
#include "AUEscenaTopItfDefs.h"

/*
 2011-10-23, Nota de rendimiento sobre MATRICES
 Implementar matrices propias en lugar del tradicional glPushMatrix-glTranslate-glRotate-glScale-glPopMatrix
 resulta un 33% mas rapido y permite contar con datos para exploracion eficiente de la pantalla (ej: deteccion de clic).
 Ciclos promedio de funcnion pintado (1 millon ciclos por segundo, en Macbook Pro 5,5):
 Usando matrices propias:	250 ciclos (enviando a recalcular todas las matrices)
 Usando glPushMatrix:		360 ciclos (glPushMatrix-glTranslate-glRotate-glScale-glPopMatrix)
 */

#define AUOBJETOESCENA_VALOR_VERIFICACION_BIN		-12345

#define AUOBJETOESCENA_BIT_ESCONTENEDOR				1	//Determina si el objeto es un contenedor
#define AUOBJETOESCENA_BIT_PREAGRUPAR_SOMBRAS		2	//Determina si el contenedor debe preagrupar las sombras
#define AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO	4	//Determina si debe heredar el agrupador de sombras que tiene establecido
#define AUOBJETOESCENA_BIT_MUNDO_FISICA_FIJO		8	//Determina si debe heredar el idMundoFisica que tiene establecido
#define AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED		16	//When my scene matrix should be updated
#define AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED		32	//When a child scene matrix should be updated
#define AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED		64	//When my scene model should be updated
#define AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED		128	//When a child scene model should be updated
//
#define AUOBJETOESCENA_TOUCH_BIT_LISTEN_TOUCH		1	//The object listen to touch events
#define AUOBJETOESCENA_TOUCH_BIT_LISTEN_SCROLL		2	//The object listen to scroll events
#define AUOBJETOESCENA_TOUCH_BIT_LISTEN_MAGNIFY		4	//The object listen to magnify/zoom events

class AUEscenaObjeto;
class AUEscenaContenedor;

//Listener to notify before leaving scene, when touches are active
typedef struct STNBEscenaObjetoMngrTouchItf_ {
	void*	param;
	void	(*objWillBeRemovedFromSceneFunc)(AUEscenaObjeto* obj, const SI32 touchesOwnedCount, void* param);
} STNBEscenaObjetoMngrTouchItf;

class IEscuchadorTouchEscenaObjeto {
	public:
		virtual ~IEscuchadorTouchEscenaObjeto(){
			//
		}
		//Touch
		virtual void touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto) = 0;
		virtual void touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto) = 0;
		virtual void touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto) = 0;
		//Hover
		virtual void hoverIniciado(AUEscenaObjeto* objeto) {}
		virtual void hoverMovido(const NBPunto &posActualEscena, AUEscenaObjeto* objeto) {}
		virtual void hoverFinalizado(AUEscenaObjeto* objeto) {}
		//Scroll
		virtual void touchScrollApply(AUEscenaObjeto* objeto, const STNBPoint posScene, const STNBSize size, const BOOL animate) {}
		//Magnify
		virtual void touchMagnifyApply(AUEscenaObjeto* objeto, const STNBPoint posScene, const float magnification, const BOOL isSmarthMag) {}
};

//Datos cache de un objeto escena.
struct STEscenaObjetoCache {
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	bool			debugCacheActualizada;	//Para evitar que se llame a "cajaEscena()" o "matrizEscena()" cuando el objeto solo ha sido creado y no ha pasado por su primera actualizacion de matriz
	#endif
	NBCajaAABB		cajaEscena;				//Caja escena del cuerpo
	NBMatriz		matrizEscena;			//Matriz escena de este objeto
};

class AUEscenaObjeto : public AUObjeto {
	public:
		//constructores y destructores
		AUEscenaObjeto();
		AUEscenaObjeto(AUEscenaObjeto* otro);
		virtual ~AUEscenaObjeto();
		//
		UI8							conteoCafeina() const;
		void						cafeinizar();
		void						decafeinizar();
		//Scene state mask
		void						enableSceneMask(const UI16 bits); //AUOBJETOESCENA_BIT_*
		void						enableSceneMaskAndParentsUntilEnabled(const UI16 bits, const UI16 bitsParent); //AUOBJETOESCENA_BIT_*
		void						setModelChangedFlag();
		//funciones de instancia
		void						copiarPropiedadesDe(AUEscenaObjeto* otroObjeto);
		void						copiarTransformacionesDe(AUEscenaObjeto* otroObjeto);
		NBMatriz					matrizLocal();						//
		NBMatriz					matrizEscena();						//Ultima matriz en escena (cache)
		NBMatriz					matrizEscenaCalculada();			//Calculada en tiempo real (no-cache)
		NBMatriz					matrizEscenaInversaCalculada();		//Calculada en tiempo real (no-cache)
		virtual NBCajaAABB			cajaAABBLocal();					//
		virtual	NBCajaAABB			cajaAABBLocalCalculada() = 0;		//Calculada en tiempo real (no-cache)
		NBCajaAABB					cajaAABBEnEscena();					//Ultima caja en escena (cache)
		UI8							mascaraPropsObjetoEscena();
		SI8							idEscena;							//optimizacion, usar varPublica en lugar de funcion "idEscena()"
		SI8							idGrupo;							//optimizacion, usar varPublica en lugar de funcion "idGrupo()"
		SI16						idCapa;								//optimizacion, usar varPublica en lugar de funcion "idCapa()"
		AUEscenaObjeto*				contenedor();
		AUEscenaObjeto*				agrupadorSombras();
		NBPropiedadesEnEscena		propiedades();
		bool						dormido();
		bool						visible();
		bool						iluminoDependiente();
		bool						esContenedor();
		NBPunto						traslacion();
		NBTamano					escalacion();
		float						rotacion();
		//Scrolls
		BOOL						getTouchScrollEnabled() const;
		void						setTouchScrollEnabled(const BOOL enabled);
		//Magnify
		BOOL						getTouchMagnifyEnabled() const;
		void						setTouchMagnifyEnabled(const BOOL enabled);
		//
		NBPropiedadesEnEscena		_propiedades;				//visible, transformaciones(traslacion, escalacion y rotacion) y color //Optimizacion, usar esta variable publica en lugar de metodos de acceso
		//NBColor8					multiplicadorColor8;		//Optimizacion, usar esta variable publica "multiplicadorColor" en lugar del metodo "multiplicadorColor()" (alta demanda de este valor en ejecucion)
		NBColor8					multiplicadorColor8Func();	//Optimizacion, usar la variable publica "multiplicadorColor" en lugar del metodo "multiplicadorColor()" (alta demanda de este valor en ejecucion)
		UI8							multiplicadorAlpha8();
		NBColorF					ultimoColorInfluenciaIluminacion(); //el prefijo "ultimo" se refiere a ultimo valor utilizado (no calculado en tiempo real)
		NBPunto						coordenadaLocalAEscena(float xLocal, float yLocal);
		NBPunto						coordenadaEscenaALocal(float xEnEscena, float yEnEscena);
		void						establecerPropiedades(const NBPropiedadesEnEscena &nuevasPropiedades);
		void						setVisibleAndAwake(const BOOL visibleAndAwake);
		virtual void				establecerDormido(bool congelado);
		void						establecerVisible(bool visible);
		void						establecerTraslacion(const NBPunto &nuevaTraslacion);
		void						establecerTraslacion(float nuevaTraslacionX, float nuevaTraslacionY);
		void						establecerEscalacion(const NBTamano &nuevoEscalacion);
		void						establecerEscalacion(const float nuevoEscalacionAnchoAlto);
		void						establecerEscalacion(const float nuevoEscalacionAncho, const float nuevoEscalacionAlto);
		void						establecerRotacion(float nuevaRotacion);
		void						establecerRotacionNormalizada(float nuevaRotacion);
		void						establecerMultiplicadorColor(const NBColor &multipplicadorColor);
		void						establecerMultiplicadorColor(float r, float g, float b, float a);
		void						establecerMultiplicadorColor8(const NBColor8 &multipplicadorColor);
		void						establecerMultiplicadorColor8(UI8 r, UI8 g, UI8 b, UI8 a);
		void						establecerMultiplicadorAlpha8(UI8 a);
		void						establecerIluminoDependencia(bool iluminoDependiente);
		void						setVisibleAwakeAndAlphaByParentArea(const STNBAABox outter, const STNBAABox inner);
		//PUERTO DE VISION
		virtual void				puertoVisionModificado();
		//Top-scene-tools (quick-tips, selectboxes, anything that must go at top of content)
		BOOL						showQuickTipText(const STNBPoint localPos, const char* text, AUTextura* iconLeft, AUTextura* iconRight, const float secsShow, const ENEscenaTipAim aimDir);
		virtual BOOL				showQuickTipTextScene(const STNBPoint scenePos, const char* text, AUTextura* iconLeft, AUTextura* iconRight, const float secsShow, const ENEscenaTipAim aimDir);
		virtual BOOL				addToTop(AUEscenaObjeto* obj);
		STNBRectI					viewPortForMe();
		STNBRectI					viewPortForLocalBox(const STNBAABox box);
		virtual STNBRectI			viewPortForSceneBox(const STNBAABox box);
		//TOUCHES
		IEscuchadorTouchEscenaObjeto* escuchadorTouches();
		void						establecerEscuchadorTouches(AUObjeto* obj, IEscuchadorTouchEscenaObjeto* interfaz);
		void						setMngrTouchesItf(const STNBEscenaObjetoMngrTouchItf* itf);
		SI32						touchesCount();
		void						touchesReleaseAll();
		void						touchIniciado(void* escuchador, STGTouch* touch, const NBPunto &posInicialEscena);
		void						touchMovido(void* escuchador, STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena);
		void						touchFinalizado(void* escuchador, STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena);
		void						hoverIniciado(void* escuchador);
		void						hoverMovido(void* escuchador, const NBPunto &posActualEscena);
		void						hoverFinalizado(void* escuchador);
		void						touchScrollApply(void* escuchador, const STNBPoint posScene, const STNBSize size, const BOOL animate);
		void						touchMagnifyApply(void* escuchador, const STNBPoint posScene, const float magnification, const BOOL isSmarthMag);
		void						liberarTouch(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, bool cancelar, AUEscenaObjeto* nuevoDestinatario);
		virtual AUEscenaObjeto*		consumidorTouchEnPosicion(const NBPunto &posTouchEscena, const UI8 touchFilterMask, AUObjeto** outConsumidorEscuchadorObj, void** outConsumidorEscuchadorInterfaz);
		virtual AUEscenaObjeto*		consumidorTouchEnPosicionSegunArreglo(const NBPunto &posTouchEscena, const UI8 touchFilterMask, const AUArreglo* arregloFiltroObjetosEscena);
		//HOVERS
		//A HEREDAR AL CONTENEDOR
		//virtual void				actualizarArbolMatricesEscena(STNBSceneModelsResult* dst, const NBPropRenderizado &propsRenderizado, const NBPropHeredadasModelos &propsHeredadas);
		//virtual void				actualizarArbolModelosGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		//A HEREDAR OBLIGATORIAS
		virtual PTRfunCmdsGL		actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas) = 0;
		//
		//virtual void				debug_verificaNivelEnEscena(int nivelEsperado);
		//guardar y cargar en XML
		static bool					agregarXmlInternoEn(AUEscenaObjeto* objEscena, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool					interpretarXmlInterno(AUEscenaObjeto* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool					agregarBitsInternosEn(AUEscenaObjeto* objEscena, AUArchivo* guardarEn);
		static bool					interpretarBitsInternos(AUEscenaObjeto* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		//AUOBJMETODOS_CLONAR_DEFINICION
		//friend
		friend class				AUEscenaContenedor;					//para poder acceder al miembro private::_contenedor
	protected:
		UI16						_sceneMask;			//Mascara de las propiedades del objetoEscena.
		UI8							_touchFilterMask;	//Filter touches, scroll and magnifications
		bool						_dormido;			//ToDo: move to '_sceneMask'
		bool						_iluminoDependiente;//ToDo: move to '_sceneMask'
		UI8							_contadorCafeina;	//Para determinar cuando el objeto puede dormirse en escena.
		SI8							_idMundoFisica;
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		bool						_debugBloqueadoActualizandoModelos;
		bool						_debugAgregadoEscena;
		bool						_debugAgregadoFisica;
#		endif
		//Touches
		struct {
			SI32					count;	//currently owned touches
			STNBEscenaObjetoMngrTouchItf itf;
		} _touches;
		AUEscenaObjeto*				_contenedor;						//Objeto de escena que contiene a este
		AUEscenaObjeto*				_agrupadorSombras;					//Objeto que define el agrupamiento de las sombras (para evitar que los objetos procesen sus propias sombra)
		AUObjeto*					_escuchadorTouchObjeto;
		IEscuchadorTouchEscenaObjeto* _escuchadorTouchInterfaz;
		STEscenaObjetoCache			_cacheObjEscena;					//Cache, solo disponible si el objeto alguna vez estuvo en escena
		//
		inline void					privInlineInicializarVariables();
		inline void					privInlineInicializarPropiedades(float traslacionX, float traslacionY, float escalacionAncho, float escalacionAlto, float rotacion, float multR, float multG, float multB, float multA);
		virtual void				agregadoEnEscena(){ /*nada*/ }
		virtual void				agregadoEnEscenaConHijosNotificados(){ /*nada*/ }
		virtual void				quitandoDeEscena(){ /*nada*/ }
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		virtual void				debugEstablecerBloqueadoActualizandoModelos(const bool bloqueado);
#		endif
};

#endif
