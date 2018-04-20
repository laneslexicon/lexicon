#ifndef _XSLTSUPPORT_H
#define _XSLTSUPPORT_H
#include <QString>
#include <QByteArray>
#include <QMap>
#include <QMapIterator>
#include <QDebug>
#include <iostream>
#include <sstream>
#ifdef USE_XALAN
#include <xalanc/Include/PlatformDefinitions.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
#include <xalanc/XalanDOM/XalanDOMString.hpp>
#include <xalanc/XalanTransformer/XalanCompiledStylesheetDefault.hpp>
#include <xalanc/XalanTransformer/XalanCompiledStylesheet.hpp>
XALAN_USING_XERCES(XMLPlatformUtils)
XALAN_USING_XALAN(XalanTransformer)
using XALAN_CPP_NAMESPACE::XalanCompiledStylesheet;
#endif
#ifdef USE_LIBXSLT
#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
//#include <libxml/DOCBparser.h>
#include <libxml/xinclude.h>
#include <libxml/catalog.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#endif
#ifdef USE_XQUERY
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QAbstractMessageHandler>
class MessageHandler : public QAbstractMessageHandler
{
public:
  MessageHandler()
    : QAbstractMessageHandler(0)
  {
  }

  QString statusMessage() const
  {
    return m_description;
  }

  int line() const
  {
    return m_sourceLocation.line();
  }

  int column() const
  {
    return m_sourceLocation.column();
  }
protected:
  virtual void handleMessage(QtMsgType type, const QString &description,
    const QUrl &identifier, const QSourceLocation &sourceLocation)
  {
    Q_UNUSED(type);
    Q_UNUSED(identifier);

    m_description = description;
    m_sourceLocation = sourceLocation;
  }

private:
  QString m_description;
  QSourceLocation m_sourceLocation;
};
#endif
enum xsltcompile_t {ENTRY_XSLT,NODE_XSLT,ENTRY_XSLT_RECOMPILE,TEST_XSLT };
void initXslt();
int compileStylesheet(int type,const QString & xsl);
QString xsltTransform(int type,const QString & xml);
const char * getCompileError();
void freeXslt();
void parseErrorHandler(void *,const char *, ...);
QStringList getParseErrors();
void clearParseErrors();
#endif
