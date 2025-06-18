
#include "AUFrameworkMediaStdAfx.h"
#include "AUTextura.h"

//NOTA IMPORTANTE: OpenGL mapea las coordenadas relativas en las texturas a partir de los bordes de los mapas de bits,
//pero, las coordenadas relativas que se envian al motorGL deben apuntar al centro de cada texel. Es decir evitar usar los (0,0) y (1,1).
//Esto evita que se pinten los texeles continuos de cada texturas en el atlas.

AUTextura::AUTextura(const UI32 pIdTexturaAtlas, const ENTexturaOrdenV ordenFilasImagen, const ENTexturaEstadoCarga estadoCarga, const UI16 anchoAtlas, const UI16 altoAtlas, const UI16 posicionXEnAtlas, const UI16 posicionYEnAtlas, const UI16 ancho, const UI16 alto, const UI8 escalaBase2, const FLOAT escalaParaHD, const ENTexturaModoPintado modoPintado, const char* refName, AUMallaMapa* mallaMapa) : AUObjeto()
	, idTexturaGL(pIdTexturaAtlas)
	, _ordenFilasImagen(ordenFilasImagen)
	, _estadoCarga(estadoCarga)
	, _modoPintado(modoPintado)
	//, _tamAtlas(otra->_tamAtlas)
	//, _areaEnAtlas(otra->_areaEnAtlas)
	, _escalaBase2(escalaBase2)
	, _escalaParaHD(escalaParaHD)
	//, _cajaRelTexeles(escalaParaHD)
	, _refName(this, refName)
	, _mallaMapa(NULL)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::AUTextura")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUTextura")
	_tamAtlas.ancho		= anchoAtlas;
	_tamAtlas.alto		= altoAtlas;
	_areaEnAtlas.x		= posicionXEnAtlas;
	_areaEnAtlas.y		= posicionYEnAtlas;
	_areaEnAtlas.ancho	= ancho;
	_areaEnAtlas.alto	= alto;
	_cajaRelTexeles		= AUTextura::cajaRelativaTexeles(_tamAtlas.ancho, _tamAtlas.alto, _areaEnAtlas.x, _areaEnAtlas.y, _areaEnAtlas.ancho, _areaEnAtlas.alto);
	if(mallaMapa != NULL){
		_mallaMapa		= new(this) AUMallaMapa(mallaMapa);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUTextura::AUTextura(AUTextura* otra) : AUObjeto()
	, idTexturaGL(otra->idTexturaGL)
	, _ordenFilasImagen(otra->_ordenFilasImagen)
	, _estadoCarga(otra->_estadoCarga)
	, _modoPintado(otra->_modoPintado)
	, _tamAtlas(otra->_tamAtlas)
	, _areaEnAtlas(otra->_areaEnAtlas)
	, _escalaBase2(otra->_escalaBase2)
	, _escalaParaHD(otra->_escalaParaHD)
	, _cajaRelTexeles(otra->_cajaRelTexeles)
	, _refName(this, &otra->_refName)
	, _mallaMapa(NULL)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::AUTextura")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUTextura")
	if(otra->_mallaMapa != NULL){
		_mallaMapa = new(this) AUMallaMapa(otra->_mallaMapa);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUTextura::privInicializar(const UI32 pIdTexturaAtlas, const ENTexturaOrdenV ordenFilasImagen, const ENTexturaEstadoCarga estadoCarga, const UI16 anchoAtlas, const UI16 altoAtlas, const UI16 posicionXEnAtlas, const UI16 posicionYEnAtlas, const UI16 ancho, const UI16 alto, const UI8 escalaBase2, const FLOAT escalaParaHD, const ENTexturaModoPintado modoPintado, const char* refName, AUMallaMapa* mallaMapa){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::privInicializar")
	idTexturaGL			= pIdTexturaAtlas;
	_ordenFilasImagen	= ordenFilasImagen;
	_estadoCarga		= estadoCarga;
	_modoPintado		= modoPintado;
	_tamAtlas.ancho		= anchoAtlas;
	_tamAtlas.alto		= altoAtlas;
	_areaEnAtlas.x		= posicionXEnAtlas;
	_areaEnAtlas.y		= posicionYEnAtlas;
	_areaEnAtlas.ancho	= ancho;
	_areaEnAtlas.alto	= alto;
	_escalaBase2		= escalaBase2;
	_escalaParaHD		= escalaParaHD;
	_cajaRelTexeles		= AUTextura::cajaRelativaTexeles(_tamAtlas.ancho, _tamAtlas.alto, _areaEnAtlas.x, _areaEnAtlas.y, _areaEnAtlas.ancho, _areaEnAtlas.alto);
	if(refName != NULL){
		_refName.establecer(refName);
	} else {
		_refName.vaciar();
	}
	if(_mallaMapa != NULL){
		_mallaMapa->liberar(NB_RETENEDOR_THIS);
		_mallaMapa		= NULL;
	}
	if(mallaMapa != NULL){
		_mallaMapa		= new(this) AUMallaMapa(mallaMapa);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUTextura::privEstablecerEstadoCarga(const ENTexturaEstadoCarga estadoCarga){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::privEstablecerEstadoCarga")
	_estadoCarga		= estadoCarga;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUTextura::~AUTextura(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::~AUTextura")
	if(_mallaMapa != NULL) _mallaMapa->liberar(NB_RETENEDOR_THIS); _mallaMapa = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUTextura::relinkearTextura(const UI32 pIdTexturaAtlas){
	idTexturaGL	 = pIdTexturaAtlas;
}

/*Optmizacion: usar la propiedad publica "idTexturaGL" en lugar del metodo "idTexturaGL()". Esta tiene alta demanda en ejecucion.
/ *GLuint AUTextura::idTexturaGL(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::idTextura")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _idTexturaAtlas;
}*/

NBRectanguloP<UI16> AUTextura::areaEnAtlas() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::areaEnAtlas")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _areaEnAtlas;
}

//

const char* AUTextura::refName() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::refName")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _refName.str();
}

AUMallaMapa* AUTextura::mallaMapa() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::mallaMapa")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _mallaMapa;
}

float AUTextura::meshFirstPortionSzHD(const ENMallaCorteSentido sense) const {
	const NBTamano szHD = this->tamanoHD();
	const float szBaseHD = (sense == ENMallaCorteSentido_Vertical ? szHD.alto : szHD.ancho);
	float r = szBaseHD;
	if(_mallaMapa != NULL){
		const float mapMultiplier = (float)this->escalaBase2() / (float)_mallaMapa->escalaBase2();
		const AUArregloNativoP<STMallaCorte>* cuts = _mallaMapa->cortesEnSentido(sense);
		if(cuts->conteo > 0){
			const NBTamano sz	= this->tamano();
			const float szBase	= (sense == ENMallaCorteSentido_Vertical ? sz.alto : sz.ancho);
			const float cutSz	= (float)cuts->elemPtr(0)->tamano * mapMultiplier;
			r = ((float)cutSz / (float)szBase) * szHD.alto;
		}
	}
	return r;
}

float AUTextura::meshLastPortionSzHD(const ENMallaCorteSentido sense) const {
	const NBTamano szHD = this->tamanoHD();
	const float szBaseHD = (sense == ENMallaCorteSentido_Vertical ? szHD.alto : szHD.ancho);
	float r = szBaseHD;
	if(_mallaMapa != NULL){
		const float mapMultiplier = (float)this->escalaBase2() / (float)_mallaMapa->escalaBase2();
		const AUArregloNativoP<STMallaCorte>* cuts = _mallaMapa->cortesEnSentido(sense);
		if(cuts->conteo > 0){
			const NBTamano sz	= this->tamano();
			const float szBase	= (sense == ENMallaCorteSentido_Vertical ? sz.alto : sz.ancho);
			const float cutSz	= (float)cuts->elemPtr(cuts->conteo - 1)->tamano * mapMultiplier;
			r = ((float)cutSz / (float)szBase) * szHD.alto;
			//PRINTF_INFO("Last: %f.\n", r);
		}
		/*const AUArregloNativoP<STMallaCorte>* cuts = _mallaMapa->cortesEnSentido(sense);
		if(cuts->conteo > 0){
			const NBTamano sz = this->tamano();
			const float szBase = (sense == ENMallaCorteSentido_Vertical ? sz.alto : sz.ancho);
			const float mapMultiplier = (float)this->escalaBase2() / (float)_mallaMapa->escalaBase2();
			float curVPos = 0.0f;
			SI32 i; for(i = 0; i < cuts->conteo; i++){
				PRINTF_INFO("Portion %d: %f.\n", i, (float)(cuts->elemPtr(i)->tamano * mapMultiplier));
				curVPos += (float)cuts->elemPtr(i)->tamano * mapMultiplier;
			}
			{
				r = ((float)(szBase - curVPos) / (float)szBase) * szHD.alto; //the remainig
				PRINTF_INFO("Last %d: %f.\n", i, r);
			}
		}*/
	}
	return r;
}

//

//NOTA IMPORTANTE: OpenGL mapea las coordenadas relativas en las texturas a partir de los bordes de los mapas de bits,
//pero, las coordenadas relativas que se envian al motorGL deben apuntar al centro de cada texel. Es decir evitar usar los (0,0) y (1,1).
//Esto evita que se pinten los texeles continuos de cada texturas en el atlas.
/*NBPunto AUTextura::centroTexelRelativoEnAtlas(SI32 xTexel, SI32 yTexel){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::centroTexelRelativoEnAtlas")
	NBPunto centroRelativoEnAtlas;
	//Coordenadas relativas, iniciando en los bordes de los texeles, pero apuntando al centro de cada texel
	centroRelativoEnAtlas.x = ((float)xTexel/(float)(_tamAtlas.ancho))+(0.5f/(float)_tamAtlas.ancho);
	centroRelativoEnAtlas.y	= ((float)yTexel/(float)(_tamAtlas.alto))+(0.5f/(float)_tamAtlas.alto);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return centroRelativoEnAtlas;
}*/

/*NBPunto AUTextura::centroTexelMultiplicadoMatrizInversaEnAtlas(SI32 xTexel, SI32 yTexel){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::centroTexelMultiplicadoMatrizInversaEnAtlas")
	NBPunto centroRelativoEnAtlas;
	//Coordenadas relativas, iniciando en los bordes de los texeles, pero apuntando al centro de cada texel
	centroRelativoEnAtlas.x = (((float)xTexel/(float)(_tamAtlas.ancho))+(0.5f/(float)_tamAtlas.ancho));
	centroRelativoEnAtlas.y	= (((float)yTexel/(float)(_tamAtlas.alto))+(0.5f/(float)_tamAtlas.alto));
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return centroRelativoEnAtlas;
}*/

NBCajaAABBP<double> AUTextura::cajaRelativaTexeles(const UI16 anchoAtlas, const UI16 altoAtlas, const UI16 posicionXEnAtlas, const UI16 posicionYEnAtlas, const UI16 anchoEnAtlas, const UI16 altoEnAtlas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cajaRelativaTexeles")
	NBCajaAABBP<double> cajaRelTexeles;
	if(anchoEnAtlas <= 0 || altoEnAtlas <= 0){
		cajaRelTexeles.xMin = 0.0f;
		cajaRelTexeles.yMin = 0.0f;
		cajaRelTexeles.xMax = 0.0f;
		cajaRelTexeles.yMax = 0.0f;
		//PRINTF_ERROR("TEX, el ancho o alto de textura es cero o negativo (TEXTURA NO SE PINTARA).\n");
	} else if(anchoAtlas <= 1 && altoAtlas <= 1){
		cajaRelTexeles.xMin = 0.0f;
		cajaRelTexeles.yMin = 0.0f;
		cajaRelTexeles.xMax = 1.0f;
		cajaRelTexeles.yMax = 1.0f;
		//PRINTF_ERROR("TEX, el ancho o alto del atlas es cero o negativo (TEXTURA NO SE PINTARA).\n");
	} else {
		//(0.0, 0.0) es el borde sup-izq del primer texel
		//(1.0, 1.0) es el borde inf-der del ultimo texel
		const double margenPix	= 0.50;
		const double xMin		= (double)posicionXEnAtlas;
		const double yMin		= (double)posicionYEnAtlas;
		const double xMax		= (double)posicionXEnAtlas + (double)anchoEnAtlas - 1.0f;
		const double yMax		= (double)posicionYEnAtlas + (double)altoEnAtlas - 1.0f;
		const double anchoAtlasD= (double) anchoAtlas;
		const double altoAtlasD	= (double) altoAtlas;
		const double xMaxAtlas	= anchoAtlasD - 1.0;
		const double yMaxAtlas	= altoAtlasD - 1.0;
		//
		NBASSERT(xMin >= 0.0 && xMin <= xMaxAtlas && yMin >= 0.0 && yMin <= yMaxAtlas)
		NBASSERT(xMax >= 0.0 && xMax <= xMaxAtlas && yMax >= 0.0 && yMax <= yMaxAtlas)
		//no-invertida-eje-y
		cajaRelTexeles.xMin	= (xMin / xMaxAtlas) - ((xMin / xMaxAtlas) / anchoAtlasD) + (margenPix / anchoAtlasD);
		cajaRelTexeles.yMin	= (yMin / yMaxAtlas) - ((yMin / yMaxAtlas) / altoAtlasD) + (margenPix / altoAtlasD);
		cajaRelTexeles.xMax	= (xMax / xMaxAtlas) + (((xMaxAtlas - xMax) / xMaxAtlas) / anchoAtlasD) - (margenPix / anchoAtlasD);
		cajaRelTexeles.yMax	= (yMax / yMaxAtlas) + (((yMaxAtlas - yMax) / yMaxAtlas) / altoAtlasD) - (margenPix / altoAtlasD);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return cajaRelTexeles;
}

NBCajaAABBP<double> AUTextura::cajaRelativaTexeles() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cajaRelativaTexeles")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _cajaRelTexeles;
}

NBCajaAABBP<double> AUTextura::cajaRelativaTexelesConMatrizInversa() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cajaRelativaTexelesConMatrizInversa")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _cajaRelTexeles;
}

ENTexturaOrdenV AUTextura::ordenFilasImagen() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::ordenFilasImagen")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (ENTexturaOrdenV)_ordenFilasImagen;
}

