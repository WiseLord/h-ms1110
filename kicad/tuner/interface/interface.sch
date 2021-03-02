EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L h-ms1110:H-MS1110_Bus_connector J1
U 1 1 5E2487D7
P 3950 1000
F 0 "J1" V 4300 2850 50  0000 L CNN
F 1 "H-MS1110_Bus" V 4300 650 50  0000 L CNN
F 2 "Connector_IDC:IDC-Header_2x20_P2.54mm_Horizontal" V 4300 1000 50  0001 C CNN
F 3 "~" H 3750 1000 50  0001 C CNN
	1    3950 1000
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J8
U 1 1 5E21D9E6
P 10800 3700
F 0 "J8" H 10800 3900 50  0000 C CNN
F 1 "I2C" V 10900 3650 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B4B-EH-A_1x04_P2.50mm_Vertical" H 10800 3700 50  0001 C CNN
F 3 "~" H 10800 3700 50  0001 C CNN
	1    10800 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	10600 3700 10400 3700
Wire Wire Line
	10600 3800 10400 3800
$Comp
L power:GNDD #PWR0101
U 1 1 5E22176D
P 10450 3950
F 0 "#PWR0101" H 10450 3700 50  0001 C CNN
F 1 "GNDD" H 10454 3795 50  0000 C CNN
F 2 "" H 10450 3950 50  0001 C CNN
F 3 "" H 10450 3950 50  0001 C CNN
	1    10450 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	10600 3900 10450 3900
Wire Wire Line
	10450 3900 10450 3950
NoConn ~ 10600 3600
$Comp
L Connector_Generic:Conn_01x05 J2
U 1 1 5E22EB45
P 900 1850
F 0 "J2" H 900 2150 50  0000 C CNN
F 1 "EXT_INPUT" V 1000 1850 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B5B-EH-A_1x05_P2.50mm_Vertical" H 900 1850 50  0001 C CNN
F 3 "~" H 900 1850 50  0001 C CNN
	1    900  1850
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3050 1400 3050 2050
Text Label 1150 1850 0    50   ~ 0
RDDA
Text Label 1150 1950 0    50   ~ 0
RDCL
Text Label 1150 2050 0    50   ~ 0
GNDD
$Comp
L Connector_Generic:Conn_01x04 J3
U 1 1 5E236DDB
P 900 2450
F 0 "J3" H 900 2650 50  0000 C CNN
F 1 "SYNC" V 1000 2400 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B4B-EH-A_1x04_P2.50mm_Vertical" H 900 2450 50  0001 C CNN
F 3 "~" H 900 2450 50  0001 C CNN
	1    900  2450
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1100 2450 1750 2450
Wire Wire Line
	1600 2650 1100 2650
Text Label 1150 2550 0    50   ~ 0
SYNC_SCK
Text Label 1150 2450 0    50   ~ 0
SYNC_DATA
Text Label 1150 2650 0    50   ~ 0
GNDD
NoConn ~ 2250 1400
NoConn ~ 2350 1400
NoConn ~ 2450 1400
NoConn ~ 2550 1400
NoConn ~ 2650 1400
NoConn ~ 2750 1400
$Comp
L Connector_Generic:Conn_01x04 J5
U 1 1 5E277273
P 900 3550
F 0 "J5" H 900 3750 50  0000 C CNN
F 1 "CONTROL" V 1000 3500 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B4B-EH-A_1x04_P2.50mm_Vertical" H 900 3550 50  0001 C CNN
F 3 "~" H 900 3550 50  0001 C CNN
	1    900  3550
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3550 2450 3550 1400
Wire Wire Line
	3750 2550 3750 1400
Wire Wire Line
	1100 3750 1600 3750
Connection ~ 1600 3750
Text Label 1150 3450 0    50   ~ 0
MUTE
Text Label 1150 3550 0    50   ~ 0
STBY
Text Label 1150 3650 0    50   ~ 0
REMOTE
Wire Wire Line
	4350 1400 4350 3450
Wire Wire Line
	1100 3450 2450 3450
Wire Wire Line
	4150 1400 4150 3550
Wire Wire Line
	1100 3550 2750 3550
Wire Wire Line
	3450 1400 3450 3650
$Comp
L Connector_Generic:Conn_01x03 J7
U 1 1 5E284892
P 10800 1600
F 0 "J7" H 10800 1800 50  0000 C CNN
F 1 "SPECT" V 10900 1600 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B3B-EH-A_1x03_P2.50mm_Vertical" H 10800 1600 50  0001 C CNN
F 3 "~" H 10800 1600 50  0001 C CNN
	1    10800 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 1500 4650 1400
Wire Wire Line
	5350 1700 5350 1400
Text Label 1150 3750 0    50   ~ 0
GNDD
$Comp
L power:GNDD #PWR0104
U 1 1 5E289601
P 10300 1800
F 0 "#PWR0104" H 10300 1550 50  0001 C CNN
F 1 "GNDD" H 10304 1645 50  0000 C CNN
F 2 "" H 10300 1800 50  0001 C CNN
F 3 "" H 10300 1800 50  0001 C CNN
	1    10300 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	10600 1600 10300 1600
