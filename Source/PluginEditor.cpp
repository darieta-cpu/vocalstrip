#include "PluginEditor.h"

namespace
{
    const juce::Colour kText   (0xffe6e6e6);
    const juce::Colour kDim    (0xff9a9da5);
    const juce::Colour kAccent (0xffff6a2b);   // orange stripe / name
    const juce::Colour kAmber  (0xffe8a317);
    const juce::Colour kLedOn  (0xffff3b2a);
    const juce::Colour kLedOff (0xff3a1210);
    const juce::Colour kLcd    (0xff7dff8a);
}

//==============================================================================
void StripLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float, float startAngle, float endAngle, juce::Slider& slider)
{
    const float pos = (float) slider.valueToProportionOfLength (slider.getValue());

    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (3.0f);
    const float d = juce::jmin (bounds.getWidth(), bounds.getHeight());
    const auto centre = bounds.getCentre();
    const float R = d * 0.5f;
    const float angle = startAngle + pos * (endAngle - startAngle);

    // scale ticks (white silkscreen)
    for (int i = 0; i <= 10; ++i)
    {
        const float a = startAngle + (endAngle - startAngle) * (float) i / 10.0f;
        const float s = std::sin (a), c = std::cos (a);
        const bool major = (i % 5 == 0);
        const float r1 = R * 0.84f;
        const float r2 = R * (major ? 1.0f : 0.93f);
        g.setColour (juce::Colours::white.withAlpha (0.8f));
        g.drawLine (centre.x + r1 * s, centre.y - r1 * c, centre.x + r2 * s, centre.y - r2 * c, major ? 1.6f : 1.0f);
    }

    // knob body
    const float bodyR = R * 0.70f;
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.fillEllipse (centre.x - bodyR, centre.y - bodyR + 2.0f, bodyR * 2.0f, bodyR * 2.0f);

    juce::ColourGradient grad (juce::Colour (0xff505053), centre.x - bodyR * 0.5f, centre.y - bodyR * 0.6f,
                               juce::Colour (0xff0b0b0c), centre.x + bodyR * 0.6f, centre.y + bodyR * 0.7f, false);
    g.setGradientFill (grad);
    g.fillEllipse (centre.x - bodyR, centre.y - bodyR, bodyR * 2.0f, bodyR * 2.0f);

    g.setColour (juce::Colour (0xff6c6e74));
    g.drawEllipse (centre.x - bodyR, centre.y - bodyR, bodyR * 2.0f, bodyR * 2.0f, 1.2f);

    // inner cap
    const float capR = bodyR * 0.72f;
    g.setColour (juce::Colour (0xff161618));
    g.fillEllipse (centre.x - capR, centre.y - capR, capR * 2.0f, capR * 2.0f);

    // white pointer line
    juce::Path pointer;
    pointer.addRoundedRectangle (-1.3f, -bodyR * 0.95f, 2.6f, bodyR * 0.75f, 1.0f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));
    g.setColour (juce::Colour (0xfff4f4f4));
    g.fillPath (pointer);
}

void StripLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float minSliderPos, float maxSliderPos,
                                         juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (style != juce::Slider::LinearVertical)
    {
        LookAndFeel_V4::drawLinearSlider (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        return;
    }

    const float cx = (float) x + (float) width * 0.5f;

    // slot
    juce::Rectangle<float> slot (cx - 2.5f, (float) y + 4.0f, 5.0f, (float) height - 8.0f);
    g.setColour (juce::Colour (0xff050506));
    g.fillRoundedRectangle (slot, 2.0f);
    g.setColour (juce::Colour (0xff6a6c72));
    g.drawRoundedRectangle (slot, 2.0f, 1.0f);

    // scale ticks
    for (int i = 0; i <= 6; ++i)
    {
        const float ty = maxSliderPos + (minSliderPos - maxSliderPos) * (float) i / 6.0f;
        g.setColour (juce::Colours::white.withAlpha (0.7f));
        g.drawHorizontalLine ((int) ty, cx - 16.0f, cx - 9.0f);
        g.drawHorizontalLine ((int) ty, cx + 9.0f, cx + 16.0f);
    }

    // 0 dB mark
    const double range = slider.getMaximum() - slider.getMinimum();
    if (range > 0.0 && slider.getMaximum() > 0.0 && slider.getMinimum() < 0.0)
    {
        const float frac = (float) (slider.getMaximum() / range);
        const float y0 = maxSliderPos + (minSliderPos - maxSliderPos) * frac;
        g.setColour (kAmber);
        g.fillRect (cx - 19.0f, y0 - 1.0f, 10.0f, 2.0f);
        g.fillRect (cx + 9.0f, y0 - 1.0f, 10.0f, 2.0f);
    }

    // fader cap
    juce::Rectangle<float> cap (cx - 15.0f, sliderPos - 9.0f, 30.0f, 18.0f);
    juce::ColourGradient grad (juce::Colour (0xff55565a), cap.getX(), cap.getY(),
                               juce::Colour (0xff1a1a1c), cap.getX(), cap.getBottom(), false);
    g.setGradientFill (grad);
    g.fillRoundedRectangle (cap, 2.0f);
    g.setColour (juce::Colour (0xff8a8c92));
    g.drawRoundedRectangle (cap, 2.0f, 1.0f);
    g.setColour (juce::Colour (0xfff4f4f4));
    g.fillRect (cap.getX() + 3.0f, cap.getCentreY() - 1.0f, cap.getWidth() - 6.0f, 2.0f);
}

void StripLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&,
                                             bool highlighted, bool down)
{
    auto b = button.getLocalBounds().toFloat().reduced (0.5f);
    const bool on = button.getToggleState();

    g.setColour (juce::Colour (highlighted ? 0xff46474c : 0xff3b3c40));
    g.fillRoundedRectangle (b, 3.0f);
    if (down)
    {
        g.setColour (juce::Colours::black.withAlpha (0.25f));
        g.fillRoundedRectangle (b, 3.0f);
    }
    g.setColour (juce::Colour (0xff6a6c72));
    g.drawRoundedRectangle (b, 3.0f, 1.0f);

    // square LED
    juce::Rectangle<float> led (b.getX() + 6.0f, b.getCentreY() - 4.0f, 8.0f, 8.0f);
    if (on)
    {
        g.setColour (kLedOn.withAlpha (0.35f));
        g.fillEllipse (led.expanded (3.0f));
        g.setColour (kLedOn);
    }
    else
        g.setColour (kLedOff);

    g.fillRect (led);
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.drawRect (led, 1.0f);
}

void StripLookAndFeel::drawButtonText (juce::Graphics& g, juce::TextButton& button, bool, bool)
{
    auto r = button.getLocalBounds().withTrimmedLeft (20);
    g.setColour (kText);
    g.setFont (juce::FontOptions (10.5f).withStyle ("Bold"));
    g.drawText (button.getButtonText(), r, juce::Justification::centred, false);
}

