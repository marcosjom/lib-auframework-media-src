
#-------------------------
# PROJECT
#-------------------------

$(eval $(call nbCall,nbInitProject))

NB_PROJECT_NAME_VAR       := auframework_media

NB_PROJECT_NAME           := auframework-media

NB_PROJECT_CXXFLAGS       := -std=c++11

NB_PROJECT_INCLUDES       := \
   include \
   ../../sys-nbframework/lib-nbframework-src/include \
   ../lib-auframework-src/include \
   ../lib-nixtla-audio-src/include \
   ../lib-nixtla-audio-src/src/c

#-------------------------
# TARGET
#-------------------------

$(eval $(call nbCall,nbInitTarget))

NB_TARGET_NAME_VAR       := auframework_media

NB_TARGET_NAME           := auframework-media

NB_TARGET_PREFIX         := lib

NB_TARGET_SUFIX          := .a

NB_TARGET_TYPE           := static

#-------------------------
# CODE GRP
#-------------------------

$(eval $(call nbCall,nbInitCodeGrp))

NB_CODE_GRP_NAME_VAR  := core

NB_CODE_GRP_NAME      := core

NB_CODE_GRP_SRCS      := \
   src/cpp/AUAnimadorAnimacion.cpp \
   src/cpp/AUAnimadorContenedor.cpp \
   src/cpp/AUAnimadorObjetoEscena.cpp \
   src/cpp/AUAnimadorSonido.cpp \
   src/cpp/AUAnimadorSonidoVolumen.cpp \
   src/cpp/AUBateria.cpp \
   src/cpp/AUBombillo.cpp \
   src/cpp/AUCargadorAnimaciones.cpp \
   src/cpp/AUCargadorCuerpos.cpp \
   src/cpp/AUDialogo.cpp \
   src/cpp/AUDialogoMutable.cpp \
   src/cpp/AUDiccionarioTextoMulti.cpp \
   src/cpp/AUElectronodo.cpp \
   src/cpp/AUEscenaAnimacion.cpp \
   src/cpp/AUEscenaAnimacionI.cpp \
   src/cpp/AUEscenaCol.cpp \
   src/cpp/AUEscenaCols.cpp \
   src/cpp/AUEscenaContenedor.cpp \
   src/cpp/AUEscenaContenedorLimitado.cpp \
   src/cpp/AUEscenaContenedorRaiz.cpp \
   src/cpp/AUEscenaCuerda.cpp \
   src/cpp/AUEscenaCuerpo.cpp \
   src/cpp/AUEscenaCuerpoI.cpp \
   src/cpp/AUEscenaCuerpoMulti.cpp \
   src/cpp/AUEscenaElectroRayo.cpp \
   src/cpp/AUEscenaFigura.cpp \
   src/cpp/AUEscenaFiguraTransformable.cpp \
   src/cpp/AUEscenaFiltro.cpp \
   src/cpp/AUEscenaGeneradorParticulas.cpp \
   src/cpp/AUEscenaGrupoParticulas.cpp \
   src/cpp/AUEscenaGrupoParticulasFuego.cpp \
   src/cpp/AUEscenaGrupoParticulasLinea.cpp \
   src/cpp/AUEscenaIluminacion.cpp \
   src/cpp/AUEscenaLaser.cpp \
   src/cpp/AUEscenaLuzEspejo.cpp \
   src/cpp/AUEscenaLuzRadial.cpp \
   src/cpp/AUEscenaLuzSensor.cpp \
   src/cpp/AUEscenaLuzSombra.cpp \
   src/cpp/AUEscenaMalla.cpp \
   src/cpp/AUEscenaMalla3.cpp \
   src/cpp/AUEscenaMalla9.cpp \
   src/cpp/AUEscenaMalla9Patron.cpp \
   src/cpp/AUEscenaMallaBezier.cpp \
   src/cpp/AUEscenaMallaConHueso.cpp \
   src/cpp/AUEscenaMallaGelatina.cpp \
   src/cpp/AUEscenaMallaXBase.cpp \
   src/cpp/AUEscenaObjeto.cpp \
   src/cpp/AUEscenaPanel.cpp \
   src/cpp/AUEscenaPanelTexto.cpp \
   src/cpp/AUEscenaPolea.cpp \
   src/cpp/AUEscenaPuasLinea.cpp \
   src/cpp/AUEscenaPuasLineaAnimIndiv.cpp \
   src/cpp/AUEscenaPuasLineaAnimSinc.cpp \
   src/cpp/AUEscenaRuta.cpp \
   src/cpp/AUEscenaRutaAnimador.cpp \
   src/cpp/AUEscenaSensor.cpp \
   src/cpp/AUEscenaSnd.cpp \
   src/cpp/AUEscenaSonido.cpp \
   src/cpp/AUEscenaSonidoStream.cpp \
   src/cpp/AUEscenaSonidoStreamMulti.cpp \
   src/cpp/AUEscenaSprite.cpp \
   src/cpp/AUEscenaSpriteElastico.cpp \
   src/cpp/AUEscenaSpriteMascara.cpp \
   src/cpp/AUEscenaSpritePorcion.cpp \
   src/cpp/AUEscenaSpriteWithLoader.cpp \
   src/cpp/AUEscenaTexto.cpp \
   src/cpp/AUEscenaTextoEditable.cpp \
   src/cpp/AUEscenaTextoEditableOffscreen.cpp \
   src/cpp/AUFrameworkMediaRegistro.cpp \
   src/cpp/AUFrameworkMediaStdAfx.cpp \
   src/cpp/AUFuenteRaster.cpp \
   src/cpp/AUFuenteRender.cpp \
   src/cpp/AUFuenteTextura.cpp \
   src/cpp/AUFuenteVisualModel.cpp \
   src/cpp/AUIButton.cpp \
   src/cpp/AUIDateBox.cpp \
   src/cpp/AUIMsgBox.cpp \
   src/cpp/AUISelectBox.cpp \
   src/cpp/AUITextBox.cpp \
   src/cpp/AULanzallamas.cpp \
   src/cpp/AULaser.cpp \
   src/cpp/AUMngrFonts.cpp \
   src/cpp/AUNodoElectroCarga.cpp \
   src/cpp/AUPanelSolar.cpp \
   src/cpp/AUParticlesGenerator.cpp \
   src/cpp/AURenderBuffer.cpp \
   src/cpp/AUSimuladorLiquidos.cpp \
   src/cpp/AUTextoFiltro.cpp \
   src/cpp/AUTextoFiltroLimitado.cpp \
   src/cpp/AUTextura.cpp \
   src/cpp/NBGestorAnimaciones.cpp \
   src/cpp/NBGestorCuerpos.cpp \
   src/cpp/NBGestorEscena.cpp \
   src/cpp/NBGestorFisica.cpp \
   src/cpp/NBGestorGL.cpp \
   src/cpp/NBGestorIdioma.cpp \
   src/cpp/NBGestorPersonajes.cpp \
   src/cpp/NBGestorSonidos.cpp \
   src/cpp/NBGestorTeclas.cpp \
   src/cpp/NBGestorTexturas.cpp \
   src/cpp/NBGestorTouches.cpp \
   src/cpp/NBMngrFonts.cpp \
   src/cpp/NBSegmentadorFiguras.cpp \
   src/cpp/NBSimuladorLiquidosMotor1.cpp \
   src/cpp/NBSimuladorLiquidosMotor2.cpp

$(eval $(call nbCall,nbBuildCodeGrpRules))

#-------------------------
# TARGET RULES
#-------------------------

$(eval $(call nbCall,nbBuildTargetRules))

#-------------------------
# PROJECT RULES
#-------------------------

$(eval $(call nbCall,nbBuildProjectRules))
