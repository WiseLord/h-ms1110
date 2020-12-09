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
L h-ms1110:TDA7719 U3
U 1 1 5E221CCA
P 7550 4300
F 0 "U3" H 7525 5415 50  0000 C CNN
F 1 "TDA7719" H 7525 5324 50  0000 C CNN
F 2 "Package_SO:TSSOP-28_4.4x9.7mm_P0.65mm" H 9950 4700 50  0001 C CNN
F 3 "" H 9950 4700 50  0001 C CNN
	1    7550 4300
	1    0    0    -1  
$EndComp
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
Text Label 8550 3700 0    50   ~ 0
SDA
Text Label 8550 3800 0    50   ~ 0
SCL
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
$Comp
L power:GNDA #PWR0102
U 1 1 5E222B52
P 8550 5300
F 0 "#PWR0102" H 8550 5050 50  0001 C CNN
F 1 "GNDA" H 8555 5127 50  0000 C CNN
F 2 "" H 8550 5300 50  0001 C CNN
F 3 "" H 8550 5300 50  0001 C CNN
	1    8550 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 5300 8550 5050
Wire Wire Line
	8550 5050 8450 5050
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
	2850 1400 2850 1650
Wire Wire Line
	2850 1650 1100 1650
Wire Wire Line
	1100 1750 2050 1750
Wire Wire Line
	2050 1750 2050 1400
Wire Wire Line
	1100 1850 2150 1850
Wire Wire Line
	2150 1850 2150 1400
Wire Wire Line
	1100 1950 3350 1950
Wire Wire Line
	3350 1950 3350 1400
Wire Wire Line
	3050 1400 3050 2050
Text Label 1150 1650 0    50   ~ 0
+3.3VA
Text Label 1150 1750 0    50   ~ 0
AD2
Text Label 1150 1850 0    50   ~ 0
AD1
Text Label 1150 1950 0    50   ~ 0
AD3
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
Wire Wire Line
	1100 2050 1600 2050
Connection ~ 1600 2050
Wire Wire Line
	1600 2050 3050 2050
Text Label 1150 2550 0    50   ~ 0
SYNC_SCK
Text Label 1150 2450 0    50   ~ 0
SYNC_DATA
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
Text Label 1150 2650 0    50   ~ 0
GNDD
NoConn ~ 2250 1400
NoConn ~ 2350 1400
NoConn ~ 2450 1400
NoConn ~ 2550 1400
NoConn ~ 2650 1400
NoConn ~ 2750 1400
$Comp
L Connector_Generic:Conn_01x03 J4
U 1 1 5E26FD50
P 900 3050
F 0 "J4" H 900 3250 50  0000 C CNN
F 1 "ROT" V 1000 3050 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B3B-EH-A_1x03_P2.50mm_Vertical" H 900 3050 50  0001 C CNN
F 3 "~" H 900 3050 50  0001 C CNN
	1    900  3050
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1100 2950 2950 2950
Wire Wire Line
	1100 3150 3150 3150
Text Label 1150 2950 0    50   ~ 0
ROT-B
Text Label 1150 3150 0    50   ~ 0
ROT-A
Wire Wire Line
	1100 3050 1600 3050
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
	1600 2650 1600 3050
Connection ~ 1600 3050
Wire Wire Line
	2950 2950 2950 1400
Wire Wire Line
	3150 3150 3150 1400
Wire Wire Line
	1600 3050 1600 3750
Wire Wire Line
	1100 3750 1600 3750
Connection ~ 1600 3750
Wire Wire Line
	1600 3750 1600 3850
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
Wire Wire Line
	1100 3650 3050 3650
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
Text Label 1150 3050 0    50   ~ 0
GNDD
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
Text Label 8550 4450 0    50   ~ 0
SP_L
Text Label 8550 4550 0    50   ~ 0
SP_R
Text Label 4850 2000 1    50   ~ 0
OUT_L
Text Label 4950 2000 1    50   ~ 0
OUT_R
Wire Wire Line
	5150 1400 5150 2250
Wire Wire Line
	8850 2250 8550 2250
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
	8450 3950 8850 3950
Wire Wire Line
	1100 2550 2050 2550
