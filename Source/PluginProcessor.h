#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include <atomic>
#include <cmath>

//==============================================================================
// Small self-contained biquad (RBJ cookbook), transposed direct form II
struct Biquad
{
    double b0 = 1.0, b1 = 0.0, b2 = 0.0, a1 = 0.0, a2 = 0.0;
    double z1 = 0.0, z2 = 0.0;

    void reset() noexcept { z1 = z2 = 0.0; }

    float process (float x) noexcept
    {
        const double in = (double) x;
        const double out = b0 * in + z1;
        z1 = b1 * in - a1 * out + z2;
        z2 = b2 * in - a2 * out;
        return (float) out;
    }

    void setRaw (double B0, double B1, double B2, double A0, double A1, double A2) noexcept
    {
        b0 = B0 / A0;
        b1 = B1 / A0;
        b2 = B2 / A0;
        a1 = A1 / A0;
        a2 = A2 / A0;
    }

    static double pi() noexcept { return 3.14159265358979323846; }
    static double limitFreq (double fs, double f) noexcept { return std::min (std::max (f, 5.0), fs * 0.45); }

    void makeHighPass1 (double fs, double f) noexcept
    {
        const double k = std::tan (pi() * limitFreq (fs, f) / fs);
        setRaw (1.0, -1.0, 0.0, k + 1.0, k - 1.0, 0.0);
    }

    void makeHighPass (double fs, double f, double q) noexcept
    {
        const double w0 = 2.0 * pi() * limitFreq (fs, f) / fs;
        const double c = std::cos (w0), alpha = std::sin (w0) / (2.0 * q);
        setRaw ((1.0 + c) * 0.5, -(1.0 + c), (1.0 + c) * 0.5, 1.0 + alpha, -2.0 * c, 1.0 - alpha);
    }

    void makeLowPass (double fs, double f, double q) noexcept
    {
        const double w0 = 2.0 * pi() * limitFreq (fs, f) / fs;
        const double c = std::cos (w0), alpha = std::sin (w0) / (2.0 * q);
        setRaw ((1.0 - c) * 0.5, 1.0 - c, (1.0 - c) * 0.5, 1.0 + alpha, -2.0 * c, 1.0 - alpha);
    }

    // constant 0 dB peak-gain band-pass (RBJ). Used as de-esser band / detector.
    void makeBandPass (double fs, double f, double q) noexcept
    {
        const double w0 = 2.0 * pi() * limitFreq (fs, f) / fs;
        const double c = std::cos (w0), alpha = std::sin (w0) / (2.0 * q);
        setRaw (alpha, 0.0, -alpha, 1.0 + alpha, -2.0 * c, 1.0 - alpha);
    }

    void makePeak (double fs, double f, double q, double gainDb) noexcept
    {
        const double A = std::pow (10.0, gainDb / 40.0);
        const double w0 = 2.0 * pi() * limitFreq (fs, f) / fs;
        const double c = std::cos (w0), alpha = std::sin (w0) / (2.0 * q);
        setRaw (1.0 + alpha * A, -2.0 * c, 1.0 - alpha * A, 1.0 + alpha / A, -2.0 * c, 1.0 - alpha / A);
    }

    void makeLowShelf (double fs, double f, double q, double gainDb) noexcept
    {
        const double A = std::pow (10.0, gainDb / 40.0);
        const double w0 = 2.0 * pi() * limitFreq (fs, f) / fs;
        const double c = std::cos (w0);
        const double t = 2.0 * std::sqrt (A) * (std::sin (w0) / (2.0 * q));
        setRaw (A * ((A + 1.0) - (A - 1.0) * c + t),
                2.0 * A * ((A - 1.0) - (A + 1.0) * c),
                A * ((A + 1.0) - (A - 1.0) * c - t),
                (A + 1.0) + (A - 1.0) * c + t,
                -2.0 * ((A - 1.0) + (A + 1.0) * c),
                (A + 1.0) + (A - 1.0) * c - t);
    }

