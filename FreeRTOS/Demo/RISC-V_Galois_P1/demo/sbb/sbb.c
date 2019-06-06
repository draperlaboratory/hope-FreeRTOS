/**
 * Smart Ballot Box API
 * based on sbb.lando
 */

#include "sbb.h"
#include <stdio.h>
#include <string.h>

// FreeRTOS specific includes
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "gpio.h"
#include "serLcd.h"

TickType_t ballot_detect_timeout = 0;
TickType_t cast_or_spoil_timeout = 0;

bool has_a_barcode = false;
char barcode[BARCODE_MAX_LENGTH] = {0};
SemaphoreHandle_t barcode_mutex;


// Text defines
char * insert_ballot_text = "Insert ballot.";
char * barcode_detected_text = "Barcode detected.";
char * cast_or_spoil_text = "Cast or Spoil?";
char * casting_ballot_text = "Casting ballot.";
char * spoiling_ballot_text = "Spoiling ballot.";
char * not_a_valid_barcode_text = "Not a valid barcode!";
char * no_barcode_text = "No barcode detected!";
char * remove_ballot_text = "Remove ballot!";


/**
 * Initialize peripherals
 */
void init_sbb(void) {
    gpio_set_as_input(BUTTON_CAST_IN);
    gpio_set_as_input(BUTTON_SPOIL_IN);
    gpio_set_as_input(PAPER_SENSOR_IN);
    gpio_set_as_input(PAPER_SENSOR_OUT);
    gpio_set_as_output(MOTOR_0);
    gpio_set_as_output(MOTOR_1);
    gpio_set_as_output(BUTTON_CAST_LED);
    gpio_set_as_output(BUTTON_SPOIL_LED);

    barcode_mutex = xSemaphoreCreateMutex();
}

/**
 * @component Smart Ballot Box API
 * Perform Tally!
 */
void perform_tally(void)
{
    printf("Perorming tally\r\n");
}

/**
 * @component Smart Ballot Box API
 * Is barcode valid?
 * Check validity of the given barcode string
 */
bool is_barcode_valid(char * str, uint8_t len)
{
    (void) str;
    (void) len;
    return true;
}

/**
 * @component Smart Ballot Box
 * Is Cast button pressed?
 */
bool is_cast_button_pressed(void) {
    return gpio_read(BUTTON_CAST_IN);
}

/**
 * @component Smart Ballot Box
 * Is Spoil button pressed?
 */
bool is_spoil_button_pressed(void) {
    return gpio_read(BUTTON_SPOIL_IN);
}

/**
 * @component Smart Ballot Box
 * Just received barcode!
 */
void just_received_barcode(void) {
    if( xSemaphoreTake( barcode_mutex, portMAX_DELAY) == pdTRUE ) {
        has_a_barcode = true;
        xSemaphoreGive( barcode_mutex );
    }
}

/**
 * @component Smart Ballot Box
 * Set barcode to this value
 */
void set_received_barcode(char *str, uint8_t len) {
    configASSERT(len <= BARCODE_MAX_LENGTH);
    if( xSemaphoreTake( barcode_mutex, portMAX_DELAY) == pdTRUE ) {
        memcpy(barcode,str,len);
        xSemaphoreGive( barcode_mutex );
    }
}


/**
 * @component Smart Ballot Box
 * Has a barcode?
 */
bool has_a_bardcode(void) {
    bool val = false;
    if( xSemaphoreTake( barcode_mutex, portMAX_DELAY) == pdTRUE ) {
        val = has_a_barcode;
        xSemaphoreGive( barcode_mutex );
    }
    return val;
}

/**
 * @component Smart Ballot Box
 * What is the barcode?
 */
void what_is_the_barcode(char *str, uint8_t len) {
    configASSERT(len <= BARCODE_MAX_LENGTH);
    if( xSemaphoreTake( barcode_mutex, portMAX_DELAY) == pdTRUE ) {
        memcpy(str,barcode,len);
        xSemaphoreGive( barcode_mutex );
    }
}

/**
 * @component Smart Ballot Box
 * Spoil Button Light On!
 */
void spoil_button_light_on(void) {
    gpio_write(BUTTON_SPOIL_LED);
}

/**
 * @component Smart Ballot Box
 * Spoil Button Light Off!
 */
void spoil_button_light_off(void) {
    gpio_clear(BUTTON_SPOIL_LED);
}

/**
 * @component Smart Ballot Box
 * Cast Button Light On!
 */
void cast_button_light_on(void) {
    gpio_write(BUTTON_CAST_LED);
}

/**
 * @component Smart Ballot Box
 * Cast Button Light Off!
 */
void cast_button_light_off(void) {
    gpio_clear(BUTTON_CAST_LED);
}

/**
 * @component Smart Ballot Box
 * Move Motor Forward!
 */
void move_motor_forward(void) {
    gpio_write(MOTOR_0);
    gpio_clear(MOTOR_1);
}

