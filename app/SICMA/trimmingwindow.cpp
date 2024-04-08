#include "trimmingwindow.h"
#include "ui_trimmingwindow.h"
#include <QScreen>
#include "overlaywidget.h"


TrimmingWindow::TrimmingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrimmingWindow),
    overlay(nullptr) // nullptrで初期化
{
    ui->setupUi(this);
    setWindowTitle("SICMA");
    baseSize = this->size(); // ウィンドウの初期サイズを保存

    const auto widgets = this->findChildren<QWidget*>();
    for (auto *widget : widgets) {
        widgetBaseGeometry[widget] = widget->geometry();
        widgetInitialFontSizes[widget] = widget->font().pointSize();
    }

    // 画面解像度を取得
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int width = screenGeometry.width() * 0.6; // 幅を画面の60％に
    int height = screenGeometry.height() * 0.7; // 高さを画面の70％に

    // TrimmingWindowのサイズを設定
    this->resize(width, height);

    // Apply Trimmingボタンのシグナルスロット接続
    connect(ui->pushButtonApplyTrimming, &QPushButton::clicked, this, &TrimmingWindow::on_pushButtonApplyTrimming_clicked);

    overlay = new OverlayWidget(this);
    if (!overlay) {
        qDebug() << "OverlayWidget initialization failed.";
    } else {
        overlay->setGeometry(ui->labelImageToBeCropped->geometry());
        qDebug() << "OverlayWidget initialized successfully.";
    }

    this->setFixedSize(this->size());
}

TrimmingWindow::~TrimmingWindow()
{
    delete ui;
}

void TrimmingWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    const QSize newSize = event->size();
    const qreal widthRatio = static_cast<qreal>(newSize.width()) / baseSize.width();
    const qreal heightRatio = static_cast<qreal>(newSize.height()) / baseSize.height();
    qreal sizeRatio = std::sqrt(widthRatio * heightRatio); // 面積の比に基づく平均的なサイズ比

    // OverlayWidgetの新しい幾何情報を設定
    QRect overlayNewGeometry = QRect(
        // 新しい位置を計算
        overlay->x() * widthRatio,
        overlay->y() * heightRatio,
        // 新しいサイズを計算
        overlay->width() * widthRatio,
        overlay->height() * heightRatio
        );

    // OverlayWidgetに新しい幾何情報を適用
    overlay->setGeometry(overlayNewGeometry);


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
        int newFontSize = std::max(5, static_cast<int>(initialFontSize * sizeRatio)); // 最小フォントサイズを5に設定
        font.setPointSize(newFontSize);
        widget->setFont(font);
    }

    // 画像がセットされていれば、ラベルのサイズに合わせて画像をスケーリングする
    if (!image.isNull()) {
        // ラベルサイズに合わせて画像をスケーリング
        QPixmap scaledPixmap = image.scaled(ui->labelImageToBeCropped->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // スケーリングされた画像をラベルにセット
        ui->labelImageToBeCropped->setPixmap(scaledPixmap);

        // 画像の表示位置を中央に設定
        ui->labelImageToBeCropped->setAlignment(Qt::AlignCenter);

        // スケーリング比率を更新
        QImage originalImage = image.toImage();
        scaleX = static_cast<double>(scaledPixmap.width()) / static_cast<double>(originalImage.width());
        scaleY = static_cast<double>(scaledPixmap.height()) / static_cast<double>(originalImage.height());
    }
}

void TrimmingWindow::setBinaryImage(const QPixmap &image)
{
    this->image = image; // トリミング対象の画像をメンバ変数に保持

    // 画像の原寸大サイズを取得
    QImage originalImage = image.toImage();

    // QLabelのサイズに合わせて、アスペクト比を保持してスケーリング
    QSize labelSize = ui->labelImageToBeCropped->size();
    QPixmap scaledPixmap = image.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // スケーリングされた画像のサイズを計算
    int offsetX = (ui->labelImageToBeCropped->width() - scaledPixmap.width()) / 2;
    int offsetY = (ui->labelImageToBeCropped->height() - scaledPixmap.height()) / 2;
    QRect imageRect = QRect(offsetX, offsetY, scaledPixmap.width(), scaledPixmap.height());

    // この画像範囲情報をメンバ変数に保存
    this->imageRect = imageRect; // imageRectはTrimmingWindowのメンバ変数

    // スケーリング比率を計算（オリジナル画像とスケーリング後の画像のサイズ比）
    scaleX = static_cast<double>(scaledPixmap.width()) / static_cast<double>(originalImage.width());
    scaleY = static_cast<double>(scaledPixmap.height()) / static_cast<double>(originalImage.height());

    // 画像をラベルの中央に配置
    ui->labelImageToBeCropped->setPixmap(scaledPixmap);
    ui->labelImageToBeCropped->repaint();
    ui->labelImageToBeCropped->setAlignment(Qt::AlignCenter);
}


