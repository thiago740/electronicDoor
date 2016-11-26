/*
    Concluido:
           - Programa como interface Serial monitor
           - Reset de senha
           - Login do administrador
           - Edição de senh
           - Criar e deleta novo ID.
           - LCD 
           - Buzzer
    
    Falta: 
           
           - Registrar Data e hora do acesso do Usuario
             - Comunicação ESP/Celular
*/
#include <Wire.h>
#include <Keypad.h>
#include <EEPROM.h>
#include "pitches.h"
#include <LiquidCrystal.h>


LiquidCrystal lcd(19, 18, 17, 16, 15, 14);

int IDs [20];
 
char id[2]; // recebe o ID pelo keypad.
char pass[6]; // número de caracters 
char comparador1[6];
char bluet;
int count = 0, entrada = 0, ID, count_ID = 0;
int D0;  // Tranca
int D1; 
int buzzer = 11;
int Reset; 
int estadoReset = 0;

int LCD = 10;
long previousMillis = 0;  // Variável de controle do tempo
long delayLCD = 3000;     // Tempo em ms do intervalo a ser executado

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
/*byte rowPins[ROWS] = {0, 2, 3, 4}; // pinagem para as linhas do teclado
byte colPins[COLS] = {5, 6, 7, 8}; // pinagem para as colunas do teclado*/
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


//---------------------------------------------------------

