#include "adsbsorter.h"
#include "ui_adsbsorter.h"
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include "../zgTracker/landingpoint.h"
#include "../zgTracker/trackpoint.h"




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
    float   fileSize {float (100.0 / inFile.size())};

    quint32 time    {0};
    char    temp    {0};
    qint32  icao    {0};
    double  lat     {0};
    double  lon     {0};

    qint32  count   {0};

    QMap <qint32, AdsbTrack*> trackList; //icao, tracks
//    QList <QPair <qint32, AdsbTrack*> > trackList; //icao, tracks

    while (!inFile.atEnd()) {
        qDebug() << inFile.pos() * fileSize;
        inFile.read((char*)&time, sizeof(time));
        inFile.read((char*)&count, sizeof(count));
        for (; --count >= 0;)
        {
            inFile.read((char*)&icao, sizeof(icao));
            inFile.read((char*)&lat, sizeof(lat));
            inFile.read((char*)&lon, sizeof(lon));
        }
    }
    inFile.close();

/*
            QPair <double, double> *coords = new QPair <double, double> {lat, lon};
            adsbTrack->points.insert(&time, coords);
            ts >> temp;

  /*  qDebug() << trackList.count();
    // запись новых данных
    QMapIterator<qint32, AdsbTrack*> track(trackList);
    while (track.hasNext()) {
        track.next();
        // записать название трека и количество точек в нем
        outFile.write((char*)&(track.key()), sizeof(track.key()));
        qint32 pointsCout {track.value()->points.count()};
        outFile.write((char*)&pointsCout, sizeof(pointsCout));

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
*/
    outFile.close();
}





void AdsbSorter::on_pushButton_2_released()
{
    QString fileName = QFileDialog::getOpenFileName(0, "adsb file", QDir::current().absolutePath(), "*.adsb", 0);
    if (fileName == "") return;
}
