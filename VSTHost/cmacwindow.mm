#include "cmacwindow.h"
#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <qmacfunctions.h>
#endif

CMacWindow::CMacWindow(QWidget* parent):QWidget(parent)
{
    cocoaWin=0;
}

CMacWindow::~CMacWindow()
{
    DestroyMacWindow();
    qDebug() << "Exit CMacWindow";
}

void* CMacWindow::CreateMacWindow()
{
    QRect r(rect());
    r.translate(mapToGlobal(QPoint(0,0)));
    Rect r1;
    r1.top=r.top();
    r1.left=r.left();
    r1.right=r.right();
    r1.bottom=r.bottom();

    WindowRef testwindow;
    CreateNewWindow(kFloatingWindowClass,
                    kWindowStandardHandlerAttribute | kWindowCompositingAttribute |
                    kWindowOpaqueForEventsAttribute | kWindowNoTitleBarAttribute | kWindowNoShadowAttribute, &r1, &testwindow);

    NSWindow *mixedWindow = [[[NSWindow alloc] initWithWindowRef:testwindow] retain];
    NSView* hostView = (NSView*) effectiveWinId();
    NSWindow* cocoaWindow = [hostView window];
    [cocoaWindow addChildWindow:mixedWindow ordered:NSWindowAbove];
    /*
    HIViewRef embeddedView = attachView (testwindow, HIViewGetRoot (testwindow));
    HIRect r2;
                    r2.origin.x = 0;
                    r2.origin.y = 0;
                    r2.size.width = r.width();
                    r2.size.height = r.height();
                    HIViewSetFrame (embeddedView, &r2);
                    */
    qDebug() << "MacWindow Created" << testwindow;
    return mixedWindow;
}

void* CMacWindow::WindowReference()
{
    return [(NSWindow*)cocoaWin windowRef];
}

void CMacWindow::DestroyMacWindow()
{
    if (cocoaWin)
    {
        DisposeWindow((WindowRef)([(NSWindow*)cocoaWin windowRef]));
        [(NSWindow*)cocoaWin release];
        cocoaWin=0;
        qDebug() << "MacWindow Destroyed";
    }
}

void CMacWindow::Init()
{
    DestroyMacWindow();
    pixmap=QPixmap(size());
    cocoaWin=CreateMacWindow();
}

void CMacWindow::Move()
{
    if (cocoaWin)
    {
        QRect r(rect());
        r.translate(mapToGlobal(QPoint(0,0)));
        MoveWindow((WindowRef)([(NSWindow*)cocoaWin windowRef]),r.left(),r.top(),true);
    }
}

void CMacWindow::Size()
{
    if (cocoaWin)
    {
        SizeWindow((WindowRef)([(NSWindow*)cocoaWin windowRef]),width(),height(),true);
        Grab();
    }
}

void CMacWindow::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    Size();
}

void CMacWindow::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    Show();
}

void CMacWindow::paintEvent(QPaintEvent* e)
{
    if (cocoaWin)
    {
        if (this->isVisible())
        {
            if ([(NSWindow*)cocoaWin isVisible])
            {
                NSView* hostView = (NSView*) effectiveWinId();
                NSWindow* cocoaWindow = [hostView window];

                qDebug() << "levels" << [(NSWindow*)cocoaWin level] << [cocoaWindow level];
                if ([(NSWindow*)cocoaWin level]==[cocoaWindow level])
                {
                    ToFront();
                }
            }
            else
            {
                QPainter p(this);
                //p.setOpacity(0.5);
                p.drawPixmap(e->rect(),pixmap,e->rect());
            }
        }
    }
}

void CMacWindow::Grab()
{
    QRect r(rect());
    r.translate(mapToGlobal(QPoint(0,0)));
    if (cocoaWin)
    {
        CGRect rect;
        rect.origin.x=r.left();
        rect.origin.y=r.top();
        rect.size.width=r.width();
        rect.size.height=r.height();

        ToFront();
        CGWindowID wid = (CGWindowID)([(NSWindow*)cocoaWin windowNumber]);

        CGImageRef windowImage =  CGWindowListCreateImage( rect, kCGWindowListOptionIncludingWindow, wid, kCGWindowImageDefault );

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        pixmap = QPixmap::fromMacCGImageRef(windowImage);
#else
        pixmap = QtMacExtras::fromCGImageRef(windowImage);
#endif
    }
    update();
}

void CMacWindow::hideEvent(QHideEvent *e)
{
    Hide();
    QWidget::hideEvent(e);
}

void CMacWindow::Activate()
{
    if (cocoaWin)
    {
        ActivateWindow((WindowRef)([(NSWindow*)cocoaWin windowRef]),false);
    }
}

void CMacWindow::Hide()
{
    if (cocoaWin)
    {
        if ([(NSWindow*)cocoaWin isVisible])
        {
            Grab();
            [(NSWindow*)cocoaWin orderOut:nil];
        }
    }
}

void CMacWindow::Show()
{
    if (cocoaWin)
    {
        if (![(NSWindow*)cocoaWin isVisible])
        {
            ToFront();
        }
    }
    Activate();
}

void CMacWindow::ToFront()
{
    Move();
    if (cocoaWin)
    {
        NSView* hostView = (NSView*) effectiveWinId();
        NSWindow* cocoaWindow = [hostView window];
        [(NSWindow*)cocoaWin orderWindow:NSWindowAbove relativeTo:[cocoaWindow windowNumber]];
    }
}
