#include "castQChart.h"
#include <QtCore/QDateTime>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QXYSeries>


CastQChart::CastQChart(QGraphicsItem *parent, Qt::WindowFlags wFlags):QChart(parent, wFlags){
    maxX = minX = minY = maxY = 0;
    axisX = axisY = nullptr;
    setContentsMargins(-11,-11,-11,-11);
    setMargins(QMargins(4,12,8,4));
    legend()->detachFromChart();
    legend()->setBackgroundVisible(true);
    legend()->setMaximumSize(250,29);
    legend()->setGeometry(QRectF(4,4,250,29));
    legend()->setAlignment(Qt::AlignLeft);
    legend()->setMarkerShape(QLegend::MarkerShapeCircle);
    legend()->setBrush(QBrush(QColor(1, 1, 1, 110)));
    legend()->setPen(QPen(QColor(192, 192, 192, 192)));
    legend()->setShowToolTips(true);
    legend()->update();
}

void CastQChart::castAddAxis(QAbstractAxis *axis, Qt::Alignment align){
    if(align == Qt::AlignBottom){axisX = axis;}
    else if(align == Qt::AlignLeft){axisY = axis;}
    addAxis(axis, align);
}

void CastQChart::castAddSeries(QAbstractSeries *_series) {
    QList<QPointF> graph = static_cast<QXYSeries*>(_series)->points();
    if(series().size()&&(!graph.isEmpty())){ //подгон масштаба графиков
        maxX = graph.last().x();
        minX = graph.first().x();
        for(qsizetype i=0; i < graph.size(); i++){
            if(graph[i].y() > static_cast<QValueAxis*>(axisY)->max()){maxY = graph[i].y();}
            else if(graph[i].y() < static_cast<QValueAxis*>(axisY)->min()){minY = graph[i].y();}
        }
        QDateTime d;
        d.setMSecsSinceEpoch(maxX);
        if(d > static_cast<QDateTimeAxis*>(axisX)->max()){ static_cast<QDateTimeAxis*>(axisX)->setMax(d); }
        d.setMSecsSinceEpoch(minX);
        if(d < static_cast<QDateTimeAxis*>(axisX)->min()){ static_cast<QDateTimeAxis*>(axisX)->setMin(d); }
        if(maxY > static_cast<QValueAxis*>(axisY)->max()){ static_cast<QValueAxis*>(axisY)->setMax(maxY); }
        if(minY < static_cast<QValueAxis*>(axisY)->min()){ static_cast<QValueAxis*>(axisY)->setMin(maxY); }
    }
    else if(!series().size()&&(!graph.isEmpty())){//подгон масштаба графиков если окно было очищено
        minY = maxY = 0.0;
        QList<QPointF> graph = static_cast<QXYSeries*>(_series)->points();
        maxX = graph.last().x();
        minX = graph.first().x();
        for(qsizetype i=0; i < graph.size(); i++){
            if(graph[i].y() > static_cast<QValueAxis*>(axisY)->max()){maxY = graph[i].y();}
            if(graph[i].y() < static_cast<QValueAxis*>(axisY)->min()){minY = graph[i].y();}
        }
        QDateTime d;
        d.setMSecsSinceEpoch(maxX);
        static_cast<QDateTimeAxis*>(axisX)->setMax(d);
        d.setMSecsSinceEpoch(minX);
        static_cast<QDateTimeAxis*>(axisX)->setMin(d);
        static_cast<QValueAxis*>(axisY)->setMax(maxY);
        static_cast<QValueAxis*>(axisY)->setMin(maxY);
    }
    addSeries(_series); //добавление серии
    _series->attachAxis(axisX);
    _series->attachAxis(axisY);
    legend()->update();
}

void settingsQChartWindow2(QChart *_chart){
    _chart->legend()->setMarkerShape(QLegend::MarkerShapeCircle);
    _chart->legend()->detachFromChart();
    _chart->legend()->setMaximumSize(300, 50);
    _chart->legend()->setMinimumSize(300, 50);
    _chart->legend()->setPos(0,0);
    _chart->addAxis(new QValueAxis, Qt::AlignBottom);
    _chart->addAxis(new QValueAxis, Qt::AlignLeft);
    static_cast<QValueAxis*>(_chart->axes().at(0))->setTickCount(20);
    static_cast<QValueAxis*>(_chart->axes().at(0))->setRange(0,1);
    _chart->axes().at(0)->setLabelsVisible(0);
    static_cast<QValueAxis*>(_chart->axes().at(1))->setTickCount(5);
    static_cast<QValueAxis*>(_chart->axes().at(1))->setRange(0,1);
    _chart->axes().at(1)->setLabelsVisible(0);
    _chart->setContentsMargins(-9,-9,-9,-9);
    _chart->setBackgroundRoundness(0);
    _chart->setMargins(QMargins(-20,-1,-6,-14));
}
