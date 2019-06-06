/**
 * based on sbb.lando
 */
#ifndef __SBB_H__
#define __SBB_H__


// General includes
#include <stdbool.h>
#include <stdint.h>

// Aux
#define BARCODE_MAX_LENGTH 16

// Button defines
#define BUTTON_CAST_LED 1
#define BUTTON_SPOIL_LED 3
#define BUTTON_CAST_IN 0
#define BUTTON_SPOIL_IN 2

// Paper sensor inputs
#define PAPER_SENSOR_OUT 6
#define PAPER_SENSOR_IN 7

// Motor defines
#define MOTOR_0 4
#define MOTOR_1 5

// Timeouts
#define BALLOT_DETECT_TIMEOUT_MS 6000
#define CAST_OR_SPOIL_TIMEOUT_MS 30000

/**
 * Initialize peripherals
 */
void init_sbb(void);

/**
 * @component Smart Ballot Box API
 * Perform Tally!
 */
void perform_tally(void);

/**
 * @component Smart Ballot Box API
 * Is barcode valid?
 * Check validity of the given barcode string
 */
bool is_barcode_valid(char * str, uint8_t len);

/**
 * @component Smart Ballot Box
 * Is Cast button pressed?
 */
bool is_cast_button_pressed(void);

/**
 * @component Smart Ballot Box
 * Is Spoil button pressed?
 */
bool is_spoil_button_pressed(void);

/**
 * @component Smart Ballot Box
 * Has a barcode?
 */
bool has_a_bardcode(void);

/**
 * @component Smart Ballot Box
 * Just received barcode!
 */
void just_received_barcode(void);

/**
 * @component Smart Ballot Box
 * Set barcode to this value
 */
void set_received_barcode(char *str, uint8_t len);

/**
 * @component Smart Ballot Box
 * What is the barcode?
 */
void what_is_the_barcode(char *str, uint8_t len);

/**
 * @component Smart Ballot Box
 * Spoil Button Light On!
 */
void spoil_button_light_on(void);

/**
 * @component Smart Ballot Box
 * Spoil Button Light Off!
 */
void spoil_button_light_off(void);

/**
 * @component Smart Ballot Box
 * Cast Button Light On!
 */
void cast_button_light_on(void);

/**
 * @component Smart Ballot Box
 * Cast Button Light Off!
 */
void cast_button_light_off(void);

/**
 * @component Smart Ballot Box
 * Move Motor Forward!
 */
void move_motor_forward(void);

/**
 * @component Smart Ballot Box
 * Move Motor back!
 */
void move_motor_back(void);

/**
 * @component Smart Ballot Box
 * Stop Motor!
 */
void stop_motor(void);

/**
 * @component Smart Ballot Box
 * Display this text!
 */
void display_this_text(char *str, uint8_t len);

/**
 * @component Smart Ballot Box
 * Ballot detected?
 */
bool ballot_detected(void);

/**
 * @component Smart Ballot Box
 * Ballot inserted?
 */
bool ballot_inserted(void);

/**
 * @component Smart Ballot Box
 * Spoil ballot!
 */
void spoil_ballot(void);
  
/**
 * @component Smart Ballot Box
 * Cast balot!
 */
void cast_ballot(void);

/**
 * @component Smart Ballot Box
 * Ballot spoiled?
 */
bool ballot_spoiled(void);

/**
 * @component Smart Ballot Box
 * Go to standby!
 */
void go_to_standby(void);

/**
 * @component Smart Ballot Box
 * Ballot Detect Timeout Reset!
 */
void ballot_detect_timeout_reset(void);

/**
 * @component Smart Ballot Box
 * Ballot Detect Timeout Expired?
 */
bool ballot_detect_timeout_expired(void);

/**
 * @component Smart Ballot Box
 * Cast Or Spoil Timeout Reset!
 */
void cast_or_spoil_timeout_reset(void);

/**
 * @component Smart Ballot Box
 * Cast Or Spoil Timeout Expired?
 */
bool cast_or_spoil_timeout_expired(void);

/**
 * @component Smart Ballot Box
 * Main ballot box loop
 */
void ballot_box_main_loop(void);

#endif /* __SBB_H__ */