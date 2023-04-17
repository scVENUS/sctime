#include "pausedialog.h"

PauseDialog::PauseDialog(QDateTime starttime, QWidget *parent)
: QDialog(parent)
{
  setupUi(this);
  this->starttime=starttime;
  this->setWindowTitle(tr("sctime: Pause"));
  text=tr("Accounting has been stopped at %1.\nThe duration of the current break is %2.\nResume work with OK.");
  QDateTime now=QDateTime::currentDateTime();
  this->label->setText(text.arg(starttime.toString("H:mm")).arg(QTime(0,0).addSecs(starttime.secsTo(now)).toString("H:mm")));
}

void PauseDialog::updateTime() {
  QDateTime now=QDateTime::currentDateTime();
  this->label->setText(text.arg(starttime.toString("H:mm")).arg(QTime(0,0).addSecs(starttime.secsTo(now)).toString("H:mm")));
}

PauseDialog::~PauseDialog()
{
}
