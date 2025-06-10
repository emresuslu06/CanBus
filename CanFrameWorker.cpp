#include "CanFrameWorker.h"
#include <QCanBusFrame>
#include <QString>

CanFrameWorker::CanFrameWorker(QObject *parent) : QObject(parent)
{
}

void CanFrameWorker::processData(const QCanBusFrame &frame)
{
    // Burada işleme işlemini yapacağız
    QString frameData;
    for (int i = 0; i < frame.payload().size(); ++i) {
        frameData.append(QString::asprintf("%02X ", static_cast<unsigned char>(frame.payload().at(i))));
    }
    frameData = frameData.trimmed();
    QString frameId = QString::asprintf("0x%X", frame.frameId());

    QString formattedData = QString("ID: %2, Data: %3").arg(frameId).arg(frameData);
    emit frameProcessed(formattedData);  // UI'yi bilgilendirmek için sinyal gönderiyoruz
}
