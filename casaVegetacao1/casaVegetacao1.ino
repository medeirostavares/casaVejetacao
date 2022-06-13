#include <ThreeWire.h> //INCLUSÃO DA BIBLIOTECA
#include <RtcDS1302.h> //INCLUSÃO DA BIBLIOTECA

ThreeWire myWire(4, 5, 2); //OBJETO DO TIPO ThreeWire
RtcDS1302<ThreeWire> Rtc(myWire); //OBJETO DO TIPO RtcDS1302

//PERÍODO DE ATIVIDADE DO SISTEMA:
int horaAtivo = 9; //9
int horaInativo = 17; //17

#define BUZZ 3
#define LED 6

unsigned long millisAtual;

//ATIVIDADE DA BOMBA ASPERSORA:
#define RL2 9 
unsigned long intervaloAspercao = 1200000; //20MIN(1200000)
unsigned long tempoAspercao = 60000; //1MIN(60000)
unsigned long millisAnteriorLigASP = NULL;
unsigned long millisAtualDeslASP = NULL;
unsigned long millisAnteriorDeslASP = NULL;

//ATIVIDADE DA BOMBA GOTEJADORA:
#define RL3 10 
long intervaloGotejamento = 1800000; //30MIN(1800000)
long tempoGotejamento = 300000; //5MIN(300000)
long millisAnteriorLigGOT = NULL; 
long millisAtualDeslGOT = NULL;
long millisAnteriorDeslGOT = NULL;

void setup () 
{
    Serial.begin(115200); //INICIALIZA A SERIAL
    pinMode(RL2, OUTPUT);
    digitalWrite(RL2, HIGH);
    pinMode(RL3, OUTPUT);
    digitalWrite(RL3, HIGH);
    pinMode(BUZZ, OUTPUT);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    Rtc.Begin(); //INICIALIZA O RTC
    Serial.print("Compilado em: "); //IMPRIME O TEXTO NO MONITOR SERIAL
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__); //VARIÁVEL RECEBE DATA E HORA DE COMPILAÇÃO
    printDateTime(compiled); //PASSA OS PARÂMETROS PARA A FUNÇÃO printDateTime
    Serial.println(); //QUEBRA DE LINHA NA SERIAL
    Serial.println(); //QUEBRA DE LINHA NA SERIAL

    if(Rtc.GetIsWriteProtected()){ //SE O RTC ESTIVER PROTEGIDO CONTRA GRAVAÇÃO, FAZ
        Serial.println("RTC está protegido contra gravação. Habilitando a gravação agora..."); //IMPRIME O TEXTO NO MONITOR SERIAL
        Rtc.SetIsWriteProtected(false); //HABILITA GRAVAÇÃO NO RTC
        Serial.println(); //QUEBRA DE LINHA NA SERIAL
    }

    if(!Rtc.GetIsRunning()){ //SE RTC NÃO ESTIVER SENDO EXECUTADO, FAZ
        Serial.println("RTC não está funcionando de forma contínua. Iniciando agora..."); //IMPRIME O TEXTO NO MONITOR SERIAL
        Rtc.SetIsRunning(true); //INICIALIZA O RTC
        Serial.println(); //QUEBRA DE LINHA NA SERIAL
    }

    RtcDateTime now = Rtc.GetDateTime(); //VARIÁVEL RECEBE INFORMAÇÕES
    if (now < compiled) { //SE A INFORMAÇÃO REGISTRADA FOR MENOR QUE A INFORMAÇÃO COMPILADA, FAZ
        Serial.println("As informações atuais do RTC estão desatualizadas. Atualizando informações..."); //IMPRIME O TEXTO NO MONITOR SERIAL
        Rtc.SetDateTime(compiled); //INFORMAÇÕES COMPILADAS SUBSTITUEM AS INFORMAÇÕES ANTERIORES
        Serial.println(); //QUEBRA DE LINHA NA SERIAL
    }
    else if (now > compiled){ //SENÃO, SE A INFORMAÇÃO REGISTRADA FOR MAIOR QUE A INFORMAÇÃO COMPILADA, FAZ
        Serial.println("As informações atuais do RTC são mais recentes que as de compilação. Isso é o esperado."); //IMPRIME O TEXTO NO MONITOR SERIAL
        Serial.println(); //QUEBRA DE LINHA NA SERIAL
    }
    else if (now == compiled) { //SENÃO, SE A INFORMAÇÃO REGISTRADA FOR IGUAL A INFORMAÇÃO COMPILADA, FAZ
        Serial.println("As informações atuais do RTC são iguais as de compilação! Não é o esperado, mas está tudo OK."); //IMPRIME O TEXTO NO MONITOR SERIAL
        Serial.println(); //QUEBRA DE LINHA NA SERIAL
    }

    digitalWrite(BUZZ, HIGH);
    delay(500);
    digitalWrite(BUZZ, LOW);
    delay(100);
    digitalWrite(BUZZ, HIGH);
    delay(250);
    digitalWrite(BUZZ, LOW);
}

