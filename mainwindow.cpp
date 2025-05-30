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
#include <QFileDialog>
#include <QDesktopServices>
#include <QButtonGroup>
#include "marks_dialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->searchResultsList->hide();
    ui->icon_text_widget->setHidden(true);
    ui->teachers_dropdown->setHidden(true);
    ui->dashboard1->setChecked(true);

    connect(ui->dashboard1, &QPushButton::clicked, this, &MainWindow::switch_to_dashboard_page);
    connect(ui->dashboard2, &QPushButton::clicked, this, &MainWindow::switch_to_dashboard_page);

    connect(ui->institution1, &QPushButton::clicked, this, &MainWindow::switch_to_institution_page);
    connect(ui->institution2, &QPushButton::clicked, this, &MainWindow::switch_to_institution_page);

    connect(ui->t_info, &QPushButton::clicked, this, &MainWindow::switch_to_tinfo_page);

    connect(ui->t_salaries, &QPushButton::clicked, this, &MainWindow::switch_to_tsalaries_page);

    connect(ui->t_transactions, &QPushButton::clicked, this, &MainWindow::switch_to_ttransaction_page);
    ui->teacher2->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->teacher2, &QPushButton::clicked, this, &MainWindow::teachers_context_menu);

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

    connect(ui->uploadButton, &QPushButton::clicked, this, &MainWindow::on_uploadButton_clicked);
    connect(ui->Add_PDF, &QPushButton::clicked, this, &MainWindow::on_addPdfButton_clicked);
    connect(ui->assignMarksButton, &QPushButton::clicked, this, &MainWindow::on_assignMarksButton_clicked);




    connect(ui->searchResultsList, &QListWidget::currentItemChanged, this, &MainWindow::onStudentSelectedFromSearch);
    connect(ui->btn_saveFee, &QPushButton::clicked, this, &MainWindow::on_btn_saveFee_clicked);
    connect(ui->searchResultsList, &QListWidget::itemClicked, this, &MainWindow::loadFeesForSelectedStudent);


    connectToDatabase();
    loadAttendanceForm();
    loadStudentData();
    loadStudentProgressChart();
    loadTasks();
    setupFeeTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::switch_to_dashboard_page(){
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::switch_to_institution_page(){
    ui->stackedWidget->setCurrentIndex(1);
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

void MainWindow::teachers_context_menu() {
    QStringList menuItems = {"Teacher Information", "Teacher Salaries", "Student Attendance"};
    show_custom_context_menu(ui->teacher2, menuItems);
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

    if (text == "Teacher Information") {
        switch_to_tinfo_page();
    } else if (text == "Teacher Salaries") {
        switch_to_tsalaries_page();
    }
    else if (text == "Student Attendance") {
        switch_to_ttransaction_page();
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
            loadStudentData();
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
    currentInfo.address = "";
    currentInfo.gender = "";

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

        QSqlQuery idQuery;
        idQuery.prepare("SELECT id FROM students WHERE roll_no = :roll_no");
        idQuery.bindValue(":roll_no", rollNo);

        if (!idQuery.exec() || !idQuery.next()) {
            qDebug() << "Failed to fetch student_id for roll no" << rollNo << ":" << idQuery.lastError().text();
            continue;
        }

        int studentId = idQuery.value(0).toInt();

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
        query.prepare("INSERT INTO attendance (student_id, name, roll_no, date, status) "
                      "VALUES (:student_id, :name, :roll_no, :date, :status)");
        query.bindValue(":student_id", studentId);
        query.bindValue(":name", name);
        query.bindValue(":roll_no", rollNo);
        query.bindValue(":date", currentDate);
        query.bindValue(":status", status);

        if (!query.exec()) {
            qDebug() << "Failed to insert attendance for roll no" << rollNo << ":" << query.lastError().text();
        } else {
            qDebug() << "Attendance marked for" << name << "Roll No:" << rollNo << "Status:" << status;
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


        QSqlQuery totalDaysQuery;
        totalDaysQuery.prepare("SELECT COUNT(*) FROM attendance WHERE roll_no = :roll_no");
        totalDaysQuery.bindValue(":roll_no", rollNo);
        totalDaysQuery.exec();
        totalDaysQuery.next();
        int totalDays = totalDaysQuery.value(0).toInt();

        QSqlQuery presentDaysQuery;
        presentDaysQuery.prepare("SELECT COUNT(*) FROM attendance WHERE roll_no = :roll_no AND status = 'Present'");
        presentDaysQuery.bindValue(":roll_no", rollNo);
        presentDaysQuery.exec();
        presentDaysQuery.next();
        int presentDays = presentDaysQuery.value(0).toInt();

        int percentage = 0;
        if (totalDays > 0)
            percentage = (presentDays * 100) / totalDays;

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

void MainWindow::loadStudentProgressChart() {
    QString studentName = ui->studentNameLabel->text().trimmed();
    if (studentName.isEmpty()) {
        qDebug() << "Student name label is empty.";
        return;
    }

    QSqlQuery idQuery;
    idQuery.prepare("SELECT id FROM students WHERE name = :name");
    idQuery.bindValue(":name", studentName);

    if (!idQuery.exec() || !idQuery.next()) {
        qDebug() << "Failed to find student ID for name:" << studentName;
        return;
    }

    int studentId = idQuery.value(0).toInt();

    QList<int> progressData;
    QStringList months = {"Sep", "Oct", "Nov", "Dec", "Jan", "Feb", "Mar", "Apr", "May"};

    QSqlQuery marksQuery;
    marksQuery.prepare("SELECT test_date, marks_obtained, total_marks FROM marks WHERE student_id = :id");
    marksQuery.bindValue(":id", studentId);

    if (!marksQuery.exec()) {
        qDebug() << "Failed to fetch marks:" << marksQuery.lastError().text();
        return;
    }

    QMap<QString, double> totalPercentageMap;
    QMap<QString, int> countMap;

    while (marksQuery.next()) {
        QDate date = marksQuery.value("test_date").toDate();
        QString month = date.toString("MMM");
        int obtained = marksQuery.value("marks_obtained").toInt();
        int total = marksQuery.value("total_marks").toInt();

        if (total > 0) {
            double percentage = (double)obtained / total * 100.0;
            totalPercentageMap[month] += percentage;
            countMap[month]++;
        }
    }

    for (const QString &month : months) {
        int count = countMap.value(month, 0);
        double avg = count > 0 ? totalPercentageMap.value(month) / count : 0;
        progressData.append(static_cast<int>(avg));
    }

    auto *progressChart = new StudentProgressChart(progressData);

    QLayout *oldLayout = ui->chartContainer->layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    auto *layout = new QVBoxLayout(ui->chartContainer);
    layout->addWidget(progressChart);
}




void MainWindow::onStudentSelectedFromSearch(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);

    if (!current) return;

    QString selectedStudent = current->text();

    ui->studentNameLabel->setText(selectedStudent);

    loadStudentProgressChart();

    ui->searchResultsList->hide();
}

void MainWindow::on_addPdfButton_clicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Select PDF File", "", "PDF Files (*.pdf)");
    if (!filePath.isEmpty()) {
        selectedPdfPath = filePath;
        QMessageBox::information(this, "PDF Selected", "Selected PDF:\n" + filePath);
    } else {
        selectedPdfPath = "";
    }
}


void MainWindow::on_uploadButton_clicked() {
    QString topic = ui->topicLineEdit->text();
    QString taskType = ui->typeComboBox->currentText();
    QString subject = ui->subjectComboBox->currentText();
    QString className = ui->classComboBox->currentText();
    QDate dueDate = ui->dueDateEdit->date();

    if (selectedPdfPath.isEmpty()) {
        QMessageBox::warning(this, "Missing File", "Please add a PDF file before uploading.");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO tasks (topic, task_type, subject, class, due_date, pdf_path) "
                  "VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(topic);
    query.addBindValue(taskType);
    query.addBindValue(subject);
    query.addBindValue(className);
    query.addBindValue(dueDate);
    query.addBindValue(selectedPdfPath);

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Task uploaded successfully.");
    } else {
        QMessageBox::critical(this, "Error", "Failed to upload task:\n" + query.lastError().text());
    }
}

void MainWindow::on_assignMarksButton_clicked()
{
    QString subject = ui->SubQBox->currentText();
    QString type = ui->TypeQBox->currentText();
    QString className = ui->ClassQBox->currentText();
    QString details = ui->detailsLineEdit->text();
    QDate testDate = ui->testDateEdit->date();
    int totalMarks = ui->totalMarksLineEdit->text().toInt();

    Marks_Dialog *dialog = new Marks_Dialog(subject, type, className, details, testDate, totalMarks, this);
    dialog->exec();
}

QList<QVariantMap> MainWindow::getTasks()
{
    QList<QVariantMap> tasksList;
    QSqlQuery query("SELECT * FROM tasks ORDER BY due_date");
    while (query.next()) {
        QVariantMap task;
        task["id"] = query.value("id");
        task["topic"] = query.value("topic");
        task["task_type"] = query.value("task_type");
        task["subject"] = query.value("subject");
        task["class"] = query.value("class");
        task["due_date"] = query.value("due_date");
        task["pdf_path"] = query.value("pdf_path");
        tasksList.append(task);
    }
    return tasksList;
}

void MainWindow::loadTasks() {
    QVBoxLayout *layout = new QVBoxLayout(ui->tasksWidget);
    QSqlQuery query("SELECT * FROM tasks ORDER BY due_date");

    while (query.next()) {
        QString topic = query.value("topic").toString();
        QString type = query.value("task_type").toString();
        QString subject = query.value("subject").toString();
        QString dueDate = query.value("due_date").toDate().toString("yyyy-MM-dd");
        QString pdfPath = query.value("pdf_path").toString();

        QLabel *label = new QLabel(QString("%1 | %2 | %3 | Due: %4")
                                       .arg(type.toUpper(), subject, topic, dueDate));
        label->setStyleSheet("font-weight: bold; margin-bottom: 5px;");

        QPushButton *openButton = new QPushButton("Open PDF");
        connect(openButton, &QPushButton::clicked, this, [pdfPath]() {
            QDesktopServices::openUrl(QUrl::fromLocalFile(pdfPath));
        });

        QWidget *taskContainer = new QWidget;
        QHBoxLayout *taskLayout = new QHBoxLayout(taskContainer);
        taskLayout->addWidget(label);
        taskLayout->addStretch();
        taskLayout->addWidget(openButton);
        taskContainer->setStyleSheet("margin-bottom: 10px;");

        layout->addWidget(taskContainer);
    }

    layout->addStretch();
}

void MainWindow::clearLayout(QLayout *layout)
{
    if (!layout)
        return;

    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

// Get selected student name from the list widget
QString MainWindow::getSelectedStudentName() {
    QListWidgetItem* selectedItem = ui->searchResultsList->currentItem();
    if (!selectedItem) return QString();
    return selectedItem->text();
}

// Get student ID by querying the database using the student's name
int MainWindow::getStudentIdByName(const QString &name) {
    if (name.isEmpty()) return -1;

    QSqlQuery query;
    query.prepare("SELECT id FROM students WHERE name = :name LIMIT 1");
    query.bindValue(":name", name);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return -1;  // student not found
}

// Load fees for the currently selected student (by name -> id -> fees)
void MainWindow::loadFeesForSelectedStudent() {
    QString studentName = getSelectedStudentName();
    if (studentName.isEmpty()) return;

    int studentId = getStudentIdByName(studentName);
    if (studentId == -1) {
        qDebug() << "Student not found with name:" << studentName;
        ui->fee_table->setRowCount(0);
        ui->label_totalPaid->setText("Total Paid: 0");
        ui->label_outstanding->setText("Outstanding: 0");
        ui->label_lastPaid->setText("Last Paid Date: N/A");
        return;
    }

    qDebug() << "Loading fees for student ID:" << studentId;

    QSqlQuery query;
    query.prepare("SELECT id, month, due_date, amount, status, paid_date FROM fees WHERE student_id = :student_id");
    query.bindValue(":student_id", studentId);

    if (!query.exec()) {
        qDebug() << "Failed to load fees:" << query.lastError();
        return;
    }

    ui->fee_table->setRowCount(0); // Clear previous data

    double totalPaid = 0.0;
    double outstanding = 0.0;
    QDate lastPaidDate;

    while (query.next()) {
        QString month = query.value("month").toString();
        QDate dueDate = query.value("due_date").toDate();
        double amount = query.value("amount").toDouble();
        QString status = query.value("status").toString();
        QDate paidDate = query.value("paid_date").toDate();

        addFeeRow(month, dueDate, amount, status);

        if (status == "Paid") {
            totalPaid += amount;
            if (!paidDate.isNull() && (lastPaidDate.isNull() || paidDate > lastPaidDate))
                lastPaidDate = paidDate;
        } else {
            outstanding += amount;
        }
    }

    ui->label_totalPaid->setText(QString::number(totalPaid));
    ui->label_outstanding->setText(QString::number(outstanding));
    ui->label_lastPaid->setText((lastPaidDate.isValid() ? lastPaidDate.toString("yyyy-MM-dd") : "N/A"));
}

// Add a single fee row in the fee_table
void MainWindow::addFeeRow(QString month, QDate dueDate, double amount, QString fee_status) {
    int row = ui->fee_table->rowCount();
    ui->fee_table->insertRow(row);

    ui->fee_table->setItem(row, 0, new QTableWidgetItem(month));
    ui->fee_table->setItem(row, 1, new QTableWidgetItem(dueDate.toString("yyyy-MM-dd")));
    ui->fee_table->setItem(row, 2, new QTableWidgetItem(QString::number(amount)));
    ui->fee_table->setItem(row, 3, new QTableWidgetItem(fee_status));

    QPushButton* btn = new QPushButton("Mark Paid");
    btn->setEnabled(fee_status != "Paid");
    ui->fee_table->setCellWidget(row, 4, btn);

    // Connect button click to markFeePaid function for this row
    connect(btn, &QPushButton::clicked, [=]() {
        markFeePaid(row);
    });
}

// Mark a fee as paid for the selected student and fee row
void MainWindow::markFeePaid(int row) {
    QString studentName = getSelectedStudentName();
    if (studentName.isEmpty()) return;

    int studentId = getStudentIdByName(studentName);
    if (studentId == -1) return;

    QString month = ui->fee_table->item(row, 0)->text();

    QSqlQuery query;
    query.prepare("UPDATE fees SET status = 'Paid', paid_date = CURDATE() WHERE student_id = :student_id AND month = :month");
    query.bindValue(":student_id", studentId);
    query.bindValue(":month", month);

    if (!query.exec()) {
        qDebug() << "Failed to mark paid:" << query.lastError();
        return;
    }

    // Update UI directly
    ui->fee_table->item(row, 3)->setText("Paid");
    QWidget* widget = ui->fee_table->cellWidget(row, 4);
    if (QPushButton* btn = qobject_cast<QPushButton*>(widget)) {
        btn->setEnabled(false);
    }

    loadFeesForSelectedStudent(); // Refresh the table and summary labels
}

// Insert a new fee record for the selected student
void MainWindow::on_btn_saveFee_clicked() {
    QString studentName = getSelectedStudentName();
    if (studentName.isEmpty()) return;

    int studentId = getStudentIdByName(studentName);
    if (studentId == -1) return;

    QString month = ui->combo_month->currentText();
    QDate dueDate = ui->date_due->date();
    double amount = ui->line_amount->text().toDouble();

    QSqlQuery query;
    query.prepare("INSERT INTO fees (student_id, month, due_date, amount) VALUES (:student_id, :month, :due_date, :amount)");
    query.bindValue(":student_id", studentId);
    query.bindValue(":month", month);
    query.bindValue(":due_date", dueDate);
    query.bindValue(":amount", amount);

    if (!query.exec()) {
        qDebug() << "Insert failed:" << query.lastError();
        return;
    }

    loadFeesForSelectedStudent();
}

// Setup fee table headers and layout
void MainWindow::setupFeeTable() {
    ui->fee_table->setColumnCount(5);
    QStringList headers = {"Month", "Due Date", "Amount", "Status", "Actions"};
    ui->fee_table->setHorizontalHeaderLabels(headers);
    ui->fee_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
