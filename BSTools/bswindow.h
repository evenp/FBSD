#ifndef BS_WINDOW_H
#define BS_WINDOW_H

#include <QList>
#include <QMainWindow>
#include "bsdetectionwidget.h"


/** 
 * @class BSWindow bswindow.h
 * \brief Blurred segment extraction Qt window.
 * \author {P. Even and B. Kerautret}
 */
class BSWindow : public QMainWindow
{
    Q_OBJECT
    
public:

  /**
   * \brief Creates a blurred segment extraction window.
   */
  BSWindow ();

  /**
   * \brief Creates a segment extraction window.
   */
  BSWindow (int *val);

  /**
   * Sets the processed image.
   */
  void setFile (QString fileName);

  /**
   * Sets the gradient extraction method to be used.
   */
  inline void useGradient (int type) { gradType = type; }

  /**
   * Takes into account the option (after image load).
   */
  void runOptions ();

  /**
   * Performs a simple test.
   */
  void runTest ();


protected:
  void closeEvent (QCloseEvent *event);


private slots:
  void open ();
  void save ();
  void updateActions ();


private:

  /** Blurred segment detection widget. */
  BSDetectionWidget *detectionWidget;
  int gradType;

  void createActions ();
  void createMenus ();
  bool saveFile (const QByteArray &fileFormat);
  QMenu *saveAsMenu;
  QMenu *fileMenu;

  QAction *openAct;
  QList<QAction *> saveAsActs;
  QAction *exitAct;

};
#endif