Wire Wire Line
	10300 1600 10300 1800
Text Label 10550 1600 2    50   ~ 0
GNDD
Text Label 10550 1500 2    50   ~ 0
SP_L
Text Label 10550 1700 2    50   ~ 0
SP_R
Wire Wire Line
	5150 1400 5150 2250
$Comp
L power:GNDA #PWR0105
U 1 1 5E297AC6
P 7700 2700
F 0 "#PWR0105" H 7700 2450 50  0001 C CNN
F 1 "GNDA" H 7705 2527 50  0000 C CNN
F 2 "" H 7700 2700 50  0001 C CNN
F 3 "" H 7700 2700 50  0001 C CNN
	1    7700 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 2700 7700 2550
Wire Wire Line
	1100 2550 2050 2550
Wire Wire Line
	3650 1400 3650 2000
$Comp
L Connector_Generic:Conn_01x04 J6
U 1 1 5E2A2176
P 4500 4900
F 0 "J6" H 4500 5100 50  0000 C CNN
F 1 "FP_POWER" V 4600 4850 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B4B-EH-A_1x04_P2.50mm_Vertical" H 4500 4900 50  0001 C CNN
F 3 "~" H 4500 4900 50  0001 C CNN
	1    4500 4900
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0106
U 1 1 5E2A286B
P 4200 4500
F 0 "#PWR0106" H 4200 4350 50  0001 C CNN
F 1 "+5V" H 4215 4673 50  0000 C CNN
F 2 "" H 4200 4500 50  0001 C CNN
F 3 "" H 4200 4500 50  0001 C CNN
	1    4200 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 4800 4200 4800
Wire Wire Line
	4300 4900 4200 4900
Wire Wire Line
	4200 4900 4200 4800
Connection ~ 4200 4800
$Comp
L power:GNDD #PWR0107
U 1 1 5E2A577C
P 4200 5250
F 0 "#PWR0107" H 4200 5000 50  0001 C CNN
F 1 "GNDD" H 4204 5095 50  0000 C CNN
F 2 "" H 4200 5250 50  0001 C CNN
F 3 "" H 4200 5250 50  0001 C CNN
	1    4200 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 5100 4200 5100
Wire Wire Line
	4200 5100 4200 5150
Wire Wire Line
	4300 5000 4200 5000
Wire Wire Line
	4200 5000 4200 5100
Connection ~ 4200 5100
$Comp
L Diode_Bridge:GBU8K D1
U 1 1 5E2BD344
P 1650 4600
F 0 "D1" H 1800 4850 50  0000 L CNN
F 1 "GBU8K" H 1800 4750 50  0000 L CNN
F 2 "Diode_THT:Diode_Bridge_Vishay_GBU" H 1800 4725 50  0001 L CNN
F 3 "http://www.vishay.com/docs/88656/gbu8a.pdf" H 1650 4600 50  0001 C CNN
	1    1650 4600
	1    0    0    -1  
$EndComp
$Comp
L Device:CP_Small C1
U 1 1 5E2C8E45
P 2050 4800
F 0 "C1" H 2138 4846 50  0000 L CNN
F 1 "1000.0" H 2138 4755 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm_P7.50mm" H 2050 4800 50  0001 C CNN
F 3 "~" H 2050 4800 50  0001 C CNN
	1    2050 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 4700 2050 4600
Wire Wire Line
	2050 4600 1950 4600
Wire Wire Line
	1300 4600 1350 4600
Connection ~ 4200 5150
Wire Wire Line
	4200 5150 4200 5250
$Comp
L Device:CP_Small C2
U 1 1 5E2DC279
P 3500 4950
F 0 "C2" H 3588 4996 50  0000 L CNN
F 1 "470.0" H 3588 4905 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 3500 4950 50  0001 C CNN
F 3 "~" H 3500 4950 50  0001 C CNN
	1    3500 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 5050 3500 5150
Wire Wire Line
	3500 5150 3900 5150
Wire Wire Line
	1650 4300 1650 4250
Wire Wire Line
	1650 4250 850  4250
Wire Wire Line
	1650 4900 1650 4950
Wire Wire Line
	1650 4950 850  4950
Text Label 3650 2000 1    50   ~ 0
AC1
Text Label 3850 2000 1    50   ~ 0
AC2
Text Label 850  4250 0    50   ~ 0
AC1
Text Label 850  4950 0    50   ~ 0
AC2
Wire Wire Line
	3850 1400 3850 2000
$Comp
L Device:C_Small C3
U 1 1 5E2FABE3
P 3900 4950
F 0 "C3" H 3992 4996 50  0000 L CNN
F 1 "0.1" H 3992 4905 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W3.5mm_P5.00mm" H 3900 4950 50  0001 C CNN
F 3 "~" H 3900 4950 50  0001 C CNN
	1    3900 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 5050 3900 5150
