#include "PluginProcessor.h"
#include "PluginEditor.h"

DUCKAVERBAudioProcessor::DUCKAVERBAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
#endif
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {
}

DUCKAVERBAudioProcessor::~DUCKAVERBAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
DUCKAVERBAudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  layout.add(std::make_unique<juce::AudioParameterFloat>("space", "Space", 0.0f,
                                                         1.0f, 0.5f));
  layout.add(
      std::make_unique<juce::AudioParameterBool>("bypass", "Bypass", false));

  return layout;
}

const juce::String DUCKAVERBAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool DUCKAVERBAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool DUCKAVERBAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool DUCKAVERBAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double DUCKAVERBAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int DUCKAVERBAudioProcessor::getNumPrograms() {
  return 1; // NB: some hosts don't cope very well if you tell them there are 0
            // programs, so this should be at least 1, even if you're not really
            // implementing programs.
}

int DUCKAVERBAudioProcessor::getCurrentProgram() { return 0; }

void DUCKAVERBAudioProcessor::setCurrentProgram(int index) {}

const juce::String DUCKAVERBAudioProcessor::getProgramName(int index) {
  return {};
}

void DUCKAVERBAudioProcessor::changeProgramName(int index,
                                                const juce::String &newName) {}

void DUCKAVERBAudioProcessor::prepareToPlay(double sampleRate,
                                            int samplesPerBlock) {
  reverb.setSampleRate(sampleRate);
  currentEnv = 0.0f;
}

void DUCKAVERBAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DUCKAVERBAudioProcessor::isBusesLayoutSupported(
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

void DUCKAVERBAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
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

  // === DUCKING REVERB DSP ALGORITHM ===
  // 1. Save dry signal
  // 2. Process reverb (100% wet)
  // 3. Duck reverb based on dry signal envelope
  // 4. Mix dry + ducked wet with volume compensation

  // Copy dry signal for sidechain detection and later mixing
  juce::AudioBuffer<float> dryBuffer;
  dryBuffer.makeCopyOf(buffer);

  // Configure reverb parameters based on space (0.0 to 1.0)
  juce::Reverb::Parameters params;
  params.roomSize = 0.3f + (spaceValue * 0.6f);  // 0.3 to 0.9
  params.damping = 0.5f - (spaceValue * 0.3f);   // 0.5 to 0.2 (less damping = more tail)
  params.width = 0.5f + (spaceValue * 0.5f);     // 0.5 to 1.0 (stereo width)
  params.wetLevel = 1.0f;  // 100% wet - we control mix manually
  params.dryLevel = 0.0f;  // 0% dry - we mix it back ourselves
  params.freezeMode = 0.0f;
  reverb.setParameters(params);

  // Process reverb (buffer now contains 100% wet signal)
  if (totalNumInputChannels == 2) {
    reverb.processStereo(buffer.getWritePointer(0),
                         buffer.getWritePointer(1),
                         buffer.getNumSamples());
  } else if (totalNumInputChannels == 1) {
    reverb.processMono(buffer.getWritePointer(0), buffer.getNumSamples());
  }

  // Ducking parameters
  float threshold = 0.02f + (spaceValue * 0.08f);  // 0.02 to 0.10 (higher space = easier to trigger)
  float ratio = 3.0f + (spaceValue * 5.0f);        // 3:1 to 8:1 (more aggressive at high space)
  float attack = 0.001f;   // Very fast attack (1ms) for immediate ducking
  float release = 0.05f + (spaceValue * 0.15f);    // 50ms to 200ms (slower release at high space)

  // Wet mix amount based on space
  float wetMix = spaceValue * 0.7f;  // 0.0 to 0.7 (max 70% wet)

  // Volume compensation: space=0 -> 1.0x, space=1 -> 1.5x (+3dB)
  float volumeCompensation = 1.0f + (spaceValue * 0.5f);

  // Process each sample
  for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
    // Detect envelope from dry signal (sidechain)
    float inputLevel = 0.0f;
    if (totalNumInputChannels == 2) {
      inputLevel = std::max(std::abs(dryBuffer.getSample(0, sample)),
                           std::abs(dryBuffer.getSample(1, sample)));
    } else {
      inputLevel = std::abs(dryBuffer.getSample(0, sample));
    }

    // Envelope follower with attack/release
    if (inputLevel > currentEnv) {
      currentEnv += attack * (inputLevel - currentEnv);
    } else {
      currentEnv += release * (inputLevel - currentEnv);
    }

    // Calculate ducking gain (how much to reduce reverb)
    float duckingGain = 1.0f;
    if (currentEnv > threshold) {
      // When playing: reduce reverb based on ratio
      // Calculate how much we exceed threshold (0.0 to 1.0+)
      float excessAmount = (currentEnv - threshold) / threshold;
      // CRITICAL FIX: Clamp to 1.0 to prevent over-reduction
      excessAmount = juce::jmin(excessAmount, 1.0f);

      // Interpolate between full reverb (1.0) and reduced reverb (1/ratio)
      // ratio=3 -> min gain=0.33, ratio=8 -> min gain=0.125
      float minGain = 1.0f / ratio;
      duckingGain = 1.0f - (excessAmount * (1.0f - minGain));

      // Safety clamp: prevent complete silence, keep at least 1% for naturalness
      duckingGain = juce::jmax(0.01f, juce::jmin(1.0f, duckingGain));
    }

    // Mix dry + ducked wet for each channel
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
      float drySample = dryBuffer.getSample(channel, sample);
      float wetSample = buffer.getSample(channel, sample);

      // Apply ducking to wet signal
      float duckedWet = wetSample * duckingGain * wetMix;

      // Mix dry + wet with volume compensation
      float output = (drySample + duckedWet) * volumeCompensation;

      buffer.setSample(channel, sample, output);
    }
  }
}

bool DUCKAVERBAudioProcessor::hasEditor() const {
  return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *DUCKAVERBAudioProcessor::createEditor() {
  return new DUCKAVERBAudioProcessorEditor(*this);
}

void DUCKAVERBAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
  auto state = apvts.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void DUCKAVERBAudioProcessor::setStateInformation(const void *data,
                                                  int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));

  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(apvts.state.getType()))
      apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new DUCKAVERBAudioProcessor();
}
