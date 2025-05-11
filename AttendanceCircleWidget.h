#ifndef ATTENDANCECIRCLEWIDGET_H
#define ATTENDANCECIRCLEWIDGET_H

#include <QWidget>

class AttendanceCircleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AttendanceCircleWidget(QWidget *parent = nullptr);
    void setPercentage(int percent);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_percentage;
};

#endif // ATTENDANCECIRCLEWIDGET_H
