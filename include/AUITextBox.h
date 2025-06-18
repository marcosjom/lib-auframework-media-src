//
//  AUITextBox.h
//  AUFramework
//
//  Created by Nicaragua Binary on 10/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUITextBox_h_INCLUIDO
#define AUITextBox_h_INCLUIDO

#include "NBGestorTeclas.h"
#include "AUEscenaContenedorLimitado.h"
#include "AUEscenaSprite.h"
#include "AUEscenaSpriteElastico.h"
#include "AUEscenaTexto.h"
#include "AUEscenaTextoEditable.h"
#include "NBMargenes.h"

typedef enum ENITextboxBtnAction_ {
	ENITextboxBtnAction_None = 0,	//Do nothing
	ENITextboxBtnAction_Clear,			//Clear content and resign to focus if empty
	ENITextboxBtnAction_ResignFocus,	//Loose focus
	ENITextboxBtnAction_Count
} ENITextboxBtnAction;

class AUITextBox;

class IEscuchadorITextBox {
	public:
		virtual ~IEscuchadorITextBox(){
			//
		}
		//
		virtual void textboxFocoObtenido(AUITextBox* obj) = 0;
		virtual void textboxFocoPerdido(AUITextBox* obj, const ENNBKeybFocusLostType actionType) = 0;
		virtual bool textboxMantenerFocoAnteConsumidorDeTouch(AUITextBox* obj, AUEscenaObjeto* consumidorTouch) = 0;
		virtual void textboxContenidoModificado(AUITextBox* obj, const char* strContenido) = 0;
};

