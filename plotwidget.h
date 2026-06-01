#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <QVector>

class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget(QWidget *parent = nullptr);

    void setData(const QVector<double>& data, int stepMs);
    void setStepMs(int stepMs);
    void setDpos(int dpos);
    void setDneg(int dneg);
    void setAcpShift(int shift);
    void setVertScale(double scale);
    void setHorizScale(double scale);
    void setStartIndex(int index);

    int getDpos() const         { return m_dpos; }
    int getDneg() const         { return m_dneg; }
    int getAcpShift() const     { return m_acpShift; }
    double getVertScale() const { return m_vertScale; }
    double getHorizScale() const{ return m_horizScale; }
    int getStartIndex() const   { return m_startIndex; }
    int getTotalPoints() const  { return m_data.size(); }
    int getVisiblePoints() const;

signals:
    void paramsChanged(); // сигнал об изменении параметров или данных

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<double> m_data;
    int m_stepMs = 1;
    int m_dpos = 0;
    int m_dneg = 0;
    int m_acpShift = 0;
    double m_vertScale = 1.0;
    double m_horizScale = 1.0;
    int m_startIndex = 0;

    QRect getPlotRect() const;
    double transformX(int index) const;
    double transformY(double value, double yMin, double yMax) const;
    void computeYRange(double &yMin, double &yMax) const;
};

#endif // PLOTWIDGET_H