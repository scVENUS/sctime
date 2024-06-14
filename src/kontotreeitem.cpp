/*
    Copyright (C) 2018 science+computing ag
       Authors: Florian Schmitt et al.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "kontotreeview.h"
#include "kontotreeitem.h"

#include <QPalette>

QColor getStdBgColor(QWidget* widget) {
  auto pal = widget->palette();
  return pal.color(QPalette::Base);
}

KontoTreeItem::KontoTreeItem (QTreeWidget * parent, SCTimeXMLSettings::DefCommentDisplayModeEnum displaymode, bool sortByCommentText): QTreeWidgetItem(parent)
{
  isBoldAccount=false;
  isMicroAccount=false;
  hasSelectableMicroAccounts=false;
  this->sortByCommentText=sortByCommentText;
  this->displaymode=displaymode;
  
  m_bgColor=getStdBgColor(parent);
  setGray();
}

KontoTreeItem::KontoTreeItem (QTreeWidgetItem * parent, SCTimeXMLSettings::DefCommentDisplayModeEnum displaymode, bool sortByCommentText): QTreeWidgetItem(parent)
{
  isBoldAccount=false;
  isMicroAccount=false;
  hasSelectableMicroAccounts=false;
  this->sortByCommentText=sortByCommentText;
  this->displaymode=displaymode;
  m_bgColor=getStdBgColor(parent->treeWidget());
  setGray();
}

KontoTreeItem::KontoTreeItem (QTreeWidget * parent, SCTimeXMLSettings::DefCommentDisplayModeEnum displaymode, bool sortByCommentText, QString accountname)
     :QTreeWidgetItem(parent,0)
{
  isBoldAccount=false;
  isMicroAccount=false;
  hasSelectableMicroAccounts=false;
  this->sortByCommentText=sortByCommentText;
  this->displaymode=displaymode;
  setGray();
  m_bgColor=getStdBgColor(parent);

  this->setText(COL_ACCOUNTS, accountname);
}


KontoTreeItem::KontoTreeItem (QTreeWidgetItem *parent, SCTimeXMLSettings::DefCommentDisplayModeEnum displaymode, bool sortByCommentText, QString accountname)
    :QTreeWidgetItem(parent)
{
  isBoldAccount=false;
  isMicroAccount=false;
  hasSelectableMicroAccounts=false;
  this->sortByCommentText=sortByCommentText;
  this->displaymode=displaymode;
  setGray();
  m_bgColor=getStdBgColor(parent->treeWidget());

  this->setText(COL_ACCOUNTS, accountname);
}

void KontoTreeItem::setBoldAccount(bool bold)
{
  isBoldAccount=bold;
  QFont f = font(COL_ACCOUNTS);

  if( bold )
  {
    f.setWeight(QFont::Bold);
  }
  else
  {
    f.setWeight(QFont::Normal);
  }

  setFont(COL_ACCOUNTS, f);
}

void KontoTreeItem::setMicroAccount(bool microaccount)
{
  isMicroAccount=microaccount;
  QFont f = font(COL_ACCOUNTS);

  bool bold=false;
 
  switch(displaymode) {
      case SCTimeXMLSettings::DM_BOLD: bold=microaccount; break;
      case SCTimeXMLSettings::DM_NOTUSEDBOLD: bold=(!microaccount&&hasSelectableMicroAccounts); break;
      case SCTimeXMLSettings::DM_NOTBOLD: bold=false; break;
  }

  if(bold)
  {
    f.setWeight(QFont::Bold);
  }
  else
  {
    f.setWeight(QFont::Normal);
  }
  setFont(COL_COMMENT, f);
}

void KontoTreeItem::setHasSelectableMicroAccounts(bool hasselectablema)
{
  hasSelectableMicroAccounts=hasselectablema;
}

void KontoTreeItem::setGray()
{
  std::vector<int> columns;
  columns.push_back(COL_TIME);
  columns.push_back(COL_ACCOUNTABLE);
  for(unsigned int i=0; i<columns.size(); i++)
  {
    if ((text(columns.at(i)).simplified()=="0:00")||(text(columns.at(i)).simplified().startsWith("+"))) {
      setForeground(columns.at(i), QBrush(Qt::gray));
      isGray=true;      
    }
    else
    {
      setForeground(columns.at(i), QBrush(Qt::black));
      isGray=false;
    }
  }
}

void KontoTreeItem::setBgColor(const QColor bgColor)
{
  if (bgColor!=m_bgColor) {
    m_bgColor = bgColor;
    for( int i=0; i<NUM_COLUMNS; i++ )
    {
      if (bgColor.isValid()) {
         setBackground(i, QBrush(bgColor));
      } else {
         setBackground(i, QBrush(getStdBgColor(this->treeWidget())));
      }
    }
  }
}


KontoTreeItem* KontoTreeItem::nextSibling( )
{
  KontoTreeItem *parent = (KontoTreeItem*)this->parent();
  KontoTreeItem *nextSibling;
  if(parent){
    nextSibling = (KontoTreeItem*) this->parent()->child(parent->indexOfChild(this)+1);
  }
  else {
    KontoTreeView *treeWidget = (KontoTreeView*)this->treeWidget();
    nextSibling = (KontoTreeItem*) treeWidget->topLevelItem(treeWidget->indexOfTopLevelItem(this)+1);
  }
  return nextSibling;
}

// implements special sort order for comments, if sorting account column: either by id (numerical) or by text (other column)
bool KontoTreeItem::operator<(const QTreeWidgetItem &other) const {
  QTreeWidget* tw=(KontoTreeView*)treeWidget();
  int col = tw->sortColumn();
  if ((col!=COL_ACCOUNTS)||(KontoTreeView::getItemDepth(this)!=4)||(KontoTreeView::getItemDepth(&other)!=4)) {
    return QTreeWidgetItem::operator<(other);
  }
  if (!sortByCommentText) {
    return (text(COL_ACCOUNTS).toInt()<other.text(COL_ACCOUNTS).toInt());
  } else {
    return (text(COL_COMMENT)<other.text(COL_COMMENT));
  }
}
