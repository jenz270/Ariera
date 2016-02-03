#include "treewidgetforpresentation.h"

TreeWidgetForPresentation::TreeWidgetForPresentation():
    createtreewidget(new QTreeWidget())
{

}

TreeWidgetForPresentation::~TreeWidgetForPresentation()
{

}
/**
 * Create a tree widget class to be displayed on the tab.
 * Tree widget class is populated with all the required data to fill the
 * the tree widget.
 * @brief TreeWidgetForPresentation::getCreateTreeWidget
 * @return return a QTreeWidget
 */
QTreeWidget *TreeWidgetForPresentation::getCreateTreeWidget()
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
QTreeWidgetItem* TreeWidgetForPresentation::AddRoot(QString name, QString total)
{
    createtreewidget->setHeaderHidden(false); //set header QTreeWidget hidden
    createtreewidget->setColumnCount(2); //specify three columns for the tree widget
    createtreewidget->setColumnWidth(0, 200); //the first column width set
    createtreewidget->setColumnWidth(1, 50);
    QStringList list; //used to name the header
    list << "Faculty Name" << "# of Presentations";
    QTreeWidgetItem *headeritms = new QTreeWidgetItem(list);
    headeritms->setTextAlignment(0, Qt::AlignRight);
    headeritms->setTextAlignment(1, Qt::AlignCenter);

    createtreewidget->setHeaderItem(headeritms);
    createtreewidget->setHeaderLabels(list);


    QTreeWidgetItem *itm = new QTreeWidgetItem(createtreewidget);
    itm->setText(0, name); //first column
    itm->setText(1, total);//second column
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
QTreeWidgetItem* TreeWidgetForPresentation::AddChild(QTreeWidgetItem *par, QString name, QString total)
{
    QTreeWidgetItem *itm = new QTreeWidgetItem();
    itm->setText(0, name);
    itm->setText(1, total);
    par->addChild(itm);
    return itm;
}

/**
 * Populate the tree widget with the requested filtered data.
 * @brief TreeWidgetForPresentation::fillTreeWidgetForPresentation
 * @param db    database used to filter the data
 * @param earliest earliest date
 * @param latest    latest date
 */
void TreeWidgetForPresentation::fillTreeWidgetForPresentation(Database &db, int earliest, int latest)
{
    struct presentationSummary sum; //struct used to arrange the summary for presentation
    db.presentationSummary(sum, earliest, latest);

    QTreeWidgetItem *midRoot1 = AddRoot("Invited Lectures", QString::number(sum.invitedLectures.size()));
    PrintChildNodes(midRoot1, sum.invitedLectures);

    QTreeWidgetItem *midRoot2 = AddRoot("Abstracts Presented", QString::number(sum.abstractPresented.size()));
    PrintChildNodes(midRoot2, sum.abstractPresented);

    QTreeWidgetItem *midRoot3 = AddRoot("Visiting Professorship", QString::number(sum.visitingProfessorship.size()));
    PrintChildNodes(midRoot3, sum.visitingProfessorship);

    QTreeWidgetItem *midRoot4 = AddRoot("Plenary Presentations", QString::number(sum.plenaryPresentation.size()));
    PrintChildNodes(midRoot4, sum.plenaryPresentation);

    QTreeWidgetItem *midRoot5 = AddRoot("Poster Presentations", QString::number(sum.posterPresentation.size()));
    PrintChildNodes(midRoot5, sum.posterPresentation);

    QTreeWidgetItem *midRoot6 = AddRoot("Student Presentations", QString::number(sum.studentPresentation.size()));
    PrintChildNodes(midRoot6, sum.studentPresentation);
}

/**
 * populate the child for each parent
 * @brief TreeWidgetForPresentation::PrintChildNodes
 * @param parent parent of the child node
 * @param results verctor data of the child node
 */
void TreeWidgetForPresentation::PrintChildNodes(QTreeWidgetItem *parent, vector<vector<string>> &results)
{
  vector< vector<string> >::iterator iter = results.begin();

  if (iter == results.end())
    return;

  string currentName = (*iter)[0];
  int count = 1;//total fund of the child node
  iter++;
  while (1){
    
    if (iter != results.end() && (*iter)[0].compare(currentName) == 0){
      count++;
      iter++;
      continue;
    }
    else {
      std::ostringstream convert;
      convert << count;
      QString nameStr = QString::fromStdString(currentName); //name of the child node
      QString countStr = QString::fromStdString(convert.str()); //total fund of the child node
      AddChild(parent, nameStr, countStr);

      if (iter == results.end())
        break;

      currentName = (*iter)[0];
      count = 1;
      iter++;
    }
  } 
}