class AUITextBox : public AUEscenaContenedorLimitado, public ITextEditorListener, public IEscuchadorTouchEscenaObjeto {
	public:
		AUITextBox(const ENTextEditorType editorType, AUFuenteRender* fuenteTextura, const float anchoCaja, const float lineasCaja);
		AUITextBox(const ENTextEditorType editorType, AUFuenteRender* fuenteTextura, const float anchoCaja, const float lineasCaja, const NBMargenes margenes);
		AUITextBox(const ENTextEditorType editorType, AUFuenteRender* fuenteTextura, const float anchoCaja, const float lineasCaja, const float margenLeft, const float margenRight, const float margenTop, const float margenBottom);
		virtual ~AUITextBox();
		//
		IEscuchadorITextBox* escuchadorTextBox();
		void				establecerEscuchadorTextBox(IEscuchadorITextBox* escuchador);
		//
		NBMargenes			margenes() const;
		void				establecerMargenes(const NBMargenes margenes);
		void				establecerMargenes(const float margenLeft, const float margenRight, const float margenTop, const float margenBottom);
		//
		float				boxWidth() const;
		float				boxHeightInLines() const;
		//
		AUFuenteRender*		getFont() const;
		//
		AUTextura*			fondo() const;
		void				establecerFondo(AUTextura* textura);
		void				establecerFondo(AUTextura* textura, const STNBColor8 color);
		void				establecerFondoColor(const STNBColor8 color);
		void				establecerFondoColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		void				establecerFondoBoxColor(const STNBColor8 color);
		void				establecerFondoBoxColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		//
		bool				atFocus() const;
		void				focusObtain();
		void				focusRelease();
		//
		bool				textoPermitirMultilinea();
		void				establecerTextoPermitirMultilinea(const bool permitirMultilinea);
		//
		const char*			passwordChar() const;
		void				setPasswordChar(const char* passChar);
		void				setButton(AUTextura* tex, const ENITextboxBtnAction action, const STNBColor8 color);
		void				setButton(AUTextura* tex, const ENITextboxBtnAction action, const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		void				setButtonColor(const STNBColor8 color);
		void				setButtonColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		//
		ENNBTextLineAlignH	alineacionH() const;
		ENNBTextAlignV		alineacionV() const;
		void				establecerAlineacionH(ENNBTextLineAlignH alineacionH);
		void				establecerAlineacionV(ENNBTextAlignV alineacionV);
		void				establecerAlineaciones(ENNBTextLineAlignH alineacionH, ENNBTextAlignV alineacionV);
		//
		const char*			texto() const;
		UI32				textoLen() const;
		const STNBText*		getVisualTextDef() const;
		UI32				getVisualCharDefsCount() const;
		UI32				getVisualLinesCount() const;
		void				establecerTexto(const char* valor);
		void				establecerTextoColor(const NBColor8 color);
		void				establecerTextoColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		void				establecerTextoAyuda(const char* valor);
		void				establecerTextoAyudaColor(const NBColor8 color);
		void				establecerTextoAyudaColor(const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		void				establecerTamanoCaja(const float anchoCaja, const float lineasVisibles);
		float				altoParaLineasVisibles(const float lineasVisibles) const;
		static float		altoParaLineasVisibles(const float lineasVisibles, AUFuenteRender* fuenteRender);
		//TEXT EDITOR
		void				textEditorCloneInterfaceObjectsRetained(AUObjeto** dstVisualObj, ITextInputListener** dstVisualItf, AUObjeto** dstValueObj, ITextInputListener** dstValueItf);
		STTextEditorSyncResult textEditorCompareClones(AUObjeto* visualClone, AUObjeto* valueClone);
		STTextEditorSyncResult textEditorSyncClones(AUObjeto* visualClone, AUObjeto* valueClone);
		void				textEditorFocusGained();
		void				textEditorFocusLost(const ENNBKeybFocusLostType actionType);
		bool				textEditorMantainBeforeTouchConsumer(AUEscenaObjeto* consumidorTouch);
		//ToDo: remove, never used
		/*void				textoSeleccionEliminarAlrededor(const UI32 conteoCharDefsAntes, const UI32 conteoCharDefsDespues);
		void				reemplazarTextoMarcado(const char* nuevoTexto);
		void				reemplazarTextoEnCharDefs(const UI32 primerCharDef, const UI32 conteoCharDefs, const char* nuevoTexto);*/
		//TECLADO
		/*
		//Geometria de texto
		NBRectangulo		entradaRectanguloParaCursor(const UI32 charDefPos);
		NBRectangulo		entradaPrimerRectanguloParaCharDefs(const UI32 primerCharDef, const UI32 conteoCharDefs);
		void				entradaRectangulosParaCharDefs(const UI32 primerCharDef, const UI32 conteoCharDefs, AUArregloNativoMutableP<NBRectangulo>* dst);
		UI32				entradaIndiceCharDefMasCercano(const float xViewPort, const float yViewPort);
		UI32				entradaIndiceCharDefMasCercanoEnCharDefs(const float xLocalTexto, const float yLocalTexto, const UI32 primerCharDef, const UI32 conteoCharDefs);
		UI32				entradaIndiceCharDefPrimero();
		UI32				entradaIndiceCharDefUltimo();
		SI32				entradaIndiceCharDefDesdePosicion(const UI32 charDefPos, const SI32 movimiento, const ENTextoDireccion direccion);*/
		//TOUCHES
		void				touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto);
		void				touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
		void				touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		ENTextEditorType			_editorType;
		BOOL						_allowMultiline;
		NBMargenes					_margenes;
		float						_boxWidth;
		float						_boxHeightInLines;
		char*						_passChar;
		SI32						_passCharSz;			//Bytes for UTF8 char (does not include the '\0').
		IEscuchadorITextBox*		_escuchadorCajaTxt;
		//
		bool						_atFocus;
		AUFuenteRender*				_font;
		AUEscenaSpriteElastico*		_bg;
		AUEscenaTextoEditable*		_value;					//The real value
		UI32						_valueTextSeqIdCloned;	//SeqId at sycned fromValueToClone.
		UI32						_valueCursorSeqIdCloned; //SeqId at sycned fromValueToClone.
		AUEscenaTextoEditable*		_visual;				//The visual value (if not pass-char is defined, it points to real-value)
		AUEscenaTexto*				_visualHelp;
		//Button
		struct {
			AUEscenaSprite*			icon;
			STNBColor8				color;
			ENITextboxBtnAction		action;
			SI32					touchDepth;	//amount of touch events on it
		} _btn;
		//Subscene
		struct {
			SI32					Id;
			NBTamanoI				Tamano;
			NBCajaAABB				CajaBase;	//Caja de vision base (sin desplazar)
			STNBColor8				colorBg;
			NBTamano				Scroll;	//Desplazamiento dentro de la subescena
			AUEscenaSprite*			Sprite;
			AUEscenaContenedor*		Contenedor;
		} _subscn;
		//
		virtual void				agregadoEnEscena();
		virtual void				quitandoDeEscena();
		//
		void						privOrganizeBtn();
		void						privValueUpdated(const BOOL valueChanged, const BOOL cursrChanged);
		void						privEstablecerTamanoCaja(const float anchoCaja, const float lineasVisibles, const BOOL force);
		void						privAsegurarCursorEstaVisible();
		static STTextEditorSyncResult privTextEditorCompareClones_(const AUITextBox* obj, const AUObjeto* pVisualClone, const AUObjeto* pValueClone);
};

#endif
