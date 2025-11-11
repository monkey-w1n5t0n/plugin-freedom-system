#pragma once
#include "PluginProcessor.h"

class TapeAgeAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit TapeAgeAudioProcessorEditor(TapeAgeAudioProcessor&);
    ~TapeAgeAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    TapeAgeAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TapeAgeAudioProcessorEditor)
};
