#pragma once
#include "PluginProcessor.h"

class DriveVerbAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit DriveVerbAudioProcessorEditor(DriveVerbAudioProcessor&);
    ~DriveVerbAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    DriveVerbAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DriveVerbAudioProcessorEditor)
};
