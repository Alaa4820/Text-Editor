#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QShortcut>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QGuiApplication>
#include <QSaveFile>
#include <QFileDialog>
#include <QClipboard>
#include <QLineEdit>
#include <QPushButton>
/* Costruttore della classe MainWindow
 * */
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    font_size=10;
    findDialog=new QFindDialog();
    ui->setupUi(this);
    setGeometry(600,300,800,600);
    textarea = new QPlainTextEdit();
    setCentralWidget(textarea);
    init_menubar();
    init_statusbar();

    QFont font;
    font.setFamily("Courier");
    textarea->setFont(font);
    textarea->zoomIn(font_size);

    setCurrentFile(QString()); //file vuoto di nome "untitled.txt"

    //action menu file
    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAsFile);
    connect(exitAction, &QAction::triggered, this, &MainWindow::exit);

    //imposta che il file è stato modificato
    connect(textarea->document(), &QTextDocument::contentsChanged, this, &MainWindow::fileModified);

    //se la textarea è vuota la funzione "Find" non è cliccabile, se contiene qualcosa allora si attiva il bottone find
    connect(textarea->document(), &QTextDocument::contentsChanged, this, &MainWindow::enabledFindMenuBar);


    //action menu find
    connect(findAction, &QAction::triggered, this, &MainWindow::find);

    //toglie il colore rosse appena cambia la posizione del cursore
    connect(textarea, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::setTextColorAfterModify);

}

/* Funzione SLOT che serve a:
 * Rimuovere il colore rosso dalle parole evidenziate se la dialog è ancora attiva
 *
 * */
void MainWindow::setTextColorAfterModify(){
    if(findDialog->isVisible())
        removeColoredStringAfterFound(findDialog->getFindText());

}

/* Funzione che rende visibile o meno l'azione Find se il testo
 * è vuoto o meno o se la dialog è attiva e si tenta di aprirne
 * un'altra
 *
 **/
void MainWindow::enabledFindMenuBar(){
    if(!textarea->document()->toPlainText().isEmpty()){
        findAction->setEnabled(true);
    }else
        findAction->setEnabled(false);

    if(findDialog->isVisible())
        findAction->setEnabled(false);
}

/* Funzione che cerca e evidenzia le parole
 *
 * */
void MainWindow::find(){
    findDialog->show();
    findAction->setEnabled(false);//serve per evitare di chiamare più dialog che causano l'errore QDialog::exec: Recursive call detected
    findDialog->setErrorVisible(false);//rimuovo errore dopo aver riaperto la dialog (se questo era apparso precedentemente)
    findDialog->setCheckBox(false); //check box unchecked quando apro la dialog
    bool isColored=false; //variabile che indica che nella ricerca precedente è stato evidenziato qualcosa
    QString searchString="";

    while(findDialog->exec() == QDialog::Accepted){ //ACCEDPTED 1 - REJECTED 0
        searchString = findDialog->getFindText();
        QTextDocument *doc=textarea->document();
        bool found = searchString.isEmpty();

        if(isColored){//chiamo undo() solo se prima è stato evidenziato qualcosa, altrimenti se il file non è stato salvato e chiamo undo si cancella la modifica del file
            doc->undo();//torna indietro nel documento, spariscono le parole rosse della ricerca precedente
            isColored=false;
        }

        QTextCursor highlightCursor(doc);
        QTextCursor cursor(doc);
        cursor.beginEditBlock();
        QTextCharFormat plainFormat(highlightCursor.charFormat());
        QTextCharFormat colorFormat = plainFormat;
        colorFormat.setForeground(Qt::red);
        findDialog->setErrorVisible(false);//se l'errore è presente e nella ricerca successiva c'è un match, la label dell'errore deve sparire
        //textarea->setReadOnly(true); // evita che il highlightCursor modifichi il testo
        while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
            if(findDialog->isCheckBoxChecked())
                highlightCursor = doc->find(searchString, highlightCursor,QTextDocument::FindCaseSensitively);
            else
                highlightCursor = doc->find(searchString, highlightCursor,QTextDocument::FindWholeWords);

            if (!highlightCursor.isNull()) {
                found = true;
                isColored=true;
                highlightCursor.movePosition(QTextCursor::WordRight,QTextCursor::KeepAnchor);
                highlightCursor.mergeCharFormat(colorFormat);
            }
        }
        cursor.endEditBlock();
        if (!found){//se non trovo nulla -> mostro l'errore
            findDialog->setErrorVisible(true);
            findAction->setEnabled(false);
        }

        if(searchString.isEmpty()){
            findAction->setEnabled(false);
        }

        findDialog->setVisible(true);
        findAction->setEnabled(false);
        textarea->setReadOnly(false);
    }
    findAction->setEnabled(true);
    //textarea->setReadOnly(false);
    removeColoredStringAfterFound(searchString); //rimuove il colore rosse quando la find dialog è stata chiusa

}
/* Funzione che rimuove solo le parole evidenziate in rosso
 *
 * */
