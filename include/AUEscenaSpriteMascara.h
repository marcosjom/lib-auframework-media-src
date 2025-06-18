#ifndef AUEscenaSpriteMascara_H_INCLUIDO
#define AUEscenaSpriteMascara_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "NBRectangulo.h"
#include "AUTextura.h"
#include "NBGestorGL.h"
#include "NBGestorTexturas.h"
#include "NBColor.h"
#include "NBMatriz.h"
#include "AUArchivo.h"

struct STSpriteMascaraRender {
	UI32		idTexturaGLMascara;
	UI32		idTexturaGLFondo;
	STBloqueGL	bloqueIndicesGL;
};

class AUEscenaSpriteMascara : public AUEscenaObjeto {
	public:
		AUEscenaSpriteMascara();
		AUEscenaSpriteMascara(NBTamano tamano);
		AUEscenaSpriteMascara(float ancho, float alto);
		AUEscenaSpriteMascara(float x, float y, float ancho, float alto);
		AUEscenaSpriteMascara(AUTextura* texturaMascara, AUTextura* texturaFondo);
		AUEscenaSpriteMascara(AUEscenaSpriteMascara* otra);
		virtual ~AUEscenaSpriteMascara();
		//
		AUTextura*			texturaMascara() const;
		AUTextura*			texturaFondo() const;
		NBRectangulo		limites() const;
		NBTamano			desplazamientoFondo() const;
		NBTamano			escalaFondo() const;
		void 				establecerTexturaMascara(AUTextura* tex);
		void 				establecerTexturaFondo(AUTextura* tex);
		void				establecerDesplazamientoFondo(const float despX, const float despY);
		void				establecerEscalaFondo(const float escalaAncho, const float escalaAlto);
		void				ajustarFondoSegunMascara();
		void				redimensionar(AUTextura* tex);
		void				redimensionar(NBTamano tamano);
		void				redimensionar(float ancho, float alto);
		void				redimensionar(float x, float y, float ancho, float alto);
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void			enviarComandosGL(UI8* punteroDatosModelo);
		//guardar y cargar en XML
		static bool			agregarXmlInternoEn(AUEscenaSpriteMascara* sprite, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUEscenaSpriteMascara* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool			agregarBitsInternosEn(AUEscenaSpriteMascara* sprite, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUEscenaSpriteMascara* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUTextura* 			_texturaMascara;
		AUTextura* 			_texturaFondo;
		NBRectangulo		_limites;
		NBTamano			_desplazamientoFondo; //en unidades de escena
		NBTamano			_escalaFondo;	//en unidades de escena
		//
		virtual void		agregadoEnEscena();
		virtual void		quitandoDeEscena();
		//
		virtual void		privEstablecerCoordenadasTexturas(NBVerticeGL2* verticesGL);
		inline void			privInlineInicializar(float x, float y, float ancho, float alto);
};

#endif
