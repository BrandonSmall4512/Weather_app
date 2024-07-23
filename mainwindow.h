#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QStandardItemModel>
#include "Function.h"
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QtCore/QDateTime>
#include <QtCharts/QDateTimeAxis>
#include "ChartView.h"
#include "castQChart.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QAbstractSeries* newSeries(std::vector <QDateTime*> &date_time, std::vector<double> *_data, std::string name, QAbstractSeries::SeriesType type = QAbstractSeries::SeriesType::SeriesTypeLine);
    void createThreeItem(device *dev);
private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    enum item_type{GRAPH, DEVAICE, UNIDENTIFIED};
    QDateTimeAxis *axisX;//оси
    QValueAxis *axisY;//оси
    CastQChart *chart;
    QChart *chart2; //отвечает за окно второго графика

    std::vector<device*> data; //основная память графиков

    QImage pc_icon;
    QImage graph_icon;
    QImage none_icon;
private slots:
    void treeClick(const QModelIndex &index);
    void treeDoubleClick(const QModelIndex &index);
    void open_json();
    void open_cvs();
    void open_network();
    void clear_the_charts();
    void add_graph_internet();
    void about();
    void delete_strok(const QModelIndex&);
    void onCustomContextMenu(const QPoint &pos);
};
#endif // MAINWINDOW_H
