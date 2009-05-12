/*
 *  Thumbnail.h
 *  haydxn_tutorial
 *
 *  Created by David Rowland on 10/05/2009.
 *  Copyright 2009 dRowAudio. All rights reserved.
 *
 */

#ifndef _THUMBNAIL_H_
#define _THUMBNAIL_H_

#include <juce/juce.h>
#include <dRowAudio/dRowAudio.h>

class Thumbnail : public Component
{
public:
	
	Thumbnail(const int sourceSamplesPerThumbnailSample);
	
    /** Destructor. */
    ~Thumbnail ();
	
	void paint(Graphics &g);
			
	void setSource (AudioFilePlayer *newFilePlayer);

	/// refreshes the thumbnail's caches
	void refresh();

	void setVerticalZoomFactor(float newVerticalZoomFactor)	{	verticalZoomFactor = newVerticalZoomFactor;	}
	void setHorizontalZoomFactor(float newHorizontalZoomFactor)	{	horizontalZoomFactor = newHorizontalZoomFactor;	}
	
	void drawChannel (Graphics& g,
                      int x, int y, int w, int h,
                      double startTime,
                      double endTime,
                      int channelNum,
                      const float verticalZoomFactor);
	
private:
	
	CriticalSection readerLock;
	AudioFilePlayer	*currentFilePlayer;
	AudioFormatReader *reader;

	
	const int sourceSamplesPerThumbnailSample;
	double sampleRate;
	int noChannels;
	int64 noSourceSamples, noLowResCacheSamples; 
		
	float* mixedMonoFloatData;
	float *lowResMixedMonoCache, *lowResMixedMonoCacheMaximums, *lowResMixedMonoCacheMinimums;

	void downSampleCache(float* sourceData, float* desinationData, int sourceSamplesPerThumbnailSample);
	bool fillLowResMixedMonoCache();
	void findMaxMin(float* sourceCache, int noSourceSamples, float* destinationMaxCache, float* destinationMinCache, int noDestinationSamples);
	void findMaxMin(float* sourceCacheMaxs, float* sourceCacheMins, int noSourceSamples, int startSample,
					float* destinationMaxCache, float* destinationMinCache, int noDestinationSamples);
//	void findMaxMin(float* sourceStartSampleHighest, float* sourceStartSampleLowest, int sourceNumSamples,
//					float* destBufferHighest, float* destBufferLowest, int destBufferSize);
	
	float verticalZoomFactor, horizontalZoomFactor;
};

#endif //_THUMBNAIL_H_