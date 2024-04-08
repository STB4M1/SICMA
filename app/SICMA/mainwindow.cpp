#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QScreen>

#include <QImage>
#include <QPixmap>
#include <QProcess>
#include <QToolTip>
#include <QMouseEvent>
#include "trimmingwindow.h"
#include "tensioncalculator.h"

#include <QKeyEvent>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("SICMA");

    baseSize = this->size(); // 初期ウィンドウサイズ（Qt Designerで設定したサイズ）を保存
    // すべてのウィジェットの初期幾何情報をマップに登録
    const auto widgets = this->findChildren<QWidget*>();
    for (auto *widget : widgets) {
        widgetBaseGeometry[widget] = widget->geometry();
    }
    for (auto *widget : widgets) {
        widgetInitialFontSizes[widget] = widget->font().pointSize();
    }

    // 画面解像度を取得
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int width = screenGeometry.width() * 0.8; // 幅を画面のx％に
    int height = screenGeometry.height() * 0.82; // 高さを画面のy％に

    // MainWindowのサイズを設定
    this->resize(width, height);

    trimmingWindow = new TrimmingWindow();
    trimmingWindow->setWindowModality(Qt::ApplicationModal); // トリミングウィンドウをモーダルに設定

    // TrimmingWindowからのシグナルを受け取るための接続
    bool connected = connect(trimmingWindow, &TrimmingWindow::imageCropped, this, &MainWindow::displayCroppedImage);

    // スライダーのvalueChangedシグナルをカスタムスロットに接続
    connect(ui->sliderThreshold, &QSlider::valueChanged, this, &MainWindow::showSliderValue);

    // 接続の成功を確認
    if (!connected) {
        qDebug() << "Error: Failed to connect signal and slot.";
    } else {
        qDebug() << "Signal and slot connected successfully.";
    }

    // lineEditRhoDrop と lineEditRhoAround の textChanged シグナルをラムダ式で処理
    auto updateDRho = [this]() {
        double rhoDrop = ui->lineEditRhoDrop->text().toDouble();
        double rhoAround = ui->lineEditRhoAround->text().toDouble();
        double dRho = std::abs(rhoDrop - rhoAround);
        ui->lineEditDRho->setText(QString::number(dRho));
    };

    connect(ui->lineEditRhoDrop, &QLineEdit::textChanged, this, updateDRho);
    connect(ui->lineEditRhoAround, &QLineEdit::textChanged, this, updateDRho);

    ui->lineEditG->setText("9.81");
    ui->lineEditPixelSize->setText("2.4");

    // Tabのフォーカス順序を設定
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
    setTabOrder(ui->lineEditDRho, ui->pushButtonCalculateTension);

