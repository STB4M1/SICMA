#ifndef TENSIONCALCULATOR_H
#define TENSIONCALCULATOR_H

#include <QObject>
#include <QWidget>

class TensionCalculator
{
public:
    TensionCalculator();
    std::tuple<double, double, double> calculateTension(const QImage& image, double pix_el, double g, double d_rho);

private:
    double calculateDeDs(const QImage& image);
    double calculateGamma(double de, double ds);
};

#endif // TENSIONCALCULATOR_H
