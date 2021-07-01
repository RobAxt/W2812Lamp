#include "WS2812Node.hpp"

WS2812Node::WS2812Node(const char* id, const char* name, int ledPin, int ledCount) : HomieNode(id, name, "WS2812"), _ws2812fx(ledCount, ledPin, NEO_GRB + NEO_KHZ800), _strip(ledCount) {
  _brightness = new HomieSetting<long>("ws2812_brightness", "The Brightness [0 .. 100] Default = 100");
  _brightness->setDefaultValue(100).setValidator([](float candidate) {return (candidate >= 0) && (candidate <= 100);});

  _speed = new HomieSetting<long>("ws2812_speed", "The FX Speed [0 .. 5000] Default = 1000");
  _speed->setDefaultValue(1000).setValidator([](float candidate) {return (candidate >= 0) && (candidate <= 5000);});

  _mode = new HomieSetting<long>("ws2812_mode", "The FX Mode [0 .. 54] Default = 0");
  _mode->setDefaultValue(0).setValidator([](float candidate) {return (candidate >= 0) && (candidate <= 54);});

  _color = new HomieSetting<long>("ws2812_color", "The Color [0 .. 0xFFFFFF] Default = 0xFFFF00");
  _color->setDefaultValue(0xFFFF00).setValidator([](float candidate) {return (candidate >= 0) && (candidate <= 0xFFFFFF);});
}

WS2812Node::~WS2812Node(){}

void 
WS2812Node::setup() {
  Homie.getLogger() << F("Calling Node Setup... Node Name: ") << getName() << endl;
  setRunLoopDisconnected(true);
  advertise(RGB_TOPIC).setName("RGB Color")
                      .setDatatype("unsigned integer")
                      .setFormat("0x000000-0xFFFFFF")
                      .setUnit("RGB")
                      .settable([this](const HomieRange& range, const String& value) {
                                 unsigned long color = strtoul(value.c_str(),NULL,16);
                                 if(color == 0)
                                   _ws2812fx.strip_off();
                                 _ws2812fx.setColor(color);
                                 return true;
                                 });
  advertise(MODE_TOPIC).setName("RGB FX Mode")
                       .setDatatype("unsigned integer")
                       .setFormat("0-54")
                       .setUnit("#")
                       .settable([this](const HomieRange& range, const String& value) {
                                 _ws2812fx.setMode(value.toInt());
                                 return true;
                                 });  
  _ws2812fx.init();
  _strip.Begin();
  _strip.Show();
  _ws2812fx.setCustomShow(myCustomShow);
  _ws2812fx.setBrightness(_brightness->get());
  _ws2812fx.setSpeed(_speed->get());
  _ws2812fx.setMode(_mode->get());
  _ws2812fx.setColor(_color->get());
  _ws2812fx.start(); 
}

void
WS2812Node::onReadyToOperate() {
  Homie.getLogger() << F("Calling Ready To Operate... Node Name: ") << getName() << endl;
  if(Homie.isConnected()) {
    setProperty(RGB_TOPIC).send(String(_ws2812fx.getColor(), 3).c_str());
    setProperty(MODE_TOPIC).send(String(_ws2812fx.getMode(), 3).c_str());
  }
}

void 
WS2812Node::loop() {
  yield();
  _ws2812fx.service();
  yield();
}

bool 
WS2812Node::handleInput(const HomieRange& range, const String& property, const String& value) {
  Homie.getLogger() << F("Calling Node Handle Input...") << endl;  
  if(property != "rgb" && property != "mode") {
    Homie.getLogger() << F("  ✖ Error: property not handle: ") << property << endl; 
    return true;
  }
  if(property == "rgb" && (strtoul(value.c_str(),NULL,16) < 0 || strtoul(value.c_str(),NULL,16) > 0xFFFFFF)) {
    Homie.getLogger() << F("  ✖ Error: wrong value for mode property: ") << value << endl; 
    return true;
  }
  if(property == "mode" && (value.toInt() < 0 || value.toInt() > 54)) {
    Homie.getLogger() << F("  ✖ Error: wrong value for mode property: ") << value << endl; 
    return true;
  }
  Homie.getLogger() << F("  ✔ Receive Property/Value: ") << property  << F(" ━► ") << value << endl;
  return false;
}

void myCustomShow(void) {
  ws2812Node.customShow();
}

void
WS2812Node::customShow() {
  if(_strip.CanShow()) {
    // copy the WS2812FX pixel data to the NeoPixelBus instance
    memcpy(_strip.Pixels(), _ws2812fx.getPixels(), _strip.PixelsSize());
    _strip.Dirty();
    _strip.Show();
  }
}