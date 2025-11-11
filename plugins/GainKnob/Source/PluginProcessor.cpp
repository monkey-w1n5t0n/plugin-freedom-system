#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout GainKnobAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // GAIN - Float parameter (-60.0 to 0.0 dB)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "GAIN", 1 },
        "Gain",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f, 1.0f),
        0.0f,
        "dB"
    ));

    // PAN - Float parameter (-100.0 to 100.0, center at 0.0)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "PAN", 1 },
        "Pan",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f, 1.0f),
        0.0f,
        "%"
    ));

    return layout;
}

GainKnobAudioProcessor::GainKnobAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
}

GainKnobAudioProcessor::~GainKnobAudioProcessor()
{
}

void GainKnobAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialization will be added in Stage 4
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void GainKnobAudioProcessor::releaseResources()
{
    // Cleanup will be added in Stage 4
}

void GainKnobAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    // Read GAIN parameter (atomic read, real-time safe)
    auto* gainParam = parameters.getRawParameterValue("GAIN");
    float gainDb = gainParam->load();

    // Read PAN parameter (atomic read, real-time safe)
    auto* panParam = parameters.getRawParameterValue("PAN");
    float panPercent = panParam->load();

    // Convert dB to linear gain multiplier
    float gainLinear;

    if (gainDb <= -59.9f) {
        // Special case: treat near-minimum as complete silence
        // This avoids floating-point denormals and ensures true silence at minimum
        gainLinear = 0.0f;
    } else {
        // Standard dB to linear conversion: gain = 10^(dB/20)
        gainLinear = juce::Decibels::decibelsToGain(gainDb);
    }

    // Calculate pan coefficients using constant power panning
    // Pan range: -100 (full left) to +100 (full right)
    // At center (0), both channels are at 0.707 (-3dB) for equal power
    float panNormalized = panPercent / 100.0f; // Convert to -1.0 to +1.0
    float panRadians = (panNormalized * 0.25f + 0.25f) * juce::MathConstants<float>::pi;

    float leftGain = std::cos(panRadians) * gainLinear;
    float rightGain = std::sin(panRadians) * gainLinear;

    // Apply gain and pan to stereo channels
    int numChannels = buffer.getNumChannels();

    if (numChannels >= 1)
        buffer.applyGain(0, 0, buffer.getNumSamples(), leftGain);

    if (numChannels >= 2)
        buffer.applyGain(1, 0, buffer.getNumSamples(), rightGain);
}

juce::AudioProcessorEditor* GainKnobAudioProcessor::createEditor()
{
    return new GainKnobAudioProcessorEditor(*this);
}

void GainKnobAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GainKnobAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainKnobAudioProcessor();
}
