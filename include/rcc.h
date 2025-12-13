#pragma once
#include "helper.h"



struct rcc {
    volatile uint32_t CR, ICSCR, CFGR, PLLCFGR, PLLSAI1CFGR, PLLSAI2CFGR;
    volatile uint32_t CIER, CIFR, CICR, RESERVED0;
    volatile uint32_t AHB1RSTR, AHB2RSTR, AHB3RSTR, RESERVED1;
    volatile uint32_t APB1RSTR1, APB1RSTR2, APB2RSTR, RESERVED2;
    volatile uint32_t AHB1ENR, AHB2ENR, AHB3ENR, RESERVED3;
    volatile uint32_t APB1ENR1, APB1ENR2, APB2ENR, RESERVED4;
    volatile uint32_t AHB1SMENR, AHB2SMENR, AHB3SMENR, RESERVED5;
    volatile uint32_t APB1SMENR1, APB1SMENR2, APB2SMENR, RESERVED6;
    volatile uint32_t CCIPR, RESERVED7, BDCR, CSR, CRRCR, CCIPR2;
};

#define RCC ((struct rcc *)0x40021000)

