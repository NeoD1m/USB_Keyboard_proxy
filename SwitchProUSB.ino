#include <hidboot.h>
#include <usbhub.h>
#include <HID-Project.h>
#include <HID-Settings.h>
#include <SPI.h>

USB Usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);
uint8_t prevKey;
uint8_t isCaps = 0;

class KbdRptParser : public KeyboardReportParser {
protected:
  void OnKeyDown(uint8_t mod, uint8_t key);
  void OnKeyUp(uint8_t mod, uint8_t key);
  void OnControlKeysChanged(uint8_t before, uint8_t after);
};

// === Функция для синхронизации модификаторов ===
void sendModifiers(uint8_t mod) {
  // Левый Ctrl
  (mod & 0x01) ? Keyboard.press(KEY_LEFT_CTRL) : Keyboard.release(KEY_LEFT_CTRL);
  // Левый Shift
  (mod & 0x02) ? Keyboard.press(KEY_LEFT_SHIFT) : Keyboard.release(KEY_LEFT_SHIFT);
  // Левый Alt
  (mod & 0x04) ? Keyboard.press(KEY_LEFT_ALT) : Keyboard.release(KEY_LEFT_ALT);
  // Левый GUI (Win)
  (mod & 0x08) ? Keyboard.press(KEY_LEFT_GUI) : Keyboard.release(KEY_LEFT_GUI);

  // Правые модификаторы
  (mod & 0x10) ? Keyboard.press(KEY_RIGHT_CTRL) : Keyboard.release(KEY_RIGHT_CTRL);
  (mod & 0x20) ? Keyboard.press(KEY_RIGHT_SHIFT) : Keyboard.release(KEY_RIGHT_SHIFT);
  (mod & 0x40) ? Keyboard.press(KEY_RIGHT_ALT) : Keyboard.release(KEY_RIGHT_ALT);
  (mod & 0x80) ? Keyboard.press(KEY_RIGHT_GUI) : Keyboard.release(KEY_RIGHT_GUI);
}

// === Обработка спец. клавиш ===
void mapSpecialKey(uint8_t key, bool press) {
  switch (key) {
    case 0x29: press ? Keyboard.press(KEY_ESC) : Keyboard.release(KEY_ESC); break;
    case 0x2A: press ? Keyboard.press(KEY_BACKSPACE) : Keyboard.release(KEY_BACKSPACE); break;
    case 0x2B: press ? Keyboard.press(KEY_TAB) : Keyboard.release(KEY_TAB); break;
    case 0x28: press ? Keyboard.press(KEY_ENTER) : Keyboard.release(KEY_ENTER); break;
    case 0x58: press ? Keyboard.press(KEYPAD_ENTER) : Keyboard.release(KEYPAD_ENTER); break;

    // CAPS LOCK — особая клавиша: переключаем вручную при нажатии
    case 0x39:
      if (press) {
        Keyboard.press(KEY_CAPS_LOCK);
      }
      break;

    case 0x4A: press ? Keyboard.press(KEY_HOME) : Keyboard.release(KEY_HOME); break;
    case 0x4C: press ? Keyboard.press(KEY_DELETE) : Keyboard.release(KEY_DELETE); break;
    case 0x4D: press ? Keyboard.press(KEY_END) : Keyboard.release(KEY_END); break;

    case 0x4F: press ? Keyboard.press(KEY_RIGHT_ARROW) : Keyboard.release(KEY_RIGHT_ARROW); break;
    case 0x50: press ? Keyboard.press(KEY_LEFT_ARROW) : Keyboard.release(KEY_LEFT_ARROW); break;
    case 0x51: press ? Keyboard.press(KEY_DOWN_ARROW) : Keyboard.release(KEY_DOWN_ARROW); break;
    case 0x52: press ? Keyboard.press(KEY_UP_ARROW) : Keyboard.release(KEY_UP_ARROW); break;

    default: break;
  }
}

// === Реакция на изменение модификаторов ===
void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {
  sendModifiers(after);
}

// === Нажатие ===
void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key) {
  //Serial.println(key);
  
  sendModifiers(mod);

  // Медиа комбинация (пример)
  if (prevKey == 0x47 && key == 0x62) {
    Consumer.write(MEDIA_PLAY_PAUSE);
    prevKey = 0;
    Keyboard.releaseAll();
    return;
  }
  // WIN SWITCH
   if (prevKey == 0x47 && key == 0x59) {
    prevKey = 0;
    digitalWrite(12, HIGH);
    delay(100);
    digitalWrite(12, LOW);
    delay(100);
    digitalWrite(12, HIGH);
    delay(100);
    digitalWrite(12, LOW);
    Keyboard.releaseAll();
    return;
  }
  // APPLE SWITCH
  if (prevKey == 0x47 && key == 0x5A) {
    prevKey = 0;
    digitalWrite(12, HIGH);
    delay(100);
    digitalWrite(12, LOW);
    delay(100);
    digitalWrite(12, HIGH);
    delay(100);
    digitalWrite(12, LOW);
    delay(100);
    digitalWrite(12, HIGH);
    delay(100);
    digitalWrite(12, LOW);
    Keyboard.releaseAll();
    return;
  }
  // LINUX SWITCH
  if (prevKey == 0x47 && key == 0x5B) {
    prevKey = 0;
    digitalWrite(12, HIGH);
    delay(100);
    digitalWrite(12, LOW);
    delay(100);
    digitalWrite(12, HIGH);
    delay(100);
    digitalWrite(12, LOW);
    delay(100);
    digitalWrite(12, HIGH);
    delay(100);
    digitalWrite(12, LOW);
    delay(100);
    digitalWrite(12, HIGH);
    delay(100);
    digitalWrite(12, LOW);
    Keyboard.releaseAll();
    return;
  }
  uint8_t ascii = OemToAscii(mod, key);
  if (ascii >= 0x20 && ascii <= 0x7E) {
    Keyboard.press(ascii);
  } else {
    mapSpecialKey(key, true);
  }
  prevKey = key;
}

uint8_t changeCaps(uint8_t isCaps){
  if (isCaps == 1) isCaps = 0; else isCaps = 1;
  return isCaps;
}

// === Отпускание ===
void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key) {
  sendModifiers(mod);

  uint8_t ascii = OemToAscii(mod, key);
  if (ascii >= 0x20 && ascii <= 0x7E) {
    Keyboard.release(ascii);
  } else {
    mapSpecialKey(key, false);
  }
}

// === Инициализация ===
KbdRptParser Prs;

void setup() {
  //Serial.begin(115200);
  Usb.Init();
  HidKeyboard.SetReportParser(0, &Prs);
  Consumer.begin();
  Keyboard.begin();
  pinMode(12, OUTPUT);
}

void loop() {
  Usb.Task();
}
