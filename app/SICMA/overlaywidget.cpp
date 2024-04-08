// OverlayWidget.cpp
#include "overlaywidget.h"
#include "qpainter.h"
#include <QDebug>

OverlayWidget::OverlayWidget(QWidget *parent) : QWidget(parent), selectionRect(QRect()) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void OverlayWidget::setSelectionRect(const QRect &rect) {
    // 座標系の変換を確認
    QPoint topLeft = mapFromParent(rect.topLeft());
    QPoint bottomRight = mapFromParent(rect.bottomRight());
    selectionRect.setTopLeft(topLeft);
    selectionRect.setBottomRight(bottomRight);
    update(); // 修正後の選択範囲で画面を更新
}

void OverlayWidget::paintEvent(QPaintEvent *event) {
//    qDebug() << "OverlayWidget::paintEvent called";
    QWidget::paintEvent(event);
    QPainter painter(this);
    if (!selectionRect.isNull()) {
        QPen pen(Qt::yellow, 2, Qt::DashLine); // 枠線の設定
        painter.setPen(pen);

        // X, Y座標に沿って枠線の位置をずらすオフセット
        int offsetX = 40; // X方向のオフセット
        int offsetY = 30; // Y方向のオフセット

        // ずらした後の枠線の位置
        QRect translatedRect = selectionRect.translated(offsetX, offsetY);

        // X, Y方向に枠線を拡大縮小するオフセット
        int expandLeft = -0; // 左辺を左に15ピクセル拡大
        int expandTop = -0; // 上辺を上に20ピクセル拡大
        int expandRight = 0; // 右辺を右に15ピクセル拡大
        int expandBottom = 0; // 下辺を下に20ピクセル拡大

        // 拡大縮小を適用した選択範囲
        QRect adjustedRect = translatedRect.adjusted(expandLeft, expandTop, expandRight, expandBottom);

        painter.drawRect(adjustedRect); // 調整した選択範囲の枠線を描画
    }
}
