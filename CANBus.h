#ifndef CANBUS_H
#define CANBUS_H
#include <QObject>
#include <QtSerialBus/QCanBusDevice>
#include "CANDataModel.h"
#include <QTimer>

class CANBus : public QObject
{
    Q_OBJECT

public:
    explicit CANBus(CANDataModel *dataModel, QObject *parent = nullptr);
    ~CANBus();

    bool initializeConnection();
    bool isConnected() const { return m_connectionStatus; }


    bool getConnectionStatus() const { return m_connectionStatus; }


  Q_PROPERTY(bool connectionStatus READ getConnectionStatus NOTIFY connectionStatusChanged)

public slots:
    void setBaudRate(int newBaudRate);
    void toggleConnection(bool isConnected);

private slots:
    void checkConnectionStatus();
    void onFramesReceived();

signals:
    void baudRateChanged(int newBaudRate);
    void baudRateChangeFailed();
    void connectionClosed();
    void connectionToggled(bool isConnected);


    void connectionStatusChanged();

private:
    QCanBusDevice *canDevice;
    CANDataModel *dataModel;
    QTimer *connectionTimer;


    bool m_connectionStatus = false;
};

#endif // CANBUS_H
