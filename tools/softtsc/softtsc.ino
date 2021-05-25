#include <TouchScreen.h>
#include <Wire.h>

const uint8_t YP   = A1;                               // Вывод Y+ должен быть подключен к аналоговому входу
const uint8_t XM   = A0;                               // Вывод X- должен быть подключен к аналоговому входу
const uint8_t YM   = 13;                               // Вывод Y-
const uint8_t XP   = 12;                               // Вывод X+

#define SCREEN_WIDTH   800
#define SCREEN_HEIGHT  480
#define CALIBRATE_OFFSET  35

static uint16_t xPos[9] = { 273, 577, 960, 275, 958, 275, 578, 955, 577 };
static uint16_t yPos[9] = { 177, 177, 177, 538, 538, 890, 886, 888, 540 };
static float touch_x0, touch_x1, touch_x2, touch_x3, touch_x4, touch_x5, touch_x6, touch_x7, touch_x8, touch_y0, touch_y1, touch_y2, touch_y3, touch_y4, touch_y5, touch_y6, touch_y7, touch_y8;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

uint8_t received_command = 0;

bool beetween(float a, float b, float val)
{
  if(a <= val && val <= b) return true;
  if(b <= val && val <= a) return true;
  return false;
}

void Get_Touch_XY(volatile uint16_t *x_kor, volatile uint16_t *y_kor)
{
  uint16_t tmpx = 0, tmpy = 0, touch_x = 0, touch_y = 0;
  TSPoint tsp;

  //считываем координаты опр. кол-во раз и каждый раз находим среднее значение
  //если одна из координат равна нулю второе значение тоже обнуляем и не учитываем его
  for (uint8_t i = 0; i < 10; i++)
  {
    tsp = ts.getPoint();
    touch_x = (touch_x + tsp.x) / 2;
    touch_y = (touch_y + tsp.y) / 2;
  }
  
  //определяем в какую из 4-х частей экрана попадает нажатие, запоминаем края нужной части
  uint8_t area = 0;
  float area_touch_x0, area_touch_x1, area_touch_x2, area_touch_x3, area_touch_x4;
  float area_touch_y0, area_touch_y1, area_touch_y2, area_touch_y3, area_touch_y4;
  if (beetween(touch_x0, touch_x1, touch_x) && beetween(touch_y0, touch_y3, touch_y)) //top-left
  {
    area = 1;
    area_touch_x0 = touch_x0;
    area_touch_x1 = touch_x1;
    area_touch_x2 = touch_x3;
    area_touch_x3 = touch_x8;
    area_touch_y0 = touch_y0;
    area_touch_y1 = touch_y1;
    area_touch_y2 = touch_y3;
    area_touch_y3 = touch_y8;
  }
  else if (beetween(touch_x1, touch_x2, touch_x) && beetween(touch_y2, touch_y4, touch_y)) //top-right
  {
    area = 2;
    area_touch_x0 = touch_x1;
    area_touch_x1 = touch_x2;
    area_touch_x2 = touch_x8;
    area_touch_x3 = touch_x4;
    area_touch_y0 = touch_y1;
    area_touch_y1 = touch_y2;
    area_touch_y2 = touch_y8;
    area_touch_y3 = touch_y4;
  }
  else if (beetween(touch_x5, touch_x6, touch_x) && beetween(touch_y3, touch_y5, touch_y)) //bottom-left
  {
    area = 3;
    area_touch_x0 = touch_x3;
    area_touch_x1 = touch_x8;
    area_touch_x2 = touch_x5;
    area_touch_x3 = touch_x6;
    area_touch_y0 = touch_y3;
    area_touch_y1 = touch_y8;
    area_touch_y2 = touch_y5;
    area_touch_y3 = touch_y6;
  }
  else if (beetween(touch_x6, touch_x7, touch_x) && beetween(touch_y4, touch_y7, touch_y)) //bottom-right
  {
    area = 4;
    area_touch_x0 = touch_x8;
    area_touch_x1 = touch_x4;
    area_touch_x2 = touch_x6;
    area_touch_x3 = touch_x7;
    area_touch_y0 = touch_y8;
    area_touch_y1 = touch_y4;
    area_touch_y2 = touch_y6;
    area_touch_y3 = touch_y7;
  }
  else //весь экран
  {
    area_touch_x0 = touch_x0;
    area_touch_x1 = touch_x2;
    area_touch_x2 = touch_x5;
    area_touch_x3 = touch_x7;
    area_touch_y0 = touch_y0;
    area_touch_y1 = touch_y2;
    area_touch_y2 = touch_y5;
    area_touch_y3 = touch_y8;
  }
  //
  
  //производим расчёт используя координаты полученные при калибровке
  //вычисляем коэффициенты влияния каждой из координат тачпада
  float x0_coeff = 1 - (touch_x - area_touch_x0) / (area_touch_x1 - area_touch_x0);
  float x1_coeff = 1 - x0_coeff;
  float x2_coeff = 1 - (touch_x - area_touch_x2) / (area_touch_x3 - area_touch_x2);
  float x3_coeff = 1 - x2_coeff;

  if (x0_coeff < 0) x0_coeff = 0;
  if (x1_coeff < 0) x1_coeff = 0;
  if (x2_coeff < 0) x2_coeff = 0;
  if (x3_coeff < 0) x3_coeff = 0;
  if (x0_coeff > 1) x0_coeff = 1;
  if (x1_coeff > 1) x1_coeff = 1;
  if (x2_coeff > 1) x2_coeff = 1;
  if (x3_coeff > 1) x3_coeff = 1;

  float y0_coeff = (touch_y - area_touch_y2) / (area_touch_y0 - area_touch_y2);
  float y1_coeff = (touch_y - area_touch_y3) / (area_touch_y1 - area_touch_y3);
  float y2_coeff = 1 - y0_coeff;
  float y3_coeff = 1 - y1_coeff;
  if (y0_coeff < 0) y0_coeff = 0;
  if (y1_coeff < 0) y1_coeff = 0;
  if (y2_coeff < 0) y2_coeff = 0;
  if (y3_coeff < 0) y3_coeff = 0;
  if (y0_coeff > 1) y0_coeff = 1;
  if (y1_coeff > 1) y1_coeff = 1;
  if (y2_coeff > 1) y2_coeff = 1;
  if (y3_coeff > 1) y3_coeff = 1;

  //вычисляем коэффициенты влияния каждого из угла тачпада
  float q0 = x0_coeff * y0_coeff;
  float q1 = x1_coeff * y1_coeff;
  float q2 = x2_coeff * y2_coeff;
  float q3 = x3_coeff * y3_coeff;

  //получаем значения координат на экране
  float true_x = 0;
  float true_y = 0;
  if (area == 1)
  {
    true_x = (SCREEN_WIDTH / 2) * (q1 + q3);
    true_y = (SCREEN_HEIGHT / 2) * (q2 + q3);
  }
  else if (area == 2)
  {
    true_x = (SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 2) * (q1 + q3);
    true_y = (SCREEN_HEIGHT / 2) * (q2 + q3);
  }
  else if (area == 3)
  {
    true_x = (SCREEN_WIDTH / 2) * (q1 + q3);
    true_y = (SCREEN_HEIGHT / 2) + (SCREEN_HEIGHT / 2) * (q2 + q3);
  }
  else if (area == 4)
  {
    true_x = (SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 2) * (q1 + q3);
    true_y = (SCREEN_HEIGHT / 2) + (SCREEN_HEIGHT / 2) * (q2 + q3);
  }
  else
  {
    true_x = SCREEN_WIDTH * (q1 + q3);
    true_y = SCREEN_HEIGHT * (q2 + q3);
  }

  * x_kor = true_x;
  * y_kor = true_y; 
}

