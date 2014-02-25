#include "main.h"

MyServer::MyServer(void* owner) : VSslServer(owner)
{
  QObject::connect(this, SIGNAL(runned(VSslSession*)), this, SLOT(run___(VSslSession*)), Qt::DirectConnection);
}

void MyServer::run___(VSslSession *sslSession)
{
  while (true)
  {
    QByteArray ba;
    int readLen = sslSession->read(ba);
    if (readLen == VERR_FAIL) break;
    LOG_DEBUG("kkk %s", ba.data());
    sslSession->write(ba);
  }
}

void clientTest()
{
  VSslClient client;
  client.host = "www.twitter.com";
  client.port = 443;
  if (!client.open())
  {
    LOG_ERROR("%s", client.error.msg);
    return;
  }
  client.write("GET / HTTP/1.1\r\nHost: www.twitter.com\r\n\r\n");
  QByteArray ba = client.read();
  LOG_DEBUG("%s", ba.data());
}

void serverTest()
{
  MyServer server(NULL);
  server.port = 443;
  server.fileName = "D:/temp/qt/ssl_test/bin/www.mysite.com_key_cert.pem";
  server.open();


  sleep(1000000);
}

int main()
{
  //clientTest();
  serverTest();

  return 0;
}
