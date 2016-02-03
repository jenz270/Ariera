#include "treewidgetforgrant.h"

TreeWidgetForGrant::TreeWidgetForGrant():
    createtreewidget(new QTreeWidget())
{

}

TreeWidgetForGrant::~TreeWidgetForGrant()
{

}

QTreeWidget *TreeWidgetForGrant::getCreateTreeWidget()
{
    return createtreewidget;
}
/**
 * Create a tree widget class to be displayed on the tab.
 * Tree widget class is populated with all the required data to fill the
 * the tree widget.
 * @brief TreeWidgetForGrant::AddRoot
 * @param name name of the parent node
 * @param totalResearch total research done per person
 * @param totalFund total fund for research per person
 * @return QTreeWidgetItem
 */
QTreeWidgetItem* TreeWidgetForGrant::AddRoot(QString name, QString totalResearch, QString totalFund)
{
    createtreewidget->setHeaderHidden(false); //set header QTreeWidget hidden
    createtreewidget->setColumnCount(3); //specify three columns for the tree widget
    createtreewidget->setColumnWidth(0, 200); //the first column width set
    createtreewidget->setColumnWidth(1, 70); //set the second column width
    createtreewidget->setColumnWidth(2, 150);
    QStringList list;
    list << "" << "Total #" << "Total $"; //used to name the headers
    QTreeWidgetItem *headeritms = new QTreeWidgetItem(list);
    headeritms->setTextAlignment(0, Qt::AlignRight); //align header name to the right
    headeritms->setTextAlignment(1, Qt::AlignRight);
    headeritms->setTextAlignment(2, Qt::AlignRight);
    createtreewidget->setHeaderItem(headeritms);
    createtreewidget->setHeaderLabels(list);

    QTreeWidgetItem *itm = new QTreeWidgetItem(createtreewidget);
    itm->setText(0, name); //first column values added
    itm->setText(1, totalResearch);
    itm->setText(2, totalFund);
    createtreewidget->addTopLevelItem(itm);
    return itm;

}

/**
 *Add child to the tree widget for a give QTreeWidgetItem
 * @brief TreeWidgetForGrant::AddChild
 * @param par
 * @param name
 * @param totalResearch
 * @param totalFund
 * @return
 */
QTreeWidgetItem* TreeWidgetForGrant::AddChild(QTreeWidgetItem *par, QString name, QString totalResearch, QString totalFund)
{
    QTreeWidgetItem *itm = new QTreeWidgetItem();
    itm->setText(0, name);
    itm->setText(1, totalResearch);
    itm->setText(2, totalFund);
    par->addChild(itm);
    return itm;
}
//double getTotal(std::vector< vector<string> > list);
/**
 * poplulate the list widget with the tree node and thier values
 * @brief TreeWidgetForGrant::fillTreeWidgetForGrant
 * @param db database for filter
 * @param earliest  earliest date
 * @param latest    latest date
 */
