#ifndef ELAPSEDTIMECOUNTER_H
#define ELAPSEDTIMECOUNTER_H
#include <QTimer>
#include "qqmlhelpers.h"
class ElapsedTimeCounter : public QObject
{
    Q_OBJECT

public:
    ElapsedTimeCounter();
    void start();
    void pause();
    void resume();
    void stop();
    void reset();
    qint64 getElapsedTime();

signals:
    void elapsedTimeChanged();

private:
    void calculateTime();

    QTimer m_timer;
    qint64 m_timeAtStart;
    qint64 m_timeAtPause;
    qint64 m_pauseTime;
    qint64 m_elapsedTime;
};

#endif // ELAPSEDTIMECOUNTER_H
