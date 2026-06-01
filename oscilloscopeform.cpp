#include "oscilloscopeform.h"
#include "plotwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollBar>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>

OscilloscopeForm::OscilloscopeForm(QWidget *parent, int initDPos, int initDNeg, int initAcpShift)
    : QWidget(parent)
{
    m_plotWidget = new PlotWidget(this);
    m_scrollBar = new QScrollBar(Qt::Horizontal, this);
    m_scrollBar->setRange(0, 0);

    m_btnHorizZoomIn  = new QPushButton("H+", this);
    m_btnHorizZoomOut = new QPushButton("H-", this);
    m_btnVertZoomIn   = new QPushButton("V+", this);
    m_btnVertZoomOut  = new QPushButton("V-", this);

    m_btnDPosInc = new QPushButton("DPos+", this);
    m_btnDPosDec = new QPushButton("DPos-", this);
    m_btnDNegInc = new QPushButton("DNeg+", this);
    m_btnDNegDec = new QPushButton("DNeg-", this);
    m_btnAcpInc  = new QPushButton("Shift+", this);
    m_btnAcpDec  = new QPushButton("Shift-", this);

    m_lblDPos      = new QLabel(this);
    m_lblDNeg      = new QLabel(this);
    m_lblAcpShift  = new QLabel(this);
    m_lblVertScale = new QLabel(this);
    m_lblHorizScale= new QLabel(this);

    setOptions(initDPos, initDNeg, initAcpShift);
    m_plotWidget->setVertScale(1.0);
    m_plotWidget->setHorizScale(1.0);
    updateUIFromParams();

    QHBoxLayout *zoomLayout = new QHBoxLayout;
    zoomLayout->addWidget(m_btnHorizZoomOut);
    zoomLayout->addWidget(m_btnHorizZoomIn);
    zoomLayout->addWidget(m_btnVertZoomOut);
    zoomLayout->addWidget(m_btnVertZoomIn);

    QGridLayout *paramLayout = new QGridLayout;
    paramLayout->addWidget(new QLabel("DPos:"), 0, 0);
    paramLayout->addWidget(m_lblDPos, 0, 1);
    paramLayout->addWidget(m_btnDPosDec, 0, 2);
    paramLayout->addWidget(m_btnDPosInc, 0, 3);
    paramLayout->addWidget(new QLabel("DNeg:"), 1, 0);
    paramLayout->addWidget(m_lblDNeg, 1, 1);
    paramLayout->addWidget(m_btnDNegDec, 1, 2);
    paramLayout->addWidget(m_btnDNegInc, 1, 3);
    paramLayout->addWidget(new QLabel("AcpShift:"), 2, 0);
    paramLayout->addWidget(m_lblAcpShift, 2, 1);
    paramLayout->addWidget(m_btnAcpDec, 2, 2);
    paramLayout->addWidget(m_btnAcpInc, 2, 3);

    QHBoxLayout *scaleLabels = new QHBoxLayout;
    scaleLabels->addWidget(m_lblVertScale);
    scaleLabels->addWidget(m_lblHorizScale);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_plotWidget, 1);
    mainLayout->addWidget(m_scrollBar);
    mainLayout->addLayout(zoomLayout);
    mainLayout->addLayout(paramLayout);
    mainLayout->addLayout(scaleLabels);

    connect(m_btnHorizZoomIn,  &QPushButton::clicked, this, &OscilloscopeForm::onHorizZoomIn);
    connect(m_btnHorizZoomOut, &QPushButton::clicked, this, &OscilloscopeForm::onHorizZoomOut);
    connect(m_btnVertZoomIn,    &QPushButton::clicked, this, &OscilloscopeForm::onVertZoomIn);
    connect(m_btnVertZoomOut,   &QPushButton::clicked, this, &OscilloscopeForm::onVertZoomOut);
    connect(m_btnDPosInc,       &QPushButton::clicked, this, &OscilloscopeForm::onDPosInc);
    connect(m_btnDPosDec,       &QPushButton::clicked, this, &OscilloscopeForm::onDPosDec);
    connect(m_btnDNegInc,       &QPushButton::clicked, this, &OscilloscopeForm::onDNegInc);
    connect(m_btnDNegDec,       &QPushButton::clicked, this, &OscilloscopeForm::onDNegDec);
    connect(m_btnAcpInc,        &QPushButton::clicked, this, &OscilloscopeForm::onAcpShiftInc);
    connect(m_btnAcpDec,        &QPushButton::clicked, this, &OscilloscopeForm::onAcpShiftDec);
    connect(m_scrollBar,        &QScrollBar::valueChanged, this, &OscilloscopeForm::onScrollChanged);
    connect(m_plotWidget,       &PlotWidget::paramsChanged, this, &OscilloscopeForm::updateUIFromParams);
    connect(m_plotWidget,       &PlotWidget::paramsChanged, [this]() {
        int total = m_plotWidget->getTotalPoints();
        int visible = m_plotWidget->getVisiblePoints();
        if (total > visible) {
            m_scrollBar->setRange(0, total - visible);
            m_scrollBar->setPageStep(visible / 2);
        } else {
            m_scrollBar->setRange(0, 0);
        }
        m_scrollBar->setValue(m_plotWidget->getStartIndex());
    });
}