void MainWindow::removeColoredStringAfterFound(QString coloredString){
    QTextDocument *doc=textarea->document();
    QTextCursor highlightCursor(doc);
    QTextCursor cursor(doc);
    cursor.beginEditBlock();
    QTextCharFormat plainFormat(highlightCursor.charFormat());
    QTextCharFormat colorFormat = plainFormat;
    colorFormat.setForeground(Qt::black);
    textarea->setReadOnly(true);
    while(!highlightCursor.isNull() && !highlightCursor.atEnd()) {
        if(findDialog->isCheckBoxChecked())
            highlightCursor = doc->find(coloredString, highlightCursor,QTextDocument::FindCaseSensitively);
        else
            highlightCursor = doc->find(coloredString, highlightCursor,QTextDocument::FindWholeWords);

        if (!highlightCursor.isNull()) {
            highlightCursor.movePosition(QTextCursor::WordRight,QTextCursor::KeepAnchor);
            highlightCursor.mergeCharFormat(colorFormat);
        }
    }
    cursor.endEditBlock();
    textarea->setReadOnly(false);
}
/* OVERRIDE della funzione closeEvent(QCloseEvent*) della classe QWidget
 * Viene chiamata in automatico quando la MainWindow viene chiusa e
 * chiuse anche la find dialog se questa è aperta
 *
 * */
void MainWindow::closeEvent(QCloseEvent *event){//Buttton close in alto a destra
    if(findDialog->isVisible())
        findDialog->closeDialog();

    if(isSaved()){
        event->accept();
        findDialog->closeDialog();
    }else{
        event->ignore(); //ignoro l'evento di chiusura se si clicca su "Cancel" dopo che ho modificato il file
    }
}




/* Funzione di supporto
 * Se si crea un file vuoto, comparirà nella window in alto untitled.txt
 * altrimenti imposta il nome del file
 *
 * */
void MainWindow::setCurrentFile(QString filename){
    currentFileName = filename;
    textarea->document()->setModified(false);
    setWindowModified(false);
    QString name = currentFileName;

    if (currentFileName.isEmpty())
        name = "untitled.txt";

    setWindowFilePath(name);
}
/* Imposta che la textarea è stata modificata
 * */
void MainWindow::fileModified(){
    setWindowModified(textarea->document()->isModified());
}
/* Funzione che serve ad avvertire se il file è stato modificato e
 * si tenta di salvare con nome, aprire o chiudere senza che il file
 * sia stato salvato
 * */
