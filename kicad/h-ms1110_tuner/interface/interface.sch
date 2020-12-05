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
	2850 1400 2850 1650
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
	1100 2450 3550 2450
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
	1100 3450 4350 3450
Wire Wire Line
	4150 1400 4150 3550
Wire Wire Line
	1100 3550 4150 3550
Wire Wire Line
	3450 1400 3450 3650
Wire Wire Line
	1100 3650 3450 3650
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
Text Label 4850 2000 1    50   ~ 0
OUT_L
Text Label 4950 2000 1    50   ~ 0
OUT_R
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
	1100 2550 3750 2550
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
NoConn ~ 4050 1400
NoConn ~ 5050 1400
Text Label 4750 2000 1    50   ~ 0
QD_R
Text Label 4550 2000 1    50   ~ 0
QD_L
Text Label 4250 2000 1    50   ~ 0
QD_G
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
P 1050 6800
F 0 "J9" H 1050 7100 50  0000 C CNN
F 1 "SPI" V 1150 6750 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B6B-EH-A_1x06_P2.50mm_Vertical" H 1050 6800 50  0001 C CNN
F 3 "~" H 1050 6800 50  0001 C CNN
	1    1050 6800
	-1   0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0113
U 1 1 5EB4D563
P 1350 7200
F 0 "#PWR0113" H 1350 6950 50  0001 C CNN
F 1 "GNDD" H 1354 7045 50  0000 C CNN
F 2 "" H 1350 7200 50  0001 C CNN
F 3 "" H 1350 7200 50  0001 C CNN
	1    1350 7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1350 7200 1350 7100
Wire Wire Line
	1350 7100 1250 7100
$Comp
L Connector_Generic:Conn_01x05 J10
U 1 1 5EB52636
P 2600 7100
F 0 "J10" H 2600 7400 50  0000 C CNN
F 1 "PLL" V 2700 7100 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B5B-EH-A_1x05_P2.50mm_Vertical" H 2600 7100 50  0001 C CNN
F 3 "~" H 2600 7100 50  0001 C CNN
	1    2600 7100
	1    0    0    -1  
$EndComp
$Comp
L power:GNDA #PWR0116
U 1 1 5EB548DB
P 2300 7350
F 0 "#PWR0116" H 2300 7100 50  0001 C CNN
F 1 "GNDA" H 2305 7177 50  0000 C CNN
F 2 "" H 2300 7350 50  0001 C CNN
F 3 "" H 2300 7350 50  0001 C CNN
	1    2300 7350
	1    0    0    -1  
$EndComp
Wire Wire Line
	2400 7300 2300 7300
Wire Wire Line
	2300 7300 2300 7350
Text Label 2150 7200 0    50   ~ 0
CE
Text Label 2150 7100 0    50   ~ 0
DI
Text Label 2150 7000 0    50   ~ 0
CL
Text Label 2150 6900 0    50   ~ 0
DO
Wire Wire Line
	1250 6900 2400 6900
Wire Wire Line
	1250 7000 2400 7000
Wire Wire Line
	1250 6800 1950 6800
Wire Wire Line
	1950 6800 1950 7100
Wire Wire Line
	1950 7100 2400 7100
Wire Wire Line
	1250 6700 1850 6700
Wire Wire Line
	1850 6700 1850 7200
Wire Wire Line
	1850 7200 2400 7200
$Comp
L h-ms1110:PT2579 U?
U 1 1 5FAD7099
P 7950 4950
F 0 "U?" H 7700 5450 50  0000 C CNN
F 1 "PT2579" H 8150 5450 50  0000 C CNN
F 2 "" H 7950 4550 50  0001 C CNN
F 3 "http://sound.westhost.com/pt2399.pdf" H 7950 4550 50  0001 C CNN
	1    7950 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7500 4900 7200 4900
Text Label 7200 4900 0    50   ~ 0
RDS
Wire Wire Line
	7500 5000 7450 5000
Wire Wire Line
	7450 5000 7450 4400
$Comp
L power:+5V #PWR?
U 1 1 5FAF04F4
P 7450 4400
F 0 "#PWR?" H 7450 4250 50  0001 C CNN
F 1 "+5V" H 7465 4573 50  0000 C CNN
F 2 "" H 7450 4400 50  0001 C CNN
F 3 "" H 7450 4400 50  0001 C CNN
	1    7450 4400
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 5FAF0FF2
P 8450 4400
F 0 "#PWR?" H 8450 4250 50  0001 C CNN
F 1 "+5V" H 8465 4573 50  0000 C CNN
F 2 "" H 8450 4400 50  0001 C CNN
F 3 "" H 8450 4400 50  0001 C CNN
	1    8450 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	8400 5000 8450 5000
