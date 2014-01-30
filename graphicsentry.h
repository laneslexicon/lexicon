#ifndef __GRAPHICSENTRY_H__
#define __GRAPHICSENTRY_H__
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextStream>
#include <QColor>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QLabel>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QSplitter>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include <QApplication>
#include <QTextOption>
#include <QScrollBar>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QSettings>
#include <QRegExp>
#include <QClipboard>
#include "QsLog.h"
#include "xsltsupport.h"
#include "history.h"
#include "place.h"
//#include "laneslexicon.h"

class EntryItem : public QGraphicsTextItem {
  Q_OBJECT

 public:
  EntryItem(const QString &, QGraphicsItem * parent = 0);
  EntryItem(QGraphicsItem * parent = 0);
  void setNode(const QString & id) { m_nodeId = id;}
  void setRoot(const QString & root,bool isRootEntry = false);
  void setWord(const QString & word) { m_word = word; }
  void setSupplement(int v)  { m_supplement = v;}
  void setBackground(QColor & c) { m_backgroundColor = c;}
  bool isNode(const QString & id) { return m_nodeId ==  id;}
  bool isRoot() { return m_isRoot;}
  QString getNode() { return m_nodeId;}
  QString getRoot() { return m_root;}
  QString getWord() { return m_word;}
  int getSupplement() { return m_supplement;}
  int getPage() { return m_page;}
  QTextCursor highlight(const QString &);
  QTextCursor highlightRx(const QString &);
  public slots:
    void searchItem();
    void copy();
 protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
  void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
  void paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
  QColor m_backgroundColor;
  private:
  QString m_nodeId;
  bool m_isRoot;
  int m_page;
  int m_supplement;
  QString m_root;
  QString m_word;
};
class LaneGraphicsView;
class GraphicsEntry : public QWidget {
  Q_OBJECT

 public:
  GraphicsEntry(QWidget * parent = 0);
  ~GraphicsEntry();

  //  Place getXmlForRoot(const QString &,int supplement = 0,const QString & anchor = QString(),bool nodeOnly = false);
  Place getXmlForRoot(const Place &);
    Place getXmlForPlace(const Place &);
    Place getXmlForNode(const QString &,bool nodeOnly = false);
    Place getPlace() { return m_place;}
    int hasRoot(const QString & root,bool focus = true);
    int hasPlace(const Place & ,bool focus = true);
    QString currentRoot() { return m_currentRoot;}
    /// return the first/last root in the scene
    void setPagingForward() {
      m_pagingDir = 0;
    }
    void setPagingBackward() {
      m_pagingDir = 1;
    }
    void highlight(const QString & t);
  public slots:
    void on_findNode();
    void anchorClicked(const QUrl &);
    void linkActivated(const QString &);
    void linkHovered(const QString &);
    void anchorTest();
    void onZoomIn();
    void onZoomOut();
    void onClearScene();
    void nextPageRequested();
    void prevPageRequested();
 private:
    QColor m_supplementBg;
    bool prepareQueries();
    bool m_clearScene;
    QString lastRoot();
    QString firstRoot();
    void readSettings();
    void writeDefaultSettings();
    bool readCssFromFile(const QString &name);
    int m_pagingDir;
    int m_entryMargin;
    bool m_dumpXML;
    bool m_dumpHTML;
    void appendEntries(int);
    void prependEntries(int);
    QList<EntryItem *> m_items;
    EntryItem * createEntry(const QString & xml);
    const XalanCompiledStylesheet * m_compiledXsl;
    //    bool showNode(const QString &,bool thisPageOnly = false);
    bool showPlace(const Place &,bool thisPageOnly = false);
    qreal m_scale;

    QTransform m_transform;
    QString transform(const QString & xsl,const QString & xml);

    QTextOption m_textOption;

    QPushButton * m_clearSceneBtn;
    QPushButton * m_zoomIn;
    QPushButton * m_zoomOut;


    LaneGraphicsView * m_view;
    QGraphicsScene * m_scene;
    QGraphicsTextItem * m_item;

    QSqlQuery * m_nodeQuery;
    QSqlQuery * m_rootQuery;
    QSqlQuery * m_nextRootQuery;
    QSqlQuery * m_pageQuery;
    QString m_standardCSS;

    QString m_currentHtml;
    QString m_currentRoot;
    Place m_place;
    // read/set from readSettings

    QString m_currentCSS;
    QString m_xsltSource;
    int m_textWidth;
    XalanTransformer * m_xalan;
 protected:
    void keyPressEvent(QKeyEvent *);
    void focusInEvent(QFocusEvent *);
 signals:
    void focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason);
    void rootChanged(const QString & root,const QString & node);
    void nextRoot(const QString &);
    void prevRoot(const QString &);
    void cssChanged();
    void placeChanged(const Place &);
};

class LaneGraphicsView : public QGraphicsView {
  Q_OBJECT
 public:
  LaneGraphicsView(QGraphicsScene *,GraphicsEntry * parent = 0);
 protected:
  virtual void scrollContentsBy(int,int);
  void keyPressEvent(QKeyEvent *);
 signals:
  void nextPage();
  void backPage();
};
#endif
