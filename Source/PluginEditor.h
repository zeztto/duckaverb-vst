#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

class DuckaverbAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  DuckaverbAudioProcessorEditor(DuckaverbAudioProcessor &);
  ~DuckaverbAudioProcessorEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  DuckaverbAudioProcessor &audioProcessor;

  // Main control
  juce::Slider spaceSlider;
  juce::Label spaceLabel;

  // Bypass footswitch button (pedal style)
  juce::TextButton bypassButton;

  using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
  using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

  std::unique_ptr<SliderAttachment> spaceAttachment;
  std::unique_ptr<ButtonAttachment> bypassAttachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DuckaverbAudioProcessorEditor)
};