Wire Wire Line
	8450 5000 8450 4400
$Comp
L power:GNDD #PWR?
U 1 1 5FAF74C1
P 7450 5500
F 0 "#PWR?" H 7450 5250 50  0001 C CNN
F 1 "GNDD" H 7454 5345 50  0000 C CNN
F 2 "" H 7450 5500 50  0001 C CNN
F 3 "" H 7450 5500 50  0001 C CNN
	1    7450 5500
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR?
U 1 1 5FAF8249
P 8450 5500
F 0 "#PWR?" H 8450 5250 50  0001 C CNN
F 1 "GNDD" H 8454 5345 50  0000 C CNN
F 2 "" H 8450 5500 50  0001 C CNN
F 3 "" H 8450 5500 50  0001 C CNN
	1    8450 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8450 5100 8400 5100
Wire Wire Line
	8450 5100 8450 5200
Wire Wire Line
	7500 5100 7450 5100
Wire Wire Line
	7450 5100 7450 5500
$Comp
L Device:C_Small C?
U 1 1 5FB11A02
P 7300 5300
F 0 "C?" V 7150 5250 50  0000 L CNN
F 1 "560" V 7400 5225 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W3.5mm_P5.00mm" H 7300 5300 50  0001 C CNN
F 3 "~" H 7300 5300 50  0001 C CNN
	1    7300 5300
	0    1    1    0   
$EndComp
Wire Wire Line
	7500 5200 7150 5200
Wire Wire Line
	7150 5200 7150 5300
Wire Wire Line
	7150 5300 7200 5300
Wire Wire Line
	7500 5300 7400 5300
$Comp
L Device:CP_Small C?
U 1 1 5FB5FD58
P 6950 5050
F 0 "C?" H 7038 5096 50  0000 L CNN
F 1 "2.2" H 7038 5005 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 6950 5050 50  0001 C CNN
F 3 "~" H 6950 5050 50  0001 C CNN
	1    6950 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 4800 6950 4950
Wire Wire Line
	7500 4800 6950 4800
Wire Wire Line
	6950 5200 6950 5150
$Comp
L power:GNDD #PWR?
U 1 1 5FB52186
P 6950 5200
F 0 "#PWR?" H 6950 4950 50  0001 C CNN
F 1 "GNDD" H 6954 5045 50  0000 C CNN
F 2 "" H 6950 5200 50  0001 C CNN
F 3 "" H 6950 5200 50  0001 C CNN
	1    6950 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8400 5200 8450 5200
Connection ~ 8450 5200
Wire Wire Line
	8450 5200 8450 5300
Wire Wire Line
	8400 5300 8450 5300
Connection ~ 8450 5300
Wire Wire Line
	8450 5300 8450 5500
$Comp
L Device:CP_Small C?
U 1 1 5FB7C85B
P 9000 5250
F 0 "C?" H 9088 5296 50  0000 L CNN
F 1 "10.0" H 9088 5205 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 9000 5250 50  0001 C CNN
F 3 "~" H 9000 5250 50  0001 C CNN
	1    9000 5250
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5FB7D17E
P 8700 5250
F 0 "C?" H 8792 5296 50  0000 L CNN
F 1 "0.1" H 8792 5205 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W3.5mm_P5.00mm" H 8700 5250 50  0001 C CNN
F 3 "~" H 8700 5250 50  0001 C CNN
	1    8700 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	9000 5000 9000 5150
Connection ~ 8450 5000
Wire Wire Line
	8700 5000 8700 5150
$Comp
L power:GNDD #PWR?
U 1 1 5FB8CE62
P 9000 5500
F 0 "#PWR?" H 9000 5250 50  0001 C CNN
F 1 "GNDD" H 9004 5345 50  0000 C CNN
F 2 "" H 9000 5500 50  0001 C CNN
F 3 "" H 9000 5500 50  0001 C CNN
	1    9000 5500
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR?
U 1 1 5FB8D33C
P 8700 5500
F 0 "#PWR?" H 8700 5250 50  0001 C CNN
F 1 "GNDD" H 8704 5345 50  0000 C CNN
F 2 "" H 8700 5500 50  0001 C CNN
F 3 "" H 8700 5500 50  0001 C CNN
	1    8700 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	9000 5350 9000 5500
