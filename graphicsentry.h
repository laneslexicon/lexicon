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
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QLabel>
#include <QFile>
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
#include "xsltsupport.h"
class EntryItem : public QGraphicsTextItem {
 public:
  EntryItem(const QString &, QGraphicsItem * parent = 0);
  EntryItem(QGraphicsItem * parent = 0);
  void setNode(const QString & id) { m_nodeId = id;}
  void setRoot(const QString & root,bool isRootEntry = false);
  void setWord(const QString & word) { m_word = word; }
  bool isNode(const QString & id) { return m_nodeId ==  id;}
  bool isRoot() { return m_isRoot;}
  QString getNode() { return m_nodeId;}
  QString getRoot() { return m_root;}
  QString getWord() { return m_word;}
 protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
  void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
  private:
  QString m_nodeId;
  bool m_isRoot;
  int m_page;
  QString m_root;
  QString m_word;
};
class LaneGraphicsView : public QGraphicsView {
 public:
  LaneGraphicsView(QGraphicsScene *,QWidget * parent = 0);
 protected:
  void scrollContentsBy(int,int);
  void keyPressEvent(QKeyEvent *);
};
class GraphicsEntry : public QWidget {
  Q_OBJECT

 public:
  GraphicsEntry(QWidget * parent = 0);
  ~GraphicsEntry();
    bool prepareQueries();
    void getXmlForRoot(const QString &,const QString & anchor = QString());
    void getXmlForNode(const QString &);
  public slots:
    void on_findNode();
    void dbnameChanged();//const QString &);
    void cssChanged();
    void anchorClicked(const QUrl &);
    void linkActivated(const QString &);
    void linkHovered(const QString &);
    void anchorTest();
    void onZoomIn();
    void onZoomOut();
    void onClearScene();
 private:

    void addEntries(int);
    QList<EntryItem *> m_items;
    EntryItem * createEntry(const QString & xml);
    const XalanCompiledStylesheet * m_compXsl;
    bool showNode(const QString &);
    qreal m_scale;
    QTransform m_transform;
    QString transform(const QString & xsl,const QString & xml);
    void addEntry(const QString &);
    void setCSS(const QString &);
    QTextOption m_textOption;
    QString m_currentDb;
    QSqlDatabase m_db;
    QLineEdit * m_dbname;
    QLineEdit * m_xsl;
    QLineEdit * m_cssFile;
    QLineEdit * m_node;
    QPushButton * m_findNodeBtn;
    QPushButton * m_loadCssBtn;
    QPushButton * m_anchorBtn;
    QPushButton * m_clearSceneBtn;
    QPushButton * m_zoomIn;
    QPushButton * m_zoomOut;
    QLabel * m_root;
    QLabel * m_word;
    LaneGraphicsView * m_view;
    QGraphicsScene * m_scene;
    QGraphicsTextItem * m_item;
    QTextEdit * m_cssText;
    QTextEdit * m_nodeXml;
    QTextBrowser * m_nodeHtml;
    QTextEdit * m_nodeText;
    QSqlQuery * m_nodeQuery;
    QSqlQuery * m_rootQuery;
    QString m_standardCSS;
    QString m_currentCSS;
    QString m_currentHtml;
    XalanTransformer * m_xalan;
 protected:
    void keyPressEvent(QKeyEvent *);
    void focusInEvent(QFocusEvent *);
 signals:
    void focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason);
};
#endif