void TreeWidgetForGrant::fillTreeWidgetForGrant(Database &db, int earliest, int latest)
{
    struct grantsAndFundingSummary sum;
    db.grantsAndFundingSummary(sum, earliest, latest);

    const QLocale &clocal = QLocale::c(); //format string number
    double peerReviewedTotal = getTotal(sum.grantsPeerReviewed);
    double industrySponsoredTotal = getTotal(sum.grantsIndustrySponsored);
    double total = peerReviewedTotal + industrySponsoredTotal;
    //add root to the grant list widget
    QTreeWidgetItem *grants = AddRoot("Grants", QString::number(sum.grantsIndustrySponsored.size() + sum.grantsPeerReviewed.size()), clocal.toString(total, 'f', 2));
    //add the child to the grant list widgets
    QTreeWidgetItem *grantsPeerReviewed = AddChild(grants, "Peer Reviewed", QString::number(sum.grantsPeerReviewed.size()), clocal.toString(peerReviewedTotal, 'f', 2));
    PrintChildNodes(grantsPeerReviewed, sum.grantsPeerReviewed);

    QTreeWidgetItem *grantsIndustrySponsored = AddChild(grants, "Industry Sponsored", QString::number(sum.grantsIndustrySponsored.size()), clocal.toString(industrySponsoredTotal, 'f', 2));
    PrintChildNodes(grantsIndustrySponsored, sum.grantsIndustrySponsored);

    peerReviewedTotal = getTotal(sum.clinicalPeerReviewed);
    industrySponsoredTotal = getTotal(sum.clinicalIndustrySponsored);
    total = peerReviewedTotal + industrySponsoredTotal;

    QTreeWidgetItem *clinical = AddRoot("Clinical Funding", QString::number(sum.clinicalIndustrySponsored.size() + sum.clinicalPeerReviewed.size()), clocal.toString(total, 'f', 2));

    QTreeWidgetItem *clinicalPeerReviewed = AddChild(clinical, "Peer Reviewed", QString::number(sum.clinicalPeerReviewed.size()), clocal.toString(peerReviewedTotal, 'f', 2));
    PrintChildNodes(clinicalPeerReviewed, sum.clinicalPeerReviewed);

    QTreeWidgetItem *clinicalIndustrySponsored = AddChild(clinical, "Industry Sponsored", QString::number(sum.clinicalIndustrySponsored.size()), clocal.toString(industrySponsoredTotal, 'f', 2));
    PrintChildNodes(clinicalIndustrySponsored, sum.clinicalIndustrySponsored);
}

//double getFunding(string formattedFunding);
/**
 * Populate the child fund under the parent node, with the provided data.
 * @brief TreeWidgetForGrant::PrintChildNodes
 * @param parent
 * @param results
 */
void TreeWidgetForGrant::PrintChildNodes(QTreeWidgetItem *parent, vector<vector<string>> &results)
{
  vector< vector<string> >::iterator iter = results.begin();

  if (iter == results.end())
    return;

  string currentName = (*iter)[0];
  string formattedFunding = (*iter)[2];
  
  double funding = getFunding(formattedFunding);
  int count = 1;
  iter++;
  while (1){
    
    if (iter != results.end() && (*iter)[0].compare(currentName) == 0){
      formattedFunding = (*iter)[2];
      funding += getFunding(formattedFunding);
      count++;
      iter++;
      continue;
    }
    else {
      std::ostringstream convertInt, convertDouble;
      convertInt << count;
      QString nameStr = QString::fromStdString(currentName);
      QString countStr = QString::fromStdString(convertInt.str());
      const QLocale &clocal = QLocale::c(); //format string number

      AddChild(parent, nameStr, countStr, clocal.toString(funding, 'f', 2));

      if (iter == results.end())
        break;

      currentName = (*iter)[0];
      formattedFunding = (*iter)[2];
      funding = getFunding(formattedFunding);
      count = 1;
      iter++;
    }
  } 
}
/**
 * Get funding values from the grant string data.
 * @brief TreeWidgetForGrant::getFunding
 * @param formattedFunding formated string value
 * @return
 */
double TreeWidgetForGrant::getFunding(string formattedFunding){

  if (formattedFunding.compare("") == 0)
    return 0;
  string unformattedFunding = formattedFunding;
  // remove dollar signed
  unformattedFunding = formattedFunding.substr(1, formattedFunding.length()-1);
  if (unformattedFunding.compare("") == 0)
    return 0;

  unformattedFunding.erase(std::remove(unformattedFunding.begin(), unformattedFunding.end(), ','), unformattedFunding.end()); 

  return std::stof(unformattedFunding);
}
/**
 * Get total fund for grant list widget from a vector data.
 * @brief TreeWidgetForGrant::getTotal
 * @param list list of vector array with the total fund
 * @return
 */
double TreeWidgetForGrant::getTotal(std::vector< vector<string> > list)
{
    double sum = 0; //hold the total sum of the fund

    vector< vector<string> >::iterator iter = list.begin();

    for ( ; iter != list.end(); iter++)
    {
      sum += getFunding( (*iter)[2] );
    }
    return sum;
}