void loop () {
    RtcDateTime now = Rtc.GetDateTime(); //VARIÁVEL RECEBE INFORMAÇÕES
    printDateTime(now); //PASSA OS PARÂMETROS PARA A FUNÇÃO printDateTime
    Serial.println(); //QUEBRA DE LINHA NA SERIAL
    delay(1000); //INTERVALO DE 1 SEGUNDO

    //Serial.println(now.Hour());
    
    //PERÍODO DE FUNCIONAMENTO:
    if(((now.Hour()) >= horaAtivo) && ((now.Hour()) < horaInativo))
    {
        millisAtual = millis();
        //Serial.println("ENTROU NO PERÍODO...");

        digitalWrite(BUZZ, HIGH);
        digitalWrite(LED, HIGH);
        delay(35);
        digitalWrite(BUZZ, LOW);
        digitalWrite(LED, LOW);
        
        //BOMBA ASPERSORA:
        //SE O TEMPO ATUAL MENOS O TEMPO ANTERIOR FOR MAIOR QUE O INTERVALO SETADO PARA ASPERÇÃO:
        if ((millisAtual - millisAnteriorLigASP) > intervaloAspercao) 
        {  
            
            //LIGA ASPERÇÃO:
            digitalWrite(RL2, LOW);
            Serial.println("ASPERÇÃO LIGADA"); 
            millisAnteriorLigASP = millisAtual; 
            digitalWrite(BUZZ, HIGH);
            delay(1000);
            digitalWrite(BUZZ, LOW);
        }

        millisAtualDeslASP = millisAtual;
            
        if ((millisAtualDeslASP - millisAnteriorLigASP) > tempoAspercao) 
        {  
            //DESLIGA ASPERÇÃO:
            digitalWrite(RL2, HIGH);
            Serial.println("ASPERÇÃO DESLIGADA");
            millisAnteriorDeslASP = millisAtualDeslASP;
        }

        //BOMBA GOTEJADORA:
        //SE O TEMPO ATUAL MENOS O TEMPO ANTERIOR FOR MAIOR QUE O INTERVALO SETADO PARA GOTEJAMENTO:
        if ((millisAtual - millisAnteriorLigGOT) > intervaloGotejamento) 
        {  
            //LIGA GOTEJAMENTO:
            digitalWrite(RL3, LOW);
            Serial.println("GOTEJAMENTO LIGADO");
            millisAnteriorLigGOT = millisAtual;
            digitalWrite(BUZZ, HIGH);
            delay(1000);
            digitalWrite(BUZZ, LOW); 
        }

        millisAtualDeslGOT = millisAtual;
            
        if ((millisAtualDeslGOT - millisAnteriorLigGOT) > tempoGotejamento) 
        {  
            //DESLIGA ASPERÇÃO:
            digitalWrite(RL3, HIGH);
            Serial.println("GOTEJAMENTO DESLIGADO");
            millisAnteriorDeslGOT = millisAtualDeslGOT;
        }
        
    }
    else
    {
        millisAnteriorLigASP  = NULL;
        millisAtualDeslASP    = NULL;
        millisAnteriorDeslASP = NULL;
        millisAnteriorLigGOT  = NULL;
        millisAtualDeslGOT    = NULL;
        millisAnteriorDeslGOT = NULL; 
        digitalWrite(RL2, HIGH);
        digitalWrite(RL3, HIGH); 
        digitalWrite(LED, HIGH);     
    }
    
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt){
    char datestring[20]; //VARIÁVEL ARMAZENA AS INFORMAÇÕES DE DATA E HORA

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"), //FORMATO DE EXIBIÇÃO DAS INFORMAÇÕES
            dt.Day(), //DIA
            dt.Month(), //MÊS
            dt.Year(), //ANO
            dt.Hour(), //HORA
            dt.Minute(), //MINUTOS
            dt.Second() ); //SEGUNDOS
    Serial.print(datestring); //IMPRIME NO MONITOR SERIAL AS INFORMAÇÕES
}
