//
//  main.c
//  NixtlaDemo
//
//  Created by Marcos Ortega on 11/02/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#include <stdio.h>	//printf
#include <stdlib.h>	//malloc, free, rand()
#include <time.h>   //time
#include <string.h> //memset

#include <jni.h>
#include <android/log.h>	//for __android_log_print()

#include "NixDemosCommon.h"
#include "NixDemoPlayWav.h"
#include "NixDemoPlayStream.h"
#include "NixDemoCaptureEco.h"
#include "../tests/NixTestSamplesConverter.h"

STNixDemosCommon    gCommon = STNixDemosCommon_Zero;
STNixDemoPlayWav    gDemoPlayWav = STNixDemoPlayWav_Zero;
STNixDemoPlayStream gDemoPlayStream = STNixDemoPlayStream_Zero;
STNixDemoCaptureEco gDemoCaptureEco = STNixDemoCaptureEco_Zero;
STNixTestSamplesConverter gTestSamplesConverter = STNixTestSamplesConverter_Zero;

//STNixDemosCommon

JNIEXPORT jboolean JNICALL Java_com_mortegam_nixtla_1audio_MainActivity_commonInit(JNIEnv* env, jobject obj){
    if(NixDemosCommon_isNull(&gCommon)){
        return NixDemosCommon_init(&gCommon);
    }
    return NIX_FALSE;
}

JNIEXPORT void JNICALL Java_com_mortegam_nixtla_1audio_MainActivity_commonDestroy(JNIEnv* env, jobject obj){
    if(!NixDemosCommon_isNull(&gCommon)){
        if(!NixDemoPlayWav_isNull(&gDemoPlayWav)){
            NixDemoPlayWav_destroy(&gDemoPlayWav);
        }
        if(!NixDemoPlayStream_isNull(&gDemoPlayStream)){
            NixDemoPlayStream_destroy(&gDemoPlayStream);
        }
        if(!NixDemoCaptureEco_isNull(&gDemoCaptureEco)){
            NixDemoCaptureEco_destroy(&gDemoCaptureEco);
        }
        if(!NixTestSamplesConverter_isNull(&gTestSamplesConverter)){
            NixTestSamplesConverter_destroy(&gTestSamplesConverter, &gCommon);
        }
        NixDemosCommon_destroy(&gCommon);
    }
}

JNIEXPORT void JNICALL Java_com_mortegam_nixtla_1audio_MainActivity_commonTick(JNIEnv* env, jobject obj, jint msPassed){
    if(!NixDemosCommon_isNull(&gCommon)){
        NixDemosCommon_tick(&gCommon, msPassed);
    }
}

//STNixDemoPlayWav

JNIEXPORT jboolean JNICALL Java_com_mortegam_nixtla_1audio_MainActivity_demoPlayWavInit(JNIEnv* env, jobject obj, jobject assetManager){
    if(gCommon.ctx.itf == NULL){
        NIX_PRINTF_ERROR("NixDemosCommon_init::gCommon.ctx.itf == NULL.\n");
    } else if(gCommon.ctx.itf->mem.malloc == NULL){
        NIX_PRINTF_ERROR("NixDemosCommon_init::gCommon.ctx.itf->mem.malloc == NULL.\n");
    }
    if(!NixDemosCommon_isNull(&gCommon) && NixDemoPlayWav_isNull(&gDemoPlayWav)){
        return NixDemoPlayWav_init(&gDemoPlayWav, &gCommon, env, assetManager);
    }
    return NIX_FALSE;
}

JNIEXPORT void JNICALL Java_com_mortegam_nixtla_1audio_MainActivity_demoPlayWavDestroy(JNIEnv* env, jobject obj){
    if(!NixDemoPlayWav_isNull(&gDemoPlayWav)){
        NixDemoPlayWav_destroy(&gDemoPlayWav);
    }
}

//STNixDemoPlayStream

JNIEXPORT jboolean JNICALL Java_com_mortegam_nixtla_1audio_MainActivity_demoPlayStreamInit(JNIEnv* env, jobject obj, jobject assetManager){
    if(!NixDemosCommon_isNull(&gCommon) && NixDemoPlayStream_isNull(&gDemoPlayStream)){
        return NixDemoPlayStream_init(&gDemoPlayStream, &gCommon, env, assetManager);
    }
    return NIX_FALSE;
}

JNIEXPORT void JNICALL Java_com_mortegam_nixtla_1audio_MainActivity_demoPlayStreamDestroy(JNIEnv* env, jobject obj){
    if(!NixDemoPlayStream_isNull(&gDemoPlayStream)){
        NixDemoPlayStream_destroy(&gDemoPlayStream);
    }
}

//STNixDemoCaptureEco

JNIEXPORT jboolean JNICALL Java_com_mortegam_nixtla_1audio_MainActivity_demoCaptureEcoInit(JNIEnv* env, jobject obj){
    if(!NixDemosCommon_isNull(&gCommon) && NixDemoCaptureEco_isNull(&gDemoCaptureEco)){
        return NixDemoCaptureEco_init(&gDemoCaptureEco, &gCommon);
    }
    return NIX_FALSE;
}

JNIEXPORT void JNICALL Java_com_mortegam_nixtla_1audio_MainActivity_demoCaptureEcoDestroy(JNIEnv* env, jobject obj){
    if(!NixDemoCaptureEco_isNull(&gDemoCaptureEco)){
        NixDemoCaptureEco_destroy(&gDemoCaptureEco);
    }
}

//STNixTestSamplesConverter

JNIEXPORT jboolean JNICALL Java_com_mortegam_nixtla_1audio_MainActivity_testSamplesConverterInit(JNIEnv* env, jobject obj, jobject assetManager){
    if(!NixDemosCommon_isNull(&gCommon) && NixTestSamplesConverter_isNull(&gTestSamplesConverter)){
        return NixTestSamplesConverter_init(&gTestSamplesConverter, &gCommon, env, assetManager);
    }
    return NIX_FALSE;
}

JNIEXPORT void JNICALL Java_com_mortegam_nixtla_1audio_MainActivity_testSamplesConverterDestroy(JNIEnv* env, jobject obj){
    if(!NixTestSamplesConverter_isNull(&gTestSamplesConverter)){
        NixTestSamplesConverter_destroy(&gTestSamplesConverter, &gCommon);
    }
}

JNIEXPORT void JNICALL Java_com_mortegam_nixtla_1audio_MainActivity_testSamplesConverterTick(JNIEnv* env, jobject obj){
    if(!NixTestSamplesConverter_isNull(&gTestSamplesConverter)){
        NixTestSamplesConverter_tick(&gTestSamplesConverter, &gCommon);
    }
}