//==============================================================================
void LevelMeter::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setColour (juce::Colour (0xff0a0a0b));
    g.fillRoundedRectangle (b, 2.0f);
    g.setColour (juce::Colour (0xff55575c));
    g.drawRoundedRectangle (b.reduced (0.5f), 2.0f, 1.0f);

    auto bar = b.reduced (4.0f);
    if (mode == Mode::level)
        bar.removeFromRight (22.0f);

    const int numSeg = (mode == Mode::level) ? 22 : 12;
    const float segH = bar.getHeight() / (float) numSeg;

    auto segRect = [&] (int i) -> juce::Rectangle<float>
    {
        if (mode == Mode::level) // 0 = bottom
            return { bar.getX(), bar.getBottom() - (float) (i + 1) * segH + 1.0f, bar.getWidth(), segH - 2.0f };
        return { bar.getX(), bar.getY() + (float) i * segH + 1.0f, bar.getWidth(), segH - 2.0f }; // 0 = top
    };

    if (mode == Mode::level)
    {
        constexpr float minDb = -60.0f, step = 3.0f;

        // exact vertical position of a dB value (continuous inside each LED)
        auto yOf = [&] (float db)
        {
            const float pos = juce::jlimit (0.0f, (float) numSeg - 0.0001f, (db - minDb) / step);
            const int i = (int) std::floor (pos);
            const auto r = segRect (i);
            return r.getBottom() - r.getHeight() * (pos - (float) i);
        };

        for (int i = 0; i < numSeg; ++i)
        {
            const float lo = minDb + step * (float) i;
            const juce::Colour col = lo >= 0.0f   ? juce::Colour (0xffff3b30)
                                   : lo >= -6.0f  ? juce::Colour (0xffff9800)
                                   : lo >= -12.0f ? juce::Colour (0xffe6d23a)
                                                  : juce::Colour (0xff3ddc4a);
            const auto r = segRect (i);

            g.setColour (col.withAlpha (0.13f));
            g.fillRect (r);

            const float f = juce::jlimit (0.0f, 1.0f, (rms - lo) / step);
            if (f > 0.0f)
            {
                g.setColour (col);
                g.fillRect (r.getX(), r.getBottom() - r.getHeight() * f, r.getWidth(), r.getHeight() * f);
            }
        }

        if (showTarget)
        {
            const auto r = segRect (14); // -18 .. -15 dB
            g.setColour (kAmber);
            g.fillRect (b.getX() + 1.0f, r.getY(), 3.0f, r.getHeight());
        }

        if (peak > minDb)
        {
            g.setColour (peak >= -0.5f ? juce::Colour (0xffff3b30) : juce::Colours::white);
            g.fillRect (bar.getX(), yOf (peak) - 1.0f, bar.getWidth(), 2.0f);
        }

        const int ticks[] = { 0, -6, -12, -18, -24, -36, -48 };
        g.setFont (10.0f);
        for (int t : ticks)
        {
            const float y = yOf ((float) t);
            g.setColour (juce::Colour (0xff6c6f76));
            g.drawHorizontalLine ((int) y, bar.getRight(), bar.getRight() + 4.0f);
            g.setColour (t == -18 && showTarget ? kAmber : kDim);
            g.drawText (juce::String (t), juce::Rectangle<float> (bar.getRight() + 5.0f, y - 6.0f, 18.0f, 12.0f),
                        juce::Justification::centredLeft, false);
        }
    }
    else
    {
        // rms holds gain reduction in dB (<= 0), 2 dB per LED, filled continuously from the top
        const float reduction = -rms;

        for (int i = 0; i < numSeg; ++i)
        {
            const juce::Colour col = i >= 8 ? juce::Colour (0xffff3b30) : juce::Colour (0xffff9800);
            const auto r = segRect (i);

            g.setColour (col.withAlpha (0.13f));
            g.fillRect (r);

            const float f = juce::jlimit (0.0f, 1.0f, (reduction - (float) i * 2.0f) / 2.0f);
            if (f > 0.0f)
            {
                g.setColour (col);
                g.fillRect (r.getX(), r.getY(), r.getWidth(), r.getHeight() * f);
            }
        }
    }
}

