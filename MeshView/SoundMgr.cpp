#include "SoundMgr.h"


SoundMgr::SoundMgr()
{
	InitFMOD();
}


SoundMgr::~SoundMgr()
{
}
void SoundMgr::InitFMOD()
{
	result = FMOD::System_Create(&system);

	//ERRCHECK(result);

	result = system->getVersion(&version);
	//ERRCHECK(result);

	if (version < FMOD_VERSION)
	{
		printf("Error! You are using an old version of FMOD %08x.");
		return;
	}

	int numdrivers = 0;

	char name[256];
	FMOD_CAPS caps;
	FMOD_SPEAKERMODE speakermode;

	result = system->getNumDrivers(&numdrivers);
	//ERRCHECK(result);
	if (numdrivers == 0)
	{
		result == system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		//ERRCHECK(result);
	}
	else
	{
		result = system->getDriverCaps(0, &caps, 0, &speakermode);
		//ERRCHECK(result);
		/*
		set the user selected speaker mode.
		*/
		result = system->setSpeakerMode(speakermode);
		//ERRCHECK(result);

		if (caps & FMOD_CAPS_HARDWARE_EMULATED)
		{
			/*
			The user has the 'Acceleration' slider set to off! This is really bad for latency! You might want to warn the user about this.
			*/
			result = system->setDSPBufferSize(1024, 10);
			//ERRCHECK(result);
		}
		result = system->getDriverInfo(0, name, 256, 0);
		//ERRCHECK(result);
		if (strstr(name, "SigmaTel"))
		{
			/*
			Sigmatel sound devices crackel for some reason if the format is PCM 16bit.
			PCM floating point outout seems to solve it.
			*/
			result = system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
			//ERRCHECK(result);
		}
	}
	result = system->init(100, FMOD_INIT_NORMAL, 0);
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		/*
		ok, the speaker mode selected isn't supported by this soundcard. Switch it back to stereo...
		*/
		result = system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		//ERRCHECK(result);
		/*
		...and re-init.
		*/
		result = system->init(100, FMOD_INIT_NORMAL, 0);
	}
	//ERRCHECK(result);




	FMOD::Sound* tempSound;
	result = system->createSound("sounds/dsnoway.wav", FMOD_HARDWARE, 0, &tempSound);
	sounds.push_back(tempSound);
	//ERRCHECK(result);
	playing.push_back(false);
	channels.push_back(0);
	result = sounds[GRUNT]->setMode(FMOD_LOOP_OFF);
	//ERRCHECK(result);

	result = system->createSound("sounds/FOOT2.wav", FMOD_HARDWARE, 0, &tempSound);
	sounds.push_back(tempSound);
	//ERRCHECK(result);
	playing.push_back(false);
	channels.push_back(0);
	result = sounds[STEP_1]->setMode(FMOD_LOOP_OFF);
	//ERRCHECK(result);

	result = system->createSound("sounds/pl_step2.wav", FMOD_HARDWARE, 0, &tempSound);
	sounds.push_back(tempSound);
	//ERRCHECK(result);
	playing.push_back(false);
	channels.push_back(0);
	result = sounds[STEP_2]->setMode(FMOD_LOOP_OFF);
	//ERRCHECK(result);



	result = system->createStream("sounds/CoolRide.mp3", FMOD_HARDWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &music);
	//ERRCHECK(result);
	result = system->playSound(FMOD_CHANNEL_FREE, music, false, &musicChannel);
	//ERRCHECK(result);

	//Set valume on the channel.
	result = musicChannel->setVolume(0.05f);
	//ERRCHECK(result);
}
void SoundMgr::UpdateSound()
{
	bool channelPlaying;
	for (int i = 0; i < channels.size(); i++)
	{
		channelPlaying = false;
		if (channels[i])
		{
			result = channels[i]->isPlaying(&channelPlaying);
		}
		if (playing[i] == true && !channelPlaying)
		{
			result = system->playSound(FMOD_CHANNEL_FREE, sounds[i], false, &channels[i]);
			playing[i] = false;
			//ERRCHECK(result);
		}
	}
	system->update();
}