/* 
  Josh Hawley, July 23rd 2013
  Licensed for non-commercial use, must include this license message
  TLDR; Wil Wheaton's Law
  Essa sketch grava e identifica digitais
 
 Alterado para a versão GT-511C1 e corrigido por Marco Silva by LDG
 */

int rec = 6;//Botao 1
int zero = 7;//Botao 2
int rele = 8;//Botao 2

int temp = 0;//Comparador

#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

// Hardware setup - FPS connected to:
//   digital pin 4(arduino rx, fps tx)
//   digital pin 5(arduino tx, fps rx)

FPS_GT511C3 fps(11, 10);//Configura os tx e rx scanner

void setup()
{
  Serial.begin(9600);
  delay(50);
  Serial.println("iniciando...");
  fps.Open();
  delay(20);
  fps.SetLED(true);
  Serial.println("... ok"); 
  //Define pinos como entrada
  pinMode(rec, INPUT);
  pinMode(zero, INPUT);
  pinMode(rele, OUTPUT);
}

void loop()
{
  //Faz a leitura dos botoes e grava seus estados
  int rec1 = digitalRead(rec);
  int zero1 = digitalRead(zero);

  //Requerindo o cadastro de novas digitais
  if(rec1==0)
  {
    //Printa as informações no LCD
    Serial.println("Novo cadastro ... Aguarde");
    delay(2000);//Aguarda 2 segundos
    Serial.println("Posicione o dedo no Scanner Cad.");
    //Chama a função que faráos registros de novas digitais
    Enroll();
  }

  //Zerando o registro de digitais
  if(zero1==0)
  {
    fps.DeleteAll();// Apaga todo o registro de memoria de digitais
    //Printa as informações no LCD
    Serial.println("O Registro foi Apagado");
    delay(2000);//Aguarda 2 segundos
  }

  // Identificando digitais
  if (fps.IsPressFinger())
  {
    fps.CaptureFinger(false);//Realiza a leitura da digital no scanner
    int id = fps.Identify1_N();//Salva na variavel id o codido de identificação da digital
    if (id <20)//Compara se é menor que 20, no caso do modelo GT-511C1 voce consegue armazenar ate 20 digitais diferentes
      // Ou seja se for menor é porque esta cadastrado
    {
      //Printa as informações no LCD se houver reconhecimento de dados
      Serial.println("Liberado ID Bem Vindo  ");
      Serial.print(id);
      delay(100);
    }
    else
    {
      //Printa as informações no LCD se nao houve reconhecimento de dados
      Serial.println("Nao reconhecido ID Nao Cadast.");
      delay(1500);//Aguarda 2 segundos
    }
  }
  else
  {
    Serial.println("Pressione o dedo no Scanner ");
  }
}


//Funcao responsavel por cadastar impressoes digitais
void Enroll()
{
  int enrollid = 0;
  //É verificado quantos digitais tem cadastradas, e selecionandoa proxima posição disponivel para armazenamento
  //======================================================================================
  bool usedid = true;
  while (usedid == true)
  {
    usedid = fps.CheckEnrolled(enrollid);
    if (usedid==true) enrollid++;
  }
  fps.EnrollStart(enrollid);
  temp=enrollid;
  //======================================================================================

  //Aguarda até que o dedo seja retirado do scanner
  while(fps.IsPressFinger() == false) delay(100);
  bool bret = fps.CaptureFinger(true);
  int iret = 0;
  //Se conseguir realizar alguma leitura a variavel "bret" valerá 1
  if (bret != false)
  {
    Serial.println("Remova o dedo");
    fps.Enroll1();//Salva a primeira informacao da digital
    //Aguarda até que o dedo seja retirado do scanner
    while(fps.IsPressFinger() == true) delay(100);
    //Solicita que o dedo seja pressionado no scanner novamente
    Serial.println("Pressione o dedo Novamente");
    //Aguarda até que o dedo seja retirado do scanner
    while(fps.IsPressFinger() == false) delay(100);
    //Repete tudo por mais 2 vezes
    //=====================================================================================
    bret = fps.CaptureFinger(true);
    if (bret != false)
    {
      Serial.println("Remova o dedo");
      fps.Enroll2();
      while(fps.IsPressFinger() == true) delay(100);
      Serial.println("Pressione o dedo novamente");
      while(fps.IsPressFinger() == false) delay(100);
      //====================================================================================
      //====================================================================================
      bret = fps.CaptureFinger(true);
      if (bret != false)
      {
        Serial.println("Remova o dedo");
        iret = fps.Enroll3();
        //===================================================================================
        //Nessa parte é verificado se se o cadastro foi realizado com sucesso
        enrollid=0;          
        usedid = true;
        while (usedid == true)
        {
          usedid = fps.CheckEnrolled(enrollid);
          if (usedid==true) enrollid++;
        } 
        //Compara se o dedo foi pressionado 3 vezes e se a posição anterior da memoria é igualk a atual
        if (iret == 0&&temp!=enrollid)
        {
          //Se for igual printará as informações no LCD e salva automaticamente na memória
          Serial.println("Cadastro feito com sucesso");
          Serial.println(enrollid);
          delay(1000);//Aguarda 1 segundo
        }
        else
        {
          //Se não printará uma informação acusando falha
          Serial.println("Cadastro falho Tente novamente");
          delay(1000);//Aguarda 1 segundo
        }
      }
      // Se houver falhas durante a captura printará umas das 3 mensagens abaixo, de acordo com cada posição
      //Erro 1
      else 
      {
        Serial.println("Falha na captura do primeiro dedo");
        delay(1000);
      }
      //====================================================================================
    }
    //Erro 2
    else 
    {
      Serial.println("Falha na captura do segundo dedo");
      delay(1000);
    }
    //=====================================================================================
  }
  //Erro 3
  else 
  {
    Serial.println("Falha na captura do terceiro dedo");
    delay(1000);
  }
  //======================================================================================
}
