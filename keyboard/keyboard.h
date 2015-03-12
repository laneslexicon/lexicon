#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__
#include <QApplication>
#include <QTabWidget>
#include <QTableWidget>
#include <QDir>
#include <QDebug>
#include <QDialog>
#include <QSplitter>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSettings>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsTextItem>
#include <QGraphicsItem>
#include <QLabel>
#include <QPainter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPageSetupDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPaintEvent>
#include <QFile>
#include <QTextStream>
#include <QFontMetrics>
#include <QDropEvent>
#include <QMimeData>
#include <QDragMoveEvent>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QPaintEvent>
#include <QTextDocument>
#include <QGraphicsSceneMouseEvent>
#include <QGridLayout>
#include <QPair>
#include "keydef.h"
#include "scripts.h"
class GraphicsButton : public QGraphicsRectItem {
 public:
  GraphicsButton(const QRect &,KeyDef * key,QGraphicsItem * parent = 0);
  void setup();
  void keyReleased();
  void setColor(int, const QColor &);
  void setColor(int,const QString &);
  void setCss(const QString &);
  KeyDef * getKeyDef() { return m_keydef; }
  //  double span() const { return m_keydef->getSpan();}
  //  QGraphicsTextItem * decorateKey(const QRectF & cell,const QString & text);
  QGraphicsTextItem * decorateKey(const QRectF & cell,int group,int level);
  //  void setHtml(int i,const QString &);
  enum colorscheme { TextColor , ButtonUpColor,ButtonDownColor, KeyboardColor };
 protected:
  void mousePressEvent(QGraphicsSceneMouseEvent * event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
private:
  int m_marginLeft;
  int m_marginRight;
  int m_marginTop;
  int m_marginBottom;
  QString m_css;
  QColor m_textColor;
  QColor m_backgroundUpColor;
  QColor m_backgroundDownColor;
  QColor m_keyboardColor;
  void toggleKeyPressed(int);
  void stickyKeyPressed(int);
  QMap<QSize,QString> m_decoration;
  QSize m_size;
  KeyDef * m_keydef;
};
class KeyboardScene : public QGraphicsScene {
 public:
  KeyboardScene(QObject * parent = 0);
  void keyPressed(GraphicsButton *);
  void setLevel(int);
  void setGroup(int);
  void unstickKeys();
};
class KeyboardView : public QGraphicsView   {
  Q_OBJECT
  public:
    KeyboardView(QWidget * parent = 0);
    ~KeyboardView();
    //    QRectF sceneRect() { return m_scene->sceneRect();}
    void loadKeyboard(const QString & fileName);
    void setLevel(int);
    void setGroup(int);
    void keyPressed(int);
    void keyPressed(QList<int>);
  private:
    void addButton(int row,int col,GraphicsButton *);
    QGraphicsScene * m_scene;
    int m_hspace;        // space between cols
    int m_vspace;        // space between rows
    int m_buttonWidth;
    int m_buttonHeight;
    bool m_debug;
    KeyboardDef * m_kbd;
 signals:
    void virtualKeyPressed(int);
    void virtualKeyPressed(QList<int>);
};
#endif
