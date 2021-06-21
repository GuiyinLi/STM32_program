SET PATH=E:\Keil5 MDK\ARM\ARMCC\Bin;C:\ProgramData\Oracle\Java\javapath;C:\Program Files (x86)\Common Files\Oracle\Java\javapath;C:\Program Files (x86)\Intel\TXE Components\iCLS\;C:\Program Files\Intel\TXE Components\iCLS\;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\Program Files\Intel\TXE Components\DAL\;C:\Program Files (x86)\Intel\TXE Components\DAL\;C:\Program Files\Intel\TXE Components\IPT\;C:\Program Files (x86)\Intel\TXE Components\IPT\;C:\WINDOWS\System32\OpenSSH\;D:\MATLAB\runtime\win64;D:\MATLAB\bin;C:\Program Files\Java\jdk1.8.0_201\bin;E:\Git\cmd;C:\Users\Dell\AppData\Local\Programs\Python\Python35\Scripts\;C:\Users\Dell\AppData\Local\Programs\Python\Python35\;C:\Users\Dell\AppData\Local\Microsoft\WindowsApps;;E:\Microsoft VS Code\bin
SET ARMCC5_ASMOPT=--diag_suppress=9931
SET ARMCC5_CCOPT=--diag_suppress=9931
SET ARMCC5_LINKOPT=--diag_suppress=9931
SET CPU_TYPE=STM32F103C8
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=TIM3-PWM-DEMO
SET CPU_CLOCK=0x00B71B00
"E:\Keil5 MDK\ARM\ARMCC\Bin\ArmAsm" --Via ".\startup_stm32f10x_hd._ia"
"E:\Keil5 MDK\ARM\ARMCC\Bin\ArmCC" --Via ".\main.__i"
"E:\Keil5 MDK\ARM\ARMCC\Bin\ArmCC" --Via ".\pwm_output.__i"
"E:\Keil5 MDK\ARM\ARMCC\Bin\ArmCC" --Via ".\stm32f10x_it.__i"
"E:\Keil5 MDK\ARM\ARMCC\Bin\ArmCC" --Via ".\led.__i"
"E:\Keil5 MDK\ARM\ARMCC\Bin\ArmCC" --Via ".\misc.__i"
"E:\Keil5 MDK\ARM\ARMCC\Bin\ArmCC" --Via ".\stm32f10x_tim.__i"
"E:\Keil5 MDK\ARM\ARMCC\Bin\ArmCC" --Via ".\stm32f10x_gpio.__i"
"E:\Keil5 MDK\ARM\ARMCC\Bin\ArmCC" --Via ".\stm32f10x_rcc.__i"
"E:\Keil5 MDK\ARM\ARMCC\Bin\ArmCC" --Via ".\stm32f10x_flash.__i"
"E:\Keil5 MDK\ARM\ARMCC\Bin\ArmCC" --Via ".\core_cm3.__i"
"E:\Keil5 MDK\ARM\ARMCC\Bin\ArmCC" --Via ".\system_stm32f10x.__i"
"E:\Keil5 MDK\ARM\ARMCC\Bin\ArmLink" --Via ".\STM32-DEMO.lnp"
"E:\Keil5 MDK\ARM\ARMCC\Bin\fromelf.exe" ".\STM32-DEMO.axf" --i32combined --output ".\STM32-DEMO.hex"
