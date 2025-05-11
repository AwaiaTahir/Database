#include "AttendanceCircleWidget.h"
#include <QPainter>
#include <QPen>
#include <QFont>

AttendanceCircleWidget::AttendanceCircleWidget(QWidget *parent)
    : QWidget(parent), m_percentage(0)
{
    setMinimumSize(120, 120);  // Set preferred size
}

void AttendanceCircleWidget::setPercentage(int percent)
{
    m_percentage = qBound(0, percent, 100); // Clamp between 0 and 100
    update();
}

void AttendanceCircleWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int size = qMin(width(), height()) - 10;
    QRectF rect((width() - size) / 2, (height() - size) / 2, size, size);

    // Draw base circle (gray)
    QPen basePen(Qt::lightGray, 10);
    painter.setPen(basePen);
    painter.drawEllipse(rect);

    // Draw progress arc (green)
    QPen progressPen(QColor("#34D481"), 10);
    painter.setPen(progressPen);
    int angle = static_cast<int>(360.0 * m_percentage / 100.0 * 16);
    painter.drawArc(rect, 90 * 16, -angle);  // Start from top and go clockwise

    // Draw percentage text
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setBold(true);
    font.setPointSize(14);
    font.setFamily("Poppins");
    painter.setFont(font);
    painter.drawText(rect, Qt::AlignCenter, QString::number(m_percentage) + "%");
}
