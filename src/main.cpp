#include <Arduino.h>
#include <U8glib.h>
#include <Tone.h>
#include <pb_arduino.h>
#include <protobuf/heartbeat.pb.h>
////////////////////////////////////////////////////
//lcd pin
#define LCD_CS 42
#define LCD_RES 43
#define LCD_CD 44
#define SD_CS 13
#define LCD_SCK 30
#define LCD_MOSI 35
U8GLIB_SH1106_128X64 u8g(LCD_SCK, LCD_MOSI, LCD_CS, LCD_CD);
////////////////////////////////////////////////////
//led pin
#define RGB_LED_R 2
#define RGB_LED_G 3
#define RGB_LED_B 5
////////////////////////////////////////////////////
//key pin
#define JOY_UP 33
#define JOY_DOWN 41
#define JOY_LEFT 32
#define JOY_RIGHT 37
#define JOY_CENTER 40
#define BUTTON_A 47
#define BUTTON_B 49
#define BUTTON_C 12
#define BUTTON_D 11
#define VOLUME 15
////////////////////////////////////////////////////
//msc
#define BUZZER 45
#define GET_VARIABLE_NAME(Variable) (#Variable)
#define BAUDRATE 115200
#define TERMINATOR 0
/* window frame */
int window_x_origin = 0;
int window_y_origin = 0;
int window_x_length = 128;
int window_y_length = 63;
int window_x_split = 50;
/* cache */
int last_input = 0;
int last_volume = 0;
int last_req_id = 0;
bool last_req_decode_success = false;
/* buzzer */
Tone buzzer;
int notes[] = {NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
               NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
               NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
               NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7};
