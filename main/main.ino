#include <Servo.h>

/* ---------- 핀 설정 ---------- */
#define DIR_PIN 7
#define STEP_PIN 8

#define SERVO_PIN 6
#define RELAY_PIN 3
#define BUZZER_PIN 5

/* ---------- 예약어 설정 ---------- */
#define LEFT 0
#define RIGHT 1

/* ---------- 클래스 설정 ---------- */
#define MAGPIE 1
#define WILD_BOAR 2
#define BIRD 3
#define PHEASANT 4
#define WATERDEER 5

/* ---------- 설정값 ---------- */
#define CENTER_X 320
#define CENTER_Y 240
#define ERROR 30
#define STEP 200
#define STEP_TIME 200000
#define SERVO_INITIAL 40
#define HOME_COUNT 3

/* ---------- 초기화 ---------- */
Servo servo;

/* ---------- 객체 선언 ---------- */
struct Data
{
    int isHuman = 0;
    int isAnimal = 0;
    int obj_x = CENTER_X;
    int obj_y = CENTER_Y;
    int type;
};

/* ---------- 전역변수 설정 ---------- */
int servo_ang = 40;
int step_pos = 0;
int none_count = 3;
Data data;

/* ---------- 함수 선언 ---------- */
void read_serial()
{
    String receivedData = Serial.readStringUntil('\n');
    data.isHuman = receivedData.substring(0, receivedData.indexOf(',')).toInt();
    receivedData = receivedData.substring(receivedData.indexOf(',') + 1);
    data.isAnimal = receivedData.substring(0, receivedData.indexOf(',')).toInt();
    receivedData = receivedData.substring(receivedData.indexOf(',') + 1);
    data.obj_x = receivedData.substring(0, receivedData.indexOf(',')).toInt();
    receivedData = receivedData.substring(receivedData.indexOf(',') + 1);
    data.obj_y = receivedData.substring(0, receivedData.indexOf(',')).toInt();
    receivedData = receivedData.substring(receivedData.indexOf(',') + 1);
    data.type = receivedData.toInt();

    while (Serial.available() > 0)
        Serial.read();

    return data;
}

void move_x(int dir, int step)
{
    if (dir == LEFT)
    {
        digitalWrite(DIR_PIN, HIGH);
        step_pos += step;
    }
    else
    {
        digitalWrite(DIR_PIN, LOW);
        step_pos -= step;
    }

    if (step_pos > 8000)
    {
        step_pos -= step;
        return;
    }
    if (step_pos < 0)
    {
        step_pos += step;
        return;
    }

    for (int i = 0; i < STEP; i++)
    {
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(STEP_TIME / step / 2);
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(STEP_TIME / step / 2);
    }
}

void track_x(int err)
{
    if (err > 0)
        move_x(LEFT, STEP);
    else
        move_x(RIGHT, STEP);
}

void track_y(int err)
{
    int d_ang = 1;
    if (abs(err) > 60)
        d_ang = 2;
    if (abs(err) > 100)
        d_ang = 3;

    if (err > 0)
        servo_ang += d_ang;
    else
        servo_ang -= d_ang;

    if (servo_ang > 180)
        servo_ang = 180;
    if (servo_ang < 0)
        servo_ang = 0;

    servo.write(servo_ang);
}

void shot()
{
     digitalWrite(RELAY_PIN, LOW);
     delay(500);
     digitalWrite(RELAY_PIN, HIGH);
     delay(2000);
}

void buzz()
{
    switch (data.type)
    {
    case MAGPIE:
        tone(BUZZER_PIN, 600, 3000);
        break;
    case BIRD:
        tone(BUZZER_PIN, 600, 3000);
        break;
    case PHEASANT:
        tone(BUZZER_PIN, 600, 3000);
        break;
    case WILD_BOAR:
        tone(BUZZER_PIN, 50, 3000);
        break;
    case WATERDEER:
        tone(BUZZER_PIN, 50, 3000);
        break;
    default:
        break;
    }
}

void go_home()
{
    servo_ang = SERVO_INITIAL;
    servo.write(servo_ang);

    int err = step_pos - 4000;

    if (err > 20)
    {
        move_x(RIGHT, STEP);
        return;
    }
    if (err < -20)
    {
        move_x(LEFT, STEP);
        return;
    }
}

void setup()
{
    Serial.begin(115200);
    servo.attach(SERVO_PIN);
    pinMode(DIR_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH);
    servo.write(SERVO_INITIAL);
}

void loop()
{
    if (Serial.available() > 0)
    {
        while (true)
        {
            char read = Serial.read();
            if (read == '@')
                break;
        }
        read_serial();

        if (data.isAnimal == 0)
        {
            // none_count++;
            // if (none_count > HOME_COUNT)
                go_home();
        }
        else if (data.isHuman == 0)
        {
            none_count = 0;
            int err_x = CENTER_X - data.obj_x;
            int err_y = CENTER_Y - data.obj_y;

            if (abs(err_x) < ERROR && abs(err_y) < ERROR)
            {
                if (servo_ang > 70)
                    buzz();
                else
                    shot();
            }

            if (abs(err_x) > ERROR)
                track_x(err_x);
            if (abs(err_y) > ERROR)
                track_y(err_y);
        }
    }
}