//==============================================================================
VocalStripEditor::VocalStripEditor (VocalStripAudioProcessor& p)
    : AudioProcessorEditor (&p),
      proc (p),
      inMeter (LevelMeter::Mode::level, true),
      outMeter (LevelMeter::Mode::level, false),
      grMeter (LevelMeter::Mode::gainReduction),
      dsMeter (LevelMeter::Mode::gainReduction)
{
    setLookAndFeel (&laf);

    // Input
    addKnob ("inTrim", "TRIM");
    addToggle ("phase", "PHASE");

    // Filters
    addKnob ("hpfFreq", "HPF");
    addToggle ("hpfOn", "HPF IN");
    addKnob ("lpfFreq", "LPF");
    addToggle ("lpfOn", "LPF IN");

    // Gate
    addKnob ("gateThr", "THRESH");
    addKnob ("gateRange", "RANGE");
    addKnob ("gateHold", "HOLD");
    addKnob ("gateRel", "RELEASE");
    addToggle ("gateOn", "GATE IN");

    // Compressor
    addKnob ("compThr", "THRESH");
    addKnob ("compRatio", "RATIO");
    addKnob ("compAtk", "ATTACK");
    addKnob ("compRel", "RELEASE");
    addKnob ("compMakeup", "MAKEUP");
    addKnob ("compMix", "MIX");
    addKnob ("scHpfFreq", "SC HPF");
    addToggle ("scHpfOn", "SC IN");
    addToggle ("compOn", "COMP IN");

    // De-esser
    addKnob ("dsFreq", "FREQ");
    addKnob ("dsThr", "THRESH");
    addKnob ("dsRange", "RANGE");
    addToggle ("dsListen", "LISTEN");
    addToggle ("dsOn", "DE-ESS IN");

    // EQ
    addKnob ("lfGain", "LF GAIN");
    addKnob ("lfFreq", "LF FREQ");
    addToggle ("lfBell", "LF BELL");
    addKnob ("lmfGain", "LMF GAIN");
    addKnob ("lmfFreq", "LMF FREQ");
    addKnob ("lmfQ", "LMF Q");
    addKnob ("hmfGain", "HMF GAIN");
    addKnob ("hmfFreq", "HMF FREQ");
    addKnob ("hmfQ", "HMF Q");
    addKnob ("hfGain", "HF GAIN");
    addKnob ("hfFreq", "HF FREQ");
    addToggle ("hfBell", "HF BELL");
    addToggle ("eqOn", "EQ IN");

    // Drive + output
    addKnob ("drive", "DRIVE");
    addToggle ("dynPost", "DYN AFTER EQ");

    outFader.setSliderStyle (juce::Slider::LinearVertical);
    outFader.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 68, 16);
    outFader.setColour (juce::Slider::textBoxTextColourId, kText);
    outFader.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (outFader);
    faderAttachment = std::make_unique<APVTS::SliderAttachment> (proc.apvts, "outLevel", outFader);

    addAndMakeVisible (inMeter);
    addAndMakeVisible (outMeter);
    addAndMakeVisible (grMeter);
    addAndMakeVisible (dsMeter);

    setupLabel (peakLabel, "PEAK --", 12.0f, juce::Justification::centred, true);
    setupLabel (rmsLabel, "RMS --", 12.0f, juce::Justification::centred, true);
    setupLabel (hintLabel, "GAIN STAGING\nAim for RMS around -18 dB (amber mark), peaks below -6 dB. Use TRIM.",
                10.0f, juce::Justification::topLeft);
    hintLabel.setColour (juce::Label::textColourId, kDim);
    setupLabel (outPeakLabel, "PEAK --", 11.0f, juce::Justification::centred, true);
    setupLabel (grCaption, "GR", 11.0f, juce::Justification::centred, true);
    setupLabel (dsGrCaption, "GR", 11.0f, juce::Justification::centred, true);
    setupLabel (driveNote, "4x oversampled", 9.5f, juce::Justification::centred);
    driveNote.setColour (juce::Label::textColourId, kDim);
    setupLabel (outCaption, "OUT", 11.0f, juce::Justification::centred, true);
    setupLabel (faderCaption, "LEVEL", 11.0f, juce::Justification::centred, true);
    setupLabel (trackLabel, "TRACK  --", 13.0f, juce::Justification::centred, true);
    trackLabel.setColour (juce::Label::textColourId, kLcd);

    setSize (1270, 480);
    startTimerHz (60);
}

VocalStripEditor::~VocalStripEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void VocalStripEditor::setupLabel (juce::Label& l, const juce::String& text, float size,
                                     juce::Justification j, bool bold)
{
    auto font = juce::FontOptions (size);
    if (bold)
        font = font.withStyle ("Bold");

    l.setText (text, juce::dontSendNotification);
    l.setFont (font);
    l.setJustificationType (j);
    l.setColour (juce::Label::textColourId, kText);
    l.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (l);
}

void VocalStripEditor::addKnob (const juce::String& id, const juce::String& caption)
{
    auto k = std::make_unique<Knob>();

    k->slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    k->slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 68, 16);
    k->slider.setColour (juce::Slider::textBoxTextColourId, kText);
    k->slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    k->slider.setMouseDragSensitivity (200);

    setupLabel (k->label, caption, 11.0f, juce::Justification::centred, true);

    addAndMakeVisible (k->slider);
    k->attachment = std::make_unique<APVTS::SliderAttachment> (proc.apvts, id, k->slider);

    knobs[id] = std::move (k);
}

void VocalStripEditor::addToggle (const juce::String& id, const juce::String& text)
{
    auto t = std::make_unique<Toggle>();

    t->button.setButtonText (text);
    t->button.setClickingTogglesState (true);

    addAndMakeVisible (t->button);
    t->attachment = std::make_unique<APVTS::ButtonAttachment> (proc.apvts, id, t->button);

    toggles[id] = std::move (t);
}

void VocalStripEditor::placeKnob (const juce::String& id, juce::Rectangle<int> cell)
{
    auto& k = *knobs.at (id);
    k.label.setBounds (cell.removeFromTop (14));
    k.slider.setBounds (cell);
}