Wire Wire Line
	3650 1400 3650 2000
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
Wire Wire Line
	3850 1400 3850 2000
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
Text Label 6200 3500 0    50   ~ 0
TUN_L
Text Label 6200 3600 0    50   ~ 0
TUN_R
Text Label 6200 3950 0    50   ~ 0
MP3_L
Text Label 6200 4050 0    50   ~ 0
MP3_R
Text Label 6200 4550 0    50   ~ 0
AUX1_L
Text Label 6200 4650 0    50   ~ 0
AUX1_R
Text Label 6200 4750 0    50   ~ 0
AUX2_L
Text Label 6200 4850 0    50   ~ 0
AUX2_R
Text Label 4750 2000 1    50   ~ 0
QD_R
Text Label 4550 2000 1    50   ~ 0
QD_L
Text Label 4250 2000 1    50   ~ 0
QD_G
Text Label 6200 4200 0    50   ~ 0
QD_L
Text Label 6200 4300 0    50   ~ 0
QD_G
Text Label 6200 4400 0    50   ~ 0
QD_R
Text Label 6200 3700 0    50   ~ 0
DIG_L
Text Label 6200 3850 0    50   ~ 0
DIG_R
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
Wire Wire Line
	6350 5300 6350 5250
$Comp
L power:GNDA #PWR0113
U 1 1 5E84928B
P 6350 5300
F 0 "#PWR0113" H 6350 5050 50  0001 C CNN
F 1 "GNDA" H 6355 5127 50  0000 C CNN
F 2 "" H 6350 5300 50  0001 C CNN
F 3 "" H 6350 5300 50  0001 C CNN
	1    6350 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6350 5000 6350 5050
Wire Wire Line
	6600 5000 6350 5000
$Comp
L Device:CP_Small C4
U 1 1 5E84303E
P 6350 5150
F 0 "C4" H 6438 5196 50  0000 L CNN
F 1 "10.0" H 6438 5105 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D5.0mm_P2.50mm" H 6350 5150 50  0001 C CNN
F 3 "~" H 6350 5150 50  0001 C CNN
	1    6350 5150
	1    0    0    -1  
$EndComp
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
Connection ~ 6750 2250
Wire Wire Line
	6750 2250 7150 2250
Wire Wire Line
	5850 3500 6600 3500
Wire Wire Line
	5850 1400 5850 3500
Wire Wire Line
	5950 1400 5950 3600
Wire Wire Line
	5950 3600 6600 3600
Wire Wire Line
	5550 1400 5550 3950
Wire Wire Line
	5550 3950 6600 3950
Wire Wire Line
	5650 1400 5650 4050
Wire Wire Line
	5650 4050 6600 4050
Wire Wire Line
	8450 4550 9150 4550
Wire Wire Line
	4650 1500 9050 1500
Wire Wire Line
	5350 1700 9150 1700
Text Label 8550 4650 0    50   ~ 0
OUT_R
Text Label 8550 4350 0    50   ~ 0
OUT_L
Wire Wire Line
	9450 3000 9450 4350
Wire Wire Line
	4850 1400 4850 3000
Wire Wire Line
	9550 4650 9550 3100
Wire Wire Line
	9550 3100 4950 3100
Wire Wire Line
	4950 1400 4950 3100
Wire Wire Line
	8450 4450 9050 4450
Wire Wire Line
	4550 1400 4550 4200
Wire Wire Line
	4550 4200 6600 4200
Wire Wire Line
	4250 1400 4250 4300
Wire Wire Line
	4250 4300 6600 4300
Wire Wire Line
	4750 1400 4750 4400
Wire Wire Line
	4750 4400 6600 4400
$Comp
L power:GNDA #PWR02
U 1 1 5FB4A266
P 900 5750
F 0 "#PWR02" H 900 5500 50  0001 C CNN
F 1 "GNDA" H 905 5577 50  0000 C CNN
F 2 "" H 900 5750 50  0001 C CNN
F 3 "" H 900 5750 50  0001 C CNN
	1    900  5750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 3700 6600 3700
Wire Wire Line
	900  5750 900  5700
Wire Wire Line
	900  4900 900  4850
