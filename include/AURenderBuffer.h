//
//  Header.h
//  AUFramework
//
//  Created by Nicaragua Binary on 15/04/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AURENDERBUFFER_H_INCLUIDO
#define AURENDERBUFFER_H_INCLUIDO

#include "AUObjeto.h"
#include "NBTamano.h"

class AURenderBuffer : public AUObjeto {
	public:
		AURenderBuffer(UI32 idRenderBufferGl, FLOAT anchoDestinoGl, FLOAT altoDestinoGl, FLOAT escalaParaHD);
		virtual ~AURenderBuffer();
		//
		void					relinkearRenderBuffer(const UI32 idRenderBufferGl);
		//
		UI32					idRenderBufferGl();
		NBTamano				tamanoDestinoGl();
		FLOAT					escalaParaHD();
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		GLuint					_idRenderBufferGl;
		NBTamano				_tamanoDestinoGl;
		FLOAT					_escalaParaHD;
}; 

#endif