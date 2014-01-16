#include <VGraph>
#include <VThread> // for VThreadMgr

// ----------------------------------------------------------------------------
// VGraphObjectList
// ----------------------------------------------------------------------------
VGraphObjectList::VGraphObjectList(VGraph* graph)
{
  m_graph = graph;
  clear();
}

VGraphObjectList::~VGraphObjectList()
{
  clear();
}

void VGraphObjectList::clear()
{
  int _count = this->count();
  for (int i = 0; i < _count; i++)
  {
    VObject* object = this->at(i);
    delete object;
  }
  QList<VObject*>::clear();
}

bool VGraphObjectList::addObject(VObject* object)
{
  this->push_back(object);
  return true;
}

bool VGraphObjectList::delObject(VObject* object)
{
  int index = this->indexOf(object);
  if (index == -1) return false;
  delete object;
  this->removeAt(index);
  return true;
}

VObject* VGraphObjectList::findByName(QString name)
{
  int _count = this->count();
  for (int i = 0; i < _count; i++)
  {
    VObject* res = this->at(i);
    if (res->name == name) return res;
  }
  LOG_ERROR("can not find object for ('%s')", qPrintable(name));
  return NULL;
}

QList<VObject*> VGraphObjectList::findChildren(QString categoryName)
{
  VMetaClassMap& map  = VMetaClassMap::instance();
  VMetaClassList list = map[(char*)qPrintable(categoryName)];

  QList<VObject*> res;
  int _count = this->count();
  for (int i = 0; i < _count; i++)
  {
    VObject* obj = this->at(i);
    for (VMetaClassList::iterator it = list.begin(); it != list.end(); it++)
    {
      VMetaClass* metaClass = *it;
      if (QString(metaClass->className()) == obj->className())
        res.push_back(obj);
    }
  }
  return res;
}

void VGraphObjectList::load(VXml xml)
{
  clear();
  xml_foreach (childXML, xml.childs())
  {
    QString className = childXML.getStr("_class");
    if (className == "")
    {
      LOG_ERROR("_class is null");
      return;
    }
    VObject* object = (VObject*)VMetaClassMgr::createByClassName((char*)qPrintable(className));
    if (object == NULL)
    {
      LOG_ERROR("can not create instance for %s", qPrintable(className));
    }
    object->owner = this->m_graph;
    object->load(childXML);
    this->push_back(object);
  }
}

void VGraphObjectList::save(VXml xml)
{
  xml.clearChild();
  int _count = this->count();
  for (int i = 0; i < _count; i++)
  {
    VObject* object = this->at(i);
    VXml childXML = xml.addChild("object");
    object->save(childXML);
  }
}

// ----------------------------------------------------------------------------
// VGraphConnect
// ----------------------------------------------------------------------------
VGraphConnect::VGraphConnect()
{
  sender   = "";
  signal   = "";
  receiver = "";
  slot     = "";
}

VGraphConnect::~VGraphConnect()
{
}

bool VGraphConnect::operator == (const VGraphConnect& rhs)
{
  if (sender   != rhs.sender)   return false;
  if (signal   != rhs.signal)   return false;
  if (receiver != rhs.receiver) return false;
  if (slot     != rhs.slot)     return false;
  return true;
}

void VGraphConnect::load(VXml xml)
{
  sender   = xml.getStr("sender",   sender);
  signal   = xml.getStr("signal",   signal);
  receiver = xml.getStr("receiver", receiver);
  slot     = xml.getStr("slot",     slot);
}

void VGraphConnect::save(VXml xml)
{
  xml.setStr("sender",   sender);
  xml.setStr("signal",   signal);
  xml.setStr("receiver", receiver);
  xml.setStr("slot",     slot);
}

// ----------------------------------------------------------------------------
// VGraphConnectList
// ----------------------------------------------------------------------------
VGraphConnectList::VGraphConnectList(VGraph* graph): m_graph(graph)
{
  clear();
}

VGraphConnectList::~VGraphConnectList()
{
  clear();
}

void VGraphConnectList::clear()
{
  QList<VGraphConnect>::clear();
}

bool VGraphConnectList::addConnect(const VGraphConnect connect)
{
  int i = this->indexOf(connect); // gilgil temp 2012.07.27
  if (i != -1)
  {
    LOG_ERROR("connect(%s %s %s %d) already exists",
      qPrintable(connect.sender),
      qPrintable(connect.signal),
      qPrintable(connect.receiver),
      qPrintable(connect.slot));
    return false;
  }

  VObject*    sender   = m_graph->objectList.findByName(connect.sender);
  QMetaMethod signal   = VGraph::findMethod(sender, connect.signal);

  VObject*    receiver = m_graph->objectList.findByName(connect.receiver);
  QMetaMethod slot     = VGraph::findMethod(receiver, connect.slot);

  if (sender == NULL)   return false;
  if (receiver == NULL) return false;

  bool res = QObject::connect(sender, signal, receiver, slot, Qt::DirectConnection);
  if (!res)
  {
    LOG_ERROR("QObject::connect(%s %s %s %s) return false",
      qPrintable(sender->name),   qPrintable(connect.signal),
      qPrintable(receiver->name), qPrintable(connect.slot));
    return false;
  }

  this->push_back(connect);
  return true;
}

