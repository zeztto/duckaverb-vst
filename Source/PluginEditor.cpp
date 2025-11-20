#include "PluginEditor.h"
#include "PluginProcessor.h"

DuckaverbAudioProcessorEditor::DuckaverbAudioProcessorEditor(
    DuckaverbAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {

  // === SPACE KNOB (Single large rotary control) ===
  spaceSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  spaceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 24);
  spaceSlider.setColour(juce::Slider::rotarySliderFillColourId,
                        juce::Colour(0xff00bfff)); // Blue (reverb theme)
  spaceSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
  spaceSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
  spaceSlider.setColour(juce::Slider::textBoxBackgroundColourId,
                        juce::Colour(0x00000000)); // Transparent
  spaceSlider.setColour(juce::Slider::textBoxOutlineColourId,
                        juce::Colour(0x00000000)); // Remove border
  addAndMakeVisible(spaceSlider);

  spaceLabel.setText("SPACE", juce::dontSendNotification);
  spaceLabel.setJustificationType(juce::Justification::centred);
  spaceLabel.setColour(juce::Label::textColourId, juce::Colours::white);
  spaceLabel.setFont(juce::FontOptions(22.0f, juce::Font::bold));
  addAndMakeVisible(spaceLabel);

  spaceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
      audioProcessor.apvts, "space", spaceSlider);

  // === BYPASS FOOTSWITCH ===
  bypassButton.setButtonText("BYPASS");
  bypassButton.setClickingTogglesState(true);
  bypassButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff333333));
  bypassButton.setColour(juce::TextButton::buttonOnColourId,
                         juce::Colour(0xffdd0000)); // Red when on
  bypassButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
  bypassButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
  addAndMakeVisible(bypassButton);

  bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
      audioProcessor.apvts, "bypass", bypassButton);

  setSize(380, 440);
}

DuckaverbAudioProcessorEditor::~DuckaverbAudioProcessorEditor() {}

void DuckaverbAudioProcessorEditor::paint(juce::Graphics &g) {
  // Pedal enclosure background (dark grey with gradient)
  auto bounds = getLocalBounds();
  juce::ColourGradient gradient(
      juce::Colour(0xff1a1a1a), bounds.getX(), bounds.getY(),
      juce::Colour(0xff2d2d2d), bounds.getX(), bounds.getBottom(),
      false);
  g.setGradientFill(gradient);
  g.fillAll();

  // Border (metallic look)
  g.setColour(juce::Colour(0xff555555));
  g.drawRect(bounds, 3);

  // Top logo area
  auto logoArea = bounds.removeFromTop(70);
  logoArea.removeFromTop(8); // Top padding
  g.setColour(juce::Colours::white);
  g.setFont(juce::FontOptions(48.0f, juce::Font::bold));
  g.drawFittedText("DUCKAVERB", logoArea, juce::Justification::centred, 1);

  // Subtitle (Korean name)
  g.setFont(juce::FontOptions(14.0f));
  g.setColour(juce::Colour(0xff00bfff)); // Blue
  g.drawFittedText("덕커버브", logoArea.removeFromBottom(16),
                   juce::Justification::centred, 1);

  // Draw knob module box (single large box)
  auto knobBoxBounds = getLocalBounds().reduced(30);
  knobBoxBounds.removeFromTop(85); // Skip logo area
  knobBoxBounds.removeFromBottom(80); // Skip bypass area

  // Draw subtle box with rounded corners
  juce::Rectangle<float> box(knobBoxBounds.getX(), knobBoxBounds.getY(),
                             knobBoxBounds.getWidth(), knobBoxBounds.getHeight());
  g.setColour(juce::Colour(0x33ffffff)); // Semi-transparent white
  g.drawRoundedRectangle(box, 10.0f, 2.0f);

  // Subtle inner glow
  g.setColour(juce::Colour(0x11ffffff));
  g.fillRoundedRectangle(box.reduced(2), 9.0f);

  // Draw screws (pedal realism)
  auto drawScrew = [&g](int x, int y) {
    g.setColour(juce::Colour(0xff888888));
    g.fillEllipse(x - 5, y - 5, 10, 10);
    g.setColour(juce::Colour(0xff444444));
    g.drawLine(x - 4, y, x + 4, y, 1.5f);
  };

  drawScrew(18, 18);
  drawScrew(getWidth() - 18, 18);
  drawScrew(18, getHeight() - 18);
  drawScrew(getWidth() - 18, getHeight() - 18);

  // h4ppy Labs footer
  g.setFont(juce::FontOptions(10.0f));
  g.setColour(juce::Colour(0xff666666));
  auto footerArea = getLocalBounds().removeFromBottom(20);
  g.drawFittedText("h4ppy Labs", footerArea, juce::Justification::centred, 1);
}

void DuckaverbAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds().reduced(30);

  // Top: Logo area
  bounds.removeFromTop(70);
  bounds.removeFromTop(15); // Space after logo

  // Reserve bottom for bypass button and footer
  auto footerHeight = 80; // bypass button + footer
  auto footer = bounds.removeFromBottom(footerHeight);

  // SPACE knob module (takes remaining space)
  auto knobModuleArea = bounds;

  spaceLabel.setBounds(knobModuleArea.removeFromTop(30));
  knobModuleArea.removeFromTop(8);

  // Center the knob with padding
  spaceSlider.setBounds(knobModuleArea.reduced(20, 5));

  // Bypass footswitch (at bottom, above footer)
  footer.removeFromBottom(20); // Reserve for footer text
  auto bypassArea = footer.removeFromTop(55);
  bypassButton.setBounds(bypassArea.withSizeKeepingCentre(160, 48));
}
