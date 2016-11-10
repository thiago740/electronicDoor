

#include <Wire.h>
#include <EEPROM.h>
#include <Keypad.h>





int count = 0;
int recebe = 0;
char pass [4] = {'1', '2', '3', '3'}; // senha padrão, você pode muito bem atera-lá
char comparador1[4], comparador2[4];
int D0 = 11;  // Tranca
int D1 = 10;
int Botao = 0;
int estadoBotao = 0;
const int yellowPin = 13;
const int redPin = 11;
const int duration = 350;
const byte ROWS = 4; //Quatro linhas
const byte COLS = 4; //Três colunas
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};// deinição do layout do teclado
byte rowPins[ROWS] = {
  5, 4, 3, 2
}; // pinagem para as linhas do teclado
byte colPins[COLS] = {
  6, 7, 8, 9
}; // pinagem para as colunas do teclado
// mapeamento do teclado
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );




//---------------------------------------------------------




void setup() {

  pinMode(19, OUTPUT);
  digitalWrite(19,HIGH);


  Serial.begin(9600);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(Botao, INPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  key_init();




  Serial.println(EEPROM.read(0));
  Serial.println(EEPROM.read(1));
  Serial.println(EEPROM.read(2));
  Serial.println(EEPROM.read(3));
  Serial.println("==============================");








}
//---------------------------------------------------------
//-------------------------void-loop-----------------------
//---------------------------------------------------------
void loop() {


for(int i=0; i<4; i++) pass[i] = EEPROM.read(i); // Resgatar senha do Administrador (sem usar ID)


  estadoBotao = digitalRead(Botao);
  if (estadoBotao == LOW) {
    digitalWrite(D1, HIGH);
    delay(700);
  }
  else {
    digitalWrite(D1, LOW);
  }
  {
    char key = keypad.getKey(); // obtém informação do teclado




    if (key) {




      Serial.println(key);
    }




    if (key != NO_KEY) { // se foi teclado algo
      if (key == '#') {// se foi teclado #
        code_entry_init(); // mensagem, som e LED
        int entrada = 0; // variável de apoio; números de entradas feitas via teclado
        while (count < 4 ) { // enquanto não entrou os 4 números necessários para a senha
          char key = keypad.getKey(); // obtém informação do teclado
          if (key != NO_KEY) { // se foi teclado algo
            Serial.print('*');
            entrada += 1; // aumenta contrador de entrada
            delay(duration);
            //---------------------------------------------------------------




            pass[count] = EEPROM.read(count); // Resgata senha alterada




            //---------------------------------------------------------------
            if (key == pass[count])count += 1; // verifica na sequencia da senha, se correto aumenta contador
            if ( count == 4 ) {
              Serial.println("");
              unlocked(); // chegou a 4 digitos corretos, libera acesso
            }
            if (entrada != count && entrada == 4) {
              Serial.println("");
              Serial.println("Acesso negado!\n Senha incorreta.");




            }




            if ((key == '#') || (entrada == 4)) { // foi teclado # ou 4 entradas incorretas
              key_init();// inicializa
              break;// interrompe loop
            }
          }
        }
      }
    }




    if (key == '*') { // se foi teclado *
      old_pass_check();// mensagem para entrar a senha antiga
      int entrada = 0;
      while (count < 4 ) {
        char key = keypad.getKey();
        if (key != NO_KEY) {
          entrada += 1;
          Serial.print('*');
          delay(duration);
          if (key == pass[count])count += 1;
          if ( count == 4 ) { // foi teclado a senha antiga corretamente
            get_new_pass();// chama função para entrada da nova senha
          }
          if ((key == '*') || (entrada == 4)) { // foi teclado * ou entrou 4 números errados
            key_init();// inicializa
            break; // interrompe loop
          }
        }
      }
    }
  }
}




//---------------------------------------------------------




void locked() {
  for (int i = 0; i < 11; i++) {
    digitalWrite(yellowPin, LOW);
    digitalWrite(redPin, HIGH);
    delay(duration);
    digitalWrite(redPin, LOW);
    delay(100);
  }
  delay(1000);
}




//---------------------------------------------------------




void get_new_pass() {


  Serial.println("\nNova senha: ");
  new_pass_entry(); // mensagem, som e LED
  int entrada = 0; // inicializa entrada
  while (count < 4) { // enquanto contador for menor que 4
    char key = keypad.getKey(); // obtem informação do teclado
    if (key != NO_KEY) { // se algo foi teclado
      Serial.print('*');
      delay(duration);


      comparador1[count] = key;


      count += 1; // próximo dígito
    }
  }


  if (count == 4) {
    count = 0;
    
    Serial.println("\nDigite novamente: ");
    while (count < 4) { // enquanto contador for menor que 4
      char key = keypad.getKey(); // obtem informação do teclado
      if (key != NO_KEY) { // se algo foi teclado
        Serial.print('*');
        entrada += 1; // aumenta contador de entrada
        delay(duration);




        if (comparador1[count] == key) { // Conferi se é a mesma senha digitada


//      ERRO!!! Cada vez que a condiço  verdadeira o caracter ja fica salvo... 


          EEPROM.write(count, key); // salva a senha na memoria
          count += 1;

          }
          
          if (count == 4) Serial.println("\nSenha salva com sucesso.");
      
          if ((key == '*') || (entrada == 4)) { // foi telcado * 4 entradas          
              if(count != 4) Serial.println("\nInvalido!.");
              key_init() ; // inicializa sistema
              break; // sai
            }    
 
        }
      }
    }
  }










void conferir_senha() {


  Serial.println("");
}






//---------------------------------------------------------








void new_pass_entry() {
  // mensagem no display
  count = 0;
  delay(duration);
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, HIGH);
  digitalWrite(D0, LOW);
}




//---------------------------------------------------------




void old_pass_check() {
  Serial.println("Digite a senha antiga: ");
  count = 0;
  delay(duration);// somente LED amarelo aceso
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, HIGH);
  digitalWrite(D0, LOW);
}




//---------------------------------------------------------




void key_init() {
  count = 0;
  digitalWrite(redPin, HIGH);
  digitalWrite(yellowPin, LOW);
  digitalWrite(D0, LOW);
  delay(duration);
}




//---------------------------------------------------------




void code_entry_init() {

  Serial.print("Digite sua senha: ");
  count = 0; // contador para zero
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, HIGH);
  digitalWrite(D0, LOW);
}




//---------------------------------------------------------




void unlocked() {
  Serial.println("Acesso liberado!\n Senha correta!");
  
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, LOW);
  for (int x = 0; x < 5; x++) {
    digitalWrite(D0, HIGH);
    delay(duration);
    digitalWrite(D0, LOW);
  }
}
