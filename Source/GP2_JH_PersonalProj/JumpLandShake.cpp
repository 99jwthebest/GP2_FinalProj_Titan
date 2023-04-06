// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpLandShake.h"

#include "DSP/WaveTableOsc.h"

UJumpLandShake::UJumpLandShake()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	

	OscillationDuration = 0.25f;
	OscillationBlendInTime = 0.1f;
	OscillationBlendOutTime = 0.2f;

	//Pitch
	RotOscillation.Pitch.Amplitude = -50.0f;
	RotOscillation.Pitch.Frequency = 1.0f;
	RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	RotOscillation.Pitch.Waveform = EOscillatorWaveform::SineWave;

	//Yaw
	RotOscillation.Yaw.Amplitude = 0.0f;
	RotOscillation.Yaw.Frequency = 0.0f;
	RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Yaw.Waveform = EOscillatorWaveform::SineWave;

	//Roll
	RotOscillation.Roll.Amplitude = 0.0f;
	RotOscillation.Roll.Frequency = 0.0f;
	RotOscillation.Roll.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Roll.Waveform = EOscillatorWaveform::SineWave;

	
	//Loc Oscillation
	//X
	LocOscillation.X.Amplitude = 0.0f;
	LocOscillation.X.Frequency = 0.0f;
	LocOscillation.X.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.X.Waveform = EOscillatorWaveform::SineWave;

	//Y
	LocOscillation.Y.Amplitude = 0.0f;
	LocOscillation.Y.Frequency = 0.0f;
	LocOscillation.Y.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.Y.Waveform = EOscillatorWaveform::SineWave;

	//Z
	LocOscillation.Z.Amplitude = 0.0f;
	LocOscillation.Z.Frequency = 0.0f;
	LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.Z.Waveform = EOscillatorWaveform::SineWave;
}
