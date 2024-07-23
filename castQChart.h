#ifndef CASTQCHART_H
#define CASTQCHART_H
#include <QtCharts/QChart>

class CastQChart:public QChart{
    Q_OBJECT
public:
    CastQChart(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = Qt::WindowFlags());

    void castAddSeries(QAbstractSeries* _series);

    void castAddAxis(QAbstractAxis* axis, Qt::Alignment align);
private:
    qint64 maxX, minX;
    double maxY, minY;
    QAbstractAxis *axisX, *axisY;
};

void settingsQChartWindow2(QChart *_chart);

#endif // CASTQCHART_H
