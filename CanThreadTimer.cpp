#include "canthreadtimer.h"
#include <QDebug>

CANThreadTimer::CANThreadTimer(QObject *parent)
    : QThread(parent), m_timer(nullptr), m_running(false), m_interval(1)
{
}

void CANThreadTimer::startTimer(int interval)
{
    QMutexLocker locker(&m_mutex);
    m_interval = interval;
    m_running = true;


    if (!isRunning()) {
        start();
    }
}

void CANThreadTimer::stopTimer()
{
    QMutexLocker locker(&m_mutex);
    m_running = false;
}

void CANThreadTimer::run()
{
    m_timer = new QTimer();

    connect(m_timer, &QTimer::timeout, this, [this]() {

        emit timerTicked();
    }, Qt::DirectConnection);


    m_timer->start(m_interval);
    exec();


    delete m_timer;
    m_timer = nullptr;
}
