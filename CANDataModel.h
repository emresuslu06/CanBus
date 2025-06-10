#ifndef CANDATAMODEL_H
#define CANDATAMODEL_H

#include <QStringListModel>
#include <QTimer>
#include <QCanBusFrame>
#include <QPointF>
#include <QAbstractListModel>
#include "fileRead.h"

class CANDataModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QString connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)

public:
    enum Roles {
        XRole = Qt::UserRole + 1,
        YRole
    };

    explicit CANDataModel(QObject *parent = nullptr);

    // Model interface metodları
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addCanFrame(const QCanBusFrame &frame);
    void addcountTime();


    // Yeni eklenen fonksiyonlar
    void loadFilterData();  // Filtre verilerini yükleyecek fonksiyon

    QString connectionStatus() const { return m_connectionStatus; }
    void setConnectionStatus(const QString &status);

signals:
    void connectionStatusChanged();

    void dataPointAdded45(const QPointF &point45);
    void updateXAxis(float min, float max);
    void dataPointAddedSecondByte(const QPointF &point);
    void dataPointAdded(const QString &key, const QPointF &point,int extractedValue);


private:
    QTimer *timer;
    QVector<QPointF> dataPoints;
    QVector<QPointF> dataPoints45;
    QStringList canFrames;
    QString m_connectionStatus;
    bool isFirstFrame = true;
    int totalcountTime = 0;
    int currentIndex = 0;

      QMap<QString, QMap<int, QVector<QPointF>>> byteDataPointsById;

         QMap<QString, QVector<QPointF>> idByteDataMap;



   QMap<QString, QPair<QList<int>, QList<QPair<int, int>>>> idToData;
    fileRead fileReader;
};

#endif // CANDATAMODEL_H
