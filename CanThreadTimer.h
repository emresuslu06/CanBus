#ifndef CANTHREADTIMER_H
#define CANTHREADTIMER_H

#include <QThread>
#include <QTimer>
#include <QMutex>

class CANThreadTimer : public QThread {
    Q_OBJECT

public:
    explicit CANThreadTimer(QObject *parent = nullptr);
    void startTimer(int interval = 1);
    void stopTimer();

signals:
    void timerTicked();

protected:
    void run() override;

private:
    QTimer *m_timer;
    QMutex m_mutex;
    bool m_running;
    int m_interval;
};

#endif // CANTHREADTIMER_H
