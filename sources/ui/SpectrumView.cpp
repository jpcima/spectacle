#include "SpectrumView.h"
#include "FontEngine.h"
#include "Color.hpp"
#include "Window.hpp"
#include "Cairo.hpp"
#include <algorithm>

SpectrumView::SpectrumView(Widget *parent, FontEngine &fontEngine)
    : Widget(parent),
      fFontEngine(fontEngine)
{
}

void SpectrumView::setData(const float *frequencies, const float *magnitudes, uint32_t size, uint32_t numChannels)
{
    fFrequencies.assign(frequencies, frequencies + size * numChannels);
    fMagnitudes.assign(magnitudes, magnitudes + size * numChannels);
    fSize = size;
    fNumChannels = numChannels;
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
    const uint32_t size = fSize;
    const uint32_t numChannels = fNumChannels;
    Spline &spline = fSpline;

    if (size < 4) // need more elements for interpolation
        return;

    ///
    std::vector<PointF> points;
    points.reserve(width);

    ///
    for (uint32_t channel = 0; channel < numChannels; ++channel) {
        const float *rawFrequencies = &fFrequencies[channel * size];
        const float *rawMagnitudes = &fMagnitudes[channel * size];

        ///
        spline.setup(rawFrequencies, rawMagnitudes, size);

        ///
        auto wrap = [](double x) { return x - (long)x; };
        const Color color = Color::fromHSL(wrap(0.5 + channel / 3.0), 0.8, 0.3);

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
        cairo_set_source_rgb(cr, color.red, color.green, color.blue);
        cairo_stroke_preserve(cr);

        // plot fill
        cairo_line_to(cr, points.back().x, height);
        cairo_line_to(cr, points.front().x, height);
        cairo_set_source_rgba(cr, color.red, color.green, color.blue, 0.1f);
        cairo_fill(cr);
    }

    ///
    cairo_restore(cr);
}

void SpectrumView::displayBack()
{
    cairo_t *cr = getParentWindow().getGraphicsContext().cairo;
    FontEngine &fe = fFontEngine;

    ///
    const uint32_t width = getWidth();
    const uint32_t height = getHeight();

    ///
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.2);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);

    ///
    Font font;
    font.name = "regular";
    font.color = {0x4c, 0x4c, 0x4c, 0xff};
    font.size = 14;

    ///
    cairo_set_line_width(cr, 1.0);
    cairo_set_source_rgba(cr, 0.3, 0.3, 0.3, 0.5);
    for (int32_t midiKey = (fKeyMin + 9) % 12, keyMax = fKeyMax; midiKey <= keyMax; midiKey += 12)
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

    for (unsigned i = 0; ; ++i) {
        double g = dBmin + dBinterval * i;
        if (g > dBmax)
            break;

        double y = yOfDbMag(g);
        cairo_move_to(cr, 0, (int)y + 0.5);
        cairo_line_to(cr, width, (int)y + 0.5);
        cairo_stroke(cr);
        fe.drawInBox(cr, std::to_string(std::lrint(g)).c_str(), font, RectF(0, y + 4, width - 4, 0), FontEngine::AlignTopRight|FontEngine::AlignInside);
    }
}

double SpectrumView::frequencyOfX(double x) const
{
    return frequencyOfR(x / getWidth());
}

double SpectrumView::frequencyOfR(double r) const
{
    double midiKey = r * (fKeyMax - fKeyMin) + fKeyMin;
    return 440.0 * std::exp2((midiKey - 69.0) * (1.0 / 12.0));
}

double SpectrumView::rOfFrequency(double f) const
{
    double midiKey = 69.0 + 12.0 * std::log2(f * (1.0 / 440.0));
    return (midiKey - fKeyMin) / (fKeyMax - fKeyMin);
}

double SpectrumView::xOfFrequency(double f) const
{
    return rOfFrequency(f) * getWidth();
}

double SpectrumView::rOfDbMag(double m) const
{
    return (m - fdBmin) / (fdBmax - fdBmin);
}

double SpectrumView::yOfDbMag(double m) const
{
    return (1 - rOfDbMag(m)) * getHeight();
}
