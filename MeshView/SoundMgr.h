#pragma once
#include "d3dUtil.h"
#include <fmod.hpp>
#include <fmod_errors.h>
class SoundMgr
{
public:
	SoundMgr();
	~SoundMgr();
	//FMOD stuff(for sound, Move to a sperate class)
	FMOD::System* system;
	FMOD_RESULT result;
	FMOD::Sound* music;
	std::vector < FMOD::Sound* > sounds;
	FMOD::Channel*  musicChannel;
	std::vector < FMOD::Channel* > channels;	
	std::vector < BOOL > playing;
	int		key;
	unsigned int	version;
	void InitFMOD();
	void UpdateSound();

	void ChangeVolume(int ch, float in)
	{
		channels[ch]->setVolume(in);
	}

	void ChangeFrequency(int ch, float in)
	{

		float temp = 0;
		channels[ch]->getFrequency(&temp);
		channels[ch]->setFrequency(in* temp);
	}


};

