#include "trimmingwindow.h"
#include "ui_trimmingwindow.h"

#include <QGuiApplication>
#include <QScreen>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>
#include <QtGlobal>
#include <cmath>

#include "overlaywidget.h"

static inline QPoint clampPointToRect(const QPoint &p, const QRect &r)
{
    return QPoint(qBound(r.left(),  p.x(), r.right()),
                  qBound(r.top(),   p.y(), r.bottom()));
}

TrimmingWindow::TrimmingWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TrimmingWindow)
{
    ui->setupUi(this);
    setWindowTitle("SICMA");

    // （任意）初期サイズ調整
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        const QRect g = screen->geometry();
        resize(int(g.width() * 0.60), int(g.height() * 0.70));
    }

    // labelの設定
    ui->labelImageToBeCropped->setAlignment(Qt::AlignCenter);
    ui->labelImageToBeCropped->setMouseTracking(true);

    // ✅ レイアウトでlabelサイズが変わるので、labelのResize/Mouseを拾う
    ui->labelImageToBeCropped->installEventFilter(this);

    // ✅ overlayは label の子（座標系一致）
    overlay = new OverlayWidget(ui->labelImageToBeCropped);
    overlay->show();
    syncOverlayToLabel();

    // IMPORTANT:
    // on_pushButtonApplyTrimming_clicked() は auto-connect に任せる
}

TrimmingWindow::~TrimmingWindow()
{
    delete ui;
}

void TrimmingWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // ウィンドウ側リサイズでも一応同期（labelのResizeでも同期される）
    syncOverlayToLabel();

    if (!image.isNull()) {
        updateDisplayedPixmap();
    }
}

bool TrimmingWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->labelImageToBeCropped) {

        // labelのリサイズ（レイアウトによる伸縮）に追従
        if (event->type() == QEvent::Resize) {
            syncOverlayToLabel();
            if (!image.isNull()) updateDisplayedPixmap();
            return false;
        }

        // マウス操作は label 座標で扱う（ズレに強い）
        if (event->type() == QEvent::MouseButtonPress) {
            if (image.isNull()) return false;

            auto *me = static_cast<QMouseEvent*>(event);
            QPoint p = me->pos(); // label座標

            // 画像表示領域にクランプ
            p = clampPointToRect(p, imageRect);

            selectionStartPoint = p;
            isSelecting = true;
            updateSelectionRect(p);
            return true;
        }

        if (event->type() == QEvent::MouseMove) {
            if (!isSelecting) return false;

            auto *me = static_cast<QMouseEvent*>(event);
            QPoint p = me->pos();

            p = clampPointToRect(p, imageRect);
            updateSelectionRect(p);
            return true;
        }

        if (event->type() == QEvent::MouseButtonRelease) {
            if (!isSelecting) return false;

            auto *me = static_cast<QMouseEvent*>(event);
            QPoint p = me->pos();

            p = clampPointToRect(p, imageRect);
            updateSelectionRect(p);

            isSelecting = false;
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void TrimmingWindow::syncOverlayToLabel()
{
    if (!overlay) return;

    // overlayはlabelの子なので、rect() がそのまま一致
    overlay->setGeometry(ui->labelImageToBeCropped->rect());
    overlay->raise();
}

void TrimmingWindow::setBinaryImage(const QPixmap &img)
{
    image = img;

    // 選択リセット
    isSelecting = false;
    selectionRect = QRect();
    selectionStartPoint = QPoint();

    if (overlay) overlay->setSelectionRect(QRect());

    updateDisplayedPixmap();
}

void TrimmingWindow::updateDisplayedPixmap()
{
    if (image.isNull()) return;
    if (updatingPixmap) return;

    const QSize labelSize = ui->labelImageToBeCropped->size();
    if (labelSize.isEmpty()) return;

    updatingPixmap = true;

    // KeepAspectRatioで表示用pixmap生成
    QPixmap scaledPixmap = image.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->labelImageToBeCropped->setPixmap(scaledPixmap);

    // label内で実際に画像が描画されている矩形（余白込み）
    const int offsetX = (ui->labelImageToBeCropped->width()  - scaledPixmap.width())  / 2;
    const int offsetY = (ui->labelImageToBeCropped->height() - scaledPixmap.height()) / 2;
    imageRect = QRect(offsetX, offsetY, scaledPixmap.width(), scaledPixmap.height());

    updatingPixmap = false;

    ui->labelImageToBeCropped->update();
}

void TrimmingWindow::updateSelectionRect(const QPoint &pLabel)
{
    const int x1 = qMin(selectionStartPoint.x(), pLabel.x());
    const int y1 = qMin(selectionStartPoint.y(), pLabel.y());
    const int x2 = qMax(selectionStartPoint.x(), pLabel.x());
    const int y2 = qMax(selectionStartPoint.y(), pLabel.y());

    QRect r(x1, y1, x2 - x1, y2 - y1);

    // 画像表示領域に制限
    r = r.intersected(imageRect);
    selectionRect = r.normalized();

    if (overlay) overlay->setSelectionRect(selectionRect);
}

void TrimmingWindow::on_pushButtonApplyTrimming_clicked()
{
    qDebug() << "ApplyTrimming clicked.";

    if (image.isNull()) {
        qDebug() << "Image is null.";
        return;
    }
    if (selectionRect.isNull() || !selectionRect.isValid()) {
        qDebug() << "SelectionRect invalid:" << selectionRect;
        return;
    }

    const QRect sel = selectionRect.intersected(imageRect);
    if (sel.isEmpty()) {
        qDebug() << "Selection is outside imageRect.";
        return;
    }

    if (image.width() <= 0 || image.height() <= 0) {
        qDebug() << "Invalid image size.";
        return;
    }

    // ✅ KeepAspectRatioに忠実：縦横からscaleを決定（より堅牢）
    const double sx = double(imageRect.width())  / double(image.width());
    const double sy = double(imageRect.height()) / double(image.height());
    const double scale = std::min(sx, sy);

    if (scale <= 0.0) {
        qDebug() << "Invalid scale:" << scale;
        return;
    }

    // label座標 -> 元画像座標（余白補正 → /scale）
    const double x0 = (sel.left()   - imageRect.left()) / scale;
    const double y0 = (sel.top()    - imageRect.top())  / scale;
    const double x1 = (sel.right()  - imageRect.left()) / scale;
    const double y1 = (sel.bottom() - imageRect.top())  / scale;

    // 欠けにくい丸め：左上floor / 右下ceil
    int ix0 = int(std::floor(x0));
    int iy0 = int(std::floor(y0));
    int ix1 = int(std::ceil (x1));
    int iy1 = int(std::ceil (y1));

    QRect crop(QPoint(ix0, iy0), QPoint(ix1, iy1));
    crop = crop.normalized();

    crop = crop.intersected(QRect(0, 0, image.width(), image.height()));
    if (crop.isEmpty()) {
        qDebug() << "Crop rect empty after intersect. crop=" << crop;
        return;
    }

    const QImage croppedImage = image.toImage().copy(crop);
    emit imageCropped(QPixmap::fromImage(croppedImage));

    qDebug() << "imageRect:" << imageRect
             << "selectionRect:" << selectionRect
             << "sel:" << sel
             << "sx/sy/scale:" << sx << sy << scale
             << "crop:" << crop
             << "cropped size:" << croppedImage.size();
}
