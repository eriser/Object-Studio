#include "csoftsynthsform.h"
#include "softsynthsclasses.h"
#include "qdomlite.h"

CSoftSynthsForm::CSoftSynthsForm(IDevice* Device, const bool FixedSize, QWidget *parent)
    : QDialog(parent,Qt::Tool | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint)
{
    m_Device=Device;
    m_FixedSize=FixedSize;
}

const QString CSoftSynthsForm::Save()
{
    QDomLiteElement xml("Custom");

    QString Custom=CustomSave();
    if (!Custom.isEmpty())
    {
        QDomLiteElement CustomXML;
        CustomXML.fromString(Custom);
        xml.appendAttributesString(CustomXML.attributesString());
        foreach(QDomLiteElement* e,CustomXML.childElements)
        {
            xml.appendClone(e);
        }
    }

    QDomLiteElement* Items=xml.elementByTag("Items");
    if (!Items) Items=xml.appendChild("Items");
    QDomLiteElement* Position=Items->appendChild("Position");
    Position->setAttribute("Top",pos().y());
    Position->setAttribute("Left",pos().x());
    Position->setAttribute("Height",size().height());
    Position->setAttribute("Width",size().width());
    Position->setAttribute("Visible",isVisible());

    return xml.toString();
}

void CSoftSynthsForm::Load(const QString &XML)
{
    QDomLiteElement xml;
    xml.fromString(XML);
    if (xml.tag=="Custom")
    {
        QDomLiteElement* Items=xml.elementByTag("Items");
        {
            if (Items)
            {
                QDomLiteElement* Position=Items->elementByTag("Position");
                {
                    if (Position)
                    {
                        move(Position->attributeValue("Left"),Position->attributeValue("Top"));
                        if (!m_FixedSize) resize(Position->attributeValue("Width"),Position->attributeValue("Height"));
                        //if (m_FixedSize) setFixedSize(Position->attributeValue("Width"),Position->attributeValue("Height"));
                        if (Position->attributeValue("Visible"))
                        {
                            if (m_FixedSize) show();
                        }
                        setVisible(Position->attributeValue("Visible"));
                    }
                }
                //QDomLiteElement* Custom=Items->elementByTag("Custom");
                //if (Custom) CustomLoad(Custom->toString());
            }
        }
        CustomLoad(xml.toString());
    }
}

const QString CSoftSynthsForm::CustomSave()
{
    return QString();
}

void CSoftSynthsForm::CustomLoad(const QString& /*XML*/)
{
}

void CSoftSynthsForm::setVisible(bool visible)
{
    setWindowTitle(m_Device->DeviceID());
    QDialog::setVisible(visible);
}

void CSoftSynthsForm::show()
{
    setWindowTitle(m_Device->DeviceID());
    if (m_FixedSize) setFixedSize(rect().size());
    if (pos()==QPoint(0,0))
    {
        QDialog::show();
    }
    else
    {
        move(pos());
        setVisible(true);
    }
}

void CSoftSynthsForm::closeEvent(QCloseEvent *event)
{
    m_Device->HideForm();
    event->ignore();
}

bool CSoftSynthsForm::event(QEvent *event)
{
    if (event->type()==QEvent::NonClientAreaMouseButtonPress)
    {
        m_Device->Activate();
    }
    return QDialog::event(event);
}
