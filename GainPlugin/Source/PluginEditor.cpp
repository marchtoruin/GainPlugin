/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h" // Include for BinaryData access

//==============================================================================
GainPluginAudioProcessorEditor::GainPluginAudioProcessorEditor (GainPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Set up the gain slider
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    gainSlider.setRange(0.0, 1.0, 0.01);
    gainSlider.setValue(0.5);
    gainSlider.setTextValueSuffix(" Gain");
    gainSlider.setDoubleClickReturnValue(true, 0.5);
    addAndMakeVisible(gainSlider);
    
    // Set up the gain label
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.attachToComponent(&gainSlider, false);
    addAndMakeVisible(gainLabel);
    
    // Set up the pan slider
    panSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    panSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    panSlider.setRange(-1.0, 1.0, 0.01);
    panSlider.setValue(0.0);
    panSlider.setTextValueSuffix(" Pan");
    panSlider.setDoubleClickReturnValue(true, 0.0);
    addAndMakeVisible(panSlider);
    
    // Set up the pan label
    panLabel.setText("Pan", juce::dontSendNotification);
    panLabel.setJustificationType(juce::Justification::centred);
    panLabel.attachToComponent(&panSlider, false);
    addAndMakeVisible(panLabel);
    
    // Set up the mono button
    monoButton.setButtonText("Mono");
    monoButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(monoButton);
    
    // Create the attachments to link the controls with the parameters
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "GAIN", gainSlider);
    
    panAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "PAN", panSlider);
    
    monoAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getParameters(), "MONO", monoButton);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    // Load the background image from BinaryData
    DBG("=== DEBUGGING BINARY DATA ===");
    
    // Try to load the image from BinaryData
    try
    {
        // Load directly from BinaryData using the exact name from BinaryData.h
        backgroundImage = juce::ImageCache::getFromMemory(BinaryData::GainPlugin_png, BinaryData::GainPlugin_pngSize);
        
        if (backgroundImage.isValid())
        {
            DBG("Successfully loaded image directly from BinaryData::GainPlugin_png");
            DBG("Image size: " + juce::String(backgroundImage.getWidth()) + "x" + juce::String(backgroundImage.getHeight()));
        }
        else
        {
            DBG("Failed to load valid image directly from BinaryData::GainPlugin_png");
            
            // Create a fallback gradient background
            backgroundImage = juce::Image(juce::Image::RGB, getWidth(), getHeight(), true);
            juce::Graphics g(backgroundImage);
            juce::ColourGradient gradient(juce::Colours::darkgrey, 0, 0, 
                                         juce::Colours::black, getWidth(), getHeight(), false);
            g.setGradientFill(gradient);
            g.fillAll();
            DBG("Created fallback gradient background");
        }
    }
    catch (const std::exception& e)
    {
        DBG("Exception while loading from BinaryData: " + juce::String(e.what()));
    }
}

GainPluginAudioProcessorEditor::~GainPluginAudioProcessorEditor()
{
}

//==============================================================================
void GainPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Draw the background image if it's valid
    if (backgroundImage.isValid())
    {
        DBG("Drawing valid background image");
        g.drawImageWithin(backgroundImage, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::centred);
    }
    else
    {
        // Fallback to solid color if image isn't valid
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    }

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("GainPlugin", 20, 20, getWidth() - 40, 30, juce::Justification::centred, 1);
}

void GainPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // Position the gain slider in the top part of the editor
    gainSlider.setBounds(getWidth() / 2 - 100, 70, 200, 100);
    
    // Position the pan slider below the gain slider
    panSlider.setBounds(getWidth() / 2 - 100, 190, 200, 60);
    
    // Position the mono button at the bottom
    monoButton.setBounds(getWidth() / 2 - 50, 260, 100, 30);
}
