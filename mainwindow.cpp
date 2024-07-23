#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QMessageBox>
#include <QPointF>



MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){

    setWindowIcon(QIcon(":/Resurses/icon.png"));
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    ui->setupUi(this);
    setWindowTitle("QGraph");

    //загрузка иконок
    pc_icon = QImage(":/Resurses/3.png").scaled(QSize(25, 25));
    graph_icon = QImage(":/Resurses/4.png").scaled(QSize(25, 25));
    none_icon = QImage(":/Resurses/2.png").scaled(QSize(25, 25));

    axisX = new QDateTimeAxis;
    axisY = new QValueAxis;
    chart = new CastQChart();
    chart->castAddAxis(axisX, Qt::AlignBottom);
    chart->castAddAxis(axisY, Qt::AlignLeft);
    chart->setTitle("Chart");
    chart->legend()->layout()->setContentsMargins(5,2,1,1);

    //настройка осей
    axisX->setTickCount(10);
    axisX->setFormat("yy.MM.dd");
    axisX->setTitleText("Date");
    QDateTime* momentInTime = new QDateTime;
    momentInTime->setDate(QDate(2021,12,31));
    axisX->setMax(*momentInTime); //максимальная дата
    momentInTime->setDate(QDate(2021,1,1));
    axisX->setMin(*momentInTime); //минимальная дата

    axisY->setLabelFormat("%g");
    axisY->setTitleText("axis Y");
    axisY->setMax(20);
    axisY->setMin(0);
    axisY->setMinorGridLineVisible(1);
    axisY->setMinorTickCount(3);

    ui->graphicsView->setChartAnimOpt(chart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    chart2 = new QChart;
    settingsQChartWindow2(chart2);
    ui->graphicsView_2->setChart(chart2);
    ui->graphicsView_2->setRenderHint(QPainter::Antialiasing);

    model = new QStandardItemModel(0,2);//создаём модель и корневой элемент
    model->setHeaderData(0, Qt::Horizontal, tr("Program"));
    model->setHeaderData(1, Qt::Horizontal, tr("Type"));
    ui->treeView->setAllColumnsShowFocus(true);
    ui->treeView->setAlternatingRowColors(true);
    ui->treeView->setModel(model);

    connect(ui->actionOpen_JSON, &QAction::triggered, this, &MainWindow::open_json);
    connect(ui->actionOpen_CVS, &QAction::triggered, this, &MainWindow::open_cvs);
    connect(ui->actionOpen_from_network, &QAction::triggered, this, &MainWindow::open_network);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionClear_the_charts, &QAction::triggered, this, &MainWindow::clear_the_charts);
    connect(ui->actionAdd_a_graph_from_the_internet, &QAction::triggered, this, &MainWindow::add_graph_internet);
    connect(ui->actionAbout_the_program, &QAction::triggered, this, &MainWindow::about);
    connect(ui->treeView, &QTreeView::doubleClicked, this, &MainWindow::treeDoubleClick);
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::treeClick);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu); //включаем кастомное контекстное меню
    connect(ui->treeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));

}

QAbstractSeries* MainWindow::newSeries(std::vector<QDateTime*> &date_time, std::vector<double> *_data, std::string name, QAbstractSeries::SeriesType type){
    QAbstractSeries *series;
    if(type == QAbstractSeries::SeriesType::SeriesTypeBar){
        series = new QBarSeries();
        QBarSet *set = new QBarSet(QString::fromStdString(name));
        for(size_t i=0;i<date_time.size();i++){ set->append(_data->at(i)); }
        static_cast<QBarSeries*>(series)->append(set);
    }
    else{
        series = new QLineSeries();
        series->setName(QString::fromStdString(name));
        for(size_t i=0;i<date_time.size();i++){
            static_cast<QXYSeries*>(series)->append(date_time.at(i)->toMSecsSinceEpoch(), _data->at(i));
        }
    }
    return series;
}

