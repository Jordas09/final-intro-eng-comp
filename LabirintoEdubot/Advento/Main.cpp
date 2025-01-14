#include <iostream>
#include <time.h>
#include "libs/EdubotLib.hpp"

//edulib->move(double x); anda x unidades para frente ou para trás 
//edulib->rotate(double deg); rotaciona deg graus para a direita ou para a esquerda
//edulib->sleepMilliseconds(double t); espera t millisegundos antes da proxima instrução
//edulib->stop(); para o carro
//edulib->neutral(); deixa no neutro ('o')
//edulib->getSonar(size_t id); le a distancia marcada pelo sonar em metros
//edulib->getBumper(size_t id); le os bumpers
//edulib->getEncoderCountLeft(); le roda esquerda
//edulib->getEncoderCountRight(); le roda direita
//edulib->getEncoderCountDT(); tempo de loop dos encoders
//edulib->getX(); retorna a posição em x a partir do início
//edulib->getY(); retorna a posição em y a partir do início
//edulib->getTheta(); retorna giro do robô a partir da posição inicial
//edulib->getBatteryCellVoltage(size_t id); voltagem da bateria

#define N_SONARS 7 //numero de sonares
#define N_BUMPERS 4 //numero de bumpers 
#define N_CELLS 3 //numero de celulas de bateria

#define DIST 0.16
#define LATERAL 0.27
#define SPEED 0.2
#define ROT_TIME 1500
#define DELAY 1000

typedef struct{
	double x;
	double y;
}COORDS;

using namespace std;

bool olhaSonar(double sonar[], EdubotLib* edulib);

