//
//  AUEscenaTextoEditableOffscreen.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaTextoEditableOffscreen_H_INCLUIDO
#define AUEscenaTextoEditableOffscreen_H_INCLUIDO

#include "AUEscenaTextoEditable.h"
#include "nb/fonts/NBTextCursor.h"
#include "NBAnimador.h"
#include "NBGestorTeclasDefs.h" //ITextInputListener

class AUEscenaTextoEditableOffscreen : public AUEscenaTextoEditable, public ITextInputListener {
	public:
		AUEscenaTextoEditableOffscreen();
		AUEscenaTextoEditableOffscreen(AUFuenteRender* fuenteRender, const ENTextEditorType editorType, const BOOL allowMultiline);
		AUEscenaTextoEditableOffscreen(AUEscenaTextoEditable* other);
		AUEscenaTextoEditableOffscreen(AUEscenaTextoEditableOffscreen* other);
		virtual ~AUEscenaTextoEditableOffscreen();
		//
		ENTextEditorType	editorType() const;
		bool				isMultiline() const;
		void				setEditorType(const ENTextEditorType editorType);
		void				setMultiline(const bool isMultiline);
		void				setBehavior(const ENTextEditorType editorType, const bool isMultiline);
		//
		NBMatriz			sceneMatrix() const;
		NBRectangulo		scenePort() const;
		NBCajaAABB			sceneBox() const;
		void				setSceneProps(const NBMatriz matrix, const NBRectangulo port, const NBCajaAABB box);
		//TECLADO
		ENTextEditorType	entradaEditorType(BOOL* dstIsMultiline);
		//
		bool				entradaTieneTexto();
		bool				entradaPermiteMultilinea();
		void				entradaBackspace(const bool autoSelWord);
		void				entradaIntroducirTexto(const char* textoIntroducir, const bool autoSelWord);
		//TECLADO - ENTTRADA
		NBRangoI			entradaRangoSeleccion();
		NBRangoI			entradaRangoSeleccionAbs();
		void				entradaRangoSeleccionEstablecer(const SI32 primerCharDef, const SI32 conteoCharDefs);
		NBRangoI			entradaRangoMarcado();
		NBRangoI			entradaRangoMarcadoAbs();
		void				entradaRangoMarcadoEstablecer(const SI32 primerCharDef, const SI32 conteoCharDefs);
		void				entradaRangoDesmarcar();
		NBRangoI			entradaRangoLineaDeCharDef(const SI32 charDefPos);
		//Reemplazo y retorno de texto
		void				entradaTexto(AUCadenaMutable8* dst);
		void				entradaTextoMarcado(AUCadenaMutable8* dst);
		void				entradaTextoMarcadoReemplazar(const char* nuevoTexto, const ENTextRangeSet setNewComposingAs);
		void				entradaTextoSeleccion(AUCadenaMutable8* dst);
		void				entradaTextoAntesDeSeleccion(const UI32 conteoCharDefs, AUCadenaMutable8* dst);
		void				entradaTextoDespuesDeSeleccion(const UI32 conteoCharDefs, AUCadenaMutable8* dst);
		void				entradaTextoSeleccionEliminarAlrededor(const SI32 conteoCharDefsAntes, const SI32 conteoCharDefsDespues, const bool autoSelWord);
		void				entradaTextoEnCharDefsContenido(const UI32 primerCharDef, const UI32 conteoCharDefs, AUCadenaMutable8* dst);
		void				entradaTextoEnCharDefsReemplazar(const UI32 primerCharDef, const UI32 conteoCharDefs, const char* nuevoTexto, const ENTextRangeSet setNewComposingAs);
		bool				entradaTextoEnCharDefsPuedeReemplazarse(const UI32 primerCharDef, const UI32 conteoCharDefs, const char* nuevoTexto);
		//Geometria de texto
		NBRectangulo		entradaRectanguloParaCursor(const UI32 charDefPos);
		NBRectangulo		entradaPrimerRectanguloParaCharDefs(const UI32 primerCharDef, const UI32 conteoCharDefs);
		void				entradaRectangulosParaCharDefs(const UI32 primerCharDef, const UI32 conteoCharDefs, AUArregloNativoMutableP<NBRectangulo>* dst);
		UI32				entradaIndiceCharDefMasCercano(const float xViewPort, const float yViewPort);
		UI32				entradaIndiceCharDefMasCercanoEnCharDefs(const float xLocalTexto, const float yLocalTexto, const UI32 primerCharDef, const UI32 conteoCharDefs);
		UI32				entradaIndiceCharDefPrimero();
		UI32				entradaIndiceCharDefUltimo();
		SI32				entradaIndiceCharDefDesdePosicion(const UI32 charDefPos, const SI32 movimiento, const ENTextoDireccion direccion);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		//Behavior
		ENTextEditorType		_editorType;
		bool					_isMultiline;			//Allow multiple lines? Finish writing after "new-line" is entered?
		//Visual
		NBMatriz				_visualSceneMatrix;
		NBRectangulo			_visualScenePort;
		NBCajaAABB				_visualSceneBox;
		//
		NBPunto					privCoordSceneToPort(const float x, const float y);
        NBPunto                 privCoordPortToScene(const float x, const float y);
};

#endif
