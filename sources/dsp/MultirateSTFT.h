#pragma once
#include "STFT.h"
#include "Oversampling.h"
#include <cstdint>

template <uint32_t Rates>
class MultirateSTFT final : public BasicAnalyzer {
public:
    void configure(const Configuration &config) override;
    void setAttackAndRelease(float attack, float release) override;
    void clear() override;
    void process(const float *input, uint32_t numFrames) override;

private:
    void processMultirate(const float *input, uint32_t numFrames);
    void processOutputBins();

private:
    static constexpr uint32_t Log2Factor = Rates - 1;
    static constexpr uint32_t Factor = 1u << Log2Factor;

    STFT fStft[Rates];
    Downsampler<Log2Factor> fDownsampler;

    struct BinMapping { uint32_t rate; uint32_t bin; };
    std::vector<BinMapping> fBinMapping;

    uint32_t fNumRemainder = 0;
    float fRemainder[Factor] = {};

    static constexpr uint32_t TempSamples = 1024u;
    float fTemp[TempSamples];
};
