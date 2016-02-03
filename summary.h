#ifndef SUMMARY_H
#define SUMMARY_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <vector>
#include "Database.h"
#include "Preprocessor.h"
#include "GrantsAndFundingFields.h"
#include "FieldMatchFilter.h"
#include <iostream>
#include <string>
#include <QDebug>
#include <QtCore>
#include <cstdlib>
#include <QMessageBox>
#include <algorithm>
#include "dashboard.h"
#include "qcustomplot.h"
#include "treewidgetforgrant.h"
#include "treewidgetforpublication.h"
#include "treewidgetforpresentation.h"
#include "treewidgetforteaching.h"
#include "graphhelper.h"
#include <QSettings>
#include <ctime>
#include <QPrinter>

class DashBoard;
class TreeWidgetForGrant;
class TreeWidgetForPublication;
class TreeWidgetForPresentation;
class TreeWidgetForTeaching;

namespace Ui {
class Summary;
}

class Summary : public QMainWindow
{
    Q_OBJECT

public:
    explicit Summary(QString qs,vector<string> listititem, DashBoard *dashb, QWidget *parent = 0);
    ~Summary();
    void testGraph(QCustomPlot *customPlot);
    void loadSettings();//setting persistant
    void saveSettings();//save setting
private slots:

    void on_pushButton_clicked(); //filter
    void on_pushButton_3_clicked(); //return back to main window
    void on_pushButton_4_clicked(); //close
    void on_pushButton_5_clicked(); //filter clear
    void on_pushButton_6_clicked(); // print graph

    void on_tabWidget_currentChanged(int index);//change in the current tab
    void on_nameBox_activated(int index); //select name from the combo box

    void on_graphChoice_activated(int index);//type of graph selected

    void on_graphSelector_activated(int index);//name of the graph

    void on_dateSelector_activated(int index);//choose the date

    void on_pushButton_2_clicked();//summary button
private:
    Ui::Summary *ui;
    std::vector<std::string> listVectorTransfered; //contain array of strings with the full path all the file loaded in the main window
    QString stringTransfered;//selected csv file in the list widget
    DashBoard *dashBoardData;//connect with the dashboard class
    Database db;
    TreeWidgetForGrant *GrantSummary;
    TreeWidgetForPublication *PublicationSummary;
    TreeWidgetForPresentation *PresentationSummary;
    TreeWidgetForTeaching *TeachingSummary;
    QCustomPlot* graph;
    QCPPlotTitle* graphTitle;

    void graphTitleInit();
    void processChoices(std::vector<std::vector<std::string> > &results);
    vector<Filter *> chosenName(vector<Filter *> filters, FieldMatchFilter* nameMatch);
    QVector<QString> cleanDates(QVector<QString> years);
    QVector<QString> sortNames(QVector<QString> names);
    void graphs();
    void processDate(std::vector<std::vector<std::string> > &results);
};

#endif // summary_H
