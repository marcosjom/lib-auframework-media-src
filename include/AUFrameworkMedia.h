//
//  AUFramework.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 23/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

//Para compilar produciendo descripciones de las clases y estructuras: "/d1reportAllClassLayout"

/*
 XCODE 4:
 Para enlazar proyectos dependientes de un workspace:
 1 - En cada proyecto fuente: activar casilla del esquema como "shared"
 2 - En el proyecto destino: agregar las librerias en las "Build Phases" -> "Link Libraries"
 3 - En el proyecto destino: agregar en la variable "User Header Seach Paths" del "Build Settings": "./ $(SRCROOT)/../../PROYECTO_A $(SRCROOT)/../../PROYECTO_B"
 4 - Si tu codigo es C++, recuerda revisar que todos los archivos del proyecto destino sean ".mm" en lugar de ".m", sino las definiciones de clases daran error
 Otras instrucciones para configurar un WORKSPACE CON PROYECTOS LINKEADOS
 https://docs.google.com/document/pub?id=14XR5zcZb2Kz2s6A4AbzB00NLkrW9bWxMMprVsUao-hY&pli=1
 */

#ifndef AUFRAMEWORKMEDIA_H_INCLUIDO
#define AUFRAMEWORKMEDIA_H_INCLUIDO

//REGISTRO
#include "AUFrameworkMediaRegistro.h"

//COMUN
#include "NBGestorIdioma.h"
#include "AUDiccionarioTextoMulti.h"
#include "AUDialogo.h"
#include "AUDialogoMutable.h"

//FISICA
#include "NBPropiedadesFijacion.h"

//AUDIO
#include "NBGestorSonidos.h"

//VISUAL
#include "NBPropiedadesEnEscena.h"
#include "NBGestorGL.h"
#include "NBGestorTexturas.h"
#include "NBGestorAnimaciones.h"
#include "NBGestorPersonajes.h"
#include "AUCargadorAnimaciones.h"	//Intermediario para la carga de animaciones (solo usado en el GestorCuerpos)
#include "NBGestorTeclas.h"
#include "NBGestorTouches.h"
#include "NBGestorEscena.h"
#include "NBGestorFisica.h"
#include "AUSimuladorLiquidos.h"
#include "NBGestorCuerpos.h"
#include "AUCargadorCuerpos.h"		//Intermediario para la carga de cuerpos (solo usado en el GestorAnimaciones)
#include "AUAnimadorObjetoEscena.h"
#include "AUAnimadorContenedor.h"
#include "AUAnimadorAnimacion.h"
#include "AUAnimadorSonido.h"
#include "AUEscenaObjeto.h"
#include "AUEscenaContenedor.h"
#include "AUEscenaContenedorLimitado.h"
#include "AUEscenaContenedorRaiz.h"
#include "AUEscenaPanel.h"
#include "AUEscenaPanelTexto.h"
#include "AUEscenaAnimacion.h"
#include "AUEscenaFigura.h"
#include "AUEscenaFiguraTransformable.h"
#include "AUEscenaRuta.h"
#include "AUEscenaRutaAnimador.h"
#include "AUEscenaLaser.h"
#include "AUEscenaSpriteWithLoader.h"
#include "AUEscenaSprite.h"
#include "AUEscenaSpritePorcion.h"
#include "AUEscenaSpriteMascara.h"
#include "AUEscenaSpriteElastico.h"
#include "AUEscenaGrupoParticulas.h"
#include "AUEscenaGrupoParticulasFuego.h"
#include "AUEscenaElectroRayo.h"
#include "AUEscenaGeneradorParticulas.h"
#include "AUEscenaGrupoParticulasLinea.h"
#include "AUEscenaPuasLinea.h"
#include "AUEscenaPuasLineaAnimSinc.h"
#include "AUEscenaPuasLineaAnimIndiv.h"
#include "AUEscenaTexto.h"
#include "AUEscenaTextoEditable.h"
#include "AUEscenaTextoEditableOffscreen.h"
#include "AUEscenaTextoFade.h" //ToDo: implement as filter, not object
//#include "AUEscenaTextoSprites.h" //ToDo: implement and enable
#include "AUEscenaLuzRadial.h"
#include "AUEscenaLuzSensor.h"
#include "AUEscenaLuzEspejo.h"
#include "AUEscenaLuzSombra.h"
#include "AURenderBuffer.h"
#include "AUTextura.h"
#include "AUTextoFiltro.h"
#include "AUTextoFiltroLimitado.h"
#include "AUFuenteVisualModel.h"
#include "AUFuenteRender.h"
#include "AUFuenteTextura.h"
#include "AUEscenaSnd.h"
#include "AUEscenaSonido.h"
#include "AUEscenaSonidoStream.h"
#include "AUEscenaSonidoStreamMulti.h"
#include "AUEscenaMalla.h"
#include "AUEscenaMallaGelatina.h"
#include "AUEscenaMallaConHueso.h"
#include "AUEscenaMallaBezier.h"
#include "AUEscenaMallaXBase.h"
#include "AUEscenaMalla3.h"
#include "AUEscenaMalla9.h"
#include "AUEscenaMalla9Patron.h"
#include "AUEscenaCuerda.h"
#include "NBSegmentadorFiguras.h"
#include "AUEscenaIluminacion.h"
#include "AUEscenaCuerpo.h"
#include "AUEscenaCuerpoMulti.h"
#include "AUEscenaSensor.h"
#include "AUEscenaPolea.h"
#include "AUEscenaFiltro.h"
#include "AUParticlesGenerator.h"
//Controles
#include "AUEscenaListaItemI.h"
#include "AUITextBox.h"
#include "AUIButton.h"
#include "AUIDateBox.h"
#include "AUISelectBox.h"
#include "AUIMsgBox.h"
//
#include "AUMngrFonts.h"
#include "NBMngrFonts.h"

//AVANZADOS
#include "AUPanelSolar.h"
#include "AUBateria.h"
#include "AUBombillo.h"
#include "AULaser.h"
#include "AULanzallamas.h"
#include "AUElectronodo.h"
//

#endif


