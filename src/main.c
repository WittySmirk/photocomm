#ifdef TRANSMITTER
    #include "transmitter.h"
#elif defined(RECEIVER)
    #include "reciever.h"
#endif

int main() {
    // Transmitter code
    #ifdef TRANSMITTER
        return transmitter();
    #elif defined(RECEIVER)
        return reciever();
    #endif
}