#include "SyslogStream.hpp"

SyslogStreamClass::SyslogStreamClass() :_syslog(_udpClient,SYSLOG_PROTO_IETF), _server(NULL), _port(SYSLOG_PORT), _level(LOG_INFO) {}

SyslogStreamClass::SyslogStreamClass(const char* server, uint16_t port) :_syslog(_udpClient,SYSLOG_PROTO_IETF), _server(server), _port(port), _level(LOG_INFO) {}

void
SyslogStreamClass::setup(const char* deviceHostname, const char* appName, uint16_t priDefault) {
  _syslog.server(_server , _port);
  _syslog.deviceHostname(deviceHostname);
  _syslog.appName(appName);
  _syslog.defaultPriority(priDefault);
  _count=0;
}

void
SyslogStreamClass::setup(const char* server, const char* deviceHostname, const char* appName, uint16_t priDefault) {
  _syslog.server(server , _port);
  _syslog.deviceHostname(deviceHostname);
  _syslog.appName(appName);
  _syslog.defaultPriority(priDefault);
  _count=0;
}

void
SyslogStreamClass::server(const char* server, int port) {
  _syslog.server(server , port);
}

void
SyslogStreamClass::deviceHostname(const char* deviceHostname) {
  _syslog.deviceHostname(deviceHostname);
}

void
SyslogStreamClass::appName(const char* appName) {
  _syslog.appName(appName);
}

void
SyslogStreamClass::setSyslogLevel(uint16_t level) {
  _level = level;
}

size_t 
SyslogStreamClass::write(uint8_t val) {
  if(val != 0x0D && val != 0x0A)
    _buffer[_count++] = val;
  if ((_count == BUFFER_SIZE) || (val == 0x0A)) {
    if(Homie.isConnected()) 
      printMessage();  
     else 
      storeMessage();      
    _count = 0;
  }
  return Serial.write(val);
}

void
SyslogStreamClass::storeMessage(void){
  char *msg = new char[_count+1];
  _buffer[_count] = '\0';
  memcpy(msg,_buffer,_count);
  msg[_count] = '\0';
  _offlineQueue.unshift(msg);
  Serial.print("*** ");
  Serial.print(msg);
}

void
SyslogStreamClass::printMessage(void) {
  while(!_offlineQueue.isEmpty()) {
    char *msg = _offlineQueue.pop();
    _syslog.logf(_level,"%s", msg);
    delay(25);
    delete [] msg;
  }
  _buffer[_count] = '\0';
  _syslog.log(_level, _buffer); 
  delay(25);
}

SyslogStreamClass SyslogStream;