int main(void){
//inicializa variaveis
	EdubotLib* edulib = new EdubotLib(); 
	double sonar[N_SONARS];
	bool bumper[N_BUMPERS];
	int i = 0, choice;
	int loopcounter = 0;
	double aproxangulo;
	COORDS posi;
	bool kill;

	srand((unsigned)time(0));
//conecta
	if(!(edulib->connect()));

//loop
	else{
		edulib->sleepMilliseconds(1000);

		while(edulib->isConnected()){
			loopcounter++;
			cout << loopcounter << endl;
			if(loopcounter > 100){
			loopcounter = 0;
			}	
			//pega as posições iniciais do loop
			posi.x = edulib->getX();
			posi.y = edulib->getY(); 
			
			for(i = 0; i < N_BUMPERS; i++){//pega os bumpers
				bumper[i] = edulib->getBumper(i);
			}

			choice = (rand()%3);//escolhe entre 3 alternativas
			aproxangulo = edulib->getTheta(); // pega o angulo do robô

			if(aproxangulo != 0 && aproxangulo != 90 && aproxangulo != 180 && aproxangulo != 270 && aproxangulo != 360){//se o angulo nao é multiplo de 90°, normaliza
				cout << "corrige angulo" << aproxangulo << endl;
				if(aproxangulo > 180){
					if(aproxangulo < 270)
						aproxangulo -= 180;
					else if(aproxangulo > 270)
						aproxangulo -= 270;
				}
				if(aproxangulo < 180)
					edulib->rotate(aproxangulo);
				edulib->sleepMilliseconds(ROT_TIME * 2);
			}

			if(bumper[0] || bumper[1] || bumper[2] || bumper[3]){//checa se bateu
				cout << "bateu" << endl;
				if(bumper[0] && bumper[1]){
					cout<< "bateu frente inteira" << endl;
					edulib->sleepMilliseconds(DELAY/2);
					edulib->move(-0.1 * SPEED);
					edulib->sleepMilliseconds(DELAY / 2);
					edulib->move(-SPEED);
					edulib->sleepMilliseconds(DELAY / 2 - 100);
					edulib->stop();
					edulib->sleepMilliseconds(DELAY);
				}
				else if(bumper[0]){
					cout << "bateu frente esquerda" << endl;
					edulib->sleepMilliseconds(DELAY/2);
					edulib->rotate(15);
					edulib->sleepMilliseconds(ROT_TIME);
					edulib->move(-SPEED);
					edulib->sleepMilliseconds(DELAY / 2 - 100);
					edulib->stop();
					edulib->sleepMilliseconds(DELAY);
					edulib->rotate(75);
					edulib->sleepMilliseconds(ROT_TIME);	
				}
				else if(bumper[1]){
					cout << "bateu frente direita" << endl;
					edulib->sleepMilliseconds(DELAY/2);
					edulib->rotate(-15);
					edulib->sleepMilliseconds(ROT_TIME);
					edulib->move(-SPEED);
					edulib->sleepMilliseconds(DELAY / 2 - 100);
					edulib->stop();
					edulib->sleepMilliseconds(DELAY);
					edulib->rotate(-75);
					edulib->sleepMilliseconds(ROT_TIME);	
				}
				else if(bumper[2] || bumper[3]){
					cout<< "bateu atras" << endl;
					edulib->sleepMilliseconds(DELAY/2);
					edulib->move(0.5 * SPEED);
					edulib->sleepMilliseconds(DELAY / 2 - 100);
				}
				
			}

			
			else{
				kill = olhaSonar(sonar, edulib);//recebe informações do sonar e se ja saiu do labirinto
				if(sonar[3] < DIST){
					cout << "frente" << endl;
					if(sonar[0] >= LATERAL && sonar[6] >= LATERAL){
						cout << "bifurca" << endl;
						if(choice == 0){
							edulib->rotate(-90);
							edulib->sleepMilliseconds(ROT_TIME);					
							edulib->move(SPEED);
							edulib->sleepMilliseconds(5 * DELAY / 2);
						}
						else if(choice == 1){
							edulib->rotate(90);
							edulib->sleepMilliseconds(ROT_TIME);					
							edulib->move(SPEED);
							edulib->sleepMilliseconds(5 * DELAY / 2);
						}
						else{
							edulib->stop();
							edulib->sleepMilliseconds(DELAY / 2);	
						}
						kill = olhaSonar(sonar, edulib);
					}
	
					else if(sonar[0] < LATERAL){
						cout << "joelho direita" << endl;
						edulib->rotate(90);
						edulib->sleepMilliseconds(ROT_TIME);					
						kill = olhaSonar(sonar, edulib);			
					}

			
					else if(sonar[6] < LATERAL){
						cout << "joelho esquerda" << endl;
						edulib->rotate(-90);
						edulib->sleepMilliseconds(ROT_TIME);					
						kill = olhaSonar(sonar, edulib);
					}

					else{
					cout << "frente extra ultimo caso" << endl;
					edulib->rotate(90);
					edulib->sleepMilliseconds(ROT_TIME);
					kill = olhaSonar(sonar, edulib);
					}
				}

				
				if(sonar[3] >= DIST){
					cout << "frente longe" << endl;
					if(sonar[0] < 2 * LATERAL && sonar[6] < 2 * LATERAL){
						cout << "corredor" << endl;
						edulib->move(SPEED);
						edulib->sleepMilliseconds(30);
					}
			
					else if(sonar[0] >= 2 * LATERAL && sonar[6] >= 2 * LATERAL){
						cout << "encrusilhada" << endl;
						edulib->sleepMilliseconds(DELAY);
						switch(choice){
							case 0:
								edulib->rotate(90);
								edulib->sleepMilliseconds(ROT_TIME);
								edulib->move(SPEED);
								edulib->sleepMilliseconds(5 * DELAY / 2);
							break;
							case 1:
								edulib->rotate(-90);
								edulib->sleepMilliseconds(ROT_TIME);
								edulib->move(SPEED);
								edulib->sleepMilliseconds(5 * DELAY / 2);
							break;
							case 2:
								edulib->move(SPEED);
								edulib->sleepMilliseconds(5 * DELAY / 2);
							break;
						}
						kill = olhaSonar(sonar, edulib);
					}
			
						else if(sonar[0] >= 2 * LATERAL && sonar[6] < 2 * LATERAL){
							cout << "reto esquerda" << endl;
							edulib->sleepMilliseconds(DELAY);
							if(choice == 0){
								edulib->rotate(-90);
								edulib->sleepMilliseconds(ROT_TIME);
								edulib->move(SPEED);
								edulib->sleepMilliseconds(2.2 * DELAY);
							}	
							else{
								edulib->move(SPEED);
								edulib->sleepMilliseconds(2.2 * DELAY);
							}
							kill = olhaSonar(sonar, edulib);
						}
			
						else if(sonar[6] >= 2 * LATERAL && sonar[0] < 2 * LATERAL){
							cout << "reto direita" << endl;
							edulib->sleepMilliseconds(DELAY);
							if(choice == 0){
								edulib->rotate(90);
								edulib->sleepMilliseconds(ROT_TIME);
								edulib->move(SPEED);
								edulib->sleepMilliseconds(2.2 * DELAY);
							}
							else{
							edulib->move(SPEED);
							edulib->sleepMilliseconds(2.2 * DELAY);
							}
							kill = olhaSonar(sonar, edulib);
						}
				}	
			
				if(sonar[0] < LATERAL/4 && sonar[6] >= LATERAL / 3){
					cout << "corrige direita" << endl;
					edulib->rotate(25);
					edulib->sleepMilliseconds(ROT_TIME);
					edulib->move(2 * SPEED);	
					kill = olhaSonar(sonar, edulib);				
					edulib->sleepMilliseconds(50);
					edulib->rotate(-25);
					edulib->sleepMilliseconds(ROT_TIME);
				}
				if(sonar[6] < LATERAL/4 && sonar[0] >= LATERAL / 3){
					cout << "corrige esquerda" << endl;
					edulib->rotate(-25);
					edulib->sleepMilliseconds(ROT_TIME);
					edulib->move(2 * SPEED);	
					edulib->sleepMilliseconds(50);
					edulib->rotate(25);
					edulib->sleepMilliseconds(ROT_TIME);		
				}
			}
			
			if(kill == 1){
				cout << "escapou labirinto" << endl;
				edulib->sleepMilliseconds(500);
				edulib->stop();
				edulib->disconnect();
			}
			
			if(time(NULL) % 10 == 0){
				if(posi.x == edulib->getX() && posi.y == edulib->getY()){
					cout << "preso" << endl;
					edulib->sleepMilliseconds(ROT_TIME/3);
					edulib->move(-SPEED);
					edulib->sleepMilliseconds(DELAY/3);
					edulib->rotate(45);
					edulib->sleepMilliseconds(ROT_TIME);
				}
			}
		}
	}
	return 0;
}

bool olhaSonar(double sonar[], EdubotLib* edulib){
	bool mata = 1;
	int i;
	
	for(i = 0; i < N_SONARS; i++){// pega as distancias dos sonares e checa se saiu do labirinto
		sonar[i] = edulib->getSonar(i);
		if(sonar[i] <= 1.5){
			mata = 0;
		}
		cout << mata << endl;
	}
	return mata;
}

void moveNoEvents(double sonar[], EdubotLib* edulib){}