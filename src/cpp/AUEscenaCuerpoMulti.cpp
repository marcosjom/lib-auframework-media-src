//
//  AUEscenaCuerpoMulti.cpp
//  Gameplay_iOS
//
//  Created by Marcos Ortega on 25/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaCuerpoMulti.h"

AUEscenaCuerpoMulti::AUEscenaCuerpoMulti() : AUEscenaCuerpoI(), _cuerposPorEstado(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::AUEscenaCuerpoMulti")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaCuerpoMulti")
	_vida						= 1.0f;
	_iCuerpoActual				= -1;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaCuerpoMulti::AUEscenaCuerpoMulti(AUEscenaCuerpoMulti* otroCuerpo) : AUEscenaCuerpoI(), _cuerposPorEstado(this, otroCuerpo->_cuerposPorEstado.conteo) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::AUEscenaCuerpoMulti")
	_vida							= otroCuerpo->_vida;
	_iCuerpoActual					= -1;
	_propsCreacionCuerpo			= otroCuerpo->_propsCreacionCuerpo;
	_propsCreacionCuerpo.datoUsuario = NULL;
	_propsMultiplicadores			= otroCuerpo->_propsMultiplicadores;
	//Copiar los cuerpos de estados
	SI32 iCpo;
	for(iCpo=0; iCpo<otroCuerpo->_cuerposPorEstado.conteo; iCpo++){
		AUEscenaCuerpo* copiaCuerpo = new(this) AUEscenaCuerpo((AUEscenaCuerpo*)otroCuerpo->_cuerposPorEstado.elemento[iCpo]);
		_cuerposPorEstado.agregarElemento(copiaCuerpo);
		copiaCuerpo->liberar(NB_RETENEDOR_THIS);
	}
	this->privCambiarCuerpoSiNecesario();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaCuerpoMulti::~AUEscenaCuerpoMulti(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::~AUEscenaCuerpoMulti")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

SI16 AUEscenaCuerpoMulti::indiceCuerpoActual(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::indiceCuerpoActual")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _iCuerpoActual;
}

void AUEscenaCuerpoMulti::privAgregarCuerpoEstado(AUEscenaCuerpo* cuerpoParaEstado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::privAgregarCuerpoEstado")
	NBASSERT(cuerpoParaEstado != NULL)
	_cuerposPorEstado.agregarElemento(cuerpoParaEstado);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

float AUEscenaCuerpoMulti::vida(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::vida")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _vida;
}

void AUEscenaCuerpoMulti::reducirVida(float reduccionPositiva){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::reducirVida")
	_vida -= reduccionPositiva;
	if(_vida<0.0f) _vida = 0.0f;
	if(_vida>1.0f) _vida = 1.0f;
	this->privCambiarCuerpoSiNecesario();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

float AUEscenaCuerpoMulti::reducirVida(float velImpacto, float retitucionImpacto, float masaOtroCuerpo, float multiplicadorDanoProduceOtroCuerpo, NBPunto miVelocidad, NBPunto velOtro, AUArregloNativoMutableP<NBParticula>* arregloCrearPiezaDesprendibleEn, AUEscenaContenedor* agregarEnCapa, AUArregloMutable* arregloSonidosRecicables){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::reduceVida")
	NBASSERT(velImpacto>=0.0f)
	NBASSERT(masaOtroCuerpo>=0.0f)
	float reduccionPositiva			= 0.0f;
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	NBPunto vectorImpacto;			NBPUNTO_ESTABLECER(vectorImpacto, velOtro.x-miVelocidad.x, velOtro.y-miVelocidad.y)
	b2Body* miCuerpo				= (b2Body*)this->cuerpo();	NBASSERT(miCuerpo != NULL)
	float masaMiCuerpo				= miCuerpo->GetMass();
	if(masaMiCuerpo>0.0f){			//Solo si tengo masa
		if(masaOtroCuerpo==0.0)		masaOtroCuerpo = masaMiCuerpo * 2.0f;
		float momentumOtroEnImpacto	= masaOtroCuerpo * velImpacto * multiplicadorDanoProduceOtroCuerpo * this->multiplicadorDanoRecibe();
		float momentumMaximoEste	= masaMiCuerpo * 1000.0f;
		float restitucionLimitada	= retitucionImpacto; if(restitucionLimitada<0.0f) restitucionLimitada = 0.0f; if(restitucionLimitada>1.0f) restitucionLimitada = 1.0f;
		reduccionPositiva			= (momentumOtroEnImpacto / momentumMaximoEste) * (1.0f-restitucionLimitada);
		//PRINTF_INFO("Reduccion de vida(%f) velImpacto(%f) miMasa(%f), masaOtro(%f) restitucion(%f) factorDanoProd(%f) factorDanoRecibe(%f)\n", reduccionPositiva, velImpacto, masaMiCuerpo, masaOtroCuerpo, retitucionImpacto, multiplicadorDanoProduceOtroCuerpo, this->multiplicadorDanoRecibe());
		NBASSERT(_vida==_vida) //Si falla, es NaN
		_vida						-= reduccionPositiva;
		if(_vida<0.0f) _vida = 0.0f;
		if(_vida>1.0f) _vida = 1.0f;
		//
		AUEscenaGrupoParticulas* agregarParticulasEn = NULL;
		if(this->idEscena!=-1){
			AUArreglo* agrupadoresParticulas = NBGestorEscena::agrupadoresParticulas(this->idEscena);
			if(agrupadoresParticulas->conteo!=0){
				//Determinar la capa en la cual agregar las particulas (la primera en la lista o bien la ultima de mi capa actual)
				agregarParticulasEn = (AUEscenaGrupoParticulas*)agrupadoresParticulas->elemento[0];
				SI32 iGrupoParts;
				for(iGrupoParts=1; iGrupoParts<agrupadoresParticulas->conteo; iGrupoParts++){
					if(((AUEscenaGrupoParticulas*)agrupadoresParticulas->elemento[iGrupoParts])->idCapa==this->idCapa){
						agregarParticulasEn = (AUEscenaGrupoParticulas*)agrupadoresParticulas->elemento[iGrupoParts];
					}
				}
			}
		}
		//
		bool debeActualzarseEstado = (_iCuerpoActual!=privIndicePresentacionSegunVida(_vida));
		if(_iCuerpoActual!=-1){
			if(velImpacto<0.0f) velImpacto = -velImpacto;
			AUEscenaCuerpo* cuerpoActual = (AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual];
			//NOTIFICAR IMPACTO A CUERPO (reproduce el sonido)
			cuerpoActual->procesarImpacto(velImpacto, _vida==0?ENCuerpoSonidoTipo_Destructivo:ENCuerpoSonidoTipo_Impacto, arregloSonidosRecicables);
			//CREAR PARTICULAS DE BASURA
			if(agregarParticulasEn != NULL && reduccionPositiva>0.01f){
				if(_vida>0.0f){
					//Generar particulas en zona de generacion
					generarParticulasAleatoriasEn(agregarParticulasEn, vectorImpacto, reduccionPositiva * 40);
				} else {
					//Generar todas las particulas
					generarParticulasTodasEn(agregarParticulasEn, vectorImpacto);
				}
			}
			//CREAR COPIA DE LAS PIEZAS DESPRENDIBLES
			if(arregloCrearPiezaDesprendibleEn != NULL && agregarEnCapa != NULL && debeActualzarseEstado && _iCuerpoActual!=-1){
				((AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual])->agregarParticulasObjetosDesprendiblesHaciaCapa(arregloCrearPiezaDesprendibleEn, agregarEnCapa, vectorImpacto);
			}
		}
		this->privCambiarCuerpoSiNecesario();
	}
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return reduccionPositiva;
}

void AUEscenaCuerpoMulti::generarParticulasAleatoriasEn(AUEscenaGrupoParticulas* agregarParticulasEn, const NBPunto &vectorImpacto, SI32 cantidadParticulas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::generarParticulasAleatoriasEn")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	AUEscenaCuerpo* cuerpoActual = (AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual];
	const STParticulaDef* defParticula;
	bool particulaGenerada;
	SI32 conteoGeneradas = 0, conteoParticulasGenerar = cantidadParticulas;
	do {
		defParticula = cuerpoActual->cualquierParticulaEnZonaGeneracion();
		if(defParticula != NULL){
			NBPunto posLocalPart	= defParticula->posicion;
			NBPunto posEscenaPart;	NBMATRIZ_MULTIPLICAR_PUNTO(posEscenaPart, _cacheObjEscena.matrizEscena, posLocalPart.x, posLocalPart.y);
			if(!privGenerarParticula(defParticula->textura, posEscenaPart, vectorImpacto, agregarParticulasEn)){
				particulaGenerada = false;
			} else {
				particulaGenerada = true;
				conteoGeneradas++;
			}
		}
	} while(defParticula != NULL && particulaGenerada && conteoGeneradas<conteoParticulasGenerar);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoMulti::generarParticulasTodasEn(AUEscenaGrupoParticulas* agregarParticulasEn, const NBPunto &vectorImpacto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::generarTodasLasParticulasEn")
	//Generar todas las particulas del cuerpo
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	AUEscenaCuerpo* cuerpoActual = (AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual];
	bool particulaGenerada = true;
	AUArregloNativoP<STParticulaDef>* defParticulas = cuerpoActual->definicionesParticulas();
	SI32 iDefParticula = 0, conteoDefParticulas = defParticulas->conteo;
	for(iDefParticula=0; iDefParticula<conteoDefParticulas && particulaGenerada; iDefParticula++){
		NBPunto posLocalPart	= defParticulas->elemento[iDefParticula].posicion;
		NBPunto posEscenaPart;	NBMATRIZ_MULTIPLICAR_PUNTO(posEscenaPart, _cacheObjEscena.matrizEscena, posLocalPart.x, posLocalPart.y);
		particulaGenerada		= privGenerarParticula(defParticulas->elemento[iDefParticula].textura, posEscenaPart, vectorImpacto, agregarParticulasEn);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

SI16 AUEscenaCuerpoMulti::privIndicePresentacionSegunVida(float vida){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::privIndicePresentacionSegunVida")
	SI16 indice = -1;
	if(_cuerposPorEstado.conteo>0){
		if(vida==1.0f){
			indice = 0;
		} else if(vida==0.0f){
			indice = (_cuerposPorEstado.conteo-1);
		} else {
			float margenPorEstado = (1.0f / (float)_cuerposPorEstado.conteo);
			int i; float maxVidaEstado = 1.0f;
			for(i=0; i<_cuerposPorEstado.conteo && indice==-1; i++){
				if(vida>(maxVidaEstado-margenPorEstado) && vida<=maxVidaEstado) 
					indice = i;
				maxVidaEstado -= margenPorEstado;
			}
		}
		NBASSERT(indice!=-1) //Si falla, no se definio el indice
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return indice;
}

//

void AUEscenaCuerpoMulti::establecerCuerpoTipo(ENCuerpoTipo tipoCuerpo, ENAnimFisicaTipo tipoAnimacion){ //PENDIENTE: probar esta funcion
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::establecerCuerpoTipo")
	AUEscenaCuerpoI::establecerCuerpoTipo(tipoCuerpo, tipoAnimacion);
	if(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo) ((AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual])->establecerCuerpoTipo(tipoCuerpo, ENAnimFisicaTipo_Ninguna); //ENAnimFisicaTipo_Ninguna, el cuerpo no debe linkiarse a si mismo, sino que al contendorMulti
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoMulti::establecerCreacionCuerpoTipo(ENCuerpoTipo tipoCuerpo, ENAnimFisicaTipo tipoAnimacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::establecerCreacionCuerpoTipo")
	AUEscenaCuerpoI::establecerCreacionCuerpoTipo(tipoCuerpo, tipoAnimacion);
	if(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo) ((AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual])->establecerCreacionCuerpoTipo(tipoCuerpo, ENAnimFisicaTipo_Ninguna); //ENAnimFisicaTipo_Ninguna, el cuerpo no debe linkiarse a si mismo, sino que al contendorMulti
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoMulti::establecerCreacionCuerpoMascara(UI8 mascaraProps){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::establecerCreacionCuerpoMascara")
	AUEscenaCuerpoI::establecerCreacionCuerpoMascara(mascaraProps);
	if(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo) ((AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual])->establecerCreacionCuerpoMascara(mascaraProps);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoMulti::establecerCreacionCuerpoFiltro(SI16 filtroCuerpo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::establecerCreacionCuerpoFiltro")
	AUEscenaCuerpoI::establecerCreacionCuerpoFiltro(filtroCuerpo);
	if(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo) ((AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual])->establecerCreacionCuerpoFiltro(filtroCuerpo);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoMulti::establecerCreacionCuerpoFactorGravedad(float factorGravedad){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::establecerCreacionCuerpoFactorGravedad")
	AUEscenaCuerpoI::establecerCreacionCuerpoFactorGravedad(factorGravedad);
	if(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo) ((AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual])->establecerCreacionCuerpoFactorGravedad(factorGravedad);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoMulti::establecerCreacionCuerpoDatoUsuario(void* datoUsuario){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::establecerCreacionCuerpoDatoUsuario")
	AUEscenaCuerpoI::establecerCreacionCuerpoDatoUsuario(datoUsuario);
	if(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo) ((AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual])->establecerCreacionCuerpoDatoUsuario(datoUsuario);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaCuerpoMulti::agregarParticulasObjetosDesprendiblesHaciaCapa(AUArregloNativoMutableP<NBParticula>* arregloCrearPiezaDesprendibleEn, AUEscenaContenedor* agregarEnCapa, const NBPunto &vectorImpacto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::agregarParticulasObjetosDesprendiblesHaciaCapa")
	NBASSERT(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo)
	((AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual])->agregarParticulasObjetosDesprendiblesHaciaCapa(arregloCrearPiezaDesprendibleEn, agregarEnCapa, vectorImpacto);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

NBCajaAABB AUEscenaCuerpoMulti::cajaLocalSombra(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::cajaLocalSombra")
	NBASSERT(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo)
	AUEscenaCuerpo* cuerpoActual = (AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual];
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cuerpoActual->cajaLocalSombra();
}

AUArregloNativoP<STParticulaDef>* AUEscenaCuerpoMulti::definicionesParticulas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::definicionesParticulas")
	NBASSERT(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo)
	AUEscenaCuerpo* cuerpoActual = (AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual];
	AUArregloNativoP<STParticulaDef>* r = cuerpoActual->definicionesParticulas();
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

const STParticulaDef* AUEscenaCuerpoMulti::cualquierParticulaEnZonaGeneracion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::cualquierParticulaEnZonaGeneracion")
	NBASSERT(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo)
	AUEscenaCuerpo* cuerpoActual = (AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual];
	const STParticulaDef* r = cuerpoActual->cualquierParticulaEnZonaGeneracion();
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

void AUEscenaCuerpoMulti::procesarImpacto(float velocidadImpacto, ENCuerpoSonidoTipo tipoSonido, AUArregloMutable* arregloSonidosRecicables){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::procesarImpacto")
	NBASSERT(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo)
	AUEscenaCuerpo* cuerpoActual = (AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual];
	cuerpoActual->procesarImpacto(velocidadImpacto, tipoSonido, arregloSonidosRecicables);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

const char* AUEscenaCuerpoMulti::rutaArchivoOrigen(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::rutaArchivoOrigen")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return ""; //PENDIENTE
}

float AUEscenaCuerpoMulti::multiplicadorDanoProduce(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::multiplicadorDanoProduce")
	NBASSERT(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo)
	AUEscenaCuerpo* cuerpoActual = (AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual];
	float r = cuerpoActual->multiplicadorDanoProduce();
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

float AUEscenaCuerpoMulti::multiplicadorDanoRecibe(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::multiplicadorDanoRecibe")
	NBASSERT(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo)
	AUEscenaCuerpo* cuerpoActual = (AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual];
	float r = cuerpoActual->multiplicadorDanoRecibe();
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

//

/*b2Body*/void* AUEscenaCuerpoMulti::cuerpo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::cuerpo")
	NBASSERT(_iCuerpoActual>=-1 && _iCuerpoActual<_cuerposPorEstado.conteo)
	/*b2Body*/void* cpoFisicaActual = NULL;
	if(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo){
		AUEscenaCuerpo* cuerpoActual = (AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual];
		cpoFisicaActual = cuerpoActual->cuerpo();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cpoFisicaActual;
}

void AUEscenaCuerpoMulti::crearCuerpo(){			//Para manejo manual (los cuerpos se crean y destruyen automaticamente)
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::crearCuerpo")
	NBASSERT(_iCuerpoActual>=-1 && _iCuerpoActual<_cuerposPorEstado.conteo)
	if(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo){
		AUEscenaCuerpo* objCuerpo = (AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual];
		/*b2Body*/void* cpoFisica = objCuerpo->cuerpo();
		NBASSERT(cpoFisica == NULL)
		if(cpoFisica == NULL){
			objCuerpo->establecerCreacionCuerpoTipo((ENCuerpoTipo)_propsCreacionCuerpo.tipoCuerpo, ENAnimFisicaTipo_Ninguna);
			objCuerpo->establecerCreacionCuerpoMascara(_propsCreacionCuerpo.mascaraProps);
			objCuerpo->establecerCreacionCuerpoFactorGravedad(_propsCreacionCuerpo.factorGravedad);
			objCuerpo->establecerCreacionCuerpoFiltro(_propsCreacionCuerpo.filtroCuerpos);
			objCuerpo->establecerCreacionCuerpoDatoUsuario(this);
			objCuerpo->crearCuerpo();
			if(_propsCreacionCuerpo.tipoAnimacion==ENAnimFisicaTipo_SeguirCuerpo || _propsCreacionCuerpo.tipoAnimacion==ENAnimFisicaTipo_SeguirObjetoEscena){
#				ifndef CONFIG_NB_UNSUPPORT_BOX2D
				NBASSERT(objCuerpo->cuerpo() != NULL)
				NBGestorFisica::agregarUnion(this, (b2Body*)objCuerpo->cuerpo(), _propsCreacionCuerpo.tipoCuerpo==ENCuerpoTipo_Kinematico?ENAnimFisicaTipo_SeguirObjetoEscena:ENAnimFisicaTipo_SeguirCuerpo);
#				endif
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoMulti::crearUnionesCuerpo(){	//Para manejo manual (los cuerpos se crean y destruyen automaticamente)
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::crearUnionesCuerpo")
	NBASSERT(_iCuerpoActual>=-1 && _iCuerpoActual<_cuerposPorEstado.conteo)
	if(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo){
		((AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual])->crearUnionesCuerpo();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoMulti::destruirCuerpo(){		//Para manejo manual (los cuerpos se crean y destruyen automaticamente)
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::destruirCuerpo")
	NBASSERT(_iCuerpoActual>=-1 && _iCuerpoActual<_cuerposPorEstado.conteo)
	if(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo){
		AUEscenaCuerpo* objCuerpo = (AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual];
		/*b2Body*/void* cpoFisica = objCuerpo->cuerpo();
		NBASSERT(cpoFisica != NULL)
		if(cpoFisica != NULL){
#			ifndef CONFIG_NB_UNSUPPORT_BOX2D
			NBGestorFisica::quitarUnion(this, (b2Body*)cpoFisica);
#			endif
			objCuerpo->destruirCuerpo();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoMulti::privCambiarCuerpoSiNecesario(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::privCambiarCuerpoSiNecesario")
	SI32 iCuerpoAsegurar = privIndicePresentacionSegunVida(_vida); NBASSERT(iCuerpoAsegurar>=0 && iCuerpoAsegurar<_cuerposPorEstado.conteo)
	if(iCuerpoAsegurar!=_iCuerpoActual){
		AUEscenaCuerpo* objCuerpoAnterior	= NULL;
		/*b2Body*/void* cuerpoAnterior		= NULL;
		if(_iCuerpoActual>=0 && _iCuerpoActual<_cuerposPorEstado.conteo){
			objCuerpoAnterior	= (AUEscenaCuerpo*)_cuerposPorEstado.elemento[_iCuerpoActual];
			cuerpoAnterior		= objCuerpoAnterior->cuerpo();
		}
		//Agregar el nuevo cuerpo
		AUEscenaCuerpo* objCuerpoNuevo = (AUEscenaCuerpo*)_cuerposPorEstado.elemento[iCuerpoAsegurar];
		objCuerpoNuevo->establecerMultiplicadoresPropsCuerpo(_propsMultiplicadores);
		objCuerpoNuevo->establecerCreacionCuerpoTipo((ENCuerpoTipo)_propsCreacionCuerpo.tipoCuerpo, ENAnimFisicaTipo_Ninguna);
		objCuerpoNuevo->establecerCreacionCuerpoMascara(_propsCreacionCuerpo.mascaraProps);
		objCuerpoNuevo->establecerCreacionCuerpoFactorGravedad(_propsCreacionCuerpo.factorGravedad);
		objCuerpoNuevo->establecerCreacionCuerpoFiltro(_propsCreacionCuerpo.filtroCuerpos);
		objCuerpoNuevo->establecerCreacionCuerpoDatoUsuario(this);
		this->agregarObjetoEscenaEnIndice(objCuerpoNuevo, 0);
		/*b2Body*/void* cuerpoNuevo = objCuerpoNuevo->cuerpo(); //NBASSERT(idEscena!=-1 || cuerpoNuevo != NULL)
		if(cuerpoNuevo != NULL){
			if(cuerpoAnterior != NULL){
#				ifndef CONFIG_NB_UNSUPPORT_BOX2D
				//Copiar propiedades del cuerpo anterior
				((b2Body*)cuerpoNuevo)->SetLinearVelocity(((b2Body*)cuerpoAnterior)->GetLinearVelocity());
				((b2Body*)cuerpoNuevo)->SetAngularVelocity(((b2Body*)cuerpoAnterior)->GetAngularVelocity());
				((b2Body*)cuerpoNuevo)->SetLinearDamping(((b2Body*)cuerpoAnterior)->GetLinearDamping());
				((b2Body*)cuerpoNuevo)->SetAngularDamping(((b2Body*)cuerpoAnterior)->GetAngularDamping());
				NBGestorFisica::relinkearUnionesDeCuerpo(_idMundoFisica, (b2Body*)cuerpoAnterior, (b2Body*)cuerpoNuevo);
#				endif
			}
			if(_propsCreacionCuerpo.tipoAnimacion==ENAnimFisicaTipo_SeguirCuerpo || _propsCreacionCuerpo.tipoAnimacion==ENAnimFisicaTipo_SeguirObjetoEscena){
#				ifndef CONFIG_NB_UNSUPPORT_BOX2D
				NBGestorFisica::agregarUnion(this, (b2Body*)cuerpoNuevo, _propsCreacionCuerpo.tipoCuerpo==ENCuerpoTipo_Kinematico?ENAnimFisicaTipo_SeguirObjetoEscena:ENAnimFisicaTipo_SeguirCuerpo);
#				endif
			}
		}
		//Quitar el cuerpo actual
		if(objCuerpoAnterior != NULL){
#			ifndef CONFIG_NB_UNSUPPORT_BOX2D
			NBGestorFisica::quitarUnion(this, (b2Body*)cuerpoAnterior);
#			endif
			this->quitarObjetoEscena(objCuerpoAnterior);
		}
		_iCuerpoActual = iCuerpoAsegurar;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaCuerpoMulti::agregadoEnEscenaConHijosNotificados(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::agregadoEnEscenaConHijosNotificados")
	/*b2Body*/void* cuerpoActual = this->cuerpo();
	if(cuerpoActual != NULL && (_propsCreacionCuerpo.tipoAnimacion==ENAnimFisicaTipo_SeguirCuerpo || _propsCreacionCuerpo.tipoAnimacion==ENAnimFisicaTipo_SeguirObjetoEscena)){
#		ifndef CONFIG_NB_UNSUPPORT_BOX2D
		NBGestorFisica::agregarUnion(this, (b2Body*)cuerpoActual, _propsCreacionCuerpo.tipoCuerpo==ENCuerpoTipo_Kinematico?ENAnimFisicaTipo_SeguirObjetoEscena:ENAnimFisicaTipo_SeguirCuerpo);
#		endif
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpoMulti::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::quitandoDeEscena")
	/*b2Body*/void* cuerpoActual = this->cuerpo();
	if(cuerpoActual != NULL){
#		ifndef CONFIG_NB_UNSUPPORT_BOX2D
		NBGestorFisica::quitarUnion(this, (b2Body*)cuerpoActual);
#		endif
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaCuerpoMulti::establecerGrupoAnimacion(ENGestorAnimadoresGrupo grupoAnimacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpoMulti::establecerGrupoAnimacion")
	SI32 iCpo;
	for(iCpo=0; iCpo<_cuerposPorEstado.conteo; iCpo++){
		((AUEscenaCuerpoI*)_cuerposPorEstado.elemento[iCpo])->establecerGrupoAnimacion(grupoAnimacion);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaCuerpoMulti, AUEscenaCuerpoI)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaCuerpoMulti, "AUEscenaCuerpoMulti", AUEscenaCuerpoI)
AUOBJMETODOS_CLONAR_THIS(AUEscenaCuerpoMulti)









