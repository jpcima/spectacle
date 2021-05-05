#pragma once
#include <cstdint>

// 2x <-> 1x: TBW = 0.01
static constexpr double C2x[12] = {
	0.036681502163648017,
	0.136547624631957715,
	0.274631759379454110,
	0.423138617436566666,
	0.561098697879194752,
	0.677540049974161618,
	0.769741833863226588,
	0.839889624849638028,
	0.892260818003878908,
	0.931541959963183896,
	0.962094548378083947,
	0.987816370732897076,
};
// 4x <-> 2x: TBW = 0.255
static constexpr double C4x[4] = {
	0.041893991997656171,
	0.168903482439952013,
	0.390560772921165922,
	0.743895748268478152,
};
// 8x <-> 4x: TBW = 0.3775
static constexpr double C8x[3] = {
	0.055748680811302048,
	0.243051195741530918,
	0.646699131192682297,
};
// 16x <-> 8x: TBW = 0.43875
static constexpr double C16x[2] = {
	0.107172166664564611,
	0.530904350331903085,
};

static constexpr uint32_t NC2x = sizeof(C2x) / sizeof(double);
static constexpr uint32_t NC4x = sizeof(C4x) / sizeof(double);
static constexpr uint32_t NC8x = sizeof(C8x) / sizeof(double);
static constexpr uint32_t NC16x = sizeof(C16x) / sizeof(double);