    void makeHighShelf (double fs, double f, double q, double gainDb) noexcept
    {
        const double A = std::pow (10.0, gainDb / 40.0);
        const double w0 = 2.0 * pi() * limitFreq (fs, f) / fs;
        const double c = std::cos (w0);
        const double t = 2.0 * std::sqrt (A) * (std::sin (w0) / (2.0 * q));
        setRaw (A * ((A + 1.0) + (A - 1.0) * c + t),
                -2.0 * A * ((A - 1.0) + (A + 1.0) * c),
                A * ((A + 1.0) + (A - 1.0) * c - t),
                (A + 1.0) - (A - 1.0) * c + t,
                2.0 * ((A - 1.0) - (A + 1.0) * c),
                (A + 1.0) - (A - 1.0) * c - t);
    }
};

//==============================================================================
class VocalStripAudioProcessor : public juce::AudioProcessor
{
public:
    VocalStripAudioProcessor();
    ~VocalStripAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Vocal Strip"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Track name as reported by the host (if the host supports it)
    void updateTrackProperties (const TrackProperties& properties) override;
    juce::String getTrackName() const;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts;

    // Meters (read by the editor). Peak / gain reduction are reset by the editor via exchange(0).
    std::atomic<float> inPeak { 0.0f };       // linear, after input trim
    std::atomic<float> inRms { 0.0f };        // linear, ~300 ms window, after input trim
    std::atomic<float> outPeak { 0.0f };      // linear
    std::atomic<float> outRms { 0.0f };       // linear
    std::atomic<float> gainReduction { 0.0f }; // compressor GR, dB, <= 0
    std::atomic<float> dsReduction { 0.0f };   // de-esser GR, dB, <= 0

private:
    struct Params
    {
        float inTrimDb = 0, outDb = 0;
        bool phase = false, hpfOn = false, lpfOn = false, gateOn = false, compOn = false,
             eqOn = false, dynPost = false, lfBell = false, hfBell = false,
             scHpfOn = true, dsOn = false, dsListen = false;
        float hpfFreq = 80, lpfFreq = 18000;
        float gateThr = -50, gateRange = -20, gateRel = 0.25f, gateHoldMs = 100;
        float compThr = -18, compRatio = 3, compAtk = 12, compRel = 0.2f, compMakeup = 0;
        float compMix = 1.0f;       // 0..1 (dry/wet, parallel compression)
        float scHpfFreq = 120;      // compressor sidechain high-pass
        float dsFreq = 6500, dsThr = -30, dsRange = 6;
        float drive = 0.0f;         // 0..1
        float lfFreq = 100, lfGain = 0, lmfFreq = 700, lmfGain = 0, lmfQ = 1,
              hmfFreq = 3000, hmfGain = 0, hmfQ = 1, hfFreq = 8000, hfGain = 0;
    };

    Params readParams() const;
    void stageInput (juce::AudioBuffer<float>&, const Params&);
    void stageFilters (juce::AudioBuffer<float>&, const Params&);
    void stageDynamics (juce::AudioBuffer<float>&, const Params&);
    void stageEq (juce::AudioBuffer<float>&, const Params&);
    void stageDrive (juce::AudioBuffer<float>&, const Params&);
    void stageOutput (juce::AudioBuffer<float>&, const Params&);

    mutable juce::CriticalSection trackNameLock;
    juce::String trackName;

    double sr = 44100.0;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> trimSmooth, outSmooth;

    Biquad hpf1[2], hpf2[2], lpf[2], lf[2], lmf[2], hmf[2], hf[2];
    Biquad scHpf[2];   // compressor sidechain high-pass
    Biquad dsBand[2];  // de-esser band-pass (split band + detector)

    float gateEnv = 0.0f, gateGain = 1.0f;
    bool gateOpen = false;
    int gateHoldCount = 0;
    float compGrDb = 0.0f;
    float dsEnv = 0.0f, dsGrDb = 0.0f;

    // Drive: 4x oversampled soft saturation (polyphase IIR half-band, fixed integer latency)
    static constexpr float kMaxDriveK = 3.0f; // saturation gain at drive = 100 %
    juce::dsp::Oversampling<float> oversampler { 2, 2,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> driveSmooth;
    int osMaxBlock = 512;
    float msIn = 0.0f, msOut = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalStripAudioProcessor)
};
