// Puente H
const int VELOCIDAD = 9;
const int IN1 = 7;
const int IN2 = 8;
// Sensor
const uint8_t TRIG = 12;
const uint8_t ECHO = 3;
// Botones
const int PISO1 = 4;
const int PISO2 = 6;
const int BTN_EMERG = 13;
// Variables globales
const float tope = 27.0;
const float topeMinimo = 4.0;
int pisoDeseado;
bool emergencia = false;
float h;
float  diferencia;
unsigned long tUltima = 0;
float ultimaH = -1;

enum TipoEstado{
  Detenido,
  Subiendo,
  Bajando,
  EMERGENCIA 
};
// Estado del ascensor
TipoEstado estado = Detenido;

const char* nombreEstado( TipoEstado e){
  switch(e){
    case Detenido: return "Detenido";
    case Subiendo: return "Subiendo";
    case Bajando: return "Bajando";
    case EMERGENCIA: return "EMERGENCIA";

    default: return "Desconocido";
  }
}

// Medicion de distancia con el Sensor
float medicion(){
  if(millis()-tUltima < 40 ){ return ultimaH; }
  tUltima = millis();
  
  digitalWrite(TRIG, HIGH);
  unsigned long t0 = micros();
  while(micros() - t0 < 10){}
  digitalWrite(TRIG, LOW);
  
  unsigned long duracion = pulseIn(ECHO , HIGH, 3500UL);
  ultimaH = (duracion == 0) ? -1 : duracion/58.3;
}



void parar(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  estado = Detenido;
}

void subir(){
  digitalWrite (IN1, LOW);
  digitalWrite (IN2, HIGH);
  analogWrite (VELOCIDAD, 190);
  estado = Subiendo;
  
}


void bajar(){
  digitalWrite (IN1, HIGH);
  digitalWrite (IN2, LOW);
  analogWrite (VELOCIDAD, 180);
  estado = Bajando;

}

float objetivo(int pisoDeseado){
  if(pisoDeseado == 1){
    return 5.5;
  }else{
    return 25.5;
  }
}


void irAPiso (int pisoDeseado){
  float h = medicion();
  if(h < 0){
    return;
    }
  diferencia = objetivo(pisoDeseado)-h;

  // Muestreo
  Serial.print("Altura=");
  Serial.print(h);
  Serial.print(" Piso deseado=");
  Serial.print(pisoDeseado);
  Serial.print(" Estado actual=");
  Serial.print(nombreEstado(estado));
  Serial.println();

  if(abs(diferencia)<= 2.0 ){
    parar();
    }
  else if(diferencia>0){
      subir();
    }
  else{
      bajar();
    }

}

void setup() {
  Serial.begin(115200);
  // Puente H
  pinMode (IN1,OUTPUT);
  pinMode (IN2, OUTPUT);
  pinMode(VELOCIDAD, OUTPUT);
  // Sensor
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  digitalWrite(TRIG, LOW);
  // Botones
  pinMode(PISO1,INPUT_PULLUP);
  pinMode(PISO2,INPUT_PULLUP);
  pinMode(BTN_EMERG, INPUT_PULLUP);
}


void loop() {

  if(digitalRead(PISO1) == LOW){ 
    pisoDeseado = 1; 
    }
  if(digitalRead(PISO2) == LOW){ 
    pisoDeseado = 2; 
    }
  if(digitalRead(BTN_EMERG) == LOW){ emergencia = true ;}
  while(emergencia){
    parar();
    estado = EMERGENCIA;
    Serial.print(" Estado actual=");
    Serial.println(nombreEstado(estado));
    }

    irAPiso (pisoDeseado);

}
