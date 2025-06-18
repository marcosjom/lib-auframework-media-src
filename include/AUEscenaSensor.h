//
//  AUEscenaSensor.h
//  EditorCuerpos
//
//  Created by Nicaragua Binary on 24/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaSensor_H_INCLUIDO
#define AUEscenaSensor_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "AUCadenaMutable.h"
#include "AUArregloNativo.h"
#include "AUArregloNativoMutable.h"
#include "NBGestorEscena.h"
#include "NBGestorAnimadores.h"
#include "NBGestorFisica.h"
#include "AUAnimadorPropsDef.h"

enum ENSensorEvento {
	ENSensorEvento_ColisionHeroe = 0,
	ENSensorEvento_EntraEnCamara,
	ENSensorEvento_SaleDeCamara,
	ENSensorEvento_Manual,
	ENSensorEvento_Conteo
};

struct STSensorComando {
	bool				ejecutarSoloUnaVez;
	UI8					eventoParaEjecutar;			//ENSensorEvento
	UI8					tipoSuavizado;				//ENAnimPropVelocidad
	UI16				vecesEjecutado;
	float				segsEsperaParaEjecutar;		//Segundos a esperar entre el evento y la ejecucion del comando
	float				segsSuavizadoComando;		//Segundos que dura la aplicacion del comando (suavizado cuando este disponible)
	AUCadenaMutable8*	strComando;					//Comando del sensor
	AUObjeto*			objUnido;					//Objeto al que esta unido (se permite NULL para los comandos generales)
	bool operator==(const STSensorComando &otro) const {
		return (strComando==otro.strComando && objUnido==otro.objUnido);
	}
	bool operator!=(const STSensorComando &otro) const {
		return !(strComando==otro.strComando && objUnido==otro.objUnido);
	}
};

struct STSensorComandoEsperando {
	SI32 iComando;
	float segundosEsperar;
	bool operator==(const STSensorComandoEsperando &otro) const {
		return (iComando==otro.iComando && segundosEsperar==otro.segundosEsperar);
	}
	bool operator!=(const STSensorComandoEsperando &otro) const {
		return !(iComando==otro.iComando && segundosEsperar==otro.segundosEsperar);
	}
};

struct STSensorColision {
	AUObjeto*		objetoColisionado;
	float			segundosAcumulados;
	//
	bool operator==(const STSensorColision &otro) const {
		return (objetoColisionado==otro.objetoColisionado);
	}
	bool operator!=(const STSensorColision &otro) const {
		return !(objetoColisionado==otro.objetoColisionado);
	}
};
		
#define AUSENSOR_BITS_EJECUTAR_CMDS_COLISION	1
#define AUSENSOR_BITS_EJECUTAR_CMDS_MANUALES	2

class AUEscenaSensor;

class IEscuchadorSensor {
	public:
		virtual ~IEscuchadorSensor(){
			//
		}
		virtual bool sensorEjecutarComando(AUEscenaSensor* sensor, const char* strComando, AUObjeto* objDestino, float segundosSuavizadoComando, ENAnimPropVelocidad tipoSuavizado) = 0;
};

class AUEscenaSensor : public AUEscenaObjeto, public NBAnimador {
	public:
		AUEscenaSensor();
		AUEscenaSensor(AUEscenaSensor* otraInstancia);
		virtual ~AUEscenaSensor();
		//
		IEscuchadorSensor*			escuchadorSensor();
		void						establecerEscuchadorSensor(IEscuchadorSensor* escuchador);
		bool						sensorActivo();
		void						establecerSensorActivo(bool sensorActivo);
		void						acumularColisionConHeroe(AUObjeto* objColisionado);
		void						acumularEjecucionComandosManuales();
		float						segundosParaComandosColisionEjecutables();
		//
		void						agregarComando(const char* strComando, bool ejecutarSoloUnaVez, ENSensorEvento evento, float segundoRetardoEjecucion, ENAnimPropVelocidad tipoSuavizado, float segundosSuavizadoComando, AUObjeto* objUnido);
		void						actualizarComandoEnIndice(UI32 iComando, const char* strComandoNuevo);
		void						actualizarComandoEventoEnIndice(UI32 iComando, ENSensorEvento evento);
		void						actualizarComandoParamentroEnIndice(UI32 iComando, AUObjeto* objUnido);
		void						actualizarComandoRetardoEnIndice(UI32 iComando, float segundosRetardo);
		void						actualizarComandoTipoSuavizadoEnIndice(UI32 iComando, ENAnimPropVelocidad tipoSuavizado);
		void						actualizarComandoSuavizadoEnIndice(UI32 iComando, float segundosSuavizado);
		void						quitarComandoEnIndice(UI32 iComando);
		AUArregloNativoP<STSensorComando>* comandosSensor();
		//FISICA
		void*						cuerpo(); //b2Body*
		//
		virtual	NBCajaAABB			cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL		actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		//
		virtual void				tickAnimacion(float segsTranscurridos);
		//guardar y cargar en XML
		static bool					agregarXmlInternoEn(AUEscenaSensor* sensor, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloCapasPorID);
		static bool					interpretarXmlInterno(AUEscenaSensor* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, SI32 idCapaPorDefecto, AUArregloNativoP<SI32>* arregloIDsCapas, AUArreglo* arreglosObjetosPorCapa, AUArreglo* arregloIDsObjetosPorCapa);
		//guardar y cargar en binario
		static bool					agregarBitsInternosEn(AUEscenaSensor* sensor, AUArchivo* guardarEn, AUArreglo* arregloCapasPorID);
		static bool					interpretarBitsInternos(AUEscenaSensor* cargarEn, AUArchivo* cargarDe, AUArreglo* capasCargadas);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		virtual void				agregadoEnEscena();
		virtual void				quitandoDeEscena();
	private:
		bool						_ultimoTickDentroEscena;	//Para determinar cuando sale y entra de escena
		bool						_sensorActivo;
		UI8							_propiedadesSensor;
		IEscuchadorSensor*			_escuchador;
		void*						_cuerpo; //b2Body*
		AUArregloNativoMutableP<STSensorColision>	_colisiones;
		AUArregloNativoMutableP<STSensorComando>	_comandosSensor;
		AUArregloNativoMutableP<STSensorComandoEsperando>	_comandosEsperando;
};


#endif
