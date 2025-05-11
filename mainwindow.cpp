#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "AttendanceCircleWidget.h"
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
    ui->searchResultsList->hide();
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

    connect(ui->searchBar, &QLineEdit::textChanged, this, &MainWindow::searchStudents);
    connect(ui->searchResultsList, &QListWidget::itemClicked, this, &MainWindow::loadSelectedStudent);


    attendanceChart = new AttendanceCircleWidget(this);
    QVBoxLayout *vLayout = new QVBoxLayout(ui->frame_attendance);
    vLayout->addWidget(attendanceChart);


    connect(ui->deleteStudentButton, &QPushButton::clicked, this, &MainWindow::deleteCurrentStudent);

    connect(ui->editStudentButton, &QPushButton::clicked, this, &MainWindow::editCurrentStudent);

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
            qDebug() << "Student info inserted into the database.";
        } else {
            qDebug() << "Failed to insert student info:" << query.lastError().text();
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

void MainWindow::loadSelectedStudent(QListWidgetItem *item)
{
    QString selectedName = item->text();

    QSqlQuery query;
    query.prepare("SELECT name, phone, email, class, dob, image_path, attendance FROM students WHERE name = :name");
    query.bindValue(":name", selectedName);

    if (query.exec() && query.next()) {
        ui->studentNameLabel->setText(query.value(0).toString());
        ui->studentPhoneLabel->setText(query.value(1).toString());
        ui->studentEmailLabel->setText(query.value(2).toString());
        ui->studentClassLabel->setText(query.value(3).toString());
        ui->studentDOBLabel->setText(query.value(4).toDate().toString("dd-MM-yyyy"));
        int attendance = query.value(6).toInt();
        attendanceChart->setPercentage(attendance);

        QString imagePath = query.value(5).toString();
        QPixmap pix(imagePath);
        ui->studentPhotoLabel->setPixmap(pix.scaled(100, 100, Qt::KeepAspectRatio));

        ui->searchResultsList->hide();
    } else {
        qDebug() << "Failed to load selected student info:" << query.lastError().text();
    }
}

void MainWindow::searchStudents(const QString &text)
{
    ui->searchResultsList->clear();

    if (text.trimmed().isEmpty()) {
        ui->searchResultsList->hide();
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT name FROM students WHERE name LIKE :name");
    query.bindValue(":name", "%" + text + "%");

    if (!query.exec()) {
        qDebug() << "Search query failed:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QString name = query.value(0).toString();
        ui->searchResultsList->addItem(name);
    }

    ui->searchResultsList->setVisible(ui->searchResultsList->count() > 0);
}


void MainWindow::loadStudentData()
{
    QSqlQuery query;
    query.prepare("SELECT name, phone, email, class, dob, image_path, attendance FROM students");


    if (!query.exec()) {
        qDebug() << "Failed to fetch student data:" << query.lastError().text();
        return;
    }

    if (query.next()) {
        QString name = query.value(0).toString();
        QString phone = query.value(1).toString();
        QString email = query.value(2).toString();
        QString className = query.value(3).toString();
        QDate dob = query.value(4).toDate();
        QString imagePath = query.value(5).toString();
        int attendance = query.value(6).toInt();
        attendanceChart->setPercentage(attendance);

        ui->studentNameLabel->setText(name);
        ui->studentPhoneLabel->setText(phone);
        ui->studentEmailLabel->setText(email);
        ui->studentClassLabel->setText(className);
        ui->studentDOBLabel->setText(dob.toString("dd-MM-yyyy"));

        QPixmap pix(imagePath);
        ui->studentPhotoLabel->setPixmap(pix.scaled(100, 100, Qt::KeepAspectRatio));
    } else {
        qDebug() << "No student data found.";
    }
}

void MainWindow::deleteCurrentStudent() {
    QString studentName = ui->studentNameLabel->text();

    if (studentName.isEmpty()) {
        qDebug() << "No student selected to delete.";
        return;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM students WHERE name = :name");
    query.bindValue(":name", studentName);

    if (query.exec()) {
        qDebug() << "Student deleted successfully.";
        ui->studentNameLabel->clear();
        ui->studentPhoneLabel->clear();
        ui->studentEmailLabel->clear();
        ui->studentClassLabel->clear();
        ui->studentDOBLabel->clear();
        ui->studentPhotoLabel->clear();
        attendanceChart->setPercentage(0); // Reset attendance
    } else {
        qDebug() << "Failed to delete student:" << query.lastError().text();
    }
}

void MainWindow::editCurrentStudent() {
    QString studentName = ui->studentNameLabel->text();
    if (studentName.isEmpty()) {
        qDebug() << "⚠️ No student selected to edit.";
        return;
    }

    // Load current data into Dialog
    Dialog dialog(this);
    dialog.setWindowTitle("Edit Student Info");

    // Pre-fill the dialog with current student data
    StudentInfo currentInfo;
    currentInfo.name = studentName;
    currentInfo.phone = ui->studentPhoneLabel->text();
    currentInfo.email = ui->studentEmailLabel->text();
    currentInfo.className = ui->studentClassLabel->text();
    currentInfo.dob = QDate::fromString(ui->studentDOBLabel->text(), "dd-MM-yyyy");
    currentInfo.imagePath = "";
    currentInfo.address = "";
    currentInfo.gender = "";

    dialog.setStudentInfo(currentInfo);


    if (dialog.exec() == QDialog::Accepted) {
        StudentInfo updated = dialog.getStudentInfo();

        QSqlQuery query;
        query.prepare(R"(
            UPDATE students SET
            address = :address,
            phone = :phone,
            email = :email,
            gender = :gender,
            class = :class,
            dob = :dob,
            image_path = :image_path
            WHERE name = :name
        )");

        query.bindValue(":address", updated.address);
        query.bindValue(":phone", updated.phone);
        query.bindValue(":email", updated.email);
        query.bindValue(":gender", updated.gender);
        query.bindValue(":class", updated.className);
        query.bindValue(":dob", updated.dob.toString("yyyy-MM-dd"));
        query.bindValue(":image_path", updated.imagePath);
        query.bindValue(":name", studentName);  // Again, better to use ID

        if (query.exec()) {
            qDebug() << "Student info updated.";
            loadStudentData();  // Refresh main view
        } else {
            qDebug() << "Update failed:" << query.lastError().text();
        }
    }
}





