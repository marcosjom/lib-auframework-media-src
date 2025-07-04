//
//  AUEscenaCuerpo.cpp
//  Gameplay_iOS
//
//  Created by Marcos Ortega on 25/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaCuerpo.h"

AUEscenaCuerpo::AUEscenaCuerpo(AUCuerpoDef* definicionCuerpo) : AUEscenaCuerpoI(definicionCuerpo->plantillaPresentacion)
#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
, _sonidosDeImpacto(this)
#endif
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::AUEscenaCuerpo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaCuerpo")
	NBASSERT(definicionCuerpo != NULL)
	_cuerpo							= NULL;
	_definicionCuerpo				= definicionCuerpo; _definicionCuerpo->retener(NB_RETENEDOR_THIS);
	//Agregar los objetos sombras
	UI16 iFigSmbra, conteoFigsSombras = _definicionCuerpo->ptsFigurasSombraLocal.conteo;
	for(iFigSmbra=0; iFigSmbra<conteoFigsSombras; iFigSmbra++){
		AUEscenaLuzSombra* sombra = new(this) AUEscenaLuzSombra((AUArregloNativoP<NBPunto>*)_definicionCuerpo->ptsFigurasSombraLocal.elemento[iFigSmbra], _definicionCuerpo->texturaSombra);
		sombra->establecerMultiplicadorColor(_definicionCuerpo->colorSombra);
		this->agregarObjetoEscena(sombra);
		sombra->liberar(NB_RETENEDOR_THIS);
	}
	//Agregar el objeto iluminacion
	if(_definicionCuerpo->mascarasIluminacion.conteo>0){
		AUEscenaIluminacion* objIluminacion = new(this) AUEscenaIluminacion(definicionCuerpo);
		this->agregarObjetoEscena(objIluminacion);
		objIluminacion->liberar(NB_RETENEDOR_THIS);
	}
	//Deshablitar los objetos escena aleatorios (que no tengan suerte)
	SI32 iObjAlea;
	for(iObjAlea=_definicionCuerpo->indicesObjEscenaAleatorios.conteo-1; iObjAlea>=0; iObjAlea--){
		((AUEscenaObjeto*)this->hijos()->elemento[_definicionCuerpo->indicesObjEscenaAleatorios.elemento[iObjAlea]])->establecerVisible((rand() % 10)>=8);
	}
	//Sonidos de impacto
	UI32 iSnd, iSndConteo = _definicionCuerpo->defSonidosDeImpacto.conteo;
	for(iSnd=0; iSnd<iSndConteo; iSnd++){
#		ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
		STEscenaSonidoImpacto sonidoImpacto;
		sonidoImpacto.definicion	= &(_definicionCuerpo->defSonidosDeImpacto.elemento[iSnd]);
		sonidoImpacto.sonido		= new(this) AUEscenaSonido(ENAudioGrupo_Efectos, NULL, sonidoImpacto.definicion->bufferAL);
		sonidoImpacto.segundoVirtualFinalizariaReproduccion = 0.0f;
		_sonidosDeImpacto.agregarElemento(sonidoImpacto);
#		endif
	}
	//Copia de objetos desprendibles
	/*_objEscenaDesprendibles			= new(this) AUArregloMutable(); NB_DEFINE_NOMBRE_PUNTERO(_objEscenaDesprendibles, "AUEscenaCuerpoI::_objEscenaDesprendibles")
	 AUArreglo* hijosDeOtro			= definicionCuerpo->plantillaPresentacion->hijos();
	 AUArreglo* hijosDeEste			= this->hijos();
	 SI32 iHijo; 
	 for(iHijo=0; iHijo<hijosDeOtro->conteo; iHijo++){
	 AUEscenaObjeto* hijoOtro	= (AUEscenaObjeto*)hijosDeOtro->elemento[iHijo];
	 AUEscenaObjeto* hijoEste	= (AUEscenaObjeto*)hijosDeEste->elemento[iHijo];
	 if(definicionCuerpo->objEscenaDesprendibles->indiceDe(hijoOtro)!=-1){
	 _objEscenaDesprendibles->agregarElemento(hijoEste);
	 }
	 }*/
	//Inicializar hijos heredados
	privInicializarHijosHeredados(this->hijos(), definicionCuerpo->plantillaPresentacion->hijos());
	//Gestion de sombras
	this->fijarAgrupadorDeSombras();
	this->establecerPreagruparSombrasParaHijos(true);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaCuerpo::AUEscenaCuerpo(AUEscenaCuerpo* otroCuerpo) : AUEscenaCuerpoI(otroCuerpo)
