#include <iostream>
#include <QFile>
#include <fstream>
#include <vector>
#include <string>
#include <QDebug>
#include <QSignalMapper>
#include <QDateTime>
#include <QtWidgets>
#include "Function.h"

//#include <time.h>
using namespace std;

void merge_file_json(QString path){
    path.remove(".json");
    vector <QFile*> file;
    for(int i=0; QFile::exists(path+QString::number(i)+".json"); i++){
        file.push_back(new QFile);
        file.back()->setFileName(path+QString::number(i)+".json");
    }
    if(file.size()>1) {
        QFile outFile("//out_json.json");
        if(outFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Truncate)){
            for(int i=0; i<(int)file.size(); i++){

                qDebug()<<"file: "<<i+1<<" | "<<file.size();

                file[i]->open(QIODevice::ReadOnly);
                char a[1024];
                int nBlocksize, check = 0;
                while(!file[i]->atEnd()) {
                    nBlocksize = file[i]->read(a, sizeof(a));

                    if(check && !file[i]->atEnd()) { //середина файла
                        outFile.write(a, nBlocksize);
                    }
                    else {
                        bool buff = 0;
                        if(!check){                 //начало файла
                            if(i){ buff = true; }
                        }
                        if(file[i]->atEnd()) {      //конец файла
                            if( (i + 1) != (int)file.size() ){
                                while(a[nBlocksize - 1] != '}'){ nBlocksize-=1; }
                                a[nBlocksize-1] = ',';
                            }
                        }
                        outFile.write((a + buff), nBlocksize - buff);
                    }
                    check = 1;
                }
                file[i]->close();
            }
            outFile.close();
        }
    }
    else{
        qDebug()<<"Error\n";
    }

}

CastQAction::CastQAction(QModelIndex *index, QObject *parent):QAction(parent){
    m_index = index;
    connect(this, SIGNAL(triggered()), this, SLOT(trig()));
}
CastQAction::CastQAction(QModelIndex *index, const QString &text, QObject *parent):QAction(text,parent){
    m_index = index;
    connect(this, SIGNAL(triggered()), this, SLOT(trig()));
}
CastQAction::CastQAction(QModelIndex *index, const QIcon &icon, const QString &text, QObject *parent):QAction(icon,text,parent){
    m_index = index;
    connect(this, SIGNAL(triggered()), this, SLOT(trig()));
}
void CastQAction::trig(){
    emit triggered_m(*m_index);
}
bool CastQAction::event(QEvent *e){
    return e;
}

bool isNum(string n){//it is number?
    bool trigger = false;
    if(n.empty()) return 0;
    for(int i=0; n[i]!='\0'; i++){
        if((n[i]<'0')||(n[i]>'9')){
            if((n[i]!='.')&&(n[i]!='-')&&(n[i]!=',')){
                return 0;
            }
            if(((n[i]=='.'||n[i]==',')&&i==0)||trigger){
                return 0;
            }
            if((n[i]=='-')&&i){
                return 0;
            }
            if((n[i]=='.')||(n[i]==',')){
                trigger = true;
            }
        }
    }
    return 1;
}

