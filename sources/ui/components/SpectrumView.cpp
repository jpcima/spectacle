#include "SpectrumView.h"
#include "ui/FontEngine.h"
#include "plugin/ColorPalette.h"
#include "Color.hpp"
#include "Window.hpp"
#include "Cairo.hpp"
#include <algorithm>
#include <cassert>

///
static double mtof(double m)
{
    return 440.0 * std::exp2((m - 69.0) * (1.0 / 12.0));
}

static double ftom(double f)
{
    return 69.0 + 12.0 * std::log2(f * (1.0 / 440.0));
}

///
SpectrumView::SpectrumView(Widget *parent, FontEngine &fontEngine, ColorPalette &palette)
    : Widget(parent),
      fFontEngine(fontEngine),
      fColorPalette(palette)
{
}

void SpectrumView::setData(const float *frequencies, const float *magnitudes, uint32_t size, uint32_t numChannels)
{
    Memory &mem = fActiveMemory;
    mem.frequencies.assign(frequencies, frequencies + size * numChannels);
    mem.magnitudes.assign(magnitudes, magnitudes + size * numChannels);
    mem.size = size;
    mem.numChannels = numChannels;
    mem.dirty = true;
    repaint();
}

void SpectrumView::toggleFreeze()
{
    fFreezeMemory = fActiveMemory;
    fFreeze = !fFreeze;
}

double SpectrumView::evalMagnitudeOnDisplay(uint32_t channel, double frequency) const
{
    const Memory &mem = getDisplayMemory();
    DISTRHO_SAFE_ASSERT_RETURN(channel < mem.numChannels, 0.0);
    const Spline &spline = mem.getSpline(channel);
    return spline.interpolate(frequency);
}

SpectrumView::Peak SpectrumView::findNearbyPeakOnDisplay(uint32_t channel, double frequency)
{
    SpectrumView::Peak pk = {};

    const Memory &mem = getDisplayMemory();
    DISTRHO_SAFE_ASSERT_RETURN(channel < mem.numChannels, pk);

    const uint32_t size = mem.size;
    DISTRHO_SAFE_ASSERT_RETURN(size > 0, pk);

    const Spline &spline = mem.getSpline(channel);

    // fprintf(stderr, "----- New search (C=%u) -----\n", channel);

    // fprintf(stderr, "Size: %u %d\n", size, spline.countElements());
    // fprintf(stderr, "Real Middle: F=%f M=%f\n", frequency, spline.interpolate(frequency));

    int32_t mid = spline.findElement(frequency);

    // fprintf(stderr, "Near Middle: I=%d F=%f M=%f\n", mid, spline.getX(mid), spline.getY(mid));

    int32_t direction = 0;
    if (mid - 1 < 0)
        direction = +1;
    else if ((uint32_t)(mid + 1) >= size)
        direction = -1;
    else
        direction = (spline.interpolate(frequency) < spline.getY(mid + 1)) ? +1 : -1;

    // fprintf(stderr, "Peak direction: %d\n", direction);

    auto indexValid = [size](int32_t i) {
        return i >= 0 && (uint32_t)i < size;
    };

    int32_t near = mid;
    while (indexValid(near + direction) && spline.getY(near + direction) >= spline.getY(near)) {
        near += direction;
        // fprintf(stderr, "Peak advance to: %d F=%f M=%f\n", near, spline.getX(near), spline.getY(near));
    }

    if (0) {
        pk.frequency = spline.getX(near);
        pk.magnitude = spline.getY(near);
    }
    else {
        // narrow peak down by repeated spline evaluations
        constexpr uint32_t numEvaluations = 8;

        const int32_t left = std::max(0, near - 1);
        const int32_t right = std::min((int32_t)size - 1, near + 1);
        double f1 = 0.5 * (spline.getX(near) + spline.getX(left));
        double f2 = 0.5 * (spline.getX(near) + spline.getX(right));

        double f = 0.5 * (f1 + f2);
        double m = spline.interpolate(f);
        for (uint32_t i = 0; i < numEvaluations; ++i) {
            double fc1 = 0.5 * (f + f1);
            double mc1 = spline.interpolate(fc1);
            if (mc1 > m) {
                m = mc1;
                f2 = f;
            }
            else {
                double fc2 = 0.5 * (f + f2);
                double mc2 = spline.interpolate(fc2);
                m = mc2;
                f1 = f;
            }
            f = 0.5 * (f1 + f2);
        }
        pk.frequency = f;
        pk.magnitude = m;
    }

    // fprintf(stderr, "----- End search -----\n");

    return pk;
}

