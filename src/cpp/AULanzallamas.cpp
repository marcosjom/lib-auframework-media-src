//
//  AULanzallamas.cpp
//  EditorCuerpos
//
//  Created by Marcos Ortega on 14/12/12.
//
//

#include "AUFrameworkMediaStdAfx.h"
#include "AULanzallamas.h"

AULanzallamas::AULanzallamas() : AUEscenaContenedor(), _pasosLanzamiento(this){
	NB_DEFINE_NOMBRE_PUNTERO(this, "AULanzallamas")
	_iPasoActual			= 255;
	_segsEsperandoInicio	= 0.0f;
	_segsEsperaInicial		= 0.0f;
	_segsTotalPasoActual	= 0.0f;
	_segsAcumPasoActual		= 0.0f;
	_alcanceActual			= 0.0f;
	_alcanceDestino			= 0.0f;
	_tipoPresentacion		= ENLanzallamaPresenta_Completa;
	//
	_luzFuego				= new(this) AUEscenaLuzRadial();
	_luzFuego->establecerTipoLuz(ENFuenteIlumTipo_SoloMascara);
	_luzFuego->establecerProducirSombras(false);
	//_luzFuego->establecerAngulosMinMaxActivos(true);
	//_luzFuego->establecerRadianesMinimo((PI/-2.0f) + GRADOS_A_RADIANES(1.0f));
	//_luzFuego->establecerRadianesMaximo((PI/2.0f) - GRADOS_A_RADIANES(1.0f));
	_luzFuego->establecerAlcanceLuz(0.0f);
	this->agregarObjetoEscena(_luzFuego);
	_particulasFuego		= new(this) AUEscenaGrupoParticulasFuego();
	_particulasFuego->establecerParticulasSegsVida(AULANZALLAMAS_SEGS_VIDA_PARTICULAS_FUEGO);
	this->agregarObjetoEscena(_particulasFuego);
	_spriteBase = new(this) AUEscenaSprite(NBGestorTexturas::texturaDesdeArchivo("Escenarios/obsLanzallamas.png"), ENSpriteAlineaH_Izq, ENSpriteAlineaV_Cen);
	_spriteBase->establecerTraslacion(4.0f, 7.0f);
	this->agregarObjetoEscena(_spriteBase);
}

AULanzallamas::AULanzallamas(AULanzallamas* otraInstancia) : AUEscenaContenedor(), _pasosLanzamiento(this, &otraInstancia->_pasosLanzamiento){
	NB_DEFINE_NOMBRE_PUNTERO(this, "AULanzallamas")
	_iPasoActual			= 255;
	_segsEsperandoInicio	= 0.0f;
	_segsEsperaInicial		= otraInstancia->_segsEsperaInicial;
	_segsTotalPasoActual	= 0.0f;
	_segsAcumPasoActual		= 0.0f;
	_alcanceActual			= 0.0f;
	_alcanceDestino			= 0.0f;
	_tipoPresentacion		= otraInstancia->_tipoPresentacion;
	this->copiarPropiedadesDe(otraInstancia);
	//
	_luzFuego				= new(this) AUEscenaLuzRadial();
	_luzFuego->establecerTipoLuz(ENFuenteIlumTipo_SoloMascara);
	_luzFuego->establecerProducirSombras(false);
	//_luzFuego->establecerAngulosMinMaxActivos(true);
	//_luzFuego->establecerRadianesMinimo((PI/-2.0f) + GRADOS_A_RADIANES(1.0f));
	//_luzFuego->establecerRadianesMaximo((PI/2.0f) - GRADOS_A_RADIANES(1.0f));
	_luzFuego->establecerAlcanceLuz(0.0f);
	this->agregarObjetoEscena(_luzFuego);
	_particulasFuego		= new(this) AUEscenaGrupoParticulasFuego();
	_particulasFuego->establecerParticulasSegsVida(AULANZALLAMAS_SEGS_VIDA_PARTICULAS_FUEGO);
	this->agregarObjetoEscena(_particulasFuego);
	_spriteBase				= new(this) AUEscenaSprite(otraInstancia->_spriteBase);
	this->agregarObjetoEscena(_spriteBase);
}

