#ifndef TRIMMINGWINDOW_H
#define TRIMMINGWINDOW_H

#include <QWidget>
#include <QPixmap>
#include <QRect>
#include <QPoint>

QT_BEGIN_NAMESPACE
namespace Ui { class TrimmingWindow; }
QT_END_NAMESPACE

class OverlayWidget;

class TrimmingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TrimmingWindow(QWidget *parent = nullptr);
    ~TrimmingWindow() override;

    void setBinaryImage(const QPixmap &img);

signals:
    void imageCropped(const QPixmap &croppedImage);

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void on_pushButtonApplyTrimming_clicked();

private:
    void updateDisplayedPixmap();                 // labelサイズに合わせて表示pixmap更新
    void updateSelectionRect(const QPoint &pLabel); // label座標
    void syncOverlayToLabel();                    // overlay追従

private:
    Ui::TrimmingWindow *ui = nullptr;

    QPixmap image;            // 元画像
    QRect imageRect;          // label内の「実際に画像が表示されている領域」（余白込み）
    QRect selectionRect;      // label座標の選択矩形
    QPoint selectionStartPoint;

    bool isSelecting = false;
    bool updatingPixmap = false; // setPixmapの連鎖を避ける保険

    OverlayWidget *overlay = nullptr;
};

#endif // TRIMMINGWINDOW_H
