/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout GainPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "GAIN",                 // parameterID
        "Gain",                 // parameter name
        0.0f,                   // minimum value
        1.0f,                   // maximum value
        0.5f                    // default value
    ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "PAN",                  // parameterID
        "Pan",                  // parameter name
        -1.0f,                  // minimum value (-1 = full left)
        1.0f,                   // maximum value (1 = full right)
        0.0f                    // default value (center)
    ));
    
    layout.add(std::make_unique<juce::AudioParameterBool>(
        "MONO",                 // parameterID
        "Mono",                 // parameter name
        false                   // default value (stereo)
    ));

    return layout;
}

//==============================================================================
GainPluginAudioProcessor::GainPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

GainPluginAudioProcessor::~GainPluginAudioProcessor()
{
}

//==============================================================================
const juce::String GainPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GainPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GainPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GainPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GainPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GainPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GainPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GainPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String GainPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void GainPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void GainPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void GainPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GainPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GainPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);
    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, clear extra outputs
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Get parameter values
    float gainValue = apvts.getRawParameterValue("GAIN")->load();
    float panValue = apvts.getRawParameterValue("PAN")->load();
    bool monoOn = apvts.getRawParameterValue("MONO")->load() > 0.5f;
    
    // Calculate pan values for left and right (equal power panning)
    const float piOver2 = juce::MathConstants<float>::pi / 2.0f;
    float panLeftGain = std::cos((panValue + 1.0f) * piOver2 * 0.5f);
    float panRightGain = std::sin((panValue + 1.0f) * piOver2 * 0.5f);
    
    // Apply mono summing if enabled
    if (monoOn && totalNumInputChannels >= 2)
    {
        // Sum to mono
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float monoSample = 0.0f;
            
            // Sum all channels
            for (int chan = 0; chan < totalNumInputChannels; ++chan)
            {
                monoSample += buffer.getSample(chan, sample);
            }
            
            // Divide by number of channels to maintain original level
            monoSample /= static_cast<float>(totalNumInputChannels);
            
            // Apply mono sample to all channels
            for (int chan = 0; chan < totalNumInputChannels; ++chan)
            {
                buffer.setSample(chan, sample, monoSample);
            }
        }
    }
    
    // Apply gain and panning
    if (totalNumInputChannels >= 2)
    {
        // Stereo processing
        auto* leftChannel = buffer.getWritePointer(0);
        auto* rightChannel = buffer.getWritePointer(1);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            leftChannel[sample] = leftChannel[sample] * gainValue * panLeftGain;
            rightChannel[sample] = rightChannel[sample] * gainValue * panRightGain;
        }
    }
    else
    {
        // Mono input - just apply gain
        auto* channel = buffer.getWritePointer(0);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channel[sample] *= gainValue;
        }
    }
}

//==============================================================================
bool GainPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GainPluginAudioProcessor::createEditor()
{
    return new GainPluginAudioProcessorEditor (*this);
}

//==============================================================================
void GainPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused(destData);
}

void GainPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused(data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainPluginAudioProcessor();
}
