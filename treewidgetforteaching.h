#ifndef TREEWIDGETFORTEACHING_H
#define TREEWIDGETFORTEACHING_H

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
#include <vector>
#include <string>
#include <QDebug>
#include <QtCore>
#include <cstdlib>
#include <QMessageBox>
#include <algorithm>
#include "dashboard.h"
#include "qcustomplot.h"
#include "treewidgetforgrant.h"

class TreeWidgetForTeaching
{
public:
    TreeWidgetForTeaching();
    ~TreeWidgetForTeaching();
    QTreeWidgetItem* AddRoot(QString academic , QString hours);
    QTreeWidgetItem* AddChild(QTreeWidgetItem *par, QString academic, QString hours);
    void fillTreeWidgetForTeaching(QString typeeducation, double total, vector<vector<string>> data, vector<vector<double>> time, int earliest);
    void PrintChildNodes(QTreeWidgetItem *parent, vector<vector<string>> &result);
    double calcTotal(vector<vector<string>> that_name, vector<vector<double>> that_time);

    QTreeWidget *getCreateTreeWidget();

private:
    QTreeWidget *createtreewidget;
};

#endif // TREEWIDGETFORTEACHING_H
