#ifndef AUESCENACONTENEDOR_H_INCLUIDO
#define AUESCENACONTENEDOR_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "AUArregloMutable.h"
#include "AUArregloNativoMutable.h"

class NBGestorEscena;

class AUEscenaContenedor : public AUEscenaObjeto {
	public:
		AUEscenaContenedor();
		AUEscenaContenedor(AUEscenaContenedor* otraInstancia);
		AUEscenaContenedor(AUEscenaContenedor* otraInstancia, const bool clonarHijos);
		virtual ~AUEscenaContenedor();
		//
		AUArreglo*						hijos();
		void							hijoMover(const UI32 indiceActual, const UI32 indiceDestino);
		void							vaciar();
		void							agregarObjetoEscena(AUEscenaObjeto* objetoEscena);
		virtual void					agregarObjetoEscenaEnIndice(AUEscenaObjeto* objetoEscena, int indice);
		bool							quitarObjetoEscena(AUEscenaObjeto* objetoEscena);
		virtual bool					quitarObjetoEscenaEnIndice(int indice);
		//Sombras
		bool							preagruparSombrasParaHijos();
		void							establecerPreagruparSombrasParaHijos(bool preagruparSombrasParaHijos);
		void							fijarAgrupadorDeSombras();			//Establece este objeto como agrupador, de si mismo hacia su herencia (hijos, nietos, etc...)
		void							fijarAgrupadorDeSombras(AUEscenaObjeto* agrupadorSombras);
		void							restablecerAgrupadorDeSombras();	//Restablece al agrupador heredado del padre de este objeto como agrupador.
		//Fisica
		void							fijarIdMundoFisica(SI8 idMundoFisica);
		void							restablecerIdMundoFisica();
		//
		virtual NBCajaAABB				cajaAABBLocal();
		virtual	NBCajaAABB				cajaAABBLocalCalculada();
		//PANTALLA
		virtual void					puertoVisionModificado();
		//TOUCHES
		virtual AUEscenaObjeto*			consumidorTouchEnPosicion(const NBPunto &posTouchEscena, const UI8 touchFilterMask, AUObjeto** outConsumidorEscuchadorObj, void** outConsumidorEscuchadorInterfaz);
		virtual AUEscenaObjeto*			consumidorTouchEnPosicionSegunArreglo(const NBPunto &posTouchEscena, const UI8 touchFilterMask, const AUArreglo* arregloFiltroObjetosEscena);
		//HEREDADAS DE OBJETOESCENA
		virtual void					actualizarArbolMatricesEscena(STNBSceneModelsResult* dst, const NBPropRenderizado &propsRenderizado, const NBPropHeredadasModelos &propsHeredadas);
		virtual void					actualizarArbolModelosGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		//HERENCIAS OBLIGATORIAS
		virtual PTRfunCmdsGL			actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		//
		//virtual void					debug_verificaNivelEnEscena(int nivelEsperado);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
		// friend
		friend class					NBGestorEscena;						//para acceder al miembro private::privActualizarNivelEnEscena() y private::establecerIdMundoFisica()
	protected:
		AUArregloMutable 				_objetosEscena;
		//NBColor8						_colorLuzAmbiente8;
		virtual void					agregadoEnEscena();
		virtual void					quitandoDeEscena();
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		void							debugEstablecerBloqueadoActualizandoModelos(const bool bloqueado);
		#endif
	private:
		void							privActualizarNivelEnEscena(SI32 idEscenaP, SI32 idGrupoP, SI32 indiceCapaP);
		void							privEstablecerIdMundoFisica(SI8 idMundoFisica);
};
 
#endif
