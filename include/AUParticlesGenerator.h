//
//  AUParticlesGenerator.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 06/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUParticlesGenerator_H_INCLUIDO
#define AUParticlesGenerator_H_INCLUIDO

#include "AUObjeto.h"
#include "NBPunto.h"
#include "NBRango.h"
#include "NBRectangulo.h"
#include "NBAnimador.h"
#include "AUEscenaContenedor.h"
#include "AUArregloNativoMutable.h"
#include "AUEscenaSprite.h"

typedef enum ENPartsGenProbability_ {
	ENPartsGenProbability_Low = 0,
	ENPartsGenProbability_Medium,
	ENPartsGenProbability_High,
	ENPartsGenProbability_Count
} ENPartsGenProbability;

typedef struct STPartsGenPartDef_ {
	AUTextura* texture;
	ENPartsGenProbability probability;
	//
	float	secsToLive;
	NBColor8 colorBase;
	//float	scaleMin;
	//float	scaleMax;
	//float	velRotMin;
	//float	velRotMax;
} STPartsGenPartDef;

typedef struct STPartsGenPart_ {
	AUEscenaSprite* sprite;
	UI8				alphaMax;
	float			secsAlive;
	float			secsToLive;
	//
	float			velRot;
	NBPunto			vel;
} STPartsGenPart;

class AUParticlesGenerator: public AUObjeto, public NBAnimador {
	public:
		AUParticlesGenerator();
		virtual ~AUParticlesGenerator();
		//
		AUEscenaContenedor*	layer() const;
		void	setLayer(AUEscenaContenedor* layer);
		//
		bool	isEnabled() const;
		void	setIsEnabled(const bool isEnabled);
		//
		NBRectangulo dstArea() const;
		void	setDstArea(const NBRectangulo area);
		void	setDstArea(const float x, const float y, const float width, const float height);
		void	setDstPosition(const NBPunto dstPosition);
		void	setDstPosition(const float dstX, const float dstY);
		//
		NBPunto	defGravity() const;
		void	setDefGravity(const NBPunto vector);
		void	setDefGravity(const float x, const float y);
		//
		NBRango	defRotSpeedRange() const;
		void	setDefRotSpeedRange(const NBRango range);
		void	setDefRotSpeedRange(const float start, const float size);
		//
		NBRango	defScaleRange() const;
		void	setDefScaleRange(const NBRango range);
		void	setDefScaleRange(const float start, const float size);
		//
		NBRectangulo defSpeedRange() const;
		void	setDefSpeedRange(const NBRectangulo range);
		void	setDefSpeedRange(const float x, const float y, const float width, const float height);
		//
		float	secsBetweenParticle() const;
		void	setSecsBetweenParticle(const float secsBetweenParticle);
		//
		void	addTexture(AUTextura* texture, const ENPartsGenProbability probability, const float secsToLive, const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		void	removeTexture(AUTextura* texture);
		void	removeTextureAtIndex(const SI32 index);
		//
		void	generateRandomParticle();
		void	generateRandomParticle(const float minInitVelX, const float maxInitVelX, const float minInitVelY, const float maxInitVelY);
		void	generateRandomParticle(const float minInitVelX, const float maxInitVelX, const float minInitVelY, const float maxInitVelY, const NBColor8 color);
		void	generateRandomParticle(const float minInitVelX, const float maxInitVelX, const float minInitVelY, const float maxInitVelY, const UI8 r, const UI8 g, const UI8 b);
		void	generateRandomParticle(const float minInitVelX, const float maxInitVelX, const float minInitVelY, const float maxInitVelY, const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		//
		void	tickAnimacion(float segsTranscurridos);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUEscenaContenedor* _layer;
		AUArregloNativoMutableP<STPartsGenPartDef>	_partsDef;
		AUArregloNativoMutableP<STPartsGenPart>	_parts;
		SI32				_partsDefsByProb[ENPartsGenProbability_Count];
		//
		bool				_isEnabled;
		float				_secsBetweenParticle;
		float				_secsForNextParticle;
		NBRectangulo		_dstArea;
		//
		NBPunto				_defGravity;
		NBRango				_defRotSpeedRange;
		NBRango				_defScaleRange;
		NBRectangulo		_defSpeedRange;
};

#endif
