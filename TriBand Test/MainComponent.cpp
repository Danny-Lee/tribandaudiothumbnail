/*
 *  MainComponent.cpp
 *  haydxn_tutorial
 *
 *  Created by David Rowland on 27/12/2008.
 *  Copyright 2008 UWE. All rights reserved.
 *
 */

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent ()
	:	startTime(0.0),
		trackLength(5.00)
{
	// set up audio
	audioDeviceManager.initialise (2, /* number of input channels */
								   2, /* number of output channels */
								   0, /* no XML settings.. */
								   true  /* select default device on failure */);
	audioSourcePlayer.setSource (&filePlayer);
	audioDeviceManager.setAudioCallback (this);
	
	
	// set up controls
	sourceFilenameComponent = new FilenameComponent(String("inputFilenameComp"), File::nonexistent, true, false, false, String::empty, String::empty, String("Source file"));
	addAndMakeVisible( sourceFilenameComponent );
	sourceFilenameComponent->addListener(this);
					  
	addAndMakeVisible( playButton = new TextButton("Play") );
	playButton->addButtonListener(this);
	playButton->setColour(TextButton::buttonColourId, Colours::lightgreen);
	
	addAndMakeVisible( pauseButton = new TextButton("Pause") );
	pauseButton->addButtonListener(this);
	
	addAndMakeVisible( verticalZoomSlider = new Slider("verticalZoomSlider") );
	verticalZoomSlider->addListener(this);
	verticalZoomSlider->setRange(0, 5, 0.001);
	verticalZoomSlider->setValue(1.0);
	( new Label("verticalZoomSlider", "Vertical Zoom:"))->attachToComponent(verticalZoomSlider, true);
	
	addAndMakeVisible( horizontalZoomSlider = new Slider("horizontalZoomSlider") );
	horizontalZoomSlider->addListener(this);
	horizontalZoomSlider->setRange(0.01, 2.0, 0.01);
	horizontalZoomSlider->setValue(1.0);
	( new Label("horizontalZoomSlider", "Horizontal Zoom:"))->attachToComponent(horizontalZoomSlider, true);
	
	addAndMakeVisible( offsetSlider = new Slider("offsetSlider") );
	offsetSlider->addListener(this);
	offsetSlider->setRange(0.0, 100.0, 0.001);
	
	addAndMakeVisible( thumbnail = new Thumbnail(512) );
	thumbnail->setSource(&filePlayer);
}

MainComponent::~MainComponent ()
{
	// zero the device manager and source player to avoid crashing
	audioDeviceManager.setAudioCallback (0);
	audioSourcePlayer.setSource (0);
	
	deleteAllChildren();
}

//==============================================================================
void MainComponent::resized ()
{
	int width = getWidth();
	int height = getHeight();
	
	sourceFilenameComponent->setBounds(5, 5, width-10, 20);
	
	playButton->setBounds(5, 30, 50, 20);
	pauseButton->setBounds(60, 30, 50, 20);

	int sliderWidth = (width / 2.0) - 10;
	verticalZoomSlider->setBounds(0.5*width+50, 30, sliderWidth-50, 20);
	horizontalZoomSlider->setBounds(120, 60, sliderWidth-120, 20);
	offsetSlider->setBounds(horizontalZoomSlider->getRight()+5, 60, sliderWidth, 20);
	
	thumbnail->setBounds(5, 90, width-10, height-95);
}

void MainComponent::paint (Graphics& g)
{
	g.setColour(Colours::green);
//	thumbnail->drawChannel(g, 5, 90, getWidth()-10, getHeight()-95,
//						   0, 1, 0, 1);
}
//==============================================================================
void MainComponent::buttonClicked(Button* button)
{
	if (button == playButton)
		filePlayer.startFromZero();

	else if (button == pauseButton)
		filePlayer.pause();
}	

void MainComponent::sliderValueChanged (Slider *changedSlider)
{
	if (changedSlider == verticalZoomSlider) {
		thumbnail->setVerticalZoomFactor(verticalZoomSlider->getValue());
	}
	else if (changedSlider == offsetSlider)
	{
		startTime = trackLength * 0.01 * offsetSlider->getValue();
		endTime = startTime + (trackLength * horizontalZoomSlider->getValue());
	}
	else if (changedSlider == horizontalZoomSlider);
		thumbnail->setHorizontalZoomFactor(horizontalZoomSlider->getValue());

	repaint();
}

void MainComponent::filenameComponentChanged (FilenameComponent* filenameComponent)
{
	if (filenameComponent == sourceFilenameComponent)
	{
		
		filePlayer.setFile(sourceFilenameComponent->getCurrentFile().getFullPathName());
		thumbnail->refresh();
		repaint();
	}
}

//==============================================================================
void MainComponent::audioDeviceIOCallback (const float** inputChannelData,
										   int totalNumInputChannels,
										   float** outputChannelData,
										   int totalNumOutputChannels,
										   int numSamples)
{
	audioSourcePlayer.audioDeviceIOCallback (inputChannelData, totalNumInputChannels, outputChannelData, totalNumOutputChannels, numSamples);
}

void MainComponent::audioDeviceAboutToStart (AudioIODevice* device)
{
	audioSourcePlayer.audioDeviceAboutToStart(device);
}

void MainComponent::audioDeviceStopped()
{
	audioSourcePlayer.audioDeviceStopped();
}