AULanzallamas::~AULanzallamas(){
	if(_spriteBase != NULL) _spriteBase->liberar(NB_RETENEDOR_THIS); _spriteBase = NULL;
	if(_luzFuego != NULL) _luzFuego->liberar(NB_RETENEDOR_THIS); _luzFuego = NULL;
	if(_particulasFuego != NULL) _particulasFuego->liberar(NB_RETENEDOR_THIS); _particulasFuego = NULL;
}

//

ENLanzallamaPresenta AULanzallamas::tipoPrensentacion(){
	return (ENLanzallamaPresenta)_tipoPresentacion;
}

float AULanzallamas::segsEsperaInicial(){
	return _segsEsperaInicial;
}

AUArregloNativoP<STLanzallamaPaso>* AULanzallamas::pasosLanzamiento(){
	return &_pasosLanzamiento;
}

//

void AULanzallamas::establecerTipoPresentacion(ENLanzallamaPresenta tipoLanzallamas){
	NBASSERT(tipoLanzallamas>=0 && tipoLanzallamas<ENLanzallamaPresenta_Conteo)
	if(_tipoPresentacion!=tipoLanzallamas){
		_tipoPresentacion = tipoLanzallamas;
		if(_tipoPresentacion==ENLanzallamaPresenta_Completa){
			AUTextura* tex = NBGestorTexturas::texturaDesdeArchivo("Escenarios/obsLanzallamas.png");
			_spriteBase->redimensionar(tex, ENSpriteAlineaH_Izq, ENSpriteAlineaV_Cen);
			_spriteBase->establecerTextura(tex);
			_spriteBase->establecerTraslacion(4.0f, 7.0f);
		} else if(_tipoPresentacion==ENLanzallamaPresenta_Simple){
			AUTextura* tex = NBGestorTexturas::texturaDesdeArchivo("Escenarios/obsLanzallamasSimple.png");
			_spriteBase->redimensionar(tex, ENSpriteAlineaH_Izq, ENSpriteAlineaV_Cen);
			_spriteBase->establecerTextura(tex);
			_spriteBase->establecerTraslacion(4.0f, 0.0f);
		}
	}
}

void AULanzallamas::establecerSegsEsperaInicial(float segsEsperaInicial){
	_segsEsperaInicial = segsEsperaInicial;
}

void AULanzallamas::agregarPasoLanzamiento(float segsDuracion, float alcanceFuego){
	STLanzallamaPaso datosPaso;
	datosPaso.segsDuracion	= segsDuracion;
	datosPaso.alcanceFuego	= alcanceFuego;
	_pasosLanzamiento.agregarElemento(datosPaso);
}

void AULanzallamas::quitarPasoLanzamientoEnindice(UI16 indice){
	NBASSERT(indice<_pasosLanzamiento.conteo)
	_pasosLanzamiento.quitarElementoEnIndice(indice);
}

void AULanzallamas::moverPasoLanzamiento(UI16 indiceActual, UI16 indiceDestino){
	NBASSERT(indiceActual<_pasosLanzamiento.conteo && indiceDestino<_pasosLanzamiento.conteo)
	STLanzallamaPaso datosPaso = _pasosLanzamiento.elemento[indiceActual];
	_pasosLanzamiento.quitarElementoEnIndice(indiceActual);
	_pasosLanzamiento.agregarElementoEnIndice(datosPaso, indiceDestino);
}

//

