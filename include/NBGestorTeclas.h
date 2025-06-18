//
//  AUGameplayTouches.h
//  EditorCuerpos
//
//  Created by Marcos Ortega Morales on 16/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBGestorTeclas_H_INCLUIDO
#define NBGestorTeclas_H_INCLUIDO

#include "AUObjeto.h"
#include "AUCadena.h"
#include "AUCadenaMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUEscenaObjeto.h"
#include "NBRango.h"
#include "NBGestorTeclasDefs.h"
#include "nb/core/NBThreadMutex.h"
#include "nb/core/NBThreadCond.h"

//

typedef struct STNBKeybEditor_ {
	AUObjeto*				obj;
	ITextEditorListener*	itf;
	AUObjeto*				visualClone;
	ITextInputListener*		visualItf;
	AUObjeto*				valueClone;
	ITextInputListener*		valueItf;
	STNBThreadMutex			mutex;
	BOOL					notifiedIn;
	BOOL					focusRemoved;
	ENNBKeybFocusLostType	focusRemovedAction;
	SI32					pushCount;		//pushed in batch
	SI32					retainCount;
} STNBKeybEditor;

//

class NBGestorTeclas {
	public:
		static void					inicializar();
		static void					finalizar();
		//
		static bool					gestorInicializado();
		static bool					isGlued();
		static bool					setGlue(void* app, PTRfuncKeyboardCreate functInit);
		//
		static void					keyboardWinFocusChanged(const bool v);
		static bool					keyboardIsVisible();
		static bool					keyboardShouldBeVisible();
		static void					keyboardSetVisible(const bool v);
		//
		static void					entradaActionsBashStart();
		static void					entradaActionsBashEnd();
		//
		static void					tickProcesarComandos();
		//
		static void					agregarEscuchadorVisual(IEscuchadorTecladoVisual* escuchador);
		static void					quitarEscuchadorVisual(IEscuchadorTecladoVisual* escuchador);
		//
		static bool					escuchadorExiste();
		static void					escuchadorEstablecer(AUObjeto* listenerObj, ITextEditorListener* listenerItf);
		static void					escuchadorRenunciar(ITextEditorListener* listenerItf);
		static void					escuchadorEvaluarMantenerAnteNuevoTouch(AUEscenaObjeto* consumidorTouch);
		static void					escuchadorRemover();
		//
		static void					dbgValidateObjsItfs(ITextInputListener* visItf, ITextInputListener* valItf);
		//
		static void					entradaLockForBatch();
		static void					entradaUnlockFromBatch();
		//
		static SI32					entradaExplicitBashPush();
		static SI32					entradaExplicitBashPop();
		//
		static ENTextEditorType		entradaEditorType(BOOL* dstIsMultiline);
		//
		static bool					entradaTieneTexto();
		static void					entradaBackspace(const bool autoSelWord);
		static void					entradaIntroducirTexto(const char* textoIntroducir, const bool autoSelWord);
		//Rangos
		static NBRangoI				entradaRangoSeleccion(); //in multibytes-char unit, not bytes
		static NBRangoI				entradaRangoSeleccionAbs();
		static void					entradaRangoSeleccionEstablecer(const SI32 primerCharDef, const SI32 conteoCharDefs);
		static NBRangoI				entradaRangoMarcado();
		static NBRangoI				entradaRangoMarcadoAbs();
		static void					entradaRangoMarcadoEstablecer(const SI32 primerCharDef, const SI32 conteoCharDefs);
		static void					entradaRangoDesmarcar();
		static NBRangoI				entradaRangoLineaDeCharDef(const SI32 charDefPos);
		//Reemplazo y retorno de texto
		static void					entradaTexto(AUCadenaMutable8* dst);
		static void					entradaTextoMarcado(AUCadenaMutable8* dst);
		static void					entradaTextoMarcadoReemplazar(const char* nuevoTexto, const ENTextRangeSet setNewComposingAs);
		static void					entradaTextoSeleccion(AUCadenaMutable8* dst);
		static void					entradaTextoAntesDeSeleccion(const UI32 conteoCharDefs, AUCadenaMutable8* dst);
		static void					entradaTextoDespuesDeSeleccion(const UI32 conteoCharDefs, AUCadenaMutable8* dst);
		static void					entradaTextoSeleccionEliminarAlrededor(const SI32 conteoCharDefsAntes, const SI32 conteoCharDefsDespues, const bool autoSelWord);
		static void					entradaTextoEnCharDefsContenido(const UI32 primerCharDef, const UI32 conteoCharDefs, AUCadenaMutable8* dst);
		static void					entradaTextoEnCharDefsReemplazar(const UI32 primerCharDef, const UI32 conteoCharDefs, const char* nuevoTexto, const ENTextRangeSet setNewComposingAs);
		static bool					entradaTextoEnCharDefsPuedeReemplazarse(const UI32 primerCharDef, const UI32 conteoCharDefs, const char* nuevoTexto);
		//Geometria de texto
		static NBRectangulo			entradaRectanguloParaCursor(const UI32 charDefPos);
		static NBRectangulo			entradaPrimerRectanguloParaCharDefs(const UI32 primerCharDef, const UI32 conteoCharDefs);
		static void					entradaRectangulosParaCharDefs(const UI32 primerCharDef, const UI32 conteoCharDefs, AUArregloNativoMutableP<NBRectangulo>* dst);
		static UI32					entradaIndiceCharDefMasCercano(const float xViewPort, const float yViewPort);
		static UI32					entradaIndiceCharDefMasCercanoEnCharDefs(const float xLocalTexto, const float yLocalTexto, const UI32 primerCharDef, const UI32 conteoCharDefs);
		static UI32					entradaIndiceCharDefPrimero();
		static UI32					entradaIndiceCharDefUltimo();
		static SI32					entradaIndiceCharDefDesdePosicion(const UI32 charDefPos, const SI32 movimiento, const ENTextoDireccion direccion);
		//
		static bool					tecladoEnPantallaSuperponeContenido();
		static float				tecladoEnPantallaAlto();
		static void					establecerTecladoEnPantallaAlto(const float alto, const bool superponeContenido);
	protected:
		static bool					_gestorInicializado;
		static bool					_inputMutexLckd;
		static STNBThreadMutex		_inputMutex;
		static STNBThreadCond		_inputCond;
		static SI32					_inputBashDepth;
		//Current listeners
		static BOOL					_editrFocusDirty;
		static STNBArray			_editrFocusStack;	//STNBKeybEditor*
		static STNBArray			_editrBashStack;	//STNBKeybEditor*
		//
		static bool					_keybWinHasFocus;
		static bool					_keybVisible;
		//
		static STMngrKeyboardCalls	_calls;
		//
		static float				_tecladoEnPantallaAltoUltNotif;
		static float				_tecladoEnPantallaAlto;
		static bool					_tecladoSuperponeContenido;
		//
		//
		static AUArregloNativoMutableP<IEscuchadorTecladoVisual*>* _escuchadoresVisual; //Se usa un buffer para asegurar que los comando se ejecuten dentro del hilo deseado
		//Editors stack
		static STNBKeybEditor*		privEditorGetCurrentRetainedLocked_();
		static void					privEditorReleaseAndFree(STNBKeybEditor* editor);
};

#endif
