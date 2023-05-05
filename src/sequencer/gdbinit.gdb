target extended-remote | \
    openocd -f ../stm32f0_basic.cfg -c "gdb_port pipe; log_output openocd.log"
set pagination off
set confirm off
define reload
    load
    continue
    end
monitor reset halt
monitor reset init
set remote hardware-breakpoint-limit 4
set remote hardware-watchpoint-limit 2
load
b main
continue
