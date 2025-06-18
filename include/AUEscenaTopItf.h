#ifndef AUESCENATOPITF_H_INCLUIDO
#define AUESCENATOPITF_H_INCLUIDO

#include "AUEscenaTopItfDefs.h"
#include "nb/2d/NBPoint.h"
#include "AUEscenaObjeto.h"

class AUEscenaTopItf {
	public:
		virtual ~AUEscenaTopItf(){
			//
		}
		//
		virtual BOOL topShowQuickTipTextAtViewPort(const STNBPointI viewPortPos, const char* text, AUTextura* iconLeft, AUTextura* iconRight, const float secsShow, const ENEscenaTipAim aimDir) = 0;
		virtual BOOL topAddObject(AUEscenaObjeto* object) = 0;
};


#endif
