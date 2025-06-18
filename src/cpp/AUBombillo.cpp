//
//  AUBombillo.cpp
//  DodoTownNucleo
//
//  Created by Marcos Ortega on 22/09/12.
//
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUBombillo.h"

AUBombillo::AUBombillo() : AUNodoElectroCarga() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::AUBombillo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUBombillo")
	_tipoBombillo				= ENBombilloTipo_Conteo;
	_bombilloActivo				= true;
	_bombilloCargado			= false;
	_desactivarDespuesDeCargar	= false;
	_desactivarDespuesDeDescargar	= false;
	_cargaActualRecibida		= 0.0f;
	_cargaParaConsiderarCargada	= 0.75f;
	_alcanceMaximaLuz			= 64.0f;
	_radianesActOscilacionBrillo = 0.0f;
	//
	_escalaMinBrillo.ancho		= 0.01f;
	_escalaMinBrillo.alto		= 0.01f;
	_escalaCreceMaxBrillo.ancho	= 1.0f;
	_escalaCreceMaxBrillo.alto	= 1.0f;
	//
	_presentaBrillo				= new(this) AUEscenaSprite();
	_luzRadial					= new(this) AUEscenaLuzRadial(0.0f, false);
	_presentaLampara			= new(this) AUEscenaSprite();
	_presentaBulbo				= NULL;
	this->agregarObjetoEscena(_presentaBrillo); _presentaBrillo->liberar(NB_RETENEDOR_THIS);
	this->agregarObjetoEscena(_presentaLampara); _presentaLampara->liberar(NB_RETENEDOR_THIS);
	this->agregarObjetoEscena(_luzRadial); _luzRadial->liberar(NB_RETENEDOR_THIS);
	this->establecerBombilloTipo(ENBombilloTipo_Incandecente);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUBombillo::AUBombillo(AUBombillo* otraInstancia) : AUNodoElectroCarga(otraInstancia) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::AUBombillo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUBombillo")
	_tipoBombillo				= otraInstancia->_tipoBombillo;
	_bombilloActivo				= otraInstancia->_bombilloActivo;
	_bombilloCargado			= otraInstancia->_bombilloCargado;
	_desactivarDespuesDeCargar	= otraInstancia->_desactivarDespuesDeCargar;
	_desactivarDespuesDeDescargar	= otraInstancia->_desactivarDespuesDeDescargar;
	_cargaActualRecibida		= 0.0f;
	_cargaParaConsiderarCargada	= otraInstancia->_cargaParaConsiderarCargada;
	_alcanceMaximaLuz			= otraInstancia->_alcanceMaximaLuz;
	_radianesActOscilacionBrillo = 0.0f;
	//
	_escalaMinBrillo			= otraInstancia->_escalaMinBrillo;
	_escalaCreceMaxBrillo		= otraInstancia->_escalaCreceMaxBrillo;
	//
	_presentaBrillo				= new(this) AUEscenaSprite(otraInstancia->_presentaBrillo);
	_presentaLampara			= new(this) AUEscenaSprite(otraInstancia->_presentaLampara);
	_luzRadial					= new(this) AUEscenaLuzRadial(otraInstancia->_luzRadial);
	_presentaBulbo				= NULL;
	this->agregarObjetoEscena(_presentaBrillo); _presentaBrillo->liberar(NB_RETENEDOR_THIS);
	this->agregarObjetoEscena(_presentaLampara); _presentaLampara->liberar(NB_RETENEDOR_THIS);
	this->agregarObjetoEscena(_luzRadial); _luzRadial->liberar(NB_RETENEDOR_THIS);
	//
	if(otraInstancia->_presentaBulbo != NULL){
		_presentaBulbo			= new(this) AUEscenaSprite(otraInstancia->_presentaBulbo);
		this->agregarObjetoEscena(_presentaBulbo); _presentaBulbo->liberar(NB_RETENEDOR_THIS);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUBombillo::~AUBombillo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::~AUBombillo")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

ENBombilloTipo AUBombillo::bombilloTipo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::bombilloTipo")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return (ENBombilloTipo)_tipoBombillo;
}

