#ifndef STUDENTPROGRESSCHART_H
#define STUDENTPROGRESSCHART_H

#include <QWidget>
#include <QChartView>
#include <QLineSeries>
#include <QCategoryAxis>
#include <QValueAxis>
#include <QScatterSeries>
#include <QChart>

class StudentProgressChart : public QChartView
{
    Q_OBJECT
public:
    explicit StudentProgressChart(const QList<int> &progressData, QWidget *parent = nullptr);
};

#endif // STUDENTPROGRESSCHART_H