bool VGraphConnectList::delConnect(VGraphConnect connect)
{
  int i = this->indexOf(connect); // gilgil temp 2012.07.27
  if (i == -1)
  {
    LOG_ERROR("can not find connect(%s %s %s %d)",
      qPrintable(connect.sender),
      qPrintable(connect.signal),
      qPrintable(connect.receiver),
      qPrintable(connect.slot));
    return false;
  }

  VObject*    sender   = m_graph->objectList.findByName(connect.sender);
  QMetaMethod signal   = VGraph::findMethod(sender, connect.signal);

  VObject*    receiver = m_graph->objectList.findByName(connect.receiver);
  QMetaMethod slot     = VGraph::findMethod(receiver, connect.slot);

  if (sender == NULL)   return false;
  if (receiver == NULL) return false;

  bool res = QObject::disconnect(sender, signal, receiver, slot);
  if (!res)
  {
    LOG_ERROR("QObject::connect(%s %s %s %s) return false",
      qPrintable(sender->name),   qPrintable(connect.signal),
      qPrintable(receiver->name), qPrintable(connect.slot));
    return false;
  }

  this->removeAt(i);
  return res;
}

void VGraphConnectList::load(VXml xml)
{
  clear();
  xml_foreach (childXML, xml.childs())
  {
    VGraphConnect connect;
    connect.load(childXML);
    addConnect(connect);
  }
}

void VGraphConnectList::save(VXml xml)
{
  xml.clearChild();
  int _count = this->count();
  for (int i = 0; i < _count; i++)
  {
    VGraphConnect connect = at(i);
    connect.save(xml.addChild("connect"));
  }
}

// ----------------------------------------------------------------------------
// VGraph
// ----------------------------------------------------------------------------
VGraph::VGraph(void* owner) : objectList(this), connectList(this), VObject(owner)
{
  clear();
}

VGraph::~VGraph()
{
  close();
  clear();
}

void VGraph::clear()
{
  objectList.clear();
  connectList.clear();
}

bool VGraph::doOpen()
{
  VThreadMgr& mgr = VThreadMgr::instance();
  // mgr.clear(true); // gilgil temp 2012.08.17
  mgr.suspend();

  int res = true;
  int _count = objectList.count();
  for (int i = 0; i < _count; i++)
  {
    VObject* object = objectList.at(i);
    if (!object->open())
    {
      error = object->error;
      QString msg = QString(error.msg) + "(" + object->name + ")";
      error.setErrorMsg(qPrintable(msg));
      res = false;
      break;
    }
  }
  mgr.resume();
  return res;
}

bool VGraph::doClose()
{
  int _count = objectList.count();
  for (int i = 0; i < _count; i++)
  {
    VObject* object = objectList.at(i);
    object->close();
  }
  return true;
}

QStringList VGraph::methodList(VObject* object, QMetaMethod::MethodType methodType)
{
  QStringList res;

  int _count = object->metaObject()->methodCount();
  for (int i = 0; i < _count; i++)
  {
    QMetaMethod _method = object->metaObject()->method(i);
    if (_method.methodType() == methodType)
    {
      QString methodName = _method.methodSignature();
      res.push_back(methodName);
     }
  }
  return res;
}

QStringList VGraph::signalList(VObject *object)
{
  return methodList(object, QMetaMethod::Signal);
}

QStringList VGraph::slotList(VObject *object)
{
  return methodList(object, QMetaMethod::Slot);
}

QMetaMethod VGraph::findMethod(VObject* object, QString methodName)
{
  if (object == NULL)
  {
    LOG_ERROR("object is null for method('%s')", qPrintable(methodName));
    QMetaMethod blankMethod;
    return blankMethod;
  }
  int _count = object->metaObject()->methodCount();
  for (int i = 0; i < _count; i++)
  {
    QMetaMethod res = object->metaObject()->method(i);
    if (QString(res.methodSignature()) == methodName)
    {
      return res;
    }
  }
  LOG_ERROR("can not find method('%s')", qPrintable(methodName));
  QMetaMethod blankMethod;
  return blankMethod;
}

void VGraph::load(VXml xml)
{
  objectList.load(xml.gotoChild("objectList"));
  connectList.load(xml.gotoChild("connectList"));
}

void VGraph::save(VXml xml)
{
  objectList.save(xml.gotoChild("objectList"));
  connectList.save(xml.gotoChild("connectList"));
}
