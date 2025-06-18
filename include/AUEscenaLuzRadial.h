#ifndef AUEscenaLuzRadial_H_INCLUIDO
#define AUEscenaLuzRadial_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "NBRectangulo.h"
#include "AUTextura.h"
#include "NBGestorGL.h"
#include "NBGestorTexturas.h"
#include "NBGestorEscena.h"
#include "NBMatriz.h"

class AUEscenaLuzRadial : public AUEscenaObjeto {
	public:
		AUEscenaLuzRadial();
		AUEscenaLuzRadial(float alcanceLuz, bool produceSombras);
		AUEscenaLuzRadial(AUEscenaLuzRadial* otra);
		virtual ~AUEscenaLuzRadial();
		//
		float				alcanceLuz();
		float				factorMultiplicadorAlcance();
		float				radianesMinimo();
		float				radianesMaximo();
		ENFuenteIlumTipo	tipoLuz();
		bool				produceSombras();
		bool				angulosMinMaxActivos();
		AUTextura*			texturaLuz();
		void				establecerAlcanceLuz(float alcance);
		void				establecerFactorMultiplicadorAlcance(float factorMultiplicadorAlcance);
		void				establecerRadianesMinimo(float radianes);
		void				establecerRadianesMaximo(float radianes);
		void				establecerTipoLuz(ENFuenteIlumTipo tipoLuz);
		void				establecerProducirSombras(bool producirSombras);
		void				establecerAngulosMinMaxActivos(bool angulosMinMaxActivos);
		void				establecerTexturaLuz(AUTextura* textura);
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void			enviarComandosGL(UI8* punteroDatosModelo);
		//guardar y cargar en XML
		static bool			agregarXmlInternoEn(AUEscenaLuzRadial* sprite, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUEscenaLuzRadial* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool			agregarBitsInternosEn(AUEscenaLuzRadial* sprite, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUEscenaLuzRadial* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		bool				_registradoEnMotorIlum;
		bool				_angulosMinMaxActivos;
		bool				_produceSombras;				//determina si ademas de iluminar produce las sombras
		UI8					_tipoLuz;						//ENFuenteIlumTipo
		float				_alcanceLuz;					//en unidades GL
		float				_factorMultiplicadorAlcance;
		float				_radianesMin;
		float				_radianesMax;
		AUTextura*			_texturaLuz;
		//
		virtual void		agregadoEnEscena();
		virtual void		quitandoDeEscena();
	private:
		inline void			privInlineActualizarLuzEnGestor(){
							if(_registradoEnMotorIlum){
								float apertura = (_radianesMax>_radianesMin)?_radianesMax-_radianesMin:_radianesMax-(_radianesMin-PIx2);
								NBGestorEscena::actualizarFuenteIluminacion(this->idEscena, this, this->_agrupadorSombras, _texturaLuz, _alcanceLuz*_factorMultiplicadorAlcance, (ENFuenteIlumTipo)_tipoLuz, _produceSombras, _angulosMinMaxActivos, _radianesMin, (apertura<=PI)?apertura:PI);
							}
		}
};

#endif
