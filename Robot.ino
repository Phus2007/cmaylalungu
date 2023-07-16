#include <Wire.h> //thư viện I2c của Arduino, do PCA9685 sử dụng chuẩn giao tiếp i2c nên thư viện này bắt buộc phải khai báo 
#include <Adafruit_PWMServoDriver.h> // thư viện PCA9685
#include <PS2X_lib.h>
/******************************************************************
 * Cài đặt chân cho thư viện :
 * - Trên mạch Motorshield của VIA Makerbot BANHMI, có header 6 chân
 *   được thiết kế để cắm tay cầm PS2.
 * Sơ đồ chân header và sơ đồ GPIO tương ứng:
 *   MOSI | MISO | GND | 3.3V | CS | CLK
 *    12     13    GND   3.3V   15   14
 ******************************************************************/

#define PS2_DAT 12 // MISO
#define PS2_CMD 13 // MOSI
#define PS2_SEL 15 // SS
#define PS2_CLK 14 // SLK

/******************************************************************
 * Lựa chọn chế độ cho tay cầm PS2 :
 *   - pressures = đọc giá trị analog từ các nút bấm
 *   - rumble    = bật/tắt chế độ rung
 ******************************************************************/
#define pressures false
#define rumble false

PS2X ps2x; // khởi tạo class PS2x

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void setup()
{
  pwm.begin(); //khởi tạo PCA9685 
  pwm.setOscillatorFrequency(27000000); // cài đặt tần số dao động 
  pwm.setPWMFreq(50);// cài đặt tần số PWM. Tần số PWM có thể được cài đặt trong khoảng 24-1600 HZ, tần số này được cài đặt tùy thuộc vào nhu cầu xử dụng. Để điều khiển được cả servo và động cơ DC cùng nhau, tần số PWM điều khiển được cài đặt trong khoảng 50-60Hz.
  Wire.setClock(400000); // cài đặt tốc độ giao tiếp i2c ở tốc độ cao nhất(400 Mhz). Hàm này có thể bỏ qua nếu gặp lỗi hoặc không có nhu cầu tử dụng I2c tốc độ cao


  Serial.begin(115200);
  Serial.print("Ket noi voi tay cam PS2:");

  int error = -1;
  for (int i = 0; i < 10; i++) // thử kết nối với tay cầm ps2 trong 10 lần
  {
    delay(50); // đợi 1 giây
    // cài đặt chân và các chế độ: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
    Serial.print(".");
  }

  switch (error) // kiểm tra lỗi nếu sau 10 lần không kết nối được
  {
  case 0:
    Serial.println(" Ket noi tay cam PS2 thanh cong");
    break;
  case 1:
    Serial.println(" LOI: Khong tim thay tay cam, hay kiem tra day ket noi vơi tay cam ");
    break;
  case 2:
    Serial.println(" LOI: khong gui duoc lenh");
    break;
  case 3:
    Serial.println(" LOI: Khong vao duoc Pressures mode ");
    break;
  }
}

void loop(){
  ps2Control();
}
void ps2Control(){
  ps2x.read_gamepad(false, false); // gọi hàm để đọc tay điều khiển

  // các trả về giá trị TRUE (1) khi nút được giữ
  if (ps2x.Button(PSB_START)) // nếu nút Start được giữ, in ra S
    Serial.println("Start is being held");
  if (ps2x.Button(PSB_SELECT)) // nếu nút Select được giữ, in ra Serial monitor
    Serial.println("Select is being held");

  if (ps2x.Button(PSB_PAD_UP)) // tương tự như trên kiểm tra nút Lên (PAD UP)
  {
    Serial.print("Up held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC); // đọc giá trị analog ở nút này, xem nút này được bấm mạnh hay nhẹ
  }
  if (ps2x.Button(PSB_PAD_RIGHT))
  {
    Serial.print("Right held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
  }
  if (ps2x.Button(PSB_PAD_LEFT))
  {
    Serial.print("LEFT held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
  }
  if (ps2x.Button(PSB_PAD_DOWN))
  {
    Serial.print("DOWN held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
  }

  if (ps2x.NewButtonState())
  { // Trả về giá trị TRUE khi nút được thay đổi trạng thái (bật sang tắt, or tắt sang bật)
    if (ps2x.Button(PSB_L3))
      Serial.println("L3 pressed");
    if (ps2x.Button(PSB_R3))
      Serial.println("R3 pressed");
    if (ps2x.Button(PSB_L2))
      Serial.println("L2 pressed");
    if (ps2x.Button(PSB_R2))
      Serial.println("R2 pressed");
    if (ps2x.Button(PSB_TRIANGLE))
      Serial.println("△ pressed");
  }
  //△□○×
  if (ps2x.ButtonPressed(PSB_CIRCLE)) // Trả về giá trị TRUE khi nút được ấn (từ tắt sang bật)
    Serial.println("○ just pressed");
  if (ps2x.NewButtonState(PSB_CROSS)) // Trả về giá trị TRUE khi nút được thay đổi trạng thái
    Serial.println("× just changed");
  if (ps2x.ButtonReleased(PSB_SQUARE)) //  Trả về giá trị TRUE khi nút được ấn (từ tắt sang bật)
    Serial.println("□ just released");

  if (ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) // các trả về giá trị TRUE khi nút được giữ
  {                                               // Đọc giá trị 2 joystick khi nút L1 hoặc R1 được giữ
    Serial.print("Stick Values:");
    Serial.print(ps2x.Analog(PSS_LY)); // đọc trục Y của joystick bên trái. Other options: LX, RY, RX
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_LX), DEC);
    Serial.print(",");
    Serial.print(ps2x.Analog(PSS_RY), DEC);
    Serial.print(",");
    Serial.println(ps2x.Analog(PSS_RX), DEC);
  }
if (ps2x.Analog(PSS_RX)<107)
{
  pwm.setPWM(14,0,0);
  pwm.setPWM(15,0,1024);

  pwm.setPWM(10,0,0);
  pwm.setPWM(11,0,1024);
} else if (ps2x.Analog(PSS_RX)>147)
{
  pwm.setPWM(14,0,1024);
  pwm.setPWM(15,0,0);

  pwm.setPWM(10,0,1024);
  pwm.setPWM(11,0,0);
} else if (ps2x.Analog(PSS_LY)<118)
{
  pwm.setPWM(14,0,0);
  pwm.setPWM(15,0,2048);

  pwm.setPWM(10,0,2048);
  pwm.setPWM(11,0,0);
} else if (ps2x.Analog(PSS_LY)>138)
{
  pwm.setPWM(14,0,2048);
  pwm.setPWM(15,0,0);

  pwm.setPWM(10,0,0);
  pwm.setPWM(11,0,2048);
} else 
{
  pwm.setPWM(14,0,0);
  pwm.setPWM(15,0,0);

  pwm.setPWM(10,0,0);
  pwm.setPWM(11,0,0);
}
  
  

  delay(50);
  if (ps2x.ButtonPressed(PSB_CROSS)){
    pwm.setPWM(8, 0, 4095);
    pwm.setPWM(9, 0, 0);
    Serial.println("× Pressed");
  }
  if (ps2x.ButtonPressed(PSB_TRIANGLE)){
    pwm.setPWM(8, 0, 0);
    pwm.setPWM(9, 0, 4095);
    Serial.println("△ Pressed");
  }
    if (ps2x.ButtonPressed(PSB_SQUARE)){
    pwm.setPWM(8, 0, 0);
    pwm.setPWM(9, 0, 0);
    Serial.println("△ Pressed");
  } 
}
