#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include "corebuffer.h"
#include "cdesktopcomponent.h"
#include <softsynthsclasses.h>
#include "caddins.h"
#include <QEvent>
#include <QtDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void Play();
    void Stop();
    void Record();
    void StopRecording();
    void TogglePlay(bool value);
    void ToggleRecord(bool value);
protected:
    void timerEvent(QTimerEvent *event);
private:
    bool m_Playing;
    Ui::MainWindow *ui;
    CCoreMainBuffers MainBuffers;
};

class CObjectStudioApplication : public QApplication
{
    Q_OBJECT

public:
    MainWindow w;
    CObjectStudioApplication( int argc, char * argv[] ) : QApplication( argc, argv ) {}
    bool event( QEvent * pEvent )
    {
        if ( pEvent->type() == QEvent::ApplicationActivate )
        {
            w.activateWindow();
            qDebug() << "ApplicationActivate";
        }
        else if ( pEvent->type() == QEvent::ApplicationDeactivate )
        {
            qDebug() << "ApplicationDeactivate";
        }
        return QApplication::event( pEvent );
    }
};

#endif // MAINWINDOW_H
