#ifndef __NODEINFO_H__
#define __NODEINFo_H__
#include <QDialog>
#include <QString>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDebug>
#include <QLabel>
#include <QFont>
#include <QPushButton>
#include <QRegExp>
#include "place.h"
class NodeInfo : public QDialog {
  Q_OBJECT

 public:
  NodeInfo(QWidget * parent = 0);
  ~NodeInfo();
  void setCss(const QString &);
  void setHtml(const QString &);
  void setPlace(const Place &);
  QTextDocument * document() { return m_browser->document(); }
  QSize sizeHint() const;
  public slots:
    void accept();
    void reject();
    void openEntry();
 private:
    Place m_place;
    QTextCursor m_cursor;
    QSize m_size;
    void setPreferredSize(const QString &);
    QTextBrowser * m_browser;
    QLabel * m_rlabel;
    QLabel * m_hlabel;
    QLabel * m_vlabel;
    QString m_root;
    QString m_head;
    QString m_node;
    QString m_css;
 signals:
    void openNode(const QString & node);
};
#endif
