#include "studentprogresschart.h"

StudentProgressChart::StudentProgressChart(const QList<int> &progressData, QWidget *parent)
    : QChartView(new QChart(), parent)
{
    QStringList months = {"Sep", "Oct", "Nov", "Dec", "Jan", "Feb", "Mar", "Apr", "May"};

    QLineSeries *line = new QLineSeries();
    QScatterSeries *dots = new QScatterSeries();
    dots->setMarkerSize(10);
    dots->setColor(Qt::yellow);

    for (int i = 0; i < progressData.size(); ++i) {
        line->append(i, progressData[i]);
        dots->append(i, progressData[i]);
    }

    line->setColor(QColor("#00ffcc"));

    QChart *chart = this->chart();
    chart->addSeries(line);
    chart->addSeries(dots);
    chart->setTitle("Student's Progress");

    QFont titleFont;
    titleFont.setFamily("Poppins");
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    chart->setTitleFont(titleFont);
    chart->setTitleBrush(QBrush(QColor("white")));

    chart->setBackgroundBrush(Qt::NoBrush);
    chart->setBackgroundRoundness(18);

    chart->legend()->hide();

    QCategoryAxis *axisX = new QCategoryAxis();
    for (int i = 0; i < months.size(); ++i)
        axisX->append(months[i], i);
    axisX->setLabelsColor(Qt::white);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setLabelsColor(Qt::white);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    line->attachAxis(axisX);
    line->attachAxis(axisY);
    dots->attachAxis(axisX);
    dots->attachAxis(axisY);

    this->setRenderHint(QPainter::Antialiasing);

    this->setStyleSheet("background-color: #272727; border-radius: 18px;");

    this->setFixedSize(551, 231);
}
