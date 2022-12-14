/*******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the HAL TCPWM TIMER
*              Example for ModusToolbox.
*
* Related Document: See README.md
*
********************************************************************************
* (c) 2022-YEAR, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************/

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cyhal.h"
#include "cybsp.h"


/*******************************************************************************
* Macros
*******************************************************************************/
#define TIMER_INT_PRIORITY       (3u)
#define TIMER_TARGET_FREQUENCY   (10000u)
#define TIMER_COUNT_PERIOD       (9999u)


/*******************************************************************************
* Global Variables
*******************************************************************************/
/*timer object used*/
cyhal_timer_t timer_obj;

/*timer configuration*/
const cyhal_timer_cfg_t timer_cfg =
{
    .compare_value = 0,                  // Timer compare value, not used
    .period        = TIMER_COUNT_PERIOD, // Defines the timer period
    .direction     = CYHAL_TIMER_DIR_UP, // Timer counts up
    .is_compare    = false,              // Don't use compare mode
    .is_continuous = true,               // Run the timer indefinitely
    .value         = 0                   // Initial value of counter
};


/*******************************************************************************
* Function Prototypes
*******************************************************************************/
static void isr_timer(void* callback_arg, cyhal_timer_event_t event);


/*******************************************************************************
* Function Definitions
*******************************************************************************/

/*******************************************************************************
* Function Name: isr_timer
********************************************************************************
* Summary:
* This is the timer interrupt callback function.
*
* Parameters:
*  void* callback_arg
*  cyhal_timer_event_t event
*
* Return:
* NC
*
*******************************************************************************/
static void isr_timer(void* callback_arg, cyhal_timer_event_t event)
{
    (void)callback_arg;
    (void)event;

    /* Invert the USER LED state */
    cyhal_gpio_toggle(CYBSP_USER_LED);
}


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* Timer mode to generate the interrupt each 1s. Invert USER LED when interrupt
* generated.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    /* Board initialization failed. stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize the User LED */
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
            CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    /* LED initialization failed. stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize the timer object. Does not use pin output ('pin' is NC) and
     * does not use a pre-configured clock source ('clk' is NULL) */
    result = cyhal_timer_init(&timer_obj, NC, NULL);

    /*Apply timer configuration such as period, count direction, run mode, etc*/
    if (CY_RSLT_SUCCESS == result)
    {
        result = cyhal_timer_configure(&timer_obj, &timer_cfg);
    }

    /* Set the frequency of timer to 10000 Hz */
    if (CY_RSLT_SUCCESS == result)
    {
        result = cyhal_timer_set_frequency(&timer_obj, TIMER_TARGET_FREQUENCY);
    }

    /* register timer interrupt and enable */
    if (CY_RSLT_SUCCESS == result)
    {
        /* Assign the ISR to execute on timer interrupt */
        cyhal_timer_register_callback(&timer_obj, isr_timer, NULL);

        /* Set the event on which timer interrupt occurs and enable it */
        cyhal_timer_enable_event(&timer_obj, CYHAL_TIMER_IRQ_TERMINAL_COUNT,
                TIMER_INT_PRIORITY, true);

        /* Start the timer with the configured settings */
        result = cyhal_timer_start(&timer_obj);
    }

    for (;;)
    {

    }
}


/* [] END OF FILE */
