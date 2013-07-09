#ifndef CSOFTSYNTHSFORM_H
#define CSOFTSYNTHSFORM_H

#include <QDialog>
#include <QCloseEvent>
#include <QMessageBox>

class IDevice;

class CSoftSynthsForm : public QDialog
{
    Q_OBJECT
public:
    CSoftSynthsForm(IDevice* Device, const bool FixedSize, QWidget* parent=0);
    virtual const QString Save();
    virtual void Load(const QString& XML);
public slots:
    void show();
    void setVisible(bool visible);
protected:
    bool m_FixedSize;
    IDevice* m_Device;
    void closeEvent(QCloseEvent *event);
    bool event(QEvent *event);
    virtual const QString CustomSave();
    virtual void CustomLoad(const QString& XML);
};

#endif // CSOFTSYNTHSFORM_H
