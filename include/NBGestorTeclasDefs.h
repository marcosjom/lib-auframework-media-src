//
//  AUGameplayTouches.h
//  EditorCuerpos
//
//  Created by Marcos Ortega Morales on 16/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBGestorTeclasDefs_H_INCLUIDO
#define NBGestorTeclasDefs_H_INCLUIDO

#include "AUObjeto.h"
#include "AUCadena.h"
#include "AUCadenaMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUEscenaObjeto.h"
#include "NBRango.h"

//Callbacks
typedef struct STMngrKeyboardCalls_ STMngrKeyboardCalls;
//
typedef bool (*PTRfuncKeyboardCreate)(void* app, STMngrKeyboardCalls* obj);
typedef bool (*PTRfuncKeyboardDestroy)(void* obj);
typedef bool (*PTRfuncKeyboardRequestFocus)(void* obj);
typedef bool (*PTRfuncKeyboardSetVisible)(void* obj, const bool visible);
typedef bool (*PTRfuncKeyboardRestartKeyboard)(void* obj);
typedef bool (*PTRfuncKeyboardUpdateCursor)(void* obj, const SI32 rngSelStart, const SI32 rngSelSz, const SI32 rngMrkStart, const SI32 rngMrkSz, const char* rngMrkText);
typedef bool (*PTRfuncKeyboardRestartInputPre)(void* obj, const BOOL textChange, const BOOL rngSelChange, const BOOL rngMrkChange);
typedef bool (*PTRfuncKeyboardRestartInputPost)(void* obj, const BOOL textChange, const BOOL rngSelChange, const BOOL rngMrkChange);

//

typedef struct STMngrKeyboardCalls_ {
	PTRfuncKeyboardCreate			funcCreate;
	void*							funcCreateParam;
	PTRfuncKeyboardDestroy			funcDestroy;
	void*							funcDestroyParam;
	//
	PTRfuncKeyboardRequestFocus		funcRequestFocus;
	void*							funcRequestFocusParam;
	PTRfuncKeyboardSetVisible		funcSetVisible;
	void*							funcSetVisibleParam;
	PTRfuncKeyboardRestartKeyboard	funcRestartKeyboard;
	void*							funcRestartKeyboardParam;
	PTRfuncKeyboardUpdateCursor		funcUpdateKeyboardCursor;
	void*							funcUpdateKeyboardCursorParam;
	PTRfuncKeyboardRestartInputPre	funcRestartInputPre;
	void*							funcRestartInputPreParam;
	PTRfuncKeyboardRestartInputPost	funcRestartInputPost;
	void*							funcRestartInputPostParam;
} STMngrKeyboardCalls;

//

typedef enum ENTextEditorType_ {
	ENTextEditorType_Literal = 0,	//Text will be literal
	ENTextEditorType_Password,		//Text will be literal (and special behavior; passchar is optional)
	ENTextEditorType_Compose,		//Text will be autocompleted
	ENTextEditorType_Email,			//Text will be literal (and special keyboard distribution)
	ENTextEditorType_PhoneNumber,	//Text will be numeric (and special keyboard distribution)
	ENTextEditorType_Integer,		//Integer number
	ENTextEditorType_Decimal,		//Decimal number
	ENTextEditorType_Name,			//Names (first letter capitalized)
	ENTextEditorType_Count
} ENTextEditorType;

typedef enum ENTextoDireccion_ {
	ENTextoDireccion_Arriba = 0,
	ENTextoDireccion_Abajo,
	ENTextoDireccion_Izquierda,
	ENTextoDireccion_Derecha
} ENTextoDireccion;

typedef enum ENTextRangeSet_ {
	ENTextRangeSet_None = 0,	//The composing area will be set after cursor range.
	ENTextRangeSet_Current,		//The composing area will be set as the current worked portion of text.
	ENTextRangeSet_Word			//The composing area will be set as the current autoselected word.
} ENTextRangeSet;

