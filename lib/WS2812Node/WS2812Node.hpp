#ifndef WS2812Node_hpp
#define WS2812Node_hpp

#include <Homie.hpp>
#include <WS2812FX.h>
#include <NeoPixelBus.h>

class WS2812Node : public HomieNode {
    public:
        static const int LED_PIN = RX;
        static const int LED_COUNT = 20;
        explicit WS2812Node(const char* id, const char* name, int ledPin = LED_PIN, int ledCount = LED_COUNT);
        ~WS2812Node();
        void customShow();

    protected:
        virtual void setup() override;
        virtual void loop() override;
        virtual void onReadyToOperate() override;
        virtual bool handleInput(const HomieRange& range, const String& property, const String& value);

    private:
        const char* RGB_TOPIC = "rgb";
        const char* MODE_TOPIC = "mode";

        HomieSetting<long> *_brightness;
        HomieSetting<long> *_speed;
        HomieSetting<long> *_mode;
        HomieSetting<long> *_color; 

        WS2812FX _ws2812fx;
        NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> _strip;
};

void myCustomShow(void);

extern WS2812Node ws2812Node;
#endif //WS2812Node_hpp
//https://github.com/kitesurfer1404/WS2812FX.git#master
//https://github.com/kitesurfer1404/WS2812FX/issues/151