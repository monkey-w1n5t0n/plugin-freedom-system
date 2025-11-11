#include "PluginProcessor.h"
#include "PluginEditor.h"

TapeAgeAudioProcessor::TapeAgeAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

TapeAgeAudioProcessor::~TapeAgeAudioProcessor()
{
}

void TapeAgeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialization will be added in Stage 4
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void TapeAgeAudioProcessor::releaseResources()
{
    // Cleanup will be added in Stage 4
}

void TapeAgeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    // Pass-through for Stage 2 (DSP added in Stage 4)
    // Audio routing is already handled by JUCE
}

juce::AudioProcessorEditor* TapeAgeAudioProcessor::createEditor()
{
    return new TapeAgeAudioProcessorEditor(*this);
}

void TapeAgeAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // State management will be added in Stage 3
    juce::ignoreUnused(destData);
}

void TapeAgeAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // State management will be added in Stage 3
    juce::ignoreUnused(data, sizeInBytes);
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TapeAgeAudioProcessor();
}
