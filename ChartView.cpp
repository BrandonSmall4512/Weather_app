#include "ChartView.h"
#include <QtGui/QMouseEvent>
#include <QWheelEvent>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QDate>

ChartView::ChartView(QChart *chart, QWidget *parent): QChartView(chart, parent){
    setRubberBand(QChartView::HorizontalRubberBand);
    chart->QChart::setBackgroundRoundness(0);
    chart->setAnimationOptions(QChart::AllAnimations);
}

ChartView::ChartView(QWidget *parent): QChartView(parent){
    setRubberBand(QChartView::HorizontalRubberBand);
}

void ChartView::setChartAnimOpt(QChart *_chart, QChart::AnimationOptions options){
    _chart->setAnimationOptions(options);
    setChart(_chart);
    setBackgroundBrush(Qt::white);
}

bool ChartView::viewportEvent(QEvent *event){
    if (event->type() == QEvent::TouchBegin) {
    }
    return QChartView::viewportEvent(event);
}

void ChartView::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::RightButton){
    }
    else{
        mFactor=1;
        QChartView::mousePressEvent(event);
    }
}

void ChartView::mouseMoveEvent(QMouseEvent *event){

    QChartView::mouseMoveEvent(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::RightButton){
        mFactor=1;
        chart()->zoomReset();
    }
    else{
        QChartView::mouseReleaseEvent(event);
    }
}

void ChartView::keyPressEvent(QKeyEvent *event){
    switch (event->key()) {
    case Qt::Key_Left:
        chart()->scroll(-20, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(20, 0);
        break;
//    case Qt::Key_Up:
//        chart()->scroll(0, 20);
//        break;
//    case Qt::Key_Down:
//        chart()->scroll(0, -20);
//        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}

void ChartView::wheelEvent(QWheelEvent *event){//скролл
        chart()->zoomReset();
        if(((mFactor<=300000) && (mFactor>=0.0003))||((mFactor>300000) && (event->angleDelta().y() > 0))||((mFactor<0.0003) && (event->angleDelta().y() < 0))){
            mFactor *= event->angleDelta().y() > 0 ? 0.5 : 2;
        }
        QRectF rect = chart()->plotArea();
        qDebug()<<mFactor;
        QPointF c = chart()->plotArea().center();
        rect.setWidth(mFactor*rect.width());
        rect.moveCenter(c);
        chart()->zoomIn(rect);
}