void SpectrumView::setKeyScale(float keyMin, float keyMax)
{
    if (keyMin == fKeyMin || keyMax == fKeyMax)
        return;

    fKeyMin = keyMin;
    fKeyMax = keyMax;
    repaint();
}

void SpectrumView::setDbScale(float dbMin, float dbMax)
{
    if (dbMin == fdBmin || dbMax == fdBmax)
        return;

    fdBmin = dbMin;
    fdBmax = dbMax;
    repaint();
}

void SpectrumView::setDefaultScales()
{
    setKeyScale(kKeyMinDefault, kKeyMaxDefault);
    setDbScale(kdBminDefault, kdBmaxDefault);
}

void SpectrumView::clearReferenceLine()
{
    if (!fHaveReferenceLine)
        return;

    fHaveReferenceLine = false;
    repaint();
}

void SpectrumView::setReferenceLine(float key, float db)
{
    if (fHaveReferenceLine && fKeyRef == key && fdBref == db)
        return;

    fHaveReferenceLine = true;
    fKeyRef = key;
    fdBref = db;
    repaint();
}

void SpectrumView::onDisplay()
{
    cairo_t *cr = getParentWindow().getGraphicsContext().cairo;
    cairo_save(cr);

    ///
    const uint32_t width = getWidth();
    const uint32_t height = getHeight();

    ///
    displayBack();

    ///
    const Memory &mem = getDisplayMemory();
    const uint32_t size = mem.size;
    const uint32_t numChannels = mem.numChannels;

    if (size < 4) // need more elements for interpolation
        return;

    ///
    std::vector<PointF> points;
    points.reserve(width);

    ///
    const ColorPalette &cp = fColorPalette;

    ///
    for (uint32_t channel = 0; channel < numChannels; ++channel) {
        const Spline &spline = mem.getSpline(channel);

        ///
        points.clear();
        for (uint32_t x = 0, step = 1; x <= width; x += step) {
            const float f = frequencyOfX(x);
            const float y = yOfDbMag(spline.interpolate(f));
            points.emplace_back(x, y);
        }

        // plot line
        cairo_new_path(cr);
        cairo_move_to(cr, points[0].x, points[0].y);
        for (uint32_t i = 1, n = points.size(); i < n; ++i)
            cairo_line_to(cr, points[i].x, points[i].y);
        cairo_set_line_width(cr, 1.0);
        cairo_set_source_rgba8(cr, cp[Colors::spectrum_line_channel1 + channel]);
        cairo_stroke_preserve(cr);

        // plot fill
        cairo_line_to(cr, points.back().x, height);
        cairo_line_to(cr, points.front().x, height);
        cairo_set_source_rgba8(cr, cp[Colors::spectrum_fill_channel1 + channel]);
        cairo_fill(cr);
    }

    ///
    if (fHaveReferenceLine) {
        const double x = xOfKey(fKeyRef);
        const double y = yOfDbMag(fdBref);
        cairo_set_line_width(cr, 1.0);
        cairo_set_source_rgba8(cr, cp[Colors::spectrum_select_line]);
        cairo_new_path(cr);
        cairo_move_to(cr, 0, (int)y + 0.5);
        cairo_line_to(cr, width, (int)y + 0.5);
        cairo_stroke(cr);
        cairo_move_to(cr, (int)x + 0.5, 0);
        cairo_line_to(cr, (int)x + 0.5, height);
        cairo_stroke(cr);
    }

    ///
    cairo_restore(cr);
}