ENTexturaEstadoCarga AUTextura::estadoCarga() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::estadoCarga")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (ENTexturaEstadoCarga)_estadoCarga;
}

ENTexturaModoPintado AUTextura::modoPintado() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::estadoCarga")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (ENTexturaModoPintado)_modoPintado;
}

NBTamano AUTextura::tamano() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::tamano")
	NBASSERT(this->conteoReferencias()>0) //Objeto textura ya no existe en memoria
	NBTamano tam;
	tam.ancho = _areaEnAtlas.ancho;
	tam.alto = _areaEnAtlas.alto;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return tam;
}

NBTamano AUTextura::tamanoHD() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::tamanoHD")
	NBASSERT(this->conteoReferencias() > 0) //Objeto textura ya no existe en memoria
	NBTamano tamHD;
	tamHD.ancho	= (float)_areaEnAtlas.ancho * _escalaParaHD;
	tamHD.alto	= (float)_areaEnAtlas.alto * _escalaParaHD;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return tamHD;
}

UI8 AUTextura::escalaBase2() const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::escalaBase2")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _escalaBase2;
}

FLOAT AUTextura::escalaParaHD() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::escalaParaHD")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _escalaParaHD;
}

NBTamano AUTextura::tamanoAtlas() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::tamanoAtlas")
	NBTamano tamAtlas;
	tamAtlas.ancho	= _tamAtlas.ancho;
	tamAtlas.alto	= _tamAtlas.alto;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return tamAtlas;
}



