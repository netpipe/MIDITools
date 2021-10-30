#include "toucheventfilter.h"
#include <QDebug>


TouchEventFilter::TouchEventFilter()
{
}

bool TouchEventFilter::eventFilter(QObject *watched, QEvent *event){

    // Grab touch events and signal appropriately.
    //qDebug() << "event: " << event->type();
    //for desktop we have MouseEvents
    if (event->type() == QEvent::MouseButtonPress) {
        //qDebug() << "kikou event";

        QMouseEvent *te = static_cast<QMouseEvent*>(event);
        emit touchEventReceived(te->x(), te->y());

        //return false;
    } //mobile:
    else if (event->type() == QEvent::TouchBegin || event->type() == QEvent::TouchUpdate) {
        QTouchEvent *te = static_cast<QTouchEvent*>(event);
        foreach (QTouchEvent::TouchPoint tp, te->touchPoints()) {
            if (tp.state() == Qt::TouchPointPressed) {
                emit touchEventReceived(tp.screenPos().x(), tp.screenPos().y());
            }
        }
    }

    return QObject::eventFilter(watched, event);
}
