#include "elapsedtimecounter.h"
#include <QDateTime>

const int refreshRate = 500;
#define PRESENT_TIME QDateTime::currentMSecsSinceEpoch()/1000

ElapsedTimeCounter::ElapsedTimeCounter()
{
    reset();
    QTimer::connect(&m_timer, &QTimer::timeout, this, &ElapsedTimeCounter::calculateTime);
}

void ElapsedTimeCounter::calculateTime()
{
    m_elapsedTime = PRESENT_TIME - m_timeAtStart - m_pauseTime;
    emit elapsedTimeChanged();
}

void ElapsedTimeCounter::start()
{
    reset();
    m_timeAtStart = PRESENT_TIME;
    m_timer.start(refreshRate);
}

void ElapsedTimeCounter::stop()
{
    m_timer.stop();
}

void ElapsedTimeCounter::pause()
{
    m_timeAtPause = PRESENT_TIME;
    stop();
}

void ElapsedTimeCounter::resume()
{
    m_pauseTime += PRESENT_TIME - m_timeAtPause;
    m_timer.start(refreshRate);
}

void ElapsedTimeCounter::reset()
{
    stop();
    m_elapsedTime = 0;
    m_timeAtStart = 0;
    m_timeAtPause = 0;
    m_pauseTime = 0;
}

qint64 ElapsedTimeCounter::getElapsedTime()
{
    return m_elapsedTime;
}
