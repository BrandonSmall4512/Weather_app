#ifndef FUNCTION_H
#define FUNCTION_H

#endif // FUNCTION_H
#include <QString>
#include <QTreeView>
#include <QAction>
#include <vector>


void merge_file_json(QString path = "log.json");

class CastQAction: public QAction{
    Q_OBJECT
private:
    QModelIndex *m_index;
public:
    CastQAction(QModelIndex *index, QObject *parent = nullptr);
    CastQAction(QModelIndex *index, const QString &text, QObject *parent = nullptr);
    CastQAction(QModelIndex *index, const QIcon &icon, const QString &text, QObject *parent = nullptr);
    ~CastQAction()=default;
private slots:
    void trig();
signals:
    void triggered_m(const QModelIndex &index);
protected:
    bool event(QEvent *e);
};

struct device{
    int check_ident = false; //флаг сигнализирующий, что нужно перепроверить строку/строки данных (если в самой первой итерации встретился неопр. символ)
    std::string name_device, serial;
    std::vector <QDateTime*> date_time;
    std::vector <std::string> name_ident_data;//имена опознанных данных
    std::vector <std::vector<double>*> data_device;//опознанные/неопознанные данные

    ~device();
};

void open_csv_device(QString patch, std::vector<device*>&_data);

int open_json_device(QString patch, std::vector<device*>&_data);
