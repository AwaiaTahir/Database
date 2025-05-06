#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QString>
#include <QDate>

namespace Ui {
class Dialog;
}

struct StudentInfo {
    QString name, address, phone, email, gender, className, imagePath;
    QDate dob;
};

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

    QString getSelectedImagePath() const;
    StudentInfo getStudentInfo() const;

private slots:
    void on_selectPhotoButton_clicked();

private:
    Ui::Dialog *ui;
    QString selectedImagePath;

};

#endif // DIALOG_H
