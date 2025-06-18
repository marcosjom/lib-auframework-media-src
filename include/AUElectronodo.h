//
//  AUElectronodo.h
//  EditorCuerpos
//
//  Created by Marcos Ortega on 14/12/12.
//
//

#ifndef EditorCuerpos_AUElectronodo_h
#define EditorCuerpos_AUElectronodo_h

#include "AUEscenaObjeto.h"
#include "AUEscenaContenedor.h"
#include "AUEscenaSprite.h"
#include "AUEscenaElectroRayo.h"
#include "NBAnimador.h"
#include "NBGestorPersonajes.h"

enum ENElectronodoPresenta {
	ENElectronodoPresenta_Completa,
	ENElectronodoPresenta_Simple,
	ENElectronodoPresenta_Conteo
};

struct STElectronodoPaso {
	float segsDuracion;
	AUEscenaObjeto* objDestino;
	//
	bool operator==(const STElectronodoPaso &otro) const {
		return (segsDuracion==otro.segsDuracion && objDestino==otro.objDestino);
	}
	bool operator!=(const STElectronodoPaso &otro) const {
		return !(segsDuracion==otro.segsDuracion && objDestino==otro.objDestino);
	}
};

class AUElectronodo : public AUEscenaContenedor, public NBAnimador, public IPersonajeVictimario {
	public:
		AUElectronodo();
		AUElectronodo(AUElectronodo* otraInstancia);
		virtual ~AUElectronodo();
		//
		ENElectronodoPresenta		tipoPrensentacion();
		float						segsEsperaInicial();
		AUArregloNativoP<STElectronodoPaso>* pasosLanzamiento();
		//
		void						establecerTipoPresentacion(ENElectronodoPresenta tipoElectronodo);
		void						establecerSegsEsperaInicial(float segsEsperaInicial);
		void						agregarPasoLanzamiento(float segsDuracion, AUEscenaObjeto* objDestino);
		void						actualizarDestinoPasoLanzamiento(UI16 indice, AUEscenaObjeto* objDestino);
		void						actualizarDestinosPasosLanzamientosSegunPlantilla(AUElectronodo* instanciaPlantilla, AUArreglo* hijosPlantilla, AUArreglo* hijosLocal);
		void						quitarPasoLanzamientoEnindice(UI16 indice);
		void						moverPasoLanzamiento(UI16 indiceActual, UI16 indiceDestino);
		//
		virtual bool				personajeDanoHaciaVictima(const NBCajaAABB &cajaEscenaVictima, STPersonajeDano* guardarDanoEn);
		//
		virtual void				tickAnimacion(float segsTranscurridos);
		//guardar y cargar en XML
		static bool					agregarXmlInternoEn(AUElectronodo* electronodo, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloCapasPorID);
		static bool					interpretarXmlInterno(AUElectronodo* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, SI32 idCapaPorDefecto, AUArregloNativoP<SI32>* arregloIDsCapas, AUArreglo* arreglosObjetosPorCapa, AUArreglo* arregloIDsObjetosPorCapa);
		//guardar y cargar en binario
		static bool					agregarBitsInternosEn(AUElectronodo* electronodo, AUArchivo* guardarEn, AUArreglo* arregloCapasPorID);
		static bool					interpretarBitsInternos(AUElectronodo* cargarEn, AUArchivo* cargarDe, AUArreglo* capasCargadas);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		virtual void				agregadoEnEscena();
		virtual void				quitandoDeEscena();
	private:
		UI8							_tipoPresentacion; //ENElectronodoPresenta
		UI8							_iPasoActual;
		UI8							_conteoPasoaActual;
		float						_segsEsperandoInicio;
		float						_segsEsperaInicial;
		float						_segsTotalPasoActual;
		float						_segsAcumPasoActual;
		AUArregloNativoMutableP<STElectronodoPaso> _pasosLanzamiento;
		AUEscenaSprite*				_spriteBase;
		AUEscenaElectroRayo*		_rayosElectricos;
};

#endif
