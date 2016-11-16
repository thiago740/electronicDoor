/*
    Concluido:
           - Programa como interface Serial monitor
           - Reset de senha
           - Login do administrador
           - Edição de senh
           - Criar e deleta novo ID. 
    
    Falta: 
           - LCD
           - Registrar Data e hora do acesso do Usuario
           - Comunicação ESP/Celular
*/

#include <Keypad.h>
#include <Wire.h>
#include <EEPROM.h>



int IDs [20];
 
char id[2]; // recebe o ID pelo keypad.
char pass[6]; // número de caracters 
char comparador1[6];
int count = 0, entrada = 0, ID, count_ID = 0;
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

  Serial.begin(9600);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
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

void loop() {
  
  for(int i=0; i<6; i++) pass[i] = EEPROM.read(i); // Resgatar senha do Administrador (sem usar ID)
  for(int i=0; i<20; i++) IDs[i] = EEPROM.read(900+i);  // Os IDs estão salvos de 900 à 920 da memoria
 
  
  estadoReset = digitalRead(Reset);
  admin_Reset();
 
  char key = keypad.getKey(); // obtém informação do teclado
  if (key != NO_KEY) { // se foi teclado algo
    Serial.println(key);
    
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
    Led_state(); // mensagem, som e LED
    while (count < 6 ) { // enquanto não entrou os 4 números necessários para a senha
      char key = keypad.getKey(); // obtém informação do teclado
      if (key != NO_KEY) { // se foi teclado algo
        Serial.print('*');
        entrada += 1; // aumenta contrador de entrada
        delay(duration);
      
      //--------------- Argumento -----------------------------------------

        pass[count] = EEPROM.read(count+(ID * 6)); // Resgata senha alterada

      //-------------------------------------------------------------------
      
        if (key == pass[count])count += 1; // verifica na sequencia da senha, se correto aumenta contador
        if ( count == 6 ) {
          Serial.println("");
          unlocked(); // chegou a 4 digitos corretos, libera acesso
        }
        if (entrada != count && entrada == 6) Serial.println("\nAcesso negado!\nSenha incorreta.");
        
        if ((key == '#') || (entrada == 6)) { // foi teclado # ou 4 entradas incorretas
          key_init();// inicializa
          break;// interrompe loop
        }
      }
    }
  }
  else{
    Serial.println("ID inexistente!");   
    key_init(); 
  }
}

 //--------------------------------------------------------------------------

void add_ID(){
  
  Serial.println("Senha do administrador: ");
  Led_state();// mensagem para entrar a senha
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

//---------------------------------------------------------

void get_new_pass() {
     
  ID_login(); // Recebe oendereço na memoria
  if( (IDs[ID] == 0 && ID < 20) || ID == 0){ 
    delay(200);
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
              
    
           //--------------- SubArgumento -----------------------------
             Serial.println("\nSenha salva com sucesso.");
             Serial.print("\nID: ");
             Serial.println(ID);
             for(int i=0;i<6;i++) EEPROM.write(i+(ID*6), comparador1[i]); // salva a senha na memoria

          // IDs[ID] = 1;  // Ativa o ID como existente 

             EEPROM.write(900+ID, 1); // Ativa o ID como existente 

          //------------------------------------------------------
 
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
  else{
    Serial.println("ID INVALIDO!");   
    key_init(); 
  }
}


//-----------------------------------------------------------------------
void del_ID(){

  Serial.println("\nDel ID");
  Serial.println("\nSenha do administrador: ");
  Led_state();// mensagem para entrar a senha
  int entrada = 0;
  while (count < 6 ) {
    char key = keypad.getKey();
    if (key != NO_KEY) {
      entrada += 1;
      Serial.print('*');
      delay(duration);
      if (key == pass[count]) count += 1;
      if ( count == 6 ) { // foi teclado a senha do Administrador
        
        //-------------    Argumento   -------------------
        ID_login(); // Recebe endereço na memoria
        
        if( IDs[ID] == 1 || ID > 20){
          EEPROM.write(900+ID, 0); // Ativa o ID como existente 
          Serial.print("Senha deletada com sucesso!");
        }
        else Serial.println("ID inexistente!");
      
      //---------------------------------------------------      
      }
      if ((key == 'D') || (entrada == 6)) { // foi teclado * ou entrou 4 números errados
        key_init();// inicializa
        break; // interrompe loop
        }
      }
    }
  }  


//-----------------------------------------------------------------------

void ID_login(){
        Serial.print("\nID: ");
        count = 0;
        while (count < 2 ) { // espera ID de 0 a 99
        char key = keypad.getKey();
        if (key != NO_KEY) {
          id[count] = key;
          count += 1;
        Serial.print(key);
        }
      }

      ID = atoi(id);
      Serial.println("");
    }

//-----------------------------------------------------------------------
void admin_Reset(){
  char pass [] = {'1', '2', '3', '4', '5', '6'};
  
  if (estadoReset  == LOW) {
    IDs_Reset();
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

