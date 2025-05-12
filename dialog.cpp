#include "dialog.h"
#include "ui_dialog.h"
#include <QFileDialog>
#include <QPixmap>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->saveButton, &QPushButton::clicked, this, &Dialog::accept);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_selectPhotoButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Photo", "", "Images (*.png *.jpg *.jpeg)");
    if (!fileName.isEmpty()) {
        selectedImagePath = fileName;
    }
}

QString Dialog::getSelectedImagePath() const {
    return selectedImagePath;
}

StudentInfo Dialog::getStudentInfo() const {
    StudentInfo s;
    s.name = ui->nameLineEdit->text();
    s.address = ui->addressLineEdit->text();
    s.phone = ui->phoneLineEdit->text();
    s.email = ui->emailLineEdit->text();
    s.gender = ui->genderComboBox->currentText();
    s.className = ui->classComboBox->currentText();
    s.dob = ui->dobDateEdit->date();
    s.imagePath = selectedImagePath;
    return s;
}

void Dialog::setStudentInfo(const StudentInfo &info) {
    ui->nameLineEdit->setText(info.name);
    ui->phoneLineEdit->setText(info.phone);
    ui->emailLineEdit->setText(info.email);
    ui->classComboBox->setCurrentText(info.className);
    ui->dobDateEdit->setDate(info.dob);
    ui->genderComboBox->setCurrentText(info.gender);
    ui->addressLineEdit->setText(info.address);

    selectedImagePath = info.imagePath;
}

