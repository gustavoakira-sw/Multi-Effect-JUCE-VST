#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class AkiraMultiAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit AkiraMultiAudioProcessorEditor (AkiraMultiAudioProcessor&);
    ~AkiraMultiAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void configureSlider (juce::Slider& slider, juce::Label& label, const juce::String& labelText, const juce::String& suffix);

    AkiraMultiAudioProcessor& audioProcessor;

    juce::Slider gainSlider;
    juce::Slider bassSlider;
    juce::Slider midSlider;
    juce::Slider trebleSlider;
    juce::Slider reverbMixSlider;
    juce::Slider roomSizeSlider;
    juce::Slider dampingSlider;
    juce::Slider widthSlider;

    juce::Label titleLabel;
    juce::Label gainLabel;
    juce::Label bassLabel;
    juce::Label midLabel;
    juce::Label trebleLabel;
    juce::Label reverbMixLabel;
    juce::Label roomSizeLabel;
    juce::Label dampingLabel;
    juce::Label widthLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> gainAttachment;
    std::unique_ptr<SliderAttachment> bassAttachment;
    std::unique_ptr<SliderAttachment> midAttachment;
    std::unique_ptr<SliderAttachment> trebleAttachment;
    std::unique_ptr<SliderAttachment> reverbMixAttachment;
    std::unique_ptr<SliderAttachment> roomSizeAttachment;
    std::unique_ptr<SliderAttachment> dampingAttachment;
    std::unique_ptr<SliderAttachment> widthAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AkiraMultiAudioProcessorEditor)
};
