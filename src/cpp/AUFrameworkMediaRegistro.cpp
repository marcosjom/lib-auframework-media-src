//
//  AUEncabezadoPrecompilado.cpp
//  DodoTown
//
//  Created by Marcos Ortega Morales on 09/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUFrameworkMedia.h"
#include "AUFrameworkMediaRegistro.h"

UI16 AUAnimadorAnimacion::idTipoClase;
UI16 AUAnimadorContenedor::idTipoClase;
UI16 AUAnimadorObjetoEscena::idTipoClase;
//
UI16 AUCargadorAnimaciones::idTipoClase;
UI16 AUCargadorCuerpos::idTipoClase;
//
UI16 AUDialogo::idTipoClase;
UI16 AUDialogoMutable::idTipoClase;
UI16 AUDiccionarioTextoMulti::idTipoClase;
//
UI16 AUTextoFiltro::idTipoClase;
UI16 AUTextoFiltroLimitado::idTipoClase;
//
UI16 AUFuenteVisualModel::idTipoClase;
UI16 AUFuenteRender::idTipoClase;
UI16 AUFuenteRaster::idTipoClase;
UI16 AUFuenteTextura::idTipoClase;
UI16 AUSonidoStream::idTipoClase;
UI16 AUTextura::idTipoClase;
//
UI16 AUBateria::idTipoClase;
UI16 AUBombillo::idTipoClase;
UI16 AUElectronodo::idTipoClase;
UI16 AULanzallamas::idTipoClase;
UI16 AULaser::idTipoClase;
UI16 AUNodoElectroCarga::idTipoClase;
UI16 AUPanelSolar::idTipoClase;
//
UI16 AURenderBuffer::idTipoClase;
#ifndef CONFIG_NB_UNSUPPORT_BOX2D
UI16 AUSimuladorLiquidos::idTipoClase;
#endif
//
//
UI16 AUCargadorAnimacionesI::idTipoClase;
UI16 AUCargadorCuerposI::idTipoClase;
UI16 AUAnimacionBibliotecaDef::idTipoClase;
UI16 AUAnimacionDef::idTipoClase;
//
UI16 AUEscenaAnimacion::idTipoClase;
UI16 AUEscenaAnimacionI::idTipoClase;
UI16 AUEscenaContenedor::idTipoClase;
UI16 AUEscenaContenedorLimitado::idTipoClase;
UI16 AUEscenaContenedorRaiz::idTipoClase;
UI16 AUEscenaPanel::idTipoClase;
UI16 AUEscenaCuerda::idTipoClase;
UI16 AUCuerpoDef::idTipoClase;
UI16 AUEscenaCuerpo::idTipoClase;
UI16 AUEscenaCuerpoI::idTipoClase;
UI16 AUEscenaCuerpoMulti::idTipoClase;
UI16 AUEscenaElectroRayo::idTipoClase;
UI16 AUEscenaFigura::idTipoClase;
UI16 AUEscenaFiguraTransformable::idTipoClase;
UI16 AUEscenaFiltro::idTipoClase;
UI16 AUEscenaGeneradorParticulas::idTipoClase;
UI16 AUEscenaGrupoParticulas::idTipoClase;
UI16 AUEscenaGrupoParticulasFuego::idTipoClase;
UI16 AUEscenaGrupoParticulasLinea::idTipoClase;
UI16 AUEscenaIluminacion::idTipoClase;
UI16 AUEscenaLaser::idTipoClase;
UI16 AUEscenaLuzEspejo::idTipoClase;
UI16 AUEscenaLuzRadial::idTipoClase;
UI16 AUEscenaLuzSensor::idTipoClase;
UI16 AUEscenaLuzSombra::idTipoClase;
UI16 AUEscenaMalla::idTipoClase;
UI16 AUEscenaMalla3::idTipoClase;
UI16 AUEscenaMalla9::idTipoClase;
UI16 AUEscenaMalla9Patron::idTipoClase;
UI16 AUEscenaMallaBezier::idTipoClase;
UI16 AUEscenaMallaConHueso::idTipoClase;
UI16 AUEscenaMallaGelatina::idTipoClase;
UI16 AUEscenaMallaXBase::idTipoClase;
UI16 AUEscenaObjeto::idTipoClase;
UI16 AUEscenaPolea::idTipoClase;
UI16 AUParticlesGenerator::idTipoClase;
UI16 AUEscenaPuasLinea::idTipoClase;
UI16 AUEscenaPuasLineaAnimIndiv::idTipoClase;
UI16 AUEscenaPuasLineaAnimSinc::idTipoClase;
UI16 AUEscenaRuta::idTipoClase;
UI16 AUEscenaRutaAnimador::idTipoClase;
UI16 AUEscenaSensor::idTipoClase;
#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
UI16 AUEscenaSnd::idTipoClase;
UI16 AUEscenaSonido::idTipoClase;
UI16 AUEscenaSonidoStream::idTipoClase;
UI16 AUEscenaSonidoStreamMulti::idTipoClase;
UI16 AUAnimadorSonido::idTipoClase;
UI16 AUAnimadorSonidoVolumen::idTipoClase;
#endif
UI16 AUEscenaSpriteWithLoader::idTipoClase;
UI16 AUEscenaPanelTexto::idTipoClase;
UI16 AUEscenaSprite::idTipoClase;
UI16 AUEscenaSpriteElastico::idTipoClase;
UI16 AUEscenaSpriteMascara::idTipoClase;
UI16 AUEscenaSpritePorcion::idTipoClase;
UI16 AUEscenaTexto::idTipoClase;
UI16 AUEscenaTextoEditable::idTipoClase;
UI16 AUEscenaTextoEditableOffscreen::idTipoClase;
UI16 AUEscenaTextoFade::idTipoClase; //ToDo: implement as filter, not object
//UI16 AUEscenaTextoSprites::idTipoClase; //ToDo: implement and enable
//Controles
UI16 AUITextBox::idTipoClase;
UI16 AUIButton::idTipoClase;
UI16 AUIDateBox::idTipoClase;
UI16 AUISelectBox::idTipoClase;
UI16 AUIMsgBox::idTipoClase;
//
UI16 AUMngrFonts::idTipoClase;

