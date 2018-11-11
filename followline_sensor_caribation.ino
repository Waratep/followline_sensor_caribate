#define sensor 6
#define button 3

int _min[] = {1024, 1024, 1024, 1024, 1024, 1024};
int _max[] = {0, 0, 0, 0, 0, 0};
int _mid[] = {0, 0, 0, 0, 0, 0};
int buff_button = 0;

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
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);


}

ISR(TIMER0_COMPA_vect){
  if(digitalRead(button)){
    delay(50);
    while(digitalRead(3));
    buff_button = buff_button == 3 ? 0:buff_button;
    buff_button++;
  }
}

void loop() {
    Init();

//  print_sensor();
    Serial.println(getLeft() - getRight());
//    print_real_sensor();
//  Serial.println(digitalRead(3));
    
  //  Serial.println();
  //  digitalWrite(4,1);
  //  digitalWrite(5,0);
  //  digitalWrite(7,1);
  //  digitalWrite(8,0);
  //  digitalWrite(6,1);
  //
  //  analogWrite(3,124);
  //  analogWrite(9,124);

}

void Init() {
  int average_sensor = 0;
 
  for(int j = 0 ; j < sensor ; j++){
    _max[j] = 0;
    _mid[j] = 0;
    _min[j] = 1024;

  }
  
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
    Serial.print(map(average_sensor(i),_min[i],_max[i],0,10));
    Serial.print("  ");
  }
  Serial.print("  |    ");
  for (int j = 0; j < sensor ; j++) {
    Serial.print(_mid[j]);
    Serial.print("  ");
  }
  
  Serial.println();
}


int getLeft(){
  int left = 0;
  for(int i = 0 ; i < sensor / 2 ; i++){
    left += map(average_sensor(i),_min[i],_max[i],0,10) * (i+1);
    if(i = 0) left = left - 2;
  }
  return left;
}

int getRight(){
  int right = 0;
  for(int i = sensor / 2 ; i < sensor ; i++){
    right += map(average_sensor(i),_min[i],_max[i],0,10) * (i+1);
    if(i = 5) right = right - 1;
  }
  return right;  
}

