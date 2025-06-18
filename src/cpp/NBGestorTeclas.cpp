//
//  AUGameplayTouches.cpp
//  EditorCuerpos
//
//  Created by Marcos Ortega Morales on 16/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "NBGestorTeclas.h"

bool				NBGestorTeclas::_gestorInicializado			= false;
//
bool				NBGestorTeclas::_inputMutexLckd				= false;
STNBThreadMutex		NBGestorTeclas::_inputMutex;
STNBThreadCond		NBGestorTeclas::_inputCond;
SI32				NBGestorTeclas::_inputBashDepth				= 0;	//Includes '_editrBashStack' and more
//
BOOL				NBGestorTeclas::_editrFocusDirty			= FALSE;
STNBArray			NBGestorTeclas::_editrFocusStack;			//STNBKeybEditor*
STNBArray			NBGestorTeclas::_editrBashStack;			//STNBKeybEditor*
//
bool				NBGestorTeclas::_keybWinHasFocus			= false;
bool				NBGestorTeclas::_keybVisible				= false;
STMngrKeyboardCalls NBGestorTeclas::_calls = {
	NULL, NULL
	, NULL, NULL
	, NULL, NULL
	, NULL, NULL
	, NULL, NULL
	, NULL, NULL
	, NULL, NULL
};
//
float				NBGestorTeclas::_tecladoEnPantallaAltoUltNotif = 0.0f;
float				NBGestorTeclas::_tecladoEnPantallaAlto		= 0.0f;
bool				NBGestorTeclas::_tecladoSuperponeContenido	= false;
//
AUArregloNativoMutableP<IEscuchadorTecladoVisual*>* NBGestorTeclas::_escuchadoresVisual = NULL;

#define NBGESTORTECLAS_MUTEX_ACTIVATE					NBThreadMutex_lock(&_inputMutex); NBASSERT(!_inputMutexLckd) _inputMutexLckd = TRUE;
#define NBGESTORTECLAS_MUTEX_ACTIVATE_WHEN_NO_BATCH		{ NBGESTORTECLAS_MUTEX_ACTIVATE while(_inputBashDepth > 0){ NBASSERT(_inputMutexLckd) _inputMutexLckd = FALSE; NBThreadCond_wait(&_inputCond, &_inputMutex); NBASSERT(!_inputMutexLckd) _inputMutexLckd = TRUE; } }
#define NBGESTORTECLAS_MUTEX_DEACTIVATE					NBASSERT(_inputMutexLckd) _inputMutexLckd = FALSE; NBThreadMutex_unlock(&_inputMutex);
#define NBGESTORTECLAS_MUTEX_ASSERT_LOCKED				NBASSERT(_inputMutexLckd)

//

void NBKeybEditor_init(STNBKeybEditor* obj){
	NBMemory_setZeroSt(*obj, STNBKeybEditor);
	NBThreadMutex_init(&obj->mutex);
	obj->retainCount	= 1;
}

void NBKeybEditor_set(STNBKeybEditor* obj, AUObjeto* pObj, ITextEditorListener* pItf){
	//Retain first
	{
		if(pObj != NULL) pObj->retener(NB_RETENEDOR_NULL);
	}
	//Release then
	{
		if(obj->valueClone != NULL) obj->valueClone->liberar(NB_RETENEDOR_NULL);
		obj->valueClone		= NULL;
		obj->valueItf		= NULL;
		//
		if(obj->visualClone != NULL) obj->visualClone->liberar(NB_RETENEDOR_NULL);
		obj->visualClone	= NULL;
		obj->visualItf		= NULL;
		//
		if(obj->obj != NULL) obj->obj->liberar(NB_RETENEDOR_NULL);
	}
	obj->obj			= pObj;
	obj->itf			= pItf;
}

void NBKeybEditor_retain(STNBKeybEditor* obj){
	NBThreadMutex_lock(&obj->mutex);
	{
		NBASSERT(obj->retainCount > 0)
		obj->retainCount++;
	}
	NBThreadMutex_unlock(&obj->mutex);
}

void NBKeybEditor_release(STNBKeybEditor* obj){
	NBThreadMutex_lock(&obj->mutex);
	NBASSERT(obj->retainCount > 0)
	obj->retainCount--;
	if(obj->retainCount > 0){
		NBThreadMutex_unlock(&obj->mutex);
	} else {
		{
			obj->valueItf	= NULL;
			obj->visualItf	= NULL;
			obj->itf		= NULL;
			if(obj->visualClone != NULL) obj->visualClone->liberar(NB_RETENEDOR_NULL); obj->visualClone = NULL;
			if(obj->valueClone != NULL) obj->valueClone->liberar(NB_RETENEDOR_NULL); obj->valueClone = NULL;
			if(obj->obj != NULL) obj->obj->liberar(NB_RETENEDOR_NULL); obj->obj = NULL;
		}
		NBThreadMutex_unlock(&obj->mutex);
		NBThreadMutex_release(&obj->mutex);
	}
}

//

void NBGestorTeclas::inicializar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTeclas::inicializar")
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	{
		//If fails, '_calls' initial values are not properly set to NULL.
		SI32 i; for(i = 0; i < sizeof(_calls); i++){
			NBASSERT(((BYTE*)&_calls)[i] == 0)
		}
	}