class ITextInputListener {
	public:
		virtual ~ITextInputListener(){
			//
		}
		virtual ENTextEditorType entradaEditorType(BOOL* dstIsMultiline) = 0;
		//
		virtual bool			entradaTieneTexto() = 0;
		virtual bool			entradaPermiteMultilinea() = 0;
		virtual void			entradaBackspace(const bool autoSelWord) = 0;
		virtual void			entradaIntroducirTexto(const char* textoIntroducir, const bool autoSelWord) = 0;
		//Rangos
		virtual NBRangoI		entradaRangoSeleccion() = 0;
		virtual NBRangoI		entradaRangoSeleccionAbs() = 0;
		virtual void			entradaRangoSeleccionEstablecer(const SI32 primerCharDef, const SI32 conteoCharDefs) = 0;
		virtual NBRangoI		entradaRangoMarcado() = 0;
		virtual NBRangoI		entradaRangoMarcadoAbs() = 0;
		virtual void			entradaRangoMarcadoEstablecer(const SI32 primerCharDef, const SI32 conteoCharDefs) = 0;
		virtual void			entradaRangoDesmarcar() = 0;
		virtual NBRangoI		entradaRangoLineaDeCharDef(const SI32 charDefPos) = 0;
		//Reemplazo y retorno de texto
		virtual void			entradaTexto(AUCadenaMutable8* dst) = 0;
		virtual void			entradaTextoMarcado(AUCadenaMutable8* dst) = 0;
		virtual void			entradaTextoMarcadoReemplazar(const char* nuevoTexto, const ENTextRangeSet setNewComposingAs) = 0;
		virtual void			entradaTextoSeleccion(AUCadenaMutable8* dst) = 0;
		virtual void			entradaTextoAntesDeSeleccion(const UI32 conteoCharDefs, AUCadenaMutable8* dst) = 0;
		virtual void			entradaTextoDespuesDeSeleccion(const UI32 conteoCharDefs, AUCadenaMutable8* dst) = 0;
		virtual void			entradaTextoSeleccionEliminarAlrededor(const SI32 conteoCharDefsAntes, const SI32 conteoCharDefsDespues, const bool autoSelWord) = 0;
		virtual void			entradaTextoEnCharDefsContenido(const UI32 primerCharDef, const UI32 conteoCharDefs, AUCadenaMutable8* dst) = 0;
		virtual void			entradaTextoEnCharDefsReemplazar(const UI32 primerCharDef, const UI32 conteoCharDefs, const char* nuevoTexto, const ENTextRangeSet setNewComposingAs) = 0;
		virtual bool			entradaTextoEnCharDefsPuedeReemplazarse(const UI32 primerCharDef, const UI32 conteoCharDefs, const char* nuevoTexto) = 0;
		//Geometria de texto
		virtual NBRectangulo	entradaRectanguloParaCursor(const UI32 charDefPos) = 0;
		virtual NBRectangulo	entradaPrimerRectanguloParaCharDefs(const UI32 primerCharDef, const UI32 conteoCharDefs) = 0;
		virtual void			entradaRectangulosParaCharDefs(const UI32 primerCharDef, const UI32 conteoCharDefs, AUArregloNativoMutableP<NBRectangulo>* dst) = 0;
		virtual UI32			entradaIndiceCharDefMasCercano(const float xViewPort, const float yViewPort) = 0;
		virtual UI32			entradaIndiceCharDefMasCercanoEnCharDefs(const float xLocalTexto, const float yLocalTexto, const UI32 primerCharDef, const UI32 conteoCharDefs) = 0;
		virtual	UI32			entradaIndiceCharDefPrimero() = 0;
		virtual UI32			entradaIndiceCharDefUltimo() = 0;
		virtual SI32			entradaIndiceCharDefDesdePosicion(const UI32 charDefPos, const SI32 movimiento, const ENTextoDireccion direccion) = 0;
};

typedef enum ENTextEditorSyncResult_ {
	ENTextEditorSyncResult_None = 0,		//The clone and the listener are the same.
	ENTextEditorSyncResult_CloneSynced,		//The clone was modified (with listener properties).
	ENTextEditorSyncResult_ListenerSynced	//The linester was modified (with clone properties).
} ENTextEditorSyncResult;

typedef struct STTextEditorSyncResult_ {
	ENTextEditorSyncResult	synced;
	BOOL					valueChanged;
	BOOL					cursorChanged;
} STTextEditorSyncResult;

typedef enum ENNBKeybFocusLostType_ {
	ENNBKeybFocusLostType_Unknown = 0,		//Unknown
	ENNBKeybFocusLostType_ProgramCall,		//Focus removed by a call (resign-focus or gain-focus)
	ENNBKeybFocusLostType_TouchAction,		//Focus lost by a touch action
	ENNBKeybFocusLostType_NewLineAction,	//Focus lost by a new line action
	ENNBKeybFocusLostType_Count
} ENNBKeybFocusLostType;

class ITextEditorListener {
	public:
		virtual ~ITextEditorListener(){
			//
		}
		//Cloning
		virtual void		textEditorCloneInterfaceObjectsRetained(AUObjeto** dstVisualObj, ITextInputListener** dstVisualItf, AUObjeto** dstValueObj, ITextInputListener** dstValueItf) = 0;
		virtual STTextEditorSyncResult textEditorCompareClones(AUObjeto* visualClone, AUObjeto* valueClone) = 0;
		virtual STTextEditorSyncResult textEditorSyncClones(AUObjeto* visualClone, AUObjeto* valueClone) = 0;
		//Focus
		virtual void		textEditorFocusGained() = 0;
		virtual void		textEditorFocusLost(const ENNBKeybFocusLostType actionType) = 0;
		virtual bool		textEditorMantainBeforeTouchConsumer(AUEscenaObjeto* touchConsumer) = 0;
};

class IEscuchadorTecladoVisual {
	public:
		virtual ~IEscuchadorTecladoVisual(){
			//
		}
		//Foco
		virtual void	tecladoVisualAltoModificado(const float altoPixeles) = 0;
};

/*enum ENGestorTeclasComando {
	ENGestorTeclasComando_Backspace = 0,
	ENGestorTeclasComando_IntroducirTexto
};*/

/*struct STGestorTeclasComando {
	ENGestorTeclasComando tipo;
	AUCadena8* strParam;
};*/

#endif
