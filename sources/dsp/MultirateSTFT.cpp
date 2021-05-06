#include "MultirateSTFT.h"
#include <algorithm>
#include <cassert>

template <uint32_t Rates>
void MultirateSTFT<Rates>::configure(const Configuration &config)
{
    const uint32_t windowSize = config.windowSize;
    const uint32_t specSize = windowSize / 2;

    // one full-spectrum (lower), half-spectrum others
    const uint32_t numBins = specSize + (Rates-1) * specSize / 2;

    configureBasic(numBins);

    //
    Configuration rateConfig[Rates];
    for (uint32_t r = 0; r < Rates; ++r) {
        rateConfig[r] = config;
        rateConfig[r].stepSize = config.stepSize / (1u << r);
        rateConfig[r].sampleRate = config.sampleRate / (1u << r);
        fStft[r].configure(rateConfig[r]);
    }

    //
    float *frequencies = getFrequencies();
    uint32_t frequencyIndex = 0;

    fBinMapping.resize(numBins);
    BinMapping *binMappings = fBinMapping.data();

    // TODO skip processing the bins we don't need and their smoothers 

    for (uint32_t r = Rates; r-- > 0;) {
        double sampleRate = rateConfig[r].sampleRate;
        if (r == Rates - 1) {
            for (uint32_t b = 0; b < specSize; ++b) {
                frequencies[frequencyIndex] = b * sampleRate / windowSize;
                binMappings[frequencyIndex].rate = r;
                binMappings[frequencyIndex].bin = b;
                ++frequencyIndex;
            }
        }
        else {
            for (uint32_t b = specSize / 2; b < specSize; ++b) {
                frequencies[frequencyIndex] = b * sampleRate / windowSize;
                binMappings[frequencyIndex].rate = r;
                binMappings[frequencyIndex].bin = b;
                ++frequencyIndex;
            }
        }
    }
}

template <uint32_t Rates>
void MultirateSTFT<Rates>::setAttackAndRelease(float attack, float release)
{
    for (uint32_t r = 0; r < Rates; ++r)
        fStft[r].setAttackAndRelease(attack, release);
}

template <uint32_t Rates>
void MultirateSTFT<Rates>::clear()
{
    BasicAnalyzer::clear();

    for (uint32_t r = 0; r < Rates; ++r)
        fStft[r].clear();

    fDownsampler.clear();

    fNumRemainder = 0;

    std::fill_n(getMagnitudes(), getNumBins(), 20.0 * std::log10(kStftFloorMagnitude));
}

template <uint32_t Rates>
void MultirateSTFT<Rates>::process(const float *input, uint32_t numFrames)
{
    uint32_t numRemainder = fNumRemainder;
    float *remainder = fRemainder;

    //
    if (numRemainder > 0) {
        uint32_t numMissing = Factor - numRemainder;
        uint32_t numAvail = (numFrames < numMissing) ? numFrames : numMissing;

        std::copy_n(input, numAvail, &remainder[numRemainder]);
        numRemainder += numAvail;

        input += numAvail;
        numFrames -= numAvail;

        if (numRemainder < Factor)
            return;
        numRemainder = 0;

        processMultirate(remainder, Factor);
    }

    //
    while (numFrames > 0) {
        uint32_t currentFrames = (numFrames < TempSamples) ? numFrames : TempSamples;
        currentFrames &= ~(uint32_t)(Factor - 1);

        if (currentFrames > 0)
            processMultirate(input, currentFrames);
        else {
            currentFrames = numFrames;
            std::copy_n(input, currentFrames, remainder);
            numRemainder = currentFrames;
        }

        input += currentFrames;
        numFrames -= currentFrames;
    }

    processOutputBins();

    fNumRemainder = numRemainder;
}

template <uint32_t Rates>
void MultirateSTFT<Rates>::processMultirate(const float *input, uint32_t numFrames)
{
    assert(numFrames < TempSamples);
    assert(numFrames % Factor == 0);

    float *downsampledInputs[Rates - 1];
    downsampledInputs[0] = fTemp;
    for (uint32_t r = 1, l = TempSamples / 2; r < Rates - 1; ++r, l /= 2)
        downsampledInputs[r] = downsampledInputs[r - 1] + l;

    fDownsampler.downsample(numFrames / Factor, input, downsampledInputs);

    fStft[0].process(input, numFrames);
    for (uint32_t r = 1; r < Rates; ++r)
        fStft[r].process(downsampledInputs[r - 1], numFrames / (1u << r));
}

template <uint32_t Rates>
void MultirateSTFT<Rates>::processOutputBins()
{
    const uint32_t numBins = getNumBins();

    const float* multirateMags[Rates];
    for (uint32_t r = 0; r < Rates; ++r)
        multirateMags[r] = fStft[r].getMagnitudes();

    float* mags = getMagnitudes();
    const BinMapping *binMappings = fBinMapping.data();
    for (uint32_t b = 0; b < numBins; ++b)
        mags[b] = multirateMags[binMappings[b].rate][binMappings[b].bin];
}

///
template class MultirateSTFT<2>;
template class MultirateSTFT<3>;
template class MultirateSTFT<4>;
template class MultirateSTFT<5>;
