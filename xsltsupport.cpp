#include "xsltsupport.h"
XalanTransformer * getXalan() {
  static XalanTransformer * m_xalan;
  static bool m_init = false;
  if (! m_init){
    XMLPlatformUtils::Initialize();
    XalanTransformer::initialize();
    m_init = true;
    m_xalan = new XalanTransformer;

  }
  return m_xalan;
}
/**
   xsl - name of xslt file
 */
QString xalanTransform(const QString & xsl,const QString & xml,QMap<QString,QString> & params) {
  std::istringstream iss(xml.toStdString());
  //  std::cout << xml.toStdString();
  std::string ss;
  //  iss >> ss;
  //  std::cout  << ss;
  std::stringstream ostream;
  QByteArray ba = xsl.toLocal8Bit();
  XalanTransformer * xalan;
  xalan =  getXalan();
  QMapIterator<QString,QString> i(params);

  while(i.hasNext()) {
    i.next();
    QByteArray ka = i.key().toLocal8Bit();
    QByteArray va = i.value().toLocal8Bit();
    xalan->setStylesheetParam(ka.data(), va.data());
  }
  int theResult =
    //    x.transform("../tmp/test.xml","../xslt/tei.xsl",xmlOut)
    // x.transform("../tmp/test.xml","../xslt/tei.xsl",ostream);
    xalan->transform(iss,ba.data(),ostream);
  // x.transform(xml.toStdString(),"../xslt/tei.xsl",ostream);
  //  std::cout << ostream.str();
  QString t;
  t = QString::fromStdString(ostream.str());
  return t;
}
/*
const XalanDOMString key("param1");
const XalanDOMString expression("'Hello World'");
theXalanTransformer.setStylesheetParam(key, expression);
*/
