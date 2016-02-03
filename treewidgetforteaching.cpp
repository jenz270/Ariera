#include "treewidgetforteaching.h"

TreeWidgetForTeaching::TreeWidgetForTeaching():
    createtreewidget(new QTreeWidget())
{

}

TreeWidgetForTeaching::~TreeWidgetForTeaching()
{

}

QTreeWidget *TreeWidgetForTeaching::getCreateTreeWidget()
{
    return createtreewidget;
}

/**
 * Add root to the tree widget with the with thier values as specified
 * @brief TreeWidgetForPresentation::AddRoot
 * @param name  name of root
 * @param total total sum of the root
 * @return QTreeWidgetItem
 */
QTreeWidgetItem* TreeWidgetForTeaching::AddRoot(QString academic , QString hours)
{
    createtreewidget->setHeaderHidden(false); //set header QTreeWidget hidden
    createtreewidget->setColumnCount(2); //specify three columns for the tree widget
    createtreewidget->setColumnWidth(0, 200); //the first column width set
    createtreewidget->setColumnWidth(1, 200);
    //createtreewidget->setColumnWidth(2, 50);
    QStringList list;
    list << "Academic Year" << "Hours";
    createtreewidget->setHeaderLabels(list);

    QTreeWidgetItem *itm = new QTreeWidgetItem(createtreewidget);
    itm->setText(0, academic); //first column
    itm->setText(1, hours);
    //itm->setText(2, students);
    createtreewidget->addTopLevelItem(itm);
    return itm;
}

/**
 * Add child to the tree widget for a give QTreeWidgetItem
 * @brief TreeWidgetForPresentation::AddChild
 * @param par name of parent
 * @param name  name of child
 * @param total total value in the child
 * @return QTreeWidgetItem
 */
QTreeWidgetItem* TreeWidgetForTeaching::AddChild(QTreeWidgetItem *par, QString academic, QString hours)
{
    QTreeWidgetItem *itm = new QTreeWidgetItem();
    itm->setText(0, academic);
    itm->setText(1, hours);
    //itm->setText(2, students);
    par->addChild(itm);
    return itm;
}

/**
 * Populate the tree widget with ector<vector<string>> that_namethe requested filtered data.
 * @brief TreeWidgetForPresentation::fillTreeWidgetForPresentation
 * @param db    database used to filter the data
 * @param earliest earliest date
 * @param latest    latest date
 */
void TreeWidgetForTeaching::fillTreeWidgetForTeaching(QString typeeducation, double total, vector<vector<string>> data, vector<vector<double>> time, int earliest)
{  
    QTreeWidgetItem* root = AddRoot(typeeducation, QString::number(total));
    for(int i = 0; i < data.size(); i++)
    {
       QTreeWidgetItem* yearPointer = AddChild(root, QString::number(i+earliest)+"-"+QString::number(i+earliest+1), "");
       for(int j = 0; j < data[i].size(); j++)
       {
           AddChild(yearPointer, QString::fromStdString(data[i][j]), QString::number(time[i][j]));
       }
    }
}

double TreeWidgetForTeaching::calcTotal(vector<vector<string>> that_name, vector<vector<double>> that_time)
{
    double total = 0;
    for(int i = 0; i < that_name.size(); i++)
    {
       for(int j = 0; j < that_name[i].size(); j++)
       {
           total += that_time[i][j];
       }
    }
    return total;
}

/**
 * populate the child for each parent
 * @brief TreeWidgetForPresentation::PrintChildNodes
 * @param parent parent of the child node
 * @param results verctor data of the child node
 */
void TreeWidgetForTeaching::PrintChildNodes(QTreeWidgetItem *parent, vector<vector<string>> &results)
{

}
