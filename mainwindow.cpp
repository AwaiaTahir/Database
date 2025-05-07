#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include "dialog.h"
#include <QMenu>
#include <QPushButton>
#include <QCursor>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>


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

    connect(ui->s_transactions, &QPushButton::clicked, this, &MainWindow::switch_to_stransaction_page);

    connect(ui->s_payment, &QPushButton::clicked, this, &MainWindow::switch_to_spayment_page);

    connect(ui->t_info, &QPushButton::clicked, this, &MainWindow::switch_to_tinfo_page);

    connect(ui->t_salaries, &QPushButton::clicked, this, &MainWindow::switch_to_tsalaries_page);

    connect(ui->t_transactions, &QPushButton::clicked, this, &MainWindow::switch_to_ttransaction_page);

    connect(ui->f_budgets, &QPushButton::clicked, this, &MainWindow::switch_to_fbudget_page);

    connect(ui->f_expenses, &QPushButton::clicked, this, &MainWindow::switch_to_fexpenses_page);

    connect(ui->f_overview, &QPushButton::clicked, this, &MainWindow::switch_to_foverview_page);

    ui->pushButton_4->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->teacher2->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->finance2->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->pushButton_4, &QPushButton::customContextMenuRequested, this, &MainWindow::students_context_menu);
    connect(ui->teacher2, &QPushButton::customContextMenuRequested, this, &MainWindow::teachers_context_menu);
    connect(ui->finance2, &QPushButton::customContextMenuRequested, this, &MainWindow::finance_context_menu);

    connect(ui->Add_Student, &QPushButton::clicked, this, &MainWindow::Open_Add_Student_Dialog);

    connectToDatabase();
    loadStudentData();
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


void MainWindow::students_context_menu() {
    QStringList menuItems = {"Student Information", "Student Payments", "Student Transactions"};
    show_custom_context_menu(ui->pushButton_4, menuItems);
}

void MainWindow::teachers_context_menu() {
    QStringList menuItems = {"Teacher Information", "Teacher Salaries", "Teacher Transactions"};
    show_custom_context_menu(ui->teacher2, menuItems);
}

void MainWindow::finance_context_menu() {
    QStringList menuItems = {"Budgets", "Expenses", "Bussiness Overview"};
    show_custom_context_menu(ui->finance2, menuItems);
}

void MainWindow::show_custom_context_menu(QWidget *button, const QStringList &menu_items) {
    QMenu *menu = new QMenu(button);

    menu->setStyleSheet(
        "QMenu {"
        "  background-color: black;"
        "  color: white;"
        "}"
        "QMenu::item:selected {"
        "  background-color: white;"
        "  color: #12B298;"
        "}"
        );

    for (const QString &item : menu_items) {
        QAction *action = new QAction(item, this);
        connect(action, &QAction::triggered, this, &MainWindow::handle_menu_item_click);
        menu->addAction(action);
    }

    menu->move(button->mapToGlobal(button->rect().topRight()));
    menu->exec();
}


void MainWindow::handle_menu_item_click() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) return;

    QString text = action->text();

    if (text == "Student Information") {
        switch_to_sinfo_page();
    } else if (text == "Student Payments") {
        switch_to_spayment_page();
    } else if (text == "Student Transactions") {
        switch_to_stransaction_page();
    }
    else if (text == "Teacher Information") {
        switch_to_tinfo_page();
    } else if (text == "Teacher Salaries") {
        switch_to_tsalaries_page();
    }
    else if (text == "Teacher Transactions") {
        switch_to_ttransaction_page();
    } else if (text == "Budgets") {
        switch_to_fbudget_page();
    }
    else if (text == "Expenses") {
        switch_to_fexpenses_page();
    } else if (text == "Bussiness Overview") {
        switch_to_foverview_page();
    }

}



void MainWindow::connectToDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setPort(3306);
    db.setDatabaseName("Academy");
    db.setUserName("root");
    db.setPassword("Awais0696");

    if (!db.open()) {
        qDebug() << "Database connection failed:" << db.lastError().text();
    } else {
        qDebug() << "Connected to database successfully.";
    }
}

void MainWindow::Open_Add_Student_Dialog() {
    Dialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        StudentInfo student = dialog.getStudentInfo();

        // 1. Insert into MySQL
        QSqlQuery query;
        query.prepare(R"(
            INSERT INTO students (name, address, phone, email, gender, class, dob, image_path)
            VALUES (:name, :address, :phone, :email, :gender, :class, :dob, :image_path)
        )");

        query.bindValue(":name", student.name);
        query.bindValue(":address", student.address);
        query.bindValue(":phone", student.phone);
        query.bindValue(":email", student.email);
        query.bindValue(":gender", student.gender);
        query.bindValue(":class", student.className);
        query.bindValue(":dob", student.dob.toString("yyyy-MM-dd"));
        query.bindValue(":image_path", student.imagePath);

        if (query.exec()) {
            qDebug() << "✅ Student info inserted into the database.";
        } else {
            qDebug() << "❌ Failed to insert student info:" << query.lastError().text();
        }

        ui->studentNameLabel->setText(student.name);
        ui->studentPhoneLabel->setText(student.phone);
        ui->studentEmailLabel->setText(student.email);
        ui->studentClassLabel->setText(student.className);
        ui->studentDOBLabel->setText(student.dob.toString("dd-MM-yyyy"));

        QPixmap pix(student.imagePath);
        ui->studentPhotoLabel->setPixmap(pix.scaled(100, 100, Qt::KeepAspectRatio));
    }
}

void MainWindow::loadStudentData()
{
    QSqlQuery query("SELECT name, phone, email, class, dob, image_path FROM students");

    if (!query.exec()) {
        qDebug() << "❌ Failed to fetch student data:" << query.lastError().text();
        return;
    }

    if (query.next()) {
        QString name = query.value(0).toString();
        QString phone = query.value(1).toString();
        QString email = query.value(2).toString();
        QString className = query.value(3).toString();
        QDate dob = query.value(4).toDate();
        QString imagePath = query.value(5).toString();

        ui->studentNameLabel->setText(name);
        ui->studentPhoneLabel->setText(phone);
        ui->studentEmailLabel->setText(email);
        ui->studentClassLabel->setText(className);
        ui->studentDOBLabel->setText(dob.toString("dd-MM-yyyy"));

        QPixmap pix(imagePath);
        ui->studentPhotoLabel->setPixmap(pix.scaled(100, 100, Qt::KeepAspectRatio));
        // ui->studentPhotoLabel->setStyleSheet("border-radius : 10px;");
    } else {
        qDebug() << "ℹ️ No student data found.";
    }
}




