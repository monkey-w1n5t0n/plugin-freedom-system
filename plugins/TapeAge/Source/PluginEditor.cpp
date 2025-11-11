#include "PluginEditor.h"

TapeAgeAudioProcessorEditor::TapeAgeAudioProcessorEditor(TapeAgeAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(600, 400);
}

TapeAgeAudioProcessorEditor::~TapeAgeAudioProcessorEditor()
{
}

void TapeAgeAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("TAPE AGE - Stage 2", getLocalBounds(), juce::Justification::centred, 1);
}

void TapeAgeAudioProcessorEditor::resized()
{
    // Layout will be added in Stage 5
}