//    // ラベルのサイズに合わせて画像をスケーリング
//    QPixmap scaledPixmap = diagramPixmap.scaled(ui->labelDeDsDiagram->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
//    ui->labelDeDsDiagram->setAlignment(Qt::AlignCenter);
//    // QLabelに画像を設定
//    ui->labelDeDsDiagram->setPixmap(scaledPixmap);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    const QSize newSize = event->size();
    const qreal widthRatio = static_cast<qreal>(newSize.width()) / baseSize.width();
    const qreal heightRatio = static_cast<qreal>(newSize.height()) / baseSize.height();
    qreal sizeRatio = std::sqrt(widthRatio * heightRatio); // 面積の比に基づく平均的なサイズ比


    for (auto it = widgetBaseGeometry.begin(); it != widgetBaseGeometry.end(); ++it) {
        QWidget *widget = it.key();
        const QRect &baseGeometry = it.value();

        QRect newGeometry(
            baseGeometry.x() * widthRatio,
            baseGeometry.y() * heightRatio,
            baseGeometry.width() * widthRatio,
            baseGeometry.height() * heightRatio
            );

        widget->setGeometry(newGeometry);
    }

    for (auto *widget : findChildren<QWidget*>()) {
        int initialFontSize = widgetInitialFontSizes.value(widget, 10); // デフォルト値を10に設定
        QFont font = widget->font();
        int newFontSize = std::max(1, static_cast<int>(initialFontSize * sizeRatio)); // 最小フォントサイズを5に設定
        font.setPointSize(newFontSize);
        widget->setFont(font);
    }

    // アプリケーションのディレクトリパスを取得
    QString imageDirPath = QCoreApplication::applicationDirPath();
    // QDirオブジェクトを使用してディレクトリを二階層上へ移動
    QDir dir_image(imageDirPath);
    dir_image.cdUp(); // 一階層上のディレクトリに移動
    dir_image.cdUp(); // もう一階層上に移動
    dir_image.cd("SICMA");
    QString imagePath = dir_image.absolutePath() + "/de-ds_diagram.png";

    // QPixmapオブジェクトを使用して画像をロード
    QPixmap diagramPixmap(imagePath);
    // QLabelに設定する新しいピクセルマップをスケーリングする
    QPixmap scaledPixmap = diagramPixmap.scaled(ui->labelDeDsDiagram->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->labelDeDsDiagram->setAlignment(Qt::AlignCenter);
    ui->labelDeDsDiagram->setPixmap(scaledPixmap);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        QWidget *focusWidget = this->focusWidget();
        if (focusWidget) {
            QPushButton *button = dynamic_cast<QPushButton *>(focusWidget);
            if (button) {
                button->click();
            }
        }
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::on_pushButtonSelectImage_clicked()
{

    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Select Image File"),
                                                    QDir::currentPath()
//                                                    , tr("Image Files (*.bmp *.png)")
                                                    );
    if (!filePath.isEmpty()) {
        QPixmap originalImage(filePath);
        if (!originalImage.isNull()) {

            // データ用の画像として元の画像を保存
            colorPixmap = originalImage;

            // ラベルのアライメントを中央に設定
            ui->labelImage->setAlignment(Qt::AlignCenter);

            // 画像をラベルのサイズに合わせてスケーリングし、アスペクト比を維持
            QPixmap scaledImage = originalImage.scaled(ui->labelImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

            // スケーリングされた画像をラベルに設定して表示
            ui->labelImage->setPixmap(scaledImage);

            // 画像サイズを表示するラベルを更新
            QString imageSizeText = QString("%1×%2").arg(originalImage.width()).arg(originalImage.height());
                                    ui->labelImageSize->setText(imageSizeText);
            ui->labelImageSize->setAlignment(Qt::AlignCenter); // テキストのアライメントを左寄せに設定
        } else {
            qDebug() << "Image file could not be loaded.";
        }
    }
}


void MainWindow::on_pushButtonGrayscale_clicked()
{
    if (!colorPixmap.isNull()) { // colorPixmap は現在表示されている画像の QPixmap
        QImage grayImage = colorPixmap.toImage().convertToFormat(QImage::Format_Grayscale8);
        QPixmap grayPixmap = QPixmap::fromImage(grayImage);
        grayscaledPixmap = grayPixmap;

        // グレースケール画像を表示するラベルのアライメントを中央に設定
        ui->labelGrayscaleImage->setAlignment(Qt::AlignCenter);

        // グレースケール画像をラベルのサイズに合わせてスケーリングし、表示
        ui->labelGrayscaleImage->setPixmap(grayPixmap.scaled(ui->labelGrayscaleImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        // 画像サイズを表示するラベルを更新
        QString imageSizeText = QString("%1×%2").arg(grayImage.width()).arg(grayImage.height());
                                ui->labelGrayScaledImageSize->setText(imageSizeText);
        ui->labelGrayScaledImageSize->setAlignment(Qt::AlignCenter); // テキストのアライメントを左寄せに設定
    }

}

void MainWindow::showSliderValue(int value) {
    QSlider *slider = ui->sliderThreshold; // sliderThreshold はスライダーのオブジェクト名

    // スライダーの位置とサイズを取得
    QRect sliderRect = slider->geometry();
    QPoint globalSliderPos = slider->mapToGlobal(QPoint(0, 0));

    // スライダーのハンドル位置を計算（正規化された値を使用）
    double normalizedPosition = static_cast<double>(value - slider->minimum()) / (slider->maximum() - slider->minimum());
    int handleX = static_cast<int>(normalizedPosition * (sliderRect.width() - 8)); // 8はハンドルの幅のおおよその補正値

    // ツールチップの表示位置を調整（スライダーのハンドルの中心の上）
    QPoint tooltipPos = globalSliderPos + QPoint(handleX - 12, -sliderRect.height() - 20);

    // ツールチップを表示
    QToolTip::showText(tooltipPos, QString::number(value), slider, slider->rect());
}

void MainWindow::on_pushButtonBinary_clicked()
{
    if (!grayscaledPixmap.isNull()) {
        QImage grayImage = grayscaledPixmap.toImage().convertToFormat(QImage::Format_Grayscale8);
        QImage binaryImage(grayImage.size(), QImage::Format_Grayscale8);

        // 二値化処理の閾値
        int threshold = ui->sliderThreshold->value();

        for (int y = 0; y < grayImage.height(); ++y) {
            for (int x = 0; x < grayImage.width(); ++x) {
                int pixelValue = qGray(grayImage.pixel(x, y));
                binaryImage.setPixel(x, y, pixelValue > threshold ? qRgb(255, 255, 255) : qRgb(0, 0, 0));
            }
        }

        QPixmap binaryPixmap = QPixmap::fromImage(binaryImage);
        binarizedPixmap = binaryPixmap;

        // 二値化画像を表示するラベルのアライメントを中央に設定
        ui->labelBinaryImage->setAlignment(Qt::AlignCenter);
        // 二値化画像をラベルのサイズに合わせてスケーリングし、表示
        ui->labelBinaryImage->setPixmap(binaryPixmap.scaled(ui->labelBinaryImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        // 画像サイズを表示するラベルを更新
        QString imageSizeText = QString("%1×%2").arg(binaryImage.width()).arg(binaryImage.height());
                                ui->labelBinarizedImageSize->setText(imageSizeText);
        ui->labelBinarizedImageSize->setAlignment(Qt::AlignCenter); // テキストのアライメントを左寄せに設定
    }
}

void MainWindow::on_pushButtonOtsuBinarize_clicked()
{
    if (grayscaledPixmap.isNull()) return;

    // アプリケーションディレクトリパスを取得
    QString appDirPath = QCoreApplication::applicationDirPath();

    // QDirオブジェクトを使用してディレクトリを二階層上へ移動
    QDir dir(appDirPath);
    dir.cdUp(); // 一階層上のディレクトリに移動
    dir.cdUp(); // もう一階層上に移動
    dir.cd("SICMA");

    // スクリプトと一時ファイルのディレクトリを指定
    QString tempDirPath = dir.absolutePath();

    // 一時グレースケール画像と二値化画像のパスを指定
    QString tempImagePath = tempDirPath + "/temp_grayscale_image.png";
    QString outputImagePath = tempDirPath + "/temp_binarized_image.png";

    // グレースケール画像（currentPixmap）を一時ファイルに保存
    grayscaledPixmap.save(tempImagePath);

    // SICMAフォルダへのパスを組み立てる
    QString sicmaDirPath = dir.absolutePath();
    QString scriptPath = sicmaDirPath + "/otsu_binarize.py";

    // パスが正しいことをデバッグ出力で確認
    qDebug() << "Application dir path:" << appDirPath;
    qDebug() << "SICMA dir path:" << sicmaDirPath;
    qDebug() << "Script path:" << scriptPath;
    qDebug() << "Temporary grayscale image path:" << tempImagePath;
    qDebug() << "Output image path:" << outputImagePath;

    // Pythonスクリプトを実行して二値化処理を行う
    QProcess process;
//    QString pythonExecutable = "python"; // または、"python3" に変更する
//    process.start(pythonExecutable, QStringList() << scriptPath << tempImagePath << outputImagePath);
//    process.waitForFinished(); // スクリプトの終了を待つ

    QString appDirPath2 = QCoreApplication::applicationDirPath();
    QDir dir2(appDirPath2);
    dir2.cdUp(); // 一階層上のディレクトリに移動
    dir2.cdUp(); // もう一階層上に移動
    dir2.cd("SICMA");
    QString executablePath = dir2.absolutePath() + "/otsu_binarize.exe";

    // 画像パスを引数として追加
    QStringList arguments;
    arguments << tempImagePath << outputImagePath;
    process.start(executablePath, arguments);
    process.waitForFinished();


    // スクリプトの実行結果を確認
    int exitCode = process.exitCode();
//    QString out_example = QString::fromLocal8Bit(process.readAllStandardOutput());
//    QString error_example = QString::fromLocal8Bit(process.readAllStandardError());
    QString out_example = QString::fromUtf8(process.readAllStandardOutput());
    QString error_example = QString::fromUtf8(process.readAllStandardError());

    qDebug() << "Exit code:" << exitCode;
    qDebug() << "out_example:" << out_example;
    qDebug() << "error_example:" << error_example;

    // スクリプトの標準出力からしきい値を読み取る
    QString outputString = QString(out_example).trimmed(); // 出力の前後の空白を削除
    bool ok;
    double threshold = outputString.trimmed().toDouble(&ok);
    qDebug() << "thresh = " << threshold;
    if(ok) {
        ui->sliderThreshold->setValue(threshold);
    } else {
        qDebug() << "Failed to convert the output to an integer";
    }

    // スライダーにしきい値を設定
    ui->sliderThreshold->setValue(threshold);

    // デバッグ用: 出力画像の存在チェック
    if(QFile::exists(outputImagePath)) {
        qDebug() << "Binarized image file exists.";
        QPixmap binarizedPixmap_0;
        if(binarizedPixmap_0.load(outputImagePath)) {
            qDebug() << "Binarized image loaded successfully.";
            ui->labelBinaryImage->setPixmap(binarizedPixmap_0.scaled(ui->labelBinaryImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            qDebug() << "Failed to load the binarized image from file.";
        }
        binarizedPixmap = binarizedPixmap_0;
        // 画像サイズを表示するラベルを更新
        QString imageSizeText = QString("%1×%2").arg(binarizedPixmap_0.width()).arg(binarizedPixmap_0.height());
                                ui->labelBinarizedImageSize->setText(imageSizeText);
        ui->labelBinarizedImageSize->setAlignment(Qt::AlignCenter); // テキストのアライメントを左寄せに設定
    } else {
        qDebug() << "Binarized image file does not exist.";
    }


    // 一時ファイルを削除
    QFile::remove(tempImagePath);
    QFile::remove(outputImagePath);
}


void MainWindow::on_pushButtonTrim_clicked()
{
    qDebug() << "on_pushButtonTrim_clicked() function called.";
    if (!binarizedPixmap.isNull()) {
        // 二値化処理はすでにあるので、その結果をTrimmingWindowに渡す
        trimmingWindow->setBinaryImage(binarizedPixmap); // currentPixmapは二値化後の元の解像度を保持している画像
        trimmingWindow->show();
    }

}

void MainWindow::displayCroppedImage(const QPixmap &croppedImage)
{
    qDebug() << "MainWindow::displayCroppedImage() function called.";
    ui->labelCroppedImage->setAlignment(Qt::AlignCenter); // この行が画像を中央に配置します
    ui->labelCroppedImage->setPixmap(croppedImage.scaled(ui->labelCroppedImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    croppedPixmap = croppedImage;

    // 画像サイズを表示するラベルを更新
    QString imageSizeText = QString("%1×%2").arg(croppedImage.width()).arg(croppedImage.height());
                            ui->labelCroppedImageSize->setText(imageSizeText);
    ui->labelCroppedImageSize->setAlignment(Qt::AlignCenter); // テキストのアライメントを左寄せに設定
}




void MainWindow::on_pushButtonCalculateTension_clicked()
{
//    // この書き方やと，UIに表示されているラベルから画像を取得している。これは表示用にスケーリングされた画像やから、元の解像度の画像とは異なる。
//    QImage croppedImage = ui->labelCroppedImage->pixmap().toImage();

    qDebug() << "on_pushButtonCalculateTension_clicked() is called.";

    // lineEditから値を読み込む
    double pix_el_0 = ui->lineEditPixelSize->text().toDouble();
    double pix_el = pix_el_0*0.001;
    double g = ui->lineEditG->text().toDouble();
    double d_rho = ui->lineEditDRho->text().toDouble();

    // トリミングされた画像データをQImageに変換
    QImage croppedImage = croppedPixmap.toImage();

    TensionCalculator calculator;
    auto [gamma, de, ds] = calculator.calculateTension(croppedImage, pix_el, g, d_rho);

    ui->labelDe->setText(QString::number(de, 'f', 4) + " ");
    ui->labelDs->setText(QString::number(ds, 'f', 4) + " ");
    ui->labelTensionValue->setText(QString::number(gamma, 'f', 4) + " ");
}

