#include "CANDataModel.h"
#include <QCanBusFrame>
#include <QDebug>
#include <QPointF>
#include <QByteArray>
#include "fileRead.h"

CANDataModel::CANDataModel(QObject *parent) : QAbstractListModel(parent),
    m_connectionStatus(""),
    currentIndex(0),
    isFirstFrame(true),
    totalcountTime(0)
{

     fileRead fr;

    QList<QString> lines = fr.readFile();
    idToData = fr.parseLines(lines);  // idToData kullanıyoruz

     qDebug() << "idToData içeriği CANDATAMODEL fonksiyonundan: " << idToData;


   // QMap<QString, QList<QList<int>>> idToBytes = fr.parseLines(lines);



    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CANDataModel::addcountTime);
    timer->start(1);
}

void CANDataModel::setConnectionStatus(const QString &status)
{
    if (m_connectionStatus != status) {
        m_connectionStatus = status;
        emit connectionStatusChanged();
        qDebug() << "Bağlantı durumu güncellendi:" << m_connectionStatus;
    }
}

int CANDataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return dataPoints.size();
}

QVariant CANDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= dataPoints.size())
        return QVariant();

    const QPointF &point = dataPoints[index.row()];

    switch (role) {
    case XRole:
        return point.x();
    case YRole:
        return point.y();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> CANDataModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[XRole] = "xValue";
    roles[YRole] = "yValue";
    return roles;
}

void CANDataModel::addCanFrame(const QCanBusFrame &frame)
{
    QString frameData;
    for (int i = 0; i < frame.payload().size(); ++i) {
        frameData.append(QString::asprintf("%02X ", static_cast<unsigned char>(frame.payload().at(i))));
    }
    frameData = frameData.trimmed();
    QString frameId = QString::asprintf("0x%X", frame.frameId());
    frameId = frameId.toUpper();

    if (isFirstFrame) {
        totalcountTime = 0;
        isFirstFrame = false;
    }

    qDebug() << idToData.contains(frameId);
    qDebug() << "idToData içeriği CONTAİNSDEN ÖNCE: " << idToData;


    if (idToData.contains(frameId)) {
        QPair<QList<int>, QList<QPair<int, int>>> byteAndBits = idToData.value(frameId);
        QList<int> byteGroup = byteAndBits.first;
        QList<QPair<int, int>> bitList = byteAndBits.second;
        qDebug() << "Processing frameId:" << frameId;

        // --- BYTE GRUBU OKUMA ---
        foreach (int byteIndex, byteGroup) {
            int actualByteIndex = byteIndex - 1;  // 0-based index
            unsigned char byteVal = frame.payload().at(actualByteIndex);
            int combinedValue = static_cast<int>(byteVal);
            QString key = QString("%1_group_%2").arg(frameId).arg(byteIndex);
            QPointF point(static_cast<float>(totalcountTime), combinedValue);
            QVector<QPointF> &dataPoints = idByteDataMap[key];
            if (dataPoints.size() >= 50)
                dataPoints.removeFirst();
            dataPoints.append(point);

            int extractedValue = combinedValue;

            qDebug() << "Sending data point with key:" << key << "value:" << combinedValue << "and point:" << point;
            emit dataPointAdded(key, point , extractedValue);

        }

        // ---BİT GRUBU OKUMA ---
        for (const QPair<int, int>& bitInfo : bitList) {
            int startBit = bitInfo.first;
            int length = bitInfo.second;

            int startByteIndex = startBit / 8;
            int endByteIndex = (startBit + length - 1) / 8;

            quint32 extractedValue = 0;

            qDebug() << "Start bit:" << startBit << "Length:" << length;

            if (startByteIndex == endByteIndex) {

                quint8 byte = static_cast<quint8>(frame.payload().at(startByteIndex));
                int rightShift = startBit % 8;
                int mask = ((1 << length) - 1) << rightShift;
                extractedValue = (byte & mask) >> rightShift;

                qDebug() << "LSB Tek byte. Byte:" << byte << " Mask:" << mask << " Shift:" << rightShift;
            } else {

                int remainingBits = length;
                int currentBit = startBit;

                while (remainingBits > 0) {
                    int byteIndex = currentBit / 8;
                    int bitPosInByte = currentBit % 8;

                    int bitsToRead = qMin(8 - bitPosInByte, remainingBits);

                    quint8 byte = static_cast<quint8>(frame.payload().at(byteIndex));

                    int shift = bitPosInByte;
                    quint32 bits = (byte >> shift) & ((1 << bitsToRead) - 1);


                    extractedValue |= (bits << (length - remainingBits));

                    qDebug() << "LSB Çok byte. Byte:" << byte << " Shift:" << shift << " Bits to read:" << bitsToRead << " Extracted bits:" << bits;

                    currentBit += bitsToRead;
                    remainingBits -= bitsToRead;
                }
            }

            qDebug() << "Extracted value (decimal):" << extractedValue;

            QString key = QString("%1_bit_%2_%3").arg(frameId).arg(startBit).arg(length);
            QPointF point(static_cast<float>(totalcountTime), static_cast<float>(extractedValue));
            QVector<QPointF> &dataPoints = idByteDataMap[key];

            if (dataPoints.size() >= 50)
                dataPoints.removeFirst();
            dataPoints.append(point);

            qDebug() << "Bit field key:" << key << " value:" << extractedValue << " point:" << point;

            emit dataPointAdded(key, point, extractedValue);

            //QString keyWithValue = QString("%1 (%2)").arg(key).arg(extractedValue);
            //emit dataPointAdded(keyWithValue, point);
        }
    }

}
void CANDataModel::addcountTime()
{
    totalcountTime += 1;
}

