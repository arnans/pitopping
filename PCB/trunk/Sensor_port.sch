EESchema Schematic File Version 2  date 8/7/2556 17:49:26
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:microchip1
LIBS:UND2916B
LIBS:LB1836M
LIBS:sd-cache
EELAYER 25  0
EELAYER END
$Descr A4 11700 8267
encoding utf-8
Sheet 4 6
Title ""
Date "8 jul 2013"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text GLabel 3700 2550 0    60   Input ~ 0
CLK_SS
Wire Wire Line
	4400 6650 3700 6650
Wire Wire Line
	3700 5450 4400 5450
Wire Wire Line
	3700 4250 4400 4250
Wire Wire Line
	3700 3050 4400 3050
Wire Wire Line
	3700 2450 4400 2450
Connection ~ 3900 2550
Connection ~ 3900 5550
Wire Wire Line
	3900 6150 4400 6150
Connection ~ 3900 4350
Wire Wire Line
	3900 4950 4400 4950
Connection ~ 3900 3150
Wire Wire Line
	3900 3750 4400 3750
Connection ~ 4100 2650
Wire Wire Line
	4100 2250 4100 6850
Connection ~ 4100 3850
Wire Wire Line
	4100 3250 4400 3250
Connection ~ 4100 5050
Wire Wire Line
	4100 4450 4400 4450
Connection ~ 4100 6250
Wire Wire Line
	4100 5650 4400 5650
Connection ~ 4300 6950
Wire Wire Line
	4300 7000 4300 2750
Connection ~ 4300 5750
Wire Wire Line
	4300 6350 4400 6350
Connection ~ 4300 4550
Wire Wire Line
	4300 5150 4400 5150
Connection ~ 4300 3350
Wire Wire Line
	4300 3950 4400 3950
Wire Wire Line
	4300 2750 4400 2750
Wire Wire Line
	4300 3350 4400 3350
Wire Wire Line
	4300 4550 4400 4550
Connection ~ 4300 3950
Wire Wire Line
	4300 5750 4400 5750
Connection ~ 4300 5150
Wire Wire Line
	4300 6950 4400 6950
Connection ~ 4300 6350
Wire Wire Line
	4100 6850 4400 6850
Wire Wire Line
	4100 6250 4400 6250
Wire Wire Line
	4100 5050 4400 5050
Connection ~ 4100 5650
Wire Wire Line
	4100 3850 4400 3850
Connection ~ 4100 4450
Wire Wire Line
	4100 2650 4400 2650
Connection ~ 4100 3250
Wire Wire Line
	3900 3150 4400 3150
Wire Wire Line
	3900 4350 4400 4350
Connection ~ 3900 3750
Wire Wire Line
	3900 5550 4400 5550
Connection ~ 3900 4950
Wire Wire Line
	3900 2550 3900 6750
Wire Wire Line
	3900 6750 4400 6750
Connection ~ 3900 6150
Wire Wire Line
	3700 2550 4400 2550
Wire Wire Line
	3700 3650 4400 3650
Wire Wire Line
	3700 4850 4400 4850
Wire Wire Line
	3700 6050 4400 6050
$Comp
L +3.3V #PWR037
U 1 1 51D53F28
P 4100 2250
F 0 "#PWR037" H 4100 2210 30  0001 C CNN
F 1 "+3.3V" H 4100 2360 30  0000 C CNN
	1    4100 2250
	1    0    0    -1  
$EndComp
$Comp
L GND3V3 #PWR038
U 1 1 51D53EFD
P 4300 7000
F 0 "#PWR038" H 4300 7000 40  0001 C CNN
F 1 "GND3V3" H 4300 6930 50  0000 C CNN
	1    4300 7000
	1    0    0    -1  
$EndComp
$Comp
L CONN_4 P18
U 1 1 51D53ECD
P 4750 6800
F 0 "P18" V 4700 6800 50  0000 C CNN
F 1 "SS8" V 4800 6800 50  0000 C CNN
	1    4750 6800
	1    0    0    -1  
$EndComp
$Comp
L CONN_4 P17
U 1 1 51D53ECB
P 4750 6200
F 0 "P17" V 4700 6200 50  0000 C CNN
F 1 "SS7" V 4800 6200 50  0000 C CNN
	1    4750 6200
	1    0    0    -1  
$EndComp
$Comp
L CONN_4 P10
U 1 1 51D53EC7
P 4750 5600
F 0 "P10" V 4700 5600 50  0000 C CNN
F 1 "SS6" V 4800 5600 50  0000 C CNN
	1    4750 5600
	1    0    0    -1  
$EndComp
$Comp
L CONN_4 P9
U 1 1 51D53EC5
P 4750 5000
F 0 "P9" V 4700 5000 50  0000 C CNN
F 1 "SS5" V 4800 5000 50  0000 C CNN
	1    4750 5000
	1    0    0    -1  
$EndComp
$Comp
L CONN_4 P8
U 1 1 51D53EC1
P 4750 4400
F 0 "P8" V 4700 4400 50  0000 C CNN
F 1 "SS4" V 4800 4400 50  0000 C CNN
	1    4750 4400
	1    0    0    -1  
$EndComp
$Comp
L CONN_4 P7
U 1 1 51D53EBE
P 4750 3800
F 0 "P7" V 4700 3800 50  0000 C CNN
F 1 "SS3" V 4800 3800 50  0000 C CNN
	1    4750 3800
	1    0    0    -1  
$EndComp
$Comp
L CONN_4 P6
U 1 1 51D53EBC
P 4750 3200
F 0 "P6" V 4700 3200 50  0000 C CNN
F 1 "SS2" V 4800 3200 50  0000 C CNN
	1    4750 3200
	1    0    0    -1  
$EndComp
$Comp
L CONN_4 P5
U 1 1 51D53EB7
P 4750 2600
F 0 "P5" V 4700 2600 50  0000 C CNN
F 1 "SS1" V 4800 2600 50  0000 C CNN
	1    4750 2600
	1    0    0    -1  
$EndComp
Text HLabel 3700 6650 0    60   Input ~ 0
SS8
Text HLabel 3700 6050 0    60   Input ~ 0
SS7
Text HLabel 3700 5450 0    60   Input ~ 0
SS6
Text HLabel 3700 4850 0    60   Input ~ 0
SS5
Text HLabel 3700 4250 0    60   Input ~ 0
SS4
Text HLabel 3700 3650 0    60   Input ~ 0
SS3
Text HLabel 3700 3050 0    60   Input ~ 0
SS2
Text HLabel 3700 2450 0    60   Input ~ 0
SS1
$EndSCHEMATC
