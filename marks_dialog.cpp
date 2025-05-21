#include "marks_dialog.h"
#include "ui_marks_dialog.h"
#include <QCheckBox>
#include <QSqlQuery>
#include <QLineEdit>

Marks_Dialog::Marks_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Marks_Dialog)
{
    ui->setupUi(this);
    ui->marksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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

        QLineEdit *line_edit = new QLineEdit;
        table->setCellWidget(row, 2, line_edit);

        row++;
    }

    table->resizeColumnsToContents();
}
