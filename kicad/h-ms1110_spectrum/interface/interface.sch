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
Text Label 10550 3700 2    50   ~ 0
SDA
Text Label 10550 3800 2    50   ~ 0
SCL
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
NoConn ~ 11150 4500
Wire Wire Line
	2850 1400 2850 1650
Wire Wire Line
	3050 1400 3050 2050
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
	1100 2450 3550 2450
Wire Wire Line
	1600 2650 1100 2650
Wire Wire Line
	1600 2050 3050 2050
Text Label 1150 2550 0    50   ~ 0
SYNC_SCK
Text Label 1150 2450 0    50   ~ 0
SYNC_DATA
$Comp
L power:GNDD #PWR0103
U 1 1 5E238DAE
P 1600 4100
F 0 "#PWR0103" H 1600 3850 50  0001 C CNN
F 1 "GNDD" H 1604 3945 50  0000 C CNN
F 2 "" H 1600 4100 50  0001 C CNN
F 3 "" H 1600 4100 50  0001 C CNN
	1    1600 4100
	1    0    0    -1  
$EndComp
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
Connection ~ 1600 2650
Wire Wire Line
	1600 2650 1600 2050
Wire Wire Line
	3550 2450 3550 1400
Wire Wire Line
	3750 2550 3750 1400
Wire Wire Line
	1100 3750 1600 3750
Connection ~ 1600 3750
Wire Wire Line
	1600 3750 1600 4100
Text Label 1150 3450 0    50   ~ 0
MUTE
Text Label 1150 3550 0    50   ~ 0
STBY
Text Label 1150 3650 0    50   ~ 0
REMOTE
Wire Wire Line
	4350 1400 4350 3450
Wire Wire Line
	1100 3450 4350 3450
Wire Wire Line
	4150 1400 4150 3550
Wire Wire Line
	1100 3550 4150 3550
Wire Wire Line
	3450 1400 3450 3650
Wire Wire Line
	1100 3650 3450 3650
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
	10300 1600 10300 1800
Text Label 10550 1600 2    50   ~ 0
GNDD
Text Label 10550 1500 2    50   ~ 0
SP_L
Text Label 10550 1700 2    50   ~ 0
SP_R
Text Label 4850 2000 1    50   ~ 0
OUT_L
Text Label 4950 2000 1    50   ~ 0
OUT_R
Wire Wire Line
	1100 2550 3750 2550
$Comp
L Connector_Generic:Conn_01x04 J6
U 1 1 5E2A2176
P 5550 5800
F 0 "J6" H 5550 6000 50  0000 C CNN
F 1 "FP_POWER" V 5650 5750 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B4B-EH-A_1x04_P2.50mm_Vertical" H 5550 5800 50  0001 C CNN
F 3 "~" H 5550 5800 50  0001 C CNN
	1    5550 5800
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0106
U 1 1 5E2A286B
P 5250 5400
F 0 "#PWR0106" H 5250 5250 50  0001 C CNN
F 1 "+5V" H 5265 5573 50  0000 C CNN
F 2 "" H 5250 5400 50  0001 C CNN
F 3 "" H 5250 5400 50  0001 C CNN
	1    5250 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 5700 5250 5700
Wire Wire Line
	5350 5800 5250 5800
Wire Wire Line
	5250 5800 5250 5700
Connection ~ 5250 5700
$Comp
L power:GNDD #PWR0107
U 1 1 5E2A577C
P 5250 6150
F 0 "#PWR0107" H 5250 5900 50  0001 C CNN
F 1 "GNDD" H 5254 5995 50  0000 C CNN
F 2 "" H 5250 6150 50  0001 C CNN
F 3 "" H 5250 6150 50  0001 C CNN
	1    5250 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 6000 5250 6000
Wire Wire Line
	5250 6000 5250 6050
Wire Wire Line
	5350 5900 5250 5900
Wire Wire Line
	5250 5900 5250 6000
Connection ~ 5250 6000
$Comp
L Diode_Bridge:GBU8K D1
U 1 1 5E2BD344
P 2700 5500
F 0 "D1" H 2850 5750 50  0000 L CNN
F 1 "GBU8K" H 2850 5650 50  0000 L CNN
F 2 "Diode_THT:Diode_Bridge_Vishay_GBU" H 2850 5625 50  0001 L CNN
F 3 "http://www.vishay.com/docs/88656/gbu8a.pdf" H 2700 5500 50  0001 C CNN
	1    2700 5500
	1    0    0    -1  
$EndComp
$Comp
L Device:CP_Small C1
U 1 1 5E2C8E45
P 3100 5700
F 0 "C1" H 3188 5746 50  0000 L CNN
F 1 "1000.0" H 3188 5655 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm_P7.50mm" H 3100 5700 50  0001 C CNN
F 3 "~" H 3100 5700 50  0001 C CNN
	1    3100 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 5600 3100 5500
Wire Wire Line
	3100 5500 3000 5500
Wire Wire Line
	2350 5500 2400 5500
Connection ~ 5250 6050
Wire Wire Line
	5250 6050 5250 6150
$Comp
L Device:CP_Small C2
U 1 1 5E2DC279
P 4550 5850
F 0 "C2" H 4638 5896 50  0000 L CNN
F 1 "470.0" H 4638 5805 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 4550 5850 50  0001 C CNN
F 3 "~" H 4550 5850 50  0001 C CNN
	1    4550 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 5950 4550 6050
Wire Wire Line
	4550 6050 4950 6050
Wire Wire Line
	2700 5200 2700 5150
Wire Wire Line
	2700 5150 1900 5150
Wire Wire Line
	2700 5800 2700 5850
Wire Wire Line
	2700 5850 1900 5850
Text Label 3650 2000 1    50   ~ 0
AC1
Text Label 3850 2000 1    50   ~ 0
AC2
Text Label 1900 5150 0    50   ~ 0
AC1
Text Label 1900 5850 0    50   ~ 0
AC2
$Comp
L Device:C_Small C3
U 1 1 5E2FABE3
P 4950 5850
F 0 "C3" H 5042 5896 50  0000 L CNN
F 1 "0.1" H 5042 5805 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W3.5mm_P5.00mm" H 4950 5850 50  0001 C CNN
F 3 "~" H 4950 5850 50  0001 C CNN
	1    4950 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	4950 5950 4950 6050
Connection ~ 4950 6050
Wire Wire Line
	4950 6050 5250 6050
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
Text Label 5550 2000 1    50   ~ 0
MP3_L
Text Label 5650 2000 1    50   ~ 0
MP3_R
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
Text Label 4750 2000 1    50   ~ 0
QD_R
Text Label 4550 2000 1    50   ~ 0
QD_L
Text Label 4250 2000 1    50   ~ 0
QD_G
$Comp
L h-ms1110:MP1584EN_module U1
U 1 1 5E7F5A1F
P 3950 5650
F 0 "U1" H 3950 6165 50  0000 C CNN
F 1 "MP1584EN_module" H 3950 6074 50  0000 C CNN
F 2 "h-ms1110:MP1584EN_module" H 4250 6050 50  0001 C CNN
F 3 "" H 5300 5550 50  0001 C CNN
	1    3950 5650
	1    0    0    -1  
$EndComp
Connection ~ 3100 5500
Wire Wire Line
	3650 5950 3650 6050
Wire Wire Line
	3750 5950 3750 6050
Wire Wire Line
	3750 6050 3650 6050
Connection ~ 3650 6050
$Comp
L power:GNDD #PWR0114
U 1 1 5E823440
P 3650 6150
F 0 "#PWR0114" H 3650 5900 50  0001 C CNN
F 1 "GNDD" H 3654 5995 50  0000 C CNN
F 2 "" H 3650 6150 50  0001 C CNN
F 3 "" H 3650 6150 50  0001 C CNN
	1    3650 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 6150 3650 6050
