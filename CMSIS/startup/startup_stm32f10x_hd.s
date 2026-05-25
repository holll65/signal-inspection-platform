.syntax unified
.cpu cortex-m3
.thumb

.global g_pfnVectors
.global Reset_Handler

.word _estack

.extern _sidata
.extern _sdata
.extern _edata
.extern _sbss
.extern _ebss

.section .isr_vector,"a",%progbits
g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word MemManage_Handler
  .word BusFault_Handler
  .word UsageFault_Handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word SVC_Handler
  .word DebugMon_Handler
  .word 0
  .word PendSV_Handler
  .word SysTick_Handler

.text
Reset_Handler:
  bl SystemInit
  ldr r0, =_sidata
  ldr r1, =_sdata
  ldr r2, =_edata
CopyData:
  cmp r1, r2
  bcs ZeroBss
  ldr r3, [r0], #4
  str r3, [r1], #4
  b CopyData

ZeroBss:
  ldr r1, =_sbss
  ldr r2, =_ebss
  movs r3, #0
ZeroBssLoop:
  cmp r1, r2
  bcs CallInit
  str r3, [r1], #4
  b ZeroBssLoop

CallInit:
  bl main
LoopForever:
  b LoopForever

.weak NMI_Handler
.thumb_set NMI_Handler,Default_Handler
.weak HardFault_Handler
.thumb_set HardFault_Handler,Default_Handler
.weak MemManage_Handler
.thumb_set MemManage_Handler,Default_Handler
.weak BusFault_Handler
.thumb_set BusFault_Handler,Default_Handler
.weak UsageFault_Handler
.thumb_set UsageFault_Handler,Default_Handler
.weak SVC_Handler
.thumb_set SVC_Handler,Default_Handler
.weak DebugMon_Handler
.thumb_set DebugMon_Handler,Default_Handler
.weak PendSV_Handler
.thumb_set PendSV_Handler,Default_Handler
.weak SysTick_Handler
.thumb_set SysTick_Handler,Default_Handler

Default_Handler:
  b .