void setup() {

  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  digitalWrite(LCD, HIGH);
  pinMode(LCD, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  pinMode(Reset, INPUT_PULLUP);
  pinMode(yellowPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  key_init();
 

  
  for(int i=0; i<6; i++) Serial.println(EEPROM.read(i)-48); 
  Serial.println("==============================");

  for(int i=0; i<20; i++) Serial.println(EEPROM.read(900+i)); 
  Serial.println("==============================");
 
}

//-------------------------void-loop-----------------------
void led_LED(){
/*
  unsigned long currentMillis = millis();    //Tempo atual em ms

  //Lógica de verificação do tempo
  if (currentMillis - previousMillis > delayLCD) { 
    previousMillis = currentMillis;    // Salva o tempo atual

    //E aqui muda o estado do led
    //verificando como ele estava anteriormente
    if (LCD == LOW) {
       digitalWrite(LCD, HIGH);
    } else {
        digitalWrite(LCD, LOW);
    }


  }*/

}

void loop() {
  
  for(int i=0; i<6; i++) pass[i] = EEPROM.read(i); // Resgatar senha do Administrador (sem usar ID)
  for(int i=0; i<20; i++) IDs[i] = EEPROM.read(900+i);  // Os IDs estão salvos de 900 à 920 da memoria

   
   if (Serial.available()>0) { // se foi teclado algo
    bluet = Serial.read();
    Serial.println(bluet);
    
    switch(bluet){
      case '#': login();  
        break; 
      case '*': add_ID();
        break; 
      case 'D': del_ID();
        break; 
    }
  }



  
  estadoReset = digitalRead(Reset);

 
  char key = keypad.getKey(); // obtém informação do teclado

   if (key) tone(buzzer, NOTE_F5, 15);
  if (key != NO_KEY) { // se foi teclado algo
    Serial.println(key);
    tone(13, NOTE_C4, 100);
   
    
    switch(key){
      case '#': login();  
        break; 
      case '*': add_ID();
        break; 
      case 'D': del_ID();
        break; 
    }
  }
}


//------------------------------------------------------------

void login() {
  
  ID_login();
  if(IDs[ID] == 1 && ID < 20){
    
    delay(200);   
    Serial.print("Digite sua senha: ");
    lcd.setCursor(0,1);
    lcd.print("Senha:");
    
    Led_state(); // mensagem, som e LED
    while (count < 6 ) { // enquanto não entrou os 4 números necessários para a senha
      char key = keypad.getKey(); // obtém informação do teclado
      if (key) if (key) tone(buzzer, NOTE_F5, 15);
      if (key != NO_KEY) { // se foi teclado algo
        Serial.print('*');
        lcd.print('*');
        entrada += 1; // aumenta contrador de entrada
        //delay(duration);
      
      //--------------- Argumento -----------------------------------------

        pass[count] = EEPROM.read(count+(ID * 6)); // Resgata senha alterada

      //-------------------------------------------------------------------
      
        if (key == pass[count])count += 1; // verifica na sequencia da senha, se correto aumenta contador
        if ( count == 6 ) {
          Serial.println("");
          unlocked(); // chegou a 4 digitos corretos, libera acesso
        }
        if (entrada != count && entrada == 6) {
          Serial.println("\nAcesso Recusado!\nSenha incorreta.");
          lcd.clear();
          lcd.print("Acesso Recusado!");
          delay(1500);
        }
        
        if ((key == '#') || (entrada == 6)) { // foi teclado # ou 4 entradas incorretas
          key_init();// inicializa
          break;// interrompe loop
        }
      }
    }
  }
  else{
    Serial.println("ID inexistente!");
    lcd.setCursor(0,1);
    lcd.print("!ID inexistente!"); 
     
    delay(1500); 
    key_init(); 
  }
}

 //--------------------------------------------------------------------------

void add_ID(){
  
  Serial.println("Senha do administrador: ");
  lcd.clear();
  lcd.print("    Criar ID    ");
  lcd.setCursor(0, 1);
  lcd.print("ADMIN:");
  lcd.setCursor(6,1);
  

  
  Led_state();// mensagem para entrar a senha
  int entrada = 0;
  while (count < 6 ) {
    char key = keypad.getKey();
    if (key) if (key) tone(buzzer, NOTE_F5, 15);
    if (key != NO_KEY) {
      entrada += 1;
      Serial.print('*');
      lcd.print('*');
      //delay(duration);
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

//---------------------------------------------------------

void get_new_pass() {
     
  ID_login(); // Recebe oendereço na memoria
  if( (IDs[ID] == 0 && ID < 20) || ID == 0){ 
    
    delay(100);
    Serial.println("\nNova senha: ");
    lcd.clear();
    lcd.print("Senha:");
    lcd.setCursor(6,0);

    
    Led_state(); // mensagem, som e LED
    int entrada = 0; // inicializa entrada
      while (count < 6) { // enquanto contador for menor que 4 
        char key = keypad.getKey(); // obtem informação do teclado
          if (key) if (key) tone(buzzer, NOTE_F5, 15);
          if (key != NO_KEY) { // se algo foi teclado
          Serial.print('*');
          lcd.print('*');
          //delay(duration);

          comparador1[count] = key;
          count += 1; // próximo dígito
          }
      }

      if (count == 6) {

      count = 0;
      
      Serial.println("\nDigite novamente: ");
      lcd.setCursor(0,1);
      lcd.print("Senha:");
      lcd.setCursor(6,1);

      
        while (count < 6) { // enquanto contador for menor que 4
        char key = keypad.getKey(); // obtem informação do teclado
          if (key) if (key) tone(buzzer, NOTE_F5, 15);
          if (key != NO_KEY) { // se algo foi teclado
            
            Serial.print('*');
            lcd.print('*');
            entrada += 1; // aumenta contador de entrada
            //delay(duration);
      
            if (comparador1[count] == key) count += 1; // Conferi se é a mesma senha digitada
            if (count == 6){
              
    
           //--------------- SubArgumento -----------------------------
             Serial.println("\nSenha salva com sucesso.");
             Serial.print("\nID: ");
             Serial.println(ID);

             lcd.clear();
             lcd.print("  Senha salva!  ");
             lcd.setCursor(0,1);
             lcd.print("ID:");
             lcd.setCursor(3,1);
             lcd.print(ID);
        
             for(int i=0;i<6;i++) EEPROM.write(i+(ID*6), comparador1[i]); // salva a senha na memoria
 
             EEPROM.write(900+ID, 1); // Ativa o ID como existente
             delay(1500); 

          //------------------------------------------------------
 
           }
      
           if ((key == '*') || (entrada == 6)) { // foi telcado * 4 entradas  
            if(count != entrada){
              Serial.println("\nInvalido!.");
              lcd.clear();
              lcd.print("   !Invalido!   ");
              delay(1000); 
            }    
            key_init() ; // inicializa sistema
            break; // sai
          }    
        }
      }
    }
  }
  else{
    Serial.println("ID INVALIDO!");  
    lcd.clear(); 
    lcd.print("  ID INVALIDO!  "); 
    delay(1500); 
    key_init(); 
  }
}


//-----------------------------------------------------------------------
void del_ID(){

  lcd.clear();
  lcd.print("     Del ID     ");
  lcd.setCursor(0, 1);
  lcd.print("ADMIN:");
  lcd.setCursor(6,1);
  
  Serial.println("\nDel ID");
  Serial.println("\nSenha do administrador: ");
  
  Led_state();// mensagem para entrar a senha
  int entrada = 0;
  while (count < 6 ) {
    char key = keypad.getKey();
    if (key) if (key) tone(buzzer, NOTE_F5, 15);
    if (key != NO_KEY) {
      entrada += 1;
      Serial.print('*');
      lcd.print('*');
      //delay(duration);
      if (key == pass[count]) count += 1;
      if ( count == 6 ) { // foi teclado a senha do Administrador
        
        //-------------    Argumento   -------------------
        
        ID_login(); // Recebe endereço na memoria
   
        Serial.println("\nSenha do administrador: ");       // Pedir senha do Administrados para confirmar explusão
        lcd.setCursor(0, 1);
        lcd.print("ADMIN:");
        lcd.setCursor(6,1);
        
        Led_state();// mensagem para entrar a senha
        int entrada = 0;
        while (count < 6 ) {
          char key = keypad.getKey();
          if (key) tone(buzzer, NOTE_F5, 15);
          if (key != NO_KEY) {
          entrada += 1;
            if (key) tone(buzzer, NOTE_F5, 15);
            Serial.print('*');
            lcd.print('*');
            //delay(duration);
            if (key == pass[count]) count += 1;
            if ( count == 6 ) { // foi teclado a senha do Administrador
        
             if( IDs[ID] == 1 || ID > 20){
             
             EEPROM.write(900+ID, 0); // Ativa o ID como existente
             
             Serial.print("\nSenha deletada com sucesso!");
             lcd.clear();
             lcd.print("  ID DELETADO!  ");
             delay(2000); 
             }
             else{
              Serial.println("\nID Invalido!");
              lcd.clear();
              lcd.print("  ID Invalido!  ");
              delay(1000); 
             }
             }
             if ((key == 'D') || (entrada == 6)) { // foi teclado * ou entrou 4 números errados
              key_init();// inicializa
              break; // interrompe loop
            }
        }
     }
   }
       
      //---------------------------------------------------   

         
      }
      if ((key == 'D') || (entrada == 6)) { // foi teclado * ou entrou 4 números errados
        key_init();// inicializa
        break; // interrompe loop
        }
      }
    }
    


//-----------------------------------------------------------------------

void ID_login(){
        
        Serial.print("\nID: ");
        lcd.clear();
        lcd.home();
        lcd.print("ID:");
        lcd.setCursor(3, 0); 
        
        count = 0;
        while (count < 2 ) { // espera ID de 0 a 99
        char key = keypad.getKey();
        if (key) tone(buzzer, NOTE_F5, 15);
        if (key != NO_KEY) {
          id[count] = key;
          count += 1;
         
          Serial.print(key);     
          lcd.print(key);
          
        
        }
      }

      ID = atoi(id);
      Serial.println("");
    }

//-----------------------------------------------------------------------
void admin_Reset(){
  char pass [] = {'1', '2', '3', '4', '5', '6'};
  
  if (estadoReset  == LOW) {
    //IDs_Reset();
    Serial.println("Senha Resetada!");
    char pass [6] = {'1', '2', '3', '4', '5', '6'}; // Reset senha master
    for(int i=0; i<6;i++) {
     EEPROM.write(i, pass[i]);    
    }
    
    digitalWrite(D1, HIGH);
    delay(300);
  }
  else digitalWrite(D1, LOW);
}

//-----------------------------------------------------------------------

void IDs_Reset(){
 // int IDs_Rest [20]= {1, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
    for(int i=1; i<20;i++) EEPROM.write(i+900, 0);
    EEPROM.write(900, 1);
  
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
  lcd_inicial();

  count = 0;
  digitalWrite(redPin, HIGH);
  digitalWrite(yellowPin, LOW);
  digitalWrite(D0, LOW);
  delay(duration);
}

//---------------------------------------------------------


void unlocked() {
  Serial.println("\nAcesso liberado!\nSenha correta!");
  LCD_unlocked();
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
void LCD_unlocked(){
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Acesso liberado!");
  lcd.setCursor(0, 1);
  lcd.print("<<<<<<<<<<<<<<<<");
  delay(1500);
  
  
}
/*
void IDs_Disponiveis(){
  lcd.clear();
  delay(200);
  Serial.println("ID disponivel:");
  lcd.setCursor(0, 2);
  lcd.print("ID disponivel:");
  for(int i=0; i<200; i++) IDs[i] = EEPROM.read(900+1);
  for(int x=0; x<200; x++){
    if(IDs[x] == 1 ){
      Serial.print("ID ");
      Serial.println(x);
      lcd.setCursor(1, 5);
      lcd.print("ID :");
      lcd.setCursor(1, 9);
      lcd.print(x);

      delay(5000);
    }
  }
}
*/
