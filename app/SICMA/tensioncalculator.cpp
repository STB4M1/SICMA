#include "tensioncalculator.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QDir>

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <cstdlib>

TensionCalculator::TensionCalculator() {}

// -------------------- path helper --------------------
QString TensionCalculator::resolveDataFilePath(const QString& filename)
{
    // 1) <app>/data/filename
    {
        QDir dir(QCoreApplication::applicationDirPath());
        if (dir.exists("data")) {
            const QString p = dir.absoluteFilePath("data/" + filename);
            if (QFile::exists(p)) return p;
        }
        const QString p = dir.absoluteFilePath("data/" + filename);
        if (QFile::exists(p)) return p;
    }

    // 2) ./data/filename (current working directory)
    {
        QDir dir(QDir::currentPath());
        const QString p = dir.absoluteFilePath("data/" + filename);
        if (QFile::exists(p)) return p;
    }

    // 3) Qt resource fallback (optional)
    {
        const QString p = QString(":/data/%1").arg(filename);
        if (QFile::exists(p)) return p;
    }

    throw std::runtime_error(("Data file not found: " + filename).toStdString());
}

// -------------------- load wide table --------------------
// wide format example:
// S 0 1 2 3 4 5 6 7 8 9
// 0.66 0.93828 0.93454 ... (10 values)
void TensionCalculator::loadWideInvH(const QString& path,
                                     QVector<double>& Svals,
                                     QVector<double>& invHvals)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error(("Cannot open file: " + path).toStdString());
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        if (line.startsWith("#")) continue;
        if (line.startsWith("S")) continue; // header

        const QStringList parts =
            line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

        if (parts.size() < 11) continue; // S + 10 values

        bool okS = false;
        const double baseS = parts[0].toDouble(&okS);
        if (!okS) continue;

        for (int k = 0; k < 10; ++k) {
            bool okV = false;
            const double v = parts[1 + k].toDouble(&okV);
            if (!okV) {
                throw std::runtime_error(("Invalid numeric in table: " + path).toStdString());
            }
            const double s = baseS + 0.001 * k;
            Svals.push_back(s);
            invHvals.push_back(v);
        }
    }

    if (Svals.size() < 2) {
        throw std::runtime_error(("Table is empty or invalid: " + path).toStdString());
    }

    // sort by S
    QVector<int> idx(Svals.size());
    for (int i = 0; i < idx.size(); ++i) idx[i] = i;
    std::sort(idx.begin(), idx.end(), [&](int a, int b){ return Svals[a] < Svals[b]; });

    QVector<double> S2; S2.reserve(Svals.size());
    QVector<double> V2; V2.reserve(invHvals.size());
    for (int i : idx) { S2.push_back(Svals[i]); V2.push_back(invHvals[i]); }
    Svals = std::move(S2);
    invHvals = std::move(V2);
}

// -------------------- linear interpolation --------------------
double TensionCalculator::interpLinear(const QVector<double>& X,
                                       const QVector<double>& Y,
                                       double x)
{
    if (X.size() < 2) throw std::runtime_error("Interpolation table is empty.");

    if (x <= X.front()) return Y.front();
    if (x >= X.back())  return Y.back();

    auto it = std::lower_bound(X.begin(), X.end(), x);
    int i = int(it - X.begin());
    if (i <= 0) return Y.front();
    if (i >= X.size()) return Y.back();

    const double x0 = X[i - 1], x1 = X[i];
    const double y0 = Y[i - 1], y1 = Y[i];

    if (x1 == x0) return y0;
    const double w = (x - x0) / (x1 - x0);
    return y0 + w * (y1 - y0);
}

