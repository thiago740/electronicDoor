/*
      Programa como interface Serial monitor, reset de senha,
    login do administrador e idição de senha. 
*/

#include <Keypad.h>
#include <Wire.h>
#include <EEPROM.h>


int count = 0, entrada = 0, ID, count_ID = 0;
boolean ID_salvo = true;
char id[2];
char pass [] = {'1', '2', '3', '4', '5', '6'}; // senha padrão, você pode muito bem atera-lá
char comparador1[6];
int D0 = 11;  // Tranca
int D1 = 0;
int Reset = 10; 
int estadoReset = 0;
const int yellowPin = 13;
const int redPin = 12;
const int duration = 350;
const byte ROWS = 4; //Quatro linhas
const byte COLS = 4; //Três colunas

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};// deinição do layout do teclado

byte rowPins[ROWS] = {2, 3, 4, 5}; // pinagem para as linhas do teclado
byte colPins[COLS] = {6, 7, 8, 9}; // pinagem para as colunas do teclado
// mapeamento do teclado
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


//---------------------------------------------------------

void setup() {


  //pinMode(19, OUTPUT);
 // digitalWrite(19,HIGH);

  Serial.begin(9600);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(Reset, INPUT_PULLUP);
  pinMode(yellowPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  key_init();

for(int i=0; i<6; i++) Serial.println(EEPROM.read(i)-48); 
                       Serial.println("==============================");
for(int i=6; i<12;i++) Serial.println(EEPROM.read(i)-48);
                        Serial.println("==============================");

  EEPROM.write(1020, 1);
 /* EEPROM.write(6, '1');
  EEPROM.write(7, '2');
  EEPROM.write(8, '3');
  EEPROM.write(9, '4');
  EEPROM.write(10,'5');
  EEPROM.write(11,'6');
  */
}
//---------------------------------------------------------
//-------------------------void-loop-----------------------
//---------------------------------------------------------
void loop() {

  estadoReset = digitalRead(Reset);
  if (estadoReset  == LOW) {
    Serial.println("Senha Resetada!");
    char pass [6] = {'1', '2', '3', '4', '5', '6'}; // Reset senha master
    for(int i=0; i<6;i++) {
     EEPROM.write(i, pass[i]);    
    }
    digitalWrite(D1, HIGH);
    delay(300);
  }
  else digitalWrite(D1, LOW);
 for(int i=0; i<6; i++) pass[i] = EEPROM.read(i); // Resgatar senha do Administrador (sem usar ID)

    char key = keypad.getKey(); // obtém informação do teclado
     

    if (key != NO_KEY) { // se foi teclado algo
      Serial.println(key);

     switch(key){

      case '#': login();  
        break; 
      case '*': alterar_senha();
        break; 
      case 'A': add_ID();
      
        break; 
    }
  }
}

//-------------------------------------------------------------      
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
  Led_state(); // mensagem, som e LED
  int entrada = 0; // inicializa entrada
  while (count < 6) { // enquanto contador for menor que 4
    char key = keypad.getKey(); // obtem informação do teclado
    if (key != NO_KEY) { // se algo foi teclado
      Serial.print('*');
      delay(duration);

      comparador1[count] = key;

      count += 1; // próximo dígito
    }
  }

  if (count == 6) {
    count = 0;
    
    Serial.println("\nDigite novamente: ");
    while (count < 6) { // enquanto contador for menor que 4
      char key = keypad.getKey(); // obtem informação do teclado
      if (key != NO_KEY) { // se algo foi teclado
        Serial.print('*');
        entrada += 1; // aumenta contador de entrada
        delay(duration);

        if (comparador1[count] == key) count += 1; // Conferi se é a mesma senha digitada

          if (count == 6){
              Serial.println("\nSenha salva com sucesso.");
              for(int i=0;i<6;i++) EEPROM.write(i, comparador1[i]); // salva a senha na memoria
      }
      
          if ((key == '*') || (entrada == 6)) { // foi telcado * 4 entradas  
              
              if(count != entrada) Serial.println("\nInvalido!.");
              delay(2000);
             // key_init() ; // inicializa sistema
              break; // sai
            }    
 
        }
      }
    }
  }

//---------------------------------------------------------

void Led_state(){
   count = 0;
   entrada = 0; // variável de apoio números de entradas feitas via teclado
  delay(duration);// somente LED amarelo aceso
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, HIGH);
  digitalWrite(D0, LOW);
}

//---------------------------------------------------------

void key_init(){
 
  Serial.println("\nLaboratorio");
 
  count = 0;
  digitalWrite(redPin, HIGH);
  digitalWrite(yellowPin, LOW);
  digitalWrite(D0, LOW);
  delay(duration);
}

//---------------------------------------------------------


void unlocked() {
  Serial.println("\nAcesso liberado!\nSenha correta!");
 
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, LOW);
  for (int x = 0; x < 5; x++) {
    digitalWrite(D0, HIGH);
    delay(duration);
    digitalWrite(D0, LOW);

  }
}

