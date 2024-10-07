#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <OneButton.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // Khởi tạo đối tượng màn hình OLED U8G2


StaticJsonDocument<200> jsonDoc;

const char* jsonString = R"({
  "main": {
    "main1": {
      "text": "CAI DAT",
      "key": "CD",
      "children": {
        "CD1": {
          "key": "CD1",
          "text": "Text1",
          "defaultValue": 50,
          "configuredValue": 50,
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD2": {
          "key": "CD2",
          "text": "Text2",
          "defaultValue": 100,
          "configuredValue": 100,
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD3": {
          "key": "CD3",
          "text": "Text3",
          "defaultValue": 200,
          "configuredValue": 200,
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD4": {
          "key": "CD4",
          "text": "Text4",
          "defaultValue": 300,
          "configuredValue": 300,
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD5": {
          "key": "CD5",
          "text": "Text5",
          "defaultValue": 400,
          "configuredValue": 400,
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD6": {
          "key": "CD6",
          "text": "Text6",
          "defaultValue": 500,
          "configuredValue": 500,
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD7": {
          "key": "CD7",
          "text": "Text7",
          "defaultValue": 600,
          "configuredValue": 600,
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD8": {
          "key": "CD8",
          "text": "Text8",
          "defaultValue": 700,
          "configuredValue": 700,
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD9": {
          "key": "CD9",
          "text": "Text9",
          "defaultValue": 800,
          "configuredValue": 800,
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        },
        "CD10": {
          "key": "CD10",
          "text": "Text10",
          "defaultValue": 900,
          "configuredValue": 900,
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true
        }
      },
      "totalChildren": 10
    },
    "main2": {
      "text": "KIEM TRA",
      "key": "KT",
      "children": {},
      "totalChildren": 0
    },
    "main3": {
      "text": "GIOI THIEU",
      "key": "GT",
      "children": {},
      "totalChildren": 0
    }
  }
})";


// Khai báo các nút
/*const int btnMenu = 32;
const int btnSet = 33;
const int btnUp = 34;
const int btnDown = 35;*/


int btnSetDebounceMill = 0;  // thời gian chống nhiễu phím
int btnSetPressMill = 2000;  // thời gian nhấn giữ phím
int pIndex = 1;
int menuIndex = 1;
int totalChildren; //Tổng số tệp con trong Func loadJsonSettings
int maxValue = 0;
int minValue = 0;
int maxLength = 0; //Số kí tự hiển thị trên func showSetup
int columnIndex = 0; // Biến theo dõi hàng hiện tại (0 = đơn vị, 1 = chục, ...)
int currentValue = 0; // Giá trị hiện tại, ví dụ ban đầu

OneButton btnMenu(32, false,false); 
OneButton btnSet(33, false,false);
OneButton btnUp(34, false,false);
OneButton btnDown(35, false,false);

const char* menu1;
const char* menu2;
const char* menu3;
const char* displayScreen = "MENU";
const char* setupCodeChr;
const char* valueChr;
const char* textChr;


void wrapText(const char* text, int16_t x, int16_t y, int16_t lineHeight, int16_t maxWidth) {   // Hàm wrapText để hiển thị văn bản xuống dòng nếu dài quá
  int16_t cursorX = x;  // Vị trí x bắt đầu in
  int16_t cursorY = y;  // Vị trí y bắt đầu in
  const char* wordStart = text;  // Vị trí bắt đầu của từ trong chuỗi
  const char* currentChar = text;  // Ký tự hiện tại đang xử lý

  while (*currentChar) {     // Vòng lặp qua từng ký tự trong chuỗi
    if (*currentChar == ' ' || *(currentChar + 1) == '\0') {    
      char word[64];   // Tạo chuỗi tạm để chứa từ hiện tại
      int len = currentChar - wordStart + 1;
      strncpy(word, wordStart, len);
      word[len] = '\0';

      int16_t textWidth = u8g2.getStrWidth(word);  // Kiểm tra nếu từ có vừa với chiều rộng màn hình
      if (cursorX + textWidth > maxWidth) {
        cursorX = x;  // Nếu từ quá dài, xuống dòng
        cursorY += lineHeight;  // Tăng vị trí y để xuống dòng
      }

      u8g2.drawStr(cursorX, cursorY, word);  // Vẽ từ lên màn hình
      cursorX += textWidth;  // Cập nhật vị trí x cho từ tiếp theo
      
      if (*currentChar == ' ') {
        cursorX += u8g2.getStrWidth(" ");  // Thêm khoảng trắng nếu ký tự là ' '
      }

      wordStart = currentChar + 1; // Di chuyển đến từ tiếp theo
    }
    currentChar++;  // Chuyển ký tự hiện tại sang ký tự tiếp theo
  }
}