bool buzzer_muted = false;
/* pb */
pb_istream_s pb_in;
pb_ostream_s pb_out;
////////////////////////////////////////////////////
//prototypes
void controller_main();
void draw_status();
const char *get_input_label(int);
int read_key();
int read_volume();
bool send_msg(int);
void set_rgb_led(int, int, int);
void welcome();
///////////////////////////////////////
//controller main function
void controller_main()
{
  // process the incoming packet if the buffer is not empty
  if (Serial.available() > 0)
  {
    set_rgb_led(HIGH, LOW, HIGH);
    Request req;
    //FIXME: decoding takes too long
    if (!buzzer_muted)
    {
      buzzer.play(notes[last_req_id % (sizeof(notes) / sizeof(int))], 100);
    }
    last_req_decode_success = pb_decode(&pb_in, Request_fields, &req);
    if (last_req_id != req.id)
    {
      last_req_id = req.id;
    }
    delay(10);
  }
  set_rgb_led(HIGH, HIGH, HIGH);
  // read input
  int input_tmp = read_key();
  switch (input_tmp)
  {
  case BUTTON_A:
    buzzer_muted = false;
    break;
  case BUTTON_B:
    buzzer_muted = true;
    break;
  }
  if (input_tmp != 0)
  {
    set_rgb_led(LOW, HIGH, HIGH);
    last_input = input_tmp;
    send_msg(last_input);
    delay(10);
  }
  set_rgb_led(HIGH, HIGH, HIGH);
  draw_status();
}
///////////////////////////////////////
//draw current status
void draw_status()
{
  u8g.firstPage();
  do
  {
    // box outline
    u8g.drawFrame(window_x_origin, window_y_origin, window_x_length, window_y_length);
    // set text font and size
    u8g.setFont(u8g_font_5x8);
    // input
    u8g.setPrintPos(window_x_origin + 5, 12);
    u8g.print("Input");
    u8g.setPrintPos(window_x_origin + 5, 22);
    u8g.print(get_input_label(last_input));
    if (last_input == VOLUME)
    {
      u8g.setPrintPos(window_x_origin + 40, 22);
      u8g.print(last_volume);
    }
    if (buzzer_muted)
    {
      u8g.setPrintPos(window_x_origin + 5, 32);
      u8g.print("Muted");
    }

    // center line
    u8g.drawLine(window_x_split, window_y_origin, window_x_split, window_y_length);
    // pb
    u8g.setPrintPos(window_x_split + 5, 12);
    u8g.print("Request(proto)");
    u8g.setPrintPos(window_x_split + 5, 22);
    char c_str[10];
    sprintf(c_str, "%d/%d", last_req_id % (sizeof(notes) / sizeof(int)), sizeof(notes) / sizeof(int));
    u8g.print(c_str);
    u8g.setPrintPos(window_x_split + 5, 32);
    u8g.print("Last decoding");
    u8g.setPrintPos(window_x_split + 5, 42);
    u8g.print(last_req_decode_success ? "Success" : "Fail");
  } while (u8g.nextPage());
}
///////////////////////////////////////
//get const char* for the input
const char *get_input_label(int key)
{
  if (key == JOY_UP)
  {
    return GET_VARIABLE_NAME(JOY_UP);
  }
  else if (key == JOY_DOWN)
  {
    return GET_VARIABLE_NAME(JOY_DOWN);
  }
  else if (key == JOY_LEFT)
  {
    return GET_VARIABLE_NAME(JOY_LEFT);
  }
  else if (key == JOY_RIGHT)
  {
    return GET_VARIABLE_NAME(JOY_RIGHT);
  }
  else if (key == JOY_CENTER)
  {
    return GET_VARIABLE_NAME(JOY_CENTER);
  }
  else if (key == BUTTON_A)
  {
    return GET_VARIABLE_NAME(BUTTON_A);
  }
  else if (key == BUTTON_B)
  {
    return GET_VARIABLE_NAME(BUTTON_B);
  }
  else if (key == BUTTON_C)
  {
    return GET_VARIABLE_NAME(BUTTON_C);
  }
  else if (key == BUTTON_D)
  {
    return GET_VARIABLE_NAME(BUTTON_D);
  }
  else if (key == VOLUME)
  {
    return GET_VARIABLE_NAME(VOLUME);
  }
  return "";
}
///////////////////////////////////////
//read key value
int read_key()
{
  int key_tmp;
  if (digitalRead(JOY_UP) == LOW)
  {
    key_tmp = JOY_UP;
    return key_tmp;
  }
  if (digitalRead(JOY_DOWN) == LOW)
  {
    key_tmp = JOY_DOWN;
    return key_tmp;
  }
  if (digitalRead(JOY_LEFT) == LOW)
  {
    key_tmp = JOY_LEFT;
    return key_tmp;
  }
  if (digitalRead(JOY_RIGHT) == LOW)
  {
    key_tmp = JOY_RIGHT;
    return key_tmp;
  }
  if (digitalRead(JOY_CENTER) == LOW)
  {
    key_tmp = JOY_CENTER;
    return key_tmp;
  }
  if (digitalRead(BUTTON_A) == LOW)
  {
    key_tmp = BUTTON_A;
    return key_tmp;
  }
  if (digitalRead(BUTTON_B) == LOW)
  {
    key_tmp = BUTTON_B;
    return key_tmp;
  }
  if (digitalRead(BUTTON_C) == LOW)
  {
    key_tmp = BUTTON_C;
    return key_tmp;
  }
  if (digitalRead(BUTTON_D) == LOW)
  {
    key_tmp = BUTTON_D;
    return key_tmp;
  }
  int vol_tmp = read_volume();
  if (vol_tmp != last_volume)
  {
    last_volume = vol_tmp;
    key_tmp = VOLUME;
    return key_tmp;
  }
  return 0;
}
///////////////////////////////////////
//read volume
int read_volume()
{
  return analogRead(VOLUME) / 100;
}
///////////////////////////////////////
//read volume
bool send_msg(int i)
{
  Input input = {};
  input.pin = i;
  input.volume = last_volume;
  bool res = pb_encode(&pb_out, Input_fields, &input);
  Serial.write(TERMINATOR);
  return res;
}
////////////////////////////////////////////////////
//rgb led
void set_rgb_led(int r, int g, int b)
{
  digitalWrite(RGB_LED_R, r);
  digitalWrite(RGB_LED_G, g);
  digitalWrite(RGB_LED_B, b);
}
////////////////////////////////////////////////////
//welcome screen
void welcome()
{
  /* play welcome beep */
  buzzer.play(NOTE_C4, 250);
  delay(250);
  buzzer.play(NOTE_E4, 250);
  delay(250);
  buzzer.play(NOTE_G4, 250);
  delay(250);

  /* screen */
  u8g.firstPage();
  do
  {
    u8g.setFont(u8g_font_gdr14r);
    u8g.setPrintPos(0, 30);
    u8g.print("UFACTORY");
    u8g.setPrintPos(0, 50);
    u8g.print("      Controller");
    u8g.setFont(u8g_font_9x18);

  } while (u8g.nextPage());
}
///////////////////////////////////////
//initialization
void setup(void)
{
  /* init the serial i/f for the MES */
  Serial.begin(BAUDRATE);
  pb_in = as_pb_istream(Serial);
  pb_out = as_pb_ostream(Serial);

  pinMode(JOY_UP, INPUT_PULLUP);
  pinMode(JOY_DOWN, INPUT_PULLUP);
  pinMode(JOY_LEFT, INPUT_PULLUP);
  pinMode(JOY_RIGHT, INPUT_PULLUP);
  pinMode(JOY_CENTER, INPUT_PULLUP);
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  pinMode(BUTTON_D, INPUT_PULLUP);

  pinMode(RGB_LED_R, OUTPUT);
  pinMode(RGB_LED_G, OUTPUT);
  pinMode(RGB_LED_B, OUTPUT);

  //pinMode(BUZZER, OUTPUT);
  buzzer.begin(BUZZER);

  pinMode(LCD_RES, OUTPUT);
  digitalWrite(LCD_RES, HIGH);
  Serial.begin(115200);
  randomSeed(analogRead(A0));
  welcome();
  delay(1000);
}

void loop(void) //main loop
{
  /*
  if (digitalRead(BUTTON_A) == LOW)
  {
    controller_main();
  }
  */
  controller_main();
}
