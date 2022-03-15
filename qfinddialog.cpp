#include "qfinddialog.h"
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <iostream>
#include <QCloseEvent>
/*
 * Costruttore
 * */
QFindDialog::QFindDialog(QWidget *parent):QDialog(parent){
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::WindowStaysOnTopHint);
    setWindowTitle(QString("Find"));
    setMinimumSize(200,100);
    findText = "";
    findLabel = new QLabel("Find what:");
    lineEdit = new QLineEdit;
    findButton = new QPushButton("Find");
    closeButton=new QPushButton("Close");
    matchCaseCheckBox=new QCheckBox("Match case");
    error=new QLabel("Nessuna occorenza trovata");
    error->setVisible(false); //errore non visibile all'apertura della dialog
    error->setStyleSheet("color: red");

    //LAYOUT FINESTRA
    buttonlayout = new QVBoxLayout;
    horizontalLayout = new QHBoxLayout;
    verticalLayout=new QVBoxLayout;
    layout = new QHBoxLayout;
    horizontalLayout->addWidget(findLabel);
    horizontalLayout->addWidget(lineEdit);
    verticalLayout->addLayout(horizontalLayout);
    verticalLayout->addWidget(matchCaseCheckBox);
    verticalLayout->addWidget(error);
    buttonlayout->addWidget(findButton);
    buttonlayout->addWidget(closeButton);
    layout->addLayout(verticalLayout);
    layout->addLayout(buttonlayout);
    setLayout(layout);


    //ACTION
    connect(findButton, SIGNAL(clicked()), this, SLOT(findClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(closeDialog()));
    connect(findButton, SIGNAL(clicked()), this, SLOT(accept())); //accept mette setVisible(false);

}
/* Distruttore della classe
 * */
QFindDialog::~QFindDialog(){
    delete findLabel;
    delete lineEdit;
    delete findButton;
    delete closeButton;
    delete matchCaseCheckBox;
    delete error;
    delete buttonlayout;
    delete horizontalLayout;
    delete verticalLayout;
    delete layout;
}
/* OVERRIDE della funzione QWidget::close()
 * oltre a chiudere la dialog, cancella il contenuto
 * della lineEdit
 *
 * */
void QFindDialog::closeEvent(QCloseEvent *event){
    if(event->spontaneous()){
        closeDialog();
    }
}
/* Funzione associata al bottone "Close"
 *
 * */
void QFindDialog::closeDialog(){
    lineEdit->clear();
    QWidget::close();
}

/* Funzione se rende visibile l'errore "Nessuna occorrenza trovata" in
 * base al parametro booleano
 *
 * */
void QFindDialog::setErrorVisible(bool value){
    error->setVisible(value);
}
/* Funzione che ritona il campo findText
 * */
const QString QFindDialog::getFindText() const{
    return findText;
}
/* Funzione SLOT() che viene chimata al click del bottone "find"
 * Se il campo è vuoto compare una message box in cui si impone di scrivere il testo
 * nella line edit.
 * Altrimenti il testo viene salvato, e la line edit pulita con il metodo clear()
 * */
void QFindDialog::findClicked(){
    QString text = lineEdit->text();
    if (text.isEmpty()) {
        QMessageBox::information(this, "Empty Field","Enter a text");
        return;
    } else {
        findText = text;
        lineEdit->clear();
        return;
    }
}
/* Funzione che ritorna true se matchCaseCheckBox è checked
 *
 * */
bool QFindDialog::isCheckBoxChecked() const{
    return matchCaseCheckBox->isChecked();
}

/* Funzione che imposta il campo matchCaseCheckBox a unchecked o meno in base
 * al parametro booleano
 * */
void QFindDialog::setCheckBox(bool status){
    matchCaseCheckBox->setChecked(status);
}
