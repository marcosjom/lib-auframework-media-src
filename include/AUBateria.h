//
//  AUBateria.h
//  DodoTownNucleo
//
//  Created by Marcos Ortega on 22/09/12.
//
//

#ifndef DodoTownNucleo_AUBateria_h
#define DodoTownNucleo_AUBateria_h

#include "AUEscenaSensor.h"
#include "AUNodoElectroCarga.h"

#define AUBateria_CANTIDAD_LUCES_INDICADORAS	4

class AUBateria;

class IEscuchadorBateria {
	public:
		virtual ~IEscuchadorBateria(){
			//
		}
		//Retornan el tiempo en segundos que debe esperar el laser para ejecutar el siguiente evento
		virtual void bateriaCargada(AUBateria* objBateria, AUEscenaSensor* sensorComandos) = 0;
		virtual void bateriaDescargada(AUBateria* objBateria, AUEscenaSensor* sensorComandos) = 0;
};

class AUBateria : public AUNodoElectroCarga {
	public:
		AUBateria();
		AUBateria(AUBateria* otraInstancia);
		virtual ~AUBateria();
		//
		bool					bateriaActiva();
		bool					bateriaCargada();
		bool					desactivarDespuesDeCargar();
		bool					desactivarDespuesDeDescargar();
		float					cargaParaConsiderarCargada();
		float					segsAcumularParaCargar();
		float					segsAcumularParaDescargar();
		AUEscenaSensor*			sensorComandosAlCargar();
		AUEscenaSensor*			sensorComandosAlDescargar();
		IEscuchadorBateria*		escuchadorBateria();
		//
		void					establecerBateriaActiva(bool bateriaActiva);
		void					establecerDesactivarDespuesDeCargar(bool desactivarDespues);
		void					establecerDesactivarDespuesDeDescargar(bool desactivarDespues);
		void					establecerCargaParaConsiderarCargada(float cargaRel);
		void					establecerSegsAcumularParaCargar(float segsAcumParaEncender);
		void					establecerSegsAcumularParaDescargar(float segsAcumParaApagar);
		void					establecerSensorComandosAlCargar(AUEscenaSensor* objSensor);
		void					establecerSensorComandosAlDescargar(AUEscenaSensor* objSensor);
		void					establecerSensorComandosSegunPlantilla(AUBateria* instanciaPlantilla, AUArreglo* hijosPlantilla, AUArreglo* hijosLocal);
		void					establecerEscuchadorBateria(IEscuchadorBateria* escuchador);
		//
		virtual	NBCajaAABB		cajaEscenaPresentacion();
		virtual float			electrocargaVirtualParaDestinos();
		virtual float			electrocargaRealParaDestinos();
		virtual bool			electrocargaEsSoloConsumidor();
		virtual void			tickAnimacion(float segsTranscurridos);
		//guardar y cargar en XML
		static bool				agregarXmlInternoEn(AUBateria* bateria, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloObjetosParaID);
		static bool				interpretarXmlInterno(AUBateria* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos, AUArregloNativoP<SI32>* arregloIDs);
		//guardar y cargar en binario
		static bool				agregarBitsInternosEn(AUBateria* bateria, AUArchivo* guardarEn, AUArreglo* arregloObjetosParaID);
		static bool				interpretarBitsInternos(AUBateria* cargarEn, AUArchivo* cargarDe, AUArreglo* arregloObjetosParaID);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		bool					_bateriaActiva;
		bool					_bateriaCargada;
		bool					_desactivarDespuesDeCargar;
		bool					_desactivarDespuesDeDescargar;
		//
		float					_cargaParaConsiderarCargada;
		float					_segsAcumularParaCargar;
		float					_segsAcumularParaDescargar;
		//
		AUEscenaSprite*			_presentaBateria;
		AUEscenaSprite*			_presentaMarcoLuces;
		AUEscenaSprite*			_indicadoresCarga[AUBateria_CANTIDAD_LUCES_INDICADORAS];
		AUEscenaSensor*			_sensorComandosAlCargar;
		AUEscenaSensor*			_sensorComandosAlDescargar;
		IEscuchadorBateria*		_escuchadorBateria;
		//Animacion de luces
		float					_cargaActual;
		float					_cargaDestino;
		//Animacion de luz de carga final
		bool					_indicaEncendidosAumentado;
		UI16					_indicaMaximoCargando;
		UI16					_indicaMaximoCargado;
		float					_relIndicaActual;
		float					_relIndicaDestino;
};


#endif
