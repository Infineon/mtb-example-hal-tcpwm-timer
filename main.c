/*******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the HAL TCPWM TIMER code example
*          for ModusToolbox. It enbale TCPWM timer mode to generate 1s interrupt
*
* Related Document: See README.md
*
********************************************************************************
* Copyright 2022-2024, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
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