void VocalStripEditor::placeToggle (const juce::String& id, juce::Rectangle<int> r)
{
    toggles.at (id)->button.setBounds (r.reduced (2, 0));
}

//==============================================================================
void VocalStripEditor::paint (juce::Graphics& g)
{
    const int w = getWidth(), h = getHeight();

    // front panel
    juce::ColourGradient bg (juce::Colour (0xff2a2b2e), 0.0f, 0.0f,
                             juce::Colour (0xff151618), 0.0f, (float) h, false);
    g.setGradientFill (bg);
    g.fillAll();

    // rack ears with screws
    auto screw = [&g] (float cx, float cy)
    {
        g.setColour (juce::Colour (0xff0b0b0c));
        g.fillEllipse (cx - 6.0f, cy - 6.0f, 12.0f, 12.0f);
        g.setColour (juce::Colour (0xff8b8d93));
        g.fillEllipse (cx - 5.0f, cy - 5.0f, 10.0f, 10.0f);
        juce::Path slot;
        slot.addRectangle (-4.0f, -0.8f, 8.0f, 1.6f);
        slot.applyTransform (juce::AffineTransform::rotation (0.6f).translated (cx, cy));
        g.setColour (juce::Colour (0xff2d2e32));
        g.fillPath (slot);
    };

    for (int side = 0; side < 2; ++side)
    {
        const int ex = side == 0 ? 0 : w - 22;
        g.setColour (juce::Colour (0xff333438));
        g.fillRect (ex, 0, 22, h);
        g.setColour (juce::Colour (0xff17181a));
        g.drawVerticalLine (side == 0 ? 21 : w - 22, 0.0f, (float) h);
        screw ((float) ex + 11.0f, 22.0f);
        screw ((float) ex + 11.0f, (float) h - 22.0f);
    }

    // header
    g.setColour (juce::Colour (0xfff2f2f2));
    g.setFont (juce::FontOptions (22.0f).withStyle ("Bold"));
    g.drawText ("VOCAL STRIP", 30, 8, 300, 30, juce::Justification::centredLeft);

    // LCD display for the track name
    const juce::Rectangle<float> lcd (340.0f, 10.0f, 300.0f, 26.0f);
    g.setColour (juce::Colour (0xff08120a));
    g.fillRoundedRectangle (lcd, 3.0f);
    g.setColour (juce::Colour (0xff55575c));
    g.drawRoundedRectangle (lcd.reduced (0.5f), 3.0f, 1.0f);

    g.setColour (kAccent);
    g.setFont (juce::FontOptions (22.0f).withStyle ("Bold"));
    g.drawText ("DaniArieta", w - 30 - 260, 8, 260, 30, juce::Justification::centredRight);

    g.setColour (kAccent);
    g.fillRect (30, 42, w - 60, 2);

    // silkscreen boxes
    for (auto& s : sections)
    {
        auto r = s.second.toFloat();

        g.setColour (juce::Colours::white.withAlpha (0.04f));
        g.fillRoundedRectangle (r, 4.0f);

        g.setColour (juce::Colours::white.withAlpha (0.65f));
        g.drawRoundedRectangle (r.reduced (0.5f), 4.0f, 1.0f);

        g.setColour (juce::Colours::white.withAlpha (0.3f));
        g.drawHorizontalLine (s.second.getY() + 22, (float) s.second.getX() + 6.0f, (float) s.second.getRight() - 6.0f);

        g.setColour (juce::Colours::white);
        g.setFont (juce::FontOptions (12.0f).withStyle ("Bold"));
        g.drawText (s.first, s.second.getX() + 8, s.second.getY() + 3, s.second.getWidth() - 16, 18,
                    juce::Justification::centredLeft);
    }
}

