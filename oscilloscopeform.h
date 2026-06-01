#ifndef OSCILLOSCOPEFORM_H
#define OSCILLOSCOPEFORM_H

#include <QWidget>
#include <QVector>

class PlotWidget;
class QScrollBar;
class QPushButton;
class QLabel;

class OscilloscopeForm : public QWidget
{
    Q_OBJECT
public:
    explicit OscilloscopeForm(QWidget *parent = nullptr,
                              int initDPos = 0, int initDNeg = 0, int initAcpShift = 0);
    ~OscilloscopeForm();

    void sendData(const QVector<double>& data, int stepMs);
    void getOptions(int &dPos, int &dNeg, int &acpShift) const;
    void setOptions(int dPos, int dNeg, int acpShift);

private slots:
    void onHorizZoomIn();
    void onHorizZoomOut();
    void onVertZoomIn();
    void onVertZoomOut();
    void onDPosInc();
    void onDPosDec();
    void onDNegInc();
    void onDNegDec();
    void onAcpShiftInc();
    void onAcpShiftDec();
    void onScrollChanged(int value);
    void updateUIFromParams();

private:
    PlotWidget *m_plotWidget;
    QScrollBar *m_scrollBar;

    QPushButton *m_btnHorizZoomIn, *m_btnHorizZoomOut;
    QPushButton *m_btnVertZoomIn, *m_btnVertZoomOut;
    QPushButton *m_btnDPosInc, *m_btnDPosDec;
    QPushButton *m_btnDNegInc, *m_btnDNegDec;
    QPushButton *m_btnAcpInc, *m_btnAcpDec;
    QLabel *m_lblDPos, *m_lblDNeg, *m_lblAcpShift, *m_lblVertScale, *m_lblHorizScale;
};

#endif // OSCILLOSCOPEFORM_H