#	endif
	_keybWinHasFocus	= false;
	_keybVisible		= false;
	//Edit stack
	{
		_editrFocusDirty = TRUE;
		NBArray_init(&_editrFocusStack, sizeof(STNBKeybEditor*), NULL);
		NBArray_init(&_editrBashStack, sizeof(STNBKeybEditor*), NULL);
	}
	//
	_gestorInicializado	= true;
	_inputMutexLckd		= false;
	NBThreadMutex_init(&_inputMutex);
	NBThreadCond_init(&_inputCond);
	_inputBashDepth		= 0;
	
	_escuchadoresVisual	= new(ENMemoriaTipo_General) AUArregloNativoMutableP<IEscuchadorTecladoVisual*>();
	//
	_tecladoEnPantallaAlto		= 0.0f;
	_tecladoSuperponeContenido	= false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorTeclas::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTeclas::finalizar")
	//Destroy
	if(_calls.funcDestroy != NULL){
		if(!(*_calls.funcDestroy)(_calls.funcDestroyParam)){
			NBASSERT(false)
		}
		_calls.funcDestroy = NULL;
	}
	//Edit stack
	{
		{
			SI32 i; for(i = 0 ; i < _editrFocusStack.use; i++){
				STNBKeybEditor* editor = NBArray_itmValueAtIndex(&_editrFocusStack, STNBKeybEditor*, i);
				NBASSERT(editor != NULL)
				if(editor != NULL){
					NBGestorTeclas::privEditorReleaseAndFree(editor);
				}
			}
			NBArray_empty(&_editrFocusStack);
			NBArray_release(&_editrFocusStack);
		}
		{
			SI32 i; for(i = 0 ; i < _editrBashStack.use; i++){
				STNBKeybEditor* editor = NBArray_itmValueAtIndex(&_editrBashStack, STNBKeybEditor*, i);
				if(editor != NULL){
					NBGestorTeclas::privEditorReleaseAndFree(editor);
				}
			}
			NBArray_empty(&_editrBashStack);
			NBArray_release(&_editrBashStack);
		}
	}
	//
	if(_escuchadoresVisual != NULL) _escuchadoresVisual->liberar(NB_RETENEDOR_NULL); _escuchadoresVisual = NULL;
	//
	_gestorInicializado		= false;
	NBASSERT(_inputBashDepth == 0)
	NBThreadMutex_release(&_inputMutex);
	NBThreadCond_release(&_inputCond);
	_inputBashDepth		= 0;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorTeclas::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTeclas::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _gestorInicializado;
}

bool NBGestorTeclas::isGlued(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTeclas::isGlued")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return (_calls.funcCreate != NULL);
}

