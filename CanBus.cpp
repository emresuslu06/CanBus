#include "CANBus.h"
#include <QtSerialBus/QCanBus>
#include <QtSerialBus/QCanBusDevice>
#include <QtSerialBus/QCanBusFrame>
#include <QDebug>
#include "CANDataModel.h"
#include <QTimer>

CANBus::CANBus(CANDataModel *dataModel, QObject *parent)
    : QObject(parent), dataModel(dataModel), canDevice(nullptr)
{
    connectionTimer = new QTimer();
    //connect(connectionTimer, &QTimer::timeout, this, &CANBus::checkConnectionStatus);
    //connectionTimer->start(1000);


}

CANBus::~CANBus(){
    if(canDevice)
    {
        canDevice->disconnectDevice();
        delete canDevice;
        canDevice = nullptr;
    }
}

bool CANBus::initializeConnection()
{
    if(canDevice && canDevice->state() == QCanBusDevice::ConnectedState){
        qWarning() << "CAN cihazı zaten bağlı";
        return false;
    }

    if(canDevice && canDevice->state() != QCanBusDevice::UnconnectedState) {
        qWarning() << "Bağlantı kopmuş, eski bağlantı kesiliyor...";
        canDevice->disconnectDevice();
        delete canDevice;
        canDevice = nullptr;
    }

    QString plugin = "peakcan";
    QString interfaceName = "usb0";

    qDebug() << "CAN cihazı oluşturuluyor...";
    canDevice = QCanBus::instance()->createDevice(plugin, interfaceName);
    if(!canDevice){
        qWarning() << "CAN cihazı oluşturulamadı";
        return false;
    }

    qDebug() << "CAN cihazı başarıyla oluşturuldu";

    if (canDevice)
    {
        canDevice->setConfigurationParameter(QCanBusDevice::BitRateKey, 250000);
        qDebug() << "Baud rate 250000 olarak ayarlandı.";
    }
    else
    {
        qDebug() << "CAN cihazı mevcut değil.";
    }

    if(!canDevice->connectDevice())
    {
        qWarning() << "CAN cihazına bağlanılamadı" << canDevice->errorString();
        dataModel->setConnectionStatus("Bağlantı Başarısız");
        delete canDevice;
        canDevice = nullptr;
        return false;
    }

    qDebug() << "CAN cihazına başarıyla bağlanıldı";

    // Mesaj alma sinyalini bağlama
    connect(canDevice, &QCanBusDevice::framesReceived, this, &CANBus::onFramesReceived);

    dataModel->setConnectionStatus("Bağlantı Başarılı");
    return true;
}
void CANBus::checkConnectionStatus()
{
    if (canDevice) {
        QCanBusDevice::CanBusDeviceState currentState = canDevice->state();
        QCanBusDevice::CanBusError currentError = canDevice->error();
        QCanBusDevice::CanBusStatus currentBusStatus = canDevice->busStatus();

        qDebug() << "checkConnectionStatus() çağrıldı - Mevcut durum:" << currentState;
        qDebug() << "Mevcut hata durumu:" << currentError << " - Hata mesajı:" << canDevice->errorString();
        qDebug() << "Bus status:" << currentBusStatus;

        if (currentState == QCanBusDevice::ConnectedState) {
            if (dataModel->connectionStatus() != "Bağlantı Başarılı") {
                dataModel->setConnectionStatus("Bağlantı Başarılı");
            }
        } else if (currentState == QCanBusDevice::UnconnectedState) {
            qWarning() << "Bağlantı kopmuş, yeniden bağlanmayı dene";
            initializeConnection();
        } else {
            if (dataModel->connectionStatus() != "Bağlantı Başarısız") {
                dataModel->setConnectionStatus("Bağlantı Başarısız");
            }
        }
    } else {
        if (dataModel->connectionStatus() != "Bağlantı Başarısız") {
            dataModel->setConnectionStatus("Bağlantı Başarısız");
        }
    }
}



void CANBus::onFramesReceived()
{
    if (!canDevice) return;

    QList<QCanBusFrame> frames = canDevice->readAllFrames();
    for (const QCanBusFrame &frame : frames) {
        QString dataString;
        for (int i = 0; i < frame.payload().size(); ++i) {
            dataString.append(QString::asprintf("%02X ", static_cast<unsigned char>(frame.payload().at(i))));
        }

        // frameId()'yi hexadecimal formatta göstermek için
        qDebug() << "ID:" << QString::number(frame.frameId(), 16).toUpper() << " Data:" << dataString;
        QString message = QString("Mesaj alındı - ID: %1 Data: %2")
                              .arg(frame.frameId(), 0, 16)  // Hexadecimal ID formatında
                              .arg(dataString);
        dataModel->setConnectionStatus( message);

        if (dataModel) {
            dataModel->addCanFrame(frame);
        }
    }
}

void CANBus::setBaudRate(int newBaudRate)
{
    if (canDevice && canDevice->state() == QCanBusDevice::ConnectedState) {
        canDevice->disconnectDevice();
        delete canDevice;
        canDevice = nullptr;
    }

    canDevice = QCanBus::instance()->createDevice("peakcan", "usb0");

    if (!canDevice) {
        qWarning() << "CAN cihazı oluşturulamadı.";
        emit baudRateChangeFailed();
        return;
    }

    canDevice->setConfigurationParameter(QCanBusDevice::BitRateKey, newBaudRate);
    qDebug() << "Baud rate" << newBaudRate << " olarak ayarlandı.";

    if (!canDevice->connectDevice()) {
        qWarning() << "CAN cihazına bağlanılamadı:" << canDevice->errorString();
        emit baudRateChangeFailed();
        return;
    }

    qDebug() << "Cihaz tekrar bağlandı.";
    connect(canDevice, &QCanBusDevice::framesReceived, this, &CANBus::onFramesReceived);

    emit baudRateChanged(newBaudRate);
}

void CANBus::toggleConnection(bool isConnected)
{
     qDebug() << "toggleConnection fonksiyonu çağrıldı, isConnected durumu:" << isConnected;

    if (isConnected) {

        if (!initializeConnection()) {
            qWarning() << "Bağlantı başlatılamadı!";
        } else {
            qDebug() << "Bağlantı başarıyla başlatıldı!";
             m_connectionStatus = true;
            emit connectionStatusChanged();
        }
    } else {

        if (canDevice) {
            canDevice->disconnectDevice();
            delete canDevice;
            canDevice = nullptr;
            qDebug() << "Bağlantı kesildi.";
            m_connectionStatus = false;
            emit connectionStatusChanged();
        }
    }
}




