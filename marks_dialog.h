#ifndef MARKS_DIALOG_H
#define MARKS_DIALOG_H

#include <QDialog>

namespace Ui {
class Marks_Dialog;
}

class Marks_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Marks_Dialog(QWidget *parent = nullptr);
    ~Marks_Dialog();

private:
    Ui::Marks_Dialog *ui;
    void loadMarksForm();
};

#endif // MARKS_DIALOG_H
