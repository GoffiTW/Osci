#include "plotwidget.h"
#include <QPainter>
#include <QPen>
#include <algorithm>
#include <cmath>
#include <limits>

PlotWidget::PlotWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(400, 300);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void PlotWidget::setData(const QVector<double>& data, int stepMs)
{
    m_data = data;
    m_stepMs = stepMs;
    m_startIndex = 0;
    update();
    emit paramsChanged();
}

void PlotWidget::setStepMs(int stepMs)
{
    m_stepMs = stepMs;
    update();
    emit paramsChanged();
}

void PlotWidget::setDpos(int dpos)
{
    m_dpos = dpos;
    update();
    emit paramsChanged();
}

void PlotWidget::setDneg(int dneg)
{
    m_dneg = dneg;
    update();
    emit paramsChanged();
}

void PlotWidget::setAcpShift(int shift)
{
    m_acpShift = shift;
    update();
    emit paramsChanged();
}

void PlotWidget::setVertScale(double scale)
{
    m_vertScale = scale;
    update();
    emit paramsChanged();
}

void PlotWidget::setHorizScale(double scale)
{
    if (scale < 0.1) scale = 0.1;
    m_horizScale = scale;
    int visible = getVisiblePoints();
    if (m_startIndex + visible > m_data.size())
        m_startIndex = std::max(0, static_cast<int>(m_data.size() - visible));
    update();
    emit paramsChanged();
}

void PlotWidget::setStartIndex(int index)
{
    int visible = getVisiblePoints();
    if (visible <= 0) return;
    if (index < 0) index = 0;
    if (index + visible > m_data.size())
        index = std::max(0, static_cast<int>(m_data.size() - visible));
    m_startIndex = index;
    update();
    emit paramsChanged();
}

int PlotWidget::getVisiblePoints() const
{
    if (m_data.isEmpty()) return 0;
    int total = static_cast<int>(m_data.size());
    int vis = static_cast<int>(total / m_horizScale);
    if (vis < 1) vis = 1;
    if (vis > total) vis = total;
    return vis;
}

QRect PlotWidget::getPlotRect() const
{
    QRect rect = contentsRect();
    int left = 50, right = 30, top = 20, bottom = 30;
    return rect.adjusted(left, top, -right, -bottom);
}

double PlotWidget::transformX(int index) const
{
    QRect rect = getPlotRect();
    int visible = getVisiblePoints();
    if (visible <= 0) return rect.left();
    double t = static_cast<double>(index - m_startIndex) / visible;
    return rect.left() + t * rect.width();
}

double PlotWidget::transformY(double value, double yMin, double yMax) const
{
    QRect rect = getPlotRect();
    if (yMax <= yMin) return rect.center().y();
    double t = (value - yMin) / (yMax - yMin);
    return rect.bottom() - t * rect.height();
}

void PlotWidget::computeYRange(double &yMin, double &yMax) const
{
    yMin =  std::numeric_limits<double>::max();
    yMax = -std::numeric_limits<double>::max();

    int start = m_startIndex;
    int end = std::min(static_cast<int>(m_startIndex + getVisiblePoints()),
                       static_cast<int>(m_data.size()));
    for (int i = start; i < end; ++i) {
        double val = (m_data[i] - m_acpShift) * m_vertScale;
        if (val < yMin) yMin = val;
        if (val > yMax) yMax = val;
    }
    double dposScaled = (m_dpos - m_acpShift) * m_vertScale;
    double dnegScaled = (m_dneg - m_acpShift) * m_vertScale;
    yMin = std::min({yMin, dposScaled, dnegScaled});
    yMax = std::max({yMax, dposScaled, dnegScaled});

    double margin = (yMax - yMin) * 0.1;
    if (margin == 0) margin = 1.0;
    yMin -= margin;
    yMax += margin;
}

void PlotWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect plotRect = getPlotRect();
    if (plotRect.width() <= 0 || plotRect.height() <= 0)
        return;

    painter.fillRect(plotRect, Qt::white);
    painter.setPen(QPen(Qt::black, 1));

    QPen gridPen(Qt::lightGray, 1, Qt::DotLine);
    painter.setPen(gridPen);
    int numXTicks = 5;
    for (int i = 0; i <= numXTicks; ++i) {
        double t = static_cast<double>(i) / numXTicks;
        int x = plotRect.left() + static_cast<int>(t * plotRect.width());
        painter.drawLine(x, plotRect.top(), x, plotRect.bottom());
    }
    int numYTicks = 5;
    for (int i = 0; i <= numYTicks; ++i) {
        double t = static_cast<double>(i) / numYTicks;
        int y = plotRect.bottom() - static_cast<int>(t * plotRect.height());
        painter.drawLine(plotRect.left(), y, plotRect.right(), y);
    }

    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(plotRect);

    if (m_data.isEmpty())
        return;

    double yMin, yMax;
    computeYRange(yMin, yMax);

    painter.setPen(QPen(Qt::blue, 2));
    int visible = getVisiblePoints();
    int start = m_startIndex;
    int end = std::min(static_cast<int>(start + visible),
                       static_cast<int>(m_data.size()));
    QPolygonF polyline;
    for (int i = start; i < end; ++i) {
        double x = transformX(i);
        double val = (m_data[i] - m_acpShift) * m_vertScale;
        double y = transformY(val, yMin, yMax);
        polyline << QPointF(x, y);
    }
    painter.drawPolyline(polyline);

    QPen dPen(Qt::red, 2, Qt::DashLine);
    painter.setPen(dPen);
    double dposScaled = (m_dpos - m_acpShift) * m_vertScale;
    double dnegScaled = (m_dneg - m_acpShift) * m_vertScale;
    int yPos = static_cast<int>(transformY(dposScaled, yMin, yMax));
    int yNeg = static_cast<int>(transformY(dnegScaled, yMin, yMax));
    painter.drawLine(plotRect.left(), yPos, plotRect.right(), yPos);
    painter.drawLine(plotRect.left(), yNeg, plotRect.right(), yNeg);

    painter.setPen(Qt::black);
    int totalTimeMs = (static_cast<int>(m_data.size()) - 1) * m_stepMs;
    int visibleTimeMs = static_cast<int>(getVisiblePoints() * m_stepMs / m_horizScale);
    int startTimeMs = m_startIndex * m_stepMs;
    painter.drawText(plotRect.left(), plotRect.bottom() + 15,
                     QString("t = %1 ms").arg(startTimeMs));
    painter.drawText(plotRect.right() - 60, plotRect.bottom() + 15,
                     QString("+%1 ms").arg(visibleTimeMs));
    painter.drawText(plotRect.left() - 40, plotRect.top() + 10,
                     QString::number(yMax, 'f', 1));
    painter.drawText(plotRect.left() - 40, plotRect.bottom() - 5,
                     QString::number(yMin, 'f', 1));
}