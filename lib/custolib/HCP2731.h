/*

HCP2731

By Nuno Albuquerque

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <Ticker.h>  //Ticker Library


// Internal Variables
volatile unsigned long up_edge = 0;             // to edge counter to change when button UP is pressed
volatile unsigned long dn_edge = 0;             // to edge counter to change when button DOWN is pressed
unsigned long last_up_edge = 0;                 // last up_edge counter value
unsigned long last_dn_edge = 0;                 // last dn_edge counter value
#define bottom_edge 9500                        // Bottom edge limit. Use 9500 us for 50HZ and 7833 for 60Hz.
#define upper_edge 10500                        // Upper edge limit. Use 10500 for 50HZ and 8833 for 60Hz.
                                                // Target value are 10000 us @ 50HZ and 8333 us @ 60HZ, with margin of +-500us.
#define Pulses_per_Second 100                   // # of changes per 1000 ms. For EU is 50Hz * 2 (Raise + fall). For US is 60Hz * 2 
#define checker_interval 50                     // Key checker time interval window [ms]
unsigned long last_up_change = 0;               // last time edge UP changed
unsigned long last_dn_change = 0;               // last time edge DN changed
unsigned long last_up_time = 0;                 // last time button UP pressed
unsigned long last_dn_time = 0;                 // last time button DN pressed

Ticker edge_ticker;

// **** Functions Declaration here ...
void IRAM_ATTR up_changing();
void IRAM_ATTR dn_changing();

// **** Functions code here ...
//Counter UP edge changing due to GPIO input interrupt
void up_changing() {
    detachInterrupt(BUT_UP);
    if (micros() - last_up_change < bottom_edge) {}
    else {
        if (micros() - last_up_change < upper_edge) up_edge ++;    // The edge occoured at expected timimg, so we ++
        last_up_change = micros();
    }
    attachInterrupt(BUT_UP, up_changing, CHANGE);
}

//Counter DN edge changing due to GPIO input interrupt
void dn_changing () {
    detachInterrupt(BUT_DN);
    if (micros() - last_dn_change < bottom_edge) {}
    else {
        if (micros() - last_dn_change < upper_edge) dn_edge ++;    // The edge occoured at expected timimg, so we ++
        last_dn_change = micros();
    }
    attachInterrupt(BUT_DN, dn_changing, CHANGE);
}


void edge_checker() {
    if ((up_edge - last_up_edge) >  ulong(Pulses_per_Second/(1000/checker_interval)*0.8)) {
        if(!UP_STATUS) {
            if((millis() - last_up_time) < interval) UP_COUNT ++;
            else UP_COUNT = 1;
            last_up_time = millis();
        };
        UP_STATUS = true;
    }
    else UP_STATUS = false;


    if ((dn_edge - last_dn_edge) >  ulong(Pulses_per_Second/(1000/checker_interval)*0.8)) {
        if(!DN_STATUS) {
            if((millis() - last_dn_time) < interval) DN_COUNT ++;
            else DN_COUNT = 1;
            last_dn_time = millis();
        };
        DN_STATUS = true;
    }
    else DN_STATUS = false;

    last_up_edge = up_edge;
    last_dn_edge = dn_edge;
}

void hcp2731_start() {
    // Enable GPIO interrupts on Input Raise and Fall edges (Change) 
    attachInterrupt(BUT_UP, up_changing, CHANGE);
    attachInterrupt(BUT_DN, dn_changing, CHANGE);

    // Enable input "edge checker" timed function
    last_up_edge = up_edge;
    last_dn_edge = dn_edge;
    edge_ticker.attach_ms(checker_interval, edge_checker);
}

void hcp2731_stop() {
    detachInterrupt(BUT_UP);
    detachInterrupt(BUT_DN);
    edge_ticker.detach();
}

 void hcp2731_hw() {
   // Input GPIOs
    pinMode(BUT_UP, INPUT_PULLUP);
    pinMode(BUT_DN, INPUT_PULLUP);

}

 void hcp2731_setup() {
    hcp2731_start();
}
