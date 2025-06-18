//
//  AUNodoElectroCarga.h
//  DodoTownNucleo
//
//  Created by Marcos Ortega on 18/12/12.
//
//

#ifndef DodoTownNucleo_AUNodoElectroCarga_h
#define DodoTownNucleo_AUNodoElectroCarga_h

#include "AUEscenaSensor.h"
#include "AUEscenaCuerda.h"

#define AUNODOELECTROCARGA_TEXTURA_CABLE_CORTABLE			"Escenarios/Electricos/objCableElectrico01.png"
#define AUNODOELECTROCARGA_TEXTURA_CABLE_NO_CORTABLE		"Escenarios/Electricos/objCableElectrico03.png"

class AUNodoElectroCarga;

struct STNodoElectroSaliente {
	AUNodoElectroCarga* objNodoDestino;
	AUEscenaCuerda*		objCable;			//Cuerda competa o segmento cortado que inicia en este objeto
	AUEscenaCuerda*		objCable2;			//Solo si cortada, segmento que termina en el objeto destino
	float				factorLongitudCable;
	float				segsParaCorteCable;
	bool				cuerdaInicializada;
	//
	bool operator==(const STNodoElectroSaliente &otro) const {
		return (objNodoDestino==otro.objNodoDestino);
	}
	bool operator!=(const STNodoElectroSaliente &otro) const {
		return !(objNodoDestino==otro.objNodoDestino);
	}
};

class AUNodoElectroCarga : public AUEscenaContenedor, public NBAnimador, public IEscuchadorCuerda {
	public:
		AUNodoElectroCarga();
		AUNodoElectroCarga(AUNodoElectroCarga* otraInstancia);
		virtual ~AUNodoElectroCarga();
		//
		virtual	NBCajaAABB							cajaEscenaPresentacion() = 0;
		virtual float								electrocargaVirtualParaDestinos() = 0;
		virtual float								electrocargaRealParaDestinos() = 0;
		virtual bool								electrocargaEsSoloConsumidor() = 0;
		float										electrocargaVirtualTotalRecibida();
		float										electrocargaRealTotalRecibida();
		AUArregloNativoP<STNodoElectroSaliente>*	electroNodosSalientes();
		void										agregarDestinoElectroCarga(AUNodoElectroCarga* objDestino, float factorLongitudCable, float segsParaCorteCable);
		void										agregarDestinosElectroCargarSegunPlantilla(AUNodoElectroCarga* instanciaPlantilla, AUArreglo* hijosPlantilla, AUArreglo* hijosLocal);
		void										actualizarPropiedadesDestinoElectroCarga(AUNodoElectroCarga* objDestino, float factorLongitudCable, float factorResisteCorteCable);
		void										actualizarPropiedadesDestinoElectroCargaEnIndice(UI16 indiceDestino, float factorLongitudCable, float factorResisteCorteCable);
		void										quitarDestinoElectroCarga(AUNodoElectroCarga* objDestino);
		void										quitarDestinoElectroCargaEnIndice(UI16 indice);
		void										vaciarDestinosElectroCargas();
		//
		virtual void								cuerdaCortada(AUEscenaCuerda* objCuerda, UI16 iSegmentoCorte);
		//
		virtual void								tickAnimacion(float segsTranscurridos);	
		//guardar y cargar en XML
		static bool									agregarXmlInternoEn(AUNodoElectroCarga* bateria, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloObjetosParaID);
		static bool									interpretarXmlInterno(AUNodoElectroCarga* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos, AUArregloNativoP<SI32>* arregloIDs);
		//guardar y cargar en binario
		static bool									agregarBitsInternosEn(AUNodoElectroCarga* bateria, AUArchivo* guardarEn, AUArreglo* arregloObjetosParaID);
		static bool									interpretarBitsInternos(AUNodoElectroCarga* cargarEn, AUArchivo* cargarDe, AUArreglo* arregloObjetosParaID);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		void										privAgregarOrigenElectroCarga(AUNodoElectroCarga* objOrigen);
		void										privQuitarOrigenElectroCarga(AUNodoElectroCarga* objOrigen);
		//
		virtual void								agregadoEnEscena();
		virtual void								quitandoDeEscena();
	private:
		AUArregloNativoMutableP<STNodoElectroSaliente>	_electroNodosSalientesss;	//Nodos a  los cuales se envia carga electrica (son retenidos)
		AUArregloNativoMutableP<AUNodoElectroCarga*>	_electroNodosEntrantes;		//Nodos de los cuales se recibe cargar electrica (no son retenidos, para no producir recursividad)
};

#endif
