//
//  AUGpMenuFinPartida.h
//  game-Refranero
//
//  Created by Marcos Ortega on 14/03/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#ifndef App_AUIButton_h
#define App_AUIButton_h

#include "AUEscenaContenedor.h"
#include "AUEscenaContenedorLimitado.h"
#include "AUEscenaSprite.h"
#include "AUEscenaSpriteElastico.h"
#include "AUArregloNativoMutableP.h"
#include "AUEscenaListaItemI.h"
#include "NBAnimador.h"
#include "NBMargenes.h"

#define AUIButton_MASK_AGREGADO_ANIMADOR	1		//anim added to manager
#define AUIButton_MASK_BOTON_ACCIONADO		2		//

typedef enum ENIBotonItemType_ {
	ENIBotonItemType_Object = 0,
	ENIBotonItemType_TextMonoline,
	ENIBotonItemType_TextMultiline,
	ENIBotonItemType_Icon
} ENIBotonItemType;

typedef enum ENIBotonItemState_ {
	ENIBotonItemState_Normal = 0,
	ENIBotonItemState_Hover,
	ENIBotonItemState_Touched,
	ENIBotonItemState_Count
} ENIBotonItemState;

typedef enum ENIBotonItemAlign_ {
	ENIBotonItemAlign_Left = 0,
	ENIBotonItemAlign_Center,
	ENIBotonItemAlign_Right,
	ENIBotonItemAlign_Count
} ENIBotonItemAlign;

typedef enum ENIBotonMargin_ {
	ENIBotonMargin_Top = 0,
	ENIBotonMargin_Bottom,
	ENIBotonMargin_Left,
	ENIBotonMargin_Right,
	ENIBotonMargin_Internal,
	ENIBotonMargin_Count
} ENIBotonMargin;

typedef struct STIBotonItem_ {
	AUEscenaObjeto*		obj;
	ENIBotonItemType	type;
	ENIBotonItemAlign	align;
	float				valign;
	//Autoresize
	AUEscenaListaItemI*	itf;		//itf to automatically resize object
	STNBSize			itfRelSz;	//rel size when resizing
	//ENIBotonItemState	curState;
	void*				extraData;
	//
	STListaItemDatos	refUserData;
	NBColor8			colors[ENIBotonItemState_Count];
} STIBotonItem;

class AUIButton;

class IEscuchadorIBoton {
	public:
		virtual ~IEscuchadorIBoton(){
			//
		}
		//
		virtual void botonPresionado(AUIButton* obj) = 0;
		virtual void botonAccionado(AUIButton* obj) = 0;
		virtual void botonLongTouch(AUIButton* obj, BOOL* dstCancelTouchAction) { }
		virtual AUEscenaObjeto* botonHerederoParaTouch(AUIButton* obj, const NBPunto &posInicialEscena, const NBPunto &posActualEscena) = 0;
};

