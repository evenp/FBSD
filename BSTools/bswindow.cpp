#include "bswindow.h"
#include <QtGui>
#include <QFileDialog>
#include <QMenuBar>
#include <iostream>


BSWindow::BSWindow (int *val)
{
  Q_UNUSED (val);
  gradType = VMap::TYPE_SOBEL_5X5;
  detectionWidget = new BSDetectionWidget;
  setCentralWidget (detectionWidget);
  // setFocus();  
  // createActions ();
  // createMenus ();
  setWindowTitle (tr ("FBSD"));
  resize (400, 400);
}


BSWindow::BSWindow ()
{
  gradType = 0;
  detectionWidget = new BSDetectionWidget;
  setCentralWidget (detectionWidget);
  // setFocus ();
  // createActions ();
  // createMenus ();
  setWindowTitle (tr ("FBSD"));
  resize (400, 400);
}


void BSWindow::setFile (QString fileName)
{
  resize (detectionWidget->openImage (fileName, gradType));
  detectionWidget->setDefaults ();
}


void BSWindow::runOptions ()
{
}


void BSWindow::runTest ()
{
  detectionWidget->localTest ();
}


void BSWindow::closeEvent (QCloseEvent *event)
{
  event->accept ();
}


void BSWindow::open ()
{
  QSize windowSize;
  QString fileName = QFileDialog::getOpenFileName (this,
                                     tr ("Open File"), QDir::currentPath ());
  if (! fileName.isEmpty ())
  {
    windowSize = detectionWidget->openImage (fileName, gradType);
    updateActions ();
  }
}


void BSWindow::save ()
{
  QAction *action = qobject_cast < QAction *> (sender ());
  QByteArray fileFormat = action->data().toByteArray ();
  saveFile (fileFormat);
}


void BSWindow::updateActions ()
{
}


void BSWindow::createActions ()
{
  openAct = new QAction (tr ("&Open..."), this);
  openAct->setShortcut (tr ("Ctrl+O"));
  connect (openAct, SIGNAL (triggered ()), this, SLOT (open()));

  foreach (QByteArray format, QImageWriter::supportedImageFormats ())
  {
    QString text = tr("%1...").arg (QString(format).toUpper ());
    QAction *action = new QAction (text, this);
    action->setData (format);
    connect (action, SIGNAL (triggered ()), this, SLOT (save ()));
    saveAsActs.append (action);
  }
  exitAct = new QAction (tr ("E&xit"), this);
  exitAct->setShortcut (tr ("Ctrl+Q"));
  connect (exitAct, SIGNAL (triggered ()), this, SLOT (close ()));
}


void BSWindow::createMenus ()
{
  saveAsMenu = new QMenu (tr ("&Save As"), this);
  foreach (QAction *action, saveAsActs) saveAsMenu->addAction (action);
  fileMenu = new QMenu (tr ("&File"), this);
  fileMenu->addAction (openAct);
  fileMenu->addMenu (saveAsMenu);
  fileMenu->addSeparator ();
  fileMenu->addAction (exitAct);
  menuBar()->addMenu (fileMenu);
}


bool BSWindow::saveFile (const QByteArray &fileFormat)
{
  QString initialPath = QDir::currentPath () + "/untitled." + fileFormat;
  QString fileName = QFileDialog::getSaveFileName (this, tr ("Save As"),
                       initialPath, tr ("%1 Files (*.%2);;All Files (*)")
                           .arg(QString(fileFormat.toUpper()))
                           .arg(QString(fileFormat)));
  if (fileName.isEmpty ()) return false;
  else return detectionWidget->saveAugmentedImage (fileName, fileFormat);
}