#ifndef NB_METODO_INICIALIZADOR_CUERPO
#error "Falta inclusion. Aun no se ha definido la macro NB_METODO_INICIALIZADOR_CUERPO."
#endif

NB_METODO_INICIALIZADOR_CUERPO(AUFrameworkMediaRegistrar) {
	printf("\n\n+++++++++++++ AUFrameworkMediaRegistrar +++++++++++++++\n\n");
	NBGestorAUObjetos::registrarClase("AUAnimadorAnimacion", &AUAnimadorAnimacion::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUAnimadorContenedor", &AUAnimadorContenedor::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUAnimadorObjetoEscena", &AUAnimadorObjetoEscena::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUBateria", &AUBateria::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUBombillo", &AUBombillo::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUCargadorAnimaciones", &AUCargadorAnimaciones::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUCargadorCuerpos", &AUCargadorCuerpos::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUDialogo", &AUDialogo::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUDialogoMutable", &AUDialogoMutable::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUDiccionarioTextoMulti", &AUDiccionarioTextoMulti::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUElectronodo", &AUElectronodo::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaAnimacion", &AUEscenaAnimacion::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaAnimacionI", &AUEscenaAnimacionI::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaContenedor", &AUEscenaContenedor::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaContenedorRaiz", &AUEscenaContenedorRaiz::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaContenedorLimitado", &AUEscenaContenedorLimitado::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaPanel", &AUEscenaPanel::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaCuerda", &AUEscenaCuerda::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUCuerpoDef", &AUCuerpoDef::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaCuerpo", &AUEscenaCuerpo::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaCuerpoI", &AUEscenaCuerpoI::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaCuerpoMulti", &AUEscenaCuerpoMulti::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaElectroRayo", &AUEscenaElectroRayo::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaFigura", &AUEscenaFigura::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaFiguraTransformable", &AUEscenaFiguraTransformable::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaFiltro", &AUEscenaFiltro::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaGeneradorParticulas", &AUEscenaGeneradorParticulas::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaGrupoParticulas", &AUEscenaGrupoParticulas::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaGrupoParticulasFuego", &AUEscenaGrupoParticulasFuego::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaGrupoParticulasLinea", &AUEscenaGrupoParticulasLinea::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaIluminacion", &AUEscenaIluminacion::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaLaser", &AUEscenaLaser::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaLuzEspejo", &AUEscenaLuzEspejo::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaLuzRadial", &AUEscenaLuzRadial::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaLuzSensor", &AUEscenaLuzSensor::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaLuzSombra", &AUEscenaLuzSombra::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaMalla", &AUEscenaMalla::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaMalla3", &AUEscenaMalla3::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaMalla9", &AUEscenaMalla9::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaMalla9Patron", &AUEscenaMalla9Patron::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaMallaBezier", &AUEscenaMallaBezier::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaMallaConHueso", &AUEscenaMallaConHueso::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaMallaGelatina", &AUEscenaMallaGelatina::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaMallaXBase", &AUEscenaMallaXBase::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaObjeto", &AUEscenaObjeto::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaPolea", &AUEscenaPolea::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUParticlesGenerator", &AUParticlesGenerator::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaPuasLinea", &AUEscenaPuasLinea::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaPuasLineaAnimIndiv", &AUEscenaPuasLineaAnimIndiv::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaPuasLineaAnimSinc", &AUEscenaPuasLineaAnimSinc::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaRuta", &AUEscenaRuta::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaRutaAnimador", &AUEscenaRutaAnimador::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaSensor", &AUEscenaSensor::idTipoClase);
#	ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
	NBGestorAUObjetos::registrarClase("AUEscenaSnd", &AUEscenaSnd::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaSonido", &AUEscenaSonido::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaSonidoStream", &AUEscenaSonidoStream::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaSonidoStreamMulti", &AUEscenaSonidoStreamMulti::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUAnimadorSonido", &AUAnimadorSonido::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUAnimadorSonidoVolumen", &AUAnimadorSonidoVolumen::idTipoClase);
#	endif
	NBGestorAUObjetos::registrarClase("AUEscenaSpriteWithLoader", &AUEscenaSpriteWithLoader::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaPanelTexto", &AUEscenaPanelTexto::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaSprite", &AUEscenaSprite::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaSpriteMascara", &AUEscenaSpriteMascara::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaSpritePorcion", &AUEscenaSpritePorcion::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaSpriteElastico", &AUEscenaSpriteElastico::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaTexto", &AUEscenaTexto::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaTextoEditable", &AUEscenaTextoEditable::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaTextoEditableOffscreen", &AUEscenaTextoEditableOffscreen::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUEscenaTextoFade", &AUEscenaTextoFade::idTipoClase); //ToDo: implement as filter, not object
	//NBGestorAUObjetos::registrarClase("AUEscenaTextoSprites", &AUEscenaTextoSprites::idTipoClase); //ToDo: implement and enable
	//
	NBGestorAUObjetos::registrarClase("AUTextoFiltro", &AUTextoFiltro::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUTextoFiltroLimitado", &AUTextoFiltroLimitado::idTipoClase);
	//
	NBGestorAUObjetos::registrarClase("AUFuenteVisualModel", &AUFuenteVisualModel::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUFuenteRender", &AUFuenteRender::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUFuenteRaster", &AUFuenteRaster::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUFuenteTextura", &AUFuenteTextura::idTipoClase);
	NBGestorAUObjetos::registrarClase("AULanzallamas", &AULanzallamas::idTipoClase);
	NBGestorAUObjetos::registrarClase("AULaser", &AULaser::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUNodoElectroCarga", &AUNodoElectroCarga::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUPanelSolar", &AUPanelSolar::idTipoClase);
	NBGestorAUObjetos::registrarClase("AURenderBuffer", &AURenderBuffer::idTipoClase);
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	NBGestorAUObjetos::registrarClase("AUSimuladorLiquidos", &AUSimuladorLiquidos::idTipoClase);
#	endif
	NBGestorAUObjetos::registrarClase("AUSonidoStream", &AUSonidoStream::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUTextura", &AUTextura::idTipoClase);
	//Controles
	NBGestorAUObjetos::registrarClase("AUITextBox", &AUITextBox::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUIButton", &AUIButton::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUIDateBox", &AUIDateBox::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUISelectBox", &AUISelectBox::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUIMsgBox", &AUIMsgBox::idTipoClase);
	//
	NBGestorAUObjetos::registrarClase("AUCargadorAnimacionesI", &AUCargadorAnimacionesI::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUCargadorCuerposI", &AUCargadorCuerposI::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUAnimacionBibliotecaDef", &AUAnimacionBibliotecaDef::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUAnimacionDef", &AUAnimacionDef::idTipoClase);
	//
	NBGestorAUObjetos::registrarClase("AUMngrFonts", &AUMngrFonts::idTipoClase);
}