Wire Wire Line
	8700 5500 8700 5350
Wire Wire Line
	8450 5000 8700 5000
Connection ~ 8700 5000
Wire Wire Line
	8700 5000 9000 5000
$Comp
L Device:C_Small C?
U 1 1 5FBBA519
P 9350 5400
F 0 "C?" H 9442 5446 50  0000 L CNN
F 1 "47" H 9442 5355 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W3.5mm_P5.00mm" H 9350 5400 50  0001 C CNN
F 3 "~" H 9350 5400 50  0001 C CNN
	1    9350 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:Crystal_Small Y?
U 1 1 5FBBB654
P 9550 4900
F 0 "Y?" H 9550 5125 50  0000 C CNN
F 1 "4.332M" H 9550 5034 50  0000 C CNN
F 2 "" H 9550 4900 50  0001 C CNN
F 3 "~" H 9550 4900 50  0001 C CNN
	1    9550 4900
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5FBCAE57
P 9750 5400
F 0 "C?" H 9842 5446 50  0000 L CNN
F 1 "82" H 9842 5355 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W3.5mm_P5.00mm" H 9750 5400 50  0001 C CNN
F 3 "~" H 9750 5400 50  0001 C CNN
	1    9750 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5FBCB18B
P 9550 5250
F 0 "R?" V 9343 5250 50  0000 C CNN
F 1 "100" V 9434 5250 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 9480 5250 50  0001 C CNN
F 3 "~" H 9550 5250 50  0001 C CNN
	1    9550 5250
	0    1    1    0   
$EndComp
Wire Wire Line
	9400 5250 9350 5250
Wire Wire Line
	9350 5250 9350 5300
Wire Wire Line
	9700 5250 9750 5250
Wire Wire Line
	9750 5250 9750 5300
$Comp
L power:GNDD #PWR?
U 1 1 5FBDB16F
P 9350 5550
F 0 "#PWR?" H 9350 5300 50  0001 C CNN
F 1 "GNDD" H 9354 5395 50  0000 C CNN
F 2 "" H 9350 5550 50  0001 C CNN
F 3 "" H 9350 5550 50  0001 C CNN
	1    9350 5550
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR?
U 1 1 5FBDB69F
P 9750 5550
F 0 "#PWR?" H 9750 5300 50  0001 C CNN
F 1 "GNDD" H 9754 5395 50  0000 C CNN
F 2 "" H 9750 5550 50  0001 C CNN
F 3 "" H 9750 5550 50  0001 C CNN
	1    9750 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	9750 5550 9750 5500
Wire Wire Line
	9350 5550 9350 5500
Wire Wire Line
	9450 4900 9350 4900
Wire Wire Line
	9350 4900 9350 5250
Connection ~ 9350 5250
Wire Wire Line
	9750 5250 9750 4900
Wire Wire Line
	9750 4900 9650 4900
Connection ~ 9750 5250
Wire Wire Line
	9350 4900 8400 4900
Connection ~ 9350 4900
Wire Wire Line
	8400 4800 9750 4800
Wire Wire Line
	9750 4800 9750 4900
Connection ~ 9750 4900
NoConn ~ 8400 4700
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
L Device:R R?
U 1 1 5FC4E3B3
P 6950 4700
F 0 "R?" V 6743 4700 50  0000 C CNN
F 1 "1k" V 6834 4700 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 6880 4700 50  0001 C CNN
F 3 "~" H 6950 4700 50  0001 C CNN
	1    6950 4700
	0    1    1    0   
$EndComp
Wire Wire Line
	6800 4700 6650 4700
$Comp
L Device:R R?
U 1 1 5FC8AE2D
P 6650 4350
F 0 "R?" H 6720 4396 50  0000 L CNN
F 1 "10k" H 6720 4305 50  0000 L CNN
F 2 "" V 6580 4350 50  0001 C CNN
F 3 "~" H 6650 4350 50  0001 C CNN
	1    6650 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 4500 6650 4700
Connection ~ 6650 4700
$Comp
L Device:R R?
U 1 1 5FCA51D4
P 8700 4600
F 0 "R?" V 8493 4600 50  0000 C CNN
F 1 "1k" V 8584 4600 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 8630 4600 50  0001 C CNN
F 3 "~" H 8700 4600 50  0001 C CNN
	1    8700 4600
	0    1    1    0   
