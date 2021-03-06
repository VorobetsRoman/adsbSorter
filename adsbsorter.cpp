#include "adsbsorter.h"
#include "ui_adsbsorter.h"
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QSettings>



//===========================================
AdsbSorter::AdsbSorter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdsbSorter)
{
    ui->setupUi(this);
    loadSettings();
}




//===========================================
void AdsbSorter::loadSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "VRoman", "adsbSorter");

    if (!settings.contains("minTime")){
        settings.setValue("minTime", "2016.06.01::00:00:00");
    }
    ui->dteTimeMin->setDateTime(QDateTime::fromString(settings.value("minTime").toString(), "YYYY.MM.dd::hh:mm:ss"));

    if (!settings.contains("maxTime")){
        settings.setValue("maxTime", QDateTime::currentDateTime().toString("YYYY.MM.dd::hh:mm:ss"));
    }
        ui->dteTimeMax->setDateTime(QDateTime::fromString(settings.value("minTime").toString(), "YYYY.MM.dd::hh:mm:ss"));

    if (!settings.contains("minLat")){
        settings.setValue("minLat", 0);
    }
        ui->leLatMin->setText(QString::number(settings.value("minLat").toFloat()));

    if (!settings.contains("maxLat")){
        settings.setValue("maxLat", 180);
    }
        ui->leLatMax->setText(QString::number(settings.value("maxLat").toFloat()));

    if (!settings.contains("minLon")){
        settings.setValue("minLon", 0);
    }
        ui->leLonMin->setText(QString::number(settings.value("minLon").toFloat()));

    if (!settings.contains("maxLon")){
        settings.setValue("maxLon", 180);
    }
        ui->leLonMax->setText(QString::number(settings.value("maxLon").toFloat()));
}




//===========================================
AdsbSorter::~AdsbSorter()
{
    delete ui;
}




//===========================================
void AdsbSorter::on_pbOpen_released()
{
    inFileName = QFileDialog::getOpenFileName(0, "adsb file", QDir::current().absolutePath(), "*.adsb", 0);
    ui->lbFileName->setText(inFileName.section("/", -1, -1));
}




//===========================================
void AdsbSorter::on_pbStart_released()
{
    // проверки
    if (inFileName == "") return;

    QFile inFile(inFileName);
    if (!inFile.open(QIODevice::ReadOnly))
    {
        qDebug () << "error opening file " << inFileName;
        return;
    }


    QFile outFile(inFileName + "sorted");
    if (!outFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "error opening file " << inFileName + "sorted";
        inFile.close();
        return;
    }

    ui->lbProcessName->setText("Проверка времени");
    // получение ограничений на данные
    quint32 timeMin = ui->dteTimeMin->dateTime().toTime_t();
    quint32 timeMax = ui->dteTimeMax->dateTime().toTime_t();

    //* работа
    // чтение старых данных
    float   fileSize {float (100.0 / inFile.size())};

    quint32 time    {0};
    qint32  icao    {0};
    double  lat     {0};
    double  lon     {0};

    qint32  count   {0};

    QMap <qint32, AdsbTrack*> trackList; //icao, tracks

    while (!inFile.atEnd()) {
        ui->progressBar->setValue(int (inFile.pos() * fileSize) + 1);
        inFile.read((char*)&time, sizeof(time));

        inFile.read((char*)&count, sizeof(count));
        for (; --count >= 0;)
        {
            inFile.read((char*)&icao, sizeof(icao));
            inFile.read((char*)&lat, sizeof(lat));
            inFile.read((char*)&lon, sizeof(lon));

            if (time > timeMin && time < timeMax)
            {
                AdsbTrack *adsbTrack = trackList[icao];
                if (!adsbTrack) {
                    adsbTrack = new AdsbTrack(icao);
                    trackList.insert(icao, adsbTrack);
                }
                QPair <double, double>* coords = new QPair <double, double> {lat, lon};
                adsbTrack->points.insert(time, coords);
            }
        }
    }
    qDebug() << trackList.count();
    inFile.close();

    sortData(&trackList);
    writeOutFile(&outFile, &trackList);
    outFile.close();

    ui->lbProcessName->setText("Очистка");
    float trackListCount {float(100.0 / trackList.count())};
    int trackListPosition {0};

    for (AdsbTrack *adsbTrack : trackList)
    {
        trackListPosition++;
        ui->progressBar->setValue(int(trackListPosition * trackListCount) + 1);
        for (QPair <double, double>* points : adsbTrack->points) {
            delete points;
        }
        adsbTrack->points.clear();
        delete adsbTrack;
    }
    trackList.clear();

    ui->lbProcessName->setText("Готов");
    ui->progressBar->setValue(0);
}




//===========================================
void AdsbSorter::sortData(QMap<qint32, AdsbTrack *> *trackList)
{
    ui->lbProcessName->setText("Проверка координат");
    bool okMin {false};
    bool okMax {false};
    float latMin = ui->leLatMin->text().toFloat(&okMin);
    float latMax = ui->leLatMax->text().toFloat(&okMax);
    bool latOk {okMin && okMax && (latMin < latMax)};

    float lonMin = ui->leLonMin->text().toFloat(&okMin);
    float lonMax = ui->leLonMax->text().toFloat(&okMax);
    bool lonOk {okMin && okMax && (lonMin < lonMax)};

    float trackListCount {float(100.0 / trackList->count())};
    int trackListPosition {0};
    if (latOk || lonOk) {
        for (AdsbTrack* adsbTrack : *trackList)
        {
            trackListPosition++;
            ui->progressBar->setValue(int(trackListPosition * trackListCount));

            bool latContains {false};
            bool lonContains {false};
            for (QPair <double, double> *coords : adsbTrack->points)
            {
                if (latOk && coords->first > latMin && coords->first < latMax) {
                    latContains = true;
                }
                if (lonOk && coords->second > lonMin && coords->second < lonMax) {
                    lonContains = true;
                }
            }
            if (latOk && !latContains) {
                    trackList->remove(trackList->key(adsbTrack));
            }
            else if (lonOk && !lonContains) {
                trackList->remove(trackList->key(adsbTrack));
            }
        }
    }
    qDebug() << trackList->count();

}




//===========================================
void AdsbSorter::writeOutFile(QFile *outFile, QMap <qint32, AdsbTrack*> *trackList)
{

    ui->lbProcessName->setText("Сохранение");

    // Пишем количество треков
    qint32 trackCount {trackList->count()};
    outFile->write((char*)&trackCount, sizeof(trackCount));

    float trackListCount {float(100.0 / trackList->count())};
    int trackListPosition {0};

    // Пишем сами треки
    for (AdsbTrack *adsbTrack : *trackList)
    {
        trackListPosition++;
        ui->progressBar->setValue(int(trackListPosition * trackListCount));

        // Пишем идентификатор трека
        outFile->write((char*)&(adsbTrack->icaoName), sizeof(adsbTrack->icaoName));
        qint32 pointsCount = adsbTrack->points.count();
        // Пишем количество точек в треке
        outFile->write((char*)&(pointsCount), sizeof(pointsCount));
        // Пишем точки из трека
        QMapIterator <quint32, QPair <double, double>* > trackPoint (adsbTrack->points);
        while (trackPoint.hasNext())
        {
            trackPoint.next();
            outFile->write((char*)&trackPoint.key(), sizeof(trackPoint.key()));
            outFile->write((char*)&trackPoint.value()->first, sizeof(trackPoint.value()->first));
            outFile->write((char*)&trackPoint.value()->second, sizeof(trackPoint.value()->second));
        }
    }
}




//===========================================