class AUIButton: public AUEscenaContenedor, public NBAnimador, public AUEscenaListaItemI, public IEscuchadorTouchEscenaObjeto {
	public:
		AUIButton();
		virtual				~AUIButton();
		//
		IEscuchadorIBoton*	escuchadorBoton() const;
		void				establecerEscuchadorBoton(IEscuchadorIBoton* escuchador);
		//
		NBCajaAABB			limitesParaContenido() const;
		bool				isInternalVisualObject(AUEscenaObjeto* obj) const;
		AUEscenaContenedor*	getTouchLayer();
		//
		AUEscenaObjeto*		itemObjetoEscena();
		const char*			refOptionId() const;
		STListaItemDatos	refDatos();
		void				establecerOptionId(const char* optionId);
		void				establecerRefDatos(const SI32 tipo, const SI32 valor);
		void				organizarContenido(const bool notificarCambioAltura);
		void				organizarContenido(const float anchoParaContenido, const float altoVisible, const bool notificarCambioAltura, AUAnimadorObjetoEscena* animator, const float secsAnim);
		//
		NBMargenes			margenes() const;
		float				margenI() const;
		void				establecerMargenes(const float mIzq, const float mDer, const float mSup, const float mInf, const float mInternal);
		void				establecerMargenes(const NBMargenes margins, const float mInternal);
		void				establecerFondo(const bool mostrarFondo, AUTextura* texFondo);
		void				establecerFondoColores(const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		void				establecerFondoColores(const NBColor8 normal, const NBColor8 hover, const NBColor8 touched);
		void				establecerFondoInvertir(const bool invertirHorizontal, const bool invertirVertical);
		//
		float				segsRetrasarOnTouch() const;
		float				segsParaLongTouch() const;
		void				establecerSegsRetrasarOnTouch(const float segsRetrasarOnTouch);
		void				establecerSegsParaLongTouch(const float segsParaLongTouch);
		//
		AUEscenaObjeto*		subElementoPorTipo(const SI32 refTipo) const;
		//
		void				agregarObjeto(AUEscenaObjeto* obj, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor);
		void				agregarObjeto(AUEscenaObjeto* obj, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 color);
		void				agregarObjeto(AUEscenaObjeto* obj, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 colorNormal, const NBColor8 colorHover, const NBColor8 colorTouched);
		void				agregarObjeto(AUEscenaObjeto* obj, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		void				agregarObjeto(AUEscenaObjeto* obj, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 rN, const UI8 gN, const UI8 bN, const UI8 aN, const UI8 rH, const UI8 gH, const UI8 bH, const UI8 aH, const UI8 rT, const UI8 gT, const UI8 bT, const UI8 aT);
		//
		void				agregarObjetoResizable(AUEscenaObjeto* obj, AUEscenaListaItemI* itf, const STNBSize itfRelSz, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor);
		void				agregarObjetoResizable(AUEscenaObjeto* obj, AUEscenaListaItemI* itf, const STNBSize itfRelSz, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 color);
		void				agregarObjetoResizable(AUEscenaObjeto* obj, AUEscenaListaItemI* itf, const STNBSize itfRelSz, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 colorNormal, const NBColor8 colorHover, const NBColor8 colorTouched);
		void				agregarObjetoResizable(AUEscenaObjeto* obj, AUEscenaListaItemI* itf, const STNBSize itfRelSz, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		void				agregarObjetoResizable(AUEscenaObjeto* obj, AUEscenaListaItemI* itf, const STNBSize itfRelSz, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 rN, const UI8 gN, const UI8 bN, const UI8 aN, const UI8 rH, const UI8 gH, const UI8 bH, const UI8 aH, const UI8 rT, const UI8 gT, const UI8 bT, const UI8 aT);
		//
		void				agregarIcono(AUTextura* tex, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor);
		void				agregarIcono(AUTextura* tex, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 color);
		void				agregarIcono(AUTextura* tex, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 colorNormal, const NBColor8 colorHover, const NBColor8 colorTouched);
		void				agregarIcono(AUTextura* tex, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		void				agregarIcono(AUTextura* tex, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 rN, const UI8 gN, const UI8 bN, const UI8 aN, const UI8 rH, const UI8 gH, const UI8 bH, const UI8 aH, const UI8 rT, const UI8 gT, const UI8 bT, const UI8 aT);
		//
		void				agregarTextoMonolinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor);
		void				agregarTextoMonolinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 color);
		void				agregarTextoMonolinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 colorNormal, const NBColor8 colorHover, const NBColor8 colorTouched);
		void				agregarTextoMonolinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		void				agregarTextoMonolinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 rN, const UI8 gN, const UI8 bN, const UI8 aN, const UI8 rH, const UI8 gH, const UI8 bH, const UI8 aH, const UI8 rT, const UI8 gT, const UI8 bT, const UI8 aT);
		//
		void				agregarTextoMultilinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor);
		void				agregarTextoMultilinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 color);
		void				agregarTextoMultilinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const NBColor8 colorNormal, const NBColor8 colorHover, const NBColor8 colorTouched);
		void				agregarTextoMultilinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		void				agregarTextoMultilinea(AUFuenteRender* fuente, const char* str, const ENIBotonItemAlign align, const float vAlignRel, const SI32 refTipo, const SI32 refValor, const UI8 rN, const UI8 gN, const UI8 bN, const UI8 aN, const UI8 rH, const UI8 gH, const UI8 bH, const UI8 aH, const UI8 rT, const UI8 gT, const UI8 bT, const UI8 aT);
		//
		void				actualizarObjetoPorTipo(const SI32 refTipo, AUEscenaObjeto* obj);
		void				actualizarObjetoResizablePorTipo(const SI32 refTipo, AUEscenaObjeto* obj, AUEscenaListaItemI* itf);
		void				actualizarIconosPorTipo(const SI32 refTipo, AUTextura* tex);
		void				actualizarTextosMonolineaPorTipo(const SI32 refTipo, const char* strTexto);
		void				actualizarTextosMultilineaPorTipo(const SI32 refTipo, const char* strTexto);
		//
		void				establecerColoresParaItemsTipo(const SI32 refTipo, const NBColor8 &normal, const NBColor8 &hover, const NBColor8 &touched);
		//
		void				tickAnimacion(float segsTranscurridos);
		//TOUCHES
		void				touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto);
		void				touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
		void				touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
		void				hoverIniciado(AUEscenaObjeto* objeto);
		void				hoverMovido(const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
		void				hoverFinalizado(AUEscenaObjeto* objeto);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		IEscuchadorIBoton*	_escuchadorBoton;
		BOOL				_touchesAsLongTouch;	//current sequence already notified as long-touch				
		SI16				_touchesCount;
		SI16				_hoversCount;
		STNBString			_refOptionId;
		STListaItemDatos	_refDatos;
		NBMargenes			_margins;
		float				_marginI;
		UI8					_fondoOpciones;		//mostrar, voltearV, voltearH
		UI8					_estadoActual;		//ENIBotonItemState
		UI8					_estadoPresentado;	//ENIBotonItemState
		//Animacion de estados
		UI8					_maskAnimacionEstado;
		float				_segsRetrasarOnTouch;
		float				_segsParaLongTouch;
		float				_segsAcumColorOnTouch;
		//
		NBColor8			_fondoColors[ENIBotonItemState_Count];
		AUEscenaContenedorLimitado* _contenedor;
		AUEscenaSpriteElastico* _fondo;
		AUArregloNativoMutableP<STIBotonItem>	_items;
		//
		void				privAnalizarEstado();
		void				privActualizarObjetos(const ENIBotonItemState estado);
};

#endif
