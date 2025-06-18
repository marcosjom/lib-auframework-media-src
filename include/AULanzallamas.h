//
//  AULanzallamas.h
//  EditorCuerpos
//
//  Created by Marcos Ortega on 14/12/12.
//
//

#ifndef EditorCuerpos_AULanzallamas_h
#define EditorCuerpos_AULanzallamas_h

#include "AUEscenaSprite.h"
#include "AUEscenaLuzRadial.h"
#include "AUEscenaContenedor.h"
#include "AUEscenaGrupoParticulasFuego.h"
#include "NBAnimador.h"
#include "NBGestorPersonajes.h"

#define AULANZALLAMAS_MAX_PARTICULAS_FUEGO			120
#define AULANZALLAMAS_SEGS_VIDA_PARTICULAS_FUEGO	0.25f

enum ENLanzallamaPresenta {
	ENLanzallamaPresenta_Completa,
	ENLanzallamaPresenta_Simple,
	ENLanzallamaPresenta_Conteo
};

struct STLanzallamaPaso {
	float segsDuracion;
	float alcanceFuego;
	//
	bool operator==(const STLanzallamaPaso &otro) const {
		return (segsDuracion==otro.segsDuracion && alcanceFuego==otro.alcanceFuego);
	}
	bool operator!=(const STLanzallamaPaso &otro) const {
		return !(segsDuracion==otro.segsDuracion && alcanceFuego==otro.alcanceFuego);
	}
};

class AULanzallamas : public AUEscenaContenedor, public NBAnimador, public IPersonajeVictimario {
	public:
		AULanzallamas();
		AULanzallamas(AULanzallamas* otraInstancia);
		virtual ~AULanzallamas();
		//
		ENLanzallamaPresenta		tipoPrensentacion();
		float						segsEsperaInicial();
		AUArregloNativoP<STLanzallamaPaso>* pasosLanzamiento();
		//
		void						establecerTipoPresentacion(ENLanzallamaPresenta tipoLanzallamas);
		void						establecerSegsEsperaInicial(float segsEsperaInicial);
		void						agregarPasoLanzamiento(float segsDuracion, float alcanceFuego);
		void						quitarPasoLanzamientoEnindice(UI16 indice);
		void						moverPasoLanzamiento(UI16 indiceActual, UI16 indiceDestino);
		//
		virtual bool				personajeDanoHaciaVictima(const NBCajaAABB &cajaEscenaVictima, STPersonajeDano* guardarDanoEn);
		//
		virtual void				tickAnimacion(float segsTranscurridos);
		//guardar y cargar en XML
		static bool					agregarXmlInternoEn(AULanzallamas* lanzallamas, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool					interpretarXmlInterno(AULanzallamas* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool					agregarBitsInternosEn(AULanzallamas* lanzallamas, AUArchivo* guardarEn);
		static bool					interpretarBitsInternos(AULanzallamas* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		virtual void				agregadoEnEscena();
		virtual void				quitandoDeEscena();
	private:
		UI8							_tipoPresentacion; //ENLanzallamaPresenta
		UI8							_iPasoActual;
		float						_segsEsperaInicial;
		float						_segsEsperandoInicio;
		float						_segsTotalPasoActual;
		float						_segsAcumPasoActual;
		float						_alcanceActual;
		float						_alcanceDestino;
		AUEscenaSprite*				_spriteBase;
		AUEscenaLuzRadial*			_luzFuego;
		AUEscenaGrupoParticulasFuego* _particulasFuego;
		AUArregloNativoMutableP<STLanzallamaPaso> _pasosLanzamiento;
};

#endif
