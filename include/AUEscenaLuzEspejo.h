#ifndef AUEscenaLuzEspejo_H_INCLUIDO
#define AUEscenaLuzEspejo_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "NBRectangulo.h"
#include "AUTextura.h"
#include "NBGestorGL.h"
#include "NBGestorTexturas.h"
#include "NBGestorEscena.h"
#include "NBMatriz.h"

class AUEscenaLuzEspejo : public AUEscenaObjeto {
	public:
		AUEscenaLuzEspejo();
		AUEscenaLuzEspejo(AUEscenaLuzEspejo* otra);
		virtual ~AUEscenaLuzEspejo();
		//
		float				factorReflejaLuz();
		NBPunto				limiteEspejoInicial();
		NBPunto				limiteEspejoFinal();
		void				establecerFactorReflejaLuz(float factorReflejaLuz);
		void				establecerLimiteEspejoInicial(float xLocal, float yLocal);
		void				establecerLimiteEspejoFinal(float xLocal, float yLocal);
		void				establecerLimiteEspejoInicial(NBPunto posLocal);
		void				establecerLimiteEspejoFinal(NBPunto posLocal);
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void			enviarComandosGL(UI8* punteroDatosModelo);
		//guardar y cargar en XML
		static bool			agregarXmlInternoEn(AUEscenaLuzEspejo* sprite, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUEscenaLuzEspejo* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool			agregarBitsInternosEn(AUEscenaLuzEspejo* sprite, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUEscenaLuzEspejo* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		bool				_registradoEnMotorIlum;
		float				_factorReflejaLuz;
		NBPunto				_limitesEspejo[2];
		//
		virtual void		agregadoEnEscena();
		virtual void		quitandoDeEscena();
	private:
		inline void			privInlineActualizarLuzEnGestor(){
							if(_registradoEnMotorIlum){
								NBGestorEscena::actualizarFuenteEspejo(this->idEscena, this, _factorReflejaLuz, _limitesEspejo[0], _limitesEspejo[1]);
							}
		}
};

#endif
