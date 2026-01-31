#ifndef OTSUBINARIZER_H
#define OTSUBINARIZER_H

#include <QImage>

class OtsuBinarizer
{
public:
    // 入力はグレースケール推奨（非グレースケールでも内部で輝度化して処理）
    // 戻り値: 適用した閾値 (0..255)
    static int binarize(const QImage& src, QImage& dst, bool invert = false);

    // 閾値だけ欲しい場合
    static int computeThreshold(const QImage& src);

private:
    static inline int luminanceAt(const QImage& img, int x, int y);
};

#endif // OTSUBINARIZER_H
