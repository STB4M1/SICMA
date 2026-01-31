#include "otsubinarizer.h"

#include <array>
#include <stdexcept>
#include <QtGlobal>
#include <QRgb>

inline int OtsuBinarizer::luminanceAt(const QImage& img, int x, int y)
{
    // Format_Grayscale8 なら byte を直接読む
    if (img.format() == QImage::Format_Grayscale8) {
        const uchar* line = img.constScanLine(y);
        return int(line[x]);
    }
    // それ以外は qGray で輝度化
    return qGray(img.pixel(x, y));
}

int OtsuBinarizer::computeThreshold(const QImage& src)
{
    if (src.isNull()) {
        throw std::runtime_error("OtsuBinarizer: src image is null.");
    }

    const int w = src.width();
    const int h = src.height();
    if (w <= 0 || h <= 0) {
        throw std::runtime_error("OtsuBinarizer: invalid image size.");
    }

    // histogram
    std::array<double, 256> hist{};
    hist.fill(0.0);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const int g = luminanceAt(src, x, y);
            hist[g] += 1.0;
        }
    }

    const double total = double(w) * double(h);
    if (total <= 0.0) {
        throw std::runtime_error("OtsuBinarizer: total pixels is zero.");
    }

    // 正規化（確率）
    for (auto& v : hist) v /= total;

    // 全体平均
    double muT = 0.0;
    for (int i = 0; i < 256; ++i) muT += i * hist[i];

    // Otsu
    double w0 = 0.0;
    double mu0 = 0.0;

    double bestSigmaB2 = -1.0;
    int bestT = 128;

    for (int t = 0; t < 256; ++t) {
        w0 += hist[t];
        mu0 += t * hist[t];

        const double w1 = 1.0 - w0;
        if (w0 <= 1e-12 || w1 <= 1e-12) continue;

        const double mu0n = mu0 / w0;
        const double mu1n = (muT - mu0) / w1;

        const double sigmaB2 = w0 * w1 * (mu0n - mu1n) * (mu0n - mu1n);
        if (sigmaB2 > bestSigmaB2) {
            bestSigmaB2 = sigmaB2;
            bestT = t;
        }
    }

    return qBound(0, bestT, 255);
}

int OtsuBinarizer::binarize(const QImage& src, QImage& dst, bool invert)
{
    const int t = computeThreshold(src);

    const int w = src.width();
    const int h = src.height();

    // 出力は Grayscale8 にする（0 or 255）
    dst = QImage(w, h, QImage::Format_Grayscale8);
    if (dst.isNull()) {
        throw std::runtime_error("OtsuBinarizer: failed to allocate dst image.");
    }

    for (int y = 0; y < h; ++y) {
        uchar* outLine = dst.scanLine(y);
        for (int x = 0; x < w; ++x) {
            const int g = luminanceAt(src, x, y);
            const bool white = (g > t);
            const int v = invert ? (white ? 0 : 255) : (white ? 255 : 0);
            outLine[x] = uchar(v);
        }
    }

    return t;
}
