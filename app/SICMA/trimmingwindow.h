#ifndef TRIMMINGWINDOW_H
#define TRIMMINGWINDOW_H

#include <QWidget>
#include <QMouseEvent>
#include <QRect>
#include <QPainter>
#include "overlaywidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TrimmingWindow; }
QT_END_NAMESPACE

class TrimmingWindow : public QWidget
{
    Q_OBJECT

    double scaleX, scaleY; // スケーリング比率

public:
    explicit TrimmingWindow(QWidget *parent = nullptr);
    ~TrimmingWindow();

    void setBinaryImage(const QPixmap &image);

signals:
    void imageCropped(const QPixmap &croppedImage);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void updateSelectionRect(const QPoint &newPoint);
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_pushButtonApplyTrimming_clicked();

private:
    Ui::TrimmingWindow *ui;
    QPixmap image;
    QSize baseSize; // ベースとなるウィンドウサイズ
    QMap<QWidget*, QRect> widgetBaseGeometry; // ウィジェットとその基本幾何情報のマップ
    QMap<QWidget*, int> widgetInitialFontSizes; // ウィジェットの初期フォントサイズを保存
    QRect selectionRect;
    QRect imageRect;     // 追加：表示されている画像の領域
    bool isSelecting = false; // 追加：選択中のフラグ
    OverlayWidget *overlay; // ここに追加
    QPoint selectionStartPoint;
};
#endif // TRIMMINGWINDOW_H