void SpectrumView::displayBack()
{
    cairo_t *cr = getParentWindow().getGraphicsContext().cairo;
    FontEngine &fe = fFontEngine;
    const ColorPalette &cp = fColorPalette;

    ///
    const uint32_t width = getWidth();
    const uint32_t height = getHeight();

    ///
    cairo_set_source_rgba8(cr, cp[Colors::spectrum_background]);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);

    ///
    Font font;
    font.name = "regular";
    font.color = cp[Colors::spectrum_grid_text];
    font.size = 14;

    ///
    cairo_set_line_width(cr, 1.0);
    cairo_set_source_rgba8(cr, cp[Colors::spectrum_grid_lines]);
    int32_t midiKey = (int32_t)std::ceil(fKeyMin);
    while ((midiKey + 3) % 12 != 0) ++midiKey;
    for (const double keyMax = fKeyMax; midiKey <= keyMax; midiKey += 12)
    {
        double frequency = 440.0 * std::exp2((midiKey - 69) * (1.0 / 12.0));
        double x = xOfFrequency(frequency);
        cairo_move_to(cr, (int)x + 0.5, 0);
        cairo_line_to(cr, (int)x + 0.5, height);
        cairo_stroke(cr);

        cairo_matrix_t mat;
        cairo_get_matrix(cr, &mat);
        cairo_translate(cr, x - 4, height - 4);
        cairo_rotate(cr, -0.5 * M_PI);
        fe.draw(cr, std::to_string(std::lrint(frequency)).c_str(), font, 0, 0);
        cairo_set_matrix(cr, &mat);
    }

    const double dBmin = fdBmin;
    const double dBmax = fdBmax;
    constexpr double dBinterval = 20.0;

    const double dBminAligned = std::round(dBmin / dBinterval) * dBinterval;

    for (unsigned i = 0; ; ++i) {
        double g = dBminAligned + dBinterval * i;
        if (g > dBmax)
            break;

        double y = yOfDbMag(g);
        cairo_move_to(cr, 0, (int)y + 0.5);
        cairo_line_to(cr, width, (int)y + 0.5);
        cairo_stroke(cr);
        fe.drawInBox(cr, std::to_string(std::lrint(g)).c_str(), font, RectF(0, y + 4, width - 4, 0), kAlignTopRight|kAlignInside);
    }
}

double SpectrumView::keyOfX(double x) const
{
    return keyOfR(x / getWidth());
}

double SpectrumView::keyOfR(double r) const
{
    return r * (fKeyMax - fKeyMin) + fKeyMin;
}

double SpectrumView::xOfKey(double k) const
{
    return rOfKey(k) * getWidth();
}

double SpectrumView::rOfKey(double k) const
{
    return (k - fKeyMin) / (fKeyMax - fKeyMin);
}

double SpectrumView::frequencyOfX(double x) const
{
    return frequencyOfR(x / getWidth());
}

double SpectrumView::frequencyOfR(double r) const
{
    return mtof(keyOfR(r));
}

double SpectrumView::rOfFrequency(double f) const
{
    return (ftom(f) - fKeyMin) / (fKeyMax - fKeyMin);
}

double SpectrumView::xOfFrequency(double f) const
{
    return rOfFrequency(f) * getWidth();
}

double SpectrumView::dbMagOfY(double y) const
{
    return dbMagOfR(1 - y / getHeight());
}

double SpectrumView::dbMagOfR(double r) const
{
    return fdBmin + r * (fdBmax - fdBmin);
}

double SpectrumView::rOfDbMag(double m) const
{
    return (m - fdBmin) / (fdBmax - fdBmin);
}

double SpectrumView::yOfDbMag(double m) const
{
    return (1 - rOfDbMag(m)) * getHeight();
}

Spline &SpectrumView::Memory::getSpline(uint32_t channel) const
{
    assert(channel < numChannels);

    if (dirty) {
        lazySpline.resize(numChannels);
        for (uint32_t c = 0; c < numChannels; ++c)
            lazySpline[c].setup(&frequencies[c * size], &magnitudes[c * size], size);
        dirty = false;
    }

    return lazySpline[channel];
}

constexpr float SpectrumView::kdBminDefault;
constexpr float SpectrumView::kdBmaxDefault;
constexpr float SpectrumView::kKeyMinDefault;
constexpr float SpectrumView::kKeyMaxDefault;