/**
 * @component Smart Ballot Box
 * Move Motor back!
 */
void move_motor_back(void) {
    gpio_clear(MOTOR_0);
    gpio_write(MOTOR_1);
}

/**
 * @component Smart Ballot Box
 * Stop Motor!
 */
void stop_motor(void) {
    gpio_clear(MOTOR_0);
    gpio_clear(MOTOR_1);
}

/**
 * @component Smart Ballot Box
 * Display this text!
 */
void display_this_text(char *str, uint8_t len) {
    serLcdPrintf(str, len);
}

/**
 * @component Smart Ballot Box
 * Ballot detected?
 */
bool ballot_detected(void) {
    return gpio_read(PAPER_SENSOR_IN);
}

/**
 * @component Smart Ballot Box
 * Ballot inserted?
 */
bool ballot_inserted(void) {
    return gpio_read(PAPER_SENSOR_OUT);
}

/**
 * @component Smart Ballot Box
 * Spoil ballot!
 */
void spoil_ballot(void) {
    move_motor_back();
    while( !(ballot_detected() && !ballot_inserted()) ) {
        ;;
    }
    stop_motor();
}
  
/**
 * @component Smart Ballot Box
 * Cast ballot!
 */
void cast_ballot(void) {
    move_motor_forward();
    while( !(!ballot_detected() && ballot_inserted()) ) {
        ;;
    }
    stop_motor();
}

/**
 * @component Smart Ballot Box
 * Ballot spoiled?
 */
bool ballot_spoiled(void) {
    return (!ballot_detected() && !ballot_inserted());
}

/**
 * @component Smart Ballot Box
 * Go to standby!
 */
void go_to_standby(void) {
    stop_motor();
    cast_button_light_off();
    spoil_button_light_off();
    display_this_text(insert_ballot_text,strlen(insert_ballot_text));
}

/**
 * @component Smart Ballot Box
 * Ballot Detect Timeout Reset!
 */
void ballot_detect_timeout_reset(void) {
    ballot_detect_timeout = xTaskGetTickCount() + pdMS_TO_TICKS(BALLOT_DETECT_TIMEOUT_MS);
}

/**
 * @component Smart Ballot Box
 * Ballot Detect Timeout Expired?
 */
bool ballot_detect_timeout_expired(void) {
    return (xTaskGetTickCount() > ballot_detect_timeout);
}

/**
 * @component Smart Ballot Box
 * Cast Or Spoil Timeout Reset!
 */
void cast_or_spoil_timeout_reset(void) {
    cast_or_spoil_timeout = xTaskGetTickCount() + pdMS_TO_TICKS(CAST_OR_SPOIL_TIMEOUT_MS);
}

/**
 * @component Smart Ballot Box
 * Cast Or Spoil Timeout Expired?
 */
bool cast_or_spoil_timeout_expired(void) {
    return (xTaskGetTickCount() > cast_or_spoil_timeout);
}

void ballot_box_main_loop(void) {
    // Init happens before calling main()
    for (;;) {
        go_to_standby();
        if (ballot_detected()) {
            ballot_detect_timeout_reset();
            while (!ballot_inserted() && !ballot_detect_timeout_expired()) {
                move_motor_forward();
            }
            stop_motor();


            if (has_a_bardcode()) {
                display_this_text(barcode_detected_text,strlen(barcode_detected_text));
                what_is_the_barcode(barcode,BARCODE_MAX_LENGTH);
                
                if (is_barcode_valid(barcode,BARCODE_MAX_LENGTH)) {
                    display_this_text(cast_or_spoil_text,strlen(cast_or_spoil_text));
                    cast_button_light_on();
                    spoil_button_light_on();
                    cast_or_spoil_timeout_reset();

                    while (!cast_or_spoil_timeout_expired() 
                        && !is_cast_button_pressed()
                        && !is_spoil_button_pressed()) {
                            ;;
                        }
                    if (is_cast_button_pressed()) {
                        spoil_button_light_off();
                        display_this_text(casting_ballot_text,strlen(casting_ballot_text));
                        cast_ballot();
                        cast_button_light_off();
                        continue; // directly to Standby 
                    } else {
                        cast_button_light_off();
                        spoil_button_light_off();
                        // ->  spoil ballot
                    }
                } else  {
                    display_this_text(not_a_valid_barcode_text,strlen(not_a_valid_barcode_text));
                    // ->  spoil ballot
                }
            } else {// if has_a_barcode
                display_this_text(no_barcode_text,strlen(no_barcode_text));
                // -> spoil ballot
            }
            display_this_text(spoiling_ballot_text,strlen(spoiling_ballot_text));
            spoil_ballot();
            display_this_text(remove_ballot_text,strlen(remove_ballot_text));
            while (!ballot_spoiled()) {
                ;;
            }
        } // if ballot_detected
    } // loop iteration
}