bool MainWindow::isSaved(){
    if(!textarea->document()->isModified())
        return true;

    QMessageBox::StandardButton messageBox=QMessageBox::warning(this, "Warning", "The document was modified \nDo you want to save?",
                                                                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch(messageBox){
    case QMessageBox::Save: saveFile();
        break;
    case QMessageBox::Cancel: return false;
        break;
    case QMessageBox::Discard:
        break;
    default:
        break;
    }
    return true;
}

/* Funzione nuovo file
 * */
void MainWindow::newFile(){
    if(findDialog->isVisible())
        findDialog->closeDialog();

    if(isSaved()){
        textarea->clear();
        setCurrentFile(QString());
    }
}
/* Funzione apri file
 *
 * */
void MainWindow::openFile(){
    if(findDialog->isVisible())
        findDialog->closeDialog();

    if(isSaved()){
        currentFileName=QFileDialog::getOpenFileName(this);
        if(!currentFileName.isEmpty()){
            QFile file(currentFileName);

            if(!file.open(QFile::ReadOnly | QFile::Text))
                QMessageBox::warning(this,"Warning", "Error during file loading");

            QTextStream in(&file);
            QGuiApplication::setOverrideCursor(Qt::WaitCursor);
            textarea->document()->setPlainText(in.readAll());
            QGuiApplication::restoreOverrideCursor();
            setCurrentFile(currentFileName);
        }
    }
}
/* Funzione salva
 *
 * */
void MainWindow::saveFile(){
    if(findDialog->isVisible())
        findDialog->closeDialog();

    if(currentFileName.isEmpty()){
        saveAsFile();
    }else{
        QGuiApplication::setOverrideCursor(Qt::WaitCursor);
        QSaveFile file(currentFileName);

        if(file.open(QFile::WriteOnly | QFile::Text)){
            QTextStream out(&file);
            out<<textarea->document()->toPlainText();

            if(!file.commit()){
                QMessageBox::warning(this,"Warning", "Error during file saving");

            }

        }else{
            QMessageBox::warning(this,"Warning", "Error during file opening");
        }
        QGuiApplication::restoreOverrideCursor();
        setCurrentFile(currentFileName);
        statusBar()->showMessage("File saved",2000);//2000 ms durata nella statusbar
    }
}

/* Funzione salva con nome
 *
 * */
void MainWindow::saveAsFile(){
    if(findDialog->isVisible())
        findDialog->closeDialog();
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    if(dialog.exec() == QDialog::Accepted){
        currentFileName=dialog.selectedFiles().first();
        setCurrentFile(currentFileName);
        saveFile();
    }
}

/* Funzione della action nel menu "File" che oltre a
 * chiudere la MainWindow, chiude anche la dialog se
 * questa è attiva
 *
 * */
void MainWindow::exit(){
    if(isSaved()){
        findDialog->closeDialog();
        QApplication::quit();
    }
}

/* Crea il menu
 *
 * */
void MainWindow::init_menubar(){
    fileMenu=menuBar()->addMenu("File");
    findMenu=menuBar()->addMenu("Find");

    //Action del menu FILE
    newAction=new QAction(QIcon(),"New");
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip("Create a new file");
    fileMenu->addAction(newAction);

    openAction=new QAction(QIcon(),"Open");
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip("Open a existing file");
    fileMenu->addAction(openAction);

    saveAction=new QAction(QIcon(),"Save");
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip("Save file");
    fileMenu->addAction(saveAction);

    saveAsAction=new QAction(QIcon(),"Save As");
    saveAsAction->setShortcut(tr("Ctrl+Shift+S"));
    saveAsAction->setStatusTip("Save file as");
    fileMenu->addAction(saveAsAction);

    //Separetor menu File
    fileMenu->addSeparator();
    exitAction=new QAction(QIcon(), "Exit");
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip("Exit program");
    fileMenu->addAction(exitAction);

    //Action del menu FIND
    findAction=new QAction(QIcon(),"Find");
    findAction->setShortcut(QKeySequence::Find);
    findAction->setStatusTip("Find");
    findAction->setEnabled(false);
    findMenu->addAction(findAction);

}


/* Crea la status bar in fondo
 *
 * */
void MainWindow::init_statusbar(){
    statusBar()->addPermanentWidget(new QLabel(""));
    statusBar()->addPermanentWidget(new QLabel(tr("%1 %").arg(font_size)));
}

/* Distruttore
 *
 * */
MainWindow::~MainWindow(){
    delete textarea;
    delete fileMenu;
    delete findDialog;
    delete findMenu;
    delete findAction;
    delete newAction;
    delete openAction;
    delete saveAction;
    delete saveAsAction;
    delete exitAction;
    delete ui;
}


