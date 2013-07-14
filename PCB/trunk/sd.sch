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
Sheet 1 6
Title ""
Date "8 jul 2013"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 4650 3150 2500 2000
U 4FB621D5
F0 "PIC18F66J50" 60
F1 "PIC18F66J50.sch" 60
F2 "SS1" I R 7150 5100 60 
F3 "SS2" I R 7150 5000 60 
F4 "SS3" I R 7150 4900 60 
F5 "SS4" I R 7150 4800 60 
F6 "MD42" I R 7150 4150 60 
F7 "MD41" I R 7150 4050 60 
F8 "MD32" I R 7150 3950 60 
F9 "MD31" I R 7150 3850 60 
F10 "M2" I R 7150 3750 60 
F11 "MD21" I R 7150 3550 60 
F12 "MD22" I R 7150 3650 60 
F13 "MD12" I R 7150 3450 60 
F14 "MD11" I R 7150 3350 60 
F15 "M1" I R 7150 3250 60 
F16 "SS8" I R 7150 4400 60 
F17 "SS7" I R 7150 4500 60 
F18 "SS6" I R 7150 4600 60 
F19 "SS5" I R 7150 4700 60 
$EndSheet
$Sheet
S 8500 3150 950  1100
U 50AF30D7
F0 "Motor Driver" 60
F1 "Motor Driver.sch" 60
F2 "M2" I L 8500 3750 60 
F3 "M1" I L 8500 3250 60 
F4 "MD42" I L 8500 4150 60 
F5 "MD41" I L 8500 4050 60 
F6 "MD32" I L 8500 3950 60 
F7 "MD31" I L 8500 3850 60 
F8 "MD22" I L 8500 3650 60 
F9 "MD21" I L 8500 3550 60 
F10 "MD12" I L 8500 3450 60 
F11 "MD11" I L 8500 3350 60 
$EndSheet
Wire Wire Line
	7150 4050 8500 4050
Wire Wire Line
	7150 3850 8500 3850
Wire Wire Line
	7150 3650 8500 3650
Wire Wire Line
	7150 3450 8500 3450
Wire Wire Line
	7150 3250 8500 3250
Wire Wire Line
	7150 4600 7350 4600
Wire Wire Line
	7150 4400 7350 4400
Wire Wire Line
	7150 5000 7350 5000
Wire Wire Line
	7150 4800 7350 4800
Wire Wire Line
	7150 4900 7350 4900
Wire Wire Line
	7150 5100 7350 5100
Wire Wire Line
	7150 4500 7350 4500
Wire Wire Line
	7150 4700 7350 4700
Wire Wire Line
	7150 3350 8500 3350
Wire Wire Line
	7150 3550 8500 3550
Wire Wire Line
	7150 3750 8500 3750
Wire Wire Line
	7150 3950 8500 3950
Wire Wire Line
	7150 4150 8500 4150
$Sheet
S 7350 4350 1000 800 
U 4FB5EBA8
F0 "Sensor_port" 60
F1 "Sensor_port.sch" 60
F2 "SS1" I L 7350 5100 60 
F3 "SS2" I L 7350 5000 60 
F4 "SS3" I L 7350 4900 60 
F5 "SS4" I L 7350 4800 60 
F6 "SS8" I L 7350 4400 60 
F7 "SS7" I L 7350 4500 60 
F8 "SS6" I L 7350 4600 60 
F9 "SS5" I L 7350 4700 60 
$EndSheet
$Sheet
S 3400 4700 750  450 
U 50AF462D
F0 "Raspberry Pi" 60
F1 "Raspberry Pi.sch" 60
$EndSheet
$Sheet
S 3300 3150 800  450 
U 4FB5EB41
F0 "Power_supply" 60
F1 "Power_supply.sch" 60
$EndSheet
$EndSCHEMATC