void showList(int indexNum){

  menu1 = jsonDoc["main"]["main1"]["text"];
  menu2 = jsonDoc["main"]["main2"]["text"];
  menu3 = jsonDoc["main"]["main3"]["text"];

  u8g2.clearBuffer();  // Xóa bộ nhớ đệm của màn hình để vẽ mới
  u8g2.setFont(u8g2_font_crox3h_tf);  // Thiết lập font chữ thường (không đậm)

  
  u8g2.drawStr(12, 16, menu1);  // Hiển thị danh mục 1
  u8g2.drawStr(12, 32, menu2);  // Hiển thị danh mục 2
  u8g2.drawStr(12, 48, menu3);  // Hiển thị danh mục 3

  u8g2.drawStr(0, indexNum * 16, ">");  // Hiển thị mã cài đặt (tại vị trí x=0, y=18)
  u8g2.sendBuffer(); // Gửi nội dung đệm ra màn hình
  displayScreen = "MENU";
}

void showSetup(const char* setUpCode, const char* value, const char* text) {   // Thêm maxValue vào tham số
  u8g2.clearBuffer();  // Xóa bộ nhớ đệm của màn hình để vẽ mới
  u8g2.setFont(u8g2_font_crox3hb_tf);  // Thiết lập font chữ đậm

  char tempSetUpCode[64];    // Tạo một chuỗi tạm chứa mã cài đặt và dấu ";"
  snprintf(tempSetUpCode, sizeof(tempSetUpCode), "%s:", setUpCode);  // Nối mã cài đặt với dấu ":"
  
  u8g2.drawStr(0, 18, tempSetUpCode);  // Hiển thị mã cài đặt (tại vị trí x=0, y=18)
  u8g2.setFont(u8g2_font_crox3h_tf);  // Thiết lập font chữ thường (không đậm)
  
  // Chuyển đổi maxValue sang chuỗi
  char maxValueStr[16]; // Chuỗi chứa giá trị maxValue sau khi chuyển đổi
  snprintf(maxValueStr, sizeof(maxValueStr), "%d", maxValue);  // Chuyển maxValue thành chuỗi
  
  // Tính toán độ dài của value và maxValueStr
  int valueLength = strlen(value);
  maxLength = strlen(maxValueStr);

  // Giới hạn độ dài value không vượt quá chiều dài maxValue
  if (valueLength > maxLength) {
      valueLength = maxLength;
  }

  // Vị trí bắt đầu cho ký tự đầu tiên căn lề từ phải
  int startX = 128 - (valueLength * 10); // 8 là độ rộng trung bình của một ký tự (nếu sử dụng font có kích thước tiêu chuẩn)

  // Vẽ từng ký tự từ value lên màn hình
  for (int i = 0; i < valueLength; i++) {
      char temp[2] = {value[i], '\0'}; // Lấy từng ký tự và biến nó thành chuỗi
      u8g2.drawStr(startX + (i * 10), 18, temp); // Vẽ ký tự tại vị trí tương ứng
  }

  u8g2.drawLine(0, 23, 128, 23);  // Vẽ một đường ngang trên màn hình (tọa độ từ x=0 đến x=128)

  wrapText(text, 0, 42, 18, 128);  // Bắt đầu tại tọa độ x=0, y=46, mỗi dòng cách nhau 18 điểm, tối đa chiều rộng 128 điểm
  u8g2.sendBuffer(); // Gửi nội dung đệm ra màn hình
}

