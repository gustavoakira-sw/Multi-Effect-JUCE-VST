#include "PluginProcessor.h"
#include "PluginEditor.h"

AkiraMultiAudioProcessor::AkiraMultiAudioProcessor()
    : AudioProcessor (
        BusesProperties()
            .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
            .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
      ),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout AkiraMultiAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { "gain", 1 },
            "Gain",
            juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f),
            0.0f
        )
    );

    params.push_back (
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { "bass", 1 },
            "Bass",
            juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f),
            0.0f
        )
    );

    params.push_back (
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { "mid", 1 },
            "Mid",
            juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f),
            0.0f
        )
    );

    params.push_back (
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { "treble", 1 },
            "Treble",
            juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f),
            0.0f
        )
    );

    params.push_back (
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { "reverbMix", 1 },
            "Reverb Mix",
            juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
            0.0f
        )
    );

    params.push_back (
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { "reverbRoomSize", 1 },
            "Room Size",
            juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
            35.0f
        )
    );

    params.push_back (
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { "reverbDamping", 1 },
            "Damping",
            juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
            50.0f
        )
    );

    params.push_back (
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { "reverbWidth", 1 },
            "Width",
            juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
            100.0f
        )
    );

    return { params.begin(), params.end() };
}

const juce::String AkiraMultiAudioProcessor::getName() const
{
    return "Akira Multi";
}

bool AkiraMultiAudioProcessor::acceptsMidi() const
{
    return false;
}

bool AkiraMultiAudioProcessor::producesMidi() const
{
    return false;
}

bool AkiraMultiAudioProcessor::isMidiEffect() const
{
    return false;
}

double AkiraMultiAudioProcessor::getTailLengthSeconds() const
{
    return 2.0;
}

int AkiraMultiAudioProcessor::getNumPrograms()
{
    return 1;
}

int AkiraMultiAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AkiraMultiAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AkiraMultiAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AkiraMultiAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void AkiraMultiAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());

    lowShelfFilter.prepare (spec);
    midPeakFilter.prepare (spec);
    highShelfFilter.prepare (spec);

    lowShelfFilter.reset();
    midPeakFilter.reset();
    highShelfFilter.reset();

    reverb.prepare (spec);
    reverb.reset();

    updateEq();
    updateReverb();
}

void AkiraMultiAudioProcessor::releaseResources()
{
}

bool AkiraMultiAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& mainInput  = layouts.getMainInputChannelSet();
    const auto& mainOutput = layouts.getMainOutputChannelSet();

    if (mainInput != mainOutput)
        return false;

    return mainOutput == juce::AudioChannelSet::mono()
        || mainOutput == juce::AudioChannelSet::stereo();
}

void AkiraMultiAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;

    const auto gainDb = apvts.getRawParameterValue ("gain")->load();
    const auto gainLinear = juce::Decibels::decibelsToGain (gainDb);

    for (int channel = getTotalNumInputChannels(); channel < getTotalNumOutputChannels(); ++channel)
        buffer.clear (channel, 0, buffer.getNumSamples());

    for (int channel = 0; channel < getTotalNumInputChannels(); ++channel)
    {
        auto* samples = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            samples[sample] *= gainLinear;
        }
    }

    updateEq();
    updateReverb();

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);

    lowShelfFilter.process (context);
    midPeakFilter.process (context);
    highShelfFilter.process (context);
    reverb.process (context);
}

void AkiraMultiAudioProcessor::updateEq()
{
    const auto bassDb = apvts.getRawParameterValue ("bass")->load();
    const auto midDb = apvts.getRawParameterValue ("mid")->load();
    const auto trebleDb = apvts.getRawParameterValue ("treble")->load();

    *lowShelfFilter.state = *Coefficients::makeLowShelf (
        currentSampleRate,
        120.0,
        0.707f,
        juce::Decibels::decibelsToGain (bassDb)
    );

    *midPeakFilter.state = *Coefficients::makePeakFilter (
        currentSampleRate,
        1000.0,
        0.8f,
        juce::Decibels::decibelsToGain (midDb)
    );

    *highShelfFilter.state = *Coefficients::makeHighShelf (
        currentSampleRate,
        5000.0,
        0.707f,
        juce::Decibels::decibelsToGain (trebleDb)
    );
}

void AkiraMultiAudioProcessor::updateReverb()
{
    juce::Reverb::Parameters parameters;
    parameters.roomSize = apvts.getRawParameterValue ("reverbRoomSize")->load() / 100.0f;
    parameters.damping = apvts.getRawParameterValue ("reverbDamping")->load() / 100.0f;
    parameters.wetLevel = apvts.getRawParameterValue ("reverbMix")->load() / 100.0f;
    parameters.dryLevel = 1.0f - parameters.wetLevel;
    parameters.width = apvts.getRawParameterValue ("reverbWidth")->load() / 100.0f;
    parameters.freezeMode = 0.0f;

    reverb.setParameters (parameters);
}

bool AkiraMultiAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* AkiraMultiAudioProcessor::createEditor()
{
    return new AkiraMultiAudioProcessorEditor (*this);
}

void AkiraMultiAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());

    copyXmlToBinary (*xml, destData);
}

void AkiraMultiAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName (apvts.state.getType()))
        {
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AkiraMultiAudioProcessor();
}
