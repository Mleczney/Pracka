#ifndef PRACKA_H
#define PRACKA_H

#include "MKL25Z4.h"

void PRACKA_Init(void);
void PRACKA_NastavNapousteni(int zap);
void PRACKA_NastavTopeni(int zap);
void PRACKA_NastavBuben(int smer, int rychle);
void PRACKA_NastavCerpadlo(int zap);
int  PRACKA_CtiTeplotu(void);
int  PRACKA_CtiHladinu(void);

#endif
