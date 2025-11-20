#include "PluginProcessor.h"
#include "PluginEditor.h"

DuckaverbAudioProcessor::DuckaverbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
#endif
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {
}

DuckaverbAudioProcessor::~DuckaverbAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
DuckaverbAudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  layout.add(std::make_unique<juce::AudioParameterFloat>("space", "Space", 0.0f,
                                                         1.0f, 0.5f));
  layout.add(
      std::make_unique<juce::AudioParameterBool>("bypass", "Bypass", false));

  return layout;
}

const juce::String DuckaverbAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool DuckaverbAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool DuckaverbAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool DuckaverbAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double DuckaverbAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int DuckaverbAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs, so this should be at least 1, even if you're not really
            // implementing programs.
}

int DuckaverbAudioProcessor::getCurrentProgram() { return 0; }

void DuckaverbAudioProcessor::setCurrentProgram(int index) {}

const juce::String DuckaverbAudioProcessor::getProgramName(int index) {
  return {};
}

void DuckaverbAudioProcessor::changeProgramName(int index,
                                                const juce::String &newName) {}

void DuckaverbAudioProcessor::prepareToPlay(double sampleRate,
                                            int samplesPerBlock) {
  reverb.setSampleRate(sampleRate);
  currentEnv = 0.0f;
}

void DuckaverbAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DuckaverbAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

  // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void DuckaverbAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                           juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // Bypass check
  if (apvts.getRawParameterValue("bypass")->load() > 0.5f)
    return;

  auto spaceValue = apvts.getRawParameterValue("space")->load();

  // 1. Update Reverb Parameters based on "Space"
  juce::Reverb::Parameters params;
  params.roomSize = 0.3f + (spaceValue * 0.6f); // 0.3 to 0.9
  params.damping = 0.5f - (spaceValue * 0.3f);  // 0.5 to 0.2
  params.wetLevel = spaceValue * 0.8f;          // 0.0 to 0.8
  params.dryLevel = 1.0f - (spaceValue * 0.2f); // 1.0 to 0.8
  params.width = 0.5f + (spaceValue * 0.5f);    // 0.5 to 1.0
  params.freezeMode = 0.0f;
  reverb.setParameters(params);

  // Ducking Parameters
  float threshold =
      0.5f - (spaceValue * 0.4f);           // Simple linear threshold mapping
  float ratio = 2.0f + (spaceValue * 4.0f); // 2:1 to 6:1
  float attack = 0.01f;
  float release = 0.1f;

  // Process
  // We need to process stereo, but Envelope is usually mono sum or max
  // JUCE Reverb processes stereo buffer in-place.

  // Copy input for sidechain (Dry signal)
  juce::AudioBuffer<float> sidechainBuffer;
  sidechainBuffer.makeCopyOf(buffer);

  // Apply Reverb (In-place on buffer)
  // juce::Reverb takes stereo float**
  if (totalNumInputChannels == 2) {
    float *left = buffer.getWritePointer(0);
    float *right = buffer.getWritePointer(1);
    reverb.processStereo(left, right, buffer.getNumSamples());
  } else if (totalNumInputChannels == 1) {
    float *mono = buffer.getWritePointer(0);
    reverb.processMono(mono, buffer.getNumSamples());
  }

  // Apply Ducking (Compressor on Wet signal, triggered by Sidechain)
  for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
    // 1. Detect Envelope from Sidechain (Dry)
    float inputLevel = 0.0f;
    if (totalNumInputChannels == 2) {
      inputLevel = std::max(std::abs(sidechainBuffer.getSample(0, sample)),
                            std::abs(sidechainBuffer.getSample(1, sample)));
    } else {
      inputLevel = std::abs(sidechainBuffer.getSample(0, sample));
    }

    // Simple Envelope Follower (Attack/Release)
    if (inputLevel > currentEnv)
      currentEnv += attack * (inputLevel - currentEnv);
    else
      currentEnv += release * (inputLevel - currentEnv);

    // 2. Calculate Gain Reduction
    float gainReduction = 1.0f;
    if (currentEnv > threshold) {
      // Simple hard knee compression
      float excess = currentEnv - threshold;
      gainReduction = 1.0f - (excess / ratio);
      if (gainReduction < 0.0f)
        gainReduction = 0.0f;
    }

    // 3. Apply Gain Reduction to Reverb Output (Wet)
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
      float *data = buffer.getWritePointer(channel);
      // Note: buffer currently contains the Reverb output (Wet + Dry mixed by
      // Reverb class) Wait, juce::Reverb mixes dry/wet internally. If we want
      // to duck ONLY the wet signal, we should have processed Reverb fully wet,
      // then ducked it, then mixed with dry.
      // But juce::Reverb doesn't easily allow separate wet/dry output unless we
      // set dry=0.

      // Let's adjust: Set Reverb params to 100% Wet, 0% Dry.
      // Then we mix Dry back in manually.

      // Actually, for "Ducking Reverb", usually the Reverb tail is ducked by
      // the Dry signal. So:
      // 1. Dry Signal -> Output
      // 2. Dry Signal -> Reverb (100% Wet) -> Ducking (controlled by Dry) ->
      // Output

      // I need to refactor the Reverb params above to be 100% Wet.
    }
  }

  // REFACTORING DSP LOOP FOR CORRECT DUCKING
  // 1. Get Dry Signal
  // 2. Process Reverb (Wet Only)
  // 3. Calculate Ducking Gain
  // 4. Apply Ducking to Wet
  // 5. Mix Dry + Wet

  // Reset Reverb Params for 100% Wet
  params.wetLevel = 1.0f; // We control mix level later
  params.dryLevel = 0.0f;
  reverb.setParameters(params);

  // Process Reverb on a separate buffer
  juce::AudioBuffer<float> wetBuffer;
  wetBuffer.makeCopyOf(buffer); // Copy Dry to Wet input

  if (totalNumInputChannels == 2) {
    reverb.processStereo(wetBuffer.getWritePointer(0),
                         wetBuffer.getWritePointer(1),
                         wetBuffer.getNumSamples());
  } else {
    reverb.processMono(wetBuffer.getWritePointer(0), wetBuffer.getNumSamples());
  }

  // Ducking & Mixing Loop
  float mixLevel = spaceValue * 0.8f; // Max wet mix

  for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
    // Envelope Detection (from Dry buffer)
    float dryLevel = 0.0f;
    if (totalNumInputChannels == 2) {
      dryLevel = std::max(std::abs(buffer.getSample(0, sample)),
                          std::abs(buffer.getSample(1, sample)));
    } else {
      dryLevel = std::abs(buffer.getSample(0, sample));
    }

    if (dryLevel > currentEnv)
      currentEnv += attack * (dryLevel - currentEnv);
    else
      currentEnv += release * (dryLevel - currentEnv);

    // Gain Reduction
    float gain = 1.0f;
    if (currentEnv > threshold) {
      gain = 1.0f - ((currentEnv - threshold) * (1.0f - (1.0f / ratio)));
      if (gain < 0.0f)
        gain = 0.0f;
    }

    // Apply to Wet and Mix
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
      float drySample = buffer.getSample(channel, sample);
      float wetSample = wetBuffer.getSample(channel, sample);

      float duckedWet = wetSample * gain * mixLevel;

      buffer.setSample(channel, sample, drySample + duckedWet);
    }
  }
}

bool DuckaverbAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *DuckaverbAudioProcessor::createEditor() {
  return new DuckaverbAudioProcessorEditor(*this);
}

void DuckaverbAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
  auto state = apvts.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void DuckaverbAudioProcessor::setStateInformation(const void *data,
                                                  int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));

  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(apvts.state.getType()))
      apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new DuckaverbAudioProcessor();
}
