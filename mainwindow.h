#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
};
#endif // MAINWINDOW_H
