//
//  AUEscenaGeneradorParticulas.cpp
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaGeneradorParticulas.h"

AUEscenaGeneradorParticulas::AUEscenaGeneradorParticulas() : AUEscenaObjeto(), _datosParticulas(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::AUEscenaGeneradorParticulas")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaGeneradorParticulas")
	_grupoParticulas	= NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaGeneradorParticulas::AUEscenaGeneradorParticulas(AUEscenaGeneradorParticulas* otraInstancia) : AUEscenaObjeto(), _datosParticulas(this, &otraInstancia->_datosParticulas) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::AUEscenaGeneradorParticulas")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaGeneradorParticulas")
	this->copiarPropiedadesDe(otraInstancia);
	_grupoParticulas		= NULL;
	//retener todos los punteros de texturas
	UI32 iPart, iPartConteo = _datosParticulas.conteo;
	for(iPart=0; iPart<iPartConteo; iPart++){
		_datosParticulas.elemento[iPart].textura->retener(NB_RETENEDOR_THIS);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaGeneradorParticulas::~AUEscenaGeneradorParticulas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::~AUEscenaGeneradorParticulas")
	if(_grupoParticulas != NULL) _grupoParticulas->liberar(NB_RETENEDOR_THIS); _grupoParticulas = NULL;
	//liberar todos los punteros de texturas
	UI32 iPart, iPartConteo = _datosParticulas.conteo;
	for(iPart=0; iPart<iPartConteo; iPart++){
		_datosParticulas.elemento[iPart].textura->liberar(NB_RETENEDOR_THIS);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaGrupoParticulas* AUEscenaGeneradorParticulas::grupoParticulas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::grupoParticulas")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _grupoParticulas;
}

void AUEscenaGeneradorParticulas::establecerGrupoParticulas(AUEscenaGrupoParticulas* grupoParticulas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::establecerGrupoParticulas")
	if(grupoParticulas != NULL) grupoParticulas->retener(NB_RETENEDOR_THIS);
	if(_grupoParticulas != NULL) _grupoParticulas->liberar(NB_RETENEDOR_THIS);
	_grupoParticulas = grupoParticulas;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGeneradorParticulas::agregarParticulaLibreGenerar(AUTextura* textura, SI16 particsPorSeg, float probabilidadUsarEsta, float segsVive, float segsDesaparece, float escalaInicial, float velLanza, float margenVelLanza, float margenRadianes){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::agregarParticulaLibreGenerar")
	NBASSERT(textura != NULL)
	NBASSERT(particsPorSeg>0 && particsPorSeg<255)
	NBASSERT(probabilidadUsarEsta>0.0f)
	STParticulaGen datosGen;
	datosGen.tipoParticula						= ENParticulaTipo_Libre;
	datosGen.textura							= textura; textura->retener(NB_RETENEDOR_THIS);
	datosGen.particulasPorSeg					= particsPorSeg;
	datosGen.probabilidadUsarEsta				= probabilidadUsarEsta;
	datosGen.segundosVive						= segsVive;
	datosGen.segundosDesaparece					= segsDesaparece;
	datosGen.escalaInicial						= escalaInicial;
	datosGen.velocidadBaseLanza					= velLanza;
	datosGen.absDeltaVelocidadLanzamiento		= margenVelLanza;
	datosGen.absDeltaRadianesLanzamiento		= margenRadianes;
	_datosParticulas.agregarElemento(datosGen);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGeneradorParticulas::agregarParticulaHojaGenerar(AUTextura* textura, SI16 particsPorSeg, float probabilidadUsarEsta, float segsVive, float segsDesaparece, float escalaInicial, float velLanza, float margenVelLanza, float margenRadianes, float factorResistenciaAire, float factorTrayectoriaIrregular){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::agregarParticulaHojaGenerar")
	NBASSERT(textura != NULL)
	NBASSERT(particsPorSeg>0 && particsPorSeg<255)
	NBASSERT(probabilidadUsarEsta>0.0f)
	STParticulaGen datosGen;
	datosGen.tipoParticula						= ENParticulaTipo_Hojas;
	datosGen.textura							= textura; textura->retener(NB_RETENEDOR_THIS);
	datosGen.particulasPorSeg					= particsPorSeg;
	datosGen.probabilidadUsarEsta				= probabilidadUsarEsta;
	datosGen.segundosVive						= segsVive;
	datosGen.segundosDesaparece					= segsDesaparece;
	datosGen.escalaInicial						= escalaInicial;
	datosGen.velocidadBaseLanza					= velLanza;
	datosGen.absDeltaVelocidadLanzamiento		= margenVelLanza;
	datosGen.absDeltaRadianesLanzamiento		= margenRadianes;
	datosGen.factorResistenciaAire				= factorResistenciaAire;
	datosGen.factorTrayectoriaIrregular			= factorTrayectoriaIrregular;
	_datosParticulas.agregarElemento(datosGen);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGeneradorParticulas::agregarParticulaGotaGenerar(AUTextura* textura, SI16 particsPorSeg, float probabilidadUsarEsta, float segsVive, float segsDesaparece, float escalaInicial, float velLanza, float margenVelLanza, float margenRadianes, float segsParaConsultarColision){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::agregarParticulaGotaGenerar")
	NBASSERT(textura != NULL)
	NBASSERT(particsPorSeg>0 && particsPorSeg<255)
	NBASSERT(probabilidadUsarEsta>0.0f)
	STParticulaGen datosGen;
	datosGen.tipoParticula						= ENParticulaTipo_Gotas;
	datosGen.textura							= textura; textura->retener(NB_RETENEDOR_THIS);
	datosGen.particulasPorSeg					= particsPorSeg;
	datosGen.probabilidadUsarEsta				= probabilidadUsarEsta;
	datosGen.segundosVive						= segsVive;
	datosGen.segundosDesaparece					= segsDesaparece;
	datosGen.escalaInicial						= escalaInicial;
	datosGen.velocidadBaseLanza					= velLanza;
	datosGen.absDeltaVelocidadLanzamiento		= margenVelLanza;
	datosGen.absDeltaRadianesLanzamiento		= margenRadianes;
	datosGen.segsParaConsultarColision			= segsParaConsultarColision;
	_datosParticulas.agregarElemento(datosGen);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGeneradorParticulas::agregarParticulaHumoGenerar(AUTextura* textura, SI16 particsPorSeg, float probabilidadUsarEsta, float segsVive, float escalaInicial, float velLanza, float margenVelLanza, float margenRadianes, float escalaDisipacion, float factorResistenciaAire){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::agregarParticulaHumoGenerar")
	NBASSERT(textura != NULL)
	NBASSERT(particsPorSeg>0 && particsPorSeg<255)
	NBASSERT(probabilidadUsarEsta>0.0f)
	STParticulaGen datosGen;
	datosGen.tipoParticula						= ENParticulaTipo_Humo;
	datosGen.textura							= textura; textura->retener(NB_RETENEDOR_THIS);
	datosGen.particulasPorSeg					= particsPorSeg;
	datosGen.probabilidadUsarEsta				= probabilidadUsarEsta;
	datosGen.segundosVive						= segsVive;
	datosGen.escalaInicial						= escalaInicial;
	datosGen.velocidadBaseLanza					= velLanza;
	datosGen.absDeltaVelocidadLanzamiento		= margenVelLanza;
	datosGen.absDeltaRadianesLanzamiento		= margenRadianes;
	datosGen.escalarParaDisipar					= escalaDisipacion;
	datosGen.factorResistenciaAire				= factorResistenciaAire;
	_datosParticulas.agregarElemento(datosGen);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGeneradorParticulas::agregarParticulaInmovilGenerar(AUTextura* textura, SI16 particsPorSeg, float probabilidadUsarEsta, float segsVive, float segsDesaparece, float escalaInicial, float escalaDisipacion, float margenRadianes){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::agregarParticulaInmovilGenerar")
	NBASSERT(textura != NULL)
	NBASSERT(particsPorSeg>0 && particsPorSeg<255)
	NBASSERT(probabilidadUsarEsta>0.0f)
	STParticulaGen datosGen;
	datosGen.tipoParticula						= ENParticulaTipo_Inmovil;
	datosGen.textura							= textura; textura->retener(NB_RETENEDOR_THIS);
	datosGen.particulasPorSeg					= particsPorSeg;
	datosGen.probabilidadUsarEsta				= probabilidadUsarEsta;
	datosGen.segundosVive						= segsVive;
	datosGen.segundosDesaparece					= segsDesaparece;
	datosGen.escalaInicial						= escalaInicial;
	datosGen.escalarParaDisipar					= escalaDisipacion;
	datosGen.velocidadBaseLanza					= 0.0f;
	datosGen.absDeltaVelocidadLanzamiento		= 0.0f;
	datosGen.absDeltaRadianesLanzamiento		= margenRadianes;
	_datosParticulas.agregarElemento(datosGen);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGeneradorParticulas::actualizarParticulaGenerarEnIndice(UI32 iParticulaGen, const STParticulaGen &datosParticula){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::actualizarParticulaGenerarEnIndice")
	NBASSERT(iParticulaGen<_datosParticulas.conteo)
	NBASSERT(datosParticula.textura != NULL)
	NBASSERT(datosParticula.tipoParticula<ENParticulaTipo_Conteo)
	NBASSERT(datosParticula.particulasPorSeg>0 && datosParticula.particulasPorSeg<255)
	NBASSERT(datosParticula.probabilidadUsarEsta>0.0f)
	datosParticula.textura->retener(NB_RETENEDOR_THIS);
	_datosParticulas.elemento[iParticulaGen].textura->liberar(NB_RETENEDOR_THIS);
	_datosParticulas.elemento[iParticulaGen] = datosParticula;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGeneradorParticulas::quitarParticulaGenerarEnIndice(UI32 iParticulaGen){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::quitarParticulaGenerarEnIndice")
	NBASSERT(iParticulaGen<_datosParticulas.conteo)
	_datosParticulas.elemento[iParticulaGen].textura->liberar(NB_RETENEDOR_THIS);
	_datosParticulas.quitarElementoEnIndice(iParticulaGen);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

const STParticulaGen AUEscenaGeneradorParticulas::datosParticulaEnIndice(UI32 iParticulaGen){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::datosParticulaEnIndice")
	NBASSERT(iParticulaGen<_datosParticulas.conteo)
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _datosParticulas.elemento[iParticulaGen];
}

AUArregloNativoP<STParticulaGen>* AUEscenaGeneradorParticulas::datosParticulas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::datosParticulas")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return &_datosParticulas;
}

//

void AUEscenaGeneradorParticulas::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGeneradorParticulas::tickAnimacion")	
	if(this->idEscena!=-1 && !this->_dormido && this->_visible){
		float proporcionAlpha	= (float)this->_propiedades.color8.a / 255.0f;
		if(proporcionAlpha>0.0f){
			AUEscenaGrupoParticulas* agrupadorParticulas = _grupoParticulas;
			if(agrupadorParticulas == NULL){
				//Determinar la capa en la cual agregar las particulas (la primera en la lista o bien la ultima de mi capa actual)
				AUArreglo* agrupadoresParticulas = NBGestorEscena::agrupadoresParticulas(this->idEscena);
				if(agrupadoresParticulas->conteo!=0){
					agrupadorParticulas = (AUEscenaGrupoParticulas*)agrupadoresParticulas->elemento[0];
					UI32 iGrupoParts, iGrupoPartsConteo = agrupadoresParticulas->conteo;
					for(iGrupoParts=1; iGrupoParts<iGrupoPartsConteo; iGrupoParts++){
						if(((AUEscenaGrupoParticulas*)agrupadoresParticulas->elemento[iGrupoParts])->idCapa==this->idCapa){
							agrupadorParticulas = (AUEscenaGrupoParticulas*)agrupadoresParticulas->elemento[iGrupoParts];
						}
					}
				}
			}
			//
			if(agrupadorParticulas != NULL){
				float segundoVirtual	= NBGestorAnimadores::segundosVirtualesAcumulados();
				NBPunto posEscena;		NBMATRIZ_MULTIPLICAR_PUNTO(posEscena, _cacheObjEscena.matrizEscena, 0.0f, 0.0f);
				NBPunto posEscenaApunta; NBMATRIZ_MULTIPLICAR_PUNTO(posEscenaApunta, _cacheObjEscena.matrizEscena, 100.0f, 0.0f);
				float radianesApunta;	NBPUNTO_RADIANES_VECTOR(radianesApunta, posEscena.x, posEscena.y, posEscenaApunta.x, posEscenaApunta.y)
				UI32 iPart, iPartConteo = _datosParticulas.conteo;
				for(iPart=0; iPart<iPartConteo; iPart++){
					STParticulaGen* datosPart	= &_datosParticulas.elemento[iPart];
					float segSiguienteLanza		= datosPart->segundoVirtualUltimoLanzamiento+((1.0f/(float)datosPart->particulasPorSeg)/proporcionAlpha);
					if(segundoVirtual>=segSiguienteLanza){
						float factorDesvioRadianes	= datosPart->absDeltaRadianesLanzamiento * ((float)(rand() % 50) / 100.0f) * ((rand()%2)==0?1.0f:-1.0f);
						float factorCambioVelocidad = datosPart->absDeltaVelocidadLanzamiento * ((float)(rand() % 50) / 100.0f) * ((rand()%2)==0?1.0f:-1.0f);
						float radianesDesviado		= radianesApunta + (PIx2*factorDesvioRadianes);
						float velAfectada			= datosPart->velocidadBaseLanza+(datosPart->velocidadBaseLanza*factorCambioVelocidad);
						NBPunto vectorDireccion;
						NBPUNTO_ESTABLECER(vectorDireccion, velAfectada, 0.0f)
						NBPUNTO_ROTAR_RADIANES(vectorDireccion, vectorDireccion, radianesDesviado);
						if(datosPart->tipoParticula==ENParticulaTipo_Libre){
							agrupadorParticulas->agregarParticulaLibre(datosPart->textura, datosPart->segundosVive, datosPart->segundosDesaparece, posEscena.x, posEscena.y, datosPart->escalaInicial, GRADOS_A_RADIANES(this->rotacion()), vectorDireccion.x, vectorDireccion.y, 0.0f);
						} else if(datosPart->tipoParticula==ENParticulaTipo_Hojas){
							agrupadorParticulas->agregarParticulaHoja(datosPart->textura,  datosPart->segundosVive, datosPart->segundosDesaparece, posEscena.x, posEscena.y, datosPart->escalaInicial, GRADOS_A_RADIANES(this->rotacion()), vectorDireccion.x, vectorDireccion.y, 0.0f, datosPart->factorResistenciaAire, datosPart->factorTrayectoriaIrregular);
						} else if(datosPart->tipoParticula==ENParticulaTipo_Gotas){
							agrupadorParticulas->agregarParticulaGota(datosPart->textura, datosPart->textura, datosPart->segundosDesaparece, posEscena.x, posEscena.y, datosPart->escalaInicial, vectorDireccion.x, vectorDireccion.y, datosPart->segsParaConsultarColision);
						} else if(datosPart->tipoParticula==ENParticulaTipo_Humo){
							agrupadorParticulas->agregarParticulaHumo(datosPart->textura,  datosPart->segundosVive, posEscena.x, posEscena.y, datosPart->escalaInicial, GRADOS_A_RADIANES(this->rotacion()), vectorDireccion.x, vectorDireccion.y, 0.0f, datosPart->escalarParaDisipar, datosPart->factorResistenciaAire);
						} else if(datosPart->tipoParticula==ENParticulaTipo_Inmovil){
							agrupadorParticulas->agregarParticulaInmovil(datosPart->textura, datosPart->segundosVive, datosPart->segundosDesaparece, posEscena.x, posEscena.y, datosPart->escalaInicial, GRADOS_A_RADIANES(this->rotacion()), datosPart->escalarParaDisipar);
						} else {
							NBASSERT(false) //Tipo de particula no valido
						}
						datosPart->segundoVirtualUltimoLanzamiento = segundoVirtual;
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaGeneradorParticulas::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::agregadoEnEscena")
	NBGestorAnimadores::agregarAnimador(this, this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGeneradorParticulas::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGeneradorParticulas::quitandoDeEscena")
	NBGestorAnimadores::quitarAnimador(this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

NBCajaAABB AUEscenaGeneradorParticulas::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::cajaAABBLocalCalculada")
	NBCajaAABB cajaLocal; NBCAJAAABB_INICIALIZAR(cajaLocal)
	cajaLocal.xMin = -16.0f;
	cajaLocal.yMin = -16.0f;
	cajaLocal.xMax = 16.0f;
	cajaLocal.yMax = 16.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return cajaLocal;
}

PTRfunCmdsGL AUEscenaGeneradorParticulas::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGeneradorParticulas::actualizarModeloGL")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return NULL;
}

//

bool AUEscenaGeneradorParticulas::agregarXmlInternoEn(AUEscenaGeneradorParticulas* generador, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGeneradorParticulas::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(generador, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	//
	guardarEn->agregarConFormato("%s<particulas>\r\n", espaciosBaseIzq);
	UI32 iPart, iPartConteo = generador->_datosParticulas.conteo;
	for(iPart=0; iPart<iPartConteo; iPart++){
		STParticulaGen* datosPart = &generador->_datosParticulas.elemento[iPart];
		guardarEn->agregarConFormato("%s <p><t>%d</t><pps>%d</pps><prob>%f</prob><segs>%f</segs><segsD>%f</segsD><vel>%f</vel><mvel>%f</mvel><mrad>%f</mrad><fresist>%f</fresist><ftrayI>%f</ftrayI><segsCol>%f</segsCol><esc>%f</esc><escD>%f</escD><tex>%s</tex></p>\r\n", espaciosBaseIzq, datosPart->tipoParticula, datosPart->particulasPorSeg, datosPart->probabilidadUsarEsta, datosPart->segundosVive, datosPart->segundosDesaparece, datosPart->velocidadBaseLanza, datosPart->absDeltaVelocidadLanzamiento, datosPart->absDeltaRadianesLanzamiento, datosPart->factorResistenciaAire, datosPart->factorTrayectoriaIrregular, datosPart->segsParaConsultarColision, datosPart->escalaInicial, datosPart->escalarParaDisipar, NBGestorTexturas::nombreRecursoTextura(datosPart->textura));
	}
	guardarEn->agregarConFormato("%s</particulas>\r\n", espaciosBaseIzq);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
	
}

bool AUEscenaGeneradorParticulas::interpretarXmlInterno(AUEscenaGeneradorParticulas* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGeneradorParticulas::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
		const sNodoXML* nodoEscenaObj;	XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
		if(nodoEscenaObj != NULL){
			exito = AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj);
			//
			AUCadenaMutable8* nomTextura = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
			const sNodoXML* nodoParts	= datosXml->nodoHijo("particulas", nodoXml);
			if(nodoParts != NULL){
				const sNodoXML* nodoP	= datosXml->nodoHijo("p", nodoParts);
				while(nodoP != NULL){
					nomTextura->vaciar(); datosXml->nodoHijo("tex", nomTextura, "", nodoP);
					if(nomTextura->tamano()!=0){
						AUTextura* textura	= NBGestorTexturas::texturaDesdeArchivo(nomTextura->str());
						if(textura != NULL){
							ENParticulaTipo tipo = (ENParticulaTipo)datosXml->nodoHijo("t", -1, nodoP);
							if(tipo==ENParticulaTipo_Libre){
								cargarEn->agregarParticulaLibreGenerar(textura, datosXml->nodoHijo("pps", 1, nodoP), datosXml->nodoHijo("prob", 1.0f, nodoP), datosXml->nodoHijo("segs", 8.0f, nodoP), datosXml->nodoHijo("segsD", 0.25f, nodoP), datosXml->nodoHijo("esc", 1.0f, nodoP), datosXml->nodoHijo("vel", 256.0f, nodoP), datosXml->nodoHijo("mvel", 0.0f, nodoP), datosXml->nodoHijo("mrad", 0.0f, nodoP));
							} else if(tipo==ENParticulaTipo_Hojas){
								cargarEn->agregarParticulaHojaGenerar(textura, datosXml->nodoHijo("pps", 1, nodoP), datosXml->nodoHijo("prob", 1.0f, nodoP), datosXml->nodoHijo("segs", 8.0f, nodoP), datosXml->nodoHijo("segsD", 0.25f, nodoP), datosXml->nodoHijo("esc", 1.0f, nodoP), datosXml->nodoHijo("vel", 256.0f, nodoP), datosXml->nodoHijo("mvel", 0.0f, nodoP), datosXml->nodoHijo("mrad", 0.0f, nodoP), datosXml->nodoHijo("fresist", 0.0f, nodoP), datosXml->nodoHijo("ftrayI", 0.0f, nodoP));
							} else if(tipo==ENParticulaTipo_Gotas){
								cargarEn->agregarParticulaGotaGenerar(textura, datosXml->nodoHijo("pps", 1, nodoP), datosXml->nodoHijo("prob", 1.0f, nodoP), datosXml->nodoHijo("segs", 8.0f, nodoP), datosXml->nodoHijo("segsD", 0.25f, nodoP), datosXml->nodoHijo("esc", 1.0f, nodoP), datosXml->nodoHijo("vel", 256.0f, nodoP), datosXml->nodoHijo("mvel", 0.0f, nodoP), datosXml->nodoHijo("mrad", 0.0f, nodoP), datosXml->nodoHijo("segsCol", 0.15f, nodoP));
							} else if(tipo==ENParticulaTipo_Humo){
								cargarEn->agregarParticulaHumoGenerar(textura, datosXml->nodoHijo("pps", 1, nodoP), datosXml->nodoHijo("prob", 1.0f, nodoP), datosXml->nodoHijo("segs", 8.0f, nodoP), datosXml->nodoHijo("esc", 1.0f, nodoP), datosXml->nodoHijo("vel", 256.0f, nodoP), datosXml->nodoHijo("mvel", 0.0f, nodoP), datosXml->nodoHijo("mrad", 0.0f, nodoP), datosXml->nodoHijo("escD", 3.0f, nodoP), datosXml->nodoHijo("fresist", 0.0f, nodoP));
							} else if(tipo==ENParticulaTipo_Inmovil){
								cargarEn->agregarParticulaInmovilGenerar(textura, datosXml->nodoHijo("pps", 1, nodoP), datosXml->nodoHijo("prob", 1.0f, nodoP), datosXml->nodoHijo("segs", 8.0f, nodoP), datosXml->nodoHijo("segsD", 0.25f, nodoP), datosXml->nodoHijo("esc", 1.0f, nodoP), datosXml->nodoHijo("escD", 3.0f, nodoP), datosXml->nodoHijo("mrad", 0.0f, nodoP));
							} else {
								NBASSERT(false); //tipo de particula no soportado
							}
						}
					}
					nodoP	= datosXml->nodoHijo("p", nodoParts, nodoP);
				}
			}
			nomTextura->liberar(NB_RETENEDOR_NULL);
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaGeneradorParticulas::agregarBitsInternosEn(AUEscenaGeneradorParticulas* generador, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGeneradorParticulas::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(generador, guardarEn)){
		SI32 conteoParticulas = generador->_datosParticulas.conteo;
		guardarEn->escribir(&conteoParticulas, sizeof(conteoParticulas), 1, guardarEn);
		UI32 iPart, iPartConteo = generador->_datosParticulas.conteo;
		for(iPart=0; iPart<iPartConteo; iPart++){
			STParticulaGen* datosPart	= &generador->_datosParticulas.elemento[iPart];
			const char* nombreTextura	= NULL;
			SI32 tamanoNombreTextura	= 0;
			nombreTextura				= NBGestorTexturas::nombreRecursoTextura(datosPart->textura);
			if(nombreTextura == NULL){
				guardarEn->escribir(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, guardarEn);
			} else {
				tamanoNombreTextura = AUCadenaMutable8::tamano(nombreTextura);
				guardarEn->escribir(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, guardarEn);
				guardarEn->escribir(nombreTextura, sizeof(char), tamanoNombreTextura, guardarEn);
			}
			guardarEn->escribir(&datosPart->tipoParticula, sizeof(datosPart->tipoParticula), 1, guardarEn);
			guardarEn->escribir(&datosPart->particulasPorSeg, sizeof(datosPart->particulasPorSeg), 1, guardarEn);
			guardarEn->escribir(&datosPart->probabilidadUsarEsta, sizeof(datosPart->probabilidadUsarEsta), 1, guardarEn);
			guardarEn->escribir(&datosPart->segundosVive, sizeof(datosPart->segundosVive), 1, guardarEn);
			guardarEn->escribir(&datosPart->segundosDesaparece, sizeof(datosPart->segundosDesaparece), 1, guardarEn);
			guardarEn->escribir(&datosPart->escalaInicial, sizeof(datosPart->escalaInicial), 1, guardarEn);
			guardarEn->escribir(&datosPart->velocidadBaseLanza, sizeof(datosPart->velocidadBaseLanza), 1, guardarEn);
			guardarEn->escribir(&datosPart->absDeltaVelocidadLanzamiento, sizeof(datosPart->absDeltaVelocidadLanzamiento), 1, guardarEn);
			guardarEn->escribir(&datosPart->absDeltaRadianesLanzamiento, sizeof(datosPart->absDeltaRadianesLanzamiento), 1, guardarEn);
			//Propias de hojas
			guardarEn->escribir(&datosPart->factorResistenciaAire, sizeof(datosPart->factorResistenciaAire), 1, guardarEn);
			guardarEn->escribir(&datosPart->factorTrayectoriaIrregular, sizeof(datosPart->factorTrayectoriaIrregular), 1, guardarEn);
			//Propias de gotas
			guardarEn->escribir(&datosPart->segsParaConsultarColision, sizeof(datosPart->segsParaConsultarColision), 1, guardarEn);
			//Propias de humo
			guardarEn->escribir(&datosPart->escalarParaDisipar, sizeof(datosPart->escalarParaDisipar), 1, guardarEn);
		}
		//
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaGeneradorParticulas::interpretarBitsInternos(AUEscenaGeneradorParticulas* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGeneradorParticulas::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			SI32 conteoParticulas; cargarDe->leer(&conteoParticulas, sizeof(conteoParticulas), 1, cargarDe);
			SI32 iPart;
			for(iPart=0; iPart<conteoParticulas; iPart++){
				SI32 tamanoNombreTextura; AUTextura* textura = NULL;
				cargarDe->leer(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, cargarDe);
				if(tamanoNombreTextura>0){
					char* nombreTextura = (char*)NBGestorMemoria::reservarMemoria(tamanoNombreTextura+1, ENMemoriaTipo_Temporal); NB_DEFINE_NOMBRE_PUNTERO(nombreTextura, "AUEscenaGeneradorParticulas::nombreTextura") //+1 es el "\0"
					cargarDe->leer(nombreTextura, sizeof(char), tamanoNombreTextura, cargarDe);
					nombreTextura[tamanoNombreTextura] = '\0';
					textura = NBGestorTexturas::texturaDesdeArchivo(nombreTextura);
					NBGestorMemoria::liberarMemoria(nombreTextura);
				}
				STParticulaGen datosPart;
				cargarDe->leer(&datosPart.tipoParticula, sizeof(datosPart.tipoParticula), 1, cargarDe);
				cargarDe->leer(&datosPart.particulasPorSeg, sizeof(datosPart.particulasPorSeg), 1, cargarDe);
				cargarDe->leer(&datosPart.probabilidadUsarEsta, sizeof(datosPart.probabilidadUsarEsta), 1, cargarDe);
				cargarDe->leer(&datosPart.segundosVive, sizeof(datosPart.segundosVive), 1, cargarDe);
				cargarDe->leer(&datosPart.segundosDesaparece, sizeof(datosPart.segundosDesaparece), 1, cargarDe);
				cargarDe->leer(&datosPart.escalaInicial, sizeof(datosPart.escalaInicial), 1, cargarDe);
				cargarDe->leer(&datosPart.velocidadBaseLanza, sizeof(datosPart.velocidadBaseLanza), 1, cargarDe);
				cargarDe->leer(&datosPart.absDeltaVelocidadLanzamiento, sizeof(datosPart.absDeltaVelocidadLanzamiento), 1, cargarDe);
				cargarDe->leer(&datosPart.absDeltaRadianesLanzamiento, sizeof(datosPart.absDeltaRadianesLanzamiento), 1, cargarDe);
				//propias de hojas
				cargarDe->leer(&datosPart.factorResistenciaAire, sizeof(datosPart.factorResistenciaAire), 1, cargarDe);
				cargarDe->leer(&datosPart.factorTrayectoriaIrregular, sizeof(datosPart.factorTrayectoriaIrregular), 1, cargarDe);
				//Propias de gotas
				cargarDe->leer(&datosPart.segsParaConsultarColision, sizeof(datosPart.segsParaConsultarColision), 1, cargarDe);
				//Propias de humo
				cargarDe->leer(&datosPart.escalarParaDisipar, sizeof(datosPart.escalarParaDisipar), 1, cargarDe);
				//
				if(textura != NULL){
					if(datosPart.tipoParticula==ENParticulaTipo_Libre){
						cargarEn->agregarParticulaLibreGenerar(textura, datosPart.particulasPorSeg, datosPart.probabilidadUsarEsta, datosPart.segundosVive, datosPart.segundosDesaparece, datosPart.escalaInicial, datosPart.velocidadBaseLanza, datosPart.absDeltaVelocidadLanzamiento, datosPart.absDeltaRadianesLanzamiento);
					} else if(datosPart.tipoParticula==ENParticulaTipo_Hojas){
						cargarEn->agregarParticulaHojaGenerar(textura, datosPart.particulasPorSeg, datosPart.probabilidadUsarEsta, datosPart.segundosVive, datosPart.segundosDesaparece, datosPart.escalaInicial, datosPart.velocidadBaseLanza, datosPart.absDeltaVelocidadLanzamiento, datosPart.absDeltaRadianesLanzamiento, datosPart.factorResistenciaAire, datosPart.factorTrayectoriaIrregular);
					} else if(datosPart.tipoParticula==ENParticulaTipo_Gotas){
						cargarEn->agregarParticulaGotaGenerar(textura, datosPart.particulasPorSeg, datosPart.probabilidadUsarEsta, datosPart.segundosVive, datosPart.segundosDesaparece, datosPart.escalaInicial, datosPart.velocidadBaseLanza, datosPart.absDeltaVelocidadLanzamiento, datosPart.absDeltaRadianesLanzamiento, datosPart.segsParaConsultarColision);
					} else if(datosPart.tipoParticula==ENParticulaTipo_Humo){
						cargarEn->agregarParticulaHumoGenerar(textura, datosPart.particulasPorSeg, datosPart.probabilidadUsarEsta, datosPart.segundosVive, datosPart.escalaInicial, datosPart.velocidadBaseLanza, datosPart.absDeltaVelocidadLanzamiento, datosPart.absDeltaRadianesLanzamiento, datosPart.escalarParaDisipar, datosPart.factorResistenciaAire);
					} else if(datosPart.tipoParticula==ENParticulaTipo_Inmovil){
						cargarEn->agregarParticulaInmovilGenerar(textura, datosPart.particulasPorSeg, datosPart.probabilidadUsarEsta, datosPart.segundosVive, datosPart.segundosDesaparece, datosPart.escalaInicial, datosPart.escalarParaDisipar, datosPart.absDeltaRadianesLanzamiento);
					} else {
						NBASSERT(false); //tipo de particula no soportado
					}
				}
			}
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUEscenaGeneradorParticulas, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaGeneradorParticulas, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaGeneradorParticulas, "AUEscenaGeneradorParticulas", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaGeneradorParticulas)






