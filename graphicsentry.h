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
#include <QProgressDialog>
#include <QGraphicsSceneContextMenuEvent>
#include <QToolButton>
#include <QGraphicsWidget>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QSettings>
#include <QRegExp>
#include <QClipboard>
#include <QPrinter>
#include "entryitem.h"
class LaneGraphicsView;
class Place;
class GraphicsEntry : public QWidget {
  Q_OBJECT

 public:
  GraphicsEntry(QWidget * parent = 0);
  ~GraphicsEntry();
  enum SearchType { RootSearch, WordSearch, NodeSearch };
  //  Place getXmlForRoot(const QString &,int supplement = 0,const QString & anchor = QString(),bool nodeOnly = false);
    Place getXmlForRoot(const Place &);
    Place showPlace(const Place &,bool thisPageOnly,int options);
    Place getPage(const Place & );
    //    Place getXmlForPlace(const Place &);

    Place getPlace() const;
    //    int hasRoot(const QString & root,bool focus = true);
    int hasPlace(const Place & ,int searchtype,bool focus = true);
    QString currentRoot() { return m_currentRoot;}
    /// return the first/last root in the scene
    void setPagingForward() {
      m_pagingDir = 0;
    }
    void setPagingBackward() {
      m_pagingDir = 1;
    }
    qreal getScale() { return m_scale;}
    void setScale(qreal v, bool use = true);
    void highlight(const QString & t);
    int getTextWidth() const { return m_textWidth; }
    void setTextWidth(int w) { m_textWidth = w;}
    QGraphicsScene * getScene() { return m_scene;}
    int search();
    int m_currentSearchOptions;
    QMap<int,QList<int> > m_searchPositions;
    void addPosition(int,int);
    void searchNext();
    bool hasNode(const QString &);
    bool focusNode(const QString &);
    void home();
    void shiftFocus();
    void setCurrentItem(QGraphicsItem *);
  public slots:
    void anchorClicked(const QUrl &);
    void linkActivated(const QString &);
    void linkHovered(const QString &);
    void anchorTest();
    void onZoomIn();
    void onZoomOut();
    void onClearScene();
    void showPerseus(const Place &);
    void showHtml();
    void updateCurrentPlace(const Place &);
    void selectAll();
    void clearAll();
    void selectEntry();
    void copy();
    void onWiden();
    void onNarrow();
    void clearHighlights();
    void notesButtonPressed();
    void addButtonDecoration();
    void deleteNotes();
    void focusPlace();
    void clearSelections();
    void showSelections();
    void print(QPrinter &);
 private:

    QString m_focusNode;

    QColor m_supplementBg;
    bool m_notesEnabled;
    bool m_showNodeOnly;
    bool prepareQueries();
    bool m_clearScene;
    bool m_debug;
    QString lastRoot();
    QString firstRoot();
    void readSettings();
    void writeDefaultSettings();
    bool readCssFromFile(const QString &name);
    int m_pagingDir;
    int m_entryMargin;
    bool m_dumpXml;
    bool m_dumpHtml;
    bool m_dumpOutputHtml;
    void reposition();
    void appendEntries(int);
    void prependEntries(int);

    QString m_moveFocusUpKey;
    QString m_moveFocusDownKey;
    void moveFocusUp();
    void moveFocusDown();

    QString m_zoomInKey;
    QString m_zoomOutKey;

    QString m_moveForwardKey;
    QString m_moveBackwardKey;

    QString m_widenKey;
    QString m_narrowKey;
    QString m_searchKey;
    QString m_searchNextKey;
    QString m_clearKey;
    QString m_showKey;
    QString m_homeKey;
    int m_widenStep;
    int m_defaultWidth;

    int m_currentSearchIndex;
    int m_currentSearchPosition;
    QRegExp m_currentSearchRx;
    QString m_currentSearchTarget;
    void moveForward();
    void moveBackward();
    QList<EntryItem *> m_items;
    EntryItem * createEntry(const QString & xml);
    //    const XalanCompiledStylesheet * m_compiledXsl;
    //    bool showNode(const QString &,bool thisPageOnly = false);


    qreal m_scale;

    QTransform m_transform;
    QString transform(const QString & xsl,const QString & xml);

    QTextOption m_textOption;
#
  //    LaneGraphicsView * m_view;
 QGraphicsView * m_view;
    QGraphicsScene * m_scene;
    QGraphicsTextItem * m_item;

    QSqlQuery * m_nodeQuery;
    QSqlQuery * m_rootQuery;

    QSqlQuery * m_pageQuery;
    QString m_standardCSS;

    QString m_currentHtml;
    QString m_currentRoot;
    Place m_place;

    // read/set from readSettings

    QString m_currentCSS;
    QString m_xsltSource;
    int m_textWidth;
    //    XalanTransformer * m_xalan;

 protected:
    void keyPressEvent(QKeyEvent *);
    void focusInEvent(QFocusEvent *);
    void closeEvent(QCloseEvent *);
 signals:
    void focusItemChanged(QGraphicsItem *, QGraphicsItem *, Qt::FocusReason);
    void rootChanged(const QString & root,const QString & node);
    void nextRoot(const QString &);
    void prevRoot(const QString &);
    void next(const Place &);
    void prev(const Place &);
    void cssChanged();
    void placeChanged(const Place &);
    void historyPositionChanged(int);
    void historyAddition(const Place &);
    void bookmarkAdd(const QString & id,const Place &);
    void clearPage();
    void searchPage();
    void findCount(int);
    void gotoNode(const Place & p,int);
};

class LaneGraphicsView : public QGraphicsView {
  Q_OBJECT
 public:
  LaneGraphicsView(QGraphicsScene *,GraphicsEntry * parent = 0);
 protected:
  virtual void scrollContentsBy(int,int);
  void keyPressEvent(QKeyEvent *);
  void focusInEvent(QFocusEvent *);
  void focusOutEvent(QFocusEvent *);
 signals:
  void nextPage();
  void backPage();
};
#endif