void TrimmingWindow::mousePressEvent(QMouseEvent *event) {

    selectionStartPoint = ui->labelImageToBeCropped->mapFromParent(event->pos());
    isSelecting = true;
    updateSelectionRect(selectionStartPoint); // 選択開始点を初期選択範囲として設定

 //    qDebug() << "Mouse Press: SelectionRect" << selectionRect;
}

void TrimmingWindow::mouseMoveEvent(QMouseEvent *event) {
    if (isSelecting) {
        QPoint newPoint = ui->labelImageToBeCropped->mapFromParent(event->pos());
        updateSelectionRect(newPoint); // 選択範囲の更新
    }
//    qDebug() << "Mouse Move: SelectionRect" << selectionRect;
}

void TrimmingWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (isSelecting) {
        isSelecting = false;
        QPoint newPoint = ui->labelImageToBeCropped->mapFromParent(event->pos());

//        newPoint.setX(qMax(imageRect.left(), qMin(newPoint.x(), imageRect.right())));
//        newPoint.setY(qMax(imageRect.top(), qMin(newPoint.y(), imageRect.bottom())));
        updateSelectionRect(newPoint); // 最終的な選択範囲の更新
    }
//    qDebug() << "Mouse Release: SelectionRect" << selectionRect;
}

void TrimmingWindow::updateSelectionRect(const QPoint &newPoint) {
    // 新しい点から選択範囲を更新
    int x1 = qMin(selectionStartPoint.x(), newPoint.x());
    int y1 = qMin(selectionStartPoint.y(), newPoint.y());
    int x2 = qMax(selectionStartPoint.x(), newPoint.x());
    int y2 = qMax(selectionStartPoint.y(), newPoint.y());

    // 更新された選択範囲を設定
    selectionRect.setRect(x1, y1, x2 - x1, y2 - y1);

//    // オフセット値
//    int offsetX = 60; // X方向のオフセット
//    int offsetY = 40; // Y方向のオフセット

//    // OverlayWidgetの選択範囲も更新、ここでオフセットを適用
//    if (overlay) {
//        QRect offsetRect = selectionRect.translated(-ui->labelImageToBeCropped->pos() + QPoint(offsetX, offsetY));
//        overlay->setSelectionRect(offsetRect);
//    }

    if (overlay) {
        // labelImageToBeCroppedの位置とOverlayWidgetの位置の差分をオフセットとして計算
        QPoint offset = ui->labelImageToBeCropped->pos() - overlay->pos();

        // 計算したオフセットを適用
        QRect offsetRect = selectionRect.translated(offset);
        overlay->setSelectionRect(offsetRect);
    }
}



void TrimmingWindow::on_pushButtonApplyTrimming_clicked()
{
    qDebug() << "on_pushButtonApplyTrimming_clicked() called.";
    qDebug() << "Image is null:" << image.isNull();
    qDebug() << "SelectionRect is null or invalid:" << selectionRect.isNull() << selectionRect.isValid();
    qDebug() << "Original SelectionRect:" << selectionRect;

    if (!image.isNull() && !selectionRect.isNull() && selectionRect.isValid()) {
        // 画像の実際のサイズとラベルのサイズからスケールを計算
        qreal scaleX = static_cast<qreal>(ui->labelImageToBeCropped->width()) / image.width();
        qreal scaleY = static_cast<qreal>(ui->labelImageToBeCropped->height()) / image.height();

        // 選択範囲を約90%に調整するためのスケール比率を調整
        scaleX /= 1.0;
        scaleY /= 1.0;

        // 選択範囲の座標を調整
        QRect adjustedRect;
        adjustedRect.setX(static_cast<int>(selectionRect.x() / scaleX));
        adjustedRect.setY(static_cast<int>(selectionRect.y() / scaleY));
        adjustedRect.setWidth(static_cast<int>(selectionRect.width() / scaleX));
        adjustedRect.setHeight(static_cast<int>(selectionRect.height() / scaleY));

        QImage croppedImage = image.toImage().copy(adjustedRect);
        emit imageCropped(QPixmap::fromImage(croppedImage));

        qDebug() << "ScaleX:" << scaleX << "ScaleY:" << scaleY;
        qDebug() << "Adjusted SelectionRect:" << adjustedRect;
        qDebug() << "Trimming and signal transmission complete.";
        qDebug() << "Trimmed image size (width x height):" << croppedImage.width() << "x" << croppedImage.height();
    } else {
        qDebug() << "Trimming or signal transmission failed.";
    }
}






