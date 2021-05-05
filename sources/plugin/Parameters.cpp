#include "Parameters.h"
#include "dsp/AnalyzerDefs.h"

void InitParameter(uint32_t index, Parameter &parameter)
{
    ParameterEnumerationValue *pev;

    switch (index) {
    case kPidFftSize:
        parameter.hints = kParameterIsInteger;
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
    case kPidStepSize:
        parameter.hints = kParameterIsInteger;
        parameter.name = "Step";
        parameter.symbol = "step";
        parameter.ranges = ParameterRanges(kStftDefaultStepLog2, kStftMinStepLog2, kStftMaxStepLog2);
        parameter.enumValues.count = kStftNumSteps;
        parameter.enumValues.values = pev = new ParameterEnumerationValue[kStftNumSteps];
        for (uint32_t i = 0; i < kStftNumSteps; ++i) {
            pev[i].label = String(1u << (kStftMinStepLog2 + i));
            pev[i].value = 1u << (kStftMinStepLog2 + i);
        }
        break;
    case kPidAttackTime:
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Attack time";
        parameter.symbol = "attack_time";
        parameter.ranges = ParameterRanges(kStftDefaultAttackTime, kStftMinAttackTime, kStftMaxAttackTime);
        parameter.unit = "ms";
        break;
    case kPidReleaseTime:
        parameter.hints = kParameterIsAutomable;
        parameter.name = "Release time";
        parameter.symbol = "release_time";
        parameter.ranges = ParameterRanges(kStftDefaultReleaseTime, kStftMinReleaseTime, kStftMaxReleaseTime);
        parameter.unit = "ms";
        break;
    case kPidAlgorithm:
        parameter.hints = kParameterIsInteger;
        parameter.name = "Algorithm";
        parameter.symbol = "algorithm";
        parameter.ranges = ParameterRanges(kDefaultAlgorithm, 0, kNumAlgorithms - 1);
        parameter.enumValues.count = kNumAlgorithms;
        parameter.enumValues.values = pev = new ParameterEnumerationValue[kNumAlgorithms];
        for (uint32_t i = 0; i < kNumAlgorithms; ++i) {
            pev[i].label = String(getAlgorithmName((Algorithm)i));
            pev[i].value = i;
        }
        break;
    }
}