//NOTA IMPORTANTE: OpenGL mapea las coordenadas relativas en las texturas a partir de los bordes de los mapas de bits,
//pero, las coordenadas relativas que se envian al motorGL deben apuntar al centro de cada texel. Es decir evitar usar los (0,0) y (1,1).
//Esto evita que se pinten los texeles continuos de cada texturas en el atlas.
void AUTextura::cargarCoordenadasTextura(NBVerticeGL* guardarCoordenadasEn, const ENTexturaModoCoordenadas modoCoordenadas, const ENTexturaOrdenV ordenFilasImagen, const UI16 anchoAtlas, const UI16 altoAtlas, const UI16 posicionXEnAtlas, const UI16 posicionYEnAtlas, const UI16 anchoEnAtlas, const UI16 altoEnAtlas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasTextura")
	const NBCajaAABBP<double> cajaRelTexeles = AUTextura::cajaRelativaTexeles(anchoAtlas, altoAtlas, posicionXEnAtlas, posicionYEnAtlas, anchoEnAtlas, altoEnAtlas);
	if(modoCoordenadas == ENTexturaModoCoordenadas_Antihorario){
		guardarCoordenadasEn[0].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex.y = cajaRelTexeles.yMin;
		guardarCoordenadasEn[1].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex.y = cajaRelTexeles.yMin;
		guardarCoordenadasEn[2].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[2].tex.y = cajaRelTexeles.yMax;
		guardarCoordenadasEn[3].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[3].tex.y = cajaRelTexeles.yMax;
	} else {
		//entrelazadas
		guardarCoordenadasEn[0].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex.y = cajaRelTexeles.yMin;
		guardarCoordenadasEn[1].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex.y = cajaRelTexeles.yMin;
		guardarCoordenadasEn[2].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[2].tex.y = cajaRelTexeles.yMax;
		guardarCoordenadasEn[3].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[3].tex.y = cajaRelTexeles.yMax;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUTextura::cargarCoordenadasTextura(NBVerticeGL* guardarCoordenadasEn, UI16 conteoCoordenadas, const UI16 anchoAtlas, const UI16 altoAtlas, const UI16 posicionXEnAtlas, const UI16 posicionYEnAtlas, const UI16 anchoEnAtlas, const UI16 altoEnAtlas, const NBPunto &texPosCero, const NBPunto &texVecUnitCreceX, const NBPunto &texVecUnitCreceY, const float texMaxCrecimientoX, const float texMaxCrecimientoY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasTextura")
	// Formato de Coordenadas escena esperadas
	//
	// (0, 0) --------> CreceXEscena
	// |
	// |
	// CreceYEscena
	//
	NBPuntoP<double> texVecUnitCreceXD;	NBPUNTO_ESTABLECER(texVecUnitCreceXD, texVecUnitCreceX.x, texVecUnitCreceX.y)
	NBPuntoP<double> texVecUnitCreceYD;	NBPUNTO_ESTABLECER(texVecUnitCreceYD, texVecUnitCreceY.x, texVecUnitCreceY.y)
	const double texMaxCrecimientoXD = texMaxCrecimientoX;
	const double texMaxCrecimientoYD = texMaxCrecimientoY;
	//
	const NBCajaAABBP<double> cajaRelTexeles = AUTextura::cajaRelativaTexeles(anchoAtlas, altoAtlas, posicionXEnAtlas, posicionYEnAtlas, anchoEnAtlas, altoEnAtlas);
	const double avanceRelX	= cajaRelTexeles.xMax - cajaRelTexeles.xMin;
	const double avanceRelY	= cajaRelTexeles.yMax - cajaRelTexeles.yMin;
	//
	NBPuntoP<double> vectorCoordenada; double avanceEscenaEnX, avanceEscenaEnY;
	UI16 i; const UI16 conteo = conteoCoordenadas;
	for(i = 0; i < conteo; i++){
		vectorCoordenada.x				= guardarCoordenadasEn[i].x - texPosCero.x;
		vectorCoordenada.y				= guardarCoordenadasEn[i].y - texPosCero.y;
		avanceEscenaEnX					= NBPUNTO_PRODUCTO_VECTORIAL(texVecUnitCreceYD.x, texVecUnitCreceYD.y, vectorCoordenada.x, vectorCoordenada.y);
		avanceEscenaEnY					= -(NBPUNTO_PRODUCTO_VECTORIAL(texVecUnitCreceXD.x, texVecUnitCreceXD.y, vectorCoordenada.x, vectorCoordenada.y));
		guardarCoordenadasEn[i].tex.x	= cajaRelTexeles.xMin + (avanceRelX * (avanceEscenaEnX / texMaxCrecimientoXD));
		guardarCoordenadasEn[i].tex.y	= cajaRelTexeles.yMin + (avanceRelY * (avanceEscenaEnY / texMaxCrecimientoYD));
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUTextura::cargarCoordenadasTextura(NBVerticeGL* guardarCoordenadasEn, UI16 conteoCoordenadas, const NBPunto &texPosCero, const NBPunto &texVecUnitCreceX, const NBPunto &texVecUnitCreceY, const float texMaxCrecimientoX, const float texMaxCrecimientoY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasTextura")
	// Formato de Coordenadas escena esperadas
	//
	// PosCero --------> CreceXEscena
	// |
	// |
	// _
	// CreceYEscenas
	//
	NBPuntoP<double> texVecUnitCreceXD;	NBPUNTO_ESTABLECER(texVecUnitCreceXD, texVecUnitCreceX.x, texVecUnitCreceX.y)
	NBPuntoP<double> texVecUnitCreceYD;	NBPUNTO_ESTABLECER(texVecUnitCreceYD, texVecUnitCreceY.x, texVecUnitCreceY.y)
	const double texMaxCrecimientoXD = texMaxCrecimientoX;
	const double texMaxCrecimientoYD = texMaxCrecimientoY;
	//
	const double avanceRelX	= _cajaRelTexeles.xMax - _cajaRelTexeles.xMin;
	const double avanceRelY	= _cajaRelTexeles.yMax - _cajaRelTexeles.yMin;
	NBPuntoP<double> vectorCoordenadaD; double avanceEscenaEnX, avanceEscenaEnY;
	UI16 i; const UI16 conteo = conteoCoordenadas;
	for(i = 0; i < conteo; i++){
		vectorCoordenadaD.x				= guardarCoordenadasEn[i].x - texPosCero.x;
		vectorCoordenadaD.y				= guardarCoordenadasEn[i].y - texPosCero.y;
		avanceEscenaEnX					= NBPUNTO_PRODUCTO_VECTORIAL(texVecUnitCreceYD.x, texVecUnitCreceYD.y, vectorCoordenadaD.x, vectorCoordenadaD.y);
		avanceEscenaEnY					= -(NBPUNTO_PRODUCTO_VECTORIAL(texVecUnitCreceXD.x, texVecUnitCreceXD.y, vectorCoordenadaD.x, vectorCoordenadaD.y));
		guardarCoordenadasEn[i].tex.x	= _cajaRelTexeles.xMin + (avanceRelX * (avanceEscenaEnX / texMaxCrecimientoXD));
		guardarCoordenadasEn[i].tex.y	= _cajaRelTexeles.yMin + (avanceRelY * (avanceEscenaEnY / texMaxCrecimientoYD));
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUTextura::cargarCoordenadasTextura(NBVerticeGL* guardarCoordenadasEn, ENTexturaModoCoordenadas modoCoordenadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasTextura")
	if(modoCoordenadas == ENTexturaModoCoordenadas_Antihorario){
		guardarCoordenadasEn[0].tex.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex.y = _cajaRelTexeles.yMin;
		guardarCoordenadasEn[1].tex.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex.y = _cajaRelTexeles.yMin;
		guardarCoordenadasEn[2].tex.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[2].tex.y = _cajaRelTexeles.yMax;
		guardarCoordenadasEn[3].tex.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[3].tex.y = _cajaRelTexeles.yMax;
	} else {
		//entrelazadas
		guardarCoordenadasEn[0].tex.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex.y = _cajaRelTexeles.yMin;
		guardarCoordenadasEn[1].tex.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex.y = _cajaRelTexeles.yMin;
		guardarCoordenadasEn[2].tex.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[2].tex.y = _cajaRelTexeles.yMax;
		guardarCoordenadasEn[3].tex.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[3].tex.y = _cajaRelTexeles.yMax;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//NOTA IMPORTANTE: OpenGL mapea las coordenadas relativas en las texturas a partir de los centros de los texeles.
//Es decir: la coordenada(0,0) equivale al centro del primer texel, y la coordenada(1,1) equivale al centro del ultimo texel.
//Cuando se intenta hacer referencia a los bordes de los texeles, se pueden llegar a pintar los texeles contiguos a la textura.
void AUTextura::cargarCoordenadasTextura(NBVerticeGL* guardarCoordenadasEn, float relXMin, float relXMax, float relYMin, float relYMax, ENTexturaModoCoordenadas modoCoordenadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasTexturaRel")
	//NBASSERT(relXMin>=0.0f && relXMin<=1.0f)
	//NBASSERT(relXMax>=0.0f && relXMax<=1.0f)
	//NBASSERT(relYMin>=0.0f && relYMin<=1.0f)
	//NBASSERT(relYMax>=0.0f && relYMax<=1.0f)
	NBASSERT(relXMin <= relXMax)
	NBASSERT(relYMin <= relYMax)
	//
	const double ancho		= (_cajaRelTexeles.xMax - _cajaRelTexeles.xMin);
	const double alto		= (_cajaRelTexeles.yMax - _cajaRelTexeles.yMin);
	NBCajaAABBP<double> cajaRelTexeles = _cajaRelTexeles;
	cajaRelTexeles.xMin		+= (ancho * (double)relXMin);
	cajaRelTexeles.yMin		+= (alto * (double)relYMin);
	cajaRelTexeles.xMax		-= (ancho * (1.0 - (double)relXMax));
	cajaRelTexeles.yMax		-= (alto * (1.0 - (double)relYMax));
	//
	if(modoCoordenadas == ENTexturaModoCoordenadas_Antihorario){
		guardarCoordenadasEn[0].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex.y = cajaRelTexeles.yMin;
		guardarCoordenadasEn[1].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex.y = cajaRelTexeles.yMin;
		guardarCoordenadasEn[2].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[2].tex.y = cajaRelTexeles.yMax;
		guardarCoordenadasEn[3].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[3].tex.y = cajaRelTexeles.yMax;
	} else {
		//entrelazadas
		guardarCoordenadasEn[0].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex.y = cajaRelTexeles.yMin;
		guardarCoordenadasEn[1].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex.y = cajaRelTexeles.yMin;
		guardarCoordenadasEn[2].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[2].tex.y = cajaRelTexeles.yMax;
		guardarCoordenadasEn[3].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[3].tex.y = cajaRelTexeles.yMax;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//NOTA IMPORTANTE: OpenGL mapea las coordenadas relativas en las texturas a partir de los bordes de los mapas de bits,
//pero, las coordenadas relativas que se envian al motorGL deben apuntar al centro de cada texel. Es decir evitar usar los (0,0) y (1,1).
//Esto evita que se pinten los texeles continuos de cada texturas en el atlas.
void AUTextura::cargarCoordenadasTextura2(NBVerticeGL2* guardarCoordenadasEn, SI32 unidadTextura, const ENTexturaModoCoordenadas modoCoordenadas, const ENTexturaOrdenV ordenFilasImagen, const UI16 anchoAtlas, const UI16 altoAtlas, const UI16 posicionXEnAtlas, const UI16 posicionYEnAtlas, const UI16 anchoEnAtlas, const UI16 altoEnAtlas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasTextura2")
	const NBCajaAABBP<double> cajaRelTexeles = AUTextura::cajaRelativaTexeles(anchoAtlas, altoAtlas, posicionXEnAtlas, posicionYEnAtlas, anchoEnAtlas, altoEnAtlas);
	if(unidadTextura==0){
		if(modoCoordenadas == ENTexturaModoCoordenadas_Antihorario){
			guardarCoordenadasEn[0].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[2].tex.y = cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[3].tex.y = cajaRelTexeles.yMax;
		} else {
			//entrelazadas
			guardarCoordenadasEn[0].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[2].tex.y = cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[3].tex.y = cajaRelTexeles.yMax;
		}
	} else {
		NBASSERT(unidadTextura==1)
		if(modoCoordenadas == ENTexturaModoCoordenadas_Antihorario){
			guardarCoordenadasEn[0].tex2.x = cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex2.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex2.x = cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex2.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex2.x = cajaRelTexeles.xMax; guardarCoordenadasEn[2].tex2.y = cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex2.x = cajaRelTexeles.xMin; guardarCoordenadasEn[3].tex2.y = cajaRelTexeles.yMax;
		} else {
			//entrelazadas
			guardarCoordenadasEn[0].tex2.x = cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex2.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex2.x = cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex2.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex2.x = cajaRelTexeles.xMin; guardarCoordenadasEn[2].tex2.y = cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex2.x = cajaRelTexeles.xMax; guardarCoordenadasEn[3].tex2.y = cajaRelTexeles.yMax;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUTextura::cargarCoordenadasTextura2(NBVerticeGL2* guardarCoordenadasEn, SI32 unidadTextura, ENTexturaModoCoordenadas modoCoordenadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasTextura2")
	if(unidadTextura==0){
		if(modoCoordenadas == ENTexturaModoCoordenadas_Antihorario){
			guardarCoordenadasEn[0].tex.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[2].tex.y = _cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[3].tex.y = _cajaRelTexeles.yMax;
		} else {
			//entrelazadas
			guardarCoordenadasEn[0].tex.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[2].tex.y = _cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[3].tex.y = _cajaRelTexeles.yMax;
		}
	} else {
		NBASSERT(unidadTextura==1)
		if(modoCoordenadas == ENTexturaModoCoordenadas_Antihorario){
			guardarCoordenadasEn[0].tex2.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex2.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex2.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex2.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex2.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[2].tex2.y = _cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex2.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[3].tex2.y = _cajaRelTexeles.yMax;
		} else {
			//entrelazadas
			guardarCoordenadasEn[0].tex2.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex2.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex2.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex2.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex2.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[2].tex2.y = _cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex2.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[3].tex2.y = _cajaRelTexeles.yMax;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//NOTA IMPORTANTE: OpenGL mapea las coordenadas relativas en las texturas a partir de los centros de los texeles.
//Es decir: la coordenada(0,0) equivale al centro del primer texel, y la coordenada(1,1) equivale al centro del ultimo texel.
//Cuando se intenta hacer referencia a los bordes de los texeles, se pueden llegar a pintar los texeles contiguos a la textura.
void AUTextura::cargarCoordenadasTextura2(NBVerticeGL2* guardarCoordenadasEn, float relXMin, float relXMax, float relYMin, float relYMax, SI32 unidadTextura, ENTexturaModoCoordenadas modoCoordenadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasTexturaRel2")
	if(relXMin<0.0f) relXMin = 0.0f;
	if(relYMin<0.0f) relYMin = 0.0f;
	if(relXMax>1.0f) relXMax = 1.0f;
	if(relYMax>1.0f) relYMax = 1.0f;
	//
	const double ancho		= (_cajaRelTexeles.xMax - _cajaRelTexeles.xMin);
	const double alto		= (_cajaRelTexeles.yMax - _cajaRelTexeles.yMin);
	NBCajaAABBP<double> cajaRelTexeles = _cajaRelTexeles;
	cajaRelTexeles.xMin		+= (ancho * (double)relXMin);
	cajaRelTexeles.yMin		+= (alto * (double)relYMin);
	cajaRelTexeles.xMax		-= (ancho * (1.0 - (double)relXMax));
	cajaRelTexeles.yMax		-= (alto * (1.0 - (double)relYMax));
	//
	if(unidadTextura==0){
		if(modoCoordenadas == ENTexturaModoCoordenadas_Antihorario){
			guardarCoordenadasEn[0].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[2].tex.y = cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[3].tex.y = cajaRelTexeles.yMax;
		} else {
			//entrelazadas
			guardarCoordenadasEn[0].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex.x = cajaRelTexeles.xMin; guardarCoordenadasEn[2].tex.y = cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex.x = cajaRelTexeles.xMax; guardarCoordenadasEn[3].tex.y = cajaRelTexeles.yMax;
		}
	} else {
		NBASSERT(unidadTextura==1)
		if(modoCoordenadas == ENTexturaModoCoordenadas_Antihorario){
			guardarCoordenadasEn[0].tex2.x = cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex2.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex2.x = cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex2.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex2.x = cajaRelTexeles.xMax; guardarCoordenadasEn[2].tex2.y = cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex2.x = cajaRelTexeles.xMin; guardarCoordenadasEn[3].tex2.y = cajaRelTexeles.yMax;
		} else {
			//entrelazadas
			guardarCoordenadasEn[0].tex2.x = cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex2.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex2.x = cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex2.y = cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex2.x = cajaRelTexeles.xMin; guardarCoordenadasEn[2].tex2.y = cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex2.x = cajaRelTexeles.xMax; guardarCoordenadasEn[3].tex2.y = cajaRelTexeles.yMax;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUTextura::cargarCoordenadasTextura3(NBVerticeGL3* guardarCoordenadasEn, SI32 unidadTextura, ENTexturaModoCoordenadas modoCoordenadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasTextura3")
	if(unidadTextura==0){
		if(modoCoordenadas == ENTexturaModoCoordenadas_Antihorario){
			guardarCoordenadasEn[0].tex.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[2].tex.y = _cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[3].tex.y = _cajaRelTexeles.yMax;
		} else {
			//entrelazadas
			guardarCoordenadasEn[0].tex.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[2].tex.y = _cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[3].tex.y = _cajaRelTexeles.yMax;
		}
	} else if(unidadTextura==1){
		if(modoCoordenadas == ENTexturaModoCoordenadas_Antihorario){
			guardarCoordenadasEn[0].tex2.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex2.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex2.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex2.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex2.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[2].tex2.y = _cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex2.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[3].tex2.y = _cajaRelTexeles.yMax;
		} else {
			//entrelazadas
			guardarCoordenadasEn[0].tex2.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex2.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex2.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex2.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex2.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[2].tex2.y = _cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex2.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[3].tex2.y = _cajaRelTexeles.yMax;
		}
	} else {
		NBASSERT(unidadTextura==2)
		if(modoCoordenadas == ENTexturaModoCoordenadas_Antihorario){
			guardarCoordenadasEn[0].tex3.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex3.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex3.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex3.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex3.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[2].tex3.y = _cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex3.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[3].tex3.y = _cajaRelTexeles.yMax;
		} else {
			//entrelazadas
			guardarCoordenadasEn[0].tex3.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[0].tex3.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[1].tex3.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[1].tex3.y = _cajaRelTexeles.yMin;
			guardarCoordenadasEn[2].tex3.x = _cajaRelTexeles.xMin; guardarCoordenadasEn[2].tex3.y = _cajaRelTexeles.yMax;
			guardarCoordenadasEn[3].tex3.x = _cajaRelTexeles.xMax; guardarCoordenadasEn[3].tex3.y = _cajaRelTexeles.yMax;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

// Texturas patron

void AUTextura::cargarCoordenadasPatron(const NBVerticeGL* coordenadasVertices, int conteoVertices, NBVerticeGL* guardarCoordenadasEn/*, const NBMatriz &matrizTransformaciones*/){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasPatron")
	const NBTamano tamTexHD = tamanoHD();
	SI32 iVertice;
	for(iVertice=0; iVertice<conteoVertices; iVertice++){
		//NBPunto punto;
		//NBMATRIZ_MULTIPLICAR_PUNTO(punto, matrizTransformaciones, coordenadasVertices[iVertice].x, coordenadasVertices[iVertice].y);
		//guardarCoordenadasEn[iVertice].tex.x = punto.x;
		//guardarCoordenadasEn[iVertice].tex.y = punto.y;
		guardarCoordenadasEn[iVertice].tex.x = coordenadasVertices[iVertice].x * (1.0f / tamTexHD.ancho);
		guardarCoordenadasEn[iVertice].tex.y = coordenadasVertices[iVertice].y * (1.0f / tamTexHD.alto);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUTextura::cargarCoordenadasPatron(const NBPunto* coordenadasVertices, int conteoVertices, NBVerticeGL* guardarCoordenadasEn/*, const NBMatriz &matrizTransformaciones*/){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasPatron")
	const NBTamano tamTexHD = tamanoHD();
	SI32 iVertice;
	for(iVertice=0; iVertice<conteoVertices; iVertice++){
		//NBPunto punto;
		//NBMATRIZ_MULTIPLICAR_PUNTO(punto, matrizTransformaciones, coordenadasVertices[iVertice].x, coordenadasVertices[iVertice].y);
		//guardarCoordenadasEn[iVertice].tex.x = punto.x;
		//guardarCoordenadasEn[iVertice].tex.y = punto.y;
		guardarCoordenadasEn[iVertice].tex.x = coordenadasVertices[iVertice].x * (1.0f / tamTexHD.ancho);
		guardarCoordenadasEn[iVertice].tex.y = coordenadasVertices[iVertice].y * (1.0f / tamTexHD.alto);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUTextura::cargarCoordenadasPatron(const NBPuntoColor* coordenadasVertices, int conteoVertices, NBVerticeGL* guardarCoordenadasEn/*, const NBMatriz &matrizTransformaciones*/){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasPatron")
	const NBTamano tamTexHD = tamanoHD();
	SI32 iVertice;
	for(iVertice=0; iVertice<conteoVertices; iVertice++){
		//NBPunto punto;
		//NBMATRIZ_MULTIPLICAR_PUNTO(punto, matrizTransformaciones, coordenadasVertices[iVertice].x, coordenadasVertices[iVertice].y);
		//guardarCoordenadasEn[iVertice].tex.x = punto.x;
		//guardarCoordenadasEn[iVertice].tex.y = punto.y;
		guardarCoordenadasEn[iVertice].tex.x = coordenadasVertices[iVertice].coordenada.x * (1.0f / tamTexHD.ancho);
		guardarCoordenadasEn[iVertice].tex.y = coordenadasVertices[iVertice].coordenada.y * (1.0f / tamTexHD.alto);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

// Texturas patron, con traslado adicional en las coordenadas

void AUTextura::cargarCoordenadasPatron(const NBVerticeGL* coordenadasVertices, int conteoVertices, NBVerticeGL* guardarCoordenadasEn, const NBPunto &traslado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasPatron")
	const NBTamano tamTexHD = tamanoHD();
	SI32 iVertice;
	for(iVertice=0; iVertice<conteoVertices; iVertice++){
		guardarCoordenadasEn[iVertice].tex.x = (coordenadasVertices[iVertice].x+traslado.x) * (1.0f / tamTexHD.ancho);
		guardarCoordenadasEn[iVertice].tex.y = (coordenadasVertices[iVertice].y+traslado.y) * (1.0f / tamTexHD.alto);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUTextura::cargarCoordenadasPatron(const NBPunto* coordenadasVertices, int conteoVertices, NBVerticeGL* guardarCoordenadasEn, const NBPunto &traslado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasPatron")
	const NBTamano tamTexHD = tamanoHD();
	SI32 iVertice;
	for(iVertice=0; iVertice<conteoVertices; iVertice++){
		guardarCoordenadasEn[iVertice].tex.x = (coordenadasVertices[iVertice].x+traslado.x)*(1.0f/tamTexHD.ancho);
		guardarCoordenadasEn[iVertice].tex.y = (coordenadasVertices[iVertice].y+traslado.y)*(1.0f/tamTexHD.alto);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUTextura::cargarCoordenadasPatron(const NBPuntoColor* coordenadasVertices, int conteoVertices, NBVerticeGL* guardarCoordenadasEn, const NBPunto &traslado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasPatron")
	const NBTamano tamTexHD = tamanoHD();
	SI32 iVertice;
	for(iVertice=0; iVertice<conteoVertices; iVertice++){
		guardarCoordenadasEn[iVertice].tex.x = (coordenadasVertices[iVertice].coordenada.x+traslado.x)*(1.0f/tamTexHD.ancho);
		guardarCoordenadasEn[iVertice].tex.y = (coordenadasVertices[iVertice].coordenada.y+traslado.y)*(1.0f/tamTexHD.alto);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

void AUTextura::cargarCoordenadasPatron2(const NBVerticeGL2* coordenadasVertices, int conteoVertices, NBVerticeGL2* guardarCoordenadasEn, SI32 unidadTextura, const NBMatriz &matrizTransformaciones){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasPatron2")
	SI32 iVertice;
	if(unidadTextura==0){
		for(iVertice=0; iVertice<conteoVertices; iVertice++){
			NBPunto punto;
			NBMATRIZ_MULTIPLICAR_PUNTO(punto, matrizTransformaciones, coordenadasVertices[iVertice].x, coordenadasVertices[iVertice].y);
			guardarCoordenadasEn[iVertice].tex.x = punto.x;
			guardarCoordenadasEn[iVertice].tex.y = punto.y;
		}
	} else {
		for(iVertice=0; iVertice<conteoVertices; iVertice++){
			NBPunto punto;
			NBMATRIZ_MULTIPLICAR_PUNTO(punto, matrizTransformaciones, coordenadasVertices[iVertice].x, coordenadasVertices[iVertice].y);
			guardarCoordenadasEn[iVertice].tex2.x = punto.x;
			guardarCoordenadasEn[iVertice].tex2.y = punto.y;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUTextura::cargarCoordenadasPatron2(const NBPunto* coordenadasVertices, int conteoVertices, NBVerticeGL2* guardarCoordenadasEn, SI32 unidadTextura, const NBMatriz &matrizTransformaciones){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUTextura::cargarCoordenadasPatron2")
	int iVertice;
	if(unidadTextura==0){
		for(iVertice=0; iVertice<conteoVertices; iVertice++){
			NBPunto punto;
			NBMATRIZ_MULTIPLICAR_PUNTO(punto, matrizTransformaciones, coordenadasVertices[iVertice].x, coordenadasVertices[iVertice].y);
			guardarCoordenadasEn[iVertice].tex.x = punto.x;
			guardarCoordenadasEn[iVertice].tex.y = punto.y;
		}
	} else {
		for(iVertice=0; iVertice<conteoVertices; iVertice++){
			NBPunto punto;
			NBMATRIZ_MULTIPLICAR_PUNTO(punto, matrizTransformaciones, coordenadasVertices[iVertice].x, coordenadasVertices[iVertice].y);
			guardarCoordenadasEn[iVertice].tex2.x = punto.x;
			guardarCoordenadasEn[iVertice].tex2.y = punto.y;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUTextura)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUTextura, "AUTextura")
AUOBJMETODOS_CLONAR_NULL(AUTextura)
//OJO: debe evitarse el clonado de objetos AUTexturas.
//Los AUTexturas deberians er creados por sus administardores (como el NBgestorTexturas y el AUFuenteTextura),
//y se retenidos y liberados por los consumidores (sin copiarlas o crearlas);






