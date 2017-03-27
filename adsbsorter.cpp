#include "adsbsorter.h"
#include "ui_adsbsorter.h"
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QDebug>




AdsbSorter::AdsbSorter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdsbSorter)
{
    ui->setupUi(this);
}




AdsbSorter::~AdsbSorter()
{
    delete ui;
}




void AdsbSorter::on_pushButton_released()
{
    QString fileName = QFileDialog::getOpenFileName(0, "adsb file", QDir::current().absolutePath(), "*.adsb", 0);
    if (fileName == "") return;

    // проверки
    QFile inFile(fileName);
    if (!inFile.open(QIODevice::ReadOnly))
    {
        qDebug () << "error opening file " << fileName;
        return;
    }

    QFile outFile(fileName + "sorted");
    if (!outFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "error opening file " << fileName + "sorted";
        inFile.close();
        return;
    }

    //* работа
    // чтение старых данных
    float fileSize {float (100.0 / inFile.size())};

    QByteArray ba = inFile.readAll();
    inFile.close();
    QTextStream ts(&ba);

    quint32 time {0};
    char temp {0};
    qint32 icao {0};
    double lat {0}, lon{0};

    QMap <qint32, AdsbTrack*> trackList; //icao, tracks
    QVector <quint32> timeList;

    while (!ts.atEnd())
    {
        qDebug() << ts.pos() * fileSize;

        ts >> time;
        timeList.append(time);

        while (!ts.atEnd())
        {
            ts >> temp;
            if (temp == char(10)) break; // конец строки

            ts >> icao;

            AdsbTrack* adsbTrack {NULL};
            if (trackList.keys().indexOf(icao) <= 0)
            {
                adsbTrack = new AdsbTrack(icao);
                trackList.insert(icao, adsbTrack);
            }
            else
            {
                adsbTrack = trackList[icao];
            }

            ts >> temp;
            ts >> lat;
            ts >> temp;
            ts >> lon;
            QPair <double, double> *coords = new QPair <double, double> {lat, lon};
            adsbTrack->points.insert(&time, coords);
        }
    }
    ba.clear();

    // запись новых данных
    QMapIterator<qint32, AdsbTrack*> track(trackList);
    qDebug() << trackList.count();
//    QDataStream ds(&outFile);
    while (track.hasNext()) {
        track.next();
        // записать название трека
        outFile.write((char*)&(track.key()), sizeof(track.key()));
        // записать количество точек в треке
        outFile.write((char*)&(track.value()->points.count()), sizeof(track.value()->points.count()));
        qDebug() << track.value()->points.count();
        QMapIterator<quint32*, QPair <double, double>* > point(track.value()->points);
        while (point.hasNext()) {
            point.next();
            // записать время, lat, lon
            outFile.write((char*)&(point.key()), sizeof(point.key()));
            outFile.write((char*)&(point.value()->first), sizeof(point.value()->first));
            outFile.write((char*)&(point.value()->second), sizeof(point.value()->second));
        }
    }

    outFile.close();
}




