/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayTutorialAudioProcessor::DelayTutorialAudioProcessor() :
    AudioProcessor(
        BusesProperties()
         .withInput("Input", juce::AudioChannelSet::stereo(), true)
         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    )
{
    auto* param = apvts.getParameter(gainParamID.getParamID());
    gainParam = dynamic_cast<juce::AudioParameterFloat*>(param);
}

DelayTutorialAudioProcessor::~DelayTutorialAudioProcessor()
{
}

//==============================================================================
const juce::String DelayTutorialAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayTutorialAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DelayTutorialAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DelayTutorialAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DelayTutorialAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayTutorialAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DelayTutorialAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayTutorialAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DelayTutorialAudioProcessor::getProgramName (int index)
{
    return {};
}

void DelayTutorialAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DelayTutorialAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void DelayTutorialAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayTutorialAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

void DelayTutorialAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this clears any channels that didn't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }

    float gainInDecibels = gainParam->get();
    float gain = juce::Decibels::decibelsToGain(gainInDecibels);
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            channelData[sample] *= gain;
        }
    }
    
}

//==============================================================================
bool DelayTutorialAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DelayTutorialAudioProcessor::createEditor()
{
    return new DelayTutorialAudioProcessorEditor (*this);
}

//==============================================================================
void DelayTutorialAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayTutorialAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayTutorialAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout DelayTutorialAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(getOutputGainParameter());
    
    return layout;
}

std::unique_ptr<juce::AudioParameterFloat> DelayTutorialAudioProcessor::getOutputGainParameter() const
{
    return std::make_unique<juce::AudioParameterFloat>(
        gainParamID,
        "Output Gain",
        juce::NormalisableRange<float> {-12.0f, 12.0f},
        0.0f
    );
}
