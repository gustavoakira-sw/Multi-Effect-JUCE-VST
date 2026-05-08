#pragma once

#include <JuceHeader.h>

class AkiraMultiAudioProcessor : public juce::AudioProcessor
{
public:
    AkiraMultiAudioProcessor();
    ~AkiraMultiAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    using Filter = juce::dsp::IIR::Filter<float>;
    using Coefficients = juce::dsp::IIR::Coefficients<float>;

    void updateEq();
    void updateReverb();

    juce::dsp::ProcessorDuplicator<Filter, Coefficients> lowShelfFilter;
    juce::dsp::ProcessorDuplicator<Filter, Coefficients> midPeakFilter;
    juce::dsp::ProcessorDuplicator<Filter, Coefficients> highShelfFilter;
    juce::dsp::Reverb reverb;

    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AkiraMultiAudioProcessor)
};
