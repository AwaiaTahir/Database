#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->icon_text_widget->setHidden(true);
    ui->students_dropdown->setHidden(true);
    ui->teachers_dropdown->setHidden(true);
    ui->finance_dropdown->setHidden(true);

    connect(ui->dashboard1, &QPushButton::clicked, this, &MainWindow::switch_to_dashboard_page);
    connect(ui->dashboard2, &QPushButton::clicked, this, &MainWindow::switch_to_dashboard_page);

    connect(ui->institution1, &QPushButton::clicked, this, &MainWindow::switch_to_institution_page);
    connect(ui->institution2, &QPushButton::clicked, this, &MainWindow::switch_to_institution_page);

    connect(ui->s_info, &QPushButton::clicked, this, &MainWindow::switch_to_sinfo_page);
    // connect(ui->institution2, &QPushButton::clicked, this, &MainWindow::switch_to_sinfo_page);

    connect(ui->s_transactions, &QPushButton::clicked, this, &MainWindow::switch_to_stransaction_page);
    // connect(ui->institution2, &QPushButton::clicked, this, &MainWindow::switch_to_stransaction_page);

    connect(ui->s_payment, &QPushButton::clicked, this, &MainWindow::switch_to_spayment_page);
    // connect(ui->institution2, &QPushButton::clicked, this, &MainWindow::switch_to_spayment_page);

    connect(ui->t_info, &QPushButton::clicked, this, &MainWindow::switch_to_tinfo_page);
    // connect(ui->institution2, &QPushButton::clicked, this, &MainWindow::switch_to_tinfo_page);

    connect(ui->t_salaries, &QPushButton::clicked, this, &MainWindow::switch_to_tsalaries_page);
    // connect(ui->institution2, &QPushButton::clicked, this, &MainWindow::switch_to_tsalaries_page);

    connect(ui->t_transactions, &QPushButton::clicked, this, &MainWindow::switch_to_ttransaction_page);
    // connect(ui->institution2, &QPushButton::clicked, this, &MainWindow::switch_to_ttransaction_page);

    connect(ui->f_budgets, &QPushButton::clicked, this, &MainWindow::switch_to_fbudget_page);
    // connect(ui->institution2, &QPushButton::clicked, this, &MainWindow::switch_to_fbudget_page);

    connect(ui->f_expenses, &QPushButton::clicked, this, &MainWindow::switch_to_fexpenses_page);
    // connect(ui->institution2, &QPushButton::clicked, this, &MainWindow::switch_to_fexpenses_page);

    connect(ui->f_overview, &QPushButton::clicked, this, &MainWindow::switch_to_foverview_page);
    // connect(ui->institution2, &QPushButton::clicked, this, &MainWindow::switch_to_foverview_page);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::switch_to_dashboard_page(){
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::switch_to_institution_page(){
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::switch_to_sinfo_page(){
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::switch_to_stransaction_page(){
    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::switch_to_spayment_page(){
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::switch_to_tinfo_page(){
    ui->stackedWidget->setCurrentIndex(5);
}

void MainWindow::switch_to_tsalaries_page(){
    ui->stackedWidget->setCurrentIndex(6);
}

void MainWindow::switch_to_ttransaction_page(){
    ui->stackedWidget->setCurrentIndex(7);
}

void MainWindow::switch_to_fbudget_page(){
    ui->stackedWidget->setCurrentIndex(9);
}

void MainWindow::switch_to_fexpenses_page(){
    ui->stackedWidget->setCurrentIndex(8);
}

void MainWindow::switch_to_foverview_page(){
    ui->stackedWidget->setCurrentIndex(10);
}
