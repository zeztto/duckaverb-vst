#include "PluginEditor.h"
#include "PluginProcessor.h"

DUCKAVERBAudioProcessorEditor::DUCKAVERBAudioProcessorEditor(
    DUCKAVERBAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {

  // === SPACE KNOB (Single large rotary control) ===
  spaceSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  spaceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 28);
  spaceSlider.setColour(juce::Slider::rotarySliderFillColourId,
                        juce::Colour(0xff1e90ff)); // Brighter blue
  spaceSlider.setColour(juce::Slider::rotarySliderOutlineColourId,
                        juce::Colour(0xff333333)); // Dark outline
  spaceSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
  spaceSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
  spaceSlider.setColour(juce::Slider::textBoxBackgroundColourId,
                        juce::Colour(0x00000000)); // Transparent
  spaceSlider.setColour(juce::Slider::textBoxOutlineColourId,
                        juce::Colour(0x00000000)); // Remove border
  addAndMakeVisible(spaceSlider);

  spaceLabel.setText("SPACE", juce::dontSendNotification);
  spaceLabel.setJustificationType(juce::Justification::centred);
  spaceLabel.setColour(juce::Label::textColourId, juce::Colour(0xffcccccc));
  spaceLabel.setFont(juce::FontOptions(20.0f, juce::Font::bold));
  addAndMakeVisible(spaceLabel);

  spaceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
      audioProcessor.apvts, "space", spaceSlider);

  // === BYPASS FOOTSWITCH (FUZZA style) ===
  bypassButton.setButtonText("BYPASS");
  bypassButton.setClickingTogglesState(true);

  // FUZZA-style button colors
  bypassButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff333333));
  bypassButton.setColour(juce::TextButton::buttonOnColourId,
                         juce::Colour(0xffdd3333)); // Brighter red when on
  bypassButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffeeeeee));
  bypassButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);

  addAndMakeVisible(bypassButton);

  bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
      audioProcessor.apvts, "bypass", bypassButton);

  setSize(380, 440);
}

DUCKAVERBAudioProcessorEditor::~DUCKAVERBAudioProcessorEditor() {}

void DUCKAVERBAudioProcessorEditor::paint(juce::Graphics &g) {
  // Pedal enclosure background (dark grey with subtle gradient)
  auto bounds = getLocalBounds();
  juce::ColourGradient gradient(
      juce::Colour(0xff0f0f0f), bounds.getX(), bounds.getY(),
      juce::Colour(0xff1f1f1f), bounds.getX(), bounds.getBottom(),
      false);
  g.setGradientFill(gradient);
  g.fillAll();

  // Outer border (dark metallic)
  g.setColour(juce::Colour(0xff000000));
  g.drawRect(bounds, 4);

  // Inner border (lighter metallic highlight)
  g.setColour(juce::Colour(0xff444444));
  g.drawRect(bounds.reduced(4), 1);

  // Top logo area
  auto logoArea = bounds.removeFromTop(70);
  logoArea.removeFromTop(12); // Top padding
  g.setColour(juce::Colours::white);
  g.setFont(juce::FontOptions(38.0f, juce::Font::bold));
  g.drawFittedText("DUCKAVERB", logoArea, juce::Justification::centred, 1);

  // Subtitle (FUZZA style - orange)
  g.setFont(juce::FontOptions(13.0f, juce::Font::plain));
  g.setColour(juce::Colour(0xffff9933)); // Orange (matching FUZZA)
  g.drawFittedText("Ducking Reverb", logoArea.removeFromBottom(18),
                   juce::Justification::centred, 1);

  // Draw knob module box (single large box)
  auto knobBoxBounds = getLocalBounds().reduced(28);
  knobBoxBounds.removeFromTop(82); // Skip logo area
  knobBoxBounds.removeFromBottom(105); // Skip bypass area

  // Reduce box width for single knob
  knobBoxBounds = knobBoxBounds.reduced(60, 0);

  // Draw inset panel with shadow
  juce::Rectangle<float> box(knobBoxBounds.getX(), knobBoxBounds.getY(),
                             knobBoxBounds.getWidth(), knobBoxBounds.getHeight());

  // Dark inset shadow
  g.setColour(juce::Colour(0xff000000));
  g.fillRoundedRectangle(box, 12.0f);

  // Main panel background
  g.setColour(juce::Colour(0xff1a1a1a));
  g.fillRoundedRectangle(box.reduced(3), 10.0f);

  // Subtle border
  g.setColour(juce::Colour(0x44ffffff));
  g.drawRoundedRectangle(box.reduced(3), 10.0f, 1.5f);

  // Draw screws (pedal realism with better detail)
  auto drawScrew = [&g](float x, float y) {
    // Screw base
    g.setColour(juce::Colour(0xff666666));
    g.fillEllipse(x - 6, y - 6, 12, 12);

    // Screw highlight
    g.setColour(juce::Colour(0xffaaaaaa));
    g.fillEllipse(x - 5.5f, y - 5.5f, 6, 6);

    // Screw slot
    g.setColour(juce::Colour(0xff333333));
    g.drawLine(x - 4, y, x + 4, y, 2.0f);
  };

  drawScrew(20, 20);
  drawScrew(getWidth() - 20, 20);
  drawScrew(20, getHeight() - 20);
  drawScrew(getWidth() - 20, getHeight() - 20);

  // h4ppy Labs footer
  g.setFont(juce::FontOptions(10.0f));
  g.setColour(juce::Colour(0xff666666));
  auto footerArea = getLocalBounds().removeFromBottom(28);
  g.drawFittedText("h4ppy Labs", footerArea, juce::Justification::centred, 1);
}

void DUCKAVERBAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds().reduced(28);

  // Top: Logo area
  bounds.removeFromTop(70);
  bounds.removeFromTop(12); // Space after logo

  // Reserve bottom for footer (just text)
  bounds.removeFromBottom(32); // Footer text area

  // Reserve bottom for bypass button
  auto bypassArea = bounds.removeFromBottom(48);
  bounds.removeFromBottom(30); // Larger gap between SPACE box and BYPASS button

  // SPACE knob module - centered and compact (single knob)
  auto knobModuleArea = bounds;

  // Reduce vertical space for compact knob box
  knobModuleArea.removeFromTop(15);
  knobModuleArea.removeFromBottom(15);

  // Horizontal centering for single knob - reduce box width
  knobModuleArea = knobModuleArea.reduced(60, 0);

  spaceLabel.setBounds(knobModuleArea.removeFromTop(32));
  knobModuleArea.removeFromTop(5);

  // Center the knob with original padding (restore knob size)
  spaceSlider.setBounds(knobModuleArea.reduced(15, 10));

  // Bypass footswitch (centered at bottom, smaller size)
  bypassButton.setBounds(bypassArea.withSizeKeepingCentre(150, 45));
}