void VocalStripEditor::resized()
{
    constexpr int kh = 86, bh = 24;

    sections.clear();
    trackLabel.setBounds (344, 12, 292, 22);
    auto area = getLocalBounds().reduced (30, 10);
    area.removeFromTop (40);

    auto take = [&] (int w, const juce::String& name)
    {
        auto r = area.removeFromLeft (w);
        area.removeFromLeft (8);
        sections.push_back ({ name, r });
        auto c = r.reduced (6);
        c.removeFromTop (22);
        return c;
    };

    // INPUT
    {
        auto c = take (160, "INPUT");
        peakLabel.setBounds (c.removeFromTop (16));
        rmsLabel.setBounds (c.removeFromTop (16));
        c.removeFromTop (4);
        inMeter.setBounds (c.removeFromLeft (62));
        c.removeFromLeft (4);
        placeKnob ("inTrim", c.removeFromTop (kh));
        c.removeFromTop (6);
        placeToggle ("phase", c.removeFromTop (bh));
        c.removeFromTop (10);
        hintLabel.setBounds (c.removeFromTop (110));
    }

    // FILTERS
    {
        auto c = take (84, "FILTERS");
        placeKnob ("hpfFreq", c.removeFromTop (kh));
        placeToggle ("hpfOn", c.removeFromTop (bh));
        c.removeFromTop (20);
        placeKnob ("lpfFreq", c.removeFromTop (kh));
        placeToggle ("lpfOn", c.removeFromTop (bh));
    }

    // GATE (2 x 2 knobs)
    {
        auto c = take (164, "GATE");
        placeToggle ("gateOn", c.removeFromBottom (bh));
        auto col0 = c.removeFromLeft (c.getWidth() / 2);
        auto col1 = c;
        placeKnob ("gateThr", col0.removeFromTop (kh));
        placeKnob ("gateHold", col0.removeFromTop (kh));
        placeKnob ("gateRange", col1.removeFromTop (kh));
        placeKnob ("gateRel", col1.removeFromTop (kh));
    }

    // COMPRESSOR (2 columns x 4 rows)
    {
        auto c = take (168, "COMPRESSOR");
        placeToggle ("compOn", c.removeFromBottom (bh));
        auto col0 = c.removeFromLeft (c.getWidth() / 2);
        auto col1 = c;
        placeKnob ("compThr", col0.removeFromTop (kh));
        placeKnob ("compAtk", col0.removeFromTop (kh));
        placeKnob ("compMakeup", col0.removeFromTop (kh));
        placeKnob ("scHpfFreq", col0.removeFromTop (kh));
        placeKnob ("compRatio", col1.removeFromTop (kh));
        placeKnob ("compRel", col1.removeFromTop (kh));
        placeKnob ("compMix", col1.removeFromTop (kh));
        placeToggle ("scHpfOn", col1.removeFromTop (kh).withTrimmedTop (kh / 2 + 6).removeFromTop (bh));
    }

    // DE-ESSER
    {
        auto c = take (112, "DE-ESSER");
        placeToggle ("dsOn", c.removeFromBottom (bh));
        c.removeFromBottom (4);
        placeToggle ("dsListen", c.removeFromBottom (bh));
        c.removeFromBottom (4);

        auto meterCol = c.removeFromLeft (18);
        c.removeFromLeft (4);
        dsGrCaption.setBounds (meterCol.removeFromTop (14).expanded (6, 0));
        dsMeter.setBounds (meterCol.withHeight (3 * kh - 14));

        placeKnob ("dsFreq", c.removeFromTop (kh));
        placeKnob ("dsThr", c.removeFromTop (kh));
        placeKnob ("dsRange", c.removeFromTop (kh));
    }

    // EQ
    {
        auto c = take (300, "EQUALIZER");
        placeToggle ("eqOn", c.removeFromBottom (bh));
        const int colW = c.getWidth() / 4;

        auto lfCol = c.removeFromLeft (colW);
        placeKnob ("lfGain", lfCol.removeFromTop (kh));
        placeKnob ("lfFreq", lfCol.removeFromTop (kh));
        placeToggle ("lfBell", lfCol.removeFromTop (bh).translated (0, 8));

        auto lmfCol = c.removeFromLeft (colW);
        placeKnob ("lmfGain", lmfCol.removeFromTop (kh));
        placeKnob ("lmfFreq", lmfCol.removeFromTop (kh));
        placeKnob ("lmfQ", lmfCol.removeFromTop (kh));

        auto hmfCol = c.removeFromLeft (colW);
        placeKnob ("hmfGain", hmfCol.removeFromTop (kh));
        placeKnob ("hmfFreq", hmfCol.removeFromTop (kh));
        placeKnob ("hmfQ", hmfCol.removeFromTop (kh));

        auto hfCol = c;
        placeKnob ("hfGain", hfCol.removeFromTop (kh));
        placeKnob ("hfFreq", hfCol.removeFromTop (kh));
        placeToggle ("hfBell", hfCol.removeFromTop (bh).translated (0, 8));
    }

    // OUTPUT (drive on top, then fader + meters)
    {
        auto c = take (area.getWidth(), "DRIVE / OUTPUT");
        placeToggle ("dynPost", c.removeFromBottom (bh));
        c.removeFromBottom (6);

        placeKnob ("drive", c.removeFromTop (kh));
        driveNote.setBounds (c.removeFromTop (14));
        c.removeFromTop (6);

        auto caps = c.removeFromTop (16);
        auto faderCol = c.removeFromLeft (70);
        c.removeFromLeft (6);
        auto grCol = c.removeFromLeft (24);
        c.removeFromLeft (6);
        auto outCol = c;

        faderCaption.setBounds (caps.removeFromLeft (70));
        caps.removeFromLeft (6);
        grCaption.setBounds (caps.removeFromLeft (24));
        caps.removeFromLeft (6);
        outCaption.setBounds (caps);

        outFader.setBounds (faderCol);

        outPeakLabel.setBounds (outCol.removeFromBottom (16));
        grMeter.setBounds (grCol.withTrimmedBottom (16));
        outMeter.setBounds (outCol);
    }
}

