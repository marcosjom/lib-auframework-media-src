//
//  AUParticlesGenerator.cpp
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 06/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUParticlesGenerator.h"
#include "NBGestorAnimadores.h"

AUParticlesGenerator::AUParticlesGenerator() : AUObjeto()
	, _layer(NULL)
	, _partsDef(this)
	, _parts(this)
	, _isEnabled(true)
	, _secsBetweenParticle(0.0f)
	, _secsForNextParticle(0.0f)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUParticlesGenerator::AUParticlesGenerator")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUParticlesGenerator")
	//
	{
		SI32 i;
		for(i = 0; i < ENPartsGenProbability_Count; i++){
			_partsDefsByProb[i] = 0;
		}
	}
	_dstArea.x		= 0.0f;
	_dstArea.y		= 0.0f;
	_dstArea.ancho	= 0.0f;
	_dstArea.alto	= 0.0f;
	//Defaults
	_defGravity.x	= 0.0f;
	_defGravity.y	= -9.8f;
	_defRotSpeedRange.inicio = 0.0f;
	_defRotSpeedRange.tamano = 0.0f;
	_defScaleRange.inicio	= 1.0f;
	_defScaleRange.tamano	= 0.0f;
	_defSpeedRange.x		= 0.0f;
	_defSpeedRange.y		= 0.0f;
	_defSpeedRange.ancho	= 0.0f;
	_defSpeedRange.alto		= 0.0f;
	//
	NBGestorAnimadores::agregarAnimador(this, this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUParticlesGenerator::~AUParticlesGenerator(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUParticlesGenerator::~AUParticlesGenerator")
	NBGestorAnimadores::quitarAnimador(this);
	//Release sprites
	{
		SI32 i; const SI32 count = _parts.conteo;
		for(i = 0; i < count; i++){
			STPartsGenPart* part = _parts.elemPtr(i);
			if(part->sprite != NULL){
				AUEscenaContenedor* parent = (AUEscenaContenedor*)part->sprite->contenedor();
				if(parent != NULL) parent->quitarObjetoEscena(part->sprite);
				//
				part->sprite->liberar(NB_RETENEDOR_THIS);
				part->sprite = NULL;
			}
		}
		_parts.vaciar();
	}
	//Release definitions
	{
		SI32 i; const SI32 count = _partsDef.conteo;
		for(i = 0; i < count; i++){
			STPartsGenPartDef* part = _partsDef.elemPtr(i);
			if(part->texture != NULL) part->texture->liberar(NB_RETENEDOR_THIS); part->texture = NULL;
		}
		_partsDef.vaciar();
	}
	if(_layer != NULL) _layer-> liberar(NB_RETENEDOR_THIS); _layer = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

AUEscenaContenedor* AUParticlesGenerator::layer() const {
	return _layer;
}

void AUParticlesGenerator::setLayer(AUEscenaContenedor* layer){
	if(layer != NULL) layer->retener(NB_RETENEDOR_THIS);
	if(_layer != NULL) _layer->liberar(NB_RETENEDOR_THIS);
	_layer = layer;
}

//

bool AUParticlesGenerator::isEnabled() const {
	return _isEnabled;
}

void AUParticlesGenerator::setIsEnabled(const bool isEnabled){
	_isEnabled = isEnabled;
}

//

NBRectangulo AUParticlesGenerator::dstArea() const {
	return _dstArea;
}

void AUParticlesGenerator::setDstArea(const NBRectangulo area){
	_dstArea = area;
}

void AUParticlesGenerator::setDstArea(const float x, const float y, const float width, const float height){
	_dstArea.x		= x;
	_dstArea.y		= y;
	_dstArea.ancho	= width;
	_dstArea.alto	= height;
}

void AUParticlesGenerator::setDstPosition(const NBPunto dstPosition){
	_dstArea.x		= dstPosition.x;
	_dstArea.y		= dstPosition.y;
	_dstArea.ancho	= 0.0f;
	_dstArea.alto	= 0.0f;
}

void AUParticlesGenerator::setDstPosition(const float dstX, const float dstY){
	_dstArea.x		= dstX;
	_dstArea.y		= dstY;
	_dstArea.ancho	= 0.0f;
	_dstArea.alto	= 0.0f;
}

//

NBPunto AUParticlesGenerator::defGravity() const {
	return _defGravity;
}

void AUParticlesGenerator::setDefGravity(const NBPunto vector){
	_defGravity = vector;
}

void AUParticlesGenerator::setDefGravity(const float x, const float y){
	_defGravity.x = x;
	_defGravity.y = y;
}

//

NBRango AUParticlesGenerator::defRotSpeedRange() const {
	return _defRotSpeedRange;
}

void AUParticlesGenerator::setDefRotSpeedRange(const NBRango range){
	_defRotSpeedRange = range;
}

void AUParticlesGenerator::setDefRotSpeedRange(const float start, const float size){
	_defRotSpeedRange.inicio = start;
	_defRotSpeedRange.tamano = size;
}

//

NBRango AUParticlesGenerator::defScaleRange() const {
	return _defScaleRange;
}

void AUParticlesGenerator::setDefScaleRange(const NBRango range){
	_defScaleRange = range;
}

void AUParticlesGenerator::setDefScaleRange(const float start, const float size){
	_defScaleRange.inicio = start;
	_defScaleRange.tamano = size;
}


//

NBRectangulo AUParticlesGenerator::defSpeedRange() const {
	return _defSpeedRange;
}

void AUParticlesGenerator::setDefSpeedRange(const NBRectangulo range){
	_defSpeedRange = range;
}

void AUParticlesGenerator::setDefSpeedRange(const float x, const float y, const float width, const float height){
	_defSpeedRange.x		= x;
	_defSpeedRange.y		= y;
	_defSpeedRange.ancho	= width;
	_defSpeedRange.alto		= height;
}

//

float AUParticlesGenerator::secsBetweenParticle() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUParticlesGenerator::secsBetweenParticle")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _secsBetweenParticle;
}

void AUParticlesGenerator::setSecsBetweenParticle(const float secsBetweenParticle){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUParticlesGenerator::setSecsBetweenParticle")
	_secsBetweenParticle = secsBetweenParticle;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUParticlesGenerator::addTexture(AUTextura* texture, const ENPartsGenProbability probability, const float secsToLive, const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	NBASSERT(probability >= 0 && probability < ENPartsGenProbability_Count)
	STPartsGenPartDef part;
	part.texture		= texture; if(texture != NULL) texture->retener(NB_RETENEDOR_THIS);
	part.probability	= probability;
	//
	part.secsToLive		= secsToLive;
	NBCOLOR_ESTABLECER(part.colorBase, r, g, b, a)
	//
	_partsDef.agregarElemento(part);
	//
	_partsDefsByProb[probability]++;
}

void AUParticlesGenerator::removeTexture(AUTextura* texture){
	SI32 i;
	for(i = (_partsDef.conteo - 1); i >= 0; i--){
		STPartsGenPartDef* part = _partsDef.elemPtr(i);
		if(part->texture == texture){
			_partsDefsByProb[part->probability]--;
			if(part->texture != NULL){ part->texture->liberar(NB_RETENEDOR_THIS); part->texture = NULL; }
			_partsDef.quitarElementoEnIndice(i);
		}
	}
}

void AUParticlesGenerator::removeTextureAtIndex(const SI32 index){
	NBASSERT(index >= 0 && index < _partsDef.conteo)
	if(index >= 0 && index < _partsDef.conteo){
		STPartsGenPartDef* part = _partsDef.elemPtr(index);
		_partsDefsByProb[part->probability]--;
		if(part->texture != NULL){ part->texture->liberar(NB_RETENEDOR_THIS); part->texture = NULL; }
		_partsDef.quitarElementoEnIndice(index);
	}
}

void AUParticlesGenerator::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUParticlesGenerator::tickAnimacion")
	//Animate existing particles
	{
		SI32 i;
		for(i = (_parts.conteo - 1); i >= 0; i--){
			STPartsGenPart* part = _parts.elemPtr(i);
			part->secsAlive += segsTranscurridos;
			if(part->secsAlive >= part->secsToLive){
				//Remove
				if(part->sprite != NULL){
					AUEscenaContenedor* parent = (AUEscenaContenedor*)part->sprite->contenedor();
					if(parent != NULL) parent->quitarObjetoEscena(part->sprite);
					part->sprite->liberar(NB_RETENEDOR_THIS);
					part->sprite = NULL;
				}
				_parts.quitarElementoEnIndice(i);
			} else {
				if(part->sprite != NULL){
					//Animate alpha
					if(part->secsAlive < (part->secsToLive * 0.10f)){
						part->sprite->establecerMultiplicadorAlpha8((float)part->alphaMax * (part->secsAlive / (part->secsToLive * 0.10f)));
					} else if(part->secsAlive < (part->secsToLive * 0.90f)){
						part->sprite->establecerMultiplicadorAlpha8(part->alphaMax);
					} else {
						part->sprite->establecerMultiplicadorAlpha8((float)part->alphaMax * (1.0f - ((part->secsAlive - (part->secsToLive * 0.90f)) / (part->secsToLive * 0.10f))));
					}
					//Animate rotation
					{
						float rot = part->sprite->rotacion();
						rot += part->velRot * segsTranscurridos; while(rot > 360.0f) rot -= 360.0f;
						part->sprite->establecerRotacion(rot);
					}
					//Animate scale
					{
						part->sprite->establecerEscalacion(_defScaleRange.inicio + (_defScaleRange.tamano * (part->secsAlive / part->secsToLive)));
					}
					//Animate position
					{
						part->vel.x += (_defGravity.x /* * segsTranscurridos*/);
						part->vel.y += (_defGravity.y /* * segsTranscurridos*/);
						NBPunto pos = part->sprite->traslacion();
						pos.x += part->vel.x * segsTranscurridos;
						pos.y += part->vel.y * segsTranscurridos;
						part->sprite->establecerTraslacion(pos);
					}
				}
			}
		}
	}
	//Generate new particles
	if(_isEnabled){
		_secsForNextParticle += segsTranscurridos;
		while(_secsForNextParticle >= _secsBetweenParticle){
			_secsForNextParticle -= _secsBetweenParticle;
			this->generateRandomParticle();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUParticlesGenerator::generateRandomParticle(){
	if(_layer != NULL && _partsDef.conteo != 0){
		NBASSERT(ENPartsGenProbability_Count == 3)
		const SI32 maxProb = _partsDefsByProb[ENPartsGenProbability_Low] + (_partsDefsByProb[ENPartsGenProbability_Medium] * 2) + (_partsDefsByProb[ENPartsGenProbability_High] * 3);
		const SI32 iProb = (rand() % maxProb);
		SI32 iPartDef;
		if(iProb < _partsDefsByProb[ENPartsGenProbability_Low]){
			iPartDef = iProb;
		} else if(iProb < (_partsDefsByProb[ENPartsGenProbability_Low] + (_partsDefsByProb[ENPartsGenProbability_Medium] * 2))){
			iPartDef = _partsDefsByProb[ENPartsGenProbability_Low] + ((iProb - _partsDefsByProb[ENPartsGenProbability_Low]) / 2);
		} else {
			iPartDef = _partsDefsByProb[ENPartsGenProbability_Low] + _partsDefsByProb[ENPartsGenProbability_Medium] + ((iProb - _partsDefsByProb[ENPartsGenProbability_Low] - (_partsDefsByProb[ENPartsGenProbability_Medium] * 2)) / 3);
		}
		NBASSERT(iPartDef >= 0  && iPartDef < _partsDef.conteo)
		STPartsGenPartDef* partDef = _partsDef.elemPtr(iPartDef);
		if(partDef->texture != NULL){
			STPartsGenPart newPart;
			newPart.sprite		= new(this) AUEscenaSprite(partDef->texture);
			newPart.sprite->establecerTraslacion(_dstArea.x + (_dstArea.ancho * ((float)(rand() % 100) / 100.0f)), _dstArea.y + (_dstArea.alto * ((float)(rand() % 100) / 100.0f)));
			newPart.sprite->establecerMultiplicadorColor8(partDef->colorBase.r, partDef->colorBase.g, partDef->colorBase.b, 0);
			newPart.alphaMax	= partDef->colorBase.a;
			newPart.secsAlive	= 0.0f;
			newPart.secsToLive	= partDef->secsToLive;
			newPart.velRot		= _defRotSpeedRange.inicio + (_defRotSpeedRange.tamano * ((float)(rand() % 100) / 100.0f));
			newPart.vel.x		= _defSpeedRange.x + (_defSpeedRange.ancho * ((float)(rand() % 100) / 100.0f));
			newPart.vel.y		= _defSpeedRange.y + (_defSpeedRange.alto * ((float)(rand() % 100) / 100.0f));
			_layer->agregarObjetoEscena(newPart.sprite);
			_parts.agregarElemento(newPart);
		}
	}
}

void AUParticlesGenerator::generateRandomParticle(const float minInitVelX, const float maxInitVelX, const float minInitVelY, const float maxInitVelY){
	if(_layer != NULL && _partsDef.conteo != 0){
		NBASSERT(ENPartsGenProbability_Count == 3)
		const SI32 maxProb = _partsDefsByProb[ENPartsGenProbability_Low] + (_partsDefsByProb[ENPartsGenProbability_Medium] * 2) + (_partsDefsByProb[ENPartsGenProbability_High] * 3);
		const SI32 iProb = (rand() % maxProb);
		SI32 iPartDef;
		if(iProb < _partsDefsByProb[ENPartsGenProbability_Low]){
			iPartDef = iProb;
		} else if(iProb < (_partsDefsByProb[ENPartsGenProbability_Low] + (_partsDefsByProb[ENPartsGenProbability_Medium] * 2))){
			iPartDef = _partsDefsByProb[ENPartsGenProbability_Low] + ((iProb - _partsDefsByProb[ENPartsGenProbability_Low]) / 2);
		} else {
			iPartDef = _partsDefsByProb[ENPartsGenProbability_Low] + _partsDefsByProb[ENPartsGenProbability_Medium] + ((iProb - _partsDefsByProb[ENPartsGenProbability_Low] - (_partsDefsByProb[ENPartsGenProbability_Medium] * 2)) / 3);
		}
		NBASSERT(iPartDef >= 0  && iPartDef < _partsDef.conteo)
		STPartsGenPartDef* partDef = _partsDef.elemPtr(iPartDef);
		if(partDef->texture != NULL){
			STPartsGenPart newPart;
			newPart.sprite		= new(this) AUEscenaSprite(partDef->texture);
			newPart.sprite->establecerTraslacion(_dstArea.x + (_dstArea.ancho * ((float)(rand() % 100) / 100.0f)), _dstArea.y + (_dstArea.alto * ((float)(rand() % 100) / 100.0f)));
			newPart.sprite->establecerMultiplicadorColor8(partDef->colorBase.r, partDef->colorBase.g, partDef->colorBase.b, 0);
			newPart.alphaMax	= partDef->colorBase.a;
			newPart.secsAlive	= 0.0f;
			newPart.secsToLive	= partDef->secsToLive;
			newPart.velRot		= _defRotSpeedRange.inicio + (_defRotSpeedRange.tamano * ((float)(rand() % 100) / 100.0f));
			newPart.vel.x		= minInitVelX + ((maxInitVelX - minInitVelX) * ((float)(rand() % 100) / 100.0f));
			newPart.vel.y		= minInitVelY + ((maxInitVelY - minInitVelY) * ((float)(rand() % 100) / 100.0f));
			_layer->agregarObjetoEscena(newPart.sprite);
			_parts.agregarElemento(newPart);
		}
	}
}

void AUParticlesGenerator::generateRandomParticle(const float minInitVelX, const float maxInitVelX, const float minInitVelY, const float maxInitVelY, const NBColor8 color){
	this->generateRandomParticle(minInitVelX, maxInitVelX, minInitVelY, maxInitVelY, color.r, color.g, color.b, color.a);
}

void AUParticlesGenerator::generateRandomParticle(const float minInitVelX, const float maxInitVelX, const float minInitVelY, const float maxInitVelY, const UI8 r, const UI8 g, const UI8 b){
	if(_layer != NULL && _partsDef.conteo != 0){
		NBASSERT(ENPartsGenProbability_Count == 3)
		const SI32 maxProb = _partsDefsByProb[ENPartsGenProbability_Low] + (_partsDefsByProb[ENPartsGenProbability_Medium] * 2) + (_partsDefsByProb[ENPartsGenProbability_High] * 3);
		const SI32 iProb = (rand() % maxProb);
		SI32 iPartDef;
		if(iProb < _partsDefsByProb[ENPartsGenProbability_Low]){
			iPartDef = iProb;
		} else if(iProb < (_partsDefsByProb[ENPartsGenProbability_Low] + (_partsDefsByProb[ENPartsGenProbability_Medium] * 2))){
			iPartDef = _partsDefsByProb[ENPartsGenProbability_Low] + ((iProb - _partsDefsByProb[ENPartsGenProbability_Low]) / 2);
		} else {
			iPartDef = _partsDefsByProb[ENPartsGenProbability_Low] + _partsDefsByProb[ENPartsGenProbability_Medium] + ((iProb - _partsDefsByProb[ENPartsGenProbability_Low] - (_partsDefsByProb[ENPartsGenProbability_Medium] * 2)) / 3);
		}
		NBASSERT(iPartDef >= 0  && iPartDef < _partsDef.conteo)
		STPartsGenPartDef* partDef = _partsDef.elemPtr(iPartDef);
		if(partDef->texture != NULL){
			STPartsGenPart newPart;
			newPart.sprite		= new(this) AUEscenaSprite(partDef->texture);
			newPart.sprite->establecerTraslacion(_dstArea.x + (_dstArea.ancho * ((float)(rand() % 100) / 100.0f)), _dstArea.y + (_dstArea.alto * ((float)(rand() % 100) / 100.0f)));
			newPart.sprite->establecerMultiplicadorColor8(r, g, b, 0);
			newPart.alphaMax	= partDef->colorBase.a;
			newPart.secsAlive	= 0.0f;
			newPart.secsToLive	= partDef->secsToLive;
			newPart.velRot		= _defRotSpeedRange.inicio + (_defRotSpeedRange.tamano * ((float)(rand() % 100) / 100.0f));
			newPart.vel.x		= minInitVelX + ((maxInitVelX - minInitVelX) * ((float)(rand() % 100) / 100.0f));
			newPart.vel.y		= minInitVelY + ((maxInitVelY - minInitVelY) * ((float)(rand() % 100) / 100.0f));
			_layer->agregarObjetoEscena(newPart.sprite);
			_parts.agregarElemento(newPart);
		}
	}
}

void AUParticlesGenerator::generateRandomParticle(const float minInitVelX, const float maxInitVelX, const float minInitVelY, const float maxInitVelY, const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	if(_layer != NULL && _partsDef.conteo != 0){
		NBASSERT(ENPartsGenProbability_Count == 3)
		const SI32 maxProb = _partsDefsByProb[ENPartsGenProbability_Low] + (_partsDefsByProb[ENPartsGenProbability_Medium] * 2) + (_partsDefsByProb[ENPartsGenProbability_High] * 3);
		const SI32 iProb = (rand() % maxProb);
		SI32 iPartDef;
		if(iProb < _partsDefsByProb[ENPartsGenProbability_Low]){
			iPartDef = iProb;
		} else if(iProb < (_partsDefsByProb[ENPartsGenProbability_Low] + (_partsDefsByProb[ENPartsGenProbability_Medium] * 2))){
			iPartDef = _partsDefsByProb[ENPartsGenProbability_Low] + ((iProb - _partsDefsByProb[ENPartsGenProbability_Low]) / 2);
		} else {
			iPartDef = _partsDefsByProb[ENPartsGenProbability_Low] + _partsDefsByProb[ENPartsGenProbability_Medium] + ((iProb - _partsDefsByProb[ENPartsGenProbability_Low] - (_partsDefsByProb[ENPartsGenProbability_Medium] * 2)) / 3);
		}
		NBASSERT(iPartDef >= 0  && iPartDef < _partsDef.conteo)
		STPartsGenPartDef* partDef = _partsDef.elemPtr(iPartDef);
		if(partDef->texture != NULL){
			STPartsGenPart newPart;
			newPart.sprite		= new(this) AUEscenaSprite(partDef->texture);
			newPart.sprite->establecerTraslacion(_dstArea.x + (_dstArea.ancho * ((float)(rand() % 100) / 100.0f)), _dstArea.y + (_dstArea.alto * ((float)(rand() % 100) / 100.0f)));
			newPart.sprite->establecerMultiplicadorColor8(r, g, b, 0);
			newPart.alphaMax	= a;
			newPart.secsAlive	= 0.0f;
			newPart.secsToLive	= partDef->secsToLive;
			newPart.velRot		= _defRotSpeedRange.inicio + (_defRotSpeedRange.tamano * ((float)(rand() % 100) / 100.0f));
			newPart.vel.x		= minInitVelX + ((maxInitVelX - minInitVelX) * ((float)(rand() % 100) / 100.0f));
			newPart.vel.y		= minInitVelY + ((maxInitVelY - minInitVelY) * ((float)(rand() % 100) / 100.0f));
			_layer->agregarObjetoEscena(newPart.sprite);
			_parts.agregarElemento(newPart);
		}
	}
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUParticlesGenerator)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUParticlesGenerator, "AUParticlesGenerator")
AUOBJMETODOS_CLONAR_NULL(AUParticlesGenerator)






