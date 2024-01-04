#include <Servo.h>
#include <LiquidCrystal_I2C.h>

Servo meuServo1;
Servo meuServo2;

LiquidCrystal_I2C lcd(32, 16, 2); // O endereço 0x27 pode variar

int posicaoPrimaria = 0;
int posicaoSecundaria = 90;
int tempo = 1;

unsigned long tempoAnterior = 0;
unsigned long intervalo = 20;

float leitura_sensor = 0;
int temperatura = 0;
float temperaturaAnterior = 0.0;
int botaoPin = 2;

const int servo1Pin = 9;
const int servo2Pin = 8;
const int piezoPin = 7;

bool emPosicaoSecundaria = false;
unsigned long tempoEspera = 0;

void setup() {
  meuServo1.attach(servo1Pin);
  meuServo2.attach(servo2Pin);
  meuServo1.write(posicaoPrimaria);
  meuServo2.write(posicaoPrimaria);
  meuServo1.write(posicaoSecundaria);
  meuServo2.write(posicaoSecundaria);

  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(13, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(5, INPUT);
  pinMode(botaoPin, INPUT);
  pinMode(11, OUTPUT);
  pinMode(piezoPin, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema a Ligar");

  digitalWrite(7, LOW);
}

void lersensor() {
  leitura_sensor = analogRead(A0);
  botaoPin = digitalRead(2);
  temperatura = map(leitura_sensor, 20, 358, -40, 125);
}

void controloTemperatura() {
  if (botaoPin == LOW) {
    digitalWrite(10, LOW);

    if (temperatura >= 11) {
      digitalWrite(13, LOW);
      digitalWrite(4, HIGH);
    } else if (temperatura <= 10) {
      digitalWrite(13, HIGH);
      digitalWrite(4, LOW);
    }

    if (temperatura != temperaturaAnterior || temperatura == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temperatura: ");
      lcd.print(temperatura);
      temperaturaAnterior = temperatura;
    }

    lcd.setCursor(0, 1);

    if (temperatura > 11) {
      lcd.print("Muito Quente");
    } else if (temperatura < 10) {
      lcd.print("Muito Frio");
    }
  }
}

void estadodobotao() {
  digitalWrite(10, HIGH);
  digitalWrite(13, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(11, HIGH); // Liga a lâmpada

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Porta Aberta ");
  temperatura = 0;
  temperaturaAnterior = 0;
  meuServo1.write(posicaoSecundaria);
  meuServo2.write(posicaoSecundaria);
}

void loop() {
  unsigned long tempoAtual = millis();

  if (!emPosicaoSecundaria) {
    if (tempoAtual - tempoAnterior >= intervalo) {
      tempoAnterior = tempoAtual;
      int pos = meuServo1.read();
      if (pos < posicaoSecundaria) {
        meuServo1.write(pos + tempo);
        meuServo2.write(pos + tempo);
      } else {
        emPosicaoSecundaria = true;
        tempoEspera = tempoAtual + 3000; // 3 segundos
      }
    }
  } 
  else {
    if (tempoAtual >= tempoEspera) {
      int pos = meuServo1.read();
      if (pos > posicaoPrimaria) {
        meuServo1.write(pos - tempo);
        meuServo2.write(pos - tempo);
      } 
      else {
        emPosicaoSecundaria = false;
        tempoEspera = tempoAtual + 3000; // 3 segundos
      }
    }

    /*if (digitalRead(10) == HIGH) {
      digitalWrite(7, !digitalRead(7));
      delay(50);
    }*/

    lersensor();
    controloTemperatura();
    
    if(digitalRead(2) == HIGH) {
      estadodobotao();
      unsigned long inicioEspera = millis(); // Tempo atual
      while (millis() - inicioEspera < 5000) {
        if (digitalRead(2) == LOW) {
          break;
        }
      }
      if (millis() - inicioEspera >= 5000) {
        tone(piezoPin, 1000);
      }
    }
    else {
      digitalWrite(2, LOW);
      digitalWrite(11, LOW);
      noTone(piezoPin); // Para o piezo
    }
  }
}















