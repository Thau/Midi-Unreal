// Copyright -> Scott Bishel

#include "MidiInterfacePrivatePCH.h"

#if PLATFORM_ANDROID
#include "../../../Core/Public/Android/AndroidApplication.h"
#include "../../../Launch/Public/Android/AndroidJNI.h"
#include <android/log.h>

#define LOG_TAG "MIDI_LOG"

void AndroidThunkCpp_startup();

int SetupJNIMIDI(JNIEnv* env);
JNIEnv* ENV = NULL;
static jmethodID jToast;
static jmethodID AndroidThunkJava_startMidi;
static jmethodID AndroidThunkJava_stopMidi;
static jmethodID AndroidThunkJava_openPort;
static jmethodID AndroidThunkJava_getPortCountIn;
static jmethodID AndroidThunkJava_getPortName;
static jmethodID AndroidThunkJava_closePort;
static jmethodID AndroidThunkJava_getPortCountOut;
static jmethodID AndroidThunkJava_sendMessage;
static bool newFrame = false;
static unsigned char* rawDataAndroid;
#endif

#define LOCTEXT_NAMESPACE "FMidiInterfaceModule"

void FMidiInterfaceModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
#if PLATFORM_ANDROID
	AndroidThunkCpp_startup();
#endif
}

void FMidiInterfaceModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMidiInterfaceModule, MidiInterface)

//-------------- Android -----------------------------

void AndroidThunkCpp_startup() {
#if PLATFORM_ANDROID
	JNIEnv* env = FAndroidApplication::GetJavaEnv();
	SetupJNIMIDI(env);
#endif
}

#if PLATFORM_ANDROID
int SetupJNIMIDI(JNIEnv* env)
{
	if (!env) return JNI_ERR;

	ENV = env;

	AndroidThunkJava_startMidi = FJavaWrapper::FindMethod(ENV, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_startMidi", "()V", false);
	AndroidThunkJava_stopMidi = FJavaWrapper::FindMethod(ENV, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_stopMidi", "()V", false);
	AndroidThunkJava_openPort = FJavaWrapper::FindMethod(ENV, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_openPort", "()V", false);
	AndroidThunkJava_getPortCountIn = FJavaWrapper::FindMethod(ENV, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_getPortCountIn", "()I", false);
	AndroidThunkJava_getPortName = FJavaWrapper::FindMethod(ENV, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_getPortName", "()S", false);
	AndroidThunkJava_closePort = FJavaWrapper::FindMethod(ENV, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_closePort", "()V", false);
	AndroidThunkJava_getPortCountOut = FJavaWrapper::FindMethod(ENV, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_getPortCountOut", "()I", false);
	AndroidThunkJava_sendMessage = FJavaWrapper::FindMethod(ENV, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_sendMessage", "(B[];I;)V", false);

	//FJavaWrapper::CallVoidMethod(ENV, FJavaWrapper::GameActivityThis, jToast);
	__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "module load success!!! ^_^");

	return JNI_OK;
}

void AndroidThunkCpp_startMidi()
{
	if (!AndroidThunkJava_startMidi || !ENV) return;
	FJavaWrapper::CallVoidMethod(ENV, FJavaWrapper::GameActivityThis, AndroidThunkJava_startMidi);
}

void AndroidThunkCpp_stopMidi()
{
	if (!AndroidThunkJava_stopMidi || !ENV) return;
	FJavaWrapper::CallVoidMethod(ENV, FJavaWrapper::GameActivityThis, AndroidThunkJava_stopMidi);
}

void AndroidThunkCpp_openPort()
{
	if (!AndroidThunkJava_openPort || !ENV) return;
	FJavaWrapper::CallVoidMethod(ENV, FJavaWrapper::GameActivityThis, AndroidThunkJava_openPort);
}

int AndroidThunkCpp_getPortCountIn()
{
	if (!AndroidThunkJava_getPortCountIn || !ENV) return -1;
	return FJavaWrapper::CallIntMethod(ENV, FJavaWrapper::GameActivityThis, AndroidThunkJava_getPortCountIn);
}

FString AndroidThunkCpp_getPortName(int portNumber)
{
	if (!AndroidThunkJava_getPortName || !ENV) return "";
	FJavaWrapper::CallObjectMethod(ENV, FJavaWrapper::GameActivityThis, AndroidThunkJava_getPortName, portNumber);

	return "";
}

void AndroidThunkCpp_closePort()
{
	if (!AndroidThunkJava_closePort || !ENV) return;
	FJavaWrapper::CallVoidMethod(ENV, FJavaWrapper::GameActivityThis, AndroidThunkJava_closePort);
}

int AndroidThunkCpp_getPortCountOut()
{
	if (!AndroidThunkJava_getPortCountOut || !ENV) return -1;
	return FJavaWrapper::CallIntMethod(ENV, FJavaWrapper::GameActivityThis, AndroidThunkJava_getPortCountOut);
}

void AndroidThunkCpp_sendMessage(std::vector<unsigned char> *message)
{
	if (!AndroidThunkJava_sendMessage || !ENV) return;
	//jbyte* msg = reinterpret_cast<jbyte*>(message->data);
	//int size = message->size();

	//FJavaWrapper::CallVoidMethod(ENV, FJavaWrapper::GameActivityThis, AndroidThunkJava_sendMessage);
}

extern "C" bool Java_com_epicgames_ue4_GameActivity_nativeGetMessage(JNIEnv* LocalJNIEnv, jobject LocalThiz, jbyteArray data)
{
	//get the new frame
	int length = LocalJNIEnv->GetArrayLength(data);
	unsigned char* buffer = new unsigned char[length];
	LocalJNIEnv->GetByteArrayRegion(data, 0, length, reinterpret_cast<jbyte*>(buffer));
	rawDataAndroid = buffer;
	newFrame = true;
	//__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "new frame arrive ^_^");
	return JNI_TRUE;
}
#endif

