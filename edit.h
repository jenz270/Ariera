#ifndef EDIT_H
#define EDIT_H

#include <QDialog>
#include <QSettings>
#include <QDesktopWidget>
#include <QtCore>
#include <QtGui>
#include "dashboard.h"

class DashBoard;

namespace Ui {
class edit;
}

class edit : public QDialog
{
    Q_OBJECT

public:
    explicit edit(DashBoard *dboard, QString fileselected, QWidget *parent = 0);
    ~edit();
    void loadSettings();
    void saveSettings();
    void fillQtableWiget();
    void saveToCSVFile(QString filename, QString headerstring);

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::edit *ui;
    QString filename;
    DashBoard *dashb;
    QString headerReadLine;
};

#endif // EDIT_H