// -------------------- Misak constants loader --------------------
// Expected columns per line (8 numbers):
// smin smax a B4 C3 C2 C1 C0
// invH(S) = B4/S^a + C3*S^3 + C2*S^2 + C1*S + C0
void TensionCalculator::loadMisakConstants(const QString& path,
                                           QVector<MisakSegment>& segs)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error(("Cannot open file: " + path).toStdString());
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        if (line.startsWith("#")) continue;

        // allow commas too
        line.replace(",", " ");
        const QStringList parts =
            line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

        // header-like lines skip
        bool anyNumber = false;
        for (const auto& p : parts) {
            bool ok=false; p.toDouble(&ok);
            if (ok) { anyNumber = true; break; }
        }
        if (!anyNumber) continue;

        QVector<double> nums;
        nums.reserve(parts.size());
        for (const auto& p : parts) {
            bool ok = false;
            const double v = p.toDouble(&ok);
            if (ok) nums.push_back(v);
        }

        // need at least 8 numeric values
        if (nums.size() < 8) continue;

        // take last 8 (robust to leading indices etc.)
        const int n = nums.size();
        MisakSegment seg;
        seg.smin = nums[n - 8];
        seg.smax = nums[n - 7];
        seg.a    = nums[n - 6];
        seg.B4   = nums[n - 5];
        seg.C3   = nums[n - 4];
        seg.C2   = nums[n - 3];
        seg.C1   = nums[n - 2];
        seg.C0   = nums[n - 1];

        if (!(seg.smin < seg.smax)) {
            throw std::runtime_error(("Invalid Misak range in: " + path).toStdString());
        }
        segs.push_back(seg);
    }

    if (segs.isEmpty()) {
        throw std::runtime_error(("Misak constants are empty/invalid: " + path).toStdString());
    }

    std::sort(segs.begin(), segs.end(), [](const MisakSegment& a, const MisakSegment& b){
        return a.smin < b.smin;
    });
}

double TensionCalculator::invH_fromMisak(double S,
                                         const QVector<MisakSegment>& segs)
{
    constexpr double eps = 1e-12;

    for (int i = 0; i < segs.size(); ++i) {
        const auto& seg = segs[i];
        const bool isLast = (i == segs.size() - 1);

        const bool inLow = (S + eps) >= seg.smin;
        const bool inHigh = isLast ? (S <= seg.smax + eps) : (S < seg.smax - eps);

        if (!inLow || !inHigh) continue;

        const double term = seg.B4 / std::pow(S, seg.a);

        // Python と同じ: +B3*S^3 -B2*S^2 +B1*S -B0
        const double invH =
            term
            + seg.C3 * std::pow(S, 3)
            - seg.C2 * std::pow(S, 2)
            + seg.C1 * S
            - seg.C0;

        return invH;
    }

    throw std::runtime_error("S is outside Misak constants range.");
}


