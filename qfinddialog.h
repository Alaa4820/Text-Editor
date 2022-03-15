#ifndef QFINDDIALOG_H
#define QFINDDIALOG_H
#include <QDialog>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>

class QFindDialog : public QDialog{
    Q_OBJECT

public:
    QFindDialog(QWidget *parent = nullptr);
    ~QFindDialog();
    const QString getFindText() const;
    void setErrorVisible(bool);
    bool isCheckBoxChecked() const;
    void setCheckBox(bool);

protected:
    void closeEvent(QCloseEvent*);

public slots:
    void findClicked();
    void closeDialog();

private:
    QString findText;
    QLabel *findLabel;
    QPushButton *findButton;
    QPushButton *closeButton;
    QLineEdit *lineEdit;
    QCheckBox *matchCaseCheckBox;
    QLabel *error;

    //LAYOUT
    QVBoxLayout *buttonlayout;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *layout;
};
#endif // QFINDDIALOG_H