void dataRcv(int numBytes)
{
  while(Wire.available()) {
    received_command = Wire.read();
  }
}

void dataRqst()
{
  switch (received_command) {
    case 0x01:
      Wire.write(0xBA);
      break;

    case 0x02:
      if (ts.pressure() > 100 && ts.pressure() < 400)
      { 
        uint16_t x = 0, y = 0; 
        Get_Touch_XY(& x, & y);
         
        Wire.write(1);
        Wire.write((uint8_t) (x >> 8));
        Wire.write((uint8_t) (x & 0xFF));
        Wire.write((uint8_t) (y >> 8));
        Wire.write((uint8_t) (y & 0xFF));
      }
      else
      {
        Wire.write(0);
        Wire.write(0xFF);
        Wire.write(0xFF);
        Wire.write(0xFF);
        Wire.write(0xFF);
      }
      break;

    default: 
      break; 
  }
}

void setup(void){ 
    Serial.begin(115200); 
    Wire.begin(8);
    Wire.onReceive(dataRcv);
    Wire.onRequest(dataRqst);
    Wire.setClock(100000);

    float xc_top = xPos[1]; //центр экрана по верхним калибровкам (160px)
    float xc_center = xPos[8]; //центр экрана X (160px)
    float xc_bottom = xPos[6]; //центр экрана по нижним калибровкам (160px)
  
    touch_x0 = xPos[0] - ((xc_top - xPos[0]) / (SCREEN_WIDTH / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты X верхнего-левого угла
    touch_x1 = xPos[1] - ((xc_center - xPos[1]) / (SCREEN_WIDTH / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты верхнего края по центру
    touch_x2 = xPos[2] + ((xPos[2] - xc_top) / (SCREEN_WIDTH / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты X верхнего-правого угла
    touch_x3 = xPos[3] - ((xc_center - xPos[3]) / (SCREEN_WIDTH / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты левого края по центру
    touch_x4 = xPos[4] + ((xPos[4] - xc_center) / (SCREEN_WIDTH / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты правого края по центру
    touch_x5 = xPos[5] - ((xc_bottom - xPos[5]) / (SCREEN_WIDTH / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты X нижнего-левого угла
    touch_x6 = xPos[6] + ((xPos[6] - xc_center) / (SCREEN_WIDTH / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты нижнего края по центру
    touch_x7 = xPos[7] + ((xPos[7] - xc_bottom) / (SCREEN_WIDTH / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты X нижнего-правого угла
    touch_x8 = xPos[8];  //координаты X центра
  
    float yc_left = yPos[3]; //центр экрана по левым калибровкам (120px)
    float yc_center = yPos[8]; //центр экрана Y (120px)
    float yc_right = yPos[4]; //центр экрана по правым калибровкам (120px)
  
    touch_y0 = yPos[0] + ((yPos[0] - yc_left) / (SCREEN_HEIGHT / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты Y верхнего-левого угла
    touch_y1 = yPos[1] - ((yc_center - yPos[1]) / (SCREEN_HEIGHT / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты верхнего края по центру
    touch_y2 = yPos[2] + ((yPos[2] - yc_right) / (SCREEN_HEIGHT / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты Y верхнего-правого угла
    touch_y3 = yPos[3] - ((yc_center - yPos[3]) / (SCREEN_HEIGHT / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты левого края по центру
    touch_y4 = yPos[4] + ((yPos[4] - yc_center) / (SCREEN_HEIGHT / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты правого края по центру
    touch_y5 = yPos[5] - ((yc_left - yPos[5]) / (SCREEN_HEIGHT / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты Y нижнего-левого угла
    touch_y6 = yPos[6] + ((yPos[6] - yc_center) / (SCREEN_HEIGHT / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты нижнего края по центру
    touch_y7 = yPos[7] - ((yc_right - yPos[7]) / (SCREEN_HEIGHT / 2 - CALIBRATE_OFFSET))*CALIBRATE_OFFSET; //координаты Y нижнего-правого угла
    touch_y8 = yPos[8]; //координаты Y центра
} 
void loop(){ 
//  TSPoint p = ts.getPoint();
//  if(p.z > 100 && p.z < 400)
//  { 
//    uint16_t x = 0, y = 0;
//    Get_Touch_XY(& x, & y);
//    Serial.println((String) "("+x+","+y+")");
//    delay(50);
//  }
}
