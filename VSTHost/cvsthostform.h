#ifndef CVSTHOSTFORM_H
#define CVSTHOSTFORM_H

#include <QDialog>
#include <QMenu>
#include "softsynthsclasses.h"
#include "cvsthostclass.h"

namespace Ui {
    class CVSTHostForm;
}

class CVSTHostForm : public CSoftSynthsForm
{
    Q_OBJECT

public:
    explicit CVSTHostForm(IDevice* Device, QWidget *parent = 0);
    ~CVSTHostForm();
    void Init(AEffect* E,TVSTHost* OwnerClass);
    void AddStatusInfo(QString Info);
    const QString Save();
    void Load(const QString& XML);
    void StopTimer();
private slots:
    void TogglePresets();
    void ToggleStatus();
    void PresetChange(int Index);
    void ParameterIndexChange(int Index);
    void ParameterChange(int Value);
    void SaveBank();
    void LoadBank();
    void SavePreset();
    void LoadPreset();
    void Unload();
    void ViewResized();
protected:
    bool event(QEvent *e);
    void timerEvent(QTimerEvent *e);
private:
    Ui::CVSTHostForm *ui;
       AEffect* m_Effect;
       TVSTHost* m_OwnerClass;
       //int EffWidth;
       //int EffHeight;
       QRect EffRect;
       long CurrentParameter;
        void inline UpdateParam();
        QMenu* Popup;
       bool m_MD;
       QPoint CursorStart;
       QPoint PosStart;
       bool HasEditor;
       bool wasActivated;
       QAction* PresetAction;
       QAction* StatusAction;
       QRect GetEffRect();
};

#endif // CVSTHOSTFORM_H
