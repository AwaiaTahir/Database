#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QButtonGroup>
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
    void switch_to_tinfo_page();
    void switch_to_tsalaries_page();
    void switch_to_ttransaction_page();
    void teachers_context_menu();
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
    void on_uploadButton_clicked();
    void on_addPdfButton_clicked();
    void on_assignMarksButton_clicked();
    AttendanceCircleWidget *attendanceChart;
    QString selectedPdfPath;
    QList<QVariantMap> getTasks();
    void loadTasks();
    void clearLayout(QLayout *layout);
    QString getSelectedStudentName();
    int getStudentIdByName(const QString &name);
    void loadFeesForSelectedStudent();
    void addFeeRow(QString month, QDate dueDate, double amount, QString fee_status);
    void markFeePaid(int row);
    void setupFeeTable();
    void on_btn_saveFee_clicked();
};
#endif // MAINWINDOW_H
