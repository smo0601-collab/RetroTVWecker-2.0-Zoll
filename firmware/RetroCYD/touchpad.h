#pragma once
// Datentypen der Touch-Steuerung (hier, damit die Arduino-IDE sie vor ihren automatischen Prototypen kennt)
#include <stdint.h>
struct TCal { uint8_t ver, orient, swap, ok; int16_t x0, x1, y0, y1; };           // Kalibrierung
struct TouchCfg { char center; bool autoA, autoGas, diagA, upIsA; const char* hint; bool direct; };  // Bedeutung je Spiel
// direct = Spiel liest Antippen/Wischen selbst (g_kit.h), kein Steuerkreuz