$Comp
L power:GNDA #PWR01
U 1 1 5FB3AB32
P 900 4900
F 0 "#PWR01" H 900 4650 50  0001 C CNN
F 1 "GNDA" H 905 4727 50  0000 C CNN
F 2 "" H 900 4900 50  0001 C CNN
F 3 "" H 900 4900 50  0001 C CNN
	1    900  4900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_Coaxial_x2 J10
U 1 1 5FB375D7
P 900 5400
F 0 "J10" H 1050 5500 50  0000 C CNN
F 1 "AUX2" H 1100 5400 50  0000 C CNN
F 2 "" H 900 5300 50  0001 C CNN
F 3 " ~" H 900 5300 50  0001 C CNN
	1    900  5400
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_Coaxial_x2 J9
U 1 1 5FB34EF1
P 900 4550
F 0 "J9" H 1050 4650 50  0000 C CNN
F 1 "AUX1" H 1100 4550 50  0000 C CNN
F 2 "" H 900 4450 50  0001 C CNN
F 3 " ~" H 900 4450 50  0001 C CNN
	1    900  4550
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1100 4650 6600 4650
Wire Wire Line
	1200 4550 1200 4450
Wire Wire Line
	1200 4450 1100 4450
Wire Wire Line
	1200 4550 6600 4550
Wire Wire Line
	1300 4850 1300 5500
Wire Wire Line
	1300 5500 1100 5500
Wire Wire Line
	1300 4850 6600 4850
Wire Wire Line
	1200 4750 6600 4750
Wire Wire Line
	1200 5300 1100 5300
Wire Wire Line
	1200 4750 1200 5300
Wire Wire Line
	5300 3850 6600 3850
$Comp
L power:+12VA #PWR07
U 1 1 5FCC54BA
P 6750 2150
F 0 "#PWR07" H 6750 2000 50  0001 C CNN
F 1 "+12VA" H 6765 2323 50  0000 C CNN
F 2 "" H 6750 2150 50  0001 C CNN
F 3 "" H 6750 2150 50  0001 C CNN
	1    6750 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	6750 2150 6750 2250
Wire Wire Line
	5150 2250 6750 2250
$Comp
L power:+9VA #PWR08
U 1 1 5FCD6BDB
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
Wire Wire Line
	4850 3000 9450 3000
$Comp
L Regulator_Linear:LM7809_TO220 U4
U 1 1 5FCE101D
P 7700 2250
F 0 "U4" H 7700 2492 50  0000 C CNN
F 1 "LM7809_TO220" H 7700 2401 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 7700 2475 50  0001 C CIN
F 3 "https://www.onsemi.cn/PowerSolutions/document/MC7800-D.PDF" H 7700 2200 50  0001 C CNN
	1    7700 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	8850 2250 8850 3950
$Comp
L Connector_Generic:Conn_01x04 J11
U 1 1 5FD578F8
P 5500 7150
F 0 "J11" H 5500 7350 50  0000 C CNN
F 1 "RPI_POWER" V 5600 7100 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B4B-EH-A_1x04_P2.50mm_Vertical" H 5500 7150 50  0001 C CNN
F 3 "~" H 5500 7150 50  0001 C CNN
	1    5500 7150
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR05
U 1 1 5FD57D98
P 5200 6750
F 0 "#PWR05" H 5200 6600 50  0001 C CNN
F 1 "+5V" H 5215 6923 50  0000 C CNN
F 2 "" H 5200 6750 50  0001 C CNN
F 3 "" H 5200 6750 50  0001 C CNN
	1    5200 6750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 7050 5200 7050
Wire Wire Line
	5300 7150 5200 7150
Wire Wire Line
	5200 7150 5200 7050
Connection ~ 5200 7050
$Comp
L power:GNDD #PWR06
U 1 1 5FD57DA6
P 5200 7500
F 0 "#PWR06" H 5200 7250 50  0001 C CNN
F 1 "GNDD" H 5204 7345 50  0000 C CNN
F 2 "" H 5200 7500 50  0001 C CNN
F 3 "" H 5200 7500 50  0001 C CNN
	1    5200 7500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 7350 5200 7350
Wire Wire Line
	5200 7350 5200 7400
Wire Wire Line
	5300 7250 5200 7250
Wire Wire Line
	5200 7250 5200 7350
