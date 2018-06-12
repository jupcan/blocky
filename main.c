/*
-------------------------------------------------

Blocky Game for NDS
Proyecto para la asignatura ECO

Requiere DevkitARM
Requiere NightFox's Lib

Codigo por Pablo Moraga Navas, Francisco 
López Navarro, Juan Perea Campos, José
Antonio Martínez López y Rubén Pérez Rubio

(c)2014 - JPRJP Inc.

-------------------------------------------------
	Includes
-------------------------------------------------
*/

// Includes C
#include <stdio.h>

// Includes propietarios NDS
#include <nds.h>

// Includes librerias propias
#include <nf_lib.h>

// Includes librerias audio
#include <maxmod9.h>

/*
-------------------------------------------------
	Main() - Bloque general del programa
-------------------------------------------------
*/
int num;
void timeout() {
 
    char mytext[32];
    sprintf(mytext,"Time Spent: %d   ", num++);
    NF_WriteText(0, 2, 9,11, mytext);
    NF_UpdateTextLayers();	// Actualiza las capas de texto

}

int main(int argc, char **argv) {
	
	irqSet(IRQ_TIMER0, &timeout);
	irqEnable(IRQ_TIMER0);
	TIMER_DATA(0) = 0;
	TIMER_CR(0) = TIMER_DIV_1024 | TIMER_ENABLE | TIMER_IRQ_REQ;
		    
	// Pantalla de espera inicializando NitroFS
	NF_Set2D(0, 0);
	NF_Set2D(1, 0);	
	consoleDemoInit();
	iprintf("\n NitroFS init. Please wait.\n\n");
	iprintf(" Iniciando NitroFS,\n por favor, espere.\n\n");
	swiWaitForVBlank();

	// Define el ROOT e inicializa el sistema de archivos
	NF_SetRootFolder("NITROFS");	// Define la carpeta ROOT para usar NITROFS

	// Inicializa el motor 2D
	NF_Set2D(0, 0);	// Modo 2D_0 en ambas pantallas
	NF_Set2D(1, 0);

	// Inicializa el engine de audio de la DS
	soundEnable();

	// Inicializa los fondos tileados
	NF_InitTiledBgBuffers();	// Inicializa los buffers para almacenar fondos
	NF_InitTiledBgSys(0);	// Inicializa los fondos Tileados para la pantalla superior
	NF_InitTiledBgSys(1);	// Iniciliaza los fondos Tileados para la pantalla inferior

	// Inicializa los Sprites
	NF_InitSpriteBuffers();	// Inicializa los buffers para almacenar sprites y paletas
	NF_InitSpriteSys(0);	// Inicializa los sprites para la pantalla superior
	NF_InitSpriteSys(1);	// Inicializa los sprites para la pantalla inferior

	// Inicializa el motor de texto
	NF_InitTextSys(0);	// Inicializa el texto para la pantalla superior

	// Carga la fuente por defecto para el texto
	NF_LoadTextFont("fnt/default", "normal", 256, 256, 0);
	
	// Crea una capa de texto
	NF_CreateTextLayer(0, 2, 0, "normal");
	// Inicializa los buffers de mapas de colisiones
	NF_InitCmapBuffers();

	// Carga los archivos de fondo desde la FAT / NitroFS a la RAM
	NF_LoadTiledBg("bg/bg1", "bg1", 256, 256);	// Carga el fondo de la pantalla superior
	NF_LoadTiledBg("bg/bg2", "bg2", 256, 256);	// Carga el fondo para la capa 3, pantalla inferior
	NF_LoadTiledBg("bg/colmap1", "colmap1", 256, 256);	// Carga el fondo para la capa 2, pantalla inferior
	NF_LoadTiledBg("bg/menu", "menu", 256, 256);	// Carga del fondo del Menu
	NF_LoadTiledBg("bg/youwin", "youwin", 256, 256);	// Carga el fondo de Victoria
	NF_LoadTiledBg("bg/gameover", "gameover", 256, 256);	// Carga el fondo de Game Over

	// Carga los archivos de sprites desde la FAT / NitroFS a la RAM
	NF_LoadSpriteGfx("sprite/pointer", 0, 16, 16);	// Puntero
	NF_LoadSpritePal("sprite/pointer", 0);
	
	// Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 3, "bg1");
	
	// Inicializa los buffers de sonido
	NF_InitRawSoundBuffers();

	// Carga los samples de sonido en formato RAW
	NF_LoadRawSound("sfx/crash", 0, 13025, 1);
	NF_LoadRawSound("sfx/music", 1, 11025, 1);
	NF_LoadRawSound("sfx/gameover", 2, 11025, 1);
	NF_LoadRawSound("sfx/victory", 3, 11025, 1);
	
	// Transfiere a la VRAM los sprites necesarios
	NF_VramSpriteGfx(1, 0, 0, true);	// Puntero
	NF_VramSpritePal(1, 0, 0);

	// Crea el Sprite del puntero en la pantalla inferior
	NF_CreateSprite(1, 0, 0, 0, 124, 92);	// Crea el puntero en la pantalla inferior
	NF_SpriteLayer(1, 0, 2);	// Y la capa sobre la que se dibujara
	
	// Variable para la lectura del keypad
	u16 keys = 0;

	// Variables para el control de movimiento
	s16 x=0; //Coordenada x donde carga el puntero
	s16 y=0; // Coordenada y donde carga el puntero
		
	// Variables para el control de sonido
	u16 newpress = 0;
	u8 sound_id = 0;
	
	// Buffer de texto
	bool dmove=true,lmove=true,rmove=true,umove=true;
	int lev=0; 
	touchPosition touch;
	int i=0;
	
	// Bucle (repite para siempre)
	while(1) {
	
		if (lev==0){

			NF_CreateTiledBg(1, 1, "menu");

			while(lev==0) {
			num=0;				
			if(touch.px > 33 && touch.px <222 && touch.py > 75 && touch.py <116){
					lev++;	
					NF_DeleteTiledBg(1,1);
					NF_CreateTiledBg(1, 3, "bg2");
				}
			swiWaitForVBlank(); // Espera al sincronismo vertical
			touchRead(&touch);
			consoleClear();
			}
		}
	
		if (lev==1 && i==0){
			// Inicia la musica de fondo
			i++;
			num=0;
			x = 80;
			y=104;
			sound_id = NF_PlayRawSound(1, 127, 64, true, 0);

			NF_LoadColisionMap("maps/cmap1", 0, 256, 264);
			NF_CreateTiledBg(1, 2, "colmap1");
			dmove=true,lmove=true,rmove=true,umove=true;

		}
		
		if(lev==2 && i==0){
			// Inicia la musica de fondo
			sound_id = NF_PlayRawSound(1, 127, 64, true, 0);
			num=0;			
			i++;
			x=32;
			y=16;
			NF_LoadColisionMap("maps/cmap2", 0, 256, 264);
			NF_DeleteTiledBg(1,2);		
			NF_LoadTiledBg("bg/colmap2", "colmap2", 256, 256);
			NF_CreateTiledBg(1, 2, "colmap2");
			dmove=true,lmove=true,rmove=true,umove=true;
		} 

		if(lev==3 && i==0){
			// Inicia la musica de fondo
			sound_id = NF_PlayRawSound(1, 127, 64, true, 0);
			num=0;
			i++;
			x=88;
			y=112;
			NF_LoadColisionMap("maps/cmap3", 0, 256, 264);
			NF_DeleteTiledBg(1,2);		
			NF_LoadTiledBg("bg/colmap3", "colmap3", 256, 256);
			NF_CreateTiledBg(1, 2, "colmap3");
			dmove=true,lmove=true,rmove=true,umove=true;
		} 

		// Lee el teclado
		scanKeys();
		keys = keysHeld();
		newpress = keysDown();
		if (keys & KEY_UP) {
			while(umove) {
				y=y-1;
				if (y < 0) {
					y = 0;
					umove = false;
					soundPause(sound_id);
					NF_PlayRawSound(2, 127, 127, false, 0); // Reproduce sonido Game Over
					NF_CreateTiledBg(1,1, "gameover");
					while(i!=0) {
						swiWaitForVBlank(); // Espera al sincronismo vertical
						touchRead(&touch);
						consoleClear();
						if(touch.px >64 && touch.px <192 && touch.py >136 && touch.py <176){
							i=0;	
							NF_DeleteTiledBg(1,1);
							NF_UnloadColisionMap(0);
						}

					}
				}
				NF_MoveSprite(1, 0, x-8, y-8);
				NF_UpdateTextLayers();	// Actualiza las capas de texto
				NF_SpriteOamSet(1);	// Actualiza el Array del OAM
				swiWaitForVBlank();	// Espera al sincronismo vertical
				oamUpdate(&oamSub);	// Actualiza a VRAM el OAM Secundario
				switch (NF_GetTile(0, x, y-9)) {
				case 0:
					break;
				case 1:
					NF_PlayRawSound(0, 50, 127, false, 0); // Reproduce el sonido
					umove = false;
					dmove = true;
					lmove = true;
					rmove = true;				
					break;
				case 2:
					dmove=false;
					lmove=false;
					rmove=false;
					umove=false;
					NF_CreateTiledBg(1,1, "youwin");
					soundPause(sound_id);
					NF_PlayRawSound(3, 127, 127, false, 0); // Reproduce sonido 									
					while(i!=0) {
						swiWaitForVBlank(); // Espera al sincronismo vertical
							touchRead(&touch);
							consoleClear();
						if(touch.px >64 && touch.px <192 && touch.py >136 && touch.py <176){
							lev++;
							i=0;	
							NF_DeleteTiledBg(1,1);
						}
						
					}
					break;
				}
			}
		}

		if (keys & KEY_DOWN) {
			while(dmove) {
				y=y+1;
				if (y > 192) {
					y = 192;
					dmove = false;
					soundPause(sound_id);
					NF_PlayRawSound(2, 127, 127, false, 0); // Reproduce sonido Game Over
					NF_CreateTiledBg(1,1, "gameover");
					while(i!=0) {
						swiWaitForVBlank(); // Espera al sincronismo vertical
							touchRead(&touch);
							consoleClear();
						if(touch.px >64 && touch.px <192 && touch.py >136 && touch.py <176){
							i=0;		
							NF_UnloadColisionMap(0);
							NF_DeleteTiledBg(1,1);
						}
					}
				}
				NF_MoveSprite(1, 0, x-8, y-8);
				NF_UpdateTextLayers();	// Actualiza las capas de texto
				NF_SpriteOamSet(1);	// Actualiza el Array del OAM
				swiWaitForVBlank();	// Espera al sincronismo vertical
				oamUpdate(&oamSub);	// Actualiza a VRAM el OAM Secundario
				switch (NF_GetTile(0, x, y+8)) {
				case 0:
					break;
				case 1:
					NF_PlayRawSound(0, 50, 127, false, 0); // Reproduce el sonido
					dmove = false;
					umove = true;
					lmove = true;
					rmove = true;				
					break;
				case 2:		// verde
					dmove=false;
					lmove=false;
					rmove=false;
					umove=false;
					NF_CreateTiledBg(1,1, "youwin");
					soundPause(sound_id);
					NF_PlayRawSound(3, 127, 127, false, 0); // Reproduce sonido 									
					while(i!=0) {
						swiWaitForVBlank(); // Espera al sincronismo vertical
							touchRead(&touch);
							consoleClear();
						if(touch.px >64 && touch.px <192 && touch.py >136 && touch.py <176){
							lev++;
							i=0;	
							NF_UnloadColisionMap(0);
							NF_DeleteTiledBg(1,1);
						}
						
					}
					break;
				}
			}
		}
		
		if (keys & KEY_LEFT) {
			while(lmove) {
				x=x-1;
				if (x < 0) {
					x = 0;
					lmove = false;
					soundPause(sound_id);
					NF_PlayRawSound(2, 127, 127, false, 0); // Reproduce sonido Game Over
					NF_CreateTiledBg(1,1, "gameover");
					while(i!=0) {
						swiWaitForVBlank(); // Espera al sincronismo vertical
						touchRead(&touch);
						consoleClear();
						if(touch.px >64 && touch.px <192 && touch.py >136 && touch.py <176){
							i=0;
							NF_UnloadColisionMap(0);	
							NF_DeleteTiledBg(1,1);
						}
					
					}
				}
				NF_MoveSprite(1, 0, x-8, y-8);
				NF_UpdateTextLayers();	// Actualiza las capas de texto
				NF_SpriteOamSet(1);	// Actualiza el Array del OAM
				swiWaitForVBlank();	// Espera al sincronismo vertical
				oamUpdate(&oamSub);	// Actualiza a VRAM el OAM Secundario
				switch (NF_GetTile(0, x-9, y)) {
					case 0:
						break;
					case 1:
						NF_PlayRawSound(0, 50, 127, false, 0); // Reproduce el sonido
						lmove = false;
						umove = true;
						dmove = true;
						rmove = true;				
						break;
					case 2:
					dmove=false;
					lmove=false;
					rmove=false;
					umove=false;
					if (lev!=3) { 
						NF_CreateTiledBg(1,1, "youwin");
						soundPause(sound_id);
						NF_PlayRawSound(3, 127, 127, false, 0); // Reproduce sonido
						while(i!=0) {
							swiWaitForVBlank(); // Espera al sincronismo vertical
							touchRead(&touch);
							consoleClear();
							if(touch.px >64 && touch.px <192 && touch.py >136 && touch.py <176){
								lev++;
								i=0;	
								NF_UnloadColisionMap(0);
								NF_DeleteTiledBg(1,1);
							}
							
						}
					} else {
						soundPause(sound_id);
						NF_LoadTiledBg("bg/awesome", "awesome", 256, 256);	// Carga el fondo de Game Over
						NF_CreateTiledBg(1,1, "awesome");
						NF_PlayRawSound(3, 127, 127, false, 0); // Reproduce sonido
						while(i!=0) {
							swiWaitForVBlank(); // Espera al sincronismo vertical
							touchRead(&touch);
							consoleClear();
							if(touch.px >0 && touch.px <256 && touch.py >0 && touch.py <192){
								lev=0;
								i=0;	
								NF_UnloadColisionMap(0);
								NF_DeleteTiledBg(1,2);
								NF_DeleteTiledBg(1,1);
							}
							
						}
					
					}									
					break;
				}	
			}
		}

		if (keys & KEY_RIGHT){
			while(rmove){
				x=x+1;
				if (x > 256) {
					x = 256;
					rmove = false;
					soundPause(sound_id);
					NF_PlayRawSound(2, 127, 127, false, 0); // Reproduce sonido Game Over
					NF_CreateTiledBg(1,1, "gameover");	
					while(i!=0) {
						swiWaitForVBlank(); // Espera al sincronismo vertical
						touchRead(&touch);
						consoleClear();
						if(touch.px >64 && touch.px <192 && touch.py >136 && touch.py <176){
							i=0;	
							NF_DeleteTiledBg(1,1);
						}
					}
				}
				NF_MoveSprite(1, 0, x-8, y-8);
				NF_UpdateTextLayers();	// Actualiza las capas de texto
				NF_SpriteOamSet(1);	// Actualiza el Array del OAM
				swiWaitForVBlank();	// Espera al sincronismo vertical
				oamUpdate(&oamSub);	// Actualiza a VRAM el OAM Secundario
				switch (NF_GetTile(0, x+8, y)) {
					case 0:
						break;
					case 1:
						NF_PlayRawSound(0, 50, 127, false, 0); // Reproduce el sonido
						rmove = false;
						umove = true;
						dmove = true;
						lmove = true;			
						break;
					case 2:
					dmove=false;
					lmove=false;
					rmove=false;
					umove=false;
					NF_CreateTiledBg(1,1, "youwin");
					soundPause(sound_id);
					NF_PlayRawSound(3, 127, 127, false, 0); // Reproduce sonido 									
					while(i!=0) {
						swiWaitForVBlank(); // Espera al sincronismo vertical
						touchRead(&touch);
						consoleClear();
						if(touch.px >64 && touch.px <192 && touch.py >136 && touch.py <176){
							lev++;
							i=0;		
							NF_UnloadColisionMap(0);
							NF_DeleteTiledBg(1,1);
						}
					}
						break;
				}
			}
		}

		// Detiene el sonido, pero no la borra de la memoria de sonido
		if (newpress & KEY_A) soundPause(sound_id);
		// Vuelve a reproducir el sonido, pero desde el principio
		if (newpress & KEY_B) soundResume(sound_id);
	
		// Posicion del Sprite
		NF_MoveSprite(1, 0, x-8, y-8);

		// Imprime la posicion del cursor
		// sprintf(mytext,"x:%d  y:%d ", x, y);
		// NF_WriteText(0, 2, 1, 1, mytext);

		// Imprime el n� de tile
		
		
		NF_UpdateTextLayers();	// Actualiza las capas de texto
		NF_SpriteOamSet(1);	// Actualiza el Array del OAM
		swiWaitForVBlank();	// Espera al sincronismo vertical
		oamUpdate(&oamSub);	// Actualiza a VRAM el OAM Secundario
	}
	return 0; 
}
