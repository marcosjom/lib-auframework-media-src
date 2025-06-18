//
//  AUPanelSolar.h
//  DodoTownNucleo
//
//  Created by Marcos Ortega on 22/09/12.
//
//

#ifndef DodoTownNucleo_AUPanelSolar_h
#define DodoTownNucleo_AUPanelSolar_h

#include "AUEscenaSensor.h"
#include "AUNodoElectroCarga.h"
#include "AUEscenaLuzSensor.h"

#define AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS	3

class AUPanelSolar;

class IEscuchadorPanelSolar {
	public:
		virtual ~IEscuchadorPanelSolar(){
			//
		}
		//Retornan el tiempo en segundos que debe esperar el laser para ejecutar el siguiente evento
		virtual float panelSolarEncendido(AUPanelSolar* objPanelSolar, AUEscenaSensor* sensorComandos) = 0;
		virtual float panelSolarApagado(AUPanelSolar* objPanelSolar, AUEscenaSensor* sensorComandos) = 0;
};

class AUPanelSolar : public AUNodoElectroCarga {
	public:
		AUPanelSolar();
		AUPanelSolar(AUPanelSolar* otraInstancia);
		virtual ~AUPanelSolar();
		//
		bool					panelActivo();
		bool					panelEncendido();
		bool					desactivarDespuesDeEncender();
		bool					desactivarDespuesDeApagar();
		float					promedioLuzRGBParaEncender();
		float					segsAcumularParaEncender();
		float					segsAcumularParaApagar();
		AUEscenaSensor*			sensorComandosEncender();
		AUEscenaSensor*			sensorComandosApagar();
		IEscuchadorPanelSolar*	escuchadorPanelSolar();
		//
		virtual	NBCajaAABB		cajaEscenaPresentacion();
		void					establecerPanelActivo(bool panelActivo);
		void					establecerDesactivarDespuesDeEncender(bool desactivarDespues);
		void					establecerDesactivarDespuesDeApagar(bool desactivarDespues);
		void					establecerSegsAcumularParaEncender(float segsAcumParaEncender);
		void					establecerPromedioRGBParaEncender(float promedioRGB);
		void					establecerSegsAcumularParaApagar(float segsAcumParaApagar);
		void					establecerSensorComandosEncender(AUEscenaSensor* objSensor);
		void					establecerSensorComandosApagar(AUEscenaSensor* objSensor);
		void					establecerSensorComandosSegunPlantilla(AUPanelSolar* instanciaPlantilla, AUArreglo* hijosPlantilla, AUArreglo* hijosLocal);
		void					establecerEscuchadorPanelSolar(IEscuchadorPanelSolar* escuchador);
		//
		virtual float			electrocargaVirtualParaDestinos();
		virtual float			electrocargaRealParaDestinos();
		virtual bool			electrocargaEsSoloConsumidor();
		virtual void			tickAnimacion(float segsTranscurridos);
		//guardar y cargar en XML
		static bool				agregarXmlInternoEn(AUPanelSolar* explosivo, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloObjetosParaID);
		static bool				interpretarXmlInterno(AUPanelSolar* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos, AUArregloNativoP<SI32>* arregloIDs);
		//guardar y cargar en binario
		static bool				agregarBitsInternosEn(AUPanelSolar* explosivo, AUArchivo* guardarEn, AUArreglo* arregloObjetosParaID);
		static bool				interpretarBitsInternos(AUPanelSolar* cargarEn, AUArchivo* cargarDe, AUArreglo* arregloObjetosParaID);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		bool					_panelActivo;						//Determina si reacciona ante la presencia o ausencia de luz
		bool					_panelEncendido;					//Determina si se encuentra encendido por la presencia de luz
		bool					_desactivarDespuesDeEncender;
		bool					_desactivarDespuesDeApagar;
		//
		float					_promedioLuzRGBParaEncender;
		float					_segsAcumularParaEncender;
		float					_segsAcumularParaApagar;
		//
		AUEscenaSprite*			_presentaPanel;
		AUEscenaSprite*			_presentaMarco;
		AUEscenaSprite*			_indicadoresCarga[AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS];
		AUEscenaLuzSensor*		_sensorLuz;
		AUEscenaSensor*			_sensorComandosEncender;
		AUEscenaSensor*			_sensorComandosApagar;
		IEscuchadorPanelSolar*	_escuchadorPanelSolar;
		//Animacion de luces
		float					_cargaActual;
		float					_cargaDestino;
		//Animacion de luz de carga final
		bool					_marcoPanelAumentando;
		bool					_indicaEncendidosAumentado;
		UI16					_indicaMaximoCargando;		//En color amarillo
		UI16					_indicaMaximoCargado;		//En color azul
		//float					_segsAcumEnIndicaMaximo;
		//float					_segsAcumEnIndicaActual;
		float					_relIndicaActual;
		float					_relIndicaDestino;
};


#endif