Connection ~ 5200 7350
$Comp
L Diode_Bridge:GBU8K D2
U 1 1 5FD57DB5
P 2650 6850
F 0 "D2" H 2800 7100 50  0000 L CNN
F 1 "GBU8K" H 2800 7000 50  0000 L CNN
F 2 "Diode_THT:Diode_Bridge_Vishay_GBU" H 2800 6975 50  0001 L CNN
F 3 "http://www.vishay.com/docs/88656/gbu8a.pdf" H 2650 6850 50  0001 C CNN
	1    2650 6850
	1    0    0    -1  
$EndComp
$Comp
L Device:CP_Small C11
U 1 1 5FD57DBF
P 3050 7050
F 0 "C11" H 3138 7096 50  0000 L CNN
F 1 "1000.0" H 3138 7005 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm_P7.50mm" H 3050 7050 50  0001 C CNN
F 3 "~" H 3050 7050 50  0001 C CNN
	1    3050 7050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 6950 3050 6850
Wire Wire Line
	3050 6850 2950 6850
Wire Wire Line
	2300 6850 2350 6850
Connection ~ 5200 7400
Wire Wire Line
	5200 7400 5200 7500
$Comp
L Device:CP_Small C12
U 1 1 5FD57DCE
P 4500 7200
F 0 "C12" H 4588 7246 50  0000 L CNN
F 1 "470.0" H 4588 7155 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 4500 7200 50  0001 C CNN
F 3 "~" H 4500 7200 50  0001 C CNN
	1    4500 7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 7300 4500 7400
Wire Wire Line
	4500 7400 4900 7400
Wire Wire Line
	2650 6550 2650 6500
Wire Wire Line
	2650 6500 1900 6500
Wire Wire Line
	2650 7150 2650 7200
Wire Wire Line
	2650 7200 1900 7200
Text Label 1900 6500 0    50   ~ 0
RPI_AC1
Text Label 1900 7200 0    50   ~ 0
RPI_AC2
$Comp
L Device:C_Small C13
U 1 1 5FD57DE0
P 4900 7200
F 0 "C13" H 4992 7246 50  0000 L CNN
F 1 "0.1" H 4992 7155 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W3.5mm_P5.00mm" H 4900 7200 50  0001 C CNN
F 3 "~" H 4900 7200 50  0001 C CNN
	1    4900 7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 7300 4900 7400
Connection ~ 4900 7400
Wire Wire Line
	4900 7400 5200 7400
$Comp
L h-ms1110:MP1584EN_module U2
U 1 1 5FD57DED
P 3900 7000
F 0 "U2" H 3900 7515 50  0000 C CNN
F 1 "MP1584EN_module" H 3900 7424 50  0000 C CNN
F 2 "h-ms1110:MP1584EN_module" H 4200 7400 50  0001 C CNN
F 3 "" H 5250 6900 50  0001 C CNN
	1    3900 7000
	1    0    0    -1  
$EndComp
Connection ~ 3050 6850
Wire Wire Line
	3600 7300 3600 7400
Wire Wire Line
	3700 7300 3700 7400
Wire Wire Line
	3700 7400 3600 7400
Connection ~ 3600 7400
$Comp
L power:GNDD #PWR03
U 1 1 5FD57DFC
P 3600 7500
F 0 "#PWR03" H 3600 7250 50  0001 C CNN
F 1 "GNDD" H 3604 7345 50  0000 C CNN
F 2 "" H 3600 7500 50  0001 C CNN
F 3 "" H 3600 7500 50  0001 C CNN
	1    3600 7500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 7500 3600 7400
Wire Wire Line
	4100 7300 4100 7400
Wire Wire Line
	4100 7400 4200 7400
$Comp
L power:GNDD #PWR04
U 1 1 5FD57E09
P 4200 7500
F 0 "#PWR04" H 4200 7250 50  0001 C CNN
F 1 "GNDD" H 4204 7345 50  0000 C CNN
F 2 "" H 4200 7500 50  0001 C CNN
F 3 "" H 4200 7500 50  0001 C CNN
	1    4200 7500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 7500 4200 7400
Connection ~ 4200 7400
Wire Wire Line
	4200 7400 4200 7300
Wire Wire Line
	4200 7400 4500 7400
