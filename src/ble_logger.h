#ifndef __BLELOGGER_H
#define __BLELOGGER_H

#include "Particle.h"

class BleLoggerBase : public StreamLogHandler, Print {
public:
    BleLoggerBase(uint8_t *buf, size_t bufSize, LogLevel level = LOG_LEVEL_INFO, LogCategoryFilters filters = {});
    virtual ~BleLoggerBase();

    void setup();

    void loop();

    /**
     * @brief Virtual override for the StreamLogHandler to write data to the log
     */
    virtual size_t write(uint8_t);

protected:
    void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer);
    static void onDataReceivedStatic(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);

    uint8_t *buf;
    size_t bufSize;
    size_t readIndex = 0;
    size_t writeIndex = 0;

    bool doOnce;

    BleCharacteristic txCharacteristic;
    BleCharacteristic rxCharacteristic;
};

template <size_t BUFFER_SIZE>
class BleLogging : public BleLoggerBase {
public:
    explicit BleLogging(LogLevel level = LOG_LEVEL_INFO, LogCategoryFilters filters = {}) : BleLoggerBase(staticBuf, BUFFER_SIZE, level, filters) {};

protected:
    uint8_t staticBuf[BUFFER_SIZE];
};


#endif // __BLELOGGER_H