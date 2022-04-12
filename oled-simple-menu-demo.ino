#include <U8g2lib.h>
#include "custom_font.h" // credit by 益師傅credit (change font name u8g2_font_unifont_t_chinese1 to demo_font)

#define VR_PIN 34
#define BUTTON_PIN 35
#define debounceTime 30

const char *city_cname[] = {"開頭","基隆", "台北", "桃園", "新竹", "台中",
                            "彰化", "苗栗", "南投", "嘉義", "台南",
                            "高雄", "屏東", "花蓮", "宜蘭", "台東",
                            "澎湖", "金門", "馬祖", "綠島", "蘭嶼","結尾"};
                            
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

int lastVRValue, nowVRValue;

void initU8G2() {
  u8g2.begin();
  u8g2.enableUTF8Print();

  u8g2.setFontDirection(0);
  u8g2.setFontMode(0);  // partial update
  u8g2.clearBuffer();
}


int lastCityIdx = (sizeof(city_cname) / sizeof(city_cname[0])) - 1-2;
char dest[100];
// formatCitySelect 組合選單文字
String formatCitySelect(int idx) {
  memset(dest, 0, sizeof(dest));
  
  strcat(dest, city_cname[idx+1 - 1]);
  strcat(dest, ">");
  strcat(dest, city_cname[idx+1]);
  strcat(dest, "<");
  strcat(dest, city_cname[idx +1+ 1]);

  return dest;
}

// showMenu 顯示選單
void showMenu(int idx) {
  u8g2.clearBuffer();
  
  // city
  u8g2.drawUTF8(1, 16, "選擇城市：");
  u8g2.setCursor(0, 32 + 16);
  u8g2.drawFrame(16 * 3 - 8, 31, 16 * 2 + 1, 16 + 6);
  u8g2.print(formatCitySelect(idx));
  u8g2.sendBuffer();
}

// showSelected 顯示已選項目
void showSelected() {
  u8g2.clearBuffer();
  // city
  u8g2.drawUTF8(1, 16, "你選擇");
  u8g2.setCursor(48, 20 + 16);
  u8g2.print(city_cname[lastVRValue]);
  u8g2.drawUTF8(1,63,"轉動旋鈕重新選擇");
  u8g2.sendBuffer();
}

// === key press ===
uint32_t pressTime = 0;
bool isPressed = false;
bool onClick = false;
// checkBtn simple debounce algorithm, adjust debounceTime appropriate
void checkBtn() {
  if (digitalRead(BUTTON_PIN) == HIGH) {
    if (!isPressed) {
      isPressed = true;
      pressTime = millis();
    }

  } else {
    if (isPressed) {
      if ((millis() - pressTime) > debounceTime) {
        onClick = true;
      }
    }
    isPressed = false;
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(VR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  initU8G2();
  u8g2.setFont(demo_font);
  showMenu(0);
}


void loop() {
   // VR check
  nowVRValue = map(analogRead(VR_PIN), 0, 4095, 0, lastCityIdx);
  if (lastVRValue != nowVRValue) {
    lastVRValue = nowVRValue;
    // 當 VR值有改變，顯示選單
    showMenu(nowVRValue);
  }
  
  // button check
  checkBtn();

  if (onClick) {
    showSelected();
    Serial.print("選擇：");
    Serial.println(city_cname[lastVRValue]);
    onClick = false;
    
  }

}
