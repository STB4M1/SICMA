#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QPixmap>
#include <QSize>

#include "trimmingwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_pushButtonSelectImage_clicked();
    void on_pushButtonGrayscale_clicked();
    void on_pushButtonOtsuBinarize_clicked();
    void on_pushButtonBinary_clicked();
    void on_pushButtonTrim_clicked();

    void on_pushButtonCalcTensionStaufferFordham_clicked();
    void on_pushButtonCalcTensionMisak_clicked();

    void showSliderValue(int value);

public slots:
    void displayCroppedImage(const QPixmap &croppedImage);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void appendLog(const QString& msg);
    void appendError(const QString& msg);
    void updateAllPreviews();

private:
    Ui::MainWindow *ui;

    QSize baseSize;
    QMap<QWidget*, QRect> widgetBaseGeometry;
    QMap<QWidget*, int> widgetInitialFontSizes;

    TrimmingWindow *trimmingWindow = nullptr;

    QPixmap colorPixmap;
    QPixmap grayscaledPixmap;
    QPixmap binarizedPixmap;
    QPixmap croppedPixmap;
};

#endif // MAINWINDOW_H
