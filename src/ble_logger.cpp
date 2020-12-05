#include "ble_logger.h"

#define LED D7

static const size_t MAX_TO_SEND = 20;

static const char* serviceUuid = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
static const char* rxUuid = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
static const char* txUuid = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";


BleLoggerBase::BleLoggerBase(uint8_t *buf, size_t bufSize, LogLevel level, LogCategoryFilters filters) :
    StreamLogHandler(*this, level, filters),
    buf(buf), bufSize(bufSize),
    doOnce(true),
    txCharacteristic("tx", BleCharacteristicProperty::NOTIFY, txUuid, serviceUuid),
    rxCharacteristic("rx", BleCharacteristicProperty::WRITE_WO_RSP, rxUuid, serviceUuid, onDataReceivedStatic, this) {

    // Add this handler into the system log manager
    LogManager::instance()->addHandler(this);
}

BleLoggerBase::~BleLoggerBase() {
}

void BleLoggerBase::setup() {
    BLE.addCharacteristic(txCharacteristic);
    BLE.addCharacteristic(rxCharacteristic);

    BleAdvertisingData data;
    data.appendServiceUUID(serviceUuid);
    BLE.advertise(&data);

    pinMode(LED, OUTPUT);
}

void BleLoggerBase::loop() {
    if (BLE.connected()) {
        // Make sure you don't Log.info, etc. anywhere in this block, otherwise you'll recursively log
        size_t numToSend = writeIndex - readIndex;
        if (numToSend > 0) {
            if (numToSend > MAX_TO_SEND) {
                numToSend = MAX_TO_SEND;
            }
            txCharacteristic.setValue(&buf[readIndex % bufSize], numToSend);
            readIndex += numToSend;
        }

#if 0
        if (doOnce) {
            const String data = String("Jim's Argon BLE connected");
            // Trigger the integration
            Particle.publish("AlexaNotifyMe", data, PRIVATE);

            // Only trigger the Alexa notification once
            doOnce = false;
        }
    }
    else if (!BLE.connected() && !doOnce)
    {
        const String data = String("Jim's Argon BLE disconnected");
        // Trigger the integration
        Particle.publish("AlexaNotifyMe", data, PRIVATE);

        // Can trigger the Alexa connected notification once again
        doOnce = true;
    }
#else
    }
#endif

    digitalWrite(LED, (BLE.connected()) ? HIGH : LOW);
}


size_t BleLoggerBase::write(uint8_t c) {
    // Make sure you don't Log.info, etc. anywhere in this function, otherwise you'll recursively log

    if ((writeIndex - readIndex) >= bufSize) {
        // Buffer is full, discard oldest byte
        readIndex++;
    }

    buf[writeIndex++ % bufSize] = c;

    return 1;
}


void BleLoggerBase::onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer) {
    // Discard all data sent from the UART app
}

// [static]
void BleLoggerBase::onDataReceivedStatic(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
    BleLoggerBase *This = (BleLoggerBase *) context;

    This->onDataReceived(data, len, peer);
}