#include "Parameters.h"
#include "dsp/AnalyzerDefs.h"

void InitParameter(uint32_t index, Parameter &parameter)
{
    ParameterEnumerationValue *pev;

    switch (index) {
    case kPidFftSize:
        parameter.hints = kParameterIsAutomable|kParameterIsInteger;
        parameter.name = "Resolution";
        parameter.symbol = "resolution";
        parameter.ranges = ParameterRanges(kStftDefaultSizeLog2, kStftMinSizeLog2, kStftMaxSizeLog2);
        parameter.enumValues.count = kStftNumSizes;
        parameter.enumValues.values = pev = new ParameterEnumerationValue[kStftNumSizes];
        for (uint32_t i = 0; i < kStftNumSizes; ++i) {
            pev[i].label = String(1u << (kStftMinSizeLog2 + i));
            pev[i].value = 1u << (kStftMinSizeLog2 + i);
        }
        break;
    case kPidReleaseTime:
        parameter.hints = kParameterIsAutomable|kParameterIsInteger;
        parameter.name = "Release time";
        parameter.symbol = "release_time";
        parameter.ranges = ParameterRanges(kStftDefaultSmoothTime, kStftMinSmoothTime, kStftMaxSmoothTime);
        break;
    }
}
