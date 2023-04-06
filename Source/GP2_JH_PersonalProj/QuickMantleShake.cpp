// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickMantleShake.h"


UQuickMantleShake::UQuickMantleShake()
{
	OscillationDuration = 0.25f;
	OscillationBlendInTime = 0.1f;
	OscillationBlendOutTime = 0.2f;

	//Pitch
	RotOscillation.Pitch.Amplitude = 0.0f;
	RotOscillation.Pitch.Frequency = 0.0f;
	RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Pitch.Waveform = EOscillatorWaveform::SineWave;

	//Yaw
	RotOscillation.Yaw.Amplitude = 0.0f;
	RotOscillation.Yaw.Frequency = 0.0f;
	RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Yaw.Waveform = EOscillatorWaveform::SineWave;

	//Roll
	RotOscillation.Roll.Amplitude = -25.0f;
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
	LocOscillation.Z.Amplitude = 100.0f;
	LocOscillation.Z.Frequency = 1.0f;
	LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	LocOscillation.Z.Waveform = EOscillatorWaveform::SineWave;
}
