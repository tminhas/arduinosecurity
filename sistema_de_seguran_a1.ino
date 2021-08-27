#include <LiquidCrystal.h>

#define contagem_1s 62500     // PRSC em 256
#define DUTY_CYCLE(duty) (duty / 100.0 * contagem_1s) 
#define sensor 10
#define ledverde 7
#define ledvermelho 6
#define botaodir 3
#define zona_1 8
#define zona_2 9
#define zona_3 10
#define zona_4 11


LiquidCrystal lcd(14, 15, 16, 17, 18, 19);
unsigned long int password = 123456;   // Senha pré-configurada do sistema, pode ter até 9 dígitos
volatile bool alarme = false;          // Caso aconteça movimento alarme torna-se true
volatile bool button = false;		   // Caso acionado o botão, torna-se true

//Variáveis que definem estado da zona do alarme
volatile bool ativada_1 = false;
volatile bool ativada_2 = false;
volatile bool ativada_3 = false;
volatile bool ativada_4 = false; 
volatile int modo = 0;

//Rotina utilizada para validar o acesso ao sitema, pedindo para que o usuário insira a senha pré-configurada
//Se a senha for incorreta, a função avisa e permite nova tentativa
//Se a senha for incorreta por duas vezes, a função retorna false, bloquando o sistema
bool verifica_senha(){
  Serial.print("Insira a senha para validar o acesso: \n");
  //////////////////////////
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("INSIRA A SENHA");
  //////////////////////////
  char caractere_recebido;
  char msg[] = "Senha incompleta, informe o proximo digito: "; 
  long int senha = 0;
  int numero_caracteres = 0;
  int wrong = 0;
  while(password != senha){
     if ( UCSR0A & ( 1 << RXC0 ) ){   // Condição equivalente a " if ( Serial.available() ) "
        caractere_recebido = UDR0 ;
        int unidade = caractere_recebido - 48; //Pega um caractere ascII e transforma em numerico
       //Só adiciona o numero de caracteres recebido se fore um numero entre zero e nove
       if(unidade >= 0 && unidade <=9){
        senha = senha*10 + unidade;
        delay(50);
       
        Serial.print( msg );
        delay(50);						//delay para dar tempo no monitor serial 

        UDR0 = caractere_recebido;   // Envia de volta caractere recebido
        delay(1);                    // Aguarda fim do envio do caractere (aprox. 1 ms) 

        Serial.println();
        delay(2);
       numero_caracteres++;
       
       //////////////////
       lcd.setCursor(numero_caracteres, 1);
       lcd.print("*");
       //////////////////
	}
       
           if (numero_caracteres >= size_of(password) && password != senha){
             wrong++;
             	if (wrong == 2){
             	return false; //Se informado duas senhas incorretas, retorna falso
           		}
             ///////////////////
             lcd.clear();
             lcd.setCursor(0, 0);
             lcd.print("SENHA INCORRETA");
             lcd.setCursor(0, 1);
             lcd.print("TENTE NOVAMENTE");
             delay(2000);
             lcd.clear();
  	         lcd.setCursor(0, 0);
  	         lcd.print("INSIRA A SENHA");
             ///////////////////
             Serial.print("Senha incorreta, tente novamente: \n");
             senha = 0;
             numero_caracteres = 0;
           }
           
      }
     
   }
  
  Serial.println(senha);
  return true;
}

//Rotina utilizada para que o usuário possa alterar a senha do sistema
//Ela sobrescreve a senha pré-configurada pela senha inserida pelo usuário

void nova_senha(){
  ///////////////////
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("INSIRA A SENHA");
  lcd.setCursor(0, 1);
  lcd.print("DESEJADA");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AO FINALIZAR");
  lcd.setCursor(0, 1);
  lcd.print("AGUARDE 5seg");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("NOVA SENHA:");
  delay(1000);
  ///////////////////
  char caractere_recebido; 
  long int senha = 0;
  int numero_caracteres = 0;
  int wrong = 0;
  long int tempo = millis();
  //Permite salvar os caracteres recebidos com um intervalo menos que 5segundos
    while(millis() - tempo < 5000 && numero_caracteres < 9){
      if ( UCSR0A & ( 1 << RXC0 ) ){   // Condição equivalente a " if ( Serial.available() ) "
            caractere_recebido = UDR0 ;
            int unidade = caractere_recebido - 48; //Pega um caractere ascII e transforma em numerico
            if(unidade >= 0 && unidade <=9){
              senha = senha*10 + unidade;
              delay(50);
              UDR0 = caractere_recebido;
              delay(50);
              Serial.println();
              delay(2);
             numero_caracteres++;
             //////////////////
             lcd.setCursor(numero_caracteres, 1);
             lcd.print("*");
             //////////////////
           }
      tempo = millis();
    }

    }
  //Só altera password se for inserido uma nova senha
  if(numero_caracteres != 0){
  	password = senha;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SENHA ALTERADA");
    lcd.setCursor(0, 1);
    lcd.print("COM SUCESSO");
    delay(2000);
  }
  else{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SENHA NAO ");
    lcd.setCursor(0, 1);
    lcd.print("ALTERADA");
    delay(2000);
  }
  
  
}


