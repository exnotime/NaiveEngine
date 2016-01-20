/**************************************************
Zlib Copyright 2015 Daniel "MonzUn" Bengtsson & David Pejtersen
***************************************************/
#include "Randomizer.h"
//placed this function here so vc++ will generate .dll file for the other projects place it back in the future //Henrik J
void Randomizer::Seed(const unsigned int newSeed)
{
	m_Seed = newSeed;
	m_IsSeeded = true;
	m_RandomizationCounter = 0;
	srand(newSeed);

	for (auto& val : m_RandVal)
		val = 123456789123456789ULL + newSeed;
};