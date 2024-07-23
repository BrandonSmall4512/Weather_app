#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QtCharts/QChartView>
#include <QtWidgets/QRubberBand>

QT_USE_NAMESPACE

class ChartView : public QChartView
{
        Q_OBJECT
public:
    ChartView(QChart *chart, QWidget *parent = nullptr);
    ChartView(QWidget *parent = nullptr);
    ~ChartView()=default;
    void setChartAnimOpt(QChart *_chart, QChart::AnimationOptions options = QChart::SeriesAnimations);
protected:
    bool viewportEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
private:
    bool pressure;
    qreal mFactor=1.0;
};
#endif 

