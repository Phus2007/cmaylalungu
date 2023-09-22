#include <AFMotor.h>
#include <PS2X_lib.h>
#define PS2_DAT 12 // MISO
#define PS2_CMD 13 // MOSI
#define PS2_SEL 15 // SS
#define PS2_CLK 14 // SLK
#define pressures false
#define rumble false

///2 dây điều khiển 2 servo kết nối với chân số 9 và 10. Nguồn nuôi lấy trực tiếp từ board Arduino (nguồn 5V).
///Motor 1 nối với chân 11
///Motor 2 nối với chân 3
//Motor 3 nối với chân 5
//Motor 4 nối với chân 6


PS2X ps2x;
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

void setup() {
  Serial.begin(9600); // set up Serial library at 9600 bps
  Serial.println("DUC PHU IS BEST");
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

void loop() {
  ps2x.read_gamepad(false, false); //motor 1 cho bên trái motor 2 cho bên phải 
  //11 cho trái, 6 cho phải 
if (ps2x.Analog(PSS_RX)<107)
{
  analogWrite(11, 100);
  analogWrite(6, 200);
} else if (ps2x.Analog(PSS_RX)>147) 
{
  analogWrite(11, 200);
  analogWrite(6, 100);
} else if (ps2x.Analog(PSS_LY)<118)
{ 
  analogWrite(11, 50);
  analogWrite(6, 50);
} else if (ps2x.Analog(PSS_LY)>138)
{
  analogWrite(11, 150);
  analogWrite(6, 150);
} else 
{
 analogWrite(11, 0);
 analogWrite(6, 0);
}   
  delay(1000);
}

int  speed(int percent)
{
  return map(percent, 0, 100, 0, 255);
}