//==============================================================================
void VocalStripEditor::timerCallback()
{
    using juce::Decibels;

    // ---- Track name (as reported by the host)
    const auto name = proc.getTrackName();
    if (name != lastTrackName)
    {
        lastTrackName = name;
        trackLabel.setText ("TRACK  " + (name.isEmpty() ? juce::String ("--") : name.toUpperCase()),
                            juce::dontSendNotification);
    }

    // ---- Input
    const float pkDb  = Decibels::gainToDecibels (proc.inPeak.exchange (0.0f), -100.0f);
    const float rmsDb = Decibels::gainToDecibels (proc.inRms.load(), -100.0f);

    if (pkDb >= inHold) { inHold = pkDb; inHoldFrames = 120; }
    else if (inHoldFrames > 0) --inHoldFrames;
    else inHold = juce::jmax (pkDb, inHold - 0.3f);

    inPeakDisp = juce::jmax (pkDb, inPeakDisp - 0.5f);
    inRmsDisp += 0.3f * (rmsDb - inRmsDisp);
    inMeter.setValues (inPeakDisp, inRmsDisp);

    peakLabel.setText ("PEAK  " + (inHold > -99.0f ? juce::String (inHold, 1) : juce::String ("--")) + " dB",
                       juce::dontSendNotification);
    peakLabel.setColour (juce::Label::textColourId, inHold >= -0.5f ? juce::Colour (0xffff3b30) : kText);

    rmsLabel.setText ("RMS  " + (inRmsDisp > -99.0f ? juce::String (inRmsDisp, 1) : juce::String ("--")) + " dB",
                      juce::dontSendNotification);
    const bool onTarget = inRmsDisp > -21.0f && inRmsDisp < -15.0f;
    rmsLabel.setColour (juce::Label::textColourId, onTarget ? juce::Colour (0xff3ddc4a) : kText);

    // ---- Output
    const float oPk  = Decibels::gainToDecibels (proc.outPeak.exchange (0.0f), -100.0f);
    const float oRms = Decibels::gainToDecibels (proc.outRms.load(), -100.0f);

    if (oPk >= outHold) { outHold = oPk; outHoldFrames = 120; }
    else if (outHoldFrames > 0) --outHoldFrames;
    else outHold = juce::jmax (oPk, outHold - 0.3f);

    outPeakDisp = juce::jmax (oPk, outPeakDisp - 0.5f);
    outRmsDisp += 0.3f * (oRms - outRmsDisp);
    outMeter.setValues (outPeakDisp, outRmsDisp);

    outPeakLabel.setText ((outHold > -99.0f ? juce::String (outHold, 1) : juce::String ("--")) + " dB",
                          juce::dontSendNotification);
    outPeakLabel.setColour (juce::Label::textColourId, outHold >= -0.5f ? juce::Colour (0xffff3b30) : kText);

    // ---- Gain reduction
    const float gr = proc.gainReduction.exchange (0.0f);
    grDisp = juce::jmin (gr, grDisp + 0.4f);
    grMeter.setValues (0.0f, grDisp);

    const float dsGr = proc.dsReduction.exchange (0.0f);
    dsDisp = juce::jmin (dsGr, dsDisp + 0.4f);
    dsMeter.setValues (0.0f, dsDisp);
}