//rotina utilizada para habilitar a interrupção por mudança de estado
//altera o estado da zona conforme retorno da função habilitar()
void ativa_zona(){
  if(habilitar(1)){
    ativada_1 = true; 
    PCMSK0 |= ( 1 << PCINT0);
  }
  else{
	ativada_1 = false;
    PCMSK0 |= ( 0 << PCINT0);
  }
  if(habilitar(2)){
    ativada_2 = true; 
    PCMSK0 |= ( 1 << PCINT1);
  }
  else{
    ativada_2 = false;
    PCMSK0 |= ( 0 << PCINT1);
  }
  if(habilitar(3)){
    ativada_3 = true;
    PCMSK0 |= ( 1 << PCINT2);
  }
  else{
    ativada_3 = false;
    PCMSK0 |= ( 0 << PCINT2);
  }
  if(habilitar(4)){
    ativada_4 = true;
    PCMSK0 |= ( 1 << PCINT3);
  }
  else{
    ativada_4 = false;
    PCMSK0 |= ( 0 << PCINT3);
  }
}

//Insira 1 para habilitar a zona
//Insira 0 para desabilitar a zona
//Qualquer outro caractere, nada acontece
bool habilitar(int zona){
 lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("HABILITAR ZONA ");
  lcd.setCursor(15,0);
  lcd.print(zona);
  while(true){
    if ( UCSR0A & ( 1 << RXC0 ) ){   // Condição equivalente a " if ( Serial.available() ) "
          delay(50);
      	  char caractere_lido = UDR0;
          if (caractere_lido == 49){
            lcd.setCursor(0, 1);
            lcd.print("HABILITADA");
            delay(2000);
            return true;
          }
            else if(caractere_lido == 48){
            lcd.setCursor(0, 1);
            lcd.print("DESABILITADA");
            delay(2000);
            return false;
          }
      }
  }
  
}

//rotina que retorna o numero de digitos da variavel password
int size_of(unsigned long int auxiliar){
  int password_carac = 0;
  while(auxiliar > 0){
  auxiliar = auxiliar/10;
  password_carac++;
  }
  if (auxiliar == 0)
    password_carac++;
  return password_carac;
}


void setup()
{ //habilita o lcd
  lcd.begin(16, 2);
  lcd.clear();
  //status do sistema
  bool status = false; 
  // Configurando as entradas e saidas
  pinMode(sensor, INPUT);
  pinMode(ledverde, OUTPUT);
  pinMode(ledvermelho, OUTPUT);
  pinMode(botaodir, INPUT);
  
  // Configurando serial
  UBRR0 = 103;                // UBRR0 = f_osc/(16 * BAUD) - 1
  UCSR0A = 0; 
  UCSR0B =  ( 1 << RXEN0 ) |
            ( 1 << TXEN0 );    // RX e TX habilitado. 
  UCSR0C =  ( 1 << UCSZ01 ) |  
            ( 1 << UCSZ00 );   // 8 bits, sem paridade}
  
  // Configurando timer da interrupção
  TCCR1A = 0; 
  TCCR1B  = (1 << CS12);   // Prescaler em 256
  TCCR1B |= (1 << WGM12);  // Modo CTC
  
  OCR1A   = contagem_1s - 1; 
  OCR1B   = DUTY_CYCLE (50) - 1; 

  TIMSK1 = (1 << OCIE1A) | (1 << OCIE1B);
  
  TCNT1 = 0; 
  
  Serial.println("Iniciando o sistema.");
  //////////////////////////////////////
  lcd.setCursor(0, 0);
  lcd.print("INICIANDO O");
  lcd.setCursor(0, 1);
  lcd.print("SISTEMA");
  for(int x = 0;x<3;x++){
    lcd.setCursor(6 + x, 1);
    lcd.print(".");
    delay(300);
  }
  //////////////////////////////////////
  delay(50);
  status = verifica_senha();
  if(!status){
    //////////////////
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ALARME BLOQUEADO");
    lcd.setCursor(0, 1);
    lcd.print("REINICIE");
    /////////////////
    Serial.print("Sistema bloqueado. Reinicie e tente novamente.");
    delay(100);
    while(!status); //entra nesse loop bloqueando o sistema
  } 
  //habilita interrupção por mudança de estado
  PCICR =  ( 1 << PCIE0);
  ativa_zona();
  
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SISTEMA ATIVADO");
  
  
  // Habilitando a interrupção 0 e 1 apenas após a validação do sistema
  EICRA = (1 << ISC01) | (1 << ISC00);
  EIMSK = (1 << INT0);
  
  EICRA |= (1 << ISC11) | (1 << ISC10);
  EIMSK |= (1 << INT1);
  
  
  
  
}