void showEdit(int columnIndex) { 
  u8g2.clearBuffer();  // Xóa bộ nhớ đệm của màn hình để vẽ mới
  u8g2.setFont(u8g2_font_crox3hb_tf);  // Thiết lập font chữ đậm

  char tempSetUpCode[64];    // Tạo một chuỗi tạm chứa mã cài đặt và dấu ";"
  snprintf(tempSetUpCode, sizeof(tempSetUpCode), "%s:", setupCodeChr);  // Nối mã cài đặt với dấu ":"
  
  u8g2.drawStr(0, 18, tempSetUpCode);  // Hiển thị mã cài đặt (tại vị trí x=0, y=18)
  u8g2.setFont(u8g2_font_crox3h_tf);  // Thiết lập font chữ thường (không đậm)

  char valueStr[16];
  snprintf(valueStr, sizeof(valueStr), "%d", valueChr);

  int startX = 128 - (maxLength * 10);

  for (int i = 0; i < maxLength; i++) {

    char temp[2] = {valueStr[i], '\0'};
    if (i == columnIndex) {
      // Nếu là hàng hiện tại, bôi đen
      u8g2.setDrawColor(1);  // Màu nền
      u8g2.drawBox(startX + (i * 10) - 1, 5, 10, 18);  // Vẽ hộp đen
      u8g2.setDrawColor(0);  // Màu chữ
      u8g2.drawStr(startX + (i * 10), 18, temp);
      u8g2.setDrawColor(1);  // Trả lại màu cho các phần khác
    } else {
      u8g2.drawStr(startX + (i * 10), 18, temp);
    }
  }

  u8g2.drawLine(0, 23, 128, 23);  // Vẽ một đường ngang trên màn hình (tọa độ từ x=0 đến x=128)

  wrapText(textChr, 0, 42, 18, 128);  // Bắt đầu tại tọa độ x=0, y=46, mỗi dòng cách nhau 18 điểm, tối đa chiều rộng 128 điểm
  u8g2.sendBuffer(); // Gửi nội dung đệm ra màn hình
}

void loadJsonSettings() {
    try {
        const char* code = jsonDoc["main"]["main" + String(menuIndex)]["key"]; // Truy xuất key của mục menu hiện tại từ JSON
        totalChildren = jsonDoc["main"]["main" + String(menuIndex)]["totalChildren"]; // Truy xuất tổng số lượng phần tử con
        String setupCode = String(code) + String(pIndex); // Tạo setupCode dựa trên key và pIndex (số thứ tự)

        String valueStr; // Khai báo biến String

        // Kiểm tra xem configuredValue có phải là số hay không
        if (jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCode]["configuredValue"].is<int>()) {
            // Nếu là số, chuyển đổi sang String
            int valueInt = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCode]["configuredValue"];
            valueStr = String(valueInt); // Chuyển đổi value từ int thành String
        } else if (jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCode]["configuredValue"].is<const char*>()) {
            // Nếu là chuỗi, lấy giá trị trực tiếp
            valueStr = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCode]["configuredValue"].as<const char*>();
        } else {
            valueStr = ""; // Gán giá trị mặc định nếu không phải số hoặc chuỗi
        }

        maxValue = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCode]["maxValue"];
        minValue = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCode]["minValue"];
        currentValue = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCode]["configuredValue"];

        textChr = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCode]["text"].as<const char*>(); // Truy xuất text từ JSON
        setupCodeChr = setupCode.c_str();
        valueChr = valueStr.c_str();

        showSetup(setupCodeChr, valueChr , textChr); // Hiển thị thông tin cấu hình bằng cách gọi hàm showSetup
        displayScreen = "ScreenCD"; // Chuyển màn hình sau khi xử lý dữ liệu thành công
    } catch (const std::exception& e) { // Bắt lỗi nếu có ngoại lệ
        Serial.println("Error reading JSON data: "); // In thông báo lỗi
        Serial.println(e.what()); // In thông tin chi tiết từ e.what()
    } catch (...) { // Bắt mọi lỗi khác không thuộc std::exception
        Serial.println("An unknown error occurred while reading JSON data."); // In thông báo lỗi chung
    }
}


