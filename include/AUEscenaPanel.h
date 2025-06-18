//
//  AUEscenaPanel.h
//  AUFramework
//
//  Created by Nicaragua Binary on 10/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaPanel_h_INCLUIDO
#define AUEscenaPanel_h_INCLUIDO

#include "NBGestorTeclas.h"
#include "AUEscenaContenedor.h"
#include "AUEscenaSprite.h"
#include "AUEscenaSpriteElastico.h"
#include "NBAnimador.h"
#include "NBMargenes.h"

class AUEscenaPanel;

class IEscuchadorEscenaPanel {
	public:
		virtual ~IEscuchadorEscenaPanel(){
			//
		}
};

class AUEscenaPanel : public AUEscenaContenedor, public NBAnimador, public IEscuchadorTouchEscenaObjeto {
	public:
		AUEscenaPanel();
		AUEscenaPanel(const float ancho, const float alto, const NBMargenes margenes);
		AUEscenaPanel(const float ancho, const float alto, const float margenLeft, const float margenRight, const float margenTop, const float margenBottom);
		virtual ~AUEscenaPanel();
		//
		IEscuchadorEscenaPanel* escuchadorPanel();
		void				establecerEscuchadorPanel(IEscuchadorEscenaPanel* escuchador);
		//
		NBColor8			colorFondo() const;
		void				establecerColorFondo(const NBColor8 color);
		void				establecerColorFondo(const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		//
		NBMargenes			margenes() const;
		void				establecerMargenes(const NBMargenes margenes);
		void				establecerMargenes(const float margenLeft, const float margenRight, const float margenTop, const float margenBottom);
		//
		NBTamano			tamanoPanel() const;
		void				establecerTamanoPanel(const NBTamano tam);
		void				establecerTamanoPanel(const float anchoCaja, const float altoCaja);
		//
		const AUEscenaObjeto* objetoContenido() const;
		void				establecerObjetoContenido(AUEscenaObjeto* obj);
		void				scrollToLeftTop();
		//
		void				tickAnimacion(float segsTranscurridos);
		//TOUCHES
		void				touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto);
		void				touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
		void				touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		NBMargenes					_margenes;
		NBTamano					_panelTamano;
		NBColor8					_colorFondo;
		//
		IEscuchadorEscenaPanel*		_escuchadorPanel;
		AUEscenaObjeto*				_subescenaObj;
		AUEscenaObjeto*				_subescenaObjUlt;
		NBCajaAABB					_subescenaObjCajaUlt;
		SI32						_subescenaScrollAutoTouches;
		NBTamano					_subescenaScrollAuto;
		//Subescena
		SI32						_subescenaId;
		NBTamanoI					_subescenaTamano;
		NBCajaAABB					_subescenaCajaBase;	//Caja de vision base (sin desplazar)
		NBTamano					_subescenaScroll;	//Desplazamiento dentro de la subescena
		AUEscenaSprite*				_subescenaSprite;
		AUEscenaContenedor*			_subescenaContenedor;
		//
		virtual void				agregadoEnEscena();
		virtual void				quitandoDeEscena();
		//
		void						privAsegurarAreaEstaVisible(const NBRectangulo area);
		void						privScrollSubEscena(const float deltaX, const float deltaY, const bool forceLeftTop);
};

#endif
