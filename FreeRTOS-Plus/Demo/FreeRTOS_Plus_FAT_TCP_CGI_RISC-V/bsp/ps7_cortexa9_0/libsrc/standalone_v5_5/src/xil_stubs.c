#include "xil_cache.h"
#include "xil_mmu.h"

void Xil_DCacheEnable(void) {;}
void Xil_DCacheDisable(void) {;}
void Xil_DCacheInvalidate(void) {;}
void Xil_DCacheInvalidateRange(INTPTR adr, u32 len) {;}
void Xil_DCacheFlush(void) {;}
void Xil_DCacheFlushRange(INTPTR adr, u32 len) {;}

void Xil_ICacheEnable(void) {;}
void Xil_ICacheDisable(void) {;}
void Xil_ICacheInvalidate(void) {;}
void Xil_ICacheInvalidateRange(INTPTR adr, u32 len) {;}

void Xil_SetTlbAttributes(INTPTR Addr, u32 attrib) {;}