bool NBGestorTeclas::setGlue(void* app, PTRfuncKeyboardCreate functInit){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTeclas::setGlue")
	bool r = false;
	//Destroy
	if(_calls.funcDestroy != NULL){
		if(!(*_calls.funcDestroy)(_calls.funcDestroyParam)){
			NBASSERT(false)
		}
		_calls.funcDestroy = NULL;
	}
	//Create
	if(functInit != NULL){
		if(!(*functInit)(app, &_calls)){
			NBASSERT(false)
		}else {
			r = true;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

//

void NBGestorTeclas::entradaLockForBatch(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTeclas::entradaLockForBatch")
	NBGESTORTECLAS_MUTEX_ACTIVATE
	NBASSERT(_inputBashDepth >= 0)
	_inputBashDepth++;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorTeclas::entradaUnlockFromBatch(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTeclas::entradaUnlockFromBatch")
	NBASSERT(_inputBashDepth > 0)
	_inputBashDepth--;
	NBGESTORTECLAS_MUTEX_DEACTIVATE
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}


//

void NBGestorTeclas::keyboardWinFocusChanged(const bool v){
	//PRINTF_WARNING("keyboardWinFocusChanged(%d).\n", v);
	if(_keybWinHasFocus != v){
		_keybWinHasFocus = v;
		//Hide keyboard when lossing focus
		if(!v && _keybVisible){
			PRINTF_INFO("Hidding keyboard because keyboardWindow lost focus.\n");
			NBGestorTeclas::keyboardSetVisible(false);
		}
	}
}

bool NBGestorTeclas::keyboardIsVisible(){
	return _keybVisible;
}

bool NBGestorTeclas::keyboardShouldBeVisible(){
	bool r = FALSE;
	//Without lock (not necesary?)
	{
		//PRINTF_INFO("_keybWinHasFocus(%d), _editrFocusStack(%d).\n", _keybWinHasFocus, _editrFocusStack.use);
		r = (_keybWinHasFocus && _editrFocusStack.use > 0);
	}
	return r;
}

void NBGestorTeclas::keyboardSetVisible(const bool newVisible){
	//Show-hide keyboard
	if(_calls.funcSetVisible != NULL){
		if(_keybVisible != newVisible){
			//PRINTF_WARNING("NBGestorTeclas::keyboardSetVisible - applyingVisible(%d).\n", newVisible);
			if((*_calls.funcSetVisible)(_calls.funcSetVisibleParam, newVisible)){
				_keybVisible = newVisible;
				PRINTF_WARNING("keyboard funcSetVisible to (%d) - called at escuchadorEstablecer.\n", newVisible);
			}
		}
	}
}

//

#ifdef NB_CONFIG_INCLUDE_ASSERTS
void NBGestorTeclas::dbgValidateObjsItfs(ITextInputListener* visItf, ITextInputListener* valItf){
	NBASSERT(_gestorInicializado)
	if(visItf != NULL && valItf != NULL && visItf != valItf){
		//Ranges
		const NBRangoI mVis = visItf->entradaRangoMarcado();
		const NBRangoI sVis = visItf->entradaRangoSeleccion();
		//
		const NBRangoI mVal	= valItf->entradaRangoMarcado();
		const NBRangoI sVal	= valItf->entradaRangoSeleccion();
		//
		const SI32 visIni	= visItf->entradaIndiceCharDefPrimero();
		const SI32 valIni	= valItf->entradaIndiceCharDefPrimero();
		//
		const SI32 visLen	= visItf->entradaIndiceCharDefUltimo();
		const SI32 valLen	= valItf->entradaIndiceCharDefUltimo();
		//
		NBASSERT(visIni == valIni)
		NBASSERT(visLen == valLen)
		NBASSERT(mVis.inicio == mVal.inicio && mVis.tamano == mVal.tamano)
		NBASSERT(sVis.inicio == sVal.inicio && sVis.tamano == sVal.tamano)
	}
}
#endif

bool NBGestorTeclas::escuchadorExiste(){
	NBASSERT(_gestorInicializado)
	bool r = FALSE;
	NBGESTORTECLAS_MUTEX_ACTIVATE
	{
		STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
		if(topEdit != NULL){
			r = TRUE;
			NBGestorTeclas::privEditorReleaseAndFree(topEdit);
		}
	}
	NBGESTORTECLAS_MUTEX_DEACTIVATE
	return r;
}

void NBGestorTeclas::escuchadorEstablecer(AUObjeto* listenerObj, ITextEditorListener* listenerItf){
	NBASSERT(_gestorInicializado)
	if(listenerItf != NULL){
		NBGESTORTECLAS_MUTEX_ACTIVATE
		{
			//Add to the top of the focus stack
			STNBKeybEditor* editor = NBMemory_allocType(STNBKeybEditor);
			NBKeybEditor_init(editor);
			NBKeybEditor_set(editor, listenerObj, listenerItf);
			NBArray_addValue(&_editrFocusStack, editor);
			_editrFocusDirty	= TRUE;
		}
		NBGESTORTECLAS_MUTEX_DEACTIVATE
	}
}

void NBGestorTeclas::escuchadorRenunciar(ITextEditorListener* listenerItf){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ACTIVATE
	if(_editrFocusStack.use > 0){
		STNBKeybEditor* topFocus = NBArray_itmValueAtIndex(&_editrFocusStack, STNBKeybEditor*, _editrFocusStack.use - 1);
		NBASSERT(topFocus != NULL)
		if(topFocus != NULL){
			if(topFocus->itf == listenerItf){
				topFocus->focusRemoved			= TRUE;
				topFocus->focusRemovedAction	= ENNBKeybFocusLostType_ProgramCall;
				_editrFocusDirty		= TRUE;
			}
		}
	}
	NBGESTORTECLAS_MUTEX_DEACTIVATE
}

void NBGestorTeclas::escuchadorEvaluarMantenerAnteNuevoTouch(AUEscenaObjeto* consumidorTouch){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ACTIVATE
	if(_editrFocusStack.use > 0){
		STNBKeybEditor* topFocus = NBArray_itmValueAtIndex(&_editrFocusStack, STNBKeybEditor*, _editrFocusStack.use - 1);
		NBASSERT(topFocus != NULL)
		if(topFocus != NULL){
			if(topFocus->itf != NULL){
				if(!topFocus->itf->textEditorMantainBeforeTouchConsumer(consumidorTouch)){
					topFocus->focusRemoved			= TRUE;
					topFocus->focusRemovedAction	= ENNBKeybFocusLostType_TouchAction;
					_editrFocusDirty				= TRUE;
				}
			}
		}
	}
	NBGESTORTECLAS_MUTEX_DEACTIVATE
}

void NBGestorTeclas::escuchadorRemover(){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ACTIVATE
	if(_editrFocusStack.use > 0){
		SI32 i; for(i = 0; i < _editrFocusStack.use; i++){
			STNBKeybEditor* editor = NBArray_itmValueAtIndex(&_editrFocusStack, STNBKeybEditor*, i);
			NBASSERT(editor != NULL)
			if(editor != NULL){
				editor->focusRemoved		= TRUE;
				editor->focusRemovedAction	= ENNBKeybFocusLostType_ProgramCall;
				_editrFocusDirty			= TRUE;
			}
		}
	}
	NBGESTORTECLAS_MUTEX_DEACTIVATE
}

//

ENTextEditorType NBGestorTeclas::entradaEditorType(BOOL* dstIsMultiline){
	ENTextEditorType r = ENTextEditorType_Count;
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			r = topEdit->valueItf->entradaEditorType(dstIsMultiline);
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
	return r;
}

//

SI32 NBGestorTeclas::entradaExplicitBashPush(){
	SI32 r = 0;
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	{
		r = _editrBashStack.use;
		{
			//Added retained
			STNBKeybEditor* editor = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
			if(editor != NULL){
				NBASSERT(editor->pushCount >= 0)
				editor->pushCount++;
			}
			NBArray_addValue(&_editrBashStack, editor); //null is accepted
			PRINTF_INFO("NBGestorTeclas, explicit actions-bash STARTED: level %d%s.\n", r, (editor == NULL ? " (NULL-EDITOR)" : ""));
		}
		_inputBashDepth++;
		NBASSERT(_editrBashStack.use <= _inputBashDepth)
		NBThreadCond_broadcast(&_inputCond);
	}
	return r;
}

SI32 NBGestorTeclas::entradaExplicitBashPop(){
	SI32 r = -1;
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	{
		NBASSERT(_editrBashStack.use > 0)
		if(_editrBashStack.use <= 0){
			PRINTF_ERROR("NBGestorTeclas, explicit actions-bash ENDED when none STARTED remain.\n");
		} else {
			r = _editrBashStack.use - 1;
			{
				STNBKeybEditor* editor = NBArray_itmValueAtIndex(&_editrBashStack, STNBKeybEditor*, _editrBashStack.use - 1);
				if(editor != NULL){
					NBASSERT(editor->pushCount > 0)
					editor->pushCount--;
					NBGestorTeclas::privEditorReleaseAndFree(editor);
				}
				NBArray_removeItemAtIndex(&_editrBashStack, _editrBashStack.use - 1);
				PRINTF_INFO("NBGestorTeclas, explicit actions-bash STARTED: level %d%s.\n", r, (editor == NULL ? " (NULL-EDITOR)" : ""));
			}
		}
		_inputBashDepth--;
		NBASSERT(_editrBashStack.use <= _inputBashDepth)
		NBThreadCond_broadcast(&_inputCond);
	}
	return r;
}

//<<TextInput>> Rangos
NBRangoI NBGestorTeclas::entradaRangoSeleccion(){
	NBRangoI r; NBMemory_setZeroSt(r, NBRangoI);
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			r = topEdit->valueItf->entradaRangoSeleccion();
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
	return r;
}

NBRangoI NBGestorTeclas::entradaRangoSeleccionAbs(){
	NBRangoI r; NBMemory_setZeroSt(r, NBRangoI);
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			r = topEdit->valueItf->entradaRangoSeleccionAbs();
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
	return r;
}

void NBGestorTeclas::entradaRangoSeleccionEstablecer(const SI32 primerCharDef, const SI32 conteoCharDefs){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			topEdit->valueItf->entradaRangoSeleccionEstablecer(primerCharDef, conteoCharDefs);
			//Also apply to visual (if different)
			if(topEdit->visualItf != NULL && topEdit->visualItf != topEdit->valueItf){
				topEdit->visualItf->entradaRangoSeleccionEstablecer(primerCharDef, conteoCharDefs);
			}
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
}

NBRangoI NBGestorTeclas::entradaRangoMarcado(){
	NBRangoI r; NBMemory_setZeroSt(r, NBRangoI);
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			r = topEdit->valueItf->entradaRangoMarcado();
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
	return r;
}

NBRangoI NBGestorTeclas::entradaRangoMarcadoAbs(){
	NBRangoI r; NBMemory_setZeroSt(r, NBRangoI);
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			r = topEdit->valueItf->entradaRangoMarcadoAbs();
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
	return r;
}

void NBGestorTeclas::entradaRangoMarcadoEstablecer(const SI32 primerCharDef, const SI32 conteoCharDefs){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			topEdit->valueItf->entradaRangoMarcadoEstablecer(primerCharDef, conteoCharDefs);
			//Also apply to visual (if different)
			if(topEdit->visualItf != NULL && topEdit->visualItf != topEdit->valueItf){
				topEdit->visualItf->entradaRangoSeleccionEstablecer(primerCharDef, conteoCharDefs);
			}
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
}

void NBGestorTeclas::entradaRangoDesmarcar(){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			topEdit->valueItf->entradaRangoDesmarcar();
			//Also apply to visual (if different)
			if(topEdit->visualItf != NULL && topEdit->visualItf != topEdit->valueItf){
				topEdit->visualItf->entradaRangoDesmarcar();
			}
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
}

NBRangoI NBGestorTeclas::entradaRangoLineaDeCharDef(const SI32 charDefPos){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	NBRangoI r; NBMemory_setZeroSt(r, NBRangoI);
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			r = topEdit->valueItf->entradaRangoLineaDeCharDef(charDefPos);
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
	return r;
}

//<<TextInput>> Reemplazo y retorno de texto

void NBGestorTeclas::entradaTexto(AUCadenaMutable8* dst){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(dst != NULL){
			dst->vaciar();
			if(topEdit->valueItf != NULL){
				topEdit->valueItf->entradaTexto(dst);
			}
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
}

void NBGestorTeclas::entradaTextoMarcado(AUCadenaMutable8* dst){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(dst != NULL){
			dst->vaciar();
			if(topEdit->valueItf != NULL && topEdit->valueItf != NULL){
				topEdit->valueItf->entradaTextoMarcado(dst);
			}
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
}

void NBGestorTeclas::entradaTextoMarcadoReemplazar(const char* nuevoTexto, const ENTextRangeSet setNewComposingAs){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			topEdit->valueItf->entradaTextoMarcadoReemplazar(nuevoTexto, setNewComposingAs);
			//Also apply to visual (if different)
			if(topEdit->visualItf != NULL && topEdit->visualItf != topEdit->valueItf){
				topEdit->visualItf->entradaTextoMarcadoReemplazar(nuevoTexto, setNewComposingAs);
			}
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
}

void NBGestorTeclas::entradaTextoSeleccion(AUCadenaMutable8* dst){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(dst != NULL){
			dst->vaciar();
			if(topEdit->valueItf != NULL){
				topEdit->valueItf->entradaTextoSeleccion(dst);
			}
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
}

void NBGestorTeclas::entradaTextoAntesDeSeleccion(const UI32 conteoCharDefs, AUCadenaMutable8* dst){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(dst != NULL){
			dst->vaciar();
			if(topEdit->valueItf != NULL){
				topEdit->valueItf->entradaTextoAntesDeSeleccion(conteoCharDefs, dst);
			}
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
}

void NBGestorTeclas::entradaTextoDespuesDeSeleccion(const UI32 conteoCharDefs, AUCadenaMutable8* dst){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(dst != NULL){
			dst->vaciar();
			if(topEdit->valueItf != NULL){
				topEdit->valueItf->entradaTextoDespuesDeSeleccion(conteoCharDefs, dst);
			}
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
}

void NBGestorTeclas::entradaTextoSeleccionEliminarAlrededor(const SI32 conteoCharDefsAntes, const SI32 conteoCharDefsDespues, const bool autoSelWord){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			topEdit->valueItf->entradaTextoSeleccionEliminarAlrededor(conteoCharDefsAntes, conteoCharDefsDespues, autoSelWord);
			//Also apply to visual (if different)
			if(topEdit->visualItf != NULL && topEdit->visualItf != topEdit->valueItf){
				topEdit->visualItf->entradaTextoSeleccionEliminarAlrededor(conteoCharDefsAntes, conteoCharDefsDespues, autoSelWord);
				//Apply autoSelWord's behavoir from visualItf to valueItf.
				{
					const NBRangoI m = topEdit->visualItf->entradaRangoMarcado();
					const NBRangoI s = topEdit->visualItf->entradaRangoSeleccion();
					topEdit->valueItf->entradaRangoMarcadoEstablecer(m.inicio, m.tamano);
					topEdit->valueItf->entradaRangoSeleccionEstablecer(s.inicio, s.tamano);
				}
			}
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
}

void NBGestorTeclas::entradaTextoEnCharDefsContenido(const UI32 primerCharDef, const UI32 conteoCharDefs, AUCadenaMutable8* dst){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(dst != NULL){
			dst->vaciar();
			if(topEdit->valueItf != NULL){
				topEdit->valueItf->entradaTextoEnCharDefsContenido(primerCharDef, conteoCharDefs, dst);
			}
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
}

void NBGestorTeclas::entradaTextoEnCharDefsReemplazar(const UI32 primerCharDef, const UI32 conteoCharDefs, const char* nuevoTexto, const ENTextRangeSet setNewComposingAs){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			topEdit->valueItf->entradaTextoEnCharDefsReemplazar(primerCharDef, conteoCharDefs, nuevoTexto, setNewComposingAs);
			//Also apply to visual (if different)
			if(topEdit->visualItf != NULL && topEdit->visualItf != topEdit->valueItf){
				topEdit->visualItf->entradaTextoEnCharDefsReemplazar(primerCharDef, conteoCharDefs, nuevoTexto, setNewComposingAs);
			}
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
}

bool NBGestorTeclas::entradaTextoEnCharDefsPuedeReemplazarse(const UI32 primerCharDef, const UI32 conteoCharDefs, const char* nuevoTexto){
	bool r = false;
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			r = topEdit->valueItf->entradaTextoEnCharDefsPuedeReemplazarse(primerCharDef, conteoCharDefs, nuevoTexto);
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
	return r;
}

//<<TextInput>> Geometria de texto
NBRectangulo NBGestorTeclas::entradaRectanguloParaCursor(const UI32 charDefPos){
	NBRectangulo r; NBMemory_setZeroSt(r, NBRectangulo);
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->visualItf != NULL){
			r = topEdit->visualItf->entradaRectanguloParaCursor(charDefPos);
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
	return r;
}

NBRectangulo NBGestorTeclas::entradaPrimerRectanguloParaCharDefs(const UI32 primerCharDef, const UI32 conteoCharDefs){
	NBRectangulo r; NBMemory_setZeroSt(r, NBRectangulo);
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->visualItf != NULL){
			r = topEdit->visualItf->entradaPrimerRectanguloParaCharDefs(primerCharDef, conteoCharDefs);
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
	return r;
}

void NBGestorTeclas::entradaRectangulosParaCharDefs(const UI32 primerCharDef, const UI32 conteoCharDefs, AUArregloNativoMutableP<NBRectangulo>* dst){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(dst != NULL){
			dst->vaciar();
			if(topEdit->visualItf != NULL){
				topEdit->visualItf->entradaRectangulosParaCharDefs(primerCharDef, conteoCharDefs, dst);
			}
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
}

UI32 NBGestorTeclas::entradaIndiceCharDefMasCercano(const float xViewPort, const float yViewPort){
	UI32 r = 0;
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->visualItf != NULL){
			r = topEdit->visualItf->entradaIndiceCharDefMasCercano(xViewPort, yViewPort);
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
	return r;
}

UI32 NBGestorTeclas::entradaIndiceCharDefMasCercanoEnCharDefs(const float xLocalTexto, const float yLocalTexto, const UI32 primerCharDef, const UI32 conteoCharDefs){
	UI32 r = 0;
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->visualItf != NULL){
			r = topEdit->visualItf->entradaIndiceCharDefMasCercanoEnCharDefs(xLocalTexto, yLocalTexto, primerCharDef, conteoCharDefs);
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
	return r;
}

UI32 NBGestorTeclas::entradaIndiceCharDefPrimero(){
	UI32 r = 0;
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->visualItf != NULL){
			r = topEdit->visualItf->entradaIndiceCharDefPrimero();
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
	return r;
}

UI32 NBGestorTeclas::entradaIndiceCharDefUltimo(){
	UI32 r = 0;
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->visualItf != NULL){
			r = topEdit->visualItf->entradaIndiceCharDefUltimo();
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
	return r;
}

SI32 NBGestorTeclas::entradaIndiceCharDefDesdePosicion(const UI32 charDefPos, const SI32 movimiento, const ENTextoDireccion direccion){
	SI32 r = -1;
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->visualItf != NULL){
			r = topEdit->visualItf->entradaIndiceCharDefDesdePosicion(charDefPos, movimiento, direccion);
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
	return r;
}

bool NBGestorTeclas::entradaTieneTexto(){
	bool r = false;
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			r = topEdit->valueItf->entradaTieneTexto();
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
	return r;
}

void NBGestorTeclas::entradaBackspace(const bool autoSelWord){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			topEdit->valueItf->entradaBackspace(autoSelWord);
			//Also apply to visual (if different)
			if(topEdit->visualItf != NULL && topEdit->visualItf != topEdit->valueItf){
				topEdit->visualItf->entradaBackspace(autoSelWord);
				//Apply autoSelWord's behavoir from visualItf to valueItf.
				{
					const NBRangoI m = topEdit->visualItf->entradaRangoMarcado();
					const NBRangoI s = topEdit->visualItf->entradaRangoSeleccion();
					topEdit->valueItf->entradaRangoMarcadoEstablecer(m.inicio, m.tamano);
					topEdit->valueItf->entradaRangoSeleccionEstablecer(s.inicio, s.tamano);
				}
			}
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
}

void NBGestorTeclas::entradaIntroducirTexto(const char* textoIntroducir, const bool autoSelWord){
	NBASSERT(_gestorInicializado)
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	STNBKeybEditor* topEdit = NBGestorTeclas::privEditorGetCurrentRetainedLocked_();
	if(topEdit != NULL){
		if(topEdit->valueItf != NULL){
			//Detect auto-finish writing
			if(textoIntroducir != NULL){
				if(textoIntroducir[0] == '\t' || textoIntroducir[0] == '\r' || textoIntroducir[0] == '\n'){
					if(!topEdit->valueItf->entradaPermiteMultilinea()){
						topEdit->focusRemoved = TRUE;
						topEdit->focusRemovedAction	= ENNBKeybFocusLostType_NewLineAction;
					}
				}
			}
			//
			if(topEdit->valueItf != NULL){
				topEdit->valueItf->entradaIntroducirTexto(textoIntroducir, autoSelWord);
				//Also apply to visual (if different)
				if(topEdit->visualItf != NULL && topEdit->visualItf != topEdit->valueItf){
					topEdit->visualItf->entradaIntroducirTexto(textoIntroducir, autoSelWord);
					//Apply autoSelWord's behavoir from visualItf to valueItf.
					{
						const NBRangoI m = topEdit->visualItf->entradaRangoMarcado();
						const NBRangoI s = topEdit->visualItf->entradaRangoSeleccion();
						topEdit->valueItf->entradaRangoMarcadoEstablecer(m.inicio, m.tamano);
						topEdit->valueItf->entradaRangoSeleccionEstablecer(s.inicio, s.tamano);
					}
				}
			}
		}
		//
		NBGestorTeclas::privEditorReleaseAndFree(topEdit);
	}
}

//

void NBGestorTeclas::agregarEscuchadorVisual(IEscuchadorTecladoVisual* escuchador){
	NBASSERT(_gestorInicializado)
	if(_escuchadoresVisual->indiceDe(escuchador) == -1){
		_escuchadoresVisual->agregarElemento(escuchador);
	}
}

void NBGestorTeclas::quitarEscuchadorVisual(IEscuchadorTecladoVisual* escuchador){
	NBASSERT(_gestorInicializado)
	_escuchadoresVisual->quitarElemento(escuchador);
}

//

void NBGestorTeclas::tickProcesarComandos(){
	NBASSERT(_gestorInicializado)
	{
		//Lock when no bacths available
		NBGESTORTECLAS_MUTEX_ACTIVATE_WHEN_NO_BATCH
		//Process focus stack untill no changes
		{
			_editrFocusDirty = TRUE;
			while(_editrFocusDirty){
				_editrFocusDirty		= FALSE;
				{
					BOOL addedToBatchStack = FALSE;
					STNBKeybEditor* focusFnd  = NULL;
					//Notify all-focus changes and determine topEdit
					//PRINTF_INFO("NBGestorTeclas, start-of-focus analysis with focus-stack-siz(%d) batch-stach-sz(%d).\n", _editrFocusStack.use, _editrBashStack.use);
					{
						while(_editrFocusStack.use > 0){
							STNBKeybEditor* oldst = NBArray_itmValueAtIndex(&_editrFocusStack, STNBKeybEditor*, 0);
							NBASSERT(oldst != NULL)
							if(_editrFocusStack.use == 1 && !oldst->focusRemoved){
								//PRINTF_INFO("NBGestorTeclas, last-focus-obj; focus-stack-siz(%d) batch-stach-sz(%d).\n", _editrFocusStack.use, _editrBashStack.use);
								//Should be the next focus
								if(!oldst->notifiedIn){
									//Notify focusGain (unlocked)
									//PRINTF_INFO("NBGestorTeclas, notifying 'textEditorFocusGained'; focus-stack-siz(%d) batch-stach-sz(%d).\n", _editrFocusStack.use, _editrBashStack.use);
									oldst->notifiedIn = TRUE;
									if(oldst->itf != NULL){
										NBGESTORTECLAS_MUTEX_DEACTIVATE
										{
											oldst->itf->textEditorFocusGained();
										}
										NBGESTORTECLAS_MUTEX_ACTIVATE_WHEN_NO_BATCH
									}
								}
								//Last notification did not changed the stack
								if(_editrFocusStack.use == 1){
									focusFnd = oldst;
									NBKeybEditor_retain(focusFnd);
									//PRINTF_INFO("NBGestorTeclas, keeping last-focus-obj as current-focus; focus-stack-siz(%d) batch-stach-sz(%d).\n", _editrFocusStack.use, _editrBashStack.use);
									break;
								}
							} else {
								//
								if(!oldst->focusRemoved){
									oldst->focusRemoved			= TRUE;
									oldst->focusRemovedAction	= ENNBKeybFocusLostType_ProgramCall;
								}
								//
								if(!oldst->notifiedIn){
									//Just remove
									//PRINTF_INFO("NBGestorTeclas, removing, was not notified; focus-stack-siz(%d) batch-stach-sz(%d).\n", _editrFocusStack.use, _editrBashStack.use);
								} else {
									//Notify focusLost (unlocked)
									//PRINTF_INFO("NBGestorTeclas, notifying 'textEditorFocusLost' before removing; focus-stack-siz(%d) batch-stach-sz(%d).\n", _editrFocusStack.use, _editrBashStack.use);
									if(oldst->itf != NULL){
										NBGESTORTECLAS_MUTEX_DEACTIVATE
										{
											oldst->itf->textEditorFocusLost(oldst->focusRemovedAction);
										}
										NBGESTORTECLAS_MUTEX_ACTIVATE_WHEN_NO_BATCH
									}
								}
								NBGestorTeclas::privEditorReleaseAndFree(oldst);
								NBArray_removeItemAtIndex(&_editrFocusStack, 0);
							}
						}
					}
					//Update batch-stack
					{
						//Remove from batch stack any btm removed from focus
						while(_editrBashStack.use > 0) {
							STNBKeybEditor* topEdit = NBArray_itmValueAtIndex(&_editrBashStack, STNBKeybEditor*, 0);
							if(topEdit == NULL){
								break;
							} else if(!topEdit->focusRemoved || topEdit->pushCount > 0){
								break;
							} else {
								NBASSERT(topEdit->pushCount == 0)
								//PRINTF_INFO("NBGestorTeclas, removing oldest from batch-stack; focus-stack-siz(%d) batch-stach-sz(%d).\n", _editrFocusStack.use, _editrBashStack.use);
								NBGestorTeclas::privEditorReleaseAndFree(topEdit);
								NBArray_removeItemAtIndex(&_editrBashStack, 0);
							}
						}
						//Add topEdit to the begginig of the batch stack
						if(focusFnd != NULL){
							if(!focusFnd->focusRemoved){
								BOOL isAtBtm = FALSE;
								if(_editrBashStack.use > 0){
									STNBKeybEditor* btmBatch = NBArray_itmValueAtIndex(&_editrBashStack, STNBKeybEditor*, 0);
									if(btmBatch == focusFnd){
										isAtBtm = TRUE;
									}
								}
								if(!isAtBtm){
									NBASSERT(focusFnd->pushCount == 0) //must be added with depth-0
									//PRINTF_INFO("NBGestorTeclas, setting as oldest batch-ops-destination; focus-stack-siz(%d) batch-stach-sz(%d).\n", _editrFocusStack.use, _editrBashStack.use);
									//Retain
									NBKeybEditor_retain(focusFnd);
									//Add at btm of batch
									NBArray_addItemsAtIndex(&_editrBashStack, 0, &focusFnd, sizeof(focusFnd), 1);
									addedToBatchStack = TRUE;
								}
							}
						}
						//Sync current batch
						if(_editrBashStack.use > 0){
							STNBKeybEditor* topEdit = NBArray_itmValueAtIndex(&_editrBashStack, STNBKeybEditor*, _editrBashStack.use - 1);
							if(topEdit != NULL){
								if(topEdit->itf != NULL){
									if(topEdit->valueItf == NULL){
										//Clone
										//PRINTF_INFO("NBGestorTeclas, initializing clones; focus-stack-siz(%d) batch-stach-sz(%d).\n", _editrFocusStack.use, _editrBashStack.use);
										NBGESTORTECLAS_MUTEX_DEACTIVATE
										{
											topEdit->itf->textEditorCloneInterfaceObjectsRetained(&topEdit->visualClone, &topEdit->visualItf, &topEdit->valueClone, &topEdit->valueItf);
											if(_calls.funcRestartKeyboard != NULL){
												if((*_calls.funcRestartKeyboard)(_calls.funcRestartKeyboardParam)){
													//
												}
											}
										}
										NBGESTORTECLAS_MUTEX_ACTIVATE_WHEN_NO_BATCH
									} else {
										//Sync
										//PRINTF_INFO("NBGestorTeclas, syncing clones; focus-stack-siz(%d) batch-stach-sz(%d).\n", _editrFocusStack.use, _editrBashStack.use);
										STTextEditorSyncResult syncResult;
										NBMemory_setZeroSt(syncResult, STTextEditorSyncResult);
										syncResult.synced	= ENTextEditorSyncResult_None;
										NBGESTORTECLAS_MUTEX_DEACTIVATE
										{
											STTextEditorSyncResult syncCompare;
											NBMemory_setZeroSt(syncCompare, STTextEditorSyncResult);
											syncCompare.synced	= ENTextEditorSyncResult_None;
											//Compare
											{
												syncCompare		= topEdit->itf->textEditorCompareClones(topEdit->visualClone, topEdit->valueClone);
											}
											//Notify pre-change
											{
												if(syncCompare.synced == ENTextEditorSyncResult_CloneSynced){
													if(_calls.funcRestartInputPre != NULL){
														if((*_calls.funcRestartInputPre)(_calls.funcRestartInputPreParam, syncCompare.valueChanged, syncCompare.cursorChanged, syncCompare.cursorChanged)){
															//
														}
													}
												}
											}
											//Sync
											{
												syncResult = topEdit->itf->textEditorSyncClones(topEdit->visualClone, topEdit->valueClone);
												//Update composing and selection ranges
												if(syncResult.synced == ENTextEditorSyncResult_CloneSynced){
													if(_calls.funcUpdateKeyboardCursor != NULL && topEdit->valueItf != NULL){
														AUCadenaMutable8* mrkTxt = new AUCadenaMutable8();
														const NBRangoI rngSel = topEdit->valueItf->entradaRangoSeleccion();
														const NBRangoI rngMrk = topEdit->valueItf->entradaRangoMarcado();
														topEdit->valueItf->entradaTextoMarcado(mrkTxt);
														if((*_calls.funcUpdateKeyboardCursor)(_calls.funcUpdateKeyboardCursorParam, rngSel.inicio, rngSel.tamano, rngMrk.inicio, rngMrk.tamano, mrkTxt->str())){
															//
														}
														mrkTxt->liberar(NB_RETENEDOR_NULL);
														mrkTxt = NULL;
													}
												}
											}
											//Notify prost change
											{
												if(syncCompare.synced == ENTextEditorSyncResult_CloneSynced){
													if(_calls.funcRestartInputPost != NULL){
														if((*_calls.funcRestartInputPost)(_calls.funcRestartInputPostParam, syncCompare.valueChanged, syncCompare.cursorChanged, syncCompare.cursorChanged)){
															//
														}
													}
												}
											}
										}
										NBGESTORTECLAS_MUTEX_ACTIVATE_WHEN_NO_BATCH
									}
								}
							}
						}
					}
					if(focusFnd != NULL){
						//PRINTF_INFO("NBGestorTeclas, end-of-focus analysis clones; focus-stack-siz(%d) batch-stach-sz(%d).\n", _editrFocusStack.use, _editrBashStack.use);
					}
					//Request focus
					if(addedToBatchStack){
						NBGESTORTECLAS_MUTEX_DEACTIVATE
						{
							if(_calls.funcRequestFocus != NULL){
								//PRINTF_WARNING("NBGestorTeclas, requesting focus.\n");
								(*_calls.funcRequestFocus)(_calls.funcRequestFocusParam);
							}
						}
						NBGESTORTECLAS_MUTEX_ACTIVATE_WHEN_NO_BATCH
					}
					NBGestorTeclas::privEditorReleaseAndFree(focusFnd);
				}
			}
		}
		NBGESTORTECLAS_MUTEX_DEACTIVATE
	}
	//--------------------------------
	//-- Notificar cambio de altura de teclado visual
	//--------------------------------
	{
		if(_tecladoEnPantallaAltoUltNotif != _tecladoEnPantallaAlto){
			_tecladoEnPantallaAltoUltNotif = _tecladoEnPantallaAlto;
			//
			SI32 i;
			for(i = (SI32)_escuchadoresVisual->conteo - 1; i>= 0; i--){
				IEscuchadorTecladoVisual* escuchador = _escuchadoresVisual->elem(i);
				escuchador->tecladoVisualAltoModificado(_tecladoEnPantallaAltoUltNotif);
			}
		}
	}
}

//

bool NBGestorTeclas::tecladoEnPantallaSuperponeContenido(){
	NBASSERT(_gestorInicializado)
	return _tecladoSuperponeContenido;
}

float NBGestorTeclas::tecladoEnPantallaAlto(){
	NBASSERT(_gestorInicializado)
	return _tecladoEnPantallaAlto;
}

void NBGestorTeclas::establecerTecladoEnPantallaAlto(const float alto, const bool superponeContenido){
	NBASSERT(_gestorInicializado)
	_tecladoEnPantallaAlto = alto;
	_tecladoSuperponeContenido = superponeContenido;
}


//Editors stack

STNBKeybEditor* NBGestorTeclas::privEditorGetCurrentRetainedLocked_(){
	STNBKeybEditor* r = NULL;
	NBGESTORTECLAS_MUTEX_ASSERT_LOCKED
	if(_editrBashStack.use > 0){
		r = NBArray_itmValueAtIndex(&_editrBashStack, STNBKeybEditor*, _editrBashStack.use - 1);
		//Retain
		if(r != NULL){
			NBKeybEditor_retain(r);
		}
	}
	return r;
}

void NBGestorTeclas::privEditorReleaseAndFree(STNBKeybEditor* editor){
	if(editor != NULL){
		NBKeybEditor_release(editor);
		//The editor was only retained by the "privEditorStackGet*" method
		if(editor->retainCount == 0){
			//Free memory
			NBMemory_free(editor);
			editor = NULL;
		}
	}
}




