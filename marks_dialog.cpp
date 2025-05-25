#include "marks_dialog.h"
#include "ui_marks_dialog.h"
#include <QCheckBox>
#include <QSqlQuery>
#include <QLineEdit>
#include <QMessageBox>
#include <QSqlError>


Marks_Dialog::Marks_Dialog(QString subject, QString type, QString className,
                           QString details, QDate testDate, int totalMarks,
                           QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Marks_Dialog)
    , m_subject(subject)
    , m_type(type)
    , m_className(className)
    , m_details(details)
    , m_testDate(testDate)
    , m_totalMarks(totalMarks)
{
    ui->setupUi(this);
    ui->marksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(ui->saveButton, &QPushButton::clicked, this, &Marks_Dialog::on_saveButton_clicked);

    loadMarksForm();
}



Marks_Dialog::~Marks_Dialog()
{
    delete ui;
}

void Marks_Dialog::loadMarksForm() {
    QTableWidget *table = ui->marksTable;
    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Name", "Roll No", "Marks"});

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

        QLineEdit *line_edit = new QLineEdit;
        table->setCellWidget(row, 2, line_edit);

        row++;
    }

    table->resizeColumnsToContents();
}

void Marks_Dialog::on_saveButton_clicked() {
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        QMessageBox::warning(this, "Database Error", "Database not open!");
        return;
    }

    QSqlQuery query;
    QTableWidget *table = ui->marksTable;
    int rows = table->rowCount();

    for (int i = 0; i < rows; ++i) {
        int roll_no = table->item(i, 1)->text().toInt();
        QLineEdit *marksEdit = qobject_cast<QLineEdit *>(table->cellWidget(i, 2));
        if (!marksEdit) continue;

        bool ok;
        int obtainedMarks = marksEdit->text().toInt(&ok);
        if (!ok) obtainedMarks = 0;

        if (obtainedMarks > m_totalMarks) {
            QMessageBox::warning(this, "Validation Error",
                                 QString("Obtained marks for roll no %1 exceed total marks (%2).")
                                     .arg(roll_no).arg(m_totalMarks));
            return;
        }

        QSqlQuery idQuery;
        idQuery.prepare("SELECT id FROM students WHERE roll_no = :roll");
        idQuery.bindValue(":roll", roll_no);
        if (!idQuery.exec() || !idQuery.next()) {
            qDebug() << "Error finding student_id for roll no" << roll_no;
            continue;
        }

        int student_id = idQuery.value(0).toInt();

        query.prepare("INSERT INTO marks (student_id, subject, type, class, details, test_date, total_marks, marks_obtained) "
                      "VALUES (:student_id, :subject, :type, :class, :details, :test_date, :total_marks, :marks_obtained)");
        query.bindValue(":student_id", student_id);
        query.bindValue(":subject", m_subject);
        query.bindValue(":type", m_type);
        query.bindValue(":details", m_details);
        query.bindValue(":test_date", m_testDate);
        query.bindValue(":total_marks", m_totalMarks);
        query.bindValue(":marks_obtained", obtainedMarks);

        if (!query.exec()) {
            qDebug() << "Insert failed:" << query.lastError().text();
        }
    }

    QMessageBox::information(this, "Success", "Marks uploaded successfully!");
    this->accept();
}
