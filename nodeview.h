#ifndef __NODEVIEW_H__
#define __NODEVIEW_H__
#include <QDialog>
#include <QString>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDebug>
#include <QLabel>
#include <QFont>
class NodeView : public QDialog {
  Q_OBJECT

 public:
  NodeView(QWidget * parent = 0);
  ~NodeView();
  void setCSS(const QString &);
  void setHeader(const QString & root,const QString & head);
  void setPattern(const QRegExp & rx) { m_pattern = rx;}
  QTextDocument * document() { return m_browser->document(); }
  QSize sizeHint() const;
  public slots:
    void accept();
    void reject();
 private:
  QTextBrowser * m_browser;
  QLabel * m_rlabel;
  QLabel * m_hlabel;
  QString m_root;
  QString m_head;
  QList<int> m_positions;
  QRegExp m_pattern;
  QString m_css;
};
#endif
