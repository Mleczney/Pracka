#include "MKL25Z4.h"
#include "drv_lcd.h"
#include "pracka.h"
#include <stdio.h>

typedef enum {
    STATE_INIT,
    STATE_FILLING,
    STATE_HEATING,
    STATE_WASHING,
    STATE_DRAINING,
    STATE_SPINNING,
    STATE_DONE,
    STATE_STOPPED
} PrackaState;

// --- Globální proměnné ---
PrackaState state = STATE_INIT;
int targetTemp = 40;
int stopPressed = 0;

// --- Prototypy ---
void ADC_Init(void);
int  ADC_Read(void);
void DelayMs(uint32_t ms);
void Buttons_Init(void);
void CheckStopButton(void);

// --- Hlavní program ---
int main(void)
{
    SystemCoreClockUpdate();
    PRACKA_Init();
    LCD_initialize();
    LCD_backlight_on();
    ADC_Init();
    Buttons_Init();

    LCD_clear();
    LCD_puts("Pracka ready!");

    while (1) {
        CheckStopButton();
        if (stopPressed)
            state = STATE_STOPPED;

        switch (state) {
        case STATE_INIT:
            LCD_clear();
            LCD_puts("Nastav teplotu");
            DelayMs(1500);

            // Čtení potenciometru
            targetTemp = (ADC_Read() * 90) / 255;
            if (targetTemp < 30) targetTemp = 30;
            if (targetTemp > 90) targetTemp = 90;

            {
                char buf[16];
                sprintf(buf, "Cil: %dC", targetTemp);
                LCD_clear();
                LCD_puts(buf);
            }
            DelayMs(2000);
            state = STATE_FILLING;
            break;

        case STATE_FILLING:
            LCD_clear();
            LCD_puts("Napousteni vody");
            PRACKA_NastavNapousteni(1);

            while (PRACKA_CtiHladinu() < 100 && !stopPressed) {
                CheckStopButton();
            }
            PRACKA_NastavNapousteni(0);
            state = STATE_HEATING;
            break;

        case STATE_HEATING:
            LCD_clear();
            LCD_puts("Ohrev vody");
            PRACKA_NastavTopeni(1);

            while (PRACKA_CtiTeplotu() < targetTemp && !stopPressed) {
                CheckStopButton();
            }
            PRACKA_NastavTopeni(0);
            state = STATE_WASHING;
            break;

        case STATE_WASHING:
            LCD_clear();
            LCD_puts("Prani...");
            for (int i = 0; i < 6 && !stopPressed; i++) {
                PRACKA_NastavBuben(0, 0); // vlevo
                DelayMs(2000);
                PRACKA_NastavBuben(1, 0); // vpravo
                DelayMs(2000);
            }
            PRACKA_NastavBuben(0, 0);
            state = STATE_DRAINING;
            break;

        case STATE_DRAINING:
            LCD_clear();
            LCD_puts("Vypousteni");
            PRACKA_NastavCerpadlo(1);
            DelayMs(3000);
            PRACKA_NastavCerpadlo(0);
            state = STATE_SPINNING;
            break;

        case STATE_SPINNING:
            LCD_clear();
            LCD_puts("Zdimani...");
            PRACKA_NastavBuben(1, 1);
            DelayMs(5000);
            PRACKA_NastavBuben(0, 0);
            state = STATE_DONE;
            break;

        case STATE_DONE:
            LCD_clear();
            LCD_puts("Program hotov!");
            DelayMs(5000);
            state = STATE_INIT;
            break;

        case STATE_STOPPED:
            LCD_clear();
            LCD_puts("STOP stisknut!");
            PRACKA_NastavNapousteni(0);
            PRACKA_NastavTopeni(0);
            PRACKA_NastavCerpadlo(0);
            PRACKA_NastavBuben(0, 0);
            DelayMs(2000);
            stopPressed = 0;
            state = STATE_INIT;
            break;
        }
    }
}

// --- ADC (potenciometr) ---
void ADC_Init(void) {
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
    ADC0->CFG1 = ADC_CFG1_MODE(1) | ADC_CFG1_ADIV(2);
}

int ADC_Read(void) {
    ADC0->SC1[0] = 0; // kanál 0
    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
    return ADC0->R[0] >> 2; // 10bit -> 8bit
}

// --- Tlačítko STOP ---
void Buttons_Init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    PORTA->PCR[1] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; // s pull-up
    PTA->PDDR &= ~(1 << 1); // vstup
}

void CheckStopButton(void) {
    if (!(PTA->PDIR & (1 << 1))) stopPressed = 1; // aktivní LOW
}

// --- Delay ---
void DelayMs(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 6000; i++) {
        __NOP();
    }
}