void MainWindow::createThreeItem(device *dev){
    QStandardItem *parentItem = model->invisibleRootItem();

    QStandardItem *item = new QStandardItem(QString("%0 (%1)").arg(dev->name_device.c_str()).arg(dev->serial.c_str()));
    QStandardItem *item2 = new QStandardItem(QString::fromUtf8(""));

    item->setData(QVariant(QPixmap::fromImage(pc_icon)), Qt::DecorationRole);   //добавление иконки

    item->setData(item_type::DEVAICE, Qt::UserRole+1);                          //нужно для определения нужного контекстного меню
    item2->setData(item->data(), Qt::UserRole+1);                               //нужно чтобы контекстного меню на этом элементе было такое же как на первом

    parentItem->appendRow(QList<QStandardItem*>() << item << item2);
    parentItem = item;
    for (size_t i = 0; i < dev->name_ident_data.size(); i++) {
        item = new QStandardItem(dev->name_ident_data[i].c_str());
        if(dev->data_device[i]!=nullptr){
            item->setData(QVariant(QPixmap::fromImage(graph_icon)), Qt::DecorationRole);
            item2 = new QStandardItem(QString(""));
            item->setData(item_type::GRAPH, Qt::UserRole+1);                    //нужно для определения нужного контекстного меню
        }
        else{
            item->setData(QVariant(QPixmap::fromImage(none_icon)), Qt::DecorationRole);
            item2 = new QStandardItem(QString(""));
            item->setData(item_type::UNIDENTIFIED, Qt::UserRole+1);             //нужно для определения нужного контекстного меню
        }
        item2->setData(item->data(), Qt::UserRole+1);                           //нужно чтобы контекстного меню на этом элементе было такое же как на первом
        parentItem->appendRow(QList<QStandardItem*>() << item << item2);
    }
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::treeClick(const QModelIndex &index){
    ui->graphicsView_2->chart()->removeAllSeries();
    if(index.data(Qt::UserRole + 1).toInt() == item_type::GRAPH){
        chart2->addSeries(newSeries(data[index.parent().row()]->date_time, data[index.parent().row()]->data_device[index.row()], data[index.parent().row()]->name_ident_data[index.row()]));
    }
}

void MainWindow::treeDoubleClick(const QModelIndex &index){
    qDebug()<<index;
    if(index.data(Qt::UserRole + 1).toInt() == item_type::GRAPH){
        qDebug()<<"fff "<<data[index.parent().row()]->date_time.front();
        chart->castAddSeries(newSeries(data[index.parent().row()]->date_time, data[index.parent().row()]->data_device[index.row()], data[index.parent().row()]->name_ident_data[index.row()]));
    }
}

void MainWindow::open_json(){
    qDebug()<<"open_json";
    QString fileName = QFileDialog::getOpenFileName(this, tr("OpenFile"),"",tr("JSON (*.json)"));
    if(!fileName.isNull()){
        size_t i = open_json_device(fileName, data);
        for(i = data.size() - i; i < data.size(); i++){
            createThreeItem(data[i]);
        }
    }
}

void MainWindow::open_cvs(){
    qDebug()<<"open_cvs";
    QString fileName = QFileDialog::getOpenFileName(this, tr("OpenFile"),"",tr("CSV (*.csv)"));
    if(!fileName.isNull()){
        open_csv_device(fileName, data);
        createThreeItem(data.back());
    }
}

void MainWindow::open_network(){
    qDebug()<<"open_network";
}

void MainWindow::clear_the_charts(){
    qDebug()<<"clear_the_charts";
    ui->graphicsView->chart()->removeAllSeries();
}

void MainWindow::add_graph_internet(){
    qDebug()<<"add_graph_internet";
}

void MainWindow::about(){
    qDebug()<<"about";
    QMessageBox::about(0, "About", "Name: QGraph\n\nVersion: 1.0.0\n\nAutor: Zamaletdinov M. K.");
}

void MainWindow::delete_strok(const QModelIndex &index){
    if(index.data(Qt::UserRole + 1).toInt() == item_type::DEVAICE){
        ui->graphicsView_2->chart()->removeAllSeries();
        qDebug()<<"delete DEVAICE"<<index.row();
        delete data[index.row()];
        data.erase(data.begin() + index.row());
    }
    else{
        qDebug()<<"delete "<<index.row();
        data[ index.parent().row() ]->data_device.erase( data[ index.parent().row() ]->data_device.begin() + index.row() );
        qDebug()<<"delete data_device";
        data[ index.parent().row() ]->name_ident_data.erase(data[ index.parent().row() ]->name_ident_data.begin() + index.row() );
        qDebug()<<"delete name_ident_data";
    }
    qDebug()<<"delete item";
    qDebug()<<ui->treeView->model()->removeRow((index).row(), index.parent());
}


void MainWindow::onCustomContextMenu(const QPoint &point){
   QModelIndex index = ui->treeView->indexAt(point);
   if (index.isValid()) {
        QMenu menu(this);
        CastQAction* action_del = new CastQAction(&index, QString("Delete"), ui->treeView);
        CastQAction* action_1 = new CastQAction(&index, QString("Add with averaging"), ui->treeView);//добавить с осреднением
        CastQAction* action_2 = new CastQAction(&index, QString("Add all"), ui->treeView);
        connect(action_del, &CastQAction::triggered_m, this, &MainWindow::delete_strok);
        menu.addAction(action_del);
        qDebug()<<model->rowCount(index);
        if(index.data(Qt::UserRole+1).toInt()==item_type::GRAPH){
//            connect(action_1, &CastQAction::triggered_m, this, &MainWindow::test);
            menu.addAction(action_1);
        }
        if(index.data(Qt::UserRole+1).toInt()==item_type::DEVAICE){
//            connect(action_2, &CastQAction::triggered_m, this, &MainWindow::test);
            menu.addAction(action_2);
        }
        menu.exec(ui->treeView->viewport()->mapToGlobal(point));;
    }
}
