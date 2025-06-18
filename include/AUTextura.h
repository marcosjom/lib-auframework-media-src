#ifndef AUTEXTURA_H_INCLUIDO
#define AUTEXTURA_H_INCLUIDO

//#define AU_TEXTURA_DIFERENCIA_PIXEL_EN_ATLAS	0.001f	//margen a sumar o restar para evitar que dos imagenes compartan contenido dentro de un atlas

#include "AUObjeto.h"
#include "NBPunto.h"
#include "NBPuntoColor.h"
#include "NBTamano.h"
#include "NBRectangulo.h"
#include "NBMatriz.h"
#include "NBGestorGL.h"
#include "AUMallaMapa.h"

class NBGestorTexturas;

typedef enum ENTexturaOrdenV_ {
	ENTexturaOrdenV_HaciaAbajo = 0,		//cuando la imagen esta almacenada en OpenGL verticalmente bien
	ENTexturaOrdenV_HaciaArriba,		//cuando la imagen esta almacenada en OpenGL verticalmente invertida
	ENTexturaOrdenV_Conteo
} ENTexturaOrdenV;

typedef enum ENTexturaModoCoordenadas_ {
	ENTexturaModoCoordenadas_Entrelazadas = 0,
	ENTexturaModoCoordenadas_Antihorario,
	//
	ENTexturaModoCoordenadas_Conteo
} ENTexturaModoCoordenadas;

typedef enum ENTexturaEstadoCarga_ {
	ENTexturaEstadoCarga_Cargada = 0,	//Propiedades y datos cargados
	ENTexturaEstadoCarga_Cargando,		//Propiedades cargadas, datos en proceso
	ENTexturaEstadoCarga_Precargada,	//Ni propiedades ni datos cargados
	//
	ENTexturaEstadoCarga_Conteo
} ENTexturaEstadoCarga;

