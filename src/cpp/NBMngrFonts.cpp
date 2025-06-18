//
//  NBMngrFonts.cpp
//  lib-auframework-media
//
//  Created by Marcos Ortega on 31/1/19.
//

#include "AUFrameworkMediaStdAfx.h"
#include "NBMngrFonts.h"

AUMngrFonts* NBMngrFonts::_instance	= NULL;

void NBMngrFonts::init(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::init")
	_instance	= new AUMngrFonts();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void NBMngrFonts::finish(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::finish")
	if(_instance != NULL) _instance->liberar(NB_RETENEDOR_NULL); _instance = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool NBMngrFonts::isInited(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::isInited")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return (_instance != NULL);
}

//

STNBSize NBMngrFonts::getPPI(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::getPPI")
	const STNBSize r = _instance->getPPI();
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

ENMngrFontSizeMode NBMngrFonts::getSizeMode(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::getSizeMode")
	const ENMngrFontSizeMode r = _instance->getSizeMode();
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

const char* NBMngrFonts::getFontFamilyForType(const ENMngrFontType type){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::getFontFamilyForType")
	const char* r = _instance->getFontFamilyForType(type);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

//

void NBMngrFonts::setPPI(const STNBSize ppi){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::setPPI")
	_instance->setPPI(ppi);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void NBMngrFonts::setCurSizeMode(const ENMngrFontSizeMode szMode){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::setCurSizeMode")
	_instance->setCurSizeMode(szMode);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void NBMngrFonts::setFontFamilyForType(const ENMngrFontType type, const char* fontFamiliy){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::setFontFamilyForType")
	_instance->setFontFamilyForType(type, fontFamiliy);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

/*void NBMngrFonts::updateFonts(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::updateFonts")
	_instance->updateFonts();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}*/

//fontTextures

AUFuenteTextura* NBMngrFonts::fontTextures(const ENMngrFontType type){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::fontTextures")
	AUFuenteTextura* r = _instance->fontTextures(type);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

AUFuenteTextura* NBMngrFonts::fontTextures(const char* family, const char* subfamily, const float szPts){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::fontTextures")
	AUFuenteTextura* r = _instance->fontTextures(family, subfamily, szPts);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

AUFuenteTextura* NBMngrFonts::fontTextures(const char* family, const BOOL bold, const BOOL italic, const float szPts){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::fontTextures")
	AUFuenteTextura* r = _instance->fontTextures(family, bold, italic, szPts);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

//

AUFuenteTextura* NBMngrFonts::fontTexturesInPixels(const ENMngrFontType type, const float szPxs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::fontTexturesInPixels")
	AUFuenteTextura* r = _instance->fontTexturesInPixels(type, szPxs);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

AUFuenteTextura* NBMngrFonts::fontTexturesInPixels(const char* family, const char* subfamily, const float szPxs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::fontTexturesInPixels")
	AUFuenteTextura* r = _instance->fontTexturesInPixels(family, subfamily, szPxs);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

AUFuenteTextura* NBMngrFonts::fontTexturesInPixels(const char* family, const BOOL bold, const BOOL italic, const float szPxs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::fontTexturesInPixels")
	AUFuenteTextura* r = _instance->fontTexturesInPixels(family, bold, italic, szPxs);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

//fontRaster

AUFuenteRaster* NBMngrFonts::fontRaster(const ENMngrFontType type){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::fontRaster")
	AUFuenteRaster* r = _instance->fontRaster(type);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

AUFuenteRaster* NBMngrFonts::fontRaster(const char* family, const char* subfamily, const float szPts){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::fontRaster")
	AUFuenteRaster* r = _instance->fontRaster(family, subfamily, szPts);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

AUFuenteRaster* NBMngrFonts::fontRaster(const char* family, const BOOL bold, const BOOL italic, const float szPts){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::fontRaster")
	AUFuenteRaster* r = _instance->fontRaster(family, bold, italic, szPts);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

//

void NBMngrFonts::lock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::lock")
	_instance->lock();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void NBMngrFonts::unlock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBMngrFonts::unlock")
	_instance->unlock();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