// -------------------- main calculate --------------------
std::tuple<double, double, double>
TensionCalculator::calculateTension(const QImage& image,
                                    double pix_el,
                                    double g,
                                    double d_rho,
                                    Method method)
{
    int width = image.width();
    int height = image.height();

    if (width <= 0 || height <= 0) {
        throw std::runtime_error("Invalid image size.");
    }

    qDebug() << "pix_el [m/pixel] =" << pix_el;
    qDebug() << "g [m/s^2] =" << g;
    qDebug() << "d_rho [kg/m^3] =" << d_rho;
    qDebug() << "Image dimensions:" << width << "x" << height;

    // ---- 画像を 8bit 輝度配列へ ----
    auto freeDeleter = [](unsigned char* p){ if (p) std::free(p); };
    std::unique_ptr<unsigned char, decltype(freeDeleter)> image_in01(
        static_cast<unsigned char*>(std::malloc(sizeof(unsigned char) * width * height)),
        freeDeleter
        );

    if (!image_in01) {
        throw std::runtime_error("Memory allocation for image_in01 failed.");
    }

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            const QRgb pixel = image.pixel(i, j);
            const int luminance = qGray(pixel);
            image_in01.get()[width * j + i] = static_cast<unsigned char>(luminance);
        }
    }

    // ---- drop_width, bottom_pixel ----
    QVector<int> drop_width(height, 0);
    QVector<int> bottom_pixel(width, 0);

    // 各行の黒領域の左右端 → 幅
    for (int j = 0; j < height; ++j) {
        int left = -1, right = -1;

        for (int i = 0; i < width; ++i) {
            if (image_in01.get()[width * j + i] == 0) { left = i; break; }
        }
        for (int i = width - 1; i >= 0; --i) {
            if (image_in01.get()[width * j + i] == 0) { right = i; break; }
        }

        if (left >= 0 && right >= 0 && right >= left) drop_width[j] = right - left;
        else drop_width[j] = 0;
    }

    // de
    int max_drop_width = 0;
    for (int j = 0; j < height; ++j) max_drop_width = std::max(max_drop_width, drop_width[j]);

    if (max_drop_width <= 0) {
        throw std::runtime_error("Drop not detected (max_drop_width <= 0).");
    }

    // pix_el[m]
    const double de = max_drop_width * pix_el;
    qDebug() << "de =" << de << "m";

    // bottom_pixel: 各列の最下端の黒
    int max_bottom_pixel = 0;
    for (int i = 0; i < width; ++i) {
        int bottom = -1;
        for (int j = 0; j < height; ++j) {
            if (image_in01.get()[width * j + i] == 0) bottom = j;
        }
        bottom_pixel[i] = (bottom >= 0) ? bottom : 0;
        max_bottom_pixel = std::max(max_bottom_pixel, bottom_pixel[i]);
    }
    qDebug() << "max_drop_width_pixels =" << max_drop_width;
    qDebug() << "max_bottom_pixel =" << max_bottom_pixel;

    // ds
    const int width_pixel = max_bottom_pixel - max_drop_width;
    if (width_pixel < 0 || width_pixel >= height) {
        throw std::runtime_error("width_pixel out of range. Check drop detection logic.");
    }

    const double ds = drop_width[width_pixel] * pix_el;
    qDebug() << "ds =" << ds << "m";

    if (de <= 0.0) {
        throw std::runtime_error("Invalid de (<=0).");
    }

    // ---- S ----
    const double S = ds / de;
    qDebug() << "S (=ds/de) =" << S;

    // 要求仕様：S<0.30 と S>=0.999 はエラー（方式に関わらず）
    if (S < 0.30) {
        throw std::runtime_error("S (=ds/de) < 0.30 : out of valid range.");
    }
    if (S >= 0.999) {
        throw std::runtime_error("S (=ds/de) >= 0.999 : not allowed (0.999-1.00).");
    }

    // ---- invH(=1/H) ----
    double invH = 0.0;

    if (method == Method::StaufferFordhamTable) {
        static bool loaded = false;
        static QVector<double> S_st, invH_st;
        static QVector<double> S_fd, invH_fd;

        if (!loaded) {
            const QString stPath = resolveDataFilePath("stauffer_invH_wide.dat");
            const QString fdPath = resolveDataFilePath("fordham_invH_wide.dat");

            loadWideInvH(stPath, S_st, invH_st);
            loadWideInvH(fdPath, S_fd, invH_fd);

            loaded = true;

            qDebug() << "[InvH] Loaded Stauffer:" << S_st.size()
                     << "range" << S_st.front() << "to" << S_st.back()
                     << "from" << stPath;
            qDebug() << "[InvH] Loaded Fordham :" << S_fd.size()
                     << "range" << S_fd.front() << "to" << S_fd.back()
                     << "from" << fdPath;
        }

        // switch at 0.66
        if (S < 0.66) {
            if (S < S_st.front() || S > S_st.back()) {
                throw std::runtime_error("S is outside Stauffer table range.");
            }
            invH = interpLinear(S_st, invH_st, S);
        } else {
            if (S < S_fd.front() || S > S_fd.back()) {
                throw std::runtime_error("S is outside Fordham table range.");
            }
            invH = interpLinear(S_fd, invH_fd, S);
        }
    }
    else if (method == Method::MisakConstants) {
        static bool loaded = false;
        static QVector<MisakSegment> segs;

        if (!loaded) {
            const QString msPath = resolveDataFilePath("misak_invH_constants.dat");
            loadMisakConstants(msPath, segs);
            loaded = true;

            double minS = segs.front().smin;
            double maxS = segs.back().smax;
            qDebug() << "[InvH] Loaded Misak constants:" << segs.size()
                     << "range" << minS << "to" << maxS
                     << "from" << msPath;
        }

        invH = invH_fromMisak(S, segs);
    }
    else {
        throw std::runtime_error("Unknown method.");
    }

    if (!(invH > 0.0)) {
        throw std::runtime_error("invH(=1/H) is invalid (<=0).");
    }

    const double H = 1.0 / invH;

    qDebug() << "invH(=1/H) =" << invH;
    qDebug() << "H =" << H;

    // gamma = Δρ g de^2 * invH
    const double gamma = d_rho * g * (de * de) * invH;

    qDebug() << "gamma =" << gamma << " [N/m]";

    return {gamma, de, ds};
}