//NOTA IMPORTANTE: OpenGL mapea las coordenadas relativas en las texturas a partir de los bordes de los mapas de bits,
//pero, las coordenadas relativas que se envian al motorGL deben apuntar al centro de cada texel. Es decir evitar usar los (0,0) y (1,1).
//Esto evita que se pinten los texeles continuos de cada texturas en el atlas.
class AUTextura : public AUObjeto {
	public:
		AUTextura(const UI32 pIdTexturaAtlas, const ENTexturaOrdenV ordenFilasImagen, const ENTexturaEstadoCarga estadoCarga, const UI16 anchoAtlas, const UI16 altoAtlas, const UI16 posicionXEnAtlas, const UI16 posicionYEnAtlas, const UI16 ancho, const UI16 alto, const UI8 escalaBase2, const FLOAT escalaParaHD, const ENTexturaModoPintado modoPintado, const char* refName, AUMallaMapa* mallaMapa);
		AUTextura(AUTextura* otra);
		virtual					~AUTextura();
		//
		void					relinkearTextura(UI32 pIdTexturaAtlas);
		//
		ENTexturaOrdenV			ordenFilasImagen() const;
		ENTexturaEstadoCarga	estadoCarga() const;
		ENTexturaModoPintado	modoPintado() const;
		NBTamano				tamano() const;
		NBTamano				tamanoHD() const;	//este es el tamano de la textura multiplicado para la escala en escena (implementado para normalizar renderizados HD y SD)
		UI8						escalaBase2() const;
		FLOAT					escalaParaHD() const;
		NBTamano				tamanoAtlas() const;
		NBRectanguloP<UI16>		areaEnAtlas() const;
		//
		const char*				refName() const;
		AUMallaMapa*			mallaMapa() const;
		float					meshFirstPortionSzHD(const ENMallaCorteSentido sense) const;
		float					meshLastPortionSzHD(const ENMallaCorteSentido sense) const;
		//
		static NBCajaAABBP<double> cajaRelativaTexeles(const UI16 anchoAtlas, const UI16 altoAtlas, const UI16 posicionXEnAtlas, const UI16 posicionYEnAtlas, const UI16 anchoEnAtlas, const UI16 altoEnAtlas);
		NBCajaAABBP<double>		cajaRelativaTexeles() const;
		NBCajaAABBP<double>		cajaRelativaTexelesConMatrizInversa() const;
		//Nota importante: idTextura es GLuint, intente usar UI16 pero existen implementaciones
		//de OpenGL que generan valores muy por encima del rango de una UI16.
		UI32					idTexturaGL;			//Optmizacion, usar esta variable publica "idTexturaGL" en lugar del metodo "idTexturaGL()". Tiene alta demanda en ejecucion.
		//
		static void				cargarCoordenadasTextura(NBVerticeGL* guardarCoordenadasEn, const ENTexturaModoCoordenadas modoCoordenadas, const ENTexturaOrdenV ordenFilasImagen, const UI16 anchoAtlas, const UI16 altoAtlas, const UI16 posicionXEnAtlas, const UI16 posicionYEnAtlas, const UI16 anchoEnAtlas, const UI16 altoEnAtlas);
		static void				cargarCoordenadasTextura(NBVerticeGL* guardarCoordenadasEn, UI16 conteoCoordenadas, const UI16 anchoAtlas, const UI16 altoAtlas, const UI16 posicionXEnAtlas, const UI16 posicionYEnAtlas, const UI16 anchoEnAtlas, const UI16 altoEnAtlas, const NBPunto &texPosCero, const NBPunto &texVecUnitCreceX, const NBPunto &texVecUnitCreceY, const float texMaxCrecimientoX, const float texMaxCrecimientoY);
		void					cargarCoordenadasTextura(NBVerticeGL* guardarCoordenadasEn, UI16 conteoCoordenadas, const NBPunto &texPosCero, const NBPunto &texVecUnitCreceX, const NBPunto &texVecUnitCreceY, const float texMaxCrecimientoX, const float texMaxCrecimientoY);
		void					cargarCoordenadasTextura(NBVerticeGL* guardarCoordenadasEn, ENTexturaModoCoordenadas modoCoordenadas = ENTexturaModoCoordenadas_Entrelazadas);
		void					cargarCoordenadasTextura(NBVerticeGL* guardarCoordenadasEn, float relXMin, float relXMax, float relYMin, float relYMax, ENTexturaModoCoordenadas modoCoordenadas = ENTexturaModoCoordenadas_Entrelazadas);
		static void				cargarCoordenadasTextura2(NBVerticeGL2* guardarCoordenadasEn, SI32 unidadTextura, const ENTexturaModoCoordenadas modoCoordenadas, const ENTexturaOrdenV ordenFilasImagen, const UI16 anchoAtlas, const UI16 altoAtlas, const UI16 posicionXEnAtlas, const UI16 posicionYEnAtlas, const UI16 anchoEnAtlas, const UI16 altoEnAtlas);
		void					cargarCoordenadasTextura2(NBVerticeGL2* guardarCoordenadasEn, SI32 unidadTextura, ENTexturaModoCoordenadas modoCoordenadas = ENTexturaModoCoordenadas_Entrelazadas);
		void					cargarCoordenadasTextura2(NBVerticeGL2* guardarCoordenadasEn, float relXMin, float relYMin, float relXMax, float relYMax, SI32 unidadTextura, ENTexturaModoCoordenadas modoCoordenadas = ENTexturaModoCoordenadas_Entrelazadas);
		void					cargarCoordenadasTextura3(NBVerticeGL3* guardarCoordenadasEn, SI32 unidadTextura, ENTexturaModoCoordenadas modoCoordenadas);
		void					cargarCoordenadasPatron(const NBVerticeGL* coordenadasVertices, int conteoVertices, NBVerticeGL* guardarCoordenadasEn/*, const NBMatriz &matrizTransformaciones*/);
		void					cargarCoordenadasPatron(const NBPunto* coordenadasVertices, int conteoVertices, NBVerticeGL* guardarCoordenadasEn/*, const NBMatriz &matrizTransformaciones*/);
		void					cargarCoordenadasPatron(const NBPuntoColor* coordenadasVertices, int conteoVertices, NBVerticeGL* guardarCoordenadasEn/*, const NBMatriz &matrizTransformaciones*/);
		void					cargarCoordenadasPatron(const NBVerticeGL* coordenadasVertices, int conteoVertices, NBVerticeGL* guardarCoordenadasEn, const NBPunto &traslado);
		void					cargarCoordenadasPatron(const NBPunto* coordenadasVertices, int conteoVertices, NBVerticeGL* guardarCoordenadasEn, const NBPunto &traslado);
		void					cargarCoordenadasPatron(const NBPuntoColor* coordenadasVertices, int conteoVertices, NBVerticeGL* guardarCoordenadasEn, const NBPunto &traslado);
		void					cargarCoordenadasPatron2(const NBVerticeGL2* coordenadasVertices, int conteoVertices, NBVerticeGL2* guardarCoordenadasEn, SI32 unidadTextura, const NBMatriz &matrizTransformaciones);
		void					cargarCoordenadasPatron2(const NBPunto* coordenadasVertices, int conteoVertices, NBVerticeGL2* guardarCoordenadasEn, SI32 unidadTextura, const NBMatriz &matrizTransformaciones);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
		//
		friend class			NBGestorTexturas;
	protected:
		UI8						_ordenFilasImagen;	//ENTexturaOrdenV
		UI8						_estadoCarga;		//ENTexturaEstadoCarga
		UI8						_modoPintado;		//ENTexturaModoPintado
		UI8						_escalaBase2;		//Para determinar el multiplicador/divisor entre las propiedades de diseno y las de renderizado (ver AUMallaMapa y AUAtlasMapa).
		FLOAT					_escalaParaHD;
		NBTamanoP<UI16>			_tamAtlas;
		NBRectanguloP<UI16>		_areaEnAtlas;
		AUCadenaMutable8		_refName;
		AUMallaMapa*			_mallaMapa;
		//Datos pre-calculados
		NBCajaAABBP<double>		_cajaRelTexeles; //en el rango de min(0.0f) a max(1.0f)
	private:
		void					privInicializar(const UI32 pIdTexturaAtlas, const ENTexturaOrdenV ordenFilasImagen, const ENTexturaEstadoCarga estadoCarga, const UI16 anchoAtlas, const UI16 altoAtlas, const UI16 posicionXEnAtlas, const UI16 posicionYEnAtlas, const UI16 ancho, const UI16 alto, const UI8 escalaBase2, const FLOAT escalaParaHD, const ENTexturaModoPintado modoPintado, const char* refName, AUMallaMapa* mallaMapa);
		void					privEstablecerEstadoCarga(const ENTexturaEstadoCarga estadoCarga);
};

#endif
