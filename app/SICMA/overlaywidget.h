// OverlayWidget.h
#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QRect>

class OverlayWidget : public QWidget {
    Q_OBJECT

public:
    explicit OverlayWidget(QWidget *parent = nullptr);
    void setSelectionRect(const QRect &rect);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QRect selectionRect;
};

#endif // OVERLAYWIDGET_H