Connection ~ 3900 5150
Wire Wire Line
	3900 5150 4200 5150
$Comp
L power:GNDA #PWR0108
U 1 1 5E308BBA
P 5250 2000
F 0 "#PWR0108" H 5250 1750 50  0001 C CNN
F 1 "GNDA" H 5255 1827 50  0000 C CNN
F 2 "" H 5250 2000 50  0001 C CNN
F 3 "" H 5250 2000 50  0001 C CNN
	1    5250 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 1400 5750 1600
Wire Wire Line
	5250 2000 5250 1600
Wire Wire Line
	5450 1600 5250 1600
Connection ~ 5450 1600
Wire Wire Line
	5450 1600 5450 1400
Connection ~ 5250 1600
Wire Wire Line
	5250 1600 5250 1400
Wire Wire Line
	5450 1600 5750 1600
Text Label 5850 2000 1    50   ~ 0
TUN_L
Text Label 5950 2000 1    50   ~ 0
TUN_R
Wire Wire Line
	5250 1600 4450 1600
Wire Wire Line
	4450 1600 4450 1400
NoConn ~ 4050 1400
NoConn ~ 5050 1400
$Comp
L Device:CP_Small C5
U 1 1 5E7FE5FC
P 6750 2500
F 0 "C5" H 6838 2546 50  0000 L CNN
F 1 "220.0" H 6838 2455 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 6750 2500 50  0001 C CNN
F 3 "~" H 6750 2500 50  0001 C CNN
	1    6750 2500
	1    0    0    -1  
$EndComp
$Comp
L power:GNDA #PWR0109
U 1 1 5E7FF512
P 6750 2650
F 0 "#PWR0109" H 6750 2400 50  0001 C CNN
F 1 "GNDA" H 6755 2477 50  0000 C CNN
F 2 "" H 6750 2650 50  0001 C CNN
F 3 "" H 6750 2650 50  0001 C CNN
	1    6750 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	6750 2650 6750 2600
Wire Wire Line
	6750 2400 6750 2250
$Comp
L Device:C_Small C6
U 1 1 5E809119
P 7150 2500
F 0 "C6" H 7242 2546 50  0000 L CNN
F 1 "0.1" H 7242 2455 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W3.5mm_P5.00mm" H 7150 2500 50  0001 C CNN
F 3 "~" H 7150 2500 50  0001 C CNN
	1    7150 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	7150 2400 7150 2250
Connection ~ 7150 2250
Wire Wire Line
	7150 2250 7400 2250
$Comp
L power:GNDA #PWR0110
U 1 1 5E80EE36
P 7150 2650
F 0 "#PWR0110" H 7150 2400 50  0001 C CNN
F 1 "GNDA" H 7155 2477 50  0000 C CNN
F 2 "" H 7150 2650 50  0001 C CNN
F 3 "" H 7150 2650 50  0001 C CNN
	1    7150 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	7150 2650 7150 2600
$Comp
L Device:CP_Small C7
U 1 1 5E814F78
P 8150 2500
F 0 "C7" H 8238 2546 50  0000 L CNN
F 1 "220.0" H 8238 2455 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 8150 2500 50  0001 C CNN
F 3 "~" H 8150 2500 50  0001 C CNN
	1    8150 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C8
U 1 1 5E815737
P 8550 2500
F 0 "C8" H 8642 2546 50  0000 L CNN
F 1 "0.1" H 8642 2455 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W3.5mm_P5.00mm" H 8550 2500 50  0001 C CNN
F 3 "~" H 8550 2500 50  0001 C CNN
	1    8550 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 2400 8150 2250
Connection ~ 8150 2250
Wire Wire Line
	8150 2250 8000 2250
Connection ~ 8550 2250
Wire Wire Line
	8550 2250 8150 2250
$Comp
L power:GNDA #PWR0111
U 1 1 5E8201AA
P 8150 2700
F 0 "#PWR0111" H 8150 2450 50  0001 C CNN
F 1 "GNDA" H 8155 2527 50  0000 C CNN
F 2 "" H 8150 2700 50  0001 C CNN
F 3 "" H 8150 2700 50  0001 C CNN
	1    8150 2700
	1    0    0    -1  
$EndComp
$Comp
L power:GNDA #PWR0112
U 1 1 5E8203DC
P 8550 2700
F 0 "#PWR0112" H 8550 2450 50  0001 C CNN
F 1 "GNDA" H 8555 2527 50  0000 C CNN
F 2 "" H 8550 2700 50  0001 C CNN
F 3 "" H 8550 2700 50  0001 C CNN
	1    8550 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 2700 8550 2600
Wire Wire Line
	8150 2600 8150 2700
Wire Wire Line
	8550 2250 8550 2400
$Comp
L h-ms1110:MP1584EN_module U1
U 1 1 5E7F5A1F
P 2900 4750
F 0 "U1" H 2900 5265 50  0000 C CNN
F 1 "MP1584EN_module" H 2900 5174 50  0000 C CNN
F 2 "h-ms1110:MP1584EN_module" H 3200 5150 50  0001 C CNN
F 3 "" H 4250 4650 50  0001 C CNN
	1    2900 4750
	1    0    0    -1  