OscilloscopeForm::~OscilloscopeForm() {}

void OscilloscopeForm::sendData(const QVector<double>& data, int stepMs)
{
    m_plotWidget->setData(data, stepMs);
}

void OscilloscopeForm::getOptions(int &dPos, int &dNeg, int &acpShift) const
{
    dPos = m_plotWidget->getDpos();
    dNeg = m_plotWidget->getDneg();
    acpShift = m_plotWidget->getAcpShift();
}

void OscilloscopeForm::setOptions(int dPos, int dNeg, int acpShift)
{
    m_plotWidget->setDpos(dPos);
    m_plotWidget->setDneg(dNeg);
    m_plotWidget->setAcpShift(acpShift);
    updateUIFromParams();
}

void OscilloscopeForm::onHorizZoomIn()
{
    m_plotWidget->setHorizScale(m_plotWidget->getHorizScale() * 1.2);
}

void OscilloscopeForm::onHorizZoomOut()
{
    m_plotWidget->setHorizScale(m_plotWidget->getHorizScale() / 1.2);
}

void OscilloscopeForm::onVertZoomIn()
{
    m_plotWidget->setVertScale(m_plotWidget->getVertScale() * 1.2);
}

void OscilloscopeForm::onVertZoomOut()
{
    m_plotWidget->setVertScale(m_plotWidget->getVertScale() / 1.2);
}

void OscilloscopeForm::onDPosInc()
{
    setOptions(m_plotWidget->getDpos() + 1, m_plotWidget->getDneg(), m_plotWidget->getAcpShift());
}

void OscilloscopeForm::onDPosDec()
{
    setOptions(m_plotWidget->getDpos() - 1, m_plotWidget->getDneg(), m_plotWidget->getAcpShift());
}

void OscilloscopeForm::onDNegInc()
{
    setOptions(m_plotWidget->getDpos(), m_plotWidget->getDneg() + 1, m_plotWidget->getAcpShift());
}

void OscilloscopeForm::onDNegDec()
{
    setOptions(m_plotWidget->getDpos(), m_plotWidget->getDneg() - 1, m_plotWidget->getAcpShift());
}

void OscilloscopeForm::onAcpShiftInc()
{
    setOptions(m_plotWidget->getDpos(), m_plotWidget->getDneg(), m_plotWidget->getAcpShift() + 1);
}

void OscilloscopeForm::onAcpShiftDec()
{
    setOptions(m_plotWidget->getDpos(), m_plotWidget->getDneg(), m_plotWidget->getAcpShift() - 1);
}

void OscilloscopeForm::onScrollChanged(int value)
{
    m_plotWidget->setStartIndex(value);
}

void OscilloscopeForm::updateUIFromParams()
{
    m_lblDPos->setText(QString::number(m_plotWidget->getDpos()));
    m_lblDNeg->setText(QString::number(m_plotWidget->getDneg()));
    m_lblAcpShift->setText(QString::number(m_plotWidget->getAcpShift()));
    m_lblVertScale->setText(QString("V scale: %1").arg(m_plotWidget->getVertScale(), 0, 'f', 2));
    m_lblHorizScale->setText(QString("H scale: %1").arg(m_plotWidget->getHorizScale(), 0, 'f', 2));
}