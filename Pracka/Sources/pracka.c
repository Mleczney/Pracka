#include "pracka.h"

void PRACKA_Init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;

    PORTC->PCR[1] = PORT_PCR_MUX(1);  // buben vlevo
    PORTD->PCR[0] = PORT_PCR_MUX(1);  // cerpadlo
    PORTE->PCR[0] = PORT_PCR_MUX(1);  // voda
    PORTE->PCR[1] = PORT_PCR_MUX(1);  // topeni
    PORTE->PCR[4] = PORT_PCR_MUX(1);  // rychle otacky
    PORTE->PCR[5] = PORT_PCR_MUX(1);  // buben vpravo

    PTC->PDDR |= (1<<1);
    PTD->PDDR |= (1<<0);
    PTE->PDDR |= (1<<0)|(1<<1)|(1<<4)|(1<<5);
}

void PRACKA_NastavNapousteni(int zap) { zap ? (PTE->PSOR=(1<<0)) : (PTE->PCOR=(1<<0)); }
void PRACKA_NastavTopeni(int zap) { zap ? (PTE->PSOR=(1<<1)) : (PTE->PCOR=(1<<1)); }
void PRACKA_NastavCerpadlo(int zap) { zap ? (PTD->PSOR=(1<<0)) : (PTD->PCOR=(1<<0)); }

void PRACKA_NastavBuben(int smer, int rychle) {
    if (rychle) PTE->PSOR = (1<<4); else PTE->PCOR = (1<<4);
    if (smer == 0) { PTC->PSOR=(1<<1); PTE->PCOR=(1<<5); }
    else { PTE->PSOR=(1<<5); PTC->PCOR=(1<<1); }
}

int PRACKA_CtiTeplotu(void) {
    if (PTC->PDIR & (1<<5)) return 90;
    if (PTD->PDIR & (1<<2)) return 60;
    if (PTC->PDIR & (1<<16)) return 40;
    if (PTD->PDIR & (1<<3)) return 30;
    return 0;
}

int PRACKA_CtiHladinu(void) {
    if (PTD->PDIR & (1<<4)) return 100;
    if (PTC->PDIR & (1<<6)) return 50;
    return 0;
}
