//
//  AUBombillo.h
//  DodoTownNucleo
//
//  Created by Marcos Ortega on 22/09/12.
//
//

#ifndef DodoTownNucleo_AUBombillo_h
#define DodoTownNucleo_AUBombillo_h

#include "AUNodoElectroCarga.h"
#include "AUEscenaSprite.h"
#include "AUEscenaLuzRadial.h"

enum ENBombilloTipo {
	ENBombilloTipo_Incandecente = 0,
	ENBombilloTipo_IncandCono,
	ENBombilloTipo_Fluorescente,
	//
	ENBombilloTipo_Conteo
};

class AUBombillo : public AUNodoElectroCarga {
	public:
		AUBombillo();
		AUBombillo(AUBombillo* otraInstancia);
		virtual ~AUBombillo();
		//
		ENBombilloTipo			bombilloTipo();
		bool					bombilloActivo();
		bool					bombilloCargado();
		bool					desactivarDespuesDeCargar();
		bool					desactivarDespuesDeDescargar();
		float					cargaParaConsiderarCargada();
		float					alcanceMaximaLuz();
		void					establecerBombilloTipo(ENBombilloTipo tipoBombillo);
		void					establecerBombilloActivo(bool bombilloActivo);
		void					establecerDesactivarDespuesDeCargar(bool desactivarDespues);
		void					establecerDesactivarDespuesDeDescargar(bool desactivarDespues);
		void					establecerCargaParaConsiderarCargada(float cargaRel);
		void					establecerAlcanceMaximaLuz(float alcanceMaxima);
		//
		virtual	NBCajaAABB		cajaEscenaPresentacion();
		virtual float			electrocargaVirtualParaDestinos();
		virtual float			electrocargaRealParaDestinos();
		virtual bool			electrocargaEsSoloConsumidor();
		virtual void			tickAnimacion(float segsTranscurridos);
		//guardar y cargar en XML
		static bool				agregarXmlInternoEn(AUBombillo* bateria, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloObjetosParaID);
		static bool				interpretarXmlInterno(AUBombillo* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos, AUArregloNativoP<SI32>* arregloIDs);
		//guardar y cargar en binario
		static bool				agregarBitsInternosEn(AUBombillo* bateria, AUArchivo* guardarEn, AUArreglo* arregloObjetosParaID);
		static bool				interpretarBitsInternos(AUBombillo* cargarEn, AUArchivo* cargarDe, AUArreglo* arregloObjetosParaID);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		UI8						_tipoBombillo;		//ENBombilloTipo
		bool					_bombilloActivo;
		bool					_bombilloCargado;
		bool					_desactivarDespuesDeCargar;
		bool					_desactivarDespuesDeDescargar;
		//
		float					_cargaActualRecibida;
		float					_cargaParaConsiderarCargada;
		float					_alcanceMaximaLuz;
		float					_radianesActOscilacionBrillo;
		//
		AUEscenaSprite*			_presentaBrillo;
		AUEscenaSprite*			_presentaLampara;
		AUEscenaSprite*			_presentaBulbo;
		AUEscenaLuzRadial*		_luzRadial;
		NBTamano				_escalaMinBrillo;
		NBTamano				_escalaCreceMaxBrillo;
};


#endif
