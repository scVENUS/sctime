#include "oncalldialog.h"
#include "bereitschaftsview.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

OnCallDialog::OnCallDialog(QString abt, QString ko, QString uko, QStringList onCallCategories, QWidget* parent):QDialog(parent) {
  setObjectName(tr("sctime: On-call times"));
  setWindowTitle(tr("On-call times"));

  QVBoxLayout* layout=new QVBoxLayout(this);
  layout->setContentsMargins(15,15,15,15);

  QPushButton * okbutton=new QPushButton(tr("OK"), this);
  okbutton->setDefault(true);
  QPushButton * cancelbutton=new QPushButton(tr("Cancel"), this);

  layout->addStretch(1);

  QLabel* infolabel=new QLabel (tr("Please choose the rendered on-call times for this subaccount."), this);
  infolabel->setWordWrap(true);
  layout->addWidget(infolabel);

  layout->addSpacing(10);

  onCallCategoriesOrig=onCallCategories;
  this->abt=abt;
  this->ko=ko;
  this->uko=uko;

  onCallView=new BereitschaftsView(this);
  onCallView->setSelectionList(onCallCategories);
  layout->addWidget(onCallView);

  QHBoxLayout* buttonlayout=new QHBoxLayout();
  //buttonlayout->setParent(layout);
  buttonlayout->setContentsMargins(3,3,3,3);
  buttonlayout->addStretch(1);
  buttonlayout->addWidget(okbutton);
  buttonlayout->addWidget(cancelbutton);
  layout->addLayout(buttonlayout);

  connect (okbutton, SIGNAL(clicked()), this, SLOT(accept()));
  connect (cancelbutton, SIGNAL(clicked()), this, SLOT(reject()));
  connect (this, &OnCallDialog::finished, this, &OnCallDialog::postprocess);

  setModal(true);
}

void OnCallDialog::postprocess() {
    QStringList onCallCategoriesNew = onCallView->getSelectionList();
    emit finishedWithInfo(abt,ko,uko,onCallCategoriesOrig, onCallCategoriesNew);
}