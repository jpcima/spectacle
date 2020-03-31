#include "STFT.h"
#include <algorithm>
#include <unordered_map>
#include <mutex>
#include <cmath>

STFT::STFT()
{
    constexpr uint32_t capacity = 65536;

    _ring.reserve(capacity);
    _real.reserve(capacity);
    _cpx.reserve(capacity / 2 + 1);
    _window.reserve(capacity);

    for (uint32_t size = 256; size <= 65536; size <<= 1)
        prepareFFT(size);

    configure(1024, 64, 100e-3, 44100.0);
}

STFT::~STFT()
{
}

void STFT::configure(uint32_t fftSize, uint32_t stepSize, double smoothTime, double sampleRate)
{
    _fftPlan = prepareFFT(fftSize);
    _fftSize = fftSize;
    _stepSize = stepSize;
    _ring.resize(2 * fftSize);
    _real.resize(fftSize);
    _cpx.resize(fftSize / 2 + 1);
    _window.resize(fftSize);
    _outputFrequencies.resize(fftSize / 2 + 1);
    _outputMagnitudes.resize(fftSize / 2 + 1);
    _smoothPole = std::exp(-1.0 / (smoothTime * sampleRate / stepSize));

    float *window = _window.data();
    for (uint32_t i = 0; i < fftSize; ++i)
        window[i] = 0.5 * (1.0 - std::cos(2.0 * M_PI * i / (fftSize - 1)));

    float *frequencies = _outputFrequencies.data();
    for (uint32_t i = 0; i < fftSize / 2 + 1; ++i)
        frequencies[i] = i * (float)(sampleRate / fftSize);

    clear();
}

void STFT::clear()
{
    _stepCounter = 0;
    _ringIndex = 0;
    std::fill(_ring.begin(), _ring.end(), 0.0f);
    std::fill(_outputMagnitudes.begin(), _outputMagnitudes.end(), 0.0f);
}

void STFT::process(const float *input, uint32_t numFrames)
{
    const uint32_t fftSize = _fftSize;

    uint32_t stepCounter = _stepCounter;
    const uint32_t stepSize = _stepSize;

    float *ring = _ring.data();
    uint32_t ringIndex = _ringIndex;

    for (uint32_t i = 0; i < numFrames; ++i) {
        ring[ringIndex] = ring[ringIndex + fftSize] = input[i];
        ringIndex = (ringIndex + 1 != fftSize) ? (ringIndex + 1) : 0;
        if (++stepCounter == stepSize) {
            stepCounter = 0;
            processNewWindow(&ring[ringIndex]);
        }
    }

    _stepCounter = stepCounter;
    _ringIndex = ringIndex;
}

void STFT::processNewWindow(const float *rawInput)
{
    const uint32_t fftSize = _fftSize;
    const float *window = _window.data();

    float *real = _real.data();
    for (uint32_t i = 0; i < fftSize; ++i)
        real[i] = window[i] * rawInput[i];

    fftwf_plan plan = _fftPlan;
    std::complex<float> *cpx = _cpx.data();
    fftwf_execute_dft_r2c(plan, real, (fftwf_complex *)cpx);

    float *mag = _outputMagnitudes.data();
    const float smoothPole = _smoothPole;
    for (uint32_t i = 0; i < fftSize / 2 + 1; ++i) {
        double curMag = std::abs(cpx[i]) * (2.0f / fftSize);
        if (kOutputNoDcComponent && i == 0)
            curMag = 1e-9;
        if (kOutputAsDecibels)
            curMag = 20.0 * std::log10(std::max(1e-9, (double)curMag));
        mag[i] = mag[i] * smoothPole + curMag * (1.0f - smoothPole);
    }
}

static std::mutex _fftMutex;
static std::unordered_map<uint32_t, fftwf_plan_u> _fftPlans;

fftwf_plan STFT::prepareFFT(uint32_t fftSize)
{
    std::unique_lock<std::mutex> lock(_fftMutex);

    auto it = _fftPlans.find(fftSize);
    if (it != _fftPlans.end())
        return it->second.get();

    fftwf_real_vector real(fftSize);
    fftwf_complex_vector cpx(fftSize / 2 + 1);

    constexpr int planFlags = FFTW_ESTIMATE; // FFTW_MEASURE
    fftwf_plan plan = fftwf_plan_dft_r2c_1d(fftSize, real.data(), (fftwf_complex *)cpx.data(), planFlags);
    _fftPlans[fftSize] = fftwf_plan_u(plan);
    return plan;
}

constexpr bool STFT::kOutputAsDecibels;
constexpr bool STFT::kOutputNoDcComponent;
