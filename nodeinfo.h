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
class NodeInfo : public QDialog {
  Q_OBJECT

 public:
  NodeInfo(QWidget * parent = 0);
  ~NodeInfo();
  void setCss(const QString &);
  void setHtml(const QString &);
  void setHeader(const QString & root,const QString & head,const QString & node);
  QTextDocument * document() { return m_browser->document(); }
  QSize sizeHint() const;
  public slots:
    void accept();
    void reject();
    void openEntry();
 private:
    QTextCursor m_cursor;
    QSize m_size;
    void setPreferredSize(const QString &);
    QTextBrowser * m_browser;
    QLabel * m_rlabel;
    QLabel * m_hlabel;
    QString m_root;
    QString m_head;
    QString m_node;
    QString m_css;
 signals:
    void openNode(const QString & node);
};
#endif