//------------------------------------------------------------

void login() {
        ID_login();
        delay(200);
        Serial.print("Digite sua senha: ");
        Led_state(); // mensagem, som e LED
        while (count < 6 ) { // enquanto não entrou os 4 números necessários para a senha
          char key = keypad.getKey(); // obtém informação do teclado
          if (key != NO_KEY) { // se foi teclado algo
            Serial.print('*');
            entrada += 1; // aumenta contrador de entrada
            delay(duration);
            //---------------------------------------------------------------

            pass[count] = EEPROM.read(count+(ID * 6)); // Resgata senha alterada

            //---------------------------------------------------------------
            if (key == pass[count])count += 1; // verifica na sequencia da senha, se correto aumenta contador
            if ( count == 6 ) {
              Serial.println("");
              unlocked(); // chegou a 4 digitos corretos, libera acesso
            }
            if (entrada != count && entrada == 6) {
              
              Serial.println("\nAcesso negado!\nSenha incorreta.");

            }

            if ((key == '#') || (entrada == 6)) { // foi teclado # ou 4 entradas incorretas
              key_init();// inicializa
              break;// interrompe loop
            }
          }
        }
      }

 //--------------------------------------------------------------------------

 void alterar_senha(){
  
      Serial.println("Digite a senha antiga: ");
      Led_state();;// mensagem para entrar a senha antiga
      
      int entrada = 0;
      while (count < 6 ) {
        char key = keypad.getKey();
        if (key != NO_KEY) {
          entrada += 1;
          Serial.print('*');
          delay(duration);
          if (key == pass[count]) count += 1;
          if ( count == 6 ) { // foi teclado a senha antiga corretamente
         
            get_new_pass();// chama função para entrada da nova senha
        
          }
          if ((key == '*') || (entrada == 6)) { // foi teclado * ou entrou 4 números errados
                      key_init();// inicializa
            break; // interrompe loop
          }
        }
      }
    }

 //-----------------------------------------------------------------------]

 void add_ID(){
  
      Serial.println("Criando novo usuario. ");
      Serial.println("Senha do administrador:" );
      Led_state();;// mensagem para entrar a senha antiga
      
      int entrada = 0;
      while (count < 6 ) {
        char key = keypad.getKey();
        if (key != NO_KEY) {
          entrada += 1;
          Serial.print('*');
          delay(duration);
          if (key == pass[count]) count += 1;
          if ( count == 6 ) { // foi teclado a senha antiga corretamente
         
           ID_senha();
        
          }
          if ((key == 'A') || (entrada == 6)) { // foi teclado * ou entrou 4 números errados
                      key_init();// inicializa
            break; // interrompe loop
          }
        }
      }
    }

 //-----------------------------------------------------------------------]
void ID_senha(){

  Led_state();
  Serial.println("\nNova senha: ");
  //Led_state(); // mensagem, som e LED
  int entrada = 0; // inicializa entrada
  while (count < 6) { // enquanto contador for menor que 4
    char key = keypad.getKey(); // obtem informação do teclado
    if (key != NO_KEY) { // se algo foi teclado
      Serial.print('*');
      delay(duration);

      comparador1[count] = key;

      count += 1; // próximo dígito
    }
  }

  if (count == 6) {
    count_ID = EEPROM.read(1020);
    count = 0;
    Serial.println("\nDigite novamente: ");
    while (count < 6) { // enquanto contador for menor que 6
      char key = keypad.getKey(); // obtem informação do teclado
      if (key != NO_KEY) { // se algo foi teclado
        Serial.print('*');
        entrada += 1; // aumenta contador de entrada
        delay(duration);

        if (comparador1[count] == key) count += 1; // Conferi se é a mesma senha digitada

          if (count == 6){
              Serial.println("\nSenha salva com sucesso.");
              for(int i=0;i<6;i++) EEPROM.write(i + (count_ID * 6), comparador1[i]); // salva a senha do ID na memoria
    
     Serial.print("ID: ");
     Serial.println(count_ID);
      count_ID += 1; 
      delay(duration);
      EEPROM.write(1020, count_ID);  
      Serial.print("ID: ");
      
      }
          
          if ((key == 'A') || (entrada == 6)) { // foi telcado * 6 entradas  
              ID=0;
              if(count != entrada) Serial.println("\nInvalido!.");
              delay(2000);
             // key_init() ; // inicializ a sistema
              break; // sai
            }    
 
        }
      }
    }
}

//-----------------------------------------------------------------------

void ID_login(){
        Serial.print("ID: ");
        count = 0;
        while (count < 2 ) { // espera ID de 0 a 99
        char key = keypad.getKey();
        if (key != NO_KEY) {
          id[count] = key;
          count += 1;
        
        }
      }
      ID = atoi(id);
      Serial.println(ID);
    }
    