void AULanzallamas::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULanzallamas::agregadoEnEscena")
	NBGestorAnimadores::agregarAnimador(this, this);
	NBGestorPersonajes::agregarVictimario(this, 0);
	AUEscenaContenedor::agregadoEnEscena();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULanzallamas::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULanzallamas::quitandoDeEscena")
	AUEscenaContenedor::quitandoDeEscena();
	NBGestorPersonajes::quitarVictimario(this);
	NBGestorAnimadores::quitarAnimador(this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool AULanzallamas::personajeDanoHaciaVictima(const NBCajaAABB &cajaEscenaVictima, STPersonajeDano* guardarDanoEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULanzallamas::personajeDanoHaciaVictima")
	bool existeDano = false;
	if(NBCAJAAABB_INTERSECTA_CAJA(_cacheObjEscena.cajaEscena, cajaEscenaVictima)){ //Caja que inclute al sprite y los rayos
		STParticulaFuegoLinea lineasFuego[2]; _particulasFuego->lineasParticulas(&lineasFuego[0], &lineasFuego[1]);
		UI16 iLinea;
		for(iLinea=0; iLinea<2; iLinea++){
			if(lineasFuego[iLinea].lineaEstablecida){
				NBPunto posIniEscena = lineasFuego[iLinea].posEscenaIni;
				NBPunto posFinEscena = lineasFuego[iLinea].posEscenaFin;
				NBPunto vecRayo; NBPUNTO_ESTABLECER(vecRayo, posFinEscena.x-posIniEscena.x, posFinEscena.y-posIniEscena.y)
				NBPunto vectorCaja[4];
				float productoCruz[4];
				NBPUNTO_ESTABLECER(vectorCaja[0], cajaEscenaVictima.xMin-posIniEscena.x, cajaEscenaVictima.yMin-posIniEscena.y)
				NBPUNTO_ESTABLECER(vectorCaja[1], cajaEscenaVictima.xMin-posIniEscena.x, cajaEscenaVictima.yMax-posIniEscena.y)
				NBPUNTO_ESTABLECER(vectorCaja[2], cajaEscenaVictima.xMax-posIniEscena.x, cajaEscenaVictima.yMin-posIniEscena.y)
				NBPUNTO_ESTABLECER(vectorCaja[3], cajaEscenaVictima.xMax-posIniEscena.x, cajaEscenaVictima.yMax-posIniEscena.y)
				productoCruz[0]	= NBPUNTO_PRODUCTO_VECTORIAL(vecRayo.x, vecRayo.y, vectorCaja[0].x, vectorCaja[0].y);
				productoCruz[1]	= NBPUNTO_PRODUCTO_VECTORIAL(vecRayo.x, vecRayo.y, vectorCaja[1].x, vectorCaja[1].y);
				productoCruz[2]	= NBPUNTO_PRODUCTO_VECTORIAL(vecRayo.x, vecRayo.y, vectorCaja[2].x, vectorCaja[2].y);
				productoCruz[3]	= NBPUNTO_PRODUCTO_VECTORIAL(vecRayo.x, vecRayo.y, vectorCaja[3].x, vectorCaja[3].y);
				if(!((productoCruz[0]<0.0f && productoCruz[1]<0.0f && productoCruz[2]<0.0f && productoCruz[3]<0.0f) || (productoCruz[0]>=0.0f && productoCruz[1]>=0.0f && productoCruz[2]>=0.0f && productoCruz[3]>=0.0f))){
					//La caja tiene algunos vertices a la izquierda y otros a la derecha del rayo (intersecta rayo infinito)
					float longitudRayo = NBPUNTO_DISTANCIA_VECTOR(vecRayo.x, vecRayo.y); NBASSERT(longitudRayo!=0.0f)
					NBPunto vecUnitRayo; NBPUNTO_ESTABLECER(vecUnitRayo, vecRayo.x/longitudRayo, vecRayo.y/longitudRayo)
					float productoPunto[4];
					productoPunto[0]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[0].x, vectorCaja[0].y);
					productoPunto[1]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[1].x, vectorCaja[1].y);
					productoPunto[2]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[2].x, vectorCaja[2].y);
					productoPunto[3]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[3].x, vectorCaja[3].y);
					if(!((productoPunto[0]<0.0f && productoPunto[1]<0.0f && productoPunto[2]<0.0f && productoPunto[3]<0.0f) || (productoPunto[0]>longitudRayo && productoPunto[1]>longitudRayo && productoPunto[2]>longitudRayo && productoPunto[3]>longitudRayo))){
						//La caja tiene por lo menos un vertice dentro del rayo
						existeDano = true;
						guardarDanoEn->factorDanoRecibido = 1.0f;
						float promedioPuntoCaja = (productoPunto[0] + productoPunto[1] + productoPunto[2] + productoPunto[3]) / 4.0f;
						guardarDanoEn->posEscenaOrigenDano.x = posIniEscena.x + (vecUnitRayo.x * promedioPuntoCaja);
						guardarDanoEn->posEscenaOrigenDano.y = posIniEscena.y + (vecUnitRayo.y * promedioPuntoCaja);
						break;
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return existeDano;
}

void AULanzallamas::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULanzallamas::tickAnimacion")
	if(this->_dormido){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3() }
	if(_pasosLanzamiento.conteo!=0){
		if(_segsEsperandoInicio<_segsEsperaInicial){
			_segsEsperandoInicio += segsTranscurridos;
		} else {
			//Animar el alcance
			if(_alcanceActual!=_alcanceDestino){
				if(_alcanceActual<_alcanceDestino){
					_alcanceActual += (segsTranscurridos * 4096.0f);
					if(_alcanceActual>_alcanceDestino) _alcanceActual = _alcanceDestino;
				} else {
					_alcanceActual -= (segsTranscurridos * 4096.0f);
					if(_alcanceActual<_alcanceDestino) _alcanceActual = _alcanceDestino;
				}
				float particulasPorSegundo	= (AULANZALLAMAS_MAX_PARTICULAS_FUEGO * _alcanceActual / 256.0f); if(particulasPorSegundo>AULANZALLAMAS_MAX_PARTICULAS_FUEGO) particulasPorSegundo = AULANZALLAMAS_MAX_PARTICULAS_FUEGO;
				_particulasFuego->establecerAlcanceParticulas(_alcanceActual);
				_particulasFuego->establecerParticulasPorSegundo(particulasPorSegundo);
			}
			//Animar luz
			if(_particulasFuego->conteoParticulasFuegoVivas()!=0){
				NBCajaAABB cajaEscenaFuego		= _particulasFuego->cajaAABBEnEscena();
				NBPunto centroEscenaFuego;		NBCAJAAABB_CENTRO(centroEscenaFuego, cajaEscenaFuego)
				NBTamano tamEscenaFuego;		NBCAJAAABB_TAMANO(tamEscenaFuego, cajaEscenaFuego)
				_luzFuego->establecerAlcanceLuz((tamEscenaFuego.ancho>tamEscenaFuego.alto?tamEscenaFuego.ancho:tamEscenaFuego.alto) * 0.80f);
				NBMatriz matrizInv;				NBMATRIZ_INVERSA(matrizInv, _cacheObjEscena.matrizEscena)
				NBPunto centroLocalFuego;		NBMATRIZ_MULTIPLICAR_PUNTO(centroLocalFuego, matrizInv, centroEscenaFuego.x, centroEscenaFuego.y)
				_luzFuego->establecerTraslacion(centroLocalFuego);
			} else {
				_luzFuego->establecerAlcanceLuz(0.0f);
			}
			//Animar pasos
			_segsAcumPasoActual		+= segsTranscurridos;
			if(_segsAcumPasoActual>_segsTotalPasoActual){
				_segsAcumPasoActual = 0.0f;
				_iPasoActual++; if(_iPasoActual>=_pasosLanzamiento.conteo) _iPasoActual = 0;
				STLanzallamaPaso* pasoNuevo = &_pasosLanzamiento.elemento[_iPasoActual];
				_segsTotalPasoActual	= pasoNuevo->segsDuracion;
				_alcanceDestino			= pasoNuevo->alcanceFuego;
				_particulasFuego->activarSiguienteLineaParticulas();
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

bool AULanzallamas::agregarXmlInternoEn(AULanzallamas* lanzallamas, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULanzallamas::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(lanzallamas, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<segsIni>%f</segsIni>\r\n", espaciosBaseIzq, lanzallamas->_segsEsperaInicial);
	guardarEn->agregarConFormato("%s<tipP>%d</tipP>\r\n", espaciosBaseIzq, lanzallamas->_tipoPresentacion);
	guardarEn->agregarConFormato("%s<pasos>\r\n", espaciosBaseIzq);
	UI32 iCmd, iCmdConteo = lanzallamas->_pasosLanzamiento.conteo;
	for(iCmd=0; iCmd<iCmdConteo; iCmd++){
		STLanzallamaPaso* paso		= &(lanzallamas->_pasosLanzamiento.elemento[iCmd]);
		guardarEn->agregarConFormato("%s <paso>\r\n", espaciosBaseIzq);
		guardarEn->agregarConFormato("%s  <segsDur>%f</segsDur>\r\n", espaciosBaseIzq, paso->segsDuracion);
		guardarEn->agregarConFormato("%s  <alcF>%f</alcF>\r\n", espaciosBaseIzq, paso->alcanceFuego);
		guardarEn->agregarConFormato("%s </paso>\r\n", espaciosBaseIzq);
	}
	guardarEn->agregarConFormato("%s</pasos>\r\n", espaciosBaseIzq);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return true;
	
}

bool AULanzallamas::interpretarXmlInterno(AULanzallamas* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULanzallamas::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	const sNodoXML* nodoEscenaObj;			XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
	if(nodoEscenaObj != NULL){
		exito = AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj);
		if(exito){
			cargarEn->_segsEsperaInicial	= datosXml->nodoHijo("segsIni", 0.0f, nodoXml);
			SI32 tipoP						= datosXml->nodoHijo("tipP", 0, nodoXml); cargarEn->establecerTipoPresentacion((ENLanzallamaPresenta)tipoP);
			const sNodoXML* nodoPasos		= datosXml->nodoHijo("pasos", nodoXml);
			if(nodoPasos != NULL){
				const sNodoXML* nodoPaso	= datosXml->nodoHijo("paso", nodoPasos);
				while(nodoPaso != NULL){
					float segsDuracion		= datosXml->nodoHijo("segsDur", 0.0f, nodoPaso);
					float alcanceFuego		= datosXml->nodoHijo("alcF", 0.0f, nodoPaso);
					cargarEn->agregarPasoLanzamiento(segsDuracion, alcanceFuego);
					//
					nodoPaso	= datosXml->nodoHijo("paso", nodoPasos, nodoPaso);
				}
			}
		}
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

bool AULanzallamas::agregarBitsInternosEn(AULanzallamas* lanzallamas, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULanzallamas::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(lanzallamas, guardarEn)){
		guardarEn->escribir(&lanzallamas->_segsEsperaInicial, sizeof(lanzallamas->_segsEsperaInicial), 1, guardarEn);
		SI32 tipP = lanzallamas->_tipoPresentacion; guardarEn->escribir(&tipP, sizeof(tipP), 1, guardarEn);
		lanzallamas->_pasosLanzamiento.agregarBitsInternosEn(guardarEn);
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

bool AULanzallamas::interpretarBitsInternos(AULanzallamas* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULanzallamas::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			cargarDe->leer(&cargarEn->_segsEsperaInicial, sizeof(cargarEn->_segsEsperaInicial), 1, cargarDe);
			SI32 tipP; cargarDe->leer(&tipP, sizeof(tipP), 1, cargarDe); cargarEn->establecerTipoPresentacion((ENLanzallamaPresenta)tipP);
			cargarEn->_pasosLanzamiento.interpretarBitsInternos(cargarDe);
			exito = true;
		}
	} else {
		PRINTF_ERROR("AULanzallamas, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AULanzallamas, AUEscenaContenedor)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AULanzallamas, "AULanzallamas", AUEscenaContenedor)
AUOBJMETODOS_CLONAR_THIS(AULanzallamas)








