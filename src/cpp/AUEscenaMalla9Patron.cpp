//
//  AUEscenaMalla9Patron.cpp
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 06/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaMalla9Patron.h"

AUEscenaMalla9Patron::AUEscenaMalla9Patron(const char* prefijoRutaTexturas9) : AUEscenaMallaXBase(), _spritesSup(this), _spritesInf(this), _spritesIzq(this), _spritesDer(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMalla9Patron::AUEscenaMalla9Patron")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMalla9Patron")
	AUCadenaMutable8* rutaPNG = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("1.png"); _texturasMarco[0]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[0]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("2.png"); _texturasMarco[1]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[1]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("3.png"); _texturasMarco[2]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[2]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("4.png"); _texturasMarco[3]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[3]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("5.png"); _texturasMarco[4]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[4]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("6.png"); _texturasMarco[5]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[5]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("7.png"); _texturasMarco[6]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[6]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("8.png"); _texturasMarco[7]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[7]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("9.png"); _texturasMarco[8]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[8]->retener(NB_RETENEDOR_THIS);
	rutaPNG->liberar(NB_RETENEDOR_THIS);
	//
	_esquinaSupIzq		= new(this) AUEscenaSprite(_texturasMarco[0]); this->agregarObjetoEscena(_esquinaSupIzq); _esquinaSupIzq->liberar(NB_RETENEDOR_THIS);
	_esquinaSupDer 		= new(this) AUEscenaSprite(_texturasMarco[2]); this->agregarObjetoEscena(_esquinaSupDer); _esquinaSupDer->liberar(NB_RETENEDOR_THIS);
	_spriteCentro		= new(this) AUEscenaSprite(_texturasMarco[4]); this->agregarObjetoEscena(_spriteCentro); _spriteCentro->liberar(NB_RETENEDOR_THIS);
	_esquinaInfIzq 		= new(this) AUEscenaSprite(_texturasMarco[6]); this->agregarObjetoEscena(_esquinaInfIzq); _esquinaInfIzq->liberar(NB_RETENEDOR_THIS);
	_esquinaInfDer 		= new(this) AUEscenaSprite(_texturasMarco[8]); this->agregarObjetoEscena(_esquinaInfDer); _esquinaInfDer->liberar(NB_RETENEDOR_THIS);
	//
	establecerCajaMarcoMalla(cajaMinima());
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaMalla9Patron::~AUEscenaMalla9Patron(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMalla9Patron::~AUEscenaMalla9Patron")
	if(_texturasMarco[0] != NULL) _texturasMarco[0]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[1] != NULL) _texturasMarco[1]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[2] != NULL) _texturasMarco[2]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[3] != NULL) _texturasMarco[3]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[4] != NULL) _texturasMarco[4]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[5] != NULL) _texturasMarco[5]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[6] != NULL) _texturasMarco[6]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[7] != NULL) _texturasMarco[7]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[8] != NULL) _texturasMarco[8]->liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

NBCajaAABB AUEscenaMalla9Patron::cajaMinima(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMalla9Patron::cajaMinima")
	NBTamano tamanoMinimo;
	tamanoMinimo.ancho = _texturasMarco[0]->tamanoHD().ancho /*+ _texturasMarco[1]->tamanoHD().ancho*/ + _texturasMarco[2]->tamanoHD().ancho;
	tamanoMinimo.alto = _texturasMarco[0]->tamanoHD().alto /*+ _texturasMarco[3]->tamanoHD().alto*/ + _texturasMarco[6]->tamanoHD().alto;
	NBCajaAABB cajaMinima;
	NBCAJAAABB_INICIALIZAR(cajaMinima);
	NBCAJAAABB_ENVOLVER_PUNTO(cajaMinima, tamanoMinimo.ancho/-2.0f, tamanoMinimo.alto/-2.0f);
	NBCAJAAABB_ENVOLVER_PUNTO(cajaMinima, tamanoMinimo.ancho/2.0f, tamanoMinimo.alto/2.0f);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cajaMinima;
}

