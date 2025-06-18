#ifndef AUEscenaSpriteElastico_H_INCLUIDO
#define AUEscenaSpriteElastico_H_INCLUIDO

#include "AUEscenaSprite.h"
#include "NBRectangulo.h"
#include "AUTextura.h"
#include "NBGestorGL.h"
#include "NBGestorTexturas.h"
#include "NBColor.h"
#include "NBMatriz.h"
#include "AUArchivo.h"
#include "AUMallaMapa.h"

//Para renderizar un sprite no-segmentado
struct STSpriteElasticoRender {
	UI32		idTexturaGL;
	bool		esMascaraAlpha;
	STBloqueGL	bloqueIndicesGL;
	UI16		verticesUsadosH;	//Cantidad de vertices horizontales
	UI16		verticesUsadosV;	//Cantidad de vertices verticales
	UI16		conteoIndicesSaltosV;	//Cantidad de indices de filas que deben omitirse (son repeticiones)
};

class AUEscenaSpriteElastico : public AUEscenaSprite {
	public:
		AUEscenaSpriteElastico();
		AUEscenaSpriteElastico(const NBTamano tamano);
		AUEscenaSpriteElastico(const float ancho, const float alto);
		AUEscenaSpriteElastico(const float x, const float y, const float ancho, const float alto);
		AUEscenaSpriteElastico(AUTextura* textura);
		AUEscenaSpriteElastico(AUTextura* textura, const ENSpriteAlineaH alinH, const ENSpriteAlineaV alinV);
		AUEscenaSpriteElastico(AUEscenaSpriteElastico* otra);
		virtual ~AUEscenaSpriteElastico();
		//
		AUMallaMapa*		mallaMapa() const;
		void				establecerMallaMapa(AUMallaMapa* mallaMapa);
		//
		NBTamano			mallaFactorEstira() const;
		void				establecerMallaFactorEstira(const NBTamano factor);
		void				establecerMallaFactorEstira(const float factorH, const float factorV);
		//Para renderizar
		virtual PTRfunCmdsGL actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void			enviarComandosGL(UI8* punteroDatosModelo);
		//guardar y cargar en XML
		static bool			agregarXmlInternoEn(AUEscenaSpriteElastico* sprite, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUEscenaSpriteElastico* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool			agregarBitsInternosEn(AUEscenaSpriteElastico* sprite, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUEscenaSpriteElastico* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUMallaMapa*		_mallaMapa;
		NBTamano			_mallaFactorEstira;
		struct {
			STMallaMapaBase	data;
			void*			texPtr;
			void*			meshPtr;
		} _mapCache;
		//
		static NBVerticeGL* privBuildModelAddRow(const float yLocal, const float texAtlasY, NBVerticeGL* verticesGL, const NBVerticeGL* verticesGLSigUlt, const AUArregloNativoP<STMallaCorte>* cutsV, const float mapMultiplier, const bool forceElasticH, const float totalElasticWidths, const SI32 totalElasticCutsH, const float factorElasticH, const float scaleFixedCols, const float scaleElasticCols, const float xLocalMin, const float texWidth, const float texScaleH, const double texAtlasXMin, const double texAtlasXMax, const NBColor8* colorMultiplicado, const NBMatriz &matrizEscena);
};

#endif
