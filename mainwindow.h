#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QLabel>
#include <QStatusBar>
#include <QToolBar>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QFont>
#include <iostream>
#include <QMessageBox>
#include "qfinddialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent*);

public slots:
    //metodi menu file
    void saveFile();
    void saveAsFile();
    void openFile();
    void newFile();
    void exit();

    //metodi menu find
    void find();



private:
    Ui::MainWindow *ui;
    QPlainTextEdit *textarea;

    //Menu
    QMenu *fileMenu;
    QMenu *findMenu;


    //Action nel menu "File"
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;

    QString currentFileName;
    int font_size;

    //find dialog
    QFindDialog *findDialog;
    //Action del menu "Find"
    QAction *findAction;

    //metodi di inizializzazione
    void init_menubar();
    void init_statusbar();
    
    void setCurrentFile(QString filename);
    void fileModified();
    bool isSaved();

    //find dialog
    void enabledFindMenuBar();
    void setTextColorAfterModify();
    void removeColoredStringAfterFound(QString colordString);

};
#endif // MAINWINDOW_H
