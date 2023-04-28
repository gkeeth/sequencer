target remote localhost:3333
set pagination off
set confirm off
monitor reset halt
monitor reset init
set remote hardware-breakpoint-limit 4
set remote hardware-watchpoint-limit 2
load
b main
continue
