#include "overlaywidget.h"

#include <QPainter>
#include <QDebug>

OverlayWidget::OverlayWidget(QWidget *parent)
    : QWidget(parent),
    selectionRect(QRect())
{
    // ✅ マウスイベントを透過（下のlabel側/親側で処理するため）
    setAttribute(Qt::WA_TransparentForMouseEvents, true);

    // 透過表示（背景を塗らない）
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
}

void OverlayWidget::setSelectionRect(const QRect &rect)
{
    // ✅ 座標変換は一切しない（親がlabelなのでrectはlabel座標＝overlay座標）
    selectionRect = rect.normalized();
    update();
}

void OverlayWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (selectionRect.isNull() || !selectionRect.isValid()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(Qt::yellow, 2, Qt::DashLine);
    painter.setPen(pen);

    painter.drawRect(selectionRect);
}