void open_csv_device(QString patch, std::vector<device*> &_data){//проблема: не читает кириллицу из файлов в кодировке ASCI
    ifstream file(patch.toStdString());
    if(!file){
        QMessageBox::warning(0, "Error", "Error open file csv!") ;
        return;
    }
    _data.push_back(new device);
    string sbuff;
    getline(file, sbuff);
    _data.back()->name_device = sbuff.substr(sbuff.find(";")+1, sbuff.find(" (")-sbuff.find(";")-1);
    _data.back()->serial = sbuff.substr(sbuff.find("(")+1, (sbuff.find(")"))-sbuff.find("(")-1);
    qDebug()<<_data.back()->name_device.c_str();
    qDebug()<<_data.back()->serial.c_str();

    getline(file, sbuff);
    for(size_t i=sbuff.find(";")+1; i<sbuff.size(); i = sbuff.find(";", i)+1){//создание списка имён устройств
        _data.back()->name_ident_data.push_back(sbuff.substr(i, sbuff.find(";", i+1)-i));
        _data.back()->data_device.push_back(new std::vector<double>); //выделяем память под каждый параметер
        qDebug()<<_data.back()->name_ident_data.back().c_str();
    }
    while(!file.eof()){
        getline(file, sbuff);
        if(sbuff.empty()){break;}
        QDateTime* d = new QDateTime;
        _data.back()->date_time.push_back(d);
        d->setDate(QDate(std::stoi(sbuff.substr(0, 4)),std::stoi(sbuff.substr(5, 2)),std::stoi(sbuff.substr(8, 2))));
        d->setTime(QTime(std::stoi(sbuff.substr(11, 2)),std::stoi(sbuff.substr(14, 2)),std::stoi(sbuff.substr(17, 2))));
        string sbuff2;
        for(size_t k=0,i=20; i<sbuff.size(); i = sbuff.find(";",i)+1, k++){
            if(_data.back()->data_device[k]!=nullptr){//не совсем безопасный способ
                sbuff2 = sbuff.substr(i,sbuff.find(";",i)-i);

                if(isNum(sbuff2)){ //если строка является цифрой
                    _data.back()->data_device[k]->push_back(std::stod(sbuff2));
                }
                else if(sbuff2==""||sbuff2=="none"){
                    _data.back()->data_device[k]->push_back(0);
                }
                else{
                    delete _data.back()->data_device[k];
                    _data.back()->data_device[k] = nullptr;
                }
            }
        }
    }
    file.close();
}

device::~device()
{
    this->date_time.clear();
    this->name_ident_data.clear();
    for(size_t i = 0; i<data_device.size(); i++){
        if(data_device[i]!=nullptr){
            delete data_device[i];
        }
    }
}

int open_json_device(QString patch, std::vector<device *> &_data){
    QFile jsonFile(patch);
    if (!jsonFile.open(QIODevice::ReadOnly)) { return 0; }
    QString saveData = jsonFile.readAll();
    jsonFile.close();
    std::string patchName = patch.toStdString().substr(patch.toStdString().find_last_of('/')+1);
    QJsonDocument json = QJsonDocument::fromJson(saveData.toUtf8());
    QJsonObject jsonObj = json.object();
    int x = 0;
    bool flagFind;
    device *buffPoint;
    for(const auto& i : jsonObj.keys()){
        flagFind = false;
        QJsonValue value = jsonObj.value(QString(i));
        QJsonObject item = value.toObject();
        QString date = item.value(QString("Date")).toString();
        QString uName = item.value(QString("uName")).toString();
        QString serial = item.value(QString("serial")).toString()+" | "+patchName.c_str();
        item = item.value(QString("data")).toObject();
        for(int k=0; k<_data.size(); k++){
            if((_data[k]->name_device.c_str()==uName)&&(_data[k]->serial.c_str()==serial)){
                flagFind = true;
                buffPoint=_data[k];
            }
        }
        if(!flagFind){
            _data.push_back(new device);
            _data.back()->serial = serial.toStdString();
            _data.back()->name_device = uName.toStdString();
            buffPoint=_data.back();
            x++;
        }
        QDateTime* d = new QDateTime();
        buffPoint->date_time.push_back(d);
        d->setDate(QDate(std::stoi(date.toStdString().substr(0, 4)),std::stoi(date.toStdString().substr(5, 2)),std::stoi(date.toStdString().substr(8, 2))));
        d->setTime(QTime(std::stoi(date.toStdString().substr(11, 2)),std::stoi(date.toStdString().substr(14, 2)),std::stoi(date.toStdString().substr(17, 2))));
        int iter = 0;
        for(const auto& k : item.keys()){
            if(!flagFind){
                buffPoint->name_ident_data.push_back(k.toStdString());
                buffPoint->data_device.push_back(new vector <double>);
            }
            if(buffPoint->data_device[iter] != nullptr){
                if(isNum(item.value(k).toString().toStdString())){
                    buffPoint->data_device[iter]->push_back(std::stod(item.value(k).toString().toStdString()));
                }
                else if((item.value(k).toString()=="")||(item.value(k).toString()=="none")){
                    buffPoint->data_device[iter]->push_back(0);
                }
                else{
                    delete _data.back()->data_device[iter];
                    _data.back()->data_device[iter] = nullptr;
                }
            }
            iter++;
        }
    }
    return x;
}
