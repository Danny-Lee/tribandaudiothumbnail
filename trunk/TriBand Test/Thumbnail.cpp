/*
 *  Thumbnail.cpp
 *  haydxn_tutorial
 *
 *  Created by David Rowland on 10/05/2009.
 *  Copyright 2009 dRowAudio. All rights reserved.
 *
 */

#include "Thumbnail.h"

Thumbnail::Thumbnail(const int sourceSamplesPerThumbnailSample_)
:	currentFilePlayer(0),
	reader(0),
	sourceSamplesPerThumbnailSample (sourceSamplesPerThumbnailSample_),
	lowResMixedMonoCache(0)
{
	
}


Thumbnail::~Thumbnail ()
{
	
}

void Thumbnail::paint(Graphics &g)
{
	g.fillAll(Colours::black);
	g.setColour(Colours::green);
	if ( (currentFilePlayer != 0) && (currentFilePlayer->getAudioFormatReaderSource() !=0) )
		drawChannel(g, 0, 0, getWidth(), getHeight(),
					0, 0, 0, verticalZoomFactor);
}

void Thumbnail::setSource (AudioFilePlayer *newFilePlayer)
{
	currentFilePlayer = newFilePlayer;
	
    if ( currentFilePlayer != 0 )
		refresh();
}

void Thumbnail::refresh()
{
	if ( (currentFilePlayer != 0) && (currentFilePlayer->getAudioFormatReaderSource() !=0) )
	{
		reader = currentFilePlayer->getAudioFormatReaderSource()->getAudioFormatReader();

		if (reader != 0)
		{
			noChannels = reader->numChannels;
			sampleRate = reader->sampleRate;
			noSourceSamples = reader->lengthInSamples;
			
			noLowResCacheSamples = (int)(noSourceSamples / sourceSamplesPerThumbnailSample);
			
			DBG(String(T("No channels: "))<<(int)noChannels
				<< "\nSample rate: "<<(int)sampleRate
				<< "\nNo. Source samples: "<<(int)noSourceSamples
				<< "\nNo. Low res cache samples: "<<(int)noLowResCacheSamples<<"\n");
			
			fillLowResMixedMonoCache();
		}
	}
}

bool Thumbnail::fillLowResMixedMonoCache()
{
	MemoryBlock originalData(noSourceSamples * sizeof(int));
	int **sourceSamples = new int*[2];
	sourceSamples[0] = (int*)originalData.getData();
	sourceSamples[1] = 0;
	
	reader->read(sourceSamples,
				 0, noSourceSamples);
	
	// convert data to float if needed
	if (reader->usesFloatingPointData )
		printf("Uses floating point data\n");
	else
	{
		printf("Does not use floating point data\n");
		#if JUCE_BIG_ENDIAN
			AudioDataConverters::convertInt32BEToFloat((void*)sourceSamples[0], (float*)sourceSamples[0], noSourceSamples, sizeof(int));
		#else
			AudioDataConverters::convertInt32LEToFloat((void*)sourceSamples[0], (float*)sourceSamples[0], noSourceSamples, sizeof(int));
		#endif
	}
	
	mixedMonoFloatData = (float*)sourceSamples[0];
	delete[] lowResMixedMonoCache;
	lowResMixedMonoCache = new float[noLowResCacheSamples];

//	downSampleCache(mixedMonoFloatData, lowResMixedMonoCache, sourceSamplesPerThumbnailSample);
	
	lowResMixedMonoCacheMaximums = new float[noLowResCacheSamples];
	lowResMixedMonoCacheMinimums = new float[noLowResCacheSamples];
	findMaxMin(mixedMonoFloatData, noSourceSamples, lowResMixedMonoCacheMaximums, lowResMixedMonoCacheMinimums, noLowResCacheSamples);
	
	return true;
}

void Thumbnail::downSampleCache(float* sourceData, float* desinationData, int sourceSamplesPerThumbnailSample)
{
	for (int s = 0; s < noLowResCacheSamples; ++s)
	{
		int startSample = s * sourceSamplesPerThumbnailSample;
		int endSample = (s + 1) * sourceSamplesPerThumbnailSample;
		
		float avg = 0;
		for (int i = startSample; i < endSample; ++i)
		{
			avg += sourceData[i];
		}
		avg /= sourceSamplesPerThumbnailSample;
		desinationData[s] = avg;

		DBG(String((int)s)<<"\t- Start: "<<startSample<<"\t\t\t- End: "<<endSample << "\t\t\t - " << desinationData[s]);
	}
}

void Thumbnail::findMaxMin(float* sourceCache, int noSourceSamples, float* destinationMaxCache, float* destinationMinCache, int noDestinationSamples)
{
	int noSourceToDestinationSamples = (int)((double)noSourceSamples / (double)noDestinationSamples);

	for (int d = 0; d < noDestinationSamples; ++d)
	{
		int startSample = d * noSourceToDestinationSamples;
		int endSample = (d + 1) * noSourceToDestinationSamples;
		
		float max = 0.0f;
		float min = 0.0f;
		for (int s = startSample; s < endSample; ++s)
		{
			float sample = sourceCache[s];

			if (sample > max)
				max = sample;
			else if (sample < min)
				min = sample;
		}
		destinationMaxCache[d] = max;
		destinationMinCache[d] = min;
	}
}

void Thumbnail::findMaxMin(float* sourceCacheMaxs, float* sourceCacheMins, int noSourceSamples, int startSample,
						   float* destinationMaxCache, float* destinationMinCache, int noDestinationSamples)
{
	int noSourceToDestinationSamples = (int)((double)(noSourceSamples) / (double)noDestinationSamples);
	
	for (int d = 0; d < noDestinationSamples; ++d)
	{
		int startSample = d * noSourceToDestinationSamples;
		int endSample = (d + 1) * noSourceToDestinationSamples;
		
		float max = 0.0f;
		float min = 0.0f;
		for (int s = startSample; s < endSample; ++s)
		{
			float sampleMax = sourceCacheMaxs[s];
			float sampleMin = sourceCacheMins[s];
			
			if (sampleMax > max)
				max = sampleMax;
			else if (sampleMin > max)
				max = sampleMin;
			if (sampleMin < min)
				min = sampleMin;
			else if (sampleMax < min)
				min = sampleMin;
		}
		destinationMaxCache[d] = max;
		destinationMinCache[d] = min;
	}
}

void Thumbnail::drawChannel (Graphics& g,
							 int x, int y, int w, int h,
							 double startTime,
							 double endTime,
							 int channelNum,
							 const float verticalZoomFactor)
{
	int centreY = y + (0.5 * h);
	float verticalScale = (0.5 * h) * verticalZoomFactor;
	int startSample = horizontalZoomFactor * noLowResCacheSamples;
	
	if (w > noLowResCacheSamples)
		w = noLowResCacheSamples;
	
	float displayCacheMax[w];
	float displayCacheMin[w];
	findMaxMin(lowResMixedMonoCacheMaximums, lowResMixedMonoCacheMinimums, noLowResCacheSamples, startSample,
			   displayCacheMax, displayCacheMin, w);
	
	for (int i = 0; i < w; ++i)
	{
		g.drawLine(x+i, centreY+(verticalScale*displayCacheMax[i]), x+i, centreY+(verticalScale*displayCacheMin[i]));
//		g.drawLine(x+i, centreY+(scale*lowResMixedMonoCacheMaximums[i]), x+i, centreY+(scale*lowResMixedMonoCacheMinimums[i]));
	}
}
