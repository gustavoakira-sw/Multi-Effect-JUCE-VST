#include "PluginEditor.h"

AkiraMultiAudioProcessorEditor::AkiraMultiAudioProcessorEditor (AkiraMultiAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (620, 420);

    titleLabel.setText ("Akira Multi", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setFont (juce::FontOptions (26.0f, juce::Font::bold));
    addAndMakeVisible (titleLabel);

    configureSlider (gainSlider, gainLabel, "Gain", " dB");
    configureSlider (bassSlider, bassLabel, "Bass", " dB");
    configureSlider (midSlider, midLabel, "Mid", " dB");
    configureSlider (trebleSlider, trebleLabel, "Treble", " dB");
    configureSlider (reverbMixSlider, reverbMixLabel, "Mix", " %");
    configureSlider (roomSizeSlider, roomSizeLabel, "Room", " %");
    configureSlider (dampingSlider, dampingLabel, "Damp", " %");
    configureSlider (widthSlider, widthLabel, "Width", " %");

    gainAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.apvts,
        "gain",
        gainSlider
    );

    bassAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.apvts,
        "bass",
        bassSlider
    );

    midAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.apvts,
        "mid",
        midSlider
    );

    trebleAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.apvts,
        "treble",
        trebleSlider
    );

    reverbMixAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.apvts,
        "reverbMix",
        reverbMixSlider
    );

    roomSizeAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.apvts,
        "reverbRoomSize",
        roomSizeSlider
    );

    dampingAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.apvts,
        "reverbDamping",
        dampingSlider
    );

    widthAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.apvts,
        "reverbWidth",
        widthSlider
    );
}

void AkiraMultiAudioProcessorEditor::configureSlider (juce::Slider& slider,
                                                      juce::Label& label,
                                                      const juce::String& labelText,
                                                      const juce::String& suffix)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 78, 22);
    slider.setTextValueSuffix (suffix);
    slider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB (229, 111, 82));
    slider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour::fromRGB (74, 78, 88));
    slider.setColour (juce::Slider::thumbColourId, juce::Colour::fromRGB (245, 216, 150));
    slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour::fromRGB (234, 234, 238));
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (slider);

    label.setText (labelText, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, juce::Colour::fromRGB (220, 220, 230));
    addAndMakeVisible (label);
}

void AkiraMultiAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (24, 24, 28));

    auto bounds = getLocalBounds().reduced (12).toFloat();
    g.setColour (juce::Colour::fromRGB (220, 220, 230));
    g.drawRoundedRectangle (bounds, 8.0f, 2.0f);

    g.setColour (juce::Colour::fromRGB (42, 43, 50));
    g.fillRoundedRectangle (juce::Rectangle<float> (24.0f, 82.0f, 572.0f, 134.0f), 8.0f);
    g.fillRoundedRectangle (juce::Rectangle<float> (24.0f, 246.0f, 572.0f, 134.0f), 8.0f);

    g.setColour (juce::Colour::fromRGB (174, 178, 190));
    g.setFont (juce::FontOptions (15.0f, juce::Font::bold));
    g.drawText ("EQ", 36, 92, 120, 24, juce::Justification::left);
    g.drawText ("Reverb", 36, 256, 120, 24, juce::Justification::left);
}

void AkiraMultiAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);

    titleLabel.setBounds (area.removeFromTop (50));

    area.removeFromTop (22);

    auto setKnobBounds = [] (juce::Slider& slider, juce::Label& label, juce::Rectangle<int> bounds)
    {
        label.setBounds (bounds.removeFromTop (22));
        slider.setBounds (bounds);
    };

    auto eqArea = area.removeFromTop (140).reduced (18, 12);
    eqArea.removeFromTop (18);

    auto gainArea = eqArea.removeFromLeft (132);
    auto bassArea = eqArea.removeFromLeft (132);
    auto midArea = eqArea.removeFromLeft (132);
    auto trebleArea = eqArea.removeFromLeft (132);

    setKnobBounds (gainSlider, gainLabel, gainArea);
    setKnobBounds (bassSlider, bassLabel, bassArea);
    setKnobBounds (midSlider, midLabel, midArea);
    setKnobBounds (trebleSlider, trebleLabel, trebleArea);

    area.removeFromTop (24);

    auto reverbArea = area.removeFromTop (140).reduced (18, 12);
    reverbArea.removeFromTop (18);

    auto mixArea = reverbArea.removeFromLeft (132);
    auto roomArea = reverbArea.removeFromLeft (132);
    auto dampingArea = reverbArea.removeFromLeft (132);
    auto widthArea = reverbArea.removeFromLeft (132);

    setKnobBounds (reverbMixSlider, reverbMixLabel, mixArea);
    setKnobBounds (roomSizeSlider, roomSizeLabel, roomArea);
    setKnobBounds (dampingSlider, dampingLabel, dampingArea);
    setKnobBounds (widthSlider, widthLabel, widthArea);
}
