#ifndef TENSIONCALCULATOR_H
#define TENSIONCALCULATOR_H

#include <QImage>
#include <tuple>

class TensionCalculator
{
public:
    enum class Method {
        StaufferFordhamTable,  // .dat table (Stauffer + Fordham)
        MisakConstants         // .dat constants (Misak piecewise)
    };

    TensionCalculator();

    // pix_el: [mm/pixel] で渡す想定（あなたのMainWindowは μm→mm→ここ、の流れでOK）
    // return: (gamma [N/m], de [m], ds [m])
    std::tuple<double, double, double>
    calculateTension(const QImage& image,
                     double pix_el,
                     double g,
                     double d_rho,
                     Method method = Method::StaufferFordhamTable);

private:
    static QString resolveDataFilePath(const QString& filename);

    static void loadWideInvH(const QString& path,
                             QVector<double>& Svals,
                             QVector<double>& invHvals);

    static double interpLinear(const QVector<double>& X,
                               const QVector<double>& Y,
                               double x);

    // Misak constants: invH = B4/S^a + C3*S^3 + C2*S^2 + C1*S + C0
    struct MisakSegment {
        double smin = 0.0;
        double smax = 0.0;
        double a    = 0.0;
        double B4   = 0.0;
        double C3   = 0.0;
        double C2   = 0.0;
        double C1   = 0.0;
        double C0   = 0.0;
    };

    static void loadMisakConstants(const QString& path,
                                   QVector<MisakSegment>& segs);

    static double invH_fromMisak(double S,
                                 const QVector<MisakSegment>& segs);
};

#endif // TENSIONCALCULATOR_H
