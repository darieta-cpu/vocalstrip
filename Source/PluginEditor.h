#pragma once

#include "PluginProcessor.h"
#include <map>
#include <vector>

//==============================================================================
// 80s micro-rack look: black knobs with white pointer, square red LEDs, LED ladders
class StripLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider&) override;

    void drawLinearSlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           juce::Slider::SliderStyle, juce::Slider&) override;

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawButtonText (juce::Graphics&, juce::TextButton&,
                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

//==============================================================================
// LED ladder meter
class LevelMeter : public juce::Component
{
public:
    enum class Mode { level, gainReduction };

    explicit LevelMeter (Mode m, bool showTargetBand = false) : mode (m), showTarget (showTargetBand) {}

    void setValues (float peakDb, float rmsDb)
    {
        peak = peakDb;
        rms = rmsDb;
        repaint();
    }

    void paint (juce::Graphics&) override;

private:
    Mode mode;
    bool showTarget;
    float peak = -100.0f, rms = -100.0f;
};

//==============================================================================
class VocalStripEditor : public juce::AudioProcessorEditor,
                           private juce::Timer
{
public:
    explicit VocalStripEditor (VocalStripAudioProcessor&);
    ~VocalStripEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<APVTS::SliderAttachment> attachment;
    };

    struct Toggle
    {
        juce::TextButton button;
        std::unique_ptr<APVTS::ButtonAttachment> attachment;
    };

    void timerCallback() override;
    void addKnob (const juce::String& id, const juce::String& caption);
    void addToggle (const juce::String& id, const juce::String& text);
    void placeKnob (const juce::String& id, juce::Rectangle<int> cell);
    void placeToggle (const juce::String& id, juce::Rectangle<int> r);
    void setupLabel (juce::Label&, const juce::String& text, float size, juce::Justification j, bool bold = false);

    VocalStripAudioProcessor& proc;
    StripLookAndFeel laf;

    std::map<juce::String, std::unique_ptr<Knob>> knobs;
    std::map<juce::String, std::unique_ptr<Toggle>> toggles;

    LevelMeter inMeter, outMeter, grMeter, dsMeter;
    juce::Label peakLabel, rmsLabel, hintLabel, outPeakLabel, grCaption, outCaption, faderCaption;
    juce::Label trackLabel, dsGrCaption, driveNote;
    juce::String lastTrackName;

    juce::Slider outFader;
    std::unique_ptr<APVTS::SliderAttachment> faderAttachment;

    std::vector<std::pair<juce::String, juce::Rectangle<int>>> sections;

    float inHold = -100.0f, inPeakDisp = -100.0f, inRmsDisp = -100.0f;
    float outPeakDisp = -100.0f, outRmsDisp = -100.0f, outHold = -100.0f, grDisp = 0.0f, dsDisp = 0.0f;
    int inHoldFrames = 0, outHoldFrames = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalStripEditor)
};