bool AUBombillo::bombilloActivo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::bombilloActivo")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _bombilloActivo;
}

bool AUBombillo::bombilloCargado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::bombilloCargado")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _bombilloCargado;
	
}

bool AUBombillo::desactivarDespuesDeCargar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::desactivarDespuesDeCargar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _desactivarDespuesDeCargar;
}

bool AUBombillo::desactivarDespuesDeDescargar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::desactivarDespuesDeDescargar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _desactivarDespuesDeDescargar;
}

float AUBombillo::cargaParaConsiderarCargada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::cargaParaConsiderarCargada")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _cargaParaConsiderarCargada;
}

float AUBombillo::alcanceMaximaLuz(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::alcanceMaximaLuz")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _alcanceMaximaLuz;
}

//

void AUBombillo::establecerBombilloTipo(ENBombilloTipo tipoBombillo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::establecerBombilloTipo")
	if(_tipoBombillo!=tipoBombillo){
		if(tipoBombillo==ENBombilloTipo_Incandecente){
			AUTextura* texturaLampara	= NBGestorTexturas::texturaDesdeArchivo("Escenarios/Electricos/guiBombilloFoco.png"); NBASSERT(texturaLampara != NULL)
			AUTextura* texturaBrillo	= NBGestorTexturas::texturaDesdeArchivo("Escenarios/Electricos/objBrilloRadial.png"); NBASSERT(texturaBrillo != NULL)
			_presentaLampara->redimensionar(texturaLampara);
			_presentaLampara->establecerTextura(texturaLampara);
			_presentaLampara->establecerTraslacion(8.0f, 0.0f);
			_presentaBrillo->redimensionar(texturaBrillo);
			_presentaBrillo->establecerTextura(texturaBrillo);
			_presentaBrillo->establecerTraslacion(18.0f, 0.0f);
			_presentaBrillo->establecerMultiplicadorColor8(255, 255, 100, 155);
			_presentaBrillo->establecerIluminoDependencia(false);
			_luzRadial->establecerMultiplicadorColor8(255, 255, 100, 155);
			_luzRadial->establecerAngulosMinMaxActivos(false);
			if(_presentaBulbo != NULL) this->quitarObjetoEscena(_presentaBulbo); _presentaBulbo = NULL;
			_escalaMinBrillo.ancho		= 0.01f;
			_escalaMinBrillo.alto		= _escalaMinBrillo.ancho;
			_escalaCreceMaxBrillo.ancho	= 0.99f;
			_escalaCreceMaxBrillo.alto	= _escalaCreceMaxBrillo.ancho;
		} else if(tipoBombillo==ENBombilloTipo_IncandCono){
			AUTextura* texturaLampara	= NBGestorTexturas::texturaDesdeArchivo("Escenarios/Electricos/objLamparaConica01.png"); NBASSERT(texturaLampara != NULL)
			AUTextura* texturaBrillo	= NBGestorTexturas::texturaDesdeArchivo("Escenarios/Electricos/objLamparaConica02.png"); NBASSERT(texturaBrillo != NULL)
			_presentaLampara->redimensionar(texturaLampara, ENSpriteAlineaH_Der, ENSpriteAlineaV_Cen);
			_presentaLampara->establecerTextura(texturaLampara);
			_presentaLampara->establecerTraslacion(-6.0f, 0.0f);
			_presentaBrillo->redimensionar(texturaBrillo, ENSpriteAlineaH_Der, ENSpriteAlineaV_Cen);
			_presentaBrillo->establecerTextura(texturaBrillo);
			_presentaBrillo->establecerTraslacion(-6.0f, 0.0f);
			_presentaBrillo->establecerMultiplicadorColor8(255, 255, 100, 155);
			_presentaBrillo->establecerIluminoDependencia(false);
			_luzRadial->establecerMultiplicadorColor8(255, 255, 100, 155);
			_luzRadial->establecerRadianesMinimo(PI*-0.25f);
			_luzRadial->establecerRadianesMaximo(PI*0.25f);
			_luzRadial->establecerAngulosMinMaxActivos(true);
			if(_presentaBulbo != NULL) this->quitarObjetoEscena(_presentaBulbo); _presentaBulbo = NULL;
			_escalaMinBrillo.ancho		= 0.01f;
			_escalaMinBrillo.alto		= _escalaMinBrillo.ancho;
			_escalaCreceMaxBrillo.ancho	= 1.19f;
			_escalaCreceMaxBrillo.alto	= _escalaCreceMaxBrillo.ancho;
		} else if(tipoBombillo==ENBombilloTipo_Fluorescente){
			AUTextura* texturaLampara	= NBGestorTexturas::texturaDesdeArchivo("Escenarios/Electricos/objLamparaTubo01.png"); NBASSERT(texturaLampara != NULL)
			AUTextura* texturaBrillo	= NBGestorTexturas::texturaDesdeArchivo("Escenarios/Electricos/objBrilloRadialSemi.png"); NBASSERT(texturaBrillo != NULL)
			_presentaLampara->redimensionar(texturaLampara);
			_presentaLampara->establecerTextura(texturaLampara);
			_presentaLampara->establecerTraslacion(8.0f, 0.0f);
			_presentaBrillo->redimensionar(texturaBrillo, ENSpriteAlineaH_Der, ENSpriteAlineaV_Cen);
			_presentaBrillo->establecerTextura(texturaBrillo);
			_presentaBrillo->establecerTraslacion(8.0f, 0.0f);
			_presentaBrillo->establecerMultiplicadorColor8(255, 255, 255, 155);
			_presentaBrillo->establecerIluminoDependencia(false);
			_luzRadial->establecerMultiplicadorColor8(255, 255, 255, 155);
			_luzRadial->establecerRadianesMinimo(PI*-0.49f);
			_luzRadial->establecerRadianesMaximo(PI*0.49f);
			_luzRadial->establecerAngulosMinMaxActivos(true);
			if(_presentaBulbo == NULL){ _presentaBulbo = new(this) AUEscenaSprite(); this->agregarObjetoEscena(_presentaBulbo); _presentaBulbo->liberar(NB_RETENEDOR_THIS); }
			AUTextura* texturaBulbo	= NBGestorTexturas::texturaDesdeArchivo("Escenarios/Electricos/objLamparaTubo02.png"); NBASSERT(texturaBulbo != NULL)
			_presentaBulbo->redimensionar(texturaBulbo);
			_presentaBulbo->establecerTextura(texturaBulbo);
			_presentaBulbo->establecerTraslacion(_presentaLampara->traslacion());
			_escalaMinBrillo.ancho		= 0.10f;
			_escalaMinBrillo.alto		= 2.1f;
			_escalaCreceMaxBrillo.ancho	= 1.99f;
			_escalaCreceMaxBrillo.alto	= 0.0f;
		}
		_tipoBombillo = tipoBombillo;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUBombillo::establecerBombilloActivo(bool bombilloActivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::establecerBombilloActivo")
	_bombilloActivo = bombilloActivo;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUBombillo::establecerDesactivarDespuesDeCargar(bool desactivarDespues){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::establecerDesactivarDespuesDeCargar")
	_desactivarDespuesDeCargar = desactivarDespues;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUBombillo::establecerDesactivarDespuesDeDescargar(bool desactivarDespues){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::establecerDesactivarDespuesDeDescargar")
	_desactivarDespuesDeDescargar = desactivarDespues;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUBombillo::establecerCargaParaConsiderarCargada(float cargaRel){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::establecerCargaParaConsiderarCargada")
	_cargaParaConsiderarCargada = cargaRel;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUBombillo::establecerAlcanceMaximaLuz(float alcanceMaxima){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::establecerAlcanceMaximaLuz")
	_alcanceMaximaLuz = alcanceMaxima;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

NBCajaAABB AUBombillo::cajaEscenaPresentacion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::cajaEscenaPresentacion")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _presentaLampara->cajaAABBEnEscena();
}

float AUBombillo::electrocargaVirtualParaDestinos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::electrocargaVirtualParaDestinos")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _cargaActualRecibida;
}

float AUBombillo::electrocargaRealParaDestinos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::electrocargaRealParaDestinos")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _cargaActualRecibida;
}

bool AUBombillo::electrocargaEsSoloConsumidor(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::electrocargaEsSoloConsumidor")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return true;
}

void AUBombillo::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBombillo::tickAnimacion")
	AUNodoElectroCarga::tickAnimacion(segsTranscurridos);
	if(!this->_dormido && _bombilloActivo){
		if(_tipoBombillo==ENBombilloTipo_IncandCono || _tipoBombillo==ENBombilloTipo_Incandecente){
			_cargaActualRecibida	= this->electrocargaRealTotalRecibida();
		} else {
			_cargaActualRecibida	= this->electrocargaVirtualTotalRecibida();
		}
		float relCarga				= 1.0f; if(_cargaParaConsiderarCargada!=0.0f) relCarga = _cargaActualRecibida / _cargaParaConsiderarCargada; if(relCarga>1.0f) relCarga = 1.0f;
		if(relCarga==1.0f && !_bombilloCargado){
			_bombilloCargado = true;
			PRINTF_INFO("Bombillo cargado\n");
		} else if(relCarga==0.0f && _bombilloCargado){
			_bombilloCargado = false;
			PRINTF_INFO("Bombillo descargado\n");
		}
		_luzRadial->establecerAlcanceLuz(relCarga*_alcanceMaximaLuz);
		if(relCarga!=0.0f){
			_radianesActOscilacionBrillo += PIx2 * segsTranscurridos; if(_radianesActOscilacionBrillo>PIx2) _radianesActOscilacionBrillo -= PIx2;
			float relOscilacion			= 0.1f * sinf(_radianesActOscilacionBrillo);
			NBTamano escalaBrillo;
			escalaBrillo.ancho			= _escalaMinBrillo.ancho + (_escalaCreceMaxBrillo.ancho * relCarga);
			escalaBrillo.alto			= _escalaMinBrillo.alto + (_escalaCreceMaxBrillo.alto * relCarga);
			escalaBrillo.ancho			+= escalaBrillo.ancho * relOscilacion; //Aplicar un margen de +/- 10% de oscilacion de brillo
			escalaBrillo.alto			+= escalaBrillo.alto * relOscilacion; //Aplicar un margen de +/- 10% de oscilacion de brillo
			_presentaBrillo->establecerVisible(true);
			_presentaBrillo->establecerMultiplicadorAlpha8(55.0f + (100.0f * relCarga));
			_presentaBrillo->establecerEscalacion(escalaBrillo);
		} else {
			_presentaBrillo->establecerVisible(false);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool AUBombillo::agregarXmlInternoEn(AUBombillo* laser, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUBombillo::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	AUNodoElectroCarga::agregarXmlInternoEn(laser, guardarEn, espaciosBaseIzq, arregloObjetosParaID);
	guardarEn->agregarConFormato("%s<tip>%d</tip>\r\n", espaciosBaseIzq, laser->_tipoBombillo);
	guardarEn->agregarConFormato("%s<act>%d</act>\r\n", espaciosBaseIzq, laser->_bombilloActivo?1:0);
	guardarEn->agregarConFormato("%s<desAlEnc>%d</desAlEnc>\r\n", espaciosBaseIzq, laser->_desactivarDespuesDeCargar?1:0);
	guardarEn->agregarConFormato("%s<desAlApa>%d</desAlApa>\r\n", espaciosBaseIzq, laser->_desactivarDespuesDeDescargar?1:0);
	guardarEn->agregarConFormato("%s<crgMin>%f</crgMin>\r\n", espaciosBaseIzq, laser->_cargaParaConsiderarCargada);
	guardarEn->agregarConFormato("%s<alcMax>%f</alcMax>\r\n", espaciosBaseIzq, laser->_alcanceMaximaLuz);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUBombillo::interpretarXmlInterno(AUBombillo* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos, AUArregloNativoP<SI32>* arregloIDs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUBombillo::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	if(AUNodoElectroCarga::interpretarXmlInterno(cargarEn, datosXml, nodoXml, arregloObjetos, arregloIDs)){
		cargarEn->_bombilloActivo				= datosXml->nodoHijo("act", true, nodoXml);
		cargarEn->_desactivarDespuesDeCargar	= datosXml->nodoHijo("desAlEnc", false, nodoXml);
		cargarEn->_desactivarDespuesDeDescargar	= datosXml->nodoHijo("desAlApa", false, nodoXml);
		cargarEn->_cargaParaConsiderarCargada	= datosXml->nodoHijo("crgMin", 0.75f, nodoXml);
		cargarEn->_alcanceMaximaLuz				= datosXml->nodoHijo("alcMax", 64.0f, nodoXml);
		cargarEn->establecerBombilloTipo((ENBombilloTipo)datosXml->nodoHijo("tip", 0, nodoXml));
		exito = true;
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUBombillo::agregarBitsInternosEn(AUBombillo* laser, AUArchivo* guardarEn, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUBombillo::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUNodoElectroCarga::agregarBitsInternosEn(laser, guardarEn, arregloObjetosParaID)){
		SI32 tipoBombillo	= laser->_tipoBombillo;				guardarEn->escribir(&tipoBombillo, sizeof(tipoBombillo), 1, guardarEn);
		bool activo		= laser->_bombilloActivo;				guardarEn->escribir(&activo, sizeof(activo), 1, guardarEn);
		bool desactivar = laser->_desactivarDespuesDeCargar;	guardarEn->escribir(&desactivar, sizeof(desactivar), 1, guardarEn);
		bool desactivD	= laser->_desactivarDespuesDeDescargar;	guardarEn->escribir(&desactivD, sizeof(desactivD), 1, guardarEn);
		guardarEn->escribir(&laser->_cargaParaConsiderarCargada, sizeof(laser->_cargaParaConsiderarCargada), 1, guardarEn);
		guardarEn->escribir(&laser->_alcanceMaximaLuz, sizeof(laser->_alcanceMaximaLuz), 1, guardarEn);
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUBombillo::interpretarBitsInternos(AUBombillo* cargarEn, AUArchivo* cargarDe, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUBombillo::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUNodoElectroCarga::interpretarBitsInternos(cargarEn, cargarDe, arregloObjetosParaID)){
			SI32 tipoBombillo;	cargarDe->leer(&tipoBombillo, sizeof(tipoBombillo), 1, cargarDe);
			bool activo;		cargarDe->leer(&activo, sizeof(activo), 1, cargarDe);			cargarEn->_bombilloActivo				= activo;
			bool desactivar;	cargarDe->leer(&desactivar, sizeof(desactivar), 1, cargarDe);	cargarEn->_desactivarDespuesDeCargar	= desactivar;
			bool desactivarD;	cargarDe->leer(&desactivarD, sizeof(desactivarD), 1, cargarDe);	cargarEn->_desactivarDespuesDeDescargar	= desactivarD;
			cargarDe->leer(&cargarEn->_cargaParaConsiderarCargada, sizeof(cargarEn->_cargaParaConsiderarCargada), 1, cargarDe);
			cargarDe->leer(&cargarEn->_alcanceMaximaLuz, sizeof(cargarEn->_alcanceMaximaLuz), 1, cargarDe);
			cargarEn->establecerBombilloTipo((ENBombilloTipo)tipoBombillo);
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUBombillo, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUBombillo, AUNodoElectroCarga)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUBombillo, "AUBombillo", AUNodoElectroCarga)
AUOBJMETODOS_CLONAR_THIS(AUBombillo)






