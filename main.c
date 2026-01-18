#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"


void delay_ms(int t) {
  vTaskDelay(t /portTICK_PERIOD_MS);
}

#define DRIVER_SEAT_SENSOR GPIO_NUM_14
#define DRIVER_SEATBELT_SENSOR GPIO_NUM_13
#define PASSENGER_SEAT_SENSOR GPIO_NUM_10
#define PASSENGER_SEATBELT_SENSOR GPIO_NUM_9
#define IGNITION_PUSH_BUTTON GPIO_NUM_5
#define YELLOW_ENGINE_LED GPIO_NUM_7
#define GREEN_IGNITION_ENABLED_LED GPIO_NUM_2
#define BUZZER GPIO_NUM_17

bool DriverSeat;
bool DriverSeatBelt;
bool PassengerSeat;
bool PassengerSeatBelt;
bool IgnitionButtonPressed=false;
bool IgnitionButtonState = false;
bool DriverSeated;
bool Ignition = false;
bool lastDriverSeat = false;


void app_main(void)
{
    //Setting up Driver Seat Sensor Pin
    gpio_reset_pin(DRIVER_SEAT_SENSOR);
    gpio_pullup_en(DRIVER_SEAT_SENSOR);
    gpio_set_direction(DRIVER_SEAT_SENSOR, GPIO_MODE_INPUT);

    //Setting up Driver Seatbelt Sensor Pin
    gpio_reset_pin(DRIVER_SEATBELT_SENSOR);
    gpio_pullup_en(DRIVER_SEATBELT_SENSOR);
    gpio_set_direction(DRIVER_SEATBELT_SENSOR, GPIO_MODE_INPUT);

    //Setting up Passenger Seat Sensor Pin
    gpio_reset_pin(PASSENGER_SEAT_SENSOR);
    gpio_pullup_en(PASSENGER_SEAT_SENSOR);
    gpio_set_direction(PASSENGER_SEAT_SENSOR, GPIO_MODE_INPUT);

    //Setting up Passenger Seatbelt Sensor Pin
    gpio_reset_pin(PASSENGER_SEATBELT_SENSOR);
    gpio_pullup_en(PASSENGER_SEATBELT_SENSOR);
    gpio_set_direction(PASSENGER_SEATBELT_SENSOR, GPIO_MODE_INPUT);

    //Setting up Ignition Button Pin
    gpio_reset_pin(IGNITION_PUSH_BUTTON);
    gpio_pullup_en(IGNITION_PUSH_BUTTON);
    gpio_set_direction(IGNITION_PUSH_BUTTON, GPIO_MODE_INPUT);

    //Setting up Yellow Engine LED
    gpio_reset_pin(YELLOW_ENGINE_LED);
    gpio_set_direction(YELLOW_ENGINE_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(YELLOW_ENGINE_LED, 0);

    //Setting up Green Ignition Enabled LED
    gpio_reset_pin(GREEN_IGNITION_ENABLED_LED);
    gpio_set_direction(GREEN_IGNITION_ENABLED_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(GREEN_IGNITION_ENABLED_LED, 0);

    //Setting up Buzzer
    gpio_reset_pin(BUZZER);
    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);
    gpio_set_level(BUZZER, 0);

    DriverSeated = false;


    while (1){
        DriverSeat = gpio_get_level(DRIVER_SEAT_SENSOR)==0;
        DriverSeatBelt = gpio_get_level(DRIVER_SEATBELT_SENSOR) == 0;
        PassengerSeat = gpio_get_level(PASSENGER_SEAT_SENSOR) == 0;
        PassengerSeatBelt = gpio_get_level(PASSENGER_SEATBELT_SENSOR) == 0;
        IgnitionButtonPressed = gpio_get_level(IGNITION_PUSH_BUTTON) == 0;

        //message when driver sits down
        //maybe add the last_driver bool if code doesnt work
        if (DriverSeat &&!lastDriverSeat){
            printf("Welcome to enhanced alarm system model 218-W25\n");
            DriverSeated = true;
            lastDriverSeat=DriverSeat;
        }

        // check if the ignition can start 
        if (DriverSeat && DriverSeatBelt && PassengerSeat && PassengerSeatBelt && !IgnitionButtonPressed){
            gpio_set_level(GREEN_IGNITION_ENABLED_LED, 1);
        }
        else 
        {
            gpio_set_level(GREEN_IGNITION_ENABLED_LED, 0);
        }

        // ignition button pressed after initially being not pressed
        if (IgnitionButtonPressed && !Ignition)

        {
            Ignition = true;
         } 
         if (!IgnitionButtonPressed && Ignition){//locks the system after the first press

            if (DriverSeat && PassengerSeat && DriverSeatBelt && PassengerSeatBelt)
            {
                // engine starts
                gpio_set_level(GREEN_IGNITION_ENABLED_LED, 0);
                gpio_set_level(YELLOW_ENGINE_LED, 1);
                printf("Engine started.\n");
        
            }
            else {
                //ignition inhibited 
                gpio_set_level(BUZZER,1);
                printf("Ignition inhibited.\n");

                if (!DriverSeat)
                    printf("- Driver seat not occupied.\n");

                if (!PassengerSeat)
                    printf("- Passenger seat not occupied.\n");

                if (!DriverSeatBelt)
                    printf("- Driver seatbelt not fastened.\n");

                if (!PassengerSeatBelt)
                    printf("- Passenger seatbelt not fastened.\n");
            }
        }

       delay_ms(25);
        
    }

}