#include <Wire.h>
#include <EEPROM.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(19, 18, 17, 16, 15, 14);

int count = 0;
int addr = 0;
char pass [4] = {'1', '2', '3', '3'}; // senha padrão, você pode muito bem atera-lá
char comparador1[4], comparador2[4];
int ID, memoria;
int D0 = 12;  // Tranca
int D1 = 10;
//int ton = 9;
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
byte rowPins[ROWS] = {5, 4, 3, 2}; // pinagem para as linhas do teclado
byte colPins[COLS] = {6, 7, 8, 9}; // pinagem para as colunas do teclado
// mapeamento do teclado
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );




//---------------------------------------------------------




void setup() {


  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(Botao, INPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  key_init();

  
// Imprime a ulmima senha salva  48 = 0, 65 = A.

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
    if (key) Serial.println(key);

if (key != NO_KEY) { // se foi teclado algo
  switch(key){
  ID = Serial.read();
    case '#':
  
   
     /* if(ID){
       // Serial.print("ID: ");
       // Serial.println(ID);
       // delay(500);*/
        code_entry_init(); // mensagem, som e LED
        int entrada = 0; // variável de apoio; números de entradas feitas via teclado
        lcd.setCursor(0, 1);// Posição da senha 
        while (count < 4 ) { // enquanto não entrou os 4 números necessários para a senha
          char key = keypad.getKey(); // obtém informação do teclado
         
        
          if (key != NO_KEY) { // se foi teclado algo 
            lcd.print('*'); // Senha ni lcd
            Serial.print('*');
            entrada += 1; // aumenta contrador de entrada
            delay(duration);
            //---------------------------------------------------------------

            pass[count] = EEPROM.read(memoria); // Resgata senha alterada

            //---------------------------------------------------------------
           /* if (key == pass[count]){
              memoria += 1 + (ID * 6); // verifica na sequencia da senha, se correto aumenta contador
             */
              count=+1;
              if ( count == 4 ) {
              
                Serial.println("");
                unlocked(); // chegou a 4 digitos corretos, libera acesso
              }
            }
            if (entrada == 4  && entrada != count) {
               senha_errada();
            }
            
            if ((key == '#') || (entrada == 4)) { // foi teclado # ou 4 entradas incorretas
              key_init();// inicializa
              break;// interrompe loop
            }
            }
          }
        }
      }
    }      
 // }

 /* 
    case 'A':
        lcd_USnovo();
        int entrada = 0;
        
        
    
    
    
      }
    } 
  }    
}
*/

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

  lcd.clear();
  lcd.print("Nova senha: ");
  Serial.println("\nNova senha: ");
  lcd.setCursor(0,1);
  new_pass_entry(); // mensagem, som e LED
  int entrada = 0; // inicializa entrada
  while (count < 4) { // enquanto contador for menor que 4
    char key = keypad.getKey(); // obtem informação do teclado
    if (key != NO_KEY) { // se algo foi 
      lcd.print('*');
      Serial.print('*');
      delay(duration);


      comparador1[count] = key;


      count += 1; // próximo dígito
    }
  }


  if (count == 4) {
    count = 0;
    lcd.clear();
    lcd.print("Digite novamente: ");
    Serial.println("");
    lcd.setCursor(0,1);
    Serial.println("\nDigite novamente: ");
    while (count < 4) { // enquanto contador for menor que 4
      char key = keypad.getKey(); // obtem informação do teclado
      if (key != NO_KEY) { // se algo foi teclado
        lcd.print('*');
        Serial.print('*');
        entrada += 1; // aumenta contador de entrada
        delay(duration);


        if (comparador1[count] == key) { // Conferi se é a mesma senha digitada


          EEPROM.write(count, key); // salva a senha na memoria
          count += 1;


          if (count == 4) lcd_Salvo(); 
          if (count == 4 && entrada != 4) Serial.println("\nInvalido!.");
          if ((key == '*') || (entrada == 4)) { // foi telcado * 4 entradas
            key_init() ; // inicializa sistema
            break; // sai
          }
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

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Digite sua senha: ");
  Serial.println("Digite a senha: ");
  count = 0;
  delay(duration);// somente LED amarelo aceso
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, HIGH);
  digitalWrite(D0, LOW);
}




//---------------------------------------------------------




void key_init() {
  

  Serial.println("Laboratorio");
  count = 0;
  digitalWrite(redPin, HIGH);
  digitalWrite(yellowPin, LOW);
  digitalWrite(D0, LOW);
  delay(duration);
  lcd_inicial();
}




//---------------------------------------------------------




void code_entry_init() {

  lcd_senha();
  Serial.print("Digite sua senha: ");
  count = 0; // contador para zero
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, HIGH);
  digitalWrite(D0, LOW);
}




//---------------------------------------------------------




void unlocked() {



  Serial.println("Acesso liberado!\n Senha correta!");
  senha_correta();
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, LOW);
  for (int x = 0; x < 5; x++) {
    digitalWrite(D0, HIGH);
    delay(duration);
    digitalWrite(D0, LOW);




  }
}
//==============================================================================
//===============================      lcd     =================================
//==============================================================================
void lcd_inicial(){
  
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("LABORATORIO");
  lcd.setCursor(1, 1);
  lcd.print("IFRN PARNAMIRIM");
  }

void lcd_senha(){

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Digite sua senha: ");
  
  }
void lcd_Salvo(){
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Senha salva!");
  lcd.setCursor(2, 1);
  lcd.print("Com sucesso!");
  Serial.println("\nSenha salva com sucesso.");

  delay(2000);
  
  lcd_inicial();
}

void senha_correta(){
  
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Liberado!");
  lcd.setCursor(0,1);
  lcd.print("<<<<<<<<<<<<<<<<");
  
  delay(2000);
  
  lcd_inicial();
  
  }


void senha_errada(){
               lcd.clear();
               delay(100);
              lcd.setCursor(1, 0);
              lcd.print("Acesso negado!");
              lcd.setCursor(0, 1);  
              lcd.print("Senha incorreta!");
              Serial.println("Acesso negado!\n Senha incorreta.");

                delay(3000);
                lcd_inicial();
  }
void lcd_USnovo(){
  Serial.println("Criar novo usuario");
  lcd.clear();
  lcd.print("Criar novo usuario");
  }
// --------------------------------------------------------





