#include <Servo.h>

/* ---------- 핀 설정 ---------- */
#define DIR_PIN 7
#define STEP_PIN 8

#define SERVO_PIN 6
#define RELAY_PIN 3

/* ---------- 클래스 설정 ---------- */
#define MAGPIE 1
#define WILD_BOAR 2
#define BIRD 3
#define PHEASANT 4
#define WATERDEER 5

/* ---------- 설정값 ---------- */
#define CENTER_X 320
#define CENTER_Y 240
#define ERROR 20
#define STEP 20
#define SERVO_INITIAL 40

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

    return data;
}

void track_x(int err)
{
    int step = abs(err / 10);

    if (err > 0)
    {
        digitalWrite(DIR_PIN, HIGH);
        step_pos += step;
    }
    else
    {
        digitalWrite(DIR_PIN, LOW);
        step_pos -= step;
    }

    for (int i = 0; i < step; i++)
    {
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(1500000 / step);
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(1500000 / step);
    }
}

void track_y(int err)
{
    if (err > 0)
        servo_ang++;
    else
        servo_ang--;

    if (servo_ang > 180)
        servo_ang = 180;
    if (servo_ang < 0)
        servo_ang = 0;

    servo.write(servo_ang);
}

void shot()
{
    digitalWrite(RELAY_PIN, LOW);
    delay(3000);
    digitalWrite(RELAY_PIN, HIGH);
}

void buzz()
{
    switch (data.type)
    {
    case MAGPIE:
        break;
    case WILD_BOAR:
        break;
    case BIRD:
        break;
    case PHEASANT:
        break;
    case WATERDEER:
        break;
    default:
        break;
    }
}

void go_home(){
    // servo_ang = SERVO_INITIAL;
    // servo.write(servo_ang);

    // if(step_back > 0)
    //     digitalWrite(DIR_PIN, LOW);
    // else
    //     digitalWrite(DIR_PIN, HIGH);

    // for (int i = 0; i < step_back; i++)
    // {
    //     digitalWrite(STEP_PIN, HIGH);
    //     delayMicroseconds(1000);
    //     digitalWrite(STEP_PIN, LOW);
    //     delayMicroseconds(1000);
    // }

    // step_back = 0;
}

void setup()
{
    Serial.begin(115200);
    servo.attach(SERVO_PIN);
    pinMode(DIR_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH);
    servo.write(SERVO_INITIAL);
}

void loop()
{

    if (Serial.available() > 0)
    {
        read_serial();

        if (data.isHuman == 0 && data.isAnimal == 1)
        {
            int err_x = CENTER_X - data.obj_x;
            int err_y = CENTER_Y - data.obj_y;

            if (abs(err_x) < ERROR && abs(err_y) < ERROR)
            {
                // if(servo_ang>???)
                //     buzz();
                // else
                //     shot();
                // return;
            }

            if (!abs(err_x) < ERROR)
            {
                track_x(err_x);
            }
            if (!abs(err_y) < ERROR)
                track_y(err_y);
        }
        else
        {
            // go_home()
        }
    }
}
