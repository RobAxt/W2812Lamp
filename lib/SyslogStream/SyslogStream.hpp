#ifndef SyslogStream_hpp
#define SyslogStream_hpp

#include <Homie.h>
#include <WiFiUdp.h>
#include <Syslog.h>
#include <CircularBuffer.h>

class SyslogStreamClass : public Stream {  
  public:
    static const constexpr char*  SYSLOG_SRVR = NULL;
    static const int SYSLOG_PORT = 514;
    static const int BUFFER_SIZE = 128;
    static const int QUEUE_SIZE  = 100;

  private:
    WiFiUDP _udpClient;
    Syslog _syslog;
    const char* _server;
    uint16_t _port;
    uint16_t _level;
    char _buffer[BUFFER_SIZE];
    CircularBuffer<char*, QUEUE_SIZE> _offlineQueue;
    uint8_t _count;

    void storeMessage(void);
    void printMessage(void);
    
  public:
    SyslogStreamClass();
    SyslogStreamClass(const char* server, uint16_t port = SYSLOG_PORT);
    void setup(const char* deviceHostname = SYSLOG_NILVALUE, const char* appName = SYSLOG_NILVALUE, uint16_t priDefault = LOG_INFO);
    void setup(const char* server, const char* deviceHostname = SYSLOG_NILVALUE, const char* appName = SYSLOG_NILVALUE , uint16_t priDefault = LOG_KERN);
    void server(const char* server = SYSLOG_SRVR, int port = SYSLOG_PORT);
    void deviceHostname(const char* deviceHostname = SYSLOG_NILVALUE);
    void appName(const char* appName = SYSLOG_NILVALUE);
    void setSyslogLevel(uint16_t level = LOG_INFO);

    void loop(void);
    
    virtual int read(){return 0;};
    virtual int available(){return 0;};
    virtual int peek(){return 0;};
    using Print::write;
    virtual size_t write(uint8_t val);     
};

extern SyslogStreamClass SyslogStream;
#endif //SyslogStream_hpp