void AUEscenaMalla9Patron::actualizarPresentacionMalla(const NBCajaAABB &cajaMarco){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMalla9Patron::actualizarPresentacionMalla")
	NBTamano tamCajaMarco; NBCAJAAABB_TAMANO(tamCajaMarco, cajaMarco);
	NBPunto centroCajaMarco; NBCAJAAABB_CENTRO(centroCajaMarco, cajaMarco);
	//
	NBTamano tamEsquinas[4];
	tamEsquinas[0] = _texturasMarco[0]->tamanoHD();
	tamEsquinas[1] = _texturasMarco[2]->tamanoHD();
	tamEsquinas[2] = _texturasMarco[6]->tamanoHD();
	tamEsquinas[3] = _texturasMarco[8]->tamanoHD();
	NBTamano tamPatrones[4];
	tamPatrones[0] = _texturasMarco[1]->tamanoHD();
	tamPatrones[1] = _texturasMarco[3]->tamanoHD();
	tamPatrones[2] = _texturasMarco[5]->tamanoHD();
	tamPatrones[3] = _texturasMarco[7]->tamanoHD();
	//Acomodar las esquinas
	_esquinaSupIzq->establecerTraslacion(cajaMarco.xMin+(tamEsquinas[0].ancho/2.0f), cajaMarco.yMax-(tamEsquinas[0].alto/2.0f));
	_esquinaSupDer->establecerTraslacion(cajaMarco.xMax-(tamEsquinas[1].ancho/2.0f), cajaMarco.yMax-(tamEsquinas[1].alto/2.0f));
	_esquinaInfIzq->establecerTraslacion(cajaMarco.xMin+(tamEsquinas[2].ancho/2.0f), cajaMarco.yMin+(tamEsquinas[2].alto/2.0f));
	_esquinaInfDer->establecerTraslacion(cajaMarco.xMax-(tamEsquinas[2].ancho/2.0f), cajaMarco.yMin+(tamEsquinas[2].alto/2.0f));
	//Acomodar y estirar el centro
	_spriteCentro->establecerTraslacion(centroCajaMarco);
	_spriteCentro->redimensionar(tamCajaMarco.ancho-tamEsquinas[0].ancho-tamEsquinas[2].ancho, tamCajaMarco.alto-tamEsquinas[0].alto-tamEsquinas[2].alto);
	//
	SI32 iSprite; float posMinCubrir, posMaxCubrir;
	//Asegurar patron arriba
	posMinCubrir	= cajaMarco.xMin+tamEsquinas[0].ancho;
	posMaxCubrir	= cajaMarco.xMax-tamEsquinas[1].ancho;
	iSprite			= 0;
	while(posMinCubrir<posMaxCubrir){
		float relTamPatron = ((posMaxCubrir-posMinCubrir)>tamPatrones[0].ancho?1.0f:(posMaxCubrir-posMinCubrir)/tamPatrones[0].ancho);
		if(iSprite>=_spritesSup.conteo){
			NBASSERT(iSprite==_spritesSup.conteo);
			AUEscenaSpritePorcion* nuevaSprite = new(this) AUEscenaSpritePorcion(0.0f, 0.0f, tamPatrones[0].ancho*relTamPatron, -tamPatrones[0].alto);
			nuevaSprite->establecerTextura(_texturasMarco[1]);
			this->agregarObjetoEscena(nuevaSprite);
			_spritesSup.agregarElemento(nuevaSprite);
			nuevaSprite->liberar(NB_RETENEDOR_THIS);
		}
		AUEscenaSpritePorcion* sprite = (AUEscenaSpritePorcion*)_spritesSup.elemento[iSprite];
		sprite->redimensionar(0.0f, 0.0f, tamPatrones[0].ancho*relTamPatron, -tamPatrones[0].alto);
		sprite->establecerPorcionTextura(0.0f, relTamPatron, 0.0f, 1.0f);
		sprite->establecerTraslacion(posMinCubrir, cajaMarco.yMax);
		if(sprite->contenedor() == NULL) this->agregarObjetoEscena(sprite);
		//Siguiente
		posMinCubrir += tamPatrones[0].ancho;
		iSprite++;
	}
	//PRINTF_INFO("MALLA 9 Patron: %d texturas en patron superior.\n", iSprite);
	while(iSprite<_spritesSup.conteo){ 
		AUEscenaSpritePorcion* sprite = (AUEscenaSpritePorcion*)_spritesSup.elemento[iSprite];
		if(sprite->contenedor()==this) this->quitarObjetoEscena(sprite);
		iSprite++; 
	}
	//Asegurar patron abajo
	posMinCubrir	= cajaMarco.xMin+tamEsquinas[0].ancho;
	posMaxCubrir	= cajaMarco.xMax-tamEsquinas[1].ancho;
	iSprite			= 0;
	while(posMinCubrir<posMaxCubrir){
		float relTamPatron = ((posMaxCubrir-posMinCubrir)>tamPatrones[3].ancho?1.0f:(posMaxCubrir-posMinCubrir)/tamPatrones[3].ancho);
		if(iSprite>=_spritesInf.conteo){
			NBASSERT(iSprite==_spritesInf.conteo);
			AUEscenaSpritePorcion* nuevaSprite = new(this) AUEscenaSpritePorcion(0.0f, tamPatrones[3].alto, tamPatrones[3].ancho*relTamPatron, -tamPatrones[3].alto);
			nuevaSprite->establecerTextura(_texturasMarco[7]);
			this->agregarObjetoEscena(nuevaSprite);
			_spritesInf.agregarElemento(nuevaSprite);
			nuevaSprite->liberar(NB_RETENEDOR_THIS);
		}
		AUEscenaSpritePorcion* sprite = (AUEscenaSpritePorcion*)_spritesInf.elemento[iSprite];
		sprite->redimensionar(0.0f, tamPatrones[3].alto, tamPatrones[3].ancho*relTamPatron, -tamPatrones[3].alto);
		sprite->establecerPorcionTextura(0.0f, relTamPatron, 0.0f, 1.0f);
		sprite->establecerTraslacion(posMinCubrir, cajaMarco.yMin);
		if(sprite->contenedor() == NULL) this->agregarObjetoEscena(sprite);
		//Siguiente
		posMinCubrir += tamPatrones[3].ancho;
		iSprite++;
	}
	//PRINTF_INFO("MALLA 9 Patron: %d texturas en patron inferior.\n", iSprite);
	while(iSprite<_spritesInf.conteo){
		AUEscenaSpritePorcion* sprite = (AUEscenaSpritePorcion*)_spritesInf.elemento[iSprite];
		if(sprite->contenedor()==this) this->quitarObjetoEscena(sprite);
		iSprite++; 	
	}
	//Asegurar patron izquierda
	posMinCubrir	= cajaMarco.yMin+tamEsquinas[0].alto;
	posMaxCubrir	= cajaMarco.yMax-tamEsquinas[2].alto;
	iSprite			= 0;
	while(posMinCubrir<posMaxCubrir){
		float relTamPatron = ((posMaxCubrir-posMinCubrir)>tamPatrones[1].alto?1.0f:(posMaxCubrir-posMinCubrir)/tamPatrones[1].alto);
		if(iSprite>=_spritesIzq.conteo){
			NBASSERT(iSprite==_spritesIzq.conteo);
			AUEscenaSpritePorcion* nuevaSprite = new(this) AUEscenaSpritePorcion(0.0f, 0.0f, tamPatrones[1].ancho, tamPatrones[1].alto*relTamPatron);
			nuevaSprite->establecerTextura(_texturasMarco[3]);
			this->agregarObjetoEscena(nuevaSprite);
			_spritesIzq.agregarElemento(nuevaSprite);
			nuevaSprite->liberar(NB_RETENEDOR_THIS);
		}
		AUEscenaSpritePorcion* sprite = (AUEscenaSpritePorcion*)_spritesIzq.elemento[iSprite];
		sprite->redimensionar(0.0f, 0.0f, tamPatrones[1].ancho, tamPatrones[1].alto*relTamPatron);
		sprite->establecerPorcionTextura(0.0f, 1.0f, 0.0f, relTamPatron);
		sprite->establecerTraslacion(cajaMarco.xMin, posMinCubrir);
		if(sprite->contenedor() == NULL) this->agregarObjetoEscena(sprite);
		//Siguiente
		posMinCubrir += tamPatrones[1].alto;
		iSprite++;
	}
	//PRINTF_INFO("MALLA 9 Patron: %d texturas en patron izquierdo.\n", iSprite);
	while(iSprite<_spritesIzq.conteo){
		AUEscenaSpritePorcion* sprite = (AUEscenaSpritePorcion*)_spritesIzq.elemento[iSprite];
		if(sprite->contenedor()==this) this->quitarObjetoEscena(sprite);
		iSprite++; 	
	}
	//Asegurar patron derecha
	posMinCubrir	= cajaMarco.yMin+tamEsquinas[0].alto;
	posMaxCubrir	= cajaMarco.yMax-tamEsquinas[2].alto;
	iSprite			= 0;
	while(posMinCubrir<posMaxCubrir){
		float relTamPatron = ((posMaxCubrir-posMinCubrir)>tamPatrones[2].alto?1.0f:(posMaxCubrir-posMinCubrir)/tamPatrones[2].alto);
		if(iSprite>=_spritesDer.conteo){
			NBASSERT(iSprite==_spritesDer.conteo);
			AUEscenaSpritePorcion* nuevaSprite = new(this) AUEscenaSpritePorcion(-tamPatrones[2].ancho, 0.0f, tamPatrones[2].ancho, tamPatrones[2].alto*relTamPatron);
			nuevaSprite->establecerTextura(_texturasMarco[5]);
			this->agregarObjetoEscena(nuevaSprite);
			_spritesDer.agregarElemento(nuevaSprite);
			nuevaSprite->liberar(NB_RETENEDOR_THIS);
		}
		AUEscenaSpritePorcion* sprite = (AUEscenaSpritePorcion*)_spritesDer.elemento[iSprite];
		sprite->redimensionar(-tamPatrones[2].ancho, 0.0f, tamPatrones[2].ancho, tamPatrones[2].alto*relTamPatron);
		sprite->establecerPorcionTextura(0.0f, 1.0f, 0.0f, relTamPatron);
		sprite->establecerTraslacion(cajaMarco.xMax, posMinCubrir);
		if(sprite->contenedor() == NULL) this->agregarObjetoEscena(sprite);
		//Siguiente
		posMinCubrir += tamPatrones[1].alto;
		iSprite++;
	}
	//PRINTF_INFO("MALLA 9 Patron: %d texturas en patron derecho.\n", iSprite);
	while(iSprite<_spritesDer.conteo){ 
		AUEscenaSpritePorcion* sprite = (AUEscenaSpritePorcion*)_spritesDer.elemento[iSprite];
		if(sprite->contenedor()==this) this->quitarObjetoEscena(sprite);
		iSprite++; 	
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaMalla9Patron, AUEscenaMallaXBase)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaMalla9Patron, "AUEscenaMalla9Patron", AUEscenaMallaXBase)
AUOBJMETODOS_CLONAR_NULL(AUEscenaMalla9Patron)






