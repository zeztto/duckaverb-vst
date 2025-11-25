#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

class DUCKAVERBAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  DUCKAVERBAudioProcessorEditor(DUCKAVERBAudioProcessor &);
  ~DUCKAVERBAudioProcessorEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  DUCKAVERBAudioProcessor &audioProcessor;

  // Main control
  juce::Slider spaceSlider;
  juce::Label spaceLabel;

  // Bypass footswitch button (pedal style)
  juce::TextButton bypassButton;

  using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
  using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

  std::unique_ptr<SliderAttachment> spaceAttachment;
  std::unique_ptr<ButtonAttachment> bypassAttachment;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DUCKAVERBAudioProcessorEditor)
};
