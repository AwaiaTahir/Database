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
#include <QMessageBox>
#include <QCheckBox>
#include "studentprogresschart.h"
#include <QVBoxLayout>

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

    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::students_context_menu); //customContextMenuRequested for Right Click
    connect(ui->teacher2, &QPushButton::clicked, this, &MainWindow::teachers_context_menu);
    connect(ui->finance2, &QPushButton::clicked, this, &MainWindow::finance_context_menu);

    connect(ui->Add_Student, &QPushButton::clicked, this, &MainWindow::Open_Add_Student_Dialog);

    connect(ui->searchBar, &QLineEdit::textChanged, this, &MainWindow::searchStudents);
    connect(ui->searchResultsList, &QListWidget::itemClicked, this, &MainWindow::loadSelectedStudent);


    attendanceChart = new AttendanceCircleWidget(this);
    QVBoxLayout *vLayout = new QVBoxLayout(ui->frame_attendance);
    vLayout->addWidget(attendanceChart);


    connect(ui->deleteStudentButton, &QPushButton::clicked, this, &MainWindow::deleteCurrentStudent);

    connect(ui->editStudentButton, &QPushButton::clicked, this, &MainWindow::editCurrentStudent);
    connect(ui->submitAttendanceButton, &QPushButton::clicked, this, &MainWindow::submitAttendance);

    ui->attendanceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QList<int> data = {30, 55, 60, 40, 25, 50, 20, 35, 40};
    auto *progressChart = new StudentProgressChart(data);

    auto *layout = new QVBoxLayout(ui->chartContainer);
    layout->addWidget(progressChart);

    connectToDatabase();
    loadAttendanceForm();
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
    QStringList menuItems = {"Teacher Information", "Teacher Salaries", "Student Attendance"};
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
    else if (text == "Student Attendance") {
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
    query.prepare("SELECT roll_no, name, phone, email, class, dob, image_path, attendance FROM students WHERE name = :name");
    query.bindValue(":name", selectedName);

    if (query.exec() && query.next()) {
        int rollNo = query.value(0).toInt();
        ui->studentNameLabel->setText(query.value(1).toString());
        ui->studentPhoneLabel->setText(query.value(2).toString());
        ui->studentEmailLabel->setText(query.value(3).toString());
        ui->studentClassLabel->setText(query.value(4).toString());
        ui->studentDOBLabel->setText(query.value(5).toDate().toString("dd-MM-yyyy"));
        attendanceChart->setPercentage(query.value(7).toInt());

        QString imagePath = query.value(6).toString();
        QPixmap pix(imagePath);
        ui->studentPhotoLabel->setPixmap(pix.scaled(100, 100, Qt::KeepAspectRatio));

        ui->searchResultsList->hide();

        // --- Monthly Attendance Stats ---
        QDate currentDate = QDate::currentDate();
        QString currentMonth = currentDate.toString("yyyy-MM");

        QSqlQuery attendanceQuery;
        attendanceQuery.prepare("SELECT status FROM attendance WHERE roll_no = :roll AND DATE_FORMAT(date, '%Y-%m') = :month");
        attendanceQuery.bindValue(":roll", rollNo);
        attendanceQuery.bindValue(":month", currentMonth);

        int presentCount = 0;
        int absentCount = 0;

        if (attendanceQuery.exec()) {
            while (attendanceQuery.next()) {
                QString status = attendanceQuery.value(0).toString();
                if (status == "Present") presentCount++;
                else if (status == "Absent") absentCount++;
            }

            int total = presentCount + absentCount;
            double percentage = total > 0 ? (presentCount * 100.0 / total) : 0;

            QString statusLabel;
            QString color;

            if (percentage >= 90) {
                statusLabel = "Excellent";
                color = "green";
            } else if (percentage >= 75) {
                statusLabel = "Good";
                color = "#4caf50";
            } else if (percentage >= 60) {
                statusLabel = "Fair";
                color = "#ff9800";
            } else {
                statusLabel = "Poor";
                color = "red";
            }

            // Update attendance stat labels
            ui->label_presentCount->setText(QString::number(presentCount));
            ui->label_absentCount->setText(QString::number(absentCount));
            ui->label_attendanceStatus->setText(statusLabel);
            ui->label_attendanceStatus->setStyleSheet("color: white; background-color: " + color + "; padding: 5px; border-radius: 5px;");
        } else {
            qDebug() << "Failed to fetch monthly attendance:" << attendanceQuery.lastError().text();
        }

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

    // Confirmation prompt
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  "Confirm Deletion",
                                  QString("Are you sure you want to delete student '%1'?").arg(studentName),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
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
            attendanceChart->setPercentage(0);
            loadStudentData(); // Refresh list if applicable
        } else {
            qDebug() << "Failed to delete student:" << query.lastError().text();
        }
    } else {
        qDebug() << "Deletion cancelled.";
    }
}