void btnMenuClick() {
  //Serial.println("Button Clicked (nhấn nhả)");
  if (displayScreen == "ScreenSD") {
    displayScreen = "MENU";
    showList(menuIndex);  // Hiển thị danh sách menu hiện tại
  }
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnMenuLongPressStart() {
  //Serial.println("Button Long Press Started (BtnMenu)");
}

// Hàm callback khi nút đang được giữ
void btnMenuDuringLongPress() {
  //Serial.println("Button is being Long Pressed (BtnMenu)");
}

void btnSetClick() {
  if (displayScreen == "MENU") {
    pIndex = 1;
    loadJsonSettings(); // Hiển thị giá trị thiết lập
  } else if (displayScreen == "ScreenCD"){
    columnIndex = 1;
    showEdit(columnIndex);
  }
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnSetLongPressStart() {
  showSetup("Setup", "OFF", "BatDauNhanGiu");
}

// Hàm callback khi nút đang được giữ
void btnSetDuringLongPress() {
  showSetup("Setup", "OFF", "Dang giu nut");
}

void btnUpClick() {
  if (displayScreen == "MENU") {
    if (menuIndex + 1 > 3) {
      menuIndex = 1;  // Khi chỉ số vượt quá giới hạn, quay lại đầu danh sách
    } else {
      menuIndex++;    // Tăng menuIndex lên 1
    }
    showList(menuIndex);  // Hiển thị danh sách với chỉ số mới
  } else if (displayScreen == "ScreenCD") {
    if (pIndex + 1 > totalChildren) {
      pIndex = 1;
    } else {
      pIndex ++;
    }
    loadJsonSettings(); // Hiển thị giá trị thiết lập
  } else if (displayScreen == "ScreenEdit") {
    columnIndex ++;
    showEdit(columnIndex);
  }
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnUpLongPressStart() { 
  //Serial.println("Button Long Press Started (btnUp)");
}

// Hàm callback khi nút đang được giữ
void btnUpDuringLongPress() {
  //Serial.println("Button is being Long Pressed (btnUp)");
}

void btnDownClick() {
  if (displayScreen == "MENU") {
    if (menuIndex - 1 < 1) {
      menuIndex = 3;  // Khi chỉ số nhỏ hơn giới hạn, quay lại cuối danh sách
    } else {
      menuIndex--;    // Giảm menuIndex đi 1
    }
    showList(menuIndex);  // Hiển thị danh sách với chỉ số mới
  } else if (displayScreen == "ScreenCD"){
    if (pIndex - 1 < 1) {
      pIndex = totalChildren;
    } else {
      pIndex --;
    }
    loadJsonSettings(); // Hiển thị giá trị thiết lập
  } else if (displayScreen == "ScreenEdit"){
    columnIndex --;
    showEdit(columnIndex);
  }
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnDownLongPressStart() {
  //Serial.println("Button Long Press Started (btnDown)");
}

// Hàm callback khi nút đang được giữ
void btnDownDuringLongPress() {
  //Serial.println("Button is being Long Pressed (btnDown)");
}

void setup() {

  Serial.begin(115200);     // Khởi tạo Serial và màn hình
  u8g2.begin();  // Khởi tạo màn hình OLED
 
  DeserializationError error = deserializeJson(jsonDoc, jsonString);    // Phân tích chuỗi JSON
  if (error) {
    showSetup("Error","E002","Json Error");
    /*Serial.print(F("deserializeJson() failed: "));
    //Serial.println(error.f_str());
    return;*/
  }
  btnMenu.attachClick(btnMenuClick);
  btnMenu.attachLongPressStart(btnMenuLongPressStart);
  btnMenu.attachDuringLongPress(btnMenuDuringLongPress);

  btnSet.attachClick(btnSetClick);
  btnSet.attachLongPressStart(btnSetLongPressStart);
  btnSet.attachDuringLongPress(btnSetDuringLongPress);

  btnUp.attachClick(btnUpClick);
  btnUp.attachLongPressStart(btnUpLongPressStart);
  btnUp.attachDuringLongPress(btnUpDuringLongPress);

  btnDown.attachClick(btnDownClick);  
  btnDown.attachLongPressStart(btnDownLongPressStart);
  btnDown.attachDuringLongPress(btnDownDuringLongPress);

  btnMenu.setDebounceMs(btnSetDebounceMill);
  btnSet.setDebounceMs(btnSetDebounceMill);
  btnUp.setDebounceMs(btnSetDebounceMill);
  btnDown.setDebounceMs(btnSetDebounceMill);

  btnMenu.setPressMs(btnSetPressMill);
  btnSet.setPressMs(btnSetPressMill);
  btnUp.setPressMs(btnSetPressMill);
  btnDown.setPressMs(btnSetPressMill);

} 

void loop() {
  btnMenu.tick();
  btnSet.tick();
  btnUp.tick();
  btnDown.tick(); 
}
