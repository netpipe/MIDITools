#pragma once
#include <QEvent>
#include <QObject>
#include <QTouchEvent>

class TouchEventFilter : public QObject
{
    Q_OBJECT
public:
    TouchEventFilter();

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);

signals:
    void touchEventReceived(int x, int y);

};