$EndComp
Connection ~ 2050 4600
Wire Wire Line
	2600 5050 2600 5150
Wire Wire Line
	2700 5050 2700 5150
Wire Wire Line
	2700 5150 2600 5150
Connection ~ 2600 5150
$Comp
L power:GNDD #PWR0114
U 1 1 5E823440
P 2600 5250
F 0 "#PWR0114" H 2600 5000 50  0001 C CNN
F 1 "GNDD" H 2604 5095 50  0000 C CNN
F 2 "" H 2600 5250 50  0001 C CNN
F 3 "" H 2600 5250 50  0001 C CNN
	1    2600 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2600 5250 2600 5150
Wire Wire Line
	3100 5050 3100 5150
Wire Wire Line
	3100 5150 3200 5150
$Comp
L power:GNDD #PWR0115
U 1 1 5E830A6D
P 3200 5250
F 0 "#PWR0115" H 3200 5000 50  0001 C CNN
F 1 "GNDD" H 3204 5095 50  0000 C CNN
F 2 "" H 3200 5250 50  0001 C CNN
F 3 "" H 3200 5250 50  0001 C CNN
	1    3200 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 5250 3200 5150
Connection ~ 3200 5150
Wire Wire Line
	3200 5150 3200 5050
Wire Wire Line
	3200 5150 3500 5150
Connection ~ 3500 5150
Wire Wire Line
	3400 4600 3500 4600
Wire Wire Line
	3400 4500 3500 4500
Wire Wire Line
	3500 4500 3500 4600
Connection ~ 3500 4600
Wire Wire Line
	1300 5150 2050 5150
Wire Wire Line
	1300 4600 1300 5150
Wire Wire Line
	2050 4600 2300 4600
Wire Wire Line
	2050 4900 2050 5150
Connection ~ 2050 5150
Wire Wire Line
	2050 5150 2600 5150
Wire Wire Line
	3500 4600 3900 4600
Wire Wire Line
	4200 4500 4200 4600
Connection ~ 4200 4600
Wire Wire Line
	3500 4600 3500 4850
Wire Wire Line
	4200 4600 4200 4800
Wire Wire Line
	3900 4600 3900 4850
Connection ~ 3900 4600
Wire Wire Line
	3900 4600 4200 4600
Wire Wire Line
	2400 4500 2300 4500
Wire Wire Line
	2300 4500 2300 4600
Connection ~ 2300 4600
Wire Wire Line
	2300 4600 2400 4600
Connection ~ 6750 2250
Wire Wire Line
	6750 2250 7150 2250
$Comp
L Connector_Generic:Conn_01x05 J11
U 1 1 5EB1D822
P 10800 2450
F 0 "J11" H 10800 2750 50  0000 C CNN
F 1 "TUNER" V 10900 2450 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B5B-EH-A_1x05_P2.50mm_Vertical" H 10800 2450 50  0001 C CNN
F 3 "~" H 10800 2450 50  0001 C CNN
	1    10800 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 2250 10600 2250
Text Label 10100 2550 0    50   ~ 0
TUN_L
Text Label 10100 2350 0    50   ~ 0
TUN_R
Wire Wire Line
	10600 2650 10100 2650
Text Label 10100 2650 0    50   ~ 0
RDS
$Comp
L power:GNDA #PWR0102
U 1 1 5EB3F2E4
P 10500 2750
F 0 "#PWR0102" H 10500 2500 50  0001 C CNN
F 1 "GNDA" H 10505 2577 50  0000 C CNN
F 2 "" H 10500 2750 50  0001 C CNN
F 3 "" H 10500 2750 50  0001 C CNN
	1    10500 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	10500 2750 10500 2450
Wire Wire Line
	10500 2450 10600 2450
$Comp
L Connector_Generic:Conn_01x06 J9
U 1 1 5EB4B8B0
P 9550 4750
F 0 "J9" H 9550 5050 50  0000 C CNN
F 1 "SPI" V 9650 4700 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B6B-EH-A_1x06_P2.50mm_Vertical" H 9550 4750 50  0001 C CNN
F 3 "~" H 9550 4750 50  0001 C CNN
	1    9550 4750
	-1   0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0113
U 1 1 5EB4D563
P 9850 5150
F 0 "#PWR0113" H 9850 4900 50  0001 C CNN
F 1 "GNDD" H 9854 4995 50  0000 C CNN
F 2 "" H 9850 5150 50  0001 C CNN
F 3 "" H 9850 5150 50  0001 C CNN
	1    9850 5150
	1    0    0    -1  
$EndComp
Wire Wire Line
	9850 5150 9850 5050
Wire Wire Line
	9850 5050 9750 5050
$Comp
L Connector_Generic:Conn_01x05 J10
U 1 1 5EB52636
P 10800 5050
F 0 "J10" H 10800 5350 50  0000 C CNN
F 1 "PLL" V 10900 5050 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B5B-EH-A_1x05_P2.50mm_Vertical" H 10800 5050 50  0001 C CNN
F 3 "~" H 10800 5050 50  0001 C CNN
	1    10800 5050
	1    0    0    -1  
$EndComp
$Comp
L power:GNDA #PWR0116
U 1 1 5EB548DB
P 10500 5300
F 0 "#PWR0116" H 10500 5050 50  0001 C CNN
F 1 "GNDA" H 10505 5127 50  0000 C CNN
F 2 "" H 10500 5300 50  0001 C CNN
F 3 "" H 10500 5300 50  0001 C CNN
	1    10500 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	10600 5250 10500 5250
Wire Wire Line
	10500 5250 10500 5300
Text Label 10350 5150 0    50   ~ 0
CE
Text Label 10350 5050 0    50   ~ 0
DI
Text Label 10350 4950 0    50   ~ 0
CL
Text Label 10350 4850 0    50   ~ 0
DO
Wire Wire Line
	9750 4850 10600 4850
Wire Wire Line
	9750 4950 10600 4950
Wire Wire Line
	9750 4750 10150 4750
Wire Wire Line
	10150 4750 10150 5050
Wire Wire Line
	10150 5050 10600 5050
Wire Wire Line
	9750 4650 10050 4650
Wire Wire Line
	10050 4650 10050 5150
Wire Wire Line
	10050 5150 10600 5150
$Comp
L h-ms1110:PT2579 U3
U 1 1 5FAD7099
P 7050 4700
F 0 "U3" H 6800 5200 50  0000 C CNN
F 1 "PT2579" H 7250 5200 50  0000 C CNN
F 2 "" H 7050 4300 50  0001 C CNN
F 3 "http://sound.westhost.com/pt2399.pdf" H 7050 4300 50  0001 C CNN
	1    7050 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	6600 4650 6300 4650
Text Label 6300 4650 0    50   ~ 0
RDS
Wire Wire Line
	6600 4750 6550 4750
Wire Wire Line
	6550 4750 6550 4150
$Comp
L power:+5V #PWR04
U 1 1 5FAF04F4
P 6550 4150
F 0 "#PWR04" H 6550 4000 50  0001 C CNN
F 1 "+5V" H 6565 4323 50  0000 C CNN
F 2 "" H 6550 4150 50  0001 C CNN
F 3 "" H 6550 4150 50  0001 C CNN
	1    6550 4150
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR06
U 1 1 5FAF0FF2
P 7550 4150
F 0 "#PWR06" H 7550 4000 50  0001 C CNN
F 1 "+5V" H 7565 4323 50  0000 C CNN
F 2 "" H 7550 4150 50  0001 C CNN
F 3 "" H 7550 4150 50  0001 C CNN
	1    7550 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	7500 4750 7550 4750
Wire Wire Line
	7550 4750 7550 4150
$Comp
L power:GNDD #PWR05
U 1 1 5FAF74C1
P 6550 5250
F 0 "#PWR05" H 6550 5000 50  0001 C CNN
F 1 "GNDD" H 6554 5095 50  0000 C CNN
F 2 "" H 6550 5250 50  0001 C CNN
F 3 "" H 6550 5250 50  0001 C CNN
	1    6550 5250
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR07
U 1 1 5FAF8249
P 7550 5250
F 0 "#PWR07" H 7550 5000 50  0001 C CNN
F 1 "GNDD" H 7554 5095 50  0000 C CNN
F 2 "" H 7550 5250 50  0001 C CNN
F 3 "" H 7550 5250 50  0001 C CNN
	1    7550 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	7550 4850 7500 4850
Wire Wire Line
	7550 4850 7550 4950
Wire Wire Line
	6600 4850 6550 4850
Wire Wire Line
	6550 4850 6550 5250
$Comp
L Device:C_Small C9
U 1 1 5FB11A02
P 6400 5050
F 0 "C9" V 6250 5000 50  0000 L CNN
F 1 "560" V 6500 4975 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W3.5mm_P5.00mm" H 6400 5050 50  0001 C CNN
F 3 "~" H 6400 5050 50  0001 C CNN
	1    6400 5050
	0    1    1    0   
$EndComp
Wire Wire Line
	6600 4950 6250 4950
Wire Wire Line
	6250 4950 6250 5050
Wire Wire Line
	6250 5050 6300 5050
Wire Wire Line
	6600 5050 6500 5050
$Comp
L Device:CP_Small C4
U 1 1 5FB5FD58
P 6050 4800
F 0 "C4" H 6138 4846 50  0000 L CNN
F 1 "2.2" H 6138 4755 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 6050 4800 50  0001 C CNN
F 3 "~" H 6050 4800 50  0001 C CNN
	1    6050 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6050 4550 6050 4700
Wire Wire Line
	6600 4550 6050 4550
Wire Wire Line
	6050 4950 6050 4900
$Comp
L power:GNDD #PWR03
U 1 1 5FB52186
P 6050 4950
F 0 "#PWR03" H 6050 4700 50  0001 C CNN
F 1 "GNDD" H 6054 4795 50  0000 C CNN
F 2 "" H 6050 4950 50  0001 C CNN
F 3 "" H 6050 4950 50  0001 C CNN
	1    6050 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7500 4950 7550 4950
Connection ~ 7550 4950
Wire Wire Line
	7550 4950 7550 5050
Wire Wire Line
	7500 5050 7550 5050
Connection ~ 7550 5050
Wire Wire Line
	7550 5050 7550 5250
$Comp
L Device:CP_Small C11
U 1 1 5FB7C85B
P 8100 5000
F 0 "C11" H 8188 5046 50  0000 L CNN
F 1 "10.0" H 8188 4955 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 8100 5000 50  0001 C CNN
F 3 "~" H 8100 5000 50  0001 C CNN
	1    8100 5000
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C10
U 1 1 5FB7D17E
P 7800 5000
F 0 "C10" H 7892 5046 50  0000 L CNN
F 1 "0.1" H 7892 4955 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W3.5mm_P5.00mm" H 7800 5000 50  0001 C CNN
F 3 "~" H 7800 5000 50  0001 C CNN
	1    7800 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	8100 4750 8100 4900
Connection ~ 7550 4750
Wire Wire Line
	7800 4750 7800 4900
$Comp
L power:GNDD #PWR010
U 1 1 5FB8CE62
P 8100 5250
F 0 "#PWR010" H 8100 5000 50  0001 C CNN
F 1 "GNDD" H 8104 5095 50  0000 C CNN
F 2 "" H 8100 5250 50  0001 C CNN
F 3 "" H 8100 5250 50  0001 C CNN
	1    8100 5250
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR09
U 1 1 5FB8D33C
P 7800 5250
F 0 "#PWR09" H 7800 5000 50  0001 C CNN
F 1 "GNDD" H 7804 5095 50  0000 C CNN
F 2 "" H 7800 5250 50  0001 C CNN
F 3 "" H 7800 5250 50  0001 C CNN
	1    7800 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	8100 5100 8100 5250
Wire Wire Line
	7800 5250 7800 5100
Wire Wire Line
	7550 4750 7800 4750
Connection ~ 7800 4750
Wire Wire Line
	7800 4750 8100 4750
$Comp
L Device:C_Small C12
U 1 1 5FBBA519
P 8450 5150
F 0 "C12" H 8542 5196 50  0000 L CNN
F 1 "47" H 8542 5105 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W3.5mm_P5.00mm" H 8450 5150 50  0001 C CNN
F 3 "~" H 8450 5150 50  0001 C CNN
	1    8450 5150
	1    0    0    -1  
$EndComp
$Comp
L Device:Crystal_Small Y1
U 1 1 5FBBB654
P 8650 4650
F 0 "Y1" H 8650 4875 50  0000 C CNN
F 1 "4.332M" H 8650 4784 50  0000 C CNN
F 2 "" H 8650 4650 50  0001 C CNN
F 3 "~" H 8650 4650 50  0001 C CNN
	1    8650 4650
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C13
U 1 1 5FBCAE57
P 8850 5150
F 0 "C13" H 8942 5196 50  0000 L CNN
F 1 "82" H 8942 5105 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W3.5mm_P5.00mm" H 8850 5150 50  0001 C CNN
F 3 "~" H 8850 5150 50  0001 C CNN
	1    8850 5150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R11
U 1 1 5FBCB18B
P 8650 5000
F 0 "R11" V 8443 5000 50  0000 C CNN
F 1 "100" V 8534 5000 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 8580 5000 50  0001 C CNN
F 3 "~" H 8650 5000 50  0001 C CNN
	1    8650 5000
	0    1    1    0   
$EndComp
Wire Wire Line
	8500 5000 8450 5000
Wire Wire Line
	8450 5000 8450 5050
Wire Wire Line
	8800 5000 8850 5000
Wire Wire Line
	8850 5000 8850 5050
$Comp
L power:GNDD #PWR012
U 1 1 5FBDB16F
P 8450 5300
F 0 "#PWR012" H 8450 5050 50  0001 C CNN
F 1 "GNDD" H 8454 5145 50  0000 C CNN
F 2 "" H 8450 5300 50  0001 C CNN
F 3 "" H 8450 5300 50  0001 C CNN
	1    8450 5300
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR013
U 1 1 5FBDB69F
P 8850 5300
F 0 "#PWR013" H 8850 5050 50  0001 C CNN
F 1 "GNDD" H 8854 5145 50  0000 C CNN
F 2 "" H 8850 5300 50  0001 C CNN
F 3 "" H 8850 5300 50  0001 C CNN
	1    8850 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	8850 5300 8850 5250
Wire Wire Line
	8450 5300 8450 5250
Wire Wire Line
	8550 4650 8450 4650
Wire Wire Line
	8450 4650 8450 5000
Connection ~ 8450 5000
Wire Wire Line
	8850 5000 8850 4650
Wire Wire Line
	8850 4650 8750 4650
Connection ~ 8850 5000
Wire Wire Line
	8450 4650 7500 4650
Connection ~ 8450 4650
Wire Wire Line
	7500 4550 8850 4550
Wire Wire Line
	8850 4550 8850 4650
Connection ~ 8850 4650
NoConn ~ 7500 4450
NoConn ~ 3350 1400
NoConn ~ 2150 1400
NoConn ~ 2050 1400
Wire Wire Line
	1600 2050 3050 2050
Wire Wire Line
	1100 2050 1600 2050
Connection ~ 1600 2050
Connection ~ 1600 2650
Wire Wire Line
	1600 2650 1600 2050
$Comp
L Device:R R7
U 1 1 5FC4E3B3
P 6050 4450
F 0 "R7" V 5843 4450 50  0000 C CNN
F 1 "1k" V 5934 4450 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 5980 4450 50  0001 C CNN
F 3 "~" H 6050 4450 50  0001 C CNN
	1    6050 4450
	0    1    1    0   
$EndComp
Wire Wire Line
	5900 4450 5750 4450
$Comp
L Device:R R6
U 1 1 5FC8AE2D
P 5750 4100
F 0 "R6" H 5820 4146 50  0000 L CNN
F 1 "10k" H 5820 4055 50  0000 L CNN
F 2 "" V 5680 4100 50  0001 C CNN
F 3 "~" H 5750 4100 50  0001 C CNN
	1    5750 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 4250 5750 4450
Connection ~ 5750 4450
$Comp
L Device:R R9
U 1 1 5FCA51D4
P 7800 4350
F 0 "R9" V 7593 4350 50  0000 C CNN
F 1 "1k" V 7684 4350 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 7730 4350 50  0001 C CNN
F 3 "~" H 7800 4350 50  0001 C CNN
	1    7800 4350
	0    1    1    0   
$EndComp
Wire Wire Line
	7650 4350 7500 4350
$Comp
L Device:R R10
U 1 1 5FCAE3CC
P 8150 4100
F 0 "R10" H 8220 4146 50  0000 L CNN
F 1 "10k" H 8220 4055 50  0000 L CNN
F 2 "" V 8080 4100 50  0001 C CNN
F 3 "~" H 8150 4100 50  0001 C CNN
	1    8150 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 4250 8150 4350
Wire Wire Line
	8150 4350 7950 4350
Wire Wire Line
	8150 3950 8150 3850
Wire Wire Line
	5750 3850 5750 3950
Wire Wire Line
	6200 4450 6600 4450
Wire Wire Line
	6600 4350 6250 4350
Wire Wire Line
	6250 4350 6250 4250
$Comp
L Device:R R8
U 1 1 5FC79976
P 6250 4100
F 0 "R8" H 6320 4146 50  0000 L CNN
F 1 "1k" H 6320 4055 50  0000 L CNN
F 2 "" V 6180 4100 50  0001 C CNN
F 3 "~" H 6250 4100 50  0001 C CNN
	1    6250 4100
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR01
U 1 1 5FD0284E
P 5750 3850
F 0 "#PWR01" H 5750 3700 50  0001 C CNN
F 1 "+5V" H 5765 4023 50  0000 C CNN
F 2 "" H 5750 3850 50  0001 C CNN
F 3 "" H 5750 3850 50  0001 C CNN
	1    5750 3850
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR011
U 1 1 5FD02D2C
P 8150 3850
F 0 "#PWR011" H 8150 3700 50  0001 C CNN
F 1 "+5V" H 8165 4023 50  0000 C CNN
F 2 "" H 8150 3850 50  0001 C CNN
F 3 "" H 8150 3850 50  0001 C CNN
	1    8150 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 4350 8400 4350
Connection ~ 8150 4350
Wire Wire Line
	5450 4450 5750 4450
Text Label 5450 4450 0    50   ~ 0
RDDA
Text Label 8400 4350 2    50   ~ 0
RDCL
Text Label 6250 3700 3    50   ~ 0
QUAL
Wire Wire Line
	6250 3700 6250 3950
Text Label 1150 1750 0    50   ~ 0
QUAL
Wire Wire Line
	1600 2650 1600 3750
NoConn ~ 2950 1400
NoConn ~ 3150 1400
Wire Wire Line
	1100 1750 1450 1750
Wire Wire Line
	1100 1850 1450 1850
Wire Wire Line
	1100 1950 1450 1950
$Comp
L power:+12VA #PWR02
U 1 1 5FCC675A
P 6750 2150
F 0 "#PWR02" H 6750 2000 50  0001 C CNN
F 1 "+12VA" H 6765 2323 50  0000 C CNN
F 2 "" H 6750 2150 50  0001 C CNN
F 3 "" H 6750 2150 50  0001 C CNN
	1    6750 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	6750 2150 6750 2250
$Comp
L power:+9VA #PWR08
U 1 1 5FCD1850
P 8550 2150
F 0 "#PWR08" H 8550 2025 50  0001 C CNN
F 1 "+9VA" H 8565 2323 50  0000 C CNN
F 2 "" H 8550 2150 50  0001 C CNN
F 3 "" H 8550 2150 50  0001 C CNN
	1    8550 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 2150 8550 2250
$Comp
L Regulator_Linear:LM7809_TO220 U2
U 1 1 5FCE13D8
P 7700 2250
F 0 "U2" H 7700 2492 50  0000 C CNN
F 1 "LM7809_TO220" H 7700 2401 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 7700 2475 50  0001 C CIN
F 3 "https://www.onsemi.cn/PowerSolutions/document/MC7800-D.PDF" H 7700 2200 50  0001 C CNN
	1    7700 2250
	1    0    0    -1  
$EndComp
Wire Notes Line
	9300 4350 9300 5550
Wire Notes Line
	9300 5550 11000 5550
Wire Notes Line
	11000 5550 11000 4350
Wire Notes Line
	11000 4350 9300 4350
Text Notes 9300 4300 0    50   ~ 0
Connects front panel SPI with tuner board
Text Label 10550 3800 2    50   ~ 0
SCL
Text Label 10550 3700 2    50   ~ 0
SDA
Wire Wire Line
	5150 2250 6750 2250
Wire Wire Line
	4650 1500 10600 1500
Wire Wire Line
	10000 2350 10600 2350
Wire Wire Line
	9900 2550 10600 2550
Wire Wire Line
	5350 1700 10600 1700
NoConn ~ 1100 1650
NoConn ~ 1100 2350
Wire Wire Line
	1600 3750 1600 3850
$Comp
L power:GNDD #PWR0103
U 1 1 5E238DAE
P 1600 3850
F 0 "#PWR0103" H 1600 3600 50  0001 C CNN
F 1 "GNDD" H 1604 3695 50  0000 C CNN
F 2 "" H 1600 3850 50  0001 C CNN
F 3 "" H 1600 3850 50  0001 C CNN
	1    1600 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	9900 3300 9900 2550
Wire Wire Line
	5850 1400 5850 3300
Wire Wire Line
	10000 3200 10000 2350
Wire Wire Line
	5950 1400 5950 3200
Wire Wire Line
	5850 3300 9900 3300
Wire Wire Line
	5950 3200 10000 3200
NoConn ~ 9750 4550
$Comp
L Device:R R1
U 1 1 5FD18A69
P 1900 2450
F 0 "R1" V 1800 2450 50  0000 C CNN
F 1 "150" V 2050 2450 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 1830 2450 50  0001 C CNN
F 3 "~" H 1900 2450 50  0001 C CNN
	1    1900 2450
	0    1    1    0   
$EndComp
Wire Wire Line
	2050 2450 3550 2450
Wire Wire Line
	2350 2550 3750 2550
$Comp
L Device:R R2
U 1 1 5FD19B80
P 2200 2550
F 0 "R2" V 2050 2550 50  0000 C CNN
F 1 "150" V 2300 2550 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 2130 2550 50  0001 C CNN
F 3 "~" H 2200 2550 50  0001 C CNN
	1    2200 2550
	0    1    1    0   
$EndComp
Wire Wire Line
	3350 3650 3450 3650
Wire Wire Line
	1100 3650 3050 3650
$Comp
L Device:R R5
U 1 1 5FD1ABF3
P 3200 3650
F 0 "R5" V 3050 3650 50  0000 C CNN
F 1 "150" V 3300 3650 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 3130 3650 50  0001 C CNN
F 3 "~" H 3200 3650 50  0001 C CNN
	1    3200 3650
	0    1    1    0   
$EndComp
$Comp
L Device:R R4
U 1 1 5FD25391
P 2900 3550
F 0 "R4" V 2750 3550 50  0000 C CNN
F 1 "150" V 3050 3550 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 2830 3550 50  0001 C CNN
F 3 "~" H 2900 3550 50  0001 C CNN
	1    2900 3550
	0    1    1    0   
$EndComp
Wire Wire Line
	3050 3550 4150 3550
$Comp
L Device:R R3
U 1 1 5FD25788
P 2600 3450
F 0 "R3" V 2500 3450 50  0000 C CNN
F 1 "150" V 2750 3450 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 2530 3450 50  0001 C CNN
F 3 "~" H 2600 3450 50  0001 C CNN
	1    2600 3450
	0    1    1    0   
$EndComp
Wire Wire Line
	2750 3450 4350 3450
NoConn ~ 5650 1400
NoConn ~ 5550 1400
NoConn ~ 4950 1400
NoConn ~ 4850 1400
NoConn ~ 4750 1400
NoConn ~ 4550 1400
NoConn ~ 4250 1400
NoConn ~ 3950 1400
NoConn ~ 3250 1400
NoConn ~ 2850 1400
Text Label 9800 4650 0    50   ~ 0
CE
Text Label 9800 4750 0    50   ~ 0
DI
Text Label 9800 4850 0    50   ~ 0
DO
Text Label 9800 4950 0    50   ~ 0
CL
$EndSCHEMATC
