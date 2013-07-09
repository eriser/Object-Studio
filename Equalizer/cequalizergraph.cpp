#include "cequalizergraph.h"
#include "ui_cequalizergraph.h"
#include <QPainter>

CEqualizerGraph::CEqualizerGraph(QWidget *parent) :
    QCanvas(parent,1),
    ui(new Ui::CEqualizerGraph)
{
    ui->setupUi(this);
    emit RefreshMe();
}

CEqualizerGraph::~CEqualizerGraph()
{
    delete ui;
}

void CEqualizerGraph::resizeEvent(QResizeEvent* e)
{
    QCanvas::resizeEvent(e);
    emit RefreshMe();
}