$EndComp
Wire Wire Line
	8550 4600 8400 4600
$Comp
L Device:R R?
U 1 1 5FCAE3CC
P 9050 4350
F 0 "R?" H 9120 4396 50  0000 L CNN
F 1 "10k" H 9120 4305 50  0000 L CNN
F 2 "" V 8980 4350 50  0001 C CNN
F 3 "~" H 9050 4350 50  0001 C CNN
	1    9050 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 4500 9050 4600
Wire Wire Line
	9050 4600 8850 4600
Wire Wire Line
	9050 4200 9050 4100
Wire Wire Line
	6650 4100 6650 4200
Wire Wire Line
	7100 4700 7500 4700
Wire Wire Line
	7500 4600 7150 4600
Wire Wire Line
	7150 4600 7150 4500
$Comp
L Device:R R?
U 1 1 5FC79976
P 7150 4350
F 0 "R?" H 7220 4396 50  0000 L CNN
F 1 "1k" H 7220 4305 50  0000 L CNN
F 2 "" V 7080 4350 50  0001 C CNN
F 3 "~" H 7150 4350 50  0001 C CNN
	1    7150 4350
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 5FD0284E
P 6650 4100
F 0 "#PWR?" H 6650 3950 50  0001 C CNN
F 1 "+5V" H 6665 4273 50  0000 C CNN
F 2 "" H 6650 4100 50  0001 C CNN
F 3 "" H 6650 4100 50  0001 C CNN
	1    6650 4100
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 5FD02D2C
P 9050 4100
F 0 "#PWR?" H 9050 3950 50  0001 C CNN
F 1 "+5V" H 9065 4273 50  0000 C CNN
F 2 "" H 9050 4100 50  0001 C CNN
F 3 "" H 9050 4100 50  0001 C CNN
	1    9050 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 4600 9300 4600
Connection ~ 9050 4600
Wire Wire Line
	6350 4700 6650 4700
Text Label 6350 4700 0    50   ~ 0
RDDA
Text Label 9300 4600 2    50   ~ 0
RDCL
Text Label 7150 3950 3    50   ~ 0
QUAL
Wire Wire Line
	7150 3950 7150 4200
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
L power:+12VA #PWR?
U 1 1 5FCC675A
P 6750 2100
F 0 "#PWR?" H 6750 1950 50  0001 C CNN
F 1 "+12VA" H 6765 2273 50  0000 C CNN
F 2 "" H 6750 2100 50  0001 C CNN
F 3 "" H 6750 2100 50  0001 C CNN
	1    6750 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6750 2100 6750 2250
$Comp
L power:+9VA #PWR?
U 1 1 5FCD1850
P 8550 2100
F 0 "#PWR?" H 8550 1975 50  0001 C CNN
F 1 "+9VA" H 8565 2273 50  0000 C CNN
F 2 "" H 8550 2100 50  0001 C CNN
F 3 "" H 8550 2100 50  0001 C CNN
	1    8550 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 2100 8550 2250
$Comp
L Regulator_Linear:LM7809_TO220 U?
U 1 1 5FCE13D8
P 7700 2250
F 0 "U?" H 7700 2492 50  0000 C CNN
F 1 "LM7809_TO220" H 7700 2401 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 7700 2475 50  0001 C CIN
F 3 "https://www.onsemi.cn/PowerSolutions/document/MC7800-D.PDF" H 7700 2200 50  0001 C CNN
	1    7700 2250
	1    0    0    -1  
$EndComp
Wire Notes Line
	800  6400 800  7600
Wire Notes Line
	800  7600 2800 7600
Wire Notes Line
	2800 7600 2800 6400
Wire Notes Line
	2800 6400 800  6400
Text Notes 800  6350 0    50   ~ 0
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
Wire Wire Line
	5550 1400 5550 2100
Wire Wire Line
	5650 1400 5650 2100
NoConn ~ 5550 2100
NoConn ~ 5650 2100
NoConn ~ 4950 2100
NoConn ~ 4850 2100
NoConn ~ 4750 2100
Wire Wire Line
	4750 1400 4750 2100
Wire Wire Line
	4850 1400 4850 2100
Wire Wire Line
	4950 1400 4950 2100
NoConn ~ 4550 2100
NoConn ~ 4250 2100
Wire Wire Line
	4550 1400 4550 2100
Wire Wire Line
	4250 1400 4250 2100
NoConn ~ 2850 1650
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
NoConn ~ 1250 6600
$EndSCHEMATC
