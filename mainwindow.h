#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "AttendanceCircleWidget.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void switch_to_dashboard_page();
    void switch_to_institution_page();
    void switch_to_sinfo_page();
    void switch_to_stransaction_page();
    void switch_to_spayment_page();
    void switch_to_tinfo_page();
    void switch_to_tsalaries_page();
    void switch_to_ttransaction_page();
    void switch_to_fbudget_page();
    void switch_to_fexpenses_page();
    void switch_to_foverview_page();
    void students_context_menu();
    void teachers_context_menu();
    void finance_context_menu();
    void show_custom_context_menu(QWidget *button, const QStringList &menu_items);
    void handle_menu_item_click();
    void Open_Add_Student_Dialog();
    void connectToDatabase();
    void loadStudentData();
    void searchStudents(const QString &text);
    void loadSelectedStudent(QListWidgetItem *item);
    void deleteCurrentStudent();
    void editCurrentStudent();
    void loadAttendanceForm();
    void submitAttendance();
    void updateAttendancePercentages();
    void loadStudentProgressChart();
    void onStudentSelectedFromSearch(QListWidgetItem *current, QListWidgetItem *previous);
    void updateMonthlyAttendanceStats(QListWidgetItem *item);
    void Open_marks_dialog();
    AttendanceCircleWidget *attendanceChart;

};
#endif // MAINWINDOW_H