void MainWindow::editCurrentStudent() {
    QString studentName = ui->studentNameLabel->text();
    if (studentName.isEmpty()) {
        qDebug() << "No student selected to edit.";
        return;
    }

    QString imagePath;
    QSqlQuery query;
    query.prepare("SELECT image_path FROM students WHERE name = :name");
    query.bindValue(":name", studentName);
    if (query.exec() && query.next()) {
        imagePath = query.value("image_path").toString();
    }

    StudentInfo currentInfo;
    currentInfo.name = studentName;
    currentInfo.phone = ui->studentPhoneLabel->text();
    currentInfo.email = ui->studentEmailLabel->text();
    currentInfo.className = ui->studentClassLabel->text();
    currentInfo.dob = QDate::fromString(ui->studentDOBLabel->text(), "dd-MM-yyyy");
    currentInfo.imagePath = imagePath;
    currentInfo.address = "";  // Add if shown in UI
    currentInfo.gender = "";   // Add if shown in UI

    // Open edit dialog
    Dialog dialog(this);
    dialog.setWindowTitle("Edit Student Info");
    dialog.setStudentInfo(currentInfo);

    if (dialog.exec() == QDialog::Accepted) {
        StudentInfo updated = dialog.getStudentInfo();

        QSqlQuery updateQuery;
        updateQuery.prepare(R"(
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

        updateQuery.bindValue(":address", updated.address);
        updateQuery.bindValue(":phone", updated.phone);
        updateQuery.bindValue(":email", updated.email);
        updateQuery.bindValue(":gender", updated.gender);
        updateQuery.bindValue(":class", updated.className);
        updateQuery.bindValue(":dob", updated.dob.toString("yyyy-MM-dd"));
        updateQuery.bindValue(":image_path", updated.imagePath);
        updateQuery.bindValue(":name", studentName);

        if (updateQuery.exec()) {
            qDebug() << "Student info updated.";
            loadStudentData();
        } else {
            qDebug() << "Update failed:" << updateQuery.lastError().text();
        }
    }
}

void MainWindow::loadAttendanceForm() {
    QTableWidget *table = ui->attendanceTable;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Name", "Roll No", "Present"});

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Database not open!";
        return;
    }

    QSqlQuery query("SELECT name, roll_no FROM students");
    int row = 0;

    while (query.next()) {
        QString name = query.value("name").toString();
        int rollNo = query.value("roll_no").toInt();

        table->insertRow(row);

        QTableWidgetItem *nameItem = new QTableWidgetItem(name);
        QTableWidgetItem *rollItem = new QTableWidgetItem(QString::number(rollNo));
        nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);
        rollItem->setFlags(rollItem->flags() ^ Qt::ItemIsEditable);

        table->setItem(row, 0, nameItem);
        table->setItem(row, 1, rollItem);

        QCheckBox *checkBox = new QCheckBox();
        checkBox->setChecked(true);
        table->setCellWidget(row, 2, checkBox);

        row++;
    }

    table->resizeColumnsToContents();
}



void MainWindow::submitAttendance() {
    QTableWidget *table = ui->attendanceTable;
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isOpen()) {
        qDebug() << "Database not open!";
        return;
    }

    QDate currentDate = QDate::currentDate();

    for (int row = 0; row < table->rowCount(); ++row) {
        QString name = table->item(row, 0)->text();
        int rollNo = table->item(row, 1)->text().toInt();

        QWidget *widget = table->cellWidget(row, 2);
        QCheckBox *checkBox = qobject_cast<QCheckBox *>(widget);

        QString status = (checkBox && checkBox->isChecked()) ? "Present" : "Absent";

        // Prevent duplicate entry for same date and student
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT COUNT(*) FROM attendance WHERE roll_no = :roll_no AND date = :date");
        checkQuery.bindValue(":roll_no", rollNo);
        checkQuery.bindValue(":date", currentDate);
        checkQuery.exec();
        checkQuery.next();
        if (checkQuery.value(0).toInt() > 0) {
            qDebug() << "Attendance already exists for roll no" << rollNo << "on" << currentDate;
            continue;
        }

        QSqlQuery query;
        query.prepare("INSERT INTO attendance (name, roll_no, date, status) VALUES (:name, :roll_no, :date, :status)");
        query.bindValue(":name", name);
        query.bindValue(":roll_no", rollNo);
        query.bindValue(":date", currentDate);
        query.bindValue(":status", status);

        if (!query.exec()) {
            qDebug() << "Failed to insert attendance:" << query.lastError().text();
        }
    }

    updateAttendancePercentages();

    QMessageBox::information(this, "Success", "Attendance submitted and percentages updated.");
}


void MainWindow::updateAttendancePercentages() {
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Database not open!";
        return;
    }

    QSqlQuery getStudents("SELECT roll_no FROM students");

    while (getStudents.next()) {
        int rollNo = getStudents.value(0).toInt();

        // Get total days this student has attendance records
        QSqlQuery totalDaysQuery;
        totalDaysQuery.prepare("SELECT COUNT(*) FROM attendance WHERE roll_no = :roll_no");
        totalDaysQuery.bindValue(":roll_no", rollNo);
        totalDaysQuery.exec();
        totalDaysQuery.next();
        int totalDays = totalDaysQuery.value(0).toInt();

        // Get present count
        QSqlQuery presentDaysQuery;
        presentDaysQuery.prepare("SELECT COUNT(*) FROM attendance WHERE roll_no = :roll_no AND status = 'Present'");
        presentDaysQuery.bindValue(":roll_no", rollNo);
        presentDaysQuery.exec();
        presentDaysQuery.next();
        int presentDays = presentDaysQuery.value(0).toInt();

        int percentage = 0;
        if (totalDays > 0)
            percentage = (presentDays * 100) / totalDays;

        // Update student table
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE students SET attendance = :percentage WHERE roll_no = :roll_no");
        updateQuery.bindValue(":percentage", percentage);
        updateQuery.bindValue(":roll_no", rollNo);

        if (!updateQuery.exec()) {
            qDebug() << "Failed to update percentage for roll no" << rollNo << ":" << updateQuery.lastError().text();
        } else {
            qDebug() << "Updated attendance for roll no" << rollNo << "→" << percentage << "%";
        }
    }
}
