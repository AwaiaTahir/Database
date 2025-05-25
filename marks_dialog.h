#ifndef MARKS_DIALOG_H
#define MARKS_DIALOG_H

#include <QDialog>
#include <QDate>

namespace Ui {
class Marks_Dialog;
}

class Marks_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Marks_Dialog(QString subject, QString type, QString className,
                          QString details, QDate testDate, int totalMarks,
                          QWidget *parent = nullptr);
    ~Marks_Dialog();

private:
    Ui::Marks_Dialog *ui;
    QString m_subject;
    QString m_type;
    QString m_className;
    QString m_details;
    QDate m_testDate;
    int m_totalMarks;

    void loadMarksForm();
    void on_saveButton_clicked();
};

#endif // MARKS_DIALOG_H
