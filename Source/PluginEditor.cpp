#include "PluginEditor.h"
#include "PluginProcessor.h"

// Custom LookAndFeel
class DuckaverbLookAndFeel : public juce::LookAndFeel_V4 {
public:
  DuckaverbLookAndFeel() {
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::white);
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::grey);
    setColour(juce::Slider::thumbColourId, juce::Colours::white);
  }

  void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                        float sliderPos, const float rotaryStartAngle,
                        const float rotaryEndAngle,
                        juce::Slider &slider) override {
    auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
    auto centreX = (float)x + (float)width * 0.5f;
    auto centreY = (float)y + (float)height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle =
        rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Background
    g.setColour(juce::Colours::darkgrey.darker(0.5f));
    g.fillEllipse(rx, ry, rw, rw);

    // Outline
    g.setColour(juce::Colours::grey);
    g.drawEllipse(rx, ry, rw, rw, 2.0f);

    // Indicator
    juce::Path p;
    auto pointerLength = radius * 0.8f;
    auto pointerThickness = 3.0f;
    p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness,
                   pointerLength);
    p.applyTransform(
        juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    g.setColour(juce::Colours::white);
    g.fillPath(p);

    // Glow effect (optional)
    g.setColour(juce::Colours::white.withAlpha(0.1f + sliderPos * 0.2f));
    g.fillEllipse(rx, ry, rw, rw);
  }
};

DuckaverbAudioProcessorEditor::DuckaverbAudioProcessorEditor(
    DuckaverbAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  static DuckaverbLookAndFeel lnf;
  setLookAndFeel(&lnf);

  // Space Slider
  spaceSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  spaceSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  addAndMakeVisible(spaceSlider);

  spaceAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts,
                                                       "space", spaceSlider);

  // Bypass Button
  bypassButton.setButtonText("Bypass");
  addAndMakeVisible(bypassButton);

  bypassAttachment = std::make_unique<ButtonAttachment>(audioProcessor.apvts,
                                                        "bypass", bypassButton);

  setSize(400, 400);
}

DuckaverbAudioProcessorEditor::~DuckaverbAudioProcessorEditor() {
  setLookAndFeel(nullptr);
}

void DuckaverbAudioProcessorEditor::paint(juce::Graphics &g) {
  // Background
  g.fillAll(juce::Colours::black);

  // Gradient Background
  juce::ColourGradient grad(juce::Colours::black, 0, 0,
                            juce::Colours::darkgrey.darker(0.8f), 0,
                            (float)getHeight(), false);
  g.setGradientFill(grad);
  g.fillAll();

  // Title
  g.setColour(juce::Colours::white);
  g.setFont(juce::Font(24.0f, juce::Font::bold));
  g.drawFittedText("Duckaverb", getLocalBounds().removeFromTop(50),
                   juce::Justification::centred, 1);

  // Footer
  g.setFont(12.0f);
  g.setColour(juce::Colours::grey);
  g.drawFittedText("h4ppy Labs", getLocalBounds().removeFromBottom(20),
                   juce::Justification::centred, 1);
}

void DuckaverbAudioProcessorEditor::resized() {
  auto area = getLocalBounds();
  auto header = area.removeFromTop(50);
  auto footer = area.removeFromBottom(40);

  // Center the knob
  spaceSlider.setBounds(area.reduced(40));

  // Bypass button at bottom right
  bypassButton.setBounds(footer.removeFromRight(80).reduced(5));
}
