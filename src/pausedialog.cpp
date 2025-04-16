#include "pausedialog.h"

PauseDialog::PauseDialog(QDateTime starttime, int drift, int secSinceTick, QWidget *parent)
: QDialog(parent)
{
  setupUi(this);
  this->starttime=starttime;
  this->drift=drift;
  this->secSinceTick=secSinceTick;
  this->setWindowTitle(tr("sctime: Pause"));
  text=tr("Accounting has been stopped at %1.\nThe duration of the current break is %2.\nResume work with OK.");
  QDateTime now=QDateTime::currentDateTime();
  this->label->setText(text.arg(starttime.toString("H:mm")).arg(QTime(0,0).addSecs(starttime.secsTo(now)).toString("H:mm")));
  this->setModal(true);
  connect(this,&PauseDialog::finished, this, &PauseDialog::resumeAfterPause);
}

void PauseDialog::updateTime() {
  QDateTime now=QDateTime::currentDateTime();
  this->label->setText(text.arg(starttime.toString("H:mm")).arg(QTime(0,0).addSecs(starttime.secsTo(now)).toString("H:mm")));
}

void PauseDialog::resumeAfterPause() {
  pauseTimer.stop();
  emit pauseHasEnded(drift, secSinceTick);
}

void PauseDialog::startPause() {
  pauseTimer.setInterval(10000); // Update every 10 seconds
  connect(&pauseTimer, SIGNAL(timeout()),this,SLOT(updateTime()));
  pauseTimer.start();
  open();
  raise();
  adjustSize();
}


PauseDialog::~PauseDialog()
{
}
