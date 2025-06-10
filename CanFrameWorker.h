#ifndef CANFRAMEWORKER_H
#define CANFRAMEWORKER_H

#include <QObject>
#include <QCanBusFrame>
#include <QThread>

class CanFrameWorker : public QObject
{
    Q_OBJECT

public:
    explicit CanFrameWorker(QObject *parent = nullptr);
    void processFrame(const QCanBusFrame &frame);

signals:
    void frameProcessed(QString formattedData);

public slots:
    void processData(const QCanBusFrame &frame);

private:
         // Burada veriyi işlemek için gerekli değişkenleri tutabiliriz
};

#endif // CANFRAMEWORKER_H
