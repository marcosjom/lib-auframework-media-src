//
//  AUEscenaSonido.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 21/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaSonido.h"

#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO

AUEscenaSonido::AUEscenaSonido(ENAudioGrupo grupoAudio) : AUEscenaSnd(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::AUEscenaSonido")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSonido");
	privInlineInicializar(grupoAudio, NULL, 0, false, 1.0f);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSonido::AUEscenaSonido(ENAudioGrupo grupoAudio, AUEscenaObjeto* objetoEscena, const UI16 bufferAL) : AUEscenaSnd() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::AUEscenaSonido")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSonido");
	privInlineInicializar(grupoAudio, objetoEscena, bufferAL, false, 1.0f);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSonido::AUEscenaSonido(AUEscenaSonido* otra) : AUEscenaSnd(otra){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::AUEscenaSonido")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSonido");
	privInlineInicializar((ENAudioGrupo)otra->_grupoAudio, otra->_objetoEscena, otra->_bufferAL, otra->_repetir, otra->_volumen);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonido::privInlineInicializar(ENAudioGrupo grupoAudio, AUEscenaObjeto* objetoEscena, const UI16 bufferAL, bool repetir, float volumen){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::privInlineInicializar")
	_grupoAudio				= grupoAudio;
	_objetoEscena			= NULL;
	_bufferAL				= bufferAL; if(_bufferAL != 0) NBGestorSonidos::bufferRetener(_bufferAL);
	_repetir				= repetir;
	_volumen				= volumen;
	_fuenteOpenAL			= 0;
	if(objetoEscena != NULL)	this->establecerObjetoEscena(objetoEscena);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSonido::~AUEscenaSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::~AUEscenaSonido")
	//Advertencia, la retencion del "_objetoEscena" esta creando una retencion ciclica en las Animaciones.
	//La animacion retiene al sonido, y el sonido retiene a la animacion
	//if(_objetoEscena != NULL) _objetoEscena->liberar(NB_RETENEDOR_THIS); 
	if(_bufferAL != 0) NBGestorSonidos::bufferLiberar(_bufferAL); _bufferAL = 0;
	if(_fuenteOpenAL != 0) NBGestorSonidos::fuenteLiberar(_fuenteOpenAL); /*this->sndLiberarFuente(_fuenteOpenAL);*/ NBASSERT(_fuenteOpenAL == 0)
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

void AUEscenaSonido::privInlineAsegurarFuenteAL(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::privInlineAsegurarFuenteAL")
	if(_fuenteOpenAL==0){
		if(NBGestorSonidos::bufferEstado(_bufferAL)==ENBufferALEstado_Cargado){
			_fuenteOpenAL = NBGestorSonidos::fuenteAsignarEstatica(this, true, (ENAudioGrupo)_grupoAudio);
			if(_fuenteOpenAL!=0){
				NBGestorSonidos::fuenteRetener(_fuenteOpenAL);
				//PRINTF_INFO("SONIDO, fuenteSonido (%d) asignada\n", _fuenteOpenAL->idFuenteOpenAL());
				NBGestorSonidos::fuenteEstablecerBuffer(_fuenteOpenAL, _bufferAL);
				NBGestorSonidos::fuenteEstablecerRepetir(_fuenteOpenAL, _repetir);
				NBGestorSonidos::fuenteEstablecerVolumen(_fuenteOpenAL, _volumen);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//para ser notificado por el NBGestorSonidos que la fuenteSonidoAL ya no me pertence
void AUEscenaSonido::sndLiberarFuente(const UI16 idFuenteSonidoAL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::sndLiberarFuente")
	NBASSERT(idFuenteSonidoAL == _fuenteOpenAL)
	if(_fuenteOpenAL != 0){
		NBGestorSonidos::fuenteDetener(_fuenteOpenAL);
		NBGestorSonidos::fuenteEstablecerRepetir(_fuenteOpenAL, false);
		NBGestorSonidos::fuenteEstablecerBuffer(_fuenteOpenAL, 0);
		//PRINTF_INFO("SONIDO, fuenteSonido (%d) liberada, buffer quitado de fuente para convertirlo a tipo AL_UNDETERMINATED\n", _fuenteOpenAL->idFuenteOpenAL());
		//NBGestorSonidos::fuenteLiberar(fuenteOpenAL); //No invocar (sería recursivo)
		_fuenteOpenAL = 0;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//para ser notificado por el NBGestorSonidos que debe continuarse con el llenado de stream
void AUEscenaSonido::sndStreamBufferQuitadoDeCola(const UI16 cantBufferQuitados){
	NBASSERT(false) //No deberia de ser invocado
}

//

AUEscenaObjeto* AUEscenaSonido::objetoEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::objetoEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _objetoEscena;
}

UI16 AUEscenaSonido::fuenteAL(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::fuenteAL")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _fuenteOpenAL;
}

NBSndOrigenDatos AUEscenaSonido::fuenteDatosSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::fuenteDatosSonido")
	NBSndOrigenDatos sndFuente;
	sndFuente.tipo = ENSndFuente_buffer;
	sndFuente.indice = _bufferAL;
	sndFuente.puntero = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return sndFuente;
}

UI16 AUEscenaSonido::bufferAL(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::bufferAL")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _bufferAL;
}

bool AUEscenaSonido::repetir(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::repetir")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _repetir;
}

float AUEscenaSonido::volumen(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::volumen")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _volumen;
}

void AUEscenaSonido::establecerObjetoEscena(AUEscenaObjeto* objetoEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::establecerObjetoEscena")
	//Advertencia, la retencion del "_objetoEscena" esta creando una retencion ciclica en las Animaciones.
	//La animacion retiene al sonido, y el sonido retiene a la animacion
	//if(objetoEscena != NULL) objetoEscena->retener(NB_RETENEDOR_THIS);
	//if(_objetoEscena != NULL) _objetoEscena->liberar(NB_RETENEDOR_THIS);
	_objetoEscena = objetoEscena;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonido::establecerRepetir(bool repetir){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::establecerRepetir")
	_repetir = repetir;
	if(_fuenteOpenAL!=0){
		NBGestorSonidos::fuenteEstablecerRepetir(_fuenteOpenAL, _repetir);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonido::establecerOffSetSegundos(float segundos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::establecerOffSetSegundos")
	if(_fuenteOpenAL!=0 && segundos>=0.0f){
		NBGestorSonidos::fuenteEstablecerOffsetMuestra(_fuenteOpenAL, NBGestorSonidos::bufferFrecuencia(_bufferAL) * segundos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonido::establecerVolumen(float volumen){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::establecerVolumen")
	//NBASSERT(volumen>=0.0f && volumen<=1.5f);
	//NBASSERT(volumen>=0.0f && volumen<=1.0f);
	_volumen = volumen;
	if(_fuenteOpenAL!=0){
		NBGestorSonidos::fuenteEstablecerVolumen(_fuenteOpenAL, _volumen);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

bool AUEscenaSonido::reproduciendoSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::reproduciendoSonido")
	bool r = false;
	if(_fuenteOpenAL!=0){
		r = NBGestorSonidos::fuenteReproduciendo(_fuenteOpenAL);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

void AUEscenaSonido::reproducirSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::reproducirSonido")
	privInlineAsegurarFuenteAL();
	if(_fuenteOpenAL!=0){
		NBGestorSonidos::fuenteReproducir(_fuenteOpenAL);
	} else {
		//PRINTF_WARNING("SONIDO, no hay fuente disponible\n");
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonido::pausarSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::pausarSonido")
	privInlineAsegurarFuenteAL();
	if(_fuenteOpenAL!=0){
		NBGestorSonidos::fuentePausar(_fuenteOpenAL);
		//PRINTF_INFO("Pausado sondido(%u) con bufferAL(%u)\n", (UI32)_idFuenteOpenAL, (UI32)_bufferAL->idBufferOpenAL());
	} else {
		PRINTF_WARNING("no se pudo pausar sonido porque no hay fuente disponible\n");
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonido::detenerSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::detenerSonido")
	if(_fuenteOpenAL!=0) NBGestorSonidos::fuenteDetener(_fuenteOpenAL);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonido::vaciarCola(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::vaciarCola")
	if(_fuenteOpenAL!=0) NBGestorSonidos::fuenteVaciarCola(_fuenteOpenAL);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSonido::rebobinarSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::rebobinarSonido")
	privInlineAsegurarFuenteAL();
	if(_fuenteOpenAL!=0){
		NBGestorSonidos::fuenteRebobinar(_fuenteOpenAL);
		//PRINTF_INFO("Rebobinado sondido(%u) con bufferAL(%u)\n", (UI32)_idFuenteOpenAL, (UI32)_bufferAL->idBufferOpenAL());
	} else {
		PRINTF_WARNING("no se pudo rebobinar sonido porque no hay fuente disponible\n");
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

float AUEscenaSonido::offsetSegundos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::offsetSegundos")
	float segundo = 0.0f;
	if(_fuenteOpenAL!=0){
		segundo = (float) NBGestorSonidos::fuenteOffsetMuestras(_fuenteOpenAL) / (float)NBGestorSonidos::bufferFrecuencia(_bufferAL);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return segundo;
}

//

bool AUEscenaSonido::agregarXmlInternoEn(AUEscenaSonido* sonido, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::agregarXmlInternoEn")
	guardarEn->agregarConFormato("%s  <snd>\r\n", espaciosBaseIzq);
	AUEscenaSnd::agregarXmlInternoEn(sonido, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s  </snd>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s  <rep>%d</rep>\r\n", espaciosBaseIzq, sonido->_repetir?1:0);
	guardarEn->agregarConFormato("%s  <grp>%d</grp>\r\n", espaciosBaseIzq, (SI32)sonido->_grupoAudio);
	guardarEn->agregarConFormato("%s  <vol>%f</vol>\r\n", espaciosBaseIzq, sonido->_volumen);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
	
}

bool AUEscenaSonido::interpretarXmlInterno(AUEscenaSonido* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::interpretarXmlInterno")
	bool exito = false;
	const sNodoXML* nodoSnd = datosXml->nodoHijo("snd", nodoXml);
	if(nodoSnd != NULL){
		if(AUEscenaSnd::interpretarXmlInterno(cargarEn, datosXml, nodoSnd)){
			exito = true;
			cargarEn->establecerRepetir(datosXml->nodoHijo("rep", true, nodoXml));
			cargarEn->_grupoAudio	= datosXml->nodoHijo("grp", (SI32)ENAudioGrupo_Efectos, nodoXml); //pendiente establecer el grupo de audio en el Gestor
			cargarEn->establecerVolumen(datosXml->nodoHijo("vol", 1.0f, nodoXml));
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaSonido::agregarBitsInternosEn(AUEscenaSonido* sonido, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::agregarBitsInternosEn")
	float versionDatos = 1.0f;		guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	AUEscenaSnd::agregarBitsInternosEn(sonido, guardarEn);
	bool rep = sonido->_repetir;	guardarEn->escribir(&rep, sizeof(rep), 1, guardarEn);
	UI8 grp	 = sonido->_grupoAudio;	guardarEn->escribir(&grp, sizeof(grp), 1, guardarEn);
	float vol = sonido->_volumen;	guardarEn->escribir(&vol, sizeof(vol), 1, guardarEn);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUEscenaSonido::interpretarBitsInternos(AUEscenaSonido* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSonido::interpretarBitsInternos")
	bool exito = false;
	float versionDatos; cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		AUEscenaSnd::interpretarBitsInternos(cargarEn, cargarDe);
		bool rep; cargarDe->leer(&rep, sizeof(rep), 1, cargarDe);	cargarEn->establecerRepetir(rep);
		UI8 grp; cargarDe->leer(&grp, sizeof(grp), 1, cargarDe);	cargarEn->_grupoAudio = grp; //pendiente establecer el grupo de audio en el Gestor
		float vol; cargarDe->leer(&vol, sizeof(vol), 1, cargarDe);	cargarEn->establecerVolumen(vol);
		exito = true;
	} else {
		PRINTF_ERROR("AUEscenaSonido, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaSonido, AUEscenaSnd)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaSonido, "AUEscenaSonido", AUEscenaSnd)
AUOBJMETODOS_CLONAR_THIS(AUEscenaSonido)




#endif //#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO


