#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "otsubinarizer.h"

#include <QDebug>
#include <QFileDialog>
#include <QScreen>
#include <QToolTip>
#include <QKeyEvent>
#include <QPushButton>
#include <QProcess>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QGuiApplication>
#include <QFileInfo>

#include "tensioncalculator.h"

// ==============================
// Helper: QLabelに "見切れない" 形でPixmapをフィット表示
// ==============================
static void setPixmapFitLabel(QLabel* label, const QPixmap& src)
{
    if (!label || src.isNull()) return;

    label->setScaledContents(false);
    label->setAlignment(Qt::AlignCenter);

    // ✅ ここが重要：padding/border を除いた「内側サイズ」を使う
    const QSize innerSize = label->contentsRect().size();
    if (innerSize.width() <= 0 || innerSize.height() <= 0) return;

    const qreal dpr = label->devicePixelRatioF();
    const QSize target = innerSize * dpr;

    QPixmap scaled = src.scaled(target, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    scaled.setDevicePixelRatio(dpr);

    label->setPixmap(scaled);
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("SICMA");

    // 初期サイズ保存＆ウィジェットの初期ジオメトリ・フォント保存
    baseSize = this->size();
    const auto widgets = this->findChildren<QWidget*>();
    for (auto *w : widgets) {
        widgetBaseGeometry[w] = w->geometry();
        widgetInitialFontSizes[w] = w->font().pointSize();
    }

    // 画面サイズに合わせて少し大きめに
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect g = screen->geometry();
        this->resize(int(g.width() * 0.80), int(g.height() * 0.82));
    }

    // TrimmingWindow
    trimmingWindow = new TrimmingWindow();
    trimmingWindow->setWindowModality(Qt::ApplicationModal);

    bool connected = connect(trimmingWindow, &TrimmingWindow::imageCropped,
                             this, &MainWindow::displayCroppedImage);
    if (!connected) {
        qDebug() << "Error: Failed to connect signal and slot.";
    }

    // Slider tooltip
    connect(ui->sliderThreshold, &QSlider::valueChanged,
            this, &MainWindow::showSliderValue);

    // Δρ自動更新
    auto updateDRho = [this]() {
        const double rhoDrop = ui->lineEditRhoDrop->text().toDouble();
        const double rhoAround = ui->lineEditRhoAround->text().toDouble();
        const double dRho = std::abs(rhoDrop - rhoAround);
        ui->lineEditDRho->setText(QString::number(dRho));
    };
    connect(ui->lineEditRhoDrop, &QLineEdit::textChanged, this, updateDRho);
    connect(ui->lineEditRhoAround, &QLineEdit::textChanged, this, updateDRho);

    // 初期値
    ui->lineEditG->setText("9.80665");
    ui->lineEditPixelSize->setText("6.9"); // μm

    // Tab順（UIに存在するものだけ）
    setTabOrder(ui->pushButtonSelectImage, ui->pushButtonGrayscale);
    setTabOrder(ui->pushButtonGrayscale, ui->pushButtonOtsuBinarize);
    setTabOrder(ui->pushButtonOtsuBinarize, ui->sliderThreshold);
    setTabOrder(ui->sliderThreshold, ui->pushButtonBinary);
    setTabOrder(ui->pushButtonBinary, ui->pushButtonTrim);
    setTabOrder(ui->pushButtonTrim, ui->lineEditPixelSize);
    setTabOrder(ui->lineEditPixelSize, ui->lineEditG);
    setTabOrder(ui->lineEditG, ui->lineEditRhoDrop);
    setTabOrder(ui->lineEditRhoDrop, ui->lineEditRhoAround);
    setTabOrder(ui->lineEditRhoAround, ui->lineEditDRho);
    setTabOrder(ui->lineEditDRho, ui->pushButtonCalcTensionStaufferFordham);
    setTabOrder(ui->pushButtonCalcTensionStaufferFordham, ui->pushButtonCalcTensionMisak);

    appendLog("Ready.");

    // 念のため：起動時に「現在のラベルサイズ」に合わせて貼り直す
    updateAllPreviews();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendLog(const QString &msg)
{
    const QString t = QDateTime::currentDateTime().toString("HH:mm:ss");
    ui->plainTextEditLogViewer->appendPlainText(QString("[%1] %2").arg(t, msg));
    // 下までスクロール
    auto *sb = ui->plainTextEditLogViewer->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void MainWindow::appendError(const QString &msg)
{
    appendLog(QString("[ERROR] %1").arg(msg));
}

// ==============================
// 追加：保持している元Pixmapから、現在のラベルサイズにフィットして貼り直す
// ==============================
void MainWindow::updateAllPreviews()
{
    if (!colorPixmap.isNull())
        setPixmapFitLabel(ui->labelImage, colorPixmap);

    if (!grayscaledPixmap.isNull())
        setPixmapFitLabel(ui->labelGrayscaleImage, grayscaledPixmap);

    if (!binarizedPixmap.isNull())
        setPixmapFitLabel(ui->labelBinaryImage, binarizedPixmap);

    if (!croppedPixmap.isNull())
        setPixmapFitLabel(ui->labelCroppedImage, croppedPixmap);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    const QSize newSize = event->size();
    const qreal widthRatio  = qreal(newSize.width())  / qreal(baseSize.width());
    const qreal heightRatio = qreal(newSize.height()) / qreal(baseSize.height());
    const qreal sizeRatio = std::sqrt(widthRatio * heightRatio);

    // geometry を比率で更新
    for (auto it = widgetBaseGeometry.begin(); it != widgetBaseGeometry.end(); ++it) {
        QWidget *w = it.key();
        const QRect &baseGeom = it.value();

        QRect newGeom(
            int(baseGeom.x() * widthRatio),
            int(baseGeom.y() * heightRatio),
            int(baseGeom.width() * widthRatio),
            int(baseGeom.height() * heightRatio)
            );
        w->setGeometry(newGeom);
    }

    // フォントサイズ更新
    for (auto *w : findChildren<QWidget*>()) {
        const int initial = widgetInitialFontSizes.value(w, 10);
        QFont f = w->font();
        f.setPointSize(std::max(1, int(initial * sizeRatio)));
        w->setFont(f);
    }

    // ✅ 重要：ラベルサイズも変わるので、画像を必ず貼り直す（見切れ対策の本体）
    updateAllPreviews();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        QWidget *fw = this->focusWidget();
        if (fw) {
            if (auto *btn = qobject_cast<QPushButton*>(fw)) {
                btn->click();
                return;
            }
        }
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::showSliderValue(int value)
{
    QSlider *slider = ui->sliderThreshold;
    QRect sliderRect = slider->geometry();
    QPoint globalSliderPos = slider->mapToGlobal(QPoint(0, 0));

    double normalized = double(value - slider->minimum()) / double(slider->maximum() - slider->minimum());
    int handleX = int(normalized * (sliderRect.width() - 8));
    QPoint tooltipPos = globalSliderPos + QPoint(handleX - 12, -sliderRect.height() - 20);

    QToolTip::showText(tooltipPos, QString::number(value), slider, slider->rect());
}

void MainWindow::on_pushButtonSelectImage_clicked()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this, tr("Select Image File"), QDir::currentPath()
        );

    if (filePath.isEmpty()) return;

    QPixmap original(filePath);
    if (original.isNull()) {
        appendError("Image file could not be loaded.");
        return;
    }

    colorPixmap = original;

    // ✅ 見切れない表示
    setPixmapFitLabel(ui->labelImage, colorPixmap);

    ui->labelImageSize->setText(QString("%1×%2").arg(original.width()).arg(original.height()));
    ui->labelImageSize->setAlignment(Qt::AlignCenter);

    appendLog(QString("Loaded image: %1").arg(QFileInfo(filePath).fileName()));
}

void MainWindow::on_pushButtonGrayscale_clicked()
{
    if (colorPixmap.isNull()) {
        appendError("No color image.");
        return;
    }

    QImage gray = colorPixmap.toImage().convertToFormat(QImage::Format_Grayscale8);
    grayscaledPixmap = QPixmap::fromImage(gray);

    // ✅ 見切れない表示
    setPixmapFitLabel(ui->labelGrayscaleImage, grayscaledPixmap);

    ui->labelGrayScaledImageSize->setText(QString("%1×%2").arg(gray.width()).arg(gray.height()));
    ui->labelGrayScaledImageSize->setAlignment(Qt::AlignCenter);

    appendLog("Converted to grayscale.");
}

void MainWindow::on_pushButtonBinary_clicked()
{
    if (grayscaledPixmap.isNull()) {
        appendError("No grayscale image.");
        return;
    }

    const int threshold = ui->sliderThreshold->value();

    QImage gray = grayscaledPixmap.toImage().convertToFormat(QImage::Format_Grayscale8);
    QImage bin(gray.size(), QImage::Format_Grayscale8);

    for (int y = 0; y < gray.height(); ++y) {
        for (int x = 0; x < gray.width(); ++x) {
            int pv = qGray(gray.pixel(x, y));
            bin.setPixel(x, y, (pv > threshold) ? qRgb(255, 255, 255) : qRgb(0, 0, 0));
        }
    }

    binarizedPixmap = QPixmap::fromImage(bin);

    // ✅ 見切れない表示
    setPixmapFitLabel(ui->labelBinaryImage, binarizedPixmap);

    ui->labelBinarizedImageSize->setText(QString("%1×%2").arg(bin.width()).arg(bin.height()));
    ui->labelBinarizedImageSize->setAlignment(Qt::AlignCenter);

    appendLog(QString("Binarized (threshold=%1).").arg(threshold));
}

void MainWindow::on_pushButtonOtsuBinarize_clicked()
{
    if (grayscaledPixmap.isNull()) {
        appendError("No grayscale image.");
        return;
    }

    try {
        QImage src = grayscaledPixmap.toImage().convertToFormat(QImage::Format_Grayscale8);

        QImage bin;
        const int threshold = OtsuBinarizer::binarize(src, bin, /*invert=*/false);

        ui->sliderThreshold->setValue(threshold);

        binarizedPixmap = QPixmap::fromImage(bin);

        // ✅ 見切れない表示
        setPixmapFitLabel(ui->labelBinaryImage, binarizedPixmap);

        ui->labelBinarizedImageSize->setText(QString("%1×%2").arg(bin.width()).arg(bin.height()));
        ui->labelBinarizedImageSize->setAlignment(Qt::AlignCenter);

        appendLog(QString("Otsu binarize done. threshold=%1").arg(threshold));
    }
    catch (const std::exception& e) {
        appendError(QString("Otsu failed: %1").arg(e.what()));
        QMessageBox::critical(this, "Otsu Error", e.what());
    }
}

void MainWindow::on_pushButtonTrim_clicked()
{
    if (binarizedPixmap.isNull()) {
        appendError("No binarized image.");
        return;
    }

    trimmingWindow->setBinaryImage(binarizedPixmap);
    trimmingWindow->show();
    appendLog("Trimming window opened.");
}

void MainWindow::displayCroppedImage(const QPixmap &croppedImage)
{
    croppedPixmap = croppedImage;

    // ✅ 見切れない表示
    setPixmapFitLabel(ui->labelCroppedImage, croppedPixmap);

    ui->labelCroppedImageSize->setText(QString("%1×%2").arg(croppedPixmap.width()).arg(croppedPixmap.height()));
    ui->labelCroppedImageSize->setAlignment(Qt::AlignCenter);

    appendLog("Cropped image received.");
}

static bool readInputs(const Ui::MainWindow* ui,
                       double& pix_m, double& g, double& d_rho,
                       QString& errMsg)
{
    bool ok1=false, ok2=false, ok3=false;

    // Pixel size [μm] -> [m]
    const double pix_um = ui->lineEditPixelSize->text().toDouble(&ok1);
    g = ui->lineEditG->text().toDouble(&ok2);
    d_rho = ui->lineEditDRho->text().toDouble(&ok3);

    if (!ok1 || !ok2 || !ok3) {
        errMsg = "Input parse failed (pixel size / g / dRho).";
        return false;
    }

    if (pix_um <= 0.0) { errMsg = "Pixel size must be > 0."; return false; }
    if (g <= 0.0)      { errMsg = "g must be > 0."; return false; }
    if (d_rho <= 0.0)  { errMsg = "Δρ must be > 0."; return false; }

    pix_m = pix_um * 1e-6;
    return true;
}

void MainWindow::on_pushButtonCalcTensionStaufferFordham_clicked()
{
    if (croppedPixmap.isNull()) {
        appendError("No cropped image.");
        return;
    }

    double pix_m=0.0, g=0.0, d_rho=0.0;
    QString err;
    if (!readInputs(ui, pix_m, g, d_rho, err)) {
        appendError(err);
        return;
    }

    try {
        TensionCalculator calc;
        QImage img = croppedPixmap.toImage();

        auto [gamma, de, ds] =
            calc.calculateTension(img, pix_m, g, d_rho,
                                  TensionCalculator::Method::StaufferFordhamTable);

        ui->labelDe->setText(QString::number(de, 'g', 6));
        ui->labelDs->setText(QString::number(ds, 'g', 6));
        ui->labelTensionValue->setText(QString::number(gamma, 'g', 6));

        appendLog(QString("Calced (Stauffer+Fordham): gamma=%1 N/m, de=%2 m, ds=%3 m")
                      .arg(gamma).arg(de).arg(ds));
    }
    catch (const std::exception& e) {
        appendError(QString("Calculation failed: %1").arg(e.what()));
        QMessageBox::critical(this, "Calculation Error", e.what());
    }
}

void MainWindow::on_pushButtonCalcTensionMisak_clicked()
{
    if (croppedPixmap.isNull()) {
        appendError("No cropped image.");
        return;
    }

    double pix_m=0.0, g=0.0, d_rho=0.0;
    QString err;
    if (!readInputs(ui, pix_m, g, d_rho, err)) {
        appendError(err);
        return;
    }

    try {
        TensionCalculator calc;
        QImage img = croppedPixmap.toImage();

        auto [gamma, de, ds] =
            calc.calculateTension(img, pix_m, g, d_rho,
                                  TensionCalculator::Method::MisakConstants);

        ui->labelDe->setText(QString::number(de, 'g', 6));
        ui->labelDs->setText(QString::number(ds, 'g', 6));
        ui->labelTensionValue->setText(QString::number(gamma, 'g', 6));

        appendLog(QString("Calced (Misak): gamma=%1 N/m, de=%2 m, ds=%3 m")
                      .arg(gamma).arg(de).arg(ds));
    }
    catch (const std::exception& e) {
        appendError(QString("Calculation failed: %1").arg(e.what()));
        QMessageBox::critical(this, "Calculation Error", e.what());
    }
}
