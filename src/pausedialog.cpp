#include "pausedialog.h"

PauseDialog::PauseDialog(QDateTime starttime, QWidget *parent)
: QDialog(parent)
{
  setupUi(this);
  this->starttime=starttime;
  this->setWindowTitle(tr("sctime: Pause"));
  text=tr("Accounting has been stopped at %1.\nThe duration of the current break is %2 minutes.\nResume work with OK.");
  QDateTime now=QDateTime::currentDateTime();
  this->label->setText(text.arg(starttime.toString("H:m")).arg(starttime.secsTo(now)/60));
}

void PauseDialog::updateTime() {
  QDateTime now=QDateTime::currentDateTime();
  this->label->setText(text.arg(starttime.toString("H:m")).arg(starttime.secsTo(now)/60));
}

PauseDialog::~PauseDialog()
{
}
