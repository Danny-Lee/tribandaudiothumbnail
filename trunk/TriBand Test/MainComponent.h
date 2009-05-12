/*
 ==============================================================================
 
 JUCE library : Starting point code, v1.26
 Copyright 2005 by Julian Storer. [edited by haydxn, 3rd April 2007]
 Modified 2008 by David Rowland
 
 This is a modified base application project that uses the recent Juce cocoa code
 It is split into separate cpp and h files and uses some preprocessor macros
 
 ==============================================================================
 */
#ifndef _MAINCOMPONENT_H_
#define _MAINCOMPONENT_H_

#include <juce/juce.h>
#include <dRowAudio/dRowAudio.h>
#include "Thumbnail.h"

class MainComponent  : public Component,
					   public AudioIODeviceCallback,
					   public ButtonListener,
					   public SliderListener,
					   public FilenameComponentListener
{
private:
	
	AudioDeviceManager audioDeviceManager;
	AudioSourcePlayer audioSourcePlayer;
	AudioFilePlayer filePlayer;
	
	// controls
	FilenameComponent *sourceFilenameComponent;
	TextButton *playButton, *pauseButton;
	Slider *verticalZoomSlider, *horizontalZoomSlider, *offsetSlider;

	Thumbnail *thumbnail;

	double startTime, endTime, trackLength;
	
public:
	//==============================================================================
	MainComponent ();
	
	~MainComponent ();
	
	//==============================================================================
	void resized ();
	
	void paint (Graphics& g);

	//==============================================================================
	void buttonClicked(Button *button);
	
	void sliderValueChanged (Slider *changedSlider);
	
	void filenameComponentChanged (FilenameComponent* filenameComponent);
	
	//==============================================================================

	void audioDeviceIOCallback (const float** inputChannelData,
								int totalNumInputChannels,
								float** outputChannelData,
								int totalNumOutputChannels,
								int numSamples);
	
	void audioDeviceAboutToStart (AudioIODevice* device);
	
	void audioDeviceStopped();
};

#endif//_MAINCOMPONENT_H_ 