Wire Wire Line
	4150 5950 4150 6050
Wire Wire Line
	4150 6050 4250 6050
$Comp
L power:GNDD #PWR0115
U 1 1 5E830A6D
P 4250 6150
F 0 "#PWR0115" H 4250 5900 50  0001 C CNN
F 1 "GNDD" H 4254 5995 50  0000 C CNN
F 2 "" H 4250 6150 50  0001 C CNN
F 3 "" H 4250 6150 50  0001 C CNN
	1    4250 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 6150 4250 6050
Connection ~ 4250 6050
Wire Wire Line
	4250 6050 4250 5950
Wire Wire Line
	4250 6050 4550 6050
Connection ~ 4550 6050
Wire Wire Line
	4450 5500 4550 5500
Wire Wire Line
	4450 5400 4550 5400
Wire Wire Line
	4550 5400 4550 5500
Connection ~ 4550 5500
Wire Wire Line
	2350 6050 3100 6050
Wire Wire Line
	2350 5500 2350 6050
Wire Wire Line
	3100 5500 3350 5500
Wire Wire Line
	3100 5800 3100 6050
Connection ~ 3100 6050
Wire Wire Line
	3100 6050 3650 6050
Wire Wire Line
	4550 5500 4950 5500
Wire Wire Line
	5250 5400 5250 5500
Connection ~ 5250 5500
Wire Wire Line
	4550 5500 4550 5750
Wire Wire Line
	5250 5500 5250 5700
Wire Wire Line
	4950 5500 4950 5750
Connection ~ 4950 5500
Wire Wire Line
	4950 5500 5250 5500
Wire Wire Line
	3450 5400 3350 5400
Wire Wire Line
	3350 5400 3350 5500
Connection ~ 3350 5500
Wire Wire Line
	3350 5500 3450 5500
Wire Wire Line
	10600 1600 10300 1600
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
	4650 1500 10600 1500
Wire Wire Line
	5350 1700 10600 1700
Wire Wire Line
	1600 2650 1600 3750
NoConn ~ 2950 1400
NoConn ~ 3150 1400
NoConn ~ 2050 1400
NoConn ~ 2150 1400
NoConn ~ 3350 1400
Wire Wire Line
	5150 1400 5150 2300
$Comp
L power:+12VA #PWR?
U 1 1 5FCC8866
P 5150 2300
F 0 "#PWR?" H 5150 2150 50  0001 C CNN
F 1 "+12VA" H 5165 2473 50  0000 C CNN
F 2 "" H 5150 2300 50  0001 C CNN
F 3 "" H 5150 2300 50  0001 C CNN
	1    5150 2300
	-1   0    0    1   
$EndComp
NoConn ~ 5950 2100
NoConn ~ 5850 2100
NoConn ~ 5650 2100
NoConn ~ 5550 2100
NoConn ~ 4950 2100
NoConn ~ 4850 2100
NoConn ~ 4750 2100
NoConn ~ 4550 2100
NoConn ~ 4250 2100
NoConn ~ 3850 2100
NoConn ~ 3650 2100
Wire Wire Line
	3650 1400 3650 2100
Wire Wire Line
	3850 1400 3850 2100
Wire Wire Line
	4250 1400 4250 2100
Wire Wire Line
	4550 1400 4550 2100
Wire Wire Line
	4750 1400 4750 2100
Wire Wire Line
	4850 1400 4850 2100
Wire Wire Line
	4950 1400 4950 2100
Wire Wire Line
	5550 1400 5550 2100
Wire Wire Line
	5650 1400 5650 2100
Wire Wire Line
	5850 1400 5850 2100
Wire Wire Line
	5950 1400 5950 2100
NoConn ~ 2850 1650
NoConn ~ 1100 2350
$EndSCHEMATC