#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
, _sonidosDeImpacto(this)
#endif
{ //Ojo: si pasase como parametro al constructor de AUEscenaContenedor 'otroCuerpo->_plantillaPresentacion' no se heredarian las propiedades
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::AUEscenaCuerpo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaCuerpo")
	NBASSERT(otroCuerpo != NULL)
	_cuerpo							= NULL;
	_definicionCuerpo				= otroCuerpo->_definicionCuerpo; _definicionCuerpo->retener(NB_RETENEDOR_THIS);
	_propsCreacionCuerpo			= otroCuerpo->_propsCreacionCuerpo;
	_propsCreacionCuerpo.datoUsuario = NULL;
	_propsMultiplicadores			= otroCuerpo->_propsMultiplicadores;
	//
	//El objetoEscenaIluminacion ya es clonado
	//
	//Deshablitar los objetos escena aleatorios (que no tengan suerte)
	SI32 iObjAlea;
	for(iObjAlea=_definicionCuerpo->indicesObjEscenaAleatorios.conteo-1; iObjAlea>=0; iObjAlea--){
		((AUEscenaObjeto*)this->hijos()->elemento[_definicionCuerpo->indicesObjEscenaAleatorios.elemento[iObjAlea]])->establecerVisible((rand() % 10)>=8);
	}
	//Copia de sonidos de impacto
	SI32 iSnd, iSndConteo = _definicionCuerpo->defSonidosDeImpacto.conteo;
	for(iSnd=0; iSnd<iSndConteo; iSnd++){
#		ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
		STEscenaSonidoImpacto sonidoImpacto;
		sonidoImpacto.definicion	= &(_definicionCuerpo->defSonidosDeImpacto.elemento[iSnd]);
		sonidoImpacto.sonido		= new(this) AUEscenaSonido(ENAudioGrupo_Efectos, NULL, sonidoImpacto.definicion->bufferAL);
		sonidoImpacto.segundoVirtualFinalizariaReproduccion = 0.0f;
		_sonidosDeImpacto.agregarElemento(sonidoImpacto);
#		endif
	}
	//Copia de objetos desprendibles
	/*_objEscenaDesprendibles			= new(this) AUArregloMutable(); NB_DEFINE_NOMBRE_PUNTERO(_objEscenaDesprendibles, "AUEscenaCuerpoI::_objEscenaDesprendibles")
	 AUArreglo* hijosDeOtro			= otroCuerpo->hijos();
	 AUArreglo* hijosDeEste			= this->hijos();
	 SI32 iHijo; 
	 for(iHijo=0; iHijo<hijosDeOtro->conteo; iHijo++){
	 AUEscenaObjeto* hijoOtro	= (AUEscenaObjeto*)hijosDeOtro->elemento[iHijo];
	 AUEscenaObjeto* hijoEste	= (AUEscenaObjeto*)hijosDeEste->elemento[iHijo];
	 if(otroCuerpo->_objEscenaDesprendibles->indiceDe(hijoOtro)!=-1){
	 _objEscenaDesprendibles->agregarElemento(hijoEste);
	 }
	 }*/
	//Inicializar hijos heredados
	privInicializarHijosHeredados(this->hijos(), otroCuerpo->hijos());
	//Gestion de sombras
	this->fijarAgrupadorDeSombras();
	this->establecerPreagruparSombrasParaHijos(true);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaCuerpo::~AUEscenaCuerpo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::~AUEscenaCuerpo")
	//Detener subanimaciones
	SI32 iHijo; AUArreglo* hijos				= this->hijos();
	for(iHijo=0; iHijo<hijos->conteo; iHijo++){
		if(hijos->elemento[iHijo]->esClase(AUEscenaAnimacionI::idTipoClase)){
			((AUEscenaAnimacionI*)hijos->elemento[iHijo])->detenerAnimacion();
		}
	}
	//Liberar elementos propios
#	ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
	UI32 iSnd, iSndConteo = _sonidosDeImpacto.conteo;
	for(iSnd=0; iSnd<iSndConteo; iSnd++){
		_sonidosDeImpacto.elemento[iSnd].sonido->detenerSonido();
		_sonidosDeImpacto.elemento[iSnd].sonido->liberar(NB_RETENEDOR_THIS);
	}
#	endif
	NBASSERT(_idMundoFisica==-1)
	NBASSERT(_cuerpo == NULL)
	if(_definicionCuerpo != NULL) { _definicionCuerpo->liberar(NB_RETENEDOR_THIS); _definicionCuerpo = NULL; }
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpo::privInicializarHijosHeredados(AUArreglo* hijosCapaLocal, AUArreglo* hijosCapaOriginal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::privInicializarHijosHeredados")
	//Actualizar hijos
	UI16 iHijo, conteoHijosOrig		= hijosCapaOriginal->conteo;
	for(iHijo=0; iHijo<conteoHijosOrig; iHijo++){
		AUEscenaObjeto* hijoLocal	= (AUEscenaObjeto*)hijosCapaLocal->elemento[iHijo];
		AUEscenaObjeto* hijoOrig	= (AUEscenaObjeto*)hijosCapaOriginal->elemento[iHijo];
		NBASSERT(hijoLocal->esClase(hijoOrig->idUltimaClase()))
		if(hijoLocal->esClase(AUNodoElectroCarga::idTipoClase)){
			NBASSERT(hijoOrig->esClase(AUNodoElectroCarga::idTipoClase))
			((AUNodoElectroCarga*)hijoLocal)->agregarDestinosElectroCargarSegunPlantilla((AUNodoElectroCarga*)hijoOrig, hijosCapaOriginal, hijosCapaLocal);
		}
		//En los cuerpo no estan implementados sensores (por tanto se ignoran los Laseres, Baterias y PanelesSolares)
		if(hijoLocal->esClase(AUEscenaAnimacionI::idTipoClase)){
			AUEscenaAnimacionI* animacion = (AUEscenaAnimacionI*)hijoLocal;
			animacion->presentarFrame(animacion->indiceFrameInicio());
			if(animacion->reproducirAlCargar()) animacion->reanudarAnimacion();
		} else if(hijoLocal->esClase(AUElectronodo::idTipoClase)){
			NBASSERT(hijoOrig->esClase(AUElectronodo::idTipoClase))
			((AUElectronodo*)hijoLocal)->actualizarDestinosPasosLanzamientosSegunPlantilla((AUElectronodo*)hijoOrig, hijosCapaOriginal, hijosCapaLocal);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaCuerpo::agregarParticulasObjetosDesprendiblesHaciaCapa(AUArregloNativoMutableP<NBParticula>* arregloCrearPiezaDesprendibleEn, AUEscenaContenedor* agregarEnCapa, const NBPunto &vectorImpacto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::objEscenaDesprendibles")
	AUArreglo* hijos		= this->hijos();
	UI16 conteoHijos		= hijos->conteo;
	UI16 iObj, conteoObjs	= _definicionCuerpo->indicesObjEscenaDesprendibles.conteo;
	for(iObj=0; iObj<conteoObjs; iObj++){
		UI16 indiceHijo = _definicionCuerpo->indicesObjEscenaDesprendibles.elemento[iObj];
		if(indiceHijo<conteoHijos){
			AUEscenaObjeto* objDesprendible = (AUEscenaObjeto*)hijos->elemento[indiceHijo];
			NBPunto posicionActualEnEscena 	= objDesprendible->coordenadaLocalAEscena(0.0f, 0.0f);
			NBPunto posicionDestinoEnCapa	= agregarEnCapa->coordenadaEscenaALocal(posicionActualEnEscena.x, posicionActualEnEscena.y);
			AUEscenaObjeto* copiaObjDespr	= (AUEscenaObjeto*)objDesprendible->clonar(this->tipoMemoriaResidencia(), ENClonadoTipo_ClonarInstancias); NB_DEFINE_NOMBRE_PUNTERO(copiaObjDespr, "AUBasura::copiaObjDespr")
			copiaObjDespr->establecerVisible(true);
			copiaObjDespr->establecerTraslacion(posicionDestinoEnCapa);
			agregarEnCapa->agregarObjetoEscena(copiaObjDespr);
			NBParticula particula;
			particula.activa		= true;
			particula.objEscena		= copiaObjDespr;
			particula.tiempoViva	= 0.0f;
			particula.rotacion		= (rand() % 180) - 180;
			particula.direccion.x	= -vectorImpacto.x * 15.0f;
			particula.direccion.y	= -vectorImpacto.y * 15.0f;
			arregloCrearPiezaDesprendibleEn->agregarElemento(particula);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

NBCajaAABB AUEscenaCuerpo::cajaLocalSombra(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::cajaLocalSombra")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _definicionCuerpo->cajaLocalSombras;
}

AUArregloNativoP<STParticulaDef>* AUEscenaCuerpo::definicionesParticulas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::definicionesParticulas")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return &_definicionCuerpo->defParticulas;
}

const STParticulaDef* AUEscenaCuerpo::cualquierParticulaEnZonaGeneracion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::cualquierParticulaEnZonaGeneracion")
	const STParticulaDef* defParticula = NULL;
	if(_definicionCuerpo->indicesParticulasEnZonasGen.conteo!=0){
		defParticula = &(_definicionCuerpo->defParticulas.elemento[_definicionCuerpo->indicesParticulasEnZonasGen.elemento[rand()%_definicionCuerpo->indicesParticulasEnZonasGen.conteo]]);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return defParticula;
}

void AUEscenaCuerpo::procesarImpacto(float velocidadImpacto, ENCuerpoSonidoTipo tipoSonido, AUArregloMutable* arregloSonidosRecicables){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::procesarImpacto")
#	ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
	float segundoVirtualActual = NBGestorAnimadores::segundosVirtualesAcumulados();
	//Seleccionar la plantilla de sonido
	SI32 indiceSndReproducir = -1;
	UI32 iSnd, iSndConteo = _sonidosDeImpacto.conteo;
	for(iSnd=0; iSnd<iSndConteo; iSnd++){
		STEscenaSonidoImpactoDef* defSonido = _sonidosDeImpacto.elemento[iSnd].definicion;
		if(velocidadImpacto>=defSonido->velImpactoMin && velocidadImpacto<=defSonido->velImpactoMax){
			if(_sonidosDeImpacto.elemento[iSnd].segundoVirtualFinalizariaReproduccion<=segundoVirtualActual){
				if(indiceSndReproducir==-1 || (rand()%2)==0){
					indiceSndReproducir = iSnd;
					break;
				}
			}
		}
	}
	if(indiceSndReproducir!=-1){
		STEscenaSonidoImpactoDef* defSonido = _sonidosDeImpacto.elemento[indiceSndReproducir].definicion;
		STEscenaSonidoImpacto* datosSonido = &(_sonidosDeImpacto.elemento[indiceSndReproducir]);
		float volumenSonido = 0.01f+(0.99f*(velocidadImpacto/defSonido->velImpactoMax)); if(volumenSonido>1.0f) volumenSonido = 1.0f;
		//PRINTF_INFO("Volumen de sonido: %f (velocidad: %f / %f)\n", volumenSonido, velocidadImpacto, defSonido->velImpactoMax);
		if(arregloSonidosRecicables != NULL){
			//Intentar reutilizar alguno de los sonidos actuales
			SI32 iSnd; bool fuenteReutilizada	= false;
			for(iSnd=0; iSnd<arregloSonidosRecicables->conteo && !fuenteReutilizada; iSnd++){
				AUEscenaSonido* sonidoEscena = (AUEscenaSonido*)arregloSonidosRecicables->elemento[iSnd];
				bool usarEsteSonido = false;
				if(sonidoEscena->bufferAL()==defSonido->bufferAL){
					if(sonidoEscena->fuenteAL()==0){
						usarEsteSonido = true;
					} else {
						usarEsteSonido = (!NBGestorSonidos::fuenteReproduciendo(sonidoEscena->fuenteAL()));
					}
				}
				if(usarEsteSonido){
					sonidoEscena->establecerRepetir(false);
					sonidoEscena->establecerVolumen(volumenSonido);
					sonidoEscena->reproducirSonido();
					fuenteReutilizada = true;
					//PRINTF_INFO("Fuente existente de sonido basura REUTILIZADA (vol %f, vel %f)\n", volumenSonido, velocidadImpacto);
				}
			}
			//Crear un nuevo sonido
			if(!fuenteReutilizada){
				AUEscenaSonido* nuevoSonido = new(this) AUEscenaSonido(ENAudioGrupo_Efectos, NULL, defSonido->bufferAL);
				nuevoSonido->establecerRepetir(false);
				nuevoSonido->establecerVolumen(volumenSonido);
				nuevoSonido->reproducirSonido();
				arregloSonidosRecicables->agregarElemento(nuevoSonido);
				nuevoSonido->liberar(NB_RETENEDOR_THIS);
				//PRINTF_INFO("Fuente de sonido basura CREADA (vol %f, vel %f)\n", volumenSonido, velocidadImpacto);
			}
		} else {
			if(datosSonido->sonido == NULL){
				datosSonido->sonido = new(this) AUEscenaSonido(ENAudioGrupo_Efectos, NULL, defSonido->bufferAL);
			}
			datosSonido->sonido->establecerRepetir(false);
			datosSonido->sonido->establecerVolumen(volumenSonido);
			datosSonido->sonido->reproducirSonido();
			//PRINTF_INFO("Sonido local de basura REPRODUCIDO (vol %f, vel %f)\n", volumenSonido, velocidadImpacto);
		}
		_sonidosDeImpacto.elemento[indiceSndReproducir].segundoVirtualFinalizariaReproduccion = segundoVirtualActual + defSonido->segsDuracion;
	}
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

const char* AUEscenaCuerpo::rutaArchivoOrigen(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::rutaArchivoOrigen")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _definicionCuerpo->rutaRecurso.str();
}

float AUEscenaCuerpo::multiplicadorDanoProduce(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::multiplicadorDanoProduce")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _definicionCuerpo->multDanoProduce;
}

float AUEscenaCuerpo::multiplicadorDanoRecibe(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::multiplicadorDanoRecibe")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _definicionCuerpo->multDanoRecibe;
}

//


/*b2Body*/void* AUEscenaCuerpo::cuerpo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::cuerpo")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _cuerpo;
}

void AUEscenaCuerpo::crearCuerpo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::crearCuerpo")
	NBASSERT(_cuerpo == NULL)
	NBASSERT(_idMundoFisica>=0)
	NBASSERT(_propsCreacionCuerpo.tipoCuerpo==ENCuerpoTipo_Estatico || _propsCreacionCuerpo.tipoCuerpo==ENCuerpoTipo_Kinematico || _propsCreacionCuerpo.tipoCuerpo==ENCuerpoTipo_Dinamico)
	NBASSERT(_definicionCuerpo->fijaciones.conteo!=0)
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	//Transformaciones de referencia
	NBMatriz matrizEscenaCuerpo		= this->matrizEscenaCalculada();
	NBPunto posEscena;				NBMATRIZ_MULTIPLICAR_PUNTO(posEscena, matrizEscenaCuerpo, 0.0f, 0.0f);
	NBPunto posEscenaDer;			NBMATRIZ_MULTIPLICAR_PUNTO(posEscenaDer, matrizEscenaCuerpo, 1.0f, 0.0f);
	float radianesRotacion;			NBPUNTO_RADIANES_VECTOR(radianesRotacion, posEscena.x, posEscena.y, posEscenaDer.x, posEscenaDer.y);
	b2BodyDef defCuerpo;
	defCuerpo.position.Set(posEscena.x/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posEscena.y/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
	defCuerpo.angle					= radianesRotacion;
	defCuerpo.type					= (_propsCreacionCuerpo.tipoCuerpo==ENCuerpoTipo_Estatico?b2_staticBody:_propsCreacionCuerpo.tipoCuerpo==ENCuerpoTipo_Kinematico?b2_kinematicBody:b2_dynamicBody);
	defCuerpo.active				= ((_propsCreacionCuerpo.mascaraProps & AUCUERPODEF_MASCARA_ACTIVO)!=0);
	defCuerpo.allowSleep			= ((_propsCreacionCuerpo.mascaraProps & AUCUERPODEF_MASCARA_PERMITIR_DORMIR)!=0);
	defCuerpo.fixedRotation			= ((_propsCreacionCuerpo.mascaraProps & AUCUERPODEF_MASCARA_ROTACION_FIJA)!=0);
	defCuerpo.bullet				= ((_propsCreacionCuerpo.mascaraProps & AUCUERPODEF_MASCARA_ES_BALA)!=0);
	defCuerpo.gravityScale			= _propsCreacionCuerpo.factorGravedad;
	defCuerpo.userData				= (_propsCreacionCuerpo.datoUsuario != NULL?_propsCreacionCuerpo.datoUsuario:this);
	_cuerpo							= NBGestorFisica::crearCuerpo(_idMundoFisica, &defCuerpo);
	//crear fijaciones a cuerpo
	NBPunto vectorEscalacion; NBMATRIZ_MULTIPLICAR_PUNTO(vectorEscalacion, matrizEscenaCuerpo, 1.0f, 1.0f); //Para determinar si los vertices han sido invertido en orden contrareloj
	vectorEscalacion.x -= posEscena.x; vectorEscalacion.y -= posEscena.y; NBPUNTO_ROTAR_RADIANES(vectorEscalacion, vectorEscalacion, -radianesRotacion); //De coordenada escena a local
	NBASSERT(vectorEscalacion.x!=0.0f || vectorEscalacion.y!=0.0f)
	//
	UI32 f, fConteo = _definicionCuerpo->fijaciones.conteo;
	for(f=0; f<fConteo; f++){
		b2PolygonShape defFormaPoligono;
		b2CircleShape defFormaCirculo;
		b2EdgeShape defFormaLinea;
		b2ChainShape defFormaCadena;
		b2FixtureDef propiedadesCuerpo;
		STFijacionDefinicion* fijacion = &(_definicionCuerpo->fijaciones.elemento[f]);
		if(fijacion->tipoFijacion==0 || fijacion->tipoFijacion==1 || fijacion->tipoFijacion==2 || fijacion->tipoFijacion==3){		//0==linea || 1==circulo || 2==poligono || 3==secuenciaLineas
			propiedadesCuerpo.isSensor			= fijacion->propiedadesFijacion.esSensor;
			propiedadesCuerpo.density			= fijacion->propiedadesFijacion.densidad * _propsMultiplicadores.multDensidad;
			propiedadesCuerpo.friction			= fijacion->propiedadesFijacion.friccion * _propsMultiplicadores.multFriccion;
			propiedadesCuerpo.restitution		= fijacion->propiedadesFijacion.restitucion * _propsMultiplicadores.multRestitucion;
			propiedadesCuerpo.filter.groupIndex	= _propsCreacionCuerpo.filtroCuerpos;
			//VERTICES DE FIJACION DE FISICA
			AUArregloNativoMutableP<NBPunto>* vertices = fijacion->vertices;
			if(fijacion->tipoFijacion==ENEscenaFiguraTipo_Linea && vertices->conteo>=2){
				//linea
				b2Vec2 puntosBox2D[2];
				NBMATRIZ_MULTIPLICAR_PUNTO(puntosBox2D[0], matrizEscenaCuerpo, vertices->elemento[0].x, vertices->elemento[0].y);
				NBMATRIZ_MULTIPLICAR_PUNTO(puntosBox2D[1], matrizEscenaCuerpo, vertices->elemento[1].x, vertices->elemento[1].y);
				puntosBox2D[0].x -= posEscena.x; puntosBox2D[0].y -= posEscena.y; NBPUNTO_ROTAR_RADIANES(puntosBox2D[0], puntosBox2D[0], -radianesRotacion); //De coordenada escena a local
				puntosBox2D[1].x -= posEscena.x; puntosBox2D[1].y -= posEscena.y; NBPUNTO_ROTAR_RADIANES(puntosBox2D[1], puntosBox2D[1], -radianesRotacion); //De coordenada escena a local
				defFormaLinea.Set(puntosBox2D[0], puntosBox2D[1]);
				propiedadesCuerpo.shape = &defFormaLinea;
				((b2Body*)_cuerpo)->CreateFixture(&propiedadesCuerpo);
			} else if(fijacion->tipoFijacion==ENEscenaFiguraTipo_Circulo && vertices->conteo>=2){
				//circulo
				NBPunto centro;	NBMATRIZ_MULTIPLICAR_PUNTO(centro, matrizEscenaCuerpo, vertices->elemento[0].x, vertices->elemento[0].y);
				NBPunto circun;	NBMATRIZ_MULTIPLICAR_PUNTO(circun, matrizEscenaCuerpo, vertices->elemento[1].x, vertices->elemento[1].y);
				float radio		= NBPUNTO_DISTANCIA(centro.x, centro.y, circun.x, circun.y); NBASSERT(radio>0.0f)
				//
				defFormaCirculo.m_radius	= radio;
				defFormaCirculo.m_p.Set(centro.x-posEscena.x, centro.y-posEscena.y);
				propiedadesCuerpo.shape = &defFormaCirculo;
				((b2Body*)_cuerpo)->CreateFixture(&propiedadesCuerpo);
			} else if((fijacion->tipoFijacion==ENEscenaFiguraTipo_PoligonoCerrado || fijacion->tipoFijacion==ENEscenaFiguraTipo_SecuenciaLineas) && vertices->conteo>=3){
				//poligono
				b2Vec2* puntosBox2D = (b2Vec2*)NBGestorMemoria::reservarMemoria(sizeof(b2Vec2) * vertices->conteo, ENMemoriaTipo_Temporal);
				//b2Vec2 puntosBox2D[32]; NBASSERT(vertices->conteo<=32)
				if(vertices->conteo>8){ PRINTF_WARNING("----------ADVERTENCIA DISENO CUERPO: fijacion tiene mas de 8 vertices: '%s' (tiene %d)\n", this->_definicionCuerpo->rutaRecurso.str(), vertices->conteo); }
				SI32 v, vConteo = vertices->conteo;
				if((vectorEscalacion.x*vectorEscalacion.y)<0.0f){	//si la escala invierte las fijaciones, entonces procesar en orden inverso
					int posV = 0;
					for(v=vConteo-1; v>=0; v--){
						NBMATRIZ_MULTIPLICAR_PUNTO(puntosBox2D[posV], matrizEscenaCuerpo, vertices->elemento[v].x, vertices->elemento[v].y)
						puntosBox2D[posV].x -= posEscena.x; puntosBox2D[posV].y -= posEscena.y; NBPUNTO_ROTAR_RADIANES(puntosBox2D[posV], puntosBox2D[posV], -radianesRotacion); //De coordenada escena a local
						posV++;
					}
				} else {
					for(v=0; v<vConteo; v++){
						NBMATRIZ_MULTIPLICAR_PUNTO(puntosBox2D[v], matrizEscenaCuerpo, vertices->elemento[v].x, vertices->elemento[v].y)
						puntosBox2D[v].x -= posEscena.x; puntosBox2D[v].y -= posEscena.y; NBPUNTO_ROTAR_RADIANES(puntosBox2D[v], puntosBox2D[v], -radianesRotacion); //De coordenada escena a local
					}
				}
				if(fijacion->tipoFijacion==ENEscenaFiguraTipo_PoligonoCerrado){
					defFormaPoligono.Set(puntosBox2D, vertices->conteo);
					propiedadesCuerpo.shape = &defFormaPoligono;
				} else if(fijacion->tipoFijacion==ENEscenaFiguraTipo_SecuenciaLineas){
					defFormaCadena.CreateChain(puntosBox2D, vertices->conteo);
					propiedadesCuerpo.shape = &defFormaCadena;
				} else {
					NBASSERT(false)
				}
				((b2Body*)_cuerpo)->CreateFixture(&propiedadesCuerpo);
				NBGestorMemoria::liberarMemoria(puntosBox2D);
			} else {
				NBASSERT(false)
			}
		}
	}
	if(_propsCreacionCuerpo.tipoAnimacion==ENAnimFisicaTipo_SeguirCuerpo || _propsCreacionCuerpo.tipoAnimacion==ENAnimFisicaTipo_SeguirObjetoEscena){
		NBGestorFisica::agregarUnion(this, ((b2Body*)_cuerpo), (ENAnimFisicaTipo)_propsCreacionCuerpo.tipoAnimacion);
	}
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpo::crearUnionesCuerpo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::crearUnionesCuerpo")
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	/*
	 SI32 iDefUnion;
	 for(iDefUnion=0; iDefUnion<_definicionCuerpo->defUnionesCuerpos.conteo; iDefUnion++){
	 STUnionCuerposDef* defUnion	= &(_definicionCuerpo->defUnionesCuerpos.elemento[iDefUnion]);
	 STUnionCuerpos nuevaUnion;
	 nuevaUnion.definicion		= defUnion;
	 nuevaUnion.cuerpoA			= this;
	 nuevaUnion.cuerpoB			= this;
	 nuevaUnion.b2Union			= NULL;
	 nuevaUnion.objEscena		= NULL;
	 //
	 if(defUnion->indiceCuerpoA>=0){		nuevaUnion.cuerpoA = this->hijos()->elemento[defUnion->indiceCuerpoA]; NBASSERT(nuevaUnion.cuerpoA->esClase(AUEscenaCuerpoI") || nuevaUnion.cuerpoA->esClase(AUEscenaAnimacionI")); }
	 if(defUnion->indiceCuerpoB>=0){		nuevaUnion.cuerpoB = this->hijos()->elemento[defUnion->indiceCuerpoB]; NBASSERT(nuevaUnion.cuerpoB->esClase(AUEscenaCuerpoI") || nuevaUnion.cuerpoB->esClase(AUEscenaAnimacionI")); }
	 //
	 NBASSERT(nuevaUnion.cuerpoA != NULL);
	 NBASSERT(nuevaUnion.cuerpoB != NULL);
	 NBASSERT(nuevaUnion.cuerpoA!=nuevaUnion.cuerpoB);
	 //Ignorar uniones de cuerpos que no estan visibles (son aleatorios y no tuvieron suerte)
	 if(((AUEscenaObjeto*)nuevaUnion.cuerpoA)->visible() && ((AUEscenaObjeto*)nuevaUnion.cuerpoB)->visible()){
	 _unionesCuerpos.agregarElemento(nuevaUnion);
	 }
	 }
	 */
	//Crear las uniones
	UI16 iUnion;
	for(iUnion=0; iUnion<_definicionCuerpo->defUnionesCuerpos.conteo; iUnion++){
		STUnionCuerposDef* unionCpos = &(_definicionCuerpo->defUnionesCuerpos.elemento[iUnion]);
		if((unionCpos->indiceCuerpoA >= 0 && unionCpos->indiceCuerpoB >= 0) || (unionCpos->indiceCuerpoA < 0 && unionCpos->indiceCuerpoB >= 0) || (unionCpos->indiceCuerpoA >= 0 && unionCpos->indiceCuerpoB < 0)){
			NBASSERT(unionCpos->indiceCuerpoA < this->hijos()->conteo)
			NBASSERT(unionCpos->indiceCuerpoB < this->hijos()->conteo)
			AUEscenaObjeto* objCpoA		= this; if(unionCpos->indiceCuerpoA >= 0) objCpoA = (AUEscenaObjeto*)this->hijos()->elemento[unionCpos->indiceCuerpoA];
			AUEscenaObjeto* objCpoB		= this; if(unionCpos->indiceCuerpoB >= 0) objCpoB = (AUEscenaObjeto*)this->hijos()->elemento[unionCpos->indiceCuerpoB];
			//Ignorar uniones entre objetos no visibles (posiblemente los aleatorios sin suerte)
			if(objCpoA->visible() && objCpoB->visible()){
				b2Body* b2CpoA			= NULL;
				b2Body* b2CpoB			= NULL;
				if(objCpoA->esClase(AUEscenaCuerpoI::idTipoClase)){
					b2CpoA	= (b2Body*)((AUEscenaCuerpoI*)objCpoA)->cuerpo();
				} else if(objCpoA->esClase(AUEscenaAnimacionI::idTipoClase)){
					b2CpoA	= (b2Body*)((AUEscenaAnimacionI*)objCpoA)->cuerpo();
				} else { NBASSERT(false) }
				if(objCpoB->esClase(AUEscenaCuerpoI::idTipoClase)){
					b2CpoB	= (b2Body*)((AUEscenaCuerpoI*)objCpoB)->cuerpo();
				} else if(objCpoB->esClase(AUEscenaAnimacionI::idTipoClase)){
					b2CpoB	= (b2Body*)((AUEscenaAnimacionI*)objCpoB)->cuerpo();
				} else { NBASSERT(false) }
				if(b2CpoA != NULL && b2CpoB != NULL){
					((b2JointDef*)unionCpos->b2Definicion)->userData	= NULL; //Dato para el escuchador del mundoFisica
					((b2JointDef*)unionCpos->b2Definicion)->bodyA		= b2CpoA;
					((b2JointDef*)unionCpos->b2Definicion)->bodyB		= b2CpoB;
					b2Joint* objUnion					= NBGestorFisica::crearUnionCuerpos(_idMundoFisica, (b2JointDef*)unionCpos->b2Definicion);
					AUEscenaObjeto* objEscenaUnion		= NULL;
					//Crear representaion grafica
					//Union cuerda
					if(unionCpos->tipoUnion==ENUnionCuerpoTipo_Cuerda){
						STUnionCuerposCuerdaDef* datosCuerda = (STUnionCuerposCuerdaDef*)unionCpos->estructuraAdicional; NBASSERT(datosCuerda != NULL)
						AUTextura* texCuerda = datosCuerda->texturaCuerda; NBASSERT(texCuerda != NULL)
						float longitudCuerda = (((b2RopeJointDef*)unionCpos->b2Definicion)->maxLength*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA) - 8.0f; //-8px para tensar un poco la cuerda
						if(longitudCuerda<8.0f) longitudCuerda = 8.0f;
						AUEscenaCuerda* cuerdaEscena = new(this) AUEscenaCuerda(datosCuerda->tipoCuerda, texCuerda, longitudCuerda, datosCuerda->masaSegmentos, datosCuerda->superposicionSegmentos);
						cuerdaEscena->establecerFactorEstiramiento(datosCuerda->factorEstiramiento);
						cuerdaEscena->establecerUnionCuerda((b2RopeJoint*)objUnion);
						objEscenaUnion = cuerdaEscena;
						this->agregarObjetoEscena(cuerdaEscena);
						cuerdaEscena->liberar(NB_RETENEDOR_THIS);
					}
					//Enlazar representacion grafica
					objUnion->SetUserData(objEscenaUnion); //Dato para el escuchador del mundoFisica
				}
			}
		}
	}
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpo::destruirCuerpo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::destruirCuerpo")
	if(_idMundoFisica!=-1){
		//Destruir cuerpo (automaticamente se destruyen las uniones)
		if(_cuerpo != NULL){
#			ifndef CONFIG_NB_UNSUPPORT_BOX2D
			NBGestorFisica::quitarUnion(this, (b2Body*)_cuerpo);
			NBGestorFisica::destruirCuerpo(_idMundoFisica, (b2Body*)_cuerpo);
#			endif
			_cuerpo = NULL;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaCuerpo::establecerGrupoAnimacion(ENGestorAnimadoresGrupo grupoAnimacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::establecerGrupoAnimacion")
    int iHijo; AUArreglo* hijos = this->hijos();
	for(iHijo=0; iHijo<hijos->conteo; iHijo++){
		AUEscenaObjeto* hijo = (AUEscenaObjeto*)hijos->elemento[iHijo];
		if(hijo->esClase(AUEscenaAnimacionI::idTipoClase)){
			((AUEscenaAnimacionI*)hijo)->establecerGrupoAnimacion(grupoAnimacion);
		} else if(hijo->esClase(AUEscenaCuerpoI::idTipoClase)){
			((AUEscenaCuerpoI*)hijo)->establecerGrupoAnimacion(grupoAnimacion);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpo::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::agregadoEnEscena")
	NBASSERT(_cuerpo == NULL)
	if(_idMundoFisica!=-1 && _propsCreacionCuerpo.tipoCuerpo!=ENCuerpoTipo_Ninguno && _definicionCuerpo->fijaciones.conteo!=0){
		this->crearCuerpo();
		NBASSERT(_cuerpo != NULL)
	}
	AUEscenaContenedor::agregadoEnEscena();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpo::agregadoEnEscenaConHijosNotificados(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::agregadoEnEscenaConHijosNotificados")
	if(_idMundoFisica!=-1 && _propsCreacionCuerpo.tipoCuerpo!=ENCuerpoTipo_Ninguno && _definicionCuerpo->fijaciones.conteo!=0){
		NBASSERT(_cuerpo != NULL)
		this->crearUnionesCuerpo();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaCuerpo::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaCuerpo::quitandoDeEscena")
	AUEscenaContenedor::quitandoDeEscena();
	if(_idMundoFisica!=-1 && _cuerpo != NULL){
		this->destruirCuerpo();
	}
	NBASSERT(_cuerpo == NULL)
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaCuerpo, AUEscenaCuerpoI)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaCuerpo, "AUEscenaCuerpo", AUEscenaCuerpoI)
AUOBJMETODOS_CLONAR_THIS(AUEscenaCuerpo)








