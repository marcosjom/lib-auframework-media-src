#ifndef AUESCENASPRITEPORCION_H_INCLUIDO
#define AUESCENASPRITEPORCION_H_INCLUIDO

#include "NBCajaAABB.h"
#include "AUEscenaSprite.h"
#include "AUArchivo.h"

class AUEscenaSpritePorcion : public AUEscenaSprite {
	public:
		AUEscenaSpritePorcion();
		AUEscenaSpritePorcion(NBTamano tamano);
		AUEscenaSpritePorcion(float ancho, float alto);
		AUEscenaSpritePorcion(float x, float y, float ancho, float alto);
		AUEscenaSpritePorcion(AUTextura* textura);
		AUEscenaSpritePorcion(AUEscenaSpritePorcion* otra);
		virtual ~AUEscenaSpritePorcion();
		//
		NBCajaAABB			porcionTextura() const;
		void				establecerPorcionTextura(float relXMin, float relXMax, float relYMin, float relYMax);
		//guardar y cargar en XML
		static bool			agregarXmlInternoEn(AUEscenaSpritePorcion* sprite, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUEscenaSpritePorcion* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool			agregarBitsInternosEn(AUEscenaSpritePorcion* sprite, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUEscenaSpritePorcion* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		NBCajaAABB			_porcionTextura;
		//
		virtual STNBRectI16	privGetTextureRect();
		virtual void		privEstablecerCoordenadasTextura(NBVerticeGL* verticesGL);
}; 

#endif
