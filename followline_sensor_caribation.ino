#define sensor 6
#define button 3
#define base_motor 100
int _min[] = {1024, 1024, 1024, 1024, 1024, 1024};
int _max[] = {0, 0, 0, 0, 0, 0};
int _mid[] = {0, 0, 0, 0, 0, 0};
int _sensor[] = {0, 0, 0, 0, 0, 0};
int buff_button = 0;
int last_error = 0 , sum_error = 0 , error;
int kp = 8 , ki = 0.0001 , kd = 10 , pid = 0;

void setup() {
  cli();
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0  = 0;
  OCR0A = 249;
  TCCR0B |= (1 << WGM01);
  TCCR0B |= (0 << CS02) | (1 << CS01) | (1 << CS00);
  TIMSK0 |= (1 << OCIE0A);
  sei();

  Serial.begin(9600);
  for (int i = 3; i < 9; i++) {
    pinMode(i, OUTPUT);
  }
}


ISR(TIMER0_COMPA_vect) {
  if (digitalRead(button)) {
    delay(50);
    while (digitalRead(3));
    buff_button = buff_button == 3 ? 0 : buff_button;
    buff_button++;

  }
}


void loop() {
  
  Init();

  //  for (int i = 0; i < sensor ; i++) {
  //    Serial.print(_sensor[i]);
  //    Serial.print("  ");
  //  }
  //  Serial.print(" | ");
  //  Serial.println(getError());

  last_error = error;
  error = getError();
  sum_error += error;

  pid = error * kp + sum_error * ki + (error - last_error) * kd;

  pid = pid > 255 ? 255 : pid;
  pid = pid < 0 ? 0 : pid;

  forward(pid,pid*(-1));

}
void Init() {

  int average_sensor = 0;
  while (buff_button == 1) {
    Serial.println(buff_button);
    for (int i = 0 ; i < sensor ; i++) {
      average_sensor = analogRead(i);
      if (average_sensor < _min[i]) _min[i] = average_sensor;
      if (average_sensor > _max[i]) _max[i] = average_sensor;
    }
    for (int i = 0; i < sensor ; i++) {
      _mid[i] = _min[i] + (_max[i] - _min[i]) / 2;
    }
    print_sensor();
  }
}
int average_sensor(int i) {
  long long int average_sensor = 0;
  for (int j = 0; j < 100 ; j++) {
    average_sensor += analogRead(i);
  }
  average_sensor = average_sensor / 100;
  return average_sensor;
}
void print_sensor() {
  for (int i = 0; i < sensor ; i++) {
    _mid[i] = _min[i] + (_max[i] - _min[i]) / 2;
    Serial.print(_min[i]);
    Serial.print("  ");
  }
  Serial.print(" ||||   ");
  for (int i = 0; i < sensor ; i++) {
    _mid[i] = _min[i] + (_max[i] - _min[i]) / 2;
    Serial.print(_max[i]);
    Serial.print("  ");
  }
  Serial.print(" ||||   ");
  print_real_sensor();
  //  Serial.println();
}
void print_real_sensor() {
  for (int i = 0; i < sensor ; i++) {
    Serial.print(_mid[i]);
    Serial.print("  ");
  }
  Serial.println();
}

int getLeft() {
  int left, _min, _max = 0;
  for (int i = 0 ; i < sensor / 2 ; i++) {
    if (analogRead(i) < _mid[i]) {
      _sensor[i] = i - 6;
    } else {
      _sensor[i] = 0;
    }
  }
  int buff = _sensor[0] + _sensor[1] + _sensor[2];
  switch (buff) {
    case -6 : left = -6; break;
    case -15 : left = -5; break;
    case -11 : left = -4; break;
    case -5 : left = -3; break;
    case -9 : left = -2; break;
    case -4 : left = -1; break;
    default : left = 0; break;
  }
  return left;
}
int getRight() {
  int right, _min, _max = 0;
  for (int i = sensor / 2 ; i < sensor ; i++) {
    if (analogRead(i) < _mid[i]) {
      _sensor[i] = i + 1;
    } else {
      _sensor[i] = 0;
    }
  }
  int buff = _sensor[3] + _sensor[4] + _sensor[5];
  switch (buff) {
    case 6 : right = 6; break;
    case 15 : right = 5; break;
    case 11 : right = 4; break;
    case 5 : right = 3; break;
    case 9 : right = 2; break;
    case 4 : right = 1; break;
    default : right = 0; break;
  }

  return right;
}
int getError() {
  int error = 0;
  error = getLeft() + getRight();
  return error;
}

void forward(int x , int y) {

  digitalWrite(4, 1);
  digitalWrite(5, 0);
  digitalWrite(7, 1);
  digitalWrite(8, 0);
  digitalWrite(6, 1);

  analogWrite(3, base_motor + x);
  analogWrite(9, base_motor + y);
}

