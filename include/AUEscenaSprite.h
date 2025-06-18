#ifndef AUESCENASPRITE_H_INCLUIDO
#define AUESCENASPRITE_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "NBRectangulo.h"
#include "AUTextura.h"
#include "NBGestorGL.h"
#include "NBGestorTexturas.h"
#include "NBColor.h"
#include "NBMatriz.h"
#include "AUArchivo.h"

typedef enum ENSpriteAlineaH_ {
	ENSpriteAlineaH_Izq = 0,
	ENSpriteAlineaH_Cen,
	ENSpriteAlineaH_Der,
	ENSpriteAlineaH_Conteo
} ENSpriteAlineaH;

typedef enum ENSpriteAlineaV_ {
	ENSpriteAlineaV_Arr = 0,
	ENSpriteAlineaV_Cen,
	ENSpriteAlineaV_Aba,
	ENSpriteAlineaV_Conteo
} ENSpriteAlineaV;

typedef enum ENSpriteRenderType_ {
	ENSpriteRenderType_Geom = 0,	//Geometrics/trinagles drawing
	ENSpriteRenderType_Tex,			//Texture directly to viewport
	ENSpriteRenderType_Count
} ENSpriteRenderType;

//When 'isDrawTexture' is true (render directly to screen)
typedef struct STSpriteRenderTex_ {
	ENSpriteRenderType	type;
	UI32				idTexturaGL;
	bool				esMascaraAlpha;
	STNBRectI16			texRect;	//texture rect
	STNBRect			viewRect;	//viewport rect
	STNBColor8			color;		//color
} STSpriteRenderTex;

//Para renderizar un sprite no-segmentado
typedef struct STSpriteRenderGeo_ {
	ENSpriteRenderType	type;
	UI32				idTexturaGL;
	bool				esMascaraAlpha;
	STBloqueGL			bloqueIndicesGL;
} STSpriteRenderGeo;

class AUEscenaSprite : public AUEscenaObjeto {
	public:
		AUEscenaSprite();
		AUEscenaSprite(NBTamano tamano);
		AUEscenaSprite(float ancho, float alto);
		AUEscenaSprite(float x, float y, float ancho, float alto);
		AUEscenaSprite(AUTextura* textura);
		AUEscenaSprite(AUTextura* textura, const ENSpriteAlineaH alinH, const ENSpriteAlineaV alinV);
		AUEscenaSprite(AUEscenaSprite* otra);
		virtual ~AUEscenaSprite();
		//
		AUTextura*			textura() const;
		NBRectangulo		limites() const;
		bool				esMascaraAlpha() const;
		void 				establecerTextura(AUTextura* nuevaTextura);
		void				establecerEsMascaraAlpha(bool esMascaraAlpha);
		void				redimensionar(AUTextura* textura);
		void				redimensionar(AUTextura* textura, const ENSpriteAlineaH alinH, const ENSpriteAlineaV alinV);
		void				redimensionar(const NBTamano tamano);
		void				redimensionar(const float ancho, const float alto);
		void				redimensionar(const float x, const float y, const float ancho, const float alto);
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		//Para renderizar
		virtual PTRfunCmdsGL actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void			enviarComandosGL(UI8* punteroDatosModelo);
		//guardar y cargar en XML
		static bool			agregarXmlInternoEn(AUEscenaSprite* sprite, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUEscenaSprite* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool			agregarBitsInternosEn(AUEscenaSprite* sprite, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUEscenaSprite* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		bool				_esMascaraAlpha;
		UI8					_modoTextura;
		NBRectangulo		_limites;
		AUTextura* 			_textura;
		virtual STNBRectI16	privGetTextureRect();
		virtual void		privEstablecerCoordenadasTextura(NBVerticeGL* verticesGL);
		virtual void		agregadoEnEscena();
		virtual void		quitandoDeEscena();
	private:
		inline void			privInlineInicializar(float x, float y, float ancho, float alto, bool esMascaraAlpha);
}; 

#endif
