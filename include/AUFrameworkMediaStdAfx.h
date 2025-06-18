//
//  AUFrameworkMediaStdAfx.h
//  DodoTown
//
//  Created by Marcos Ortega Morales on 09/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUFRAMEWORKMEDIA_PRECOMPILADO_INCLUIDO
#define AUFRAMEWORKMEDIA_PRECOMPILADO_INCLUIDO

#include "AUFrameworkBaseStdAfx.h"

//#if defined(ES1_GL_H_GUARD) || defined(GL_ES_VERSION_2_0)
//	#define NB_LIB_GRAFICA_ES_EMBEDDED
//#endif

//Macros para identificar el sistema operativo:
//http://sourceforge.net/p/predef/wiki/OperatingSystems/
#if defined(WIN32) || defined(_WIN32)
	//#pragma message("lib-auframework-media para windows")
	//#include <gl/GL.h>		//C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include\gl
	//#include <gl/GLU.h>		//C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include\gl
	#include "glew/glew.h"		//Esta es una libreria que permite acceder a las extensiones GL en Windows
	//Nota: es necesario agregar "glew32.lib" en el proyecto VS2008:
	//  -> Propiedades de proyecto / Propiedades de Configuracion / Vinculador / Entrada / Dependencias adicionales
	// O bien, incluir al inicio del archivo main: #pragma comment(lib, "glew32.lib")
	//Nota: es necesario agregar "opengl32.lib" en el proyecto VS2008:
	//  -> Propiedades de proyecto / Propiedades de Configuracion / Vinculador / Entrada / Dependencias adicionales
	// O bien, incluir al inicio del archivo main: #pragma comment(lib, "OpenAL32.lib")
#elif defined(__ANDROID__)
	//#pragma message("lib-auframework-media para android")
	#define NB_LIB_GRAFICA_ES_EMBEDDED
	#include <GLES/gl.h>
	#define GL_GLEXT_PROTOTYPES //Activa las definiciones de los metodos gl*OES(..);
	#include <GLES/glext.h>
#elif defined(__QNX__) //QNX 4, QNX Neutrino, or BlackBerry 10 OS
	//#pragma message("lib-auframework-media para BB10")
	#define NB_LIB_GRAFICA_ES_EMBEDDED
	#include <GLES/gl.h>
	#define GL_GLEXT_PROTOTYPES //Activa las definiciones de los metodos gl*OES(..);
	#include <GLES/glext.h>
#elif defined(__linux__) || defined(linux)
	//#pragma message("lib-auframework-media para linux")
	//#include "glew/glew.h"		//Esta es una libreria que permite acceder a las extensiones GL en Windows
	#include <GL/glew.h>
	//#include <GL/gl.h>
	//#include <GL/glext.h>
#elif defined(__MAC_OS_X_VERSION_MAX_ALLOWED)
	//#pragma message("lib-auframework-media para osx")
	#include <OpenGL/gl.h>
#else
	//#pragma message("lib-auframework-media para ios")
	#define NB_LIB_GRAFICA_ES_EMBEDDED
	#include <OpenGLES/ES1/gl.h>
	#include <OpenGLES/ES1/glext.h>
#endif

#ifdef __OBJC__
	#ifndef __MAC_OS_X_VERSION_MAX_ALLOWED
		#import <OpenGLES/EAGL.h>
		#import <OpenGLES/EAGLDrawable.h>
	#endif
#endif

#include "AUFrameworkMediaDefiniciones.h"

#endif
