// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_SSL_SESSION_H__
#define __V_SSL_SESSION_H__

#include <VTcpSession>
#include <VSslCommon>

// ----------------------------------------------------------------------------
// VSslSession
// ----------------------------------------------------------------------------
class VSslSession : public VNetSession, protected VStateLockable
{
public:
  VSslSession(void* owner = NULL);
  virtual ~VSslSession();

protected:
  virtual bool doOpen();
  virtual bool doClose();
  virtual int  doRead(char* buf, int size);
  virtual int  doWrite(char* buf, int size);

public:
  SSL* con;
  BIO* sbio;

public:
  SOCKET       handle;     // reference, used in open // gilgil temp 2014.02.25
  SSL_CTX*     ctx;        // reference, used in open
  VTcpSession* tcpSession; // reference

public:
  Ip  getLocalIP();
  Ip  getRemoteIP();
  int getLocalPort();
  int getRemotePort();

public:
  bool operator == (const VSslSession& rhs) const;
};

#endif // __V_SSL_SESSION_H__

