#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class DrumRouletteVoice : public juce::SynthesiserVoice
{
public:
    DrumRouletteVoice(int slotNumber);

    bool canPlaySound(juce::SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setCurrentPlaybackSampleRate(double newRate) override;

    void loadSample(const juce::File& file);
    int getSlotNumber() const { return slotNumber; }

    void setParameterPointers(std::atomic<float>* attack, std::atomic<float>* decay, std::atomic<float>* pitch);

private:
    int slotNumber;
    juce::AudioSampleBuffer sampleBuffer;
    double currentPosition = 0.0;
    float noteVelocity = 1.0f;
    float pitchRatio = 1.0f;
    bool isActive = false;

    // ADSR envelope (Phase 4.2)
    juce::ADSR envelope;

    // Parameter pointers (set by processor)
    std::atomic<float>* attackParam = nullptr;
    std::atomic<float>* decayParam = nullptr;
    std::atomic<float>* pitchParam = nullptr;

    JUCE_LEAK_DETECTOR(DrumRouletteVoice)
};

class DrumRouletteSound : public juce::SynthesiserSound
{
public:
    DrumRouletteSound(int midiNote) : triggerNote(midiNote) {}

    bool appliesToNote(int midiNoteNumber) override
    {
        return midiNoteNumber == triggerNote;
    }

    bool appliesToChannel(int) override { return true; }

private:
    int triggerNote;

    JUCE_LEAK_DETECTOR(DrumRouletteSound)
};
