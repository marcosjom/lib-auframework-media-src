LOCAL_PATH 		:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    	:= auframework-media

#---------------------
# Optimization options
#---------------------
#-fvisibility=hidden:   removes not-public functions (non-JNIEXPORT) from the symbols table.
#-ffunction-sections:   place each FUCNTION item into its own section in the output file (needed for "--gc-sections").
#-fdata-sections:       place each DATA item into its own section in the output file (needed for "--gc-sections").
LOCAL_CPPFLAGS  += -ffunction-sections -fdata-sections #-fvisibility=hidden
LOCAL_CFLAGS    += -ffunction-sections -fdata-sections #-fvisibility=hidden

LOCAL_C_INCLUDES	:= \
$(LOCAL_PATH)/../../include \
$(LOCAL_PATH)/../../../../sys-nbframework/sys-nbframework-src/include \
$(LOCAL_PATH)/../../../lib-nixtla-audio-src/include \
$(LOCAL_PATH)/../../../lib-nixtla-audio-src/src/c \
$(LOCAL_PATH)/../../../lib-box2d-src/include \
$(LOCAL_PATH)/../../../lib-auframework-src/include

SRC_AUF_MEDIA_PATH := ../../src/cpp

LOCAL_SRC_FILES 	:= ../../../../../actualizarTodos.cpp \
$(SRC_AUF_MEDIA_PATH)/AUAnimadorAnimacion.cpp \
$(SRC_AUF_MEDIA_PATH)/AUAnimadorContenedor.cpp \
$(SRC_AUF_MEDIA_PATH)/AUAnimadorObjetoEscena.cpp \
$(SRC_AUF_MEDIA_PATH)/AUAnimadorSonido.cpp \
$(SRC_AUF_MEDIA_PATH)/AUAnimadorSonidoVolumen.cpp \
$(SRC_AUF_MEDIA_PATH)/AUBateria.cpp \
$(SRC_AUF_MEDIA_PATH)/AUBombillo.cpp \
$(SRC_AUF_MEDIA_PATH)/AUCargadorAnimaciones.cpp \
$(SRC_AUF_MEDIA_PATH)/AUCargadorCuerpos.cpp \
$(SRC_AUF_MEDIA_PATH)/AUDialogo.cpp \
$(SRC_AUF_MEDIA_PATH)/AUDialogoMutable.cpp \
$(SRC_AUF_MEDIA_PATH)/AUDiccionarioTextoMulti.cpp \
$(SRC_AUF_MEDIA_PATH)/AUElectronodo.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaAnimacion.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaAnimacionI.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaCol.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaCols.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaContenedor.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaContenedorLimitado.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaCuerda.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaCuerpo.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaCuerpoI.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaCuerpoMulti.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaElectroRayo.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaFigura.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaFiguraTransformable.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaFiltro.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaGeneradorParticulas.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaGrupoParticulas.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaGrupoParticulasFuego.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaGrupoParticulasLinea.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaIluminacion.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaLaser.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaLuzEspejo.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaLuzRadial.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaLuzSensor.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaLuzSombra.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaMalla.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaMalla3.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaMalla9.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaMalla9Patron.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaMallaBezier.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaMallaConHueso.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaMallaGelatina.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaMallaXBase.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaObjeto.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaPanel.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaPolea.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaPuasLinea.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaPuasLineaAnimIndiv.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaPuasLineaAnimSinc.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaRuta.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaRutaAnimador.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaSensor.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaSnd.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaSonido.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaSonidoStream.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaSonidoStreamMulti.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaSprite.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaSpriteElastico.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaSpriteMascara.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaSpritePorcion.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaSpriteWithLoader.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaTexto.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaTextoEditable.cpp \
$(SRC_AUF_MEDIA_PATH)/AUEscenaTextoEditableOffscreen.cpp \
$(SRC_AUF_MEDIA_PATH)/AUFrameworkMediaRegistro.cpp \
$(SRC_AUF_MEDIA_PATH)/AUFrameworkMediaStdAfx.cpp \
$(SRC_AUF_MEDIA_PATH)/AUFuenteRaster.cpp \
$(SRC_AUF_MEDIA_PATH)/AUFuenteRender.cpp \
$(SRC_AUF_MEDIA_PATH)/AUFuenteTextura.cpp \
$(SRC_AUF_MEDIA_PATH)/AUFuenteVisualModel.cpp \
$(SRC_AUF_MEDIA_PATH)/AUIButton.cpp \
$(SRC_AUF_MEDIA_PATH)/AUIDatebox.cpp \
$(SRC_AUF_MEDIA_PATH)/AUISelectbox.cpp \
$(SRC_AUF_MEDIA_PATH)/AUITextbox.cpp \
$(SRC_AUF_MEDIA_PATH)/AULanzallamas.cpp \
$(SRC_AUF_MEDIA_PATH)/AULaser.cpp \
$(SRC_AUF_MEDIA_PATH)/AUMngrFonts.cpp \
$(SRC_AUF_MEDIA_PATH)/AUNodoElectroCarga.cpp \
$(SRC_AUF_MEDIA_PATH)/AUPanelSolar.cpp \
$(SRC_AUF_MEDIA_PATH)/AUParticlesGenerator.cpp \
$(SRC_AUF_MEDIA_PATH)/AURenderBuffer.cpp \
$(SRC_AUF_MEDIA_PATH)/AUSimuladorLiquidos.cpp \
$(SRC_AUF_MEDIA_PATH)/AUTextura.cpp \
$(SRC_AUF_MEDIA_PATH)/NBGestorAnimaciones.cpp \
$(SRC_AUF_MEDIA_PATH)/NBGestorCuerpos.cpp \
$(SRC_AUF_MEDIA_PATH)/NBGestorEscena.cpp \
$(SRC_AUF_MEDIA_PATH)/NBGestorFisica.cpp \
$(SRC_AUF_MEDIA_PATH)/NBGestorGL.cpp \
$(SRC_AUF_MEDIA_PATH)/NBGestorIdioma.cpp \
$(SRC_AUF_MEDIA_PATH)/NBGestorPersonajes.cpp \
$(SRC_AUF_MEDIA_PATH)/NBGestorSonidos.cpp \
$(SRC_AUF_MEDIA_PATH)/NBGestorTeclas.cpp \
$(SRC_AUF_MEDIA_PATH)/NBGestorTexturas.cpp \
$(SRC_AUF_MEDIA_PATH)/NBGestorTouches.cpp \
$(SRC_AUF_MEDIA_PATH)/NBMngrFonts.cpp \
$(SRC_AUF_MEDIA_PATH)/NBSegmentadorFiguras.cpp \
$(SRC_AUF_MEDIA_PATH)/NBSimuladorLiquidosMotor1.cpp \
$(SRC_AUF_MEDIA_PATH)/NBSimuladorLiquidosMotor2.cpp

#----------------------------------
#- Compilar libreria
#----------------------------------
include $(BUILD_STATIC_LIBRARY)


