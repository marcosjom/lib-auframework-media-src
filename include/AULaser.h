//
//  AULaser.h
//  DodoTownNucleo
//
//  Created by Marcos Ortega on 22/09/12.
//
//

#ifndef DodoTownNucleo_AULaser_h
#define DodoTownNucleo_AULaser_h

#include "AUEscenaLaser.h"
#include "AUEscenaSensor.h"
#include "AUNodoElectroCarga.h"
#include "AUEscenaLuzRadial.h"
#include "AUEscenaGrupoParticulasFuego.h"

class AULaser;

class IEscuchadorLaser {
	public:
		virtual ~IEscuchadorLaser(){
			//
		}
		//Retornan el tiempo en segundos que debe esperar el laser para ejecutar el siguiente evento
		virtual float laserColisionado(AULaser* objLaser, /*b2Body*/void* cuerpoColisionLaser, AUEscenaSensor* sensorComandos) = 0;
		virtual float laserLiberado(AULaser* objLaser, AUEscenaSensor* sensorComandos) = 0;
};

class AULaser : public AUNodoElectroCarga, public IEscenaLaserEscuchador {
	public:
		AULaser();
		AULaser(AULaser* otraInstancia);
		virtual ~AULaser();
		//
		bool					laserActivo();
		bool					desactivarLaserAlContacto();
		bool					ignorarPrimeraColision();
		bool					detectarColisionConCuerdas();
		float					cargaParaConsiderarCargada();
		float					alcanceLaser();
		float					anchoLaser();
		float					factorCortaCuerdas();
		NBPunto					velocidadDesplazamientoTextura();
		AUEscenaSensor*			sensorComandosColision();
		AUEscenaSensor*			sensorComandosDescolision();
		IEscuchadorLaser*		escuchadorLaser();
		float					segundosEsperaDespuesDeObstruccion();
		float					segundosEsperaDespuesDeLiberacion();
		//
		void					establecerLaserActivo(bool activo);
		void					establecerDesactivarLaseAlContacto(bool desactivar);
		void					establecerIgnorarPrimeraColision(bool ignorarPrimeraColision);
		void					establecerCargaParaConsiderarCargada(float cargaRel);
		void					establecerAlcanceLaser(float alcanceLaser);
		void					establecerAnchoLaser(float anchoLaser);
		void					establecerFactorCortaCuerdas(float factorCortaCuerdas);
		void					establecerDetectarColisionConCuerdas(bool detectarColisionConCuerdas);
		void					establecerVelocidadDesplazamientoTextura(const NBPunto &velDespTex);
		void					establecerSensorComandosColision(AUEscenaSensor* sensorComandos);
		void					establecerSensorComandosDescolision(AUEscenaSensor* sensorComandos);
		void					establecerSensorComandosSegunPlantilla(AULaser* instanciaPlantilla, AUArreglo* hijosPlantilla, AUArreglo* hijosLocal);
		void					establecerEscuchadorLaser(IEscuchadorLaser* escuchador);
		void					establecerSegundosEsperaDespuesDeObstruccion(float segs);
		void					establecerSegundosEsperaDespuesDeLiberacion(float segs);
		//
		virtual void			tickAnimacion(float segsTranscurridos);
		//
		virtual	NBCajaAABB		cajaEscenaPresentacion();
		virtual float			electrocargaVirtualParaDestinos();
		virtual float			electrocargaRealParaDestinos();
		virtual bool			electrocargaEsSoloConsumidor();
		//
		virtual void			laserEscenaTickObstruccionPorFisica(AUEscenaLaser* objEscenaLaser, const float segsTranscurridos, /*b2Body*/ void* objCuerpo, const NBPunto &posEscena, const NBPunto &vecNormal);
		virtual void			laserEscenaTickObstruccionPorCuerda(AUEscenaLaser* objEscenaLaser, const float segsTranscurridos, ICuerdaCortable* objCuerda, const NBPunto &posEscena, const STCuerdaCorte &propsColision);
		//guardar y cargar en XML
		static bool				agregarXmlInternoEn(AULaser* explosivo, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloObjetosParaID);
		static bool				interpretarXmlInterno(AULaser* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos, AUArregloNativoP<SI32>* arregloIDs);
		//guardar y cargar en binario
		static bool				agregarBitsInternosEn(AULaser* explosivo, AUArchivo* guardarEn, AUArreglo* arregloObjetosParaID);
		static bool				interpretarBitsInternos(AULaser* cargarEn, AUArchivo* cargarDe, AUArreglo* arregloObjetosParaID);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		virtual void			agregadoEnEscena();
		virtual void			quitandoDeEscena();
	private:
		bool					_laserActivo;
		bool					_desactivarDespuesDeContacto;
		bool					_ultimoTickConColision;
		bool					_detectarColisionConCuerdas;
		//
		float					_cargaActualRecibida;
		float					_cargaParaConsiderarCargada;
		float					_factorCortaCuerdas;
		//
		AUEscenaLuzRadial*		_luzFuego;						//Para los laseres que cortan cuerdas
		AUEscenaGrupoParticulasFuego* _particulasFuego;			//Para los laseres que cortan cuerdas
		//
		AUEscenaSensor*			_sensorComandosColision;
		AUEscenaSensor*			_sensorComandosDescolision;
		IEscuchadorLaser*		_escuchadorLaser;
		float					_segundosEsperaDespuesDeObstruccion;
		float					_segundosEsperaDespuesDeLiberacion;
		//Fisica
		float					_segundosEsperarParaDispararSiguienteEvento; //Evita que el laser ejecute comando tras comando con el roce de cada cuerpo
		float					_fraccionMinimaColisionLaser;
		float					_segundaFraccionMinimaColisionLaser;
		void*					_cuerpoColisionMasCercana; //b2Body*
		void*					_segundoCuerpoColisionMasCercana; //b2Body*
		//
		AUEscenaLaser*			_presentacionLaser;
		//
		void					privActualizarFiguraLaser(const float alcance, const float ancho);
};


#endif