void loop()
{
  // Se o sistema está ativado, mas a variavel alarme == false
  if (!alarme){
  	digitalWrite(ledverde, HIGH);    
  }
  // Para alarme == true, desativa o led verde
  else{
    delay(2000); //delay para dar tempo de leitura no lcd
    digitalWrite(ledverde, LOW);
  }
  
  // É acionada apóes a interrupção 1 pelo push button
  // Se acionada uma vez, muda senha
  // Se acionada duas vezes em menos de 2 segundos, permite alterar as zonas
  if(button && !alarme){
    delay(2000);
    if(modo == 1){
      bool alterar_senha = false;
      alterar_senha = verifica_senha();
      if (alterar_senha){
        nova_senha();
        button = false;
        alterar_senha = false;
        modo = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ALARME ATIVADO");
      }
    }
    if(modo >= 2){
      if(verifica_senha()){
        ativa_zona();
        button = false;
        modo = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("SISTEMA ATIVADO");
      }
    }
  }
  
  // Com alarme == true, o usuário aperta o botão, gerando nova interrupção
  // Aqui o sistema pede para que seja confirmada a senha, e desliga o disparo do alarme
  while(alarme && button){
    
    if (verifica_senha()){
     button = false;
     alarme = false;
     Serial.println("Alarme desligado");
     ///////////////////
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("ALARME DESLIGADO");
     delay(2000);
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("SISTEMA ATIVADO");
     ///////////////////
     delay(50);
    }
    else
      button = false;
    modo = 0;  
  }
}

// Tratador de interrupção por mudança de estado
// Este tratador testa qual das entradas foi responsável pela interrupção
// Informando ao usuário qual das zonas foi responsável pelo acionamento do alarme
ISR(	PCINT0_vect	  ){
  alarme = true;
  if(ativada_1 && digitalRead(zona_1)){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ALARME DISPARADO");
      lcd.setCursor(0, 1);
      lcd.print("ZONA 1");
      
  }
  if(ativada_2 && digitalRead(zona_2)){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ALARME DISPARADO");
      lcd.setCursor(0, 1);
      lcd.print("ZONA 2");
      
  }
  if(ativada_3 && digitalRead(zona_3)){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ALARME DISPARADO");
      lcd.setCursor(0, 1);
      lcd.print("ZONA 3");
      
  }
  if(ativada_4 && digitalRead(zona_4)){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ALARME DISPARADO");
      lcd.setCursor(0, 1);
      lcd.print("ZONA 4");
      
  }
  delay(2000);
}


// Tratador da interrupção 0
// Esta interrupção é responsável pelo acionametno do alarme
// A zona principal não pode ser desabilitada, e é acionada por este tratador
ISR(INT0_vect){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ALARME DISPARADO");
  lcd.setCursor(0, 1);
  lcd.print("ZONA PRINCIPAL");
  alarme = true;
  
}


// Tratador da interrupção 1
// Esta interrupção é acionada pelo push button do sistema
// Se acionada uma vez, com a variável alarme = false, entra no modo alterar senha
// Se acionada duas vezes em menos de 2 segundos, entra no modo habilitar zona
ISR(INT1_vect){
  button = true;
  modo++;
}



//Interrupção por timer ctc
//controla o pulso do led vermelho para alto
ISR(TIMER1_COMPA_vect){
  if (alarme){
  	digitalWrite(ledvermelho, HIGH);
  }
  else{
  	digitalWrite(ledvermelho, LOW);
  }
}

//Interrupção por timer ctc
//controla o pulso do led vermelho para baixo
ISR(TIMER1_COMPB_vect){
  if (alarme){
   digitalWrite(ledvermelho, LOW); 
  }
}



