#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "trimmingwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonSelectImage_clicked();
    void on_pushButtonGrayscale_clicked(); // グレースケール変換ボタンのスロット
    void on_pushButtonBinary_clicked();

    void on_pushButtonTrim_clicked();

    void showSliderValue(int value);

    void on_pushButtonCalculateTension_clicked();

    void on_pushButtonOtsuBinarize_clicked();

private:
    Ui::MainWindow *ui;
    QSize baseSize; // ベースとなるウィンドウサイズ
    QMap<QWidget*, QRect> widgetBaseGeometry; // ウィジェットとその基本幾何情報のマップ
    QMap<QWidget*, int> widgetInitialFontSizes; // ウィジェットの初期フォントサイズを保存
    TrimmingWindow *trimmingWindow;
    QPixmap colorPixmap; // 画像表示用のメンバ変数を追加
    QPixmap grayscaledPixmap;
    QPixmap binarizedPixmap;
    QPixmap croppedPixmap; // トリミングされた画像を保存するための変数
    bool isTrimMode = false; // トリミングモードの状態
public slots:
    void displayCroppedImage(const QPixmap &croppedImage); // トリミングされた画像を表示するスロット

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // MAINWINDOW_H
