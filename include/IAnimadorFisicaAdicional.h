//
//  IAnimadorFisicaAdicional.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 08/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Gameplay_Mac_IAnimadorFisicaAdicional_h
#define Gameplay_Mac_IAnimadorFisicaAdicional_h

class IAnimadorFisicaAdicional {
	public:
		virtual ~IAnimadorFisicaAdicional(){
			//
		}
		virtual void simularFisicaCuerpos(float segundosTranscurridos) = 0;
};

#endif
