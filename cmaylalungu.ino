#include <AFMotor.h>
#include <PS2X_lib.h>
#define PS2_DAT 12 // MISO
#define PS2_CMD 13 // MOSI
#define PS2_SEL 15 // SS
#define PS2_CLK 14 // SLK
#define pressures false
#define rumble false
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
if (ps2x.Analog(PSS_RX)<107)
{
  motor1.setSpeed( speed(50) ); //set speed for motor 1 at 50%
  motor4.setSpeed( speed(50) );//set speed for motor 2 at 100%
} else if (ps2x.Analog(PSS_RX)>147) 
{
  motor1.setSpeed( speed(50) );
  motor4.setSpeed( speed(50) );
} else if (ps2x.Analog(PSS_LY)<118)
{ 
  motor1.setSpeed( speed(100) );
  motor1.run(BACKWARD);
  
  motor4.setSpeed( speed(100) );
  motor4.run(BACKWARD);
} else if (ps2x.Analog(PSS_LY)>138)
{
  motor1.setSpeed( speed(100) );
  motor1.run(FORWARD);
  
  motor4.setSpeed( speed(100) );
  motor4.run(FORWARD); 
} else 
{
 motor1.setSpeed( speed(0) );
 motor4.setSpeed( speed(0) );
}   
  delay(2000);
}

int  speed(int percent)
{
  return map(percent, 0, 100, 0, 255);
}