Connection ~ 4500 7400
Wire Wire Line
	4400 6850 4500 6850
Wire Wire Line
	4400 6750 4500 6750
Wire Wire Line
	4500 6750 4500 6850
Connection ~ 4500 6850
Wire Wire Line
	2300 7400 3050 7400
Wire Wire Line
	2300 6850 2300 7400
Wire Wire Line
	3050 6850 3300 6850
Wire Wire Line
	3050 7150 3050 7400
Connection ~ 3050 7400
Wire Wire Line
	3050 7400 3600 7400
Wire Wire Line
	4500 6850 4900 6850
Wire Wire Line
	5200 6750 5200 6850
Connection ~ 5200 6850
Wire Wire Line
	4500 6850 4500 7100
Wire Wire Line
	5200 6850 5200 7050
Wire Wire Line
	4900 6850 4900 7100
Connection ~ 4900 6850
Wire Wire Line
	4900 6850 5200 6850
Wire Wire Line
	3400 6750 3300 6750
Wire Wire Line
	3300 6750 3300 6850
Connection ~ 3300 6850
Wire Wire Line
	3300 6850 3400 6850
Wire Wire Line
	4050 1400 4050 2000
Wire Wire Line
	5050 1400 5050 2000
Text Label 4050 2000 1    50   ~ 0
RPI_AC1
Text Label 5050 2000 1    50   ~ 0
RPI_AC2
NoConn ~ 8450 4200
NoConn ~ 8450 4100
Wire Wire Line
	9050 1500 10600 1500
Connection ~ 9050 1500
Connection ~ 9150 1700
Wire Wire Line
	8450 3700 10600 3700
Wire Wire Line
	8450 3800 10600 3800
NoConn ~ 8450 3450
NoConn ~ 8450 3550
Wire Wire Line
	9150 1700 10600 1700
$Comp
L Connector_Generic:Conn_01x03 J12
U 1 1 5FD1B96C
P 10800 4900
F 0 "J12" H 10800 5100 50  0000 C CNN
F 1 "SUB" V 10900 4900 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B3B-EH-A_1x03_P2.50mm_Vertical" H 10800 4900 50  0001 C CNN
F 3 "~" H 10800 4900 50  0001 C CNN
	1    10800 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 4450 9050 1500
Wire Wire Line
	9150 4550 9150 1700
Wire Wire Line
	8450 4800 10600 4800
Wire Wire Line
	10600 5000 8850 5000
Wire Wire Line
	8850 5000 8850 4900
Wire Wire Line
	8850 4900 8450 4900
$Comp
L power:GNDA #PWR09
U 1 1 5FD5CBC9
P 10500 5300
F 0 "#PWR09" H 10500 5050 50  0001 C CNN
F 1 "GNDA" H 10505 5127 50  0000 C CNN
F 2 "" H 10500 5300 50  0001 C CNN
F 3 "" H 10500 5300 50  0001 C CNN
	1    10500 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	10600 4900 10500 4900
Wire Wire Line
	10500 4900 10500 5300
Wire Wire Line
	8450 4350 9450 4350
Wire Wire Line
	8450 4650 9550 4650
NoConn ~ 1100 2350
$Comp
L Device:R R3
U 1 1 5FD91278
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
$Comp
L Device:R R4
U 1 1 5FD91CDF
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
L Device:R R5
U 1 1 5FD9216F
P 3200 3650
F 0 "R5" V 3050 3650 50  0000 C CNN
F 1 "150" V 3300 3650 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 3130 3650 50  0001 C CNN
F 3 "~" H 3200 3650 50  0001 C CNN
	1    3200 3650
	0    1    1    0   
$EndComp
Wire Wire Line
	3350 3650 3450 3650
$Comp
L Device:R R1
U 1 1 5FDBAB59
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
$Comp
L Device:R R2
U 1 1 5FDBB4DB
P 2200 2550
F 0 "R2" V 2050 2550 50  0000 C CNN
F 1 "150" V 2300 2550 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 2130 2550 50  0001 C CNN
F 3 "~" H 2200 2550 50  0001 C CNN
	1    2200 2550
	0    1    1    0   
$EndComp
Wire Wire Line
	2350 2550 3750 2550
$EndSCHEMATC
