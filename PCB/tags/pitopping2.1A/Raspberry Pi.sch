EESchema Schematic File Version 2  date 8/7/2556 16:17:52
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
Sheet 5 6
Title ""
Date "4 jul 2013"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	3750 3300 3850 3300
Connection ~ 9300 3200
Wire Wire Line
	9100 3200 9300 3200
Connection ~ 9650 4100
Wire Wire Line
	9100 4100 9650 4100
Wire Wire Line
	9100 3400 9650 3400
Wire Wire Line
	9650 3400 9650 4550
Connection ~ 7850 4550
Wire Wire Line
	9650 4550 7850 4550
Connection ~ 7850 4400
Wire Wire Line
	8300 4400 7850 4400
Connection ~ 7750 3200
Wire Wire Line
	7750 3200 7750 4000
Wire Wire Line
	7750 4000 8300 4000
Connection ~ 4850 3200
Wire Wire Line
	4850 3200 4850 4000
Wire Wire Line
	4850 4000 5400 4000
Connection ~ 4950 4400
Wire Wire Line
	4950 4650 4950 3600
Wire Wire Line
	4950 4400 5400 4400
Wire Wire Line
	4950 3600 5400 3600
Wire Wire Line
	6200 3300 6450 3300
Wire Wire Line
	6450 3300 6450 3150
Connection ~ 6600 4100
Wire Wire Line
	6600 3400 6600 4550
Wire Wire Line
	6600 3800 6200 3800
Wire Wire Line
	6600 3400 6200 3400
Wire Wire Line
	6200 3500 6250 3500
Wire Wire Line
	6200 3600 6250 3600
Wire Wire Line
	6600 4100 6200 4100
Connection ~ 6600 3800
Wire Wire Line
	6200 3200 6450 3200
Connection ~ 6450 3200
Wire Wire Line
	6600 4550 4950 4550
Connection ~ 4950 4550
Wire Wire Line
	4800 3200 5400 3200
Wire Wire Line
	7700 3200 8300 3200
Wire Wire Line
	8300 3600 7850 3600
Wire Wire Line
	7850 3600 7850 4650
Wire Wire Line
	9100 3800 9650 3800
Connection ~ 9650 3800
Wire Wire Line
	9300 3150 9300 3300
Wire Wire Line
	9300 3300 9100 3300
Wire Wire Line
	3450 3300 3550 3300
Text GLabel 3850 3300 2    60   Input ~ 0
GPIO0
Text GLabel 3450 3300 0    60   Input ~ 0
MCLR
$Comp
L CONN_2 P19
U 1 1 51D54033
P 3650 2950
F 0 "P19" V 3600 2950 40  0000 C CNN
F 1 "Reset" V 3700 2950 40  0000 C CNN
	1    3650 2950
	0    -1   -1   0   
$EndComp
$Comp
L VCC #PWR040
U 1 1 51A43C8A
P 9300 3150
F 0 "#PWR040" H 9300 3250 30  0001 C CNN
F 1 "VCC" H 9300 3250 30  0000 C CNN
	1    9300 3150
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR041
U 1 1 51A43C83
P 6450 3150
F 0 "#PWR041" H 6450 3250 30  0001 C CNN
F 1 "VCC" H 6450 3250 30  0000 C CNN
	1    6450 3150
	1    0    0    -1  
$EndComp
Text GLabel 6200 4400 2    60   Input ~ 0
GPIO7
Text GLabel 6200 4300 2    60   Input ~ 0
GPIO8
Text GLabel 6200 4000 2    60   Input ~ 0
GPIO24
Text GLabel 6200 4200 2    60   Input ~ 0
GPIO25
Text GLabel 6200 3900 2    60   Input ~ 0
GPIO23
Text GLabel 6200 3700 2    60   Input ~ 0
GPIO18
$Comp
L GNDRP #PWR042
U 1 1 51925973
P 7850 4650
F 0 "#PWR042" H 7850 4650 40  0001 C CNN
F 1 "GNDRP" H 7850 4550 50  0000 C CNN
	1    7850 4650
	1    0    0    -1  
$EndComp
Text GLabel 9100 4400 2    60   Input ~ 0
GPIO7
Text GLabel 9100 4300 2    60   Input ~ 0
GPIO8
Text GLabel 9100 4200 2    60   Input ~ 0
GPIO25
Text GLabel 9100 4000 2    60   Input ~ 0
GPIO24
Text GLabel 9100 3900 2    60   Input ~ 0
GPIO23
Text GLabel 9100 3700 2    60   Input ~ 0
GPIO18
Text GLabel 9100 3600 2    60   Input ~ 0
Rx
Text GLabel 9100 3500 2    60   Input ~ 0
Tx
Text GLabel 8300 4300 0    60   Input ~ 0
GPIO11
Text GLabel 5400 4300 0    60   Input ~ 0
GPIO11
Text GLabel 8300 4200 0    60   Input ~ 0
GPIO9
Text GLabel 5400 4200 0    60   Input ~ 0
GPIO9
Text GLabel 8300 4100 0    60   Input ~ 0
GPIO10
Text GLabel 5400 4100 0    60   Input ~ 0
GPIO10
Text GLabel 8300 3900 0    60   Input ~ 0
GPIO22
Text GLabel 5400 3900 0    60   Input ~ 0
GPIO22
Text GLabel 8300 3800 0    60   Input ~ 0
GPIO21
Text GLabel 5400 3800 0    60   Input ~ 0
GPIO21
Text GLabel 8300 3700 0    60   Input ~ 0
GPIO17
Text GLabel 5400 3700 0    60   Input ~ 0
GPIO17
Text GLabel 8300 3500 0    60   Input ~ 0
GPIO4
Text GLabel 5400 3500 0    60   Input ~ 0
GPIO4
Text GLabel 8300 3400 0    60   Input ~ 0
GPIO1
Text GLabel 5400 3400 0    60   Input ~ 0
GPIO1
Text GLabel 8300 3300 0    60   Input ~ 0
GPIO0
Text GLabel 5400 3300 0    60   Input ~ 0
GPIO0
Text GLabel 7700 3200 0    60   Input ~ 0
3V3
Text GLabel 4800 3200 0    60   Input ~ 0
3V3
$Comp
L CONN_13X2 P15
U 1 1 5192578A
P 8700 3800
F 0 "P15" H 8700 4500 60  0000 C CNN
F 1 "CONN_13X2" V 8700 3800 50  0000 C CNN
	1    8700 3800
	1    0    0    -1  
$EndComp
Text GLabel 6250 3600 2    60   Input ~ 0
Tx
Text GLabel 6250 3500 2    60   Input ~ 0
Rx
$Comp
L GNDRP #PWR043
U 1 1 50AF4738
P 4950 4650
F 0 "#PWR043" H 4950 4650 40  0001 C CNN
F 1 "GNDRP" H 4950 4580 50  0000 C CNN
	1    4950 4650
	1    0    0    -1  
$EndComp
$Comp
L CONN_13X2 P11
U 1 1 50AF4688
P 5800 3800
F 0 "P11" H 5800 4500 60  0000 C CNN
F 1 "Pi-Topping" V 5800 3800 50  0000 C CNN
	1    5800 3800
	1    0    0    -1  
$EndComp
$EndSCHEMATC
