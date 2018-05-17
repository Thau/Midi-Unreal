// Copyright 2011 Alex Leffelman
// Updated 2016 Scott Bishel

#include "MidiThread.h"
#include "MidiPrivatePCH.h"

#include "Util/MidiProcessor.h"

bool FMidiProcessorWorker::IsFinished() const
{
	return !ThePC->isRunning();
}

//Constructor / Destructor
FMidiProcessorWorker::FMidiProcessorWorker(MidiProcessor* IN_PC, ETimingType TimingType)
	: ThePC(IN_PC)
{
	this->timingType = TimingType;

	Thread = FRunnableThread::Create(this, TEXT("FMidiProcessorWorker"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FMidiProcessorWorker::~FMidiProcessorWorker() {
	delete Thread;
	Thread = NULL;
}

bool FMidiProcessorWorker::Init() {
	return true;
}
uint32 FMidiProcessorWorker::Run() {
	UWorld* world = GEngine->GameViewport->GetWorld();
	if (!world)
		return 0;

	switch(timingType) {
	case ETimingType::GameTime:
		ThePC->setStartClock(world->TimeSeconds * 1000.0f);
		ThePC->milliFunction = NULL;
		break;
	case ETimingType::RealTime:
		ThePC->setStartClock(FPlatformTime::Cycles());
		ThePC->milliFunction = FPlatformTime::ToMilliseconds;
		break;
	case ETimingType::DeltaTime:
		ThePC->setStartClock(0.0f);
		ThePC->milliFunction = NULL;
		break;
	}

	while (!IsFinished())
	{
		switch(timingType) {
		case ETimingType::GameTime:
			ThePC->update(world->TimeSeconds * 1000.0f);
			break;
		case ETimingType::RealTime:
			ThePC->update(FPlatformTime::Cycles());
			break;
		case ETimingType::DeltaTime:
			ThePC->update(0.008f);
			break;
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//prevent thread from using too many resources
		FPlatformProcess::Sleep(0.008f);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	}
	return 0;
}
void FMidiProcessorWorker::Stop() {
	Thread->WaitForCompletion();
}
