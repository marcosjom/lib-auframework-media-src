//
//  AUEscenaTextoSprites.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaTextoSprites_H_INCLUIDO
#define AUEscenaTextoSprites_H_INCLUIDO

//ToDo: implement and enable

/*#include "AUEscenaContenedor.h"
#include "AUArregloMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUCadenaMutable.h"
#include "AUFuenteTextura.h"
#include "AUEscenaSprite.h"
#include "NBCajaAABB.h"
#include "NBAnimador.h"
#include "NBGestorAnimadores.h"
#include "NBText.h"

enum ENEscenaTextoAnimacion {
	ENEscenaTextoAnimacion_Ninguna = 0,
	ENEscenaTextoAnimacion_Alpha
};

enum ENEscenaCharEstado {
	ENEscenaCharEstado_Libre = 0,		//El caracter no esta en escena y puede ser reutilizado.
	ENEscenaCharEstado_Entering,		//Se anima la entrada del caracter
	ENEscenaCharEstado_Presentando,		//Se presenta el caracter
	ENEscenaCharEstado_Quitando			//Se anima la salida del caracter
};

struct STTextoSpriteChar {
	ENEscenaCharEstado		estado;
	ENEscenaTextoAnimacion	animacion;
	AUEscenaSprite*			sprite;
	//
	bool operator==(const STTextoSpriteChar &otro) const {
		return (sprite==otro.sprite);
	}
	bool operator!=(const STTextoSpriteChar &otro) const {
		return !(sprite==otro.sprite);
	}
};

class AUEscenaTextoSprites : public AUEscenaContenedor, public NBAnimador {
	public:
		AUEscenaTextoSprites();
		AUEscenaTextoSprites(AUFuenteTextura* fuenteTextura);
		AUEscenaTextoSprites(AUEscenaTextoSprites* otraInstancia);
		virtual ~AUEscenaTextoSprites();
		//
		AUFuenteTextura*			font() const;
		ENNBTextLineAlignH			alineacionH() const;
		ENNBTextAlignV				alineacionV() const;
		float						factorSeparacionCaracteres() const;
		float						factorSeparacionLineas() const;
		const char*					texto() const;
		void						establecerAlineacionH(ENNBTextLineAlignH alineacionH);
		void						establecerAlineacionV(ENNBTextAlignV alineacionV);
		void						establecerAlineaciones(ENNBTextLineAlignH alineacionH, ENNBTextAlignV alineacionV);
		void						establecerFactorSeparacionCaracteres(float factorSeparacionCaracteres);
		void						establecerFactorSeparacionLineas(float factorSeparacionLineas);
		void						establecerTexto(const char* texto, const ENEscenaTextoAnimacion animacion=ENEscenaTextoAnimacion_Ninguna, const float anchoLimite=99999999.0f, const float altoLimite=99999999.0f);
		NBCajaAABB					cajaParaTexto(const char* texto, const float anchoLimite=99999999.0f, const float altoLimite=99999999.0f);
		static NBCajaAABB			cajaParaTexto(const char* texto, AUFuenteTextura* objFuente, const ENNBTextLineAlignH alineacionH, const ENNBTextAlignV alineacionV, const float anchoLimite, const float altoLimite);
		//
		virtual void				tickAnimacion(float segsTranscurridos);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUFuenteRender*		_defFont;
		float				_defWidth;
		float				_defHeight;
		STNBText			_textData;
		UI32				_textDataSeqId;		//last text edited
		AUArregloMutable	_textVisuals;		//AUFuenteVisualModel (only populated/updated at rendering)
		UI32				_textVisualsSeqId;	//last visual synced (only populated/updated at rendering)
		AUArregloNativoMutableP<STTextoSpriteChar>	_spritesRecicables;		//Optimizacion: para reducir la repetitiva creacion/eliminacion de sprites.
		//
		virtual void				agregadoEnEscena();
		virtual void				quitandoDeEscena();
		STTextoSpriteChar*			privRegistroSpriteDisponible();
};*/

#endif
