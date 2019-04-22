/**
 * \file
 *
 * \brief Example of usage of the maXTouch component with USART
 *
 * This example shows how to receive touch data from a maXTouch device
 * using the maXTouch component, and display them in a terminal window by using
 * the USART driver.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

/**
 * \mainpage
 *
 * \section intro Introduction
 * This simple example reads data from the maXTouch device and sends it over
 * USART as ASCII formatted text.
 *
 * \section files Main files:
 * - example_usart.c: maXTouch component USART example file
 * - conf_mxt.h: configuration of the maXTouch component
 * - conf_board.h: configuration of board
 * - conf_clock.h: configuration of system clock
 * - conf_example.h: configuration of example
 * - conf_sleepmgr.h: configuration of sleep manager
 * - conf_twim.h: configuration of TWI driver
 * - conf_usart_serial.h: configuration of USART driver
 *
 * \section apiinfo maXTouch low level component API
 * The maXTouch component API can be found \ref mxt_group "here".
 *
 * \section deviceinfo Device Info
 * All UC3 and Xmega devices with a TWI module can be used with this component
 *
 * \section exampledescription Description of the example
 * This example will read data from the connected maXTouch explained board
 * over TWI. This data is then processed and sent over a USART data line
 * to the board controller. The board controller will create a USB CDC class
 * object on the host computer and repeat the incoming USART data from the
 * main controller to the host. On the host this object should appear as a
 * serial port object (COMx on windows, /dev/ttyxxx on your chosen Linux flavour).
 *
 * Connect a terminal application to the serial port object with the settings
 * Baud: 57600
 * Data bits: 8-bit
 * Stop bits: 1 bit
 * Parity: None
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "conf_board.h"
#include "conf_example.h"
#include "conf_uart_serial.h"

typedef struct {
	const uint8_t *data;
	uint16_t width;
	uint16_t height;
	uint8_t dataSize;
	int indice;
} tImage;


/*
typedef struct tImage icone;

struct ciclo{
	char nome[32];           // nome do ciclo, para ser exibido
	int  enxagueTempo;       // tempo que fica em cada enxague
	int  enxagueQnt;         // quantidade de enxagues
	int  centrifugacaoRPM;   // velocidade da centrifugacao
	int  centrifugacaoTempo; // tempo que centrifuga
	char heavy;              // modo pesado de lavagem
	char bubblesOn;          // smart bubbles on (???)
	icone;
	//int p_icone;
	
	t_ciclo *previous;
	t_ciclo *next;
};
*/

#include "Pesado.h"
#include "Rapido.h"
#include "Centrifuga.h"
#include "Diario.h"
#include "Enxague.h"
#include "laundry.h"
#include "voltar.h"
#include "check.h"
#include "errado.h"
#include "keypad.h"
#include "smiley.h"
#include "warning.h"
#include "maquina1.h"
#include "right.h"
#include "left.h"
#include "stop.h"
#include "tfont.h"
#include "calibri_36.h"

#define MAX_ENTRIES        3
#define STRING_LENGTH     70

#define USART_TX_MAX_LENGTH     0xff

struct ili9488_opt_t g_ili9488_display_opt;


//BuzzerBoy e uma nota
#define BUZZ_PIO			PIOA
#define BUZZ_PIO_ID			10
#define BUZZ_PIO_IDX		19u
#define BUZZ_PIO_IDX_MASK	(1u << BUZZ_PIO_IDX)
#define NOTE_C1  33

// butaum trava de crianca
#define TRAVA_PIO           PIOD
#define TRAVA_PIO_ID        ID_PIOD
#define TRAVA_PIO_IDX       28
#define TRAVA_IDX_MASK      (1u << TRAVA_PIO_IDX)

// botao porta
#define PORTA_PIO           PIOC
#define PORTA_PIO_ID        ID_PIOC
#define PORTA_PIO_IDX       17
#define PORTA_IDX_MASK      (1u << PORTA_PIO_IDX)


// LED MOTOR
#define MOTOR_PIO           PIOA
#define MOTOR_PIO_ID        ID_PIOA
#define MOTOR_PIO_IDX       0
#define MOTOR_IDX_MASK      (1u << MOTOR_PIO_IDX)

// LED AGUA
#define AGUA_PIO           PIOB
#define AGUA_PIO_ID        ID_PIOB
#define AGUA_PIO_IDX       3
#define AGUA_IDX_MASK      (1u << AGUA_PIO_IDX)

// LED TRANCA PORTA
#define TRANCA_PIO           PIOC
#define TRANCA_PIO_ID        ID_PIOC
#define TRANCA_PIO_IDX       31
#define TRANCA_IDX_MASK      (1u << TRANCA_PIO_IDX)

// POSICOES DO HOME
const uint32_t direita_x = 246;
const uint32_t direita_y = 208;
const uint32_t esquerda_x = 10;
const uint32_t esquerda_y = 208;
const uint32_t icone_x_home = 96; // 
const uint32_t icone_y_home = 106;
const uint32_t nome_x = 100;
const uint32_t nome_y = 300;

// POSICOES DO AJUSTES
const uint32_t voltar_x = 20;
const uint32_t voltar_y = 340;
const uint32_t check_x = 182;
const uint32_t check_y = 340;

// POSICOES DO PROCESSA
const uint32_t icone_x = 20;
const uint32_t icone_y = 60;
const uint32_t stop_x = 100; 
const uint32_t stop_y = 300;

//RTT
static void RTT_init(uint16_t pllPreScale, uint32_t IrqNPulses);
volatile Bool f_rtt_alarme = false;
volatile int segundo = 0;
volatile int minuto = 0;
volatile int hora = 0;
volatile char tempo[32];
volatile char tempo_faltante[32];

//Senha
volatile char senhaReal[4] = {1, 2, 3, 4};
volatile char senhaUser[4] = {0, 0, 0, 0};	
	
	
/*
Estado:
0 = home
1 = ajustes
2 = progresso
3 = porta aberta
4 = interrupcao (senha)
5 = finalizacao
*/
volatile int estadoAnterior = 0; 
volatile int estadoAtual = 0;  
volatile int indice = 0;    // para acessar o icone certo
volatile int buttonpress = 1;
volatile int portaAberta = 0;

void buzz(long frequency, long duration) {
	long delayValue = 1000000 / frequency / 2;
	long Cycles = frequency * duration / 1000;
	for (long i = 0; i < Cycles; i++) {
		pio_set(BUZZ_PIO, BUZZ_PIO_IDX_MASK);	// 1 no BUZZ
		delay_us(delayValue);	//delay de acordo com a frequencia e duracao do toque
		pio_clear(BUZZ_PIO, BUZZ_PIO_IDX_MASK); // 0 no BUZZ
		delay_us(delayValue);
	}
}

void pin_toggle(Pio *pio, uint32_t mask){
	if(pio_get_output_data_status(pio, mask))
	pio_clear(pio, mask);
	else
	pio_set(pio,mask);
}


void clear(void){
	ili9488_set_foreground_color(COLOR_CONVERT(COLOR_WHITE));
	ili9488_draw_filled_rectangle(0, 0, ILI9488_LCD_WIDTH-1, ILI9488_LCD_HEIGHT-1);
}

void clear_min(void){
	ili9488_set_foreground_color(COLOR_CONVERT(COLOR_WHITE));
	ili9488_draw_filled_rectangle(250, 230, ILI9488_LCD_WIDTH-1, 265);
}



void but_callback(void)
{
	if (buttonpress == 0) {
		buttonpress = 1;
		} else {
		buttonpress = 0;
	}
	
}



void RTT_Handler(void)
{
	uint32_t ul_status;

	/* Get RTT status */
	ul_status = rtt_get_status(RTT);

	/* IRQ due to Time has changed */
	if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) {  }

	/* IRQ due to Alarm */
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
		//pin_toggle(LED_PIO, LED_IDX_MASK);    // BLINK Led
		f_rtt_alarme = true;                  // flag RTT alarme
	}
}

static float get_time_rtt(){
	uint ul_previous_time = rtt_read_timer_value(RTT);
}

static void RTT_init(uint16_t pllPreScale, uint32_t IrqNPulses)
{
	uint32_t ul_previous_time;

	/* Configure RTT for a 1 second tick interrupt */
	rtt_sel_source(RTT, false);
	rtt_init(RTT, pllPreScale);
	
	ul_previous_time = rtt_read_timer_value(RTT);
	while (ul_previous_time == rtt_read_timer_value(RTT));
	
	rtt_write_alarm_time(RTT, IrqNPulses+ul_previous_time);

	/* Enable RTT interrupt */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 0);
	NVIC_EnableIRQ(RTT_IRQn);
	rtt_enable_interrupt(RTT, RTT_MR_ALMIEN);
}	
	
static void configure_lcd(void){
	/* Initialize display parameter */
	g_ili9488_display_opt.ul_width = ILI9488_LCD_WIDTH;
	g_ili9488_display_opt.ul_height = ILI9488_LCD_HEIGHT;
	g_ili9488_display_opt.foreground_color = COLOR_CONVERT(COLOR_WHITE);
	g_ili9488_display_opt.background_color = COLOR_CONVERT(COLOR_WHITE);

	/* Initialize LCD */
	ili9488_init(&g_ili9488_display_opt);
}

/**
 * \brief Set maXTouch configuration
 *
 * This function writes a set of predefined, optimal maXTouch configuration data
 * to the maXTouch Xplained Pro.
 *
 * \param device Pointer to mxt_device struct
 */
static void mxt_init(struct mxt_device *device)
{
	enum status_code status;

	/* T8 configuration object data */
	uint8_t t8_object[] = {
		0x0d, 0x00, 0x05, 0x0a, 0x4b, 0x00, 0x00,
		0x00, 0x32, 0x19
	};

	/* T9 configuration object data */
	uint8_t t9_object[] = {
		0x8B, 0x00, 0x00, 0x0E, 0x08, 0x00, 0x80,
		0x32, 0x05, 0x02, 0x0A, 0x03, 0x03, 0x20,
		0x02, 0x0F, 0x0F, 0x0A, 0x00, 0x00, 0x00,
		0x00, 0x18, 0x18, 0x20, 0x20, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x02,
		0x02
	};

	/* T46 configuration object data */
	uint8_t t46_object[] = {
		0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x03,
		0x00, 0x00
	};

	/* T56 configuration object data */
	uint8_t t56_object[] = {
		0x02, 0x00, 0x01, 0x18, 0x1E, 0x1E, 0x1E,
		0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
		0x1E, 0x1E, 0x1E, 0x1E, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00
	};

	/* TWI configuration */
	twihs_master_options_t twi_opt = {
		.speed = MXT_TWI_SPEED,
		.chip  = MAXTOUCH_TWI_ADDRESS,
	};

	status = (enum status_code)twihs_master_setup(MAXTOUCH_TWI_INTERFACE, &twi_opt);
	Assert(status == STATUS_OK);

	/* Initialize the maXTouch device */
	status = mxt_init_device(device, MAXTOUCH_TWI_INTERFACE,
			MAXTOUCH_TWI_ADDRESS, MAXTOUCH_XPRO_CHG_PIO);
	Assert(status == STATUS_OK);

	/* Issue soft reset of maXTouch device by writing a non-zero value to
	 * the reset register */
	mxt_write_config_reg(device, mxt_get_object_address(device,
			MXT_GEN_COMMANDPROCESSOR_T6, 0)
			+ MXT_GEN_COMMANDPROCESSOR_RESET, 0x01);

	/* Wait for the reset of the device to complete */
	delay_ms(MXT_RESET_TIME);

	/* Write data to configuration registers in T7 configuration object */
	mxt_write_config_reg(device, mxt_get_object_address(device,
			MXT_GEN_POWERCONFIG_T7, 0) + 0, 0x20);
	mxt_write_config_reg(device, mxt_get_object_address(device,
			MXT_GEN_POWERCONFIG_T7, 0) + 1, 0x10);
	mxt_write_config_reg(device, mxt_get_object_address(device,
			MXT_GEN_POWERCONFIG_T7, 0) + 2, 0x4b);
	mxt_write_config_reg(device, mxt_get_object_address(device,
			MXT_GEN_POWERCONFIG_T7, 0) + 3, 0x84);

	/* Write predefined configuration data to configuration objects */
	mxt_write_config_object(device, mxt_get_object_address(device,
			MXT_GEN_ACQUISITIONCONFIG_T8, 0), &t8_object);
	mxt_write_config_object(device, mxt_get_object_address(device,
			MXT_TOUCH_MULTITOUCHSCREEN_T9, 0), &t9_object);
	mxt_write_config_object(device, mxt_get_object_address(device,
			MXT_SPT_CTE_CONFIGURATION_T46, 0), &t46_object);
	mxt_write_config_object(device, mxt_get_object_address(device,
			MXT_PROCI_SHIELDLESS_T56, 0), &t56_object);

	/* Issue recalibration command to maXTouch device by writing a non-zero
	 * value to the calibrate register */
	mxt_write_config_reg(device, mxt_get_object_address(device,
			MXT_GEN_COMMANDPROCESSOR_T6, 0)
			+ MXT_GEN_COMMANDPROCESSOR_CALIBRATE, 0x01);
}

void draw_screen(void) {
	ili9488_set_foreground_color(COLOR_CONVERT(COLOR_WHITE));
	ili9488_draw_filled_rectangle(0, 0, ILI9488_LCD_WIDTH-1, ILI9488_LCD_HEIGHT-1);
}

void desenha_icone(tImage icone, int x, int y){
	ili9488_set_foreground_color(COLOR_CONVERT(COLOR_WHITE));
	ili9488_draw_pixmap(x, y, icone.width, icone.height, icone.data);
}

tImage selectIcon(int index){
	if (index == 0){
		return Rapido;
	} 
	if (index == 1){
		return Diario;
	}
	if (index == 2){
		return Pesado;
	}
	if (index == 3){
		return Enxague;
	}
	if (index == 4){
		return Centrifuga;
	}
}

t_ciclo selectCiclo(int index){
	if (index == 0){
		return c_rapido;
	}
	if (index == 1){
		return c_diario;
	}
	if (index == 2){
		return c_pesado;
	}
	if (index == 3){
		return c_enxague;
	}
	if (index == 4){
		return c_centrifuga;
	}
}

void home(){   // t_ciclo t_c){
	if (indice <= -1){
		indice = 4;
	}else if (indice >= 5){
		indice = 0;
	}
	
	desenha_icone(selectIcon(indice), icone_x_home, icone_y_home); 
	desenha_icone(right, direita_x, direita_y);
	desenha_icone(left, esquerda_x, esquerda_y);
	//ili9488_draw_string(icone_x_home, icone_y_home+30, "TOMA");
	
	
	//
	
	/*
	char b[32];
	sprintf(b, "%d", ILI9488_LCD_HEIGHT);*/
	ili9488_set_foreground_color(COLOR_CONVERT(COLOR_WHITE));
	ili9488_draw_filled_rectangle(nome_x-10, nome_y-10, nome_x+1000, nome_y+100);
	
	printa_texto(selectCiclo(indice).nome, nome_x, nome_y);
	
}

void ajustes(){
	desenha_icone(voltar, voltar_x, voltar_y);
	desenha_icone(check, check_x, check_y);
	char a[32];
	char b[32];
	int t = selectCiclo(indice).enxagueTempo*selectCiclo(indice).enxagueQnt+selectCiclo(indice).centrifugacaoTempo;
	if (selectCiclo(indice).heavy == true){
		t = t*1.2;
	}
	sprintf(a, "Lavagem: %s", selectCiclo(indice).nome);
	sprintf(b, "Tempo Estimado: %d", t);
	printa_texto(a, 20, 20);
	printa_texto(b, 20, 60);
}

void progresso(){ 
	
	char a[32];
	sprintf(a, "Em progresso: %s", selectCiclo(indice).nome);
	printa_texto(a, 20, 20);
	desenha_icone(selectIcon(indice), icone_x, icone_y);
	desenha_icone(stop, stop_x, stop_y);
	//sprintf(a, "Tempo de Lavagem: %d min", selectCiclo(indice).enxagueTempo*selectCiclo(indice).enxagueQnt+selectCiclo(indice).centrifugacaoTempo);
	//printa_texto(a, 10, 230);
}

void porta_aberta(){
	clear();
	desenha_icone(warning, 32, 69);
	char a[32];
	sprintf(a, "PORTA ABERTA!!!");
	printa_texto(a, 78, 395);
	buzz(NOTE_C1, 100);
}


/*

void interrupcao(){ //senha
	desenha_icone(keypad, 50, 191);
	
} 

void senha(int numero){
	char a[32];
	sprintf(a, "numero digitado: %d", numero);
}


int teclado(uint32_t tx, uint32_t ty){
	if(ty >= 191) {
		if (tx >= 50 && tx <= 270){ //esta no teclado
			if (tx <= 123){
				if (ty >= 405){
					return 10;  //clear
				}else if (ty >= 334){
					return 1;
				} else if (ty >= 263){
					return 4;
				} else if (ty >= 191){
					return 7;
				}
			}else if (tx <= 196){
				if (ty >= 405){
					return 0; 
				}else if (ty >= 334){
					return 2;
				} else if (ty >= 263){
					return 5;
				} else if (ty >= 191){
					return 8;
				}
			}else{
			if (ty >= 405){
				return 11; // ok
			}else if (ty >= 334){
				return 3;
			} else if (ty >= 263){
				return 6;
			} else if (ty >= 191){
				return 9;
			}
		}
		}
	}
	
}
*/

void finalizacao(){
	//ili9488_set_foreground_color(COLOR_CONVERT(COLOR_GREEN));
	//ili9488_draw_filled_rectangle(0, 0, ILI9488_LCD_WIDTH-1, ILI9488_LCD_HEIGHT-1);
	char a[32];
	sprintf(a, "Ciclo %s Concluido!\n\nToque para voltar ao menu", selectCiclo(indice).nome);
	printa_texto(a, 13, 40);
	desenha_icone(selectIcon(indice), icone_x_home, icone_y_home+50);
	//desenha_icone(smiley, 32, 100);
	//desenha_icone(check, icone_x_home, 320);
}



void porta_callback(void)
{
	if (portaAberta == 0 && estadoAtual != 2) {
		portaAberta = 1;
	} else {
		portaAberta = 0;
		if (estadoAtual == 3){
			estadoAtual = 1;
			clear();
			ajustes();
		}
		
		
	}
	
}



uint32_t convert_axis_system_x(uint32_t touch_y) {
	// entrada: 4096 - 0 (sistema de coordenadas atual)
	// saida: 0 - 320
	return ILI9488_LCD_WIDTH - ILI9488_LCD_WIDTH*touch_y/4096;
}

uint32_t convert_axis_system_y(uint32_t touch_x) {
	// entrada: 0 - 4096 (sistema de coordenadas atual)
	// saida: 0 - 320
	return ILI9488_LCD_HEIGHT*touch_x/4096;
}

void update_screen(uint32_t tx, uint32_t ty) {
	if (buttonpress) {
		if (estadoAtual == 0){ // home
			if(ty >= esquerda_y-10 && ty <= esquerda_y + left.height + 10) {
				if(tx >= direita_x && tx <= direita_x + right.width){
					indice--;
					home();
				} else if(tx >= esquerda_x && tx <= esquerda_x + left.width){
					indice++;
					home();
				}
			
			} if(tx >= icone_x_home && tx <= icone_x_home + Diario.width) {
				if(ty >= icone_y_home-10 && ty <= icone_y_home+Diario.height+10) {
					estadoAtual = 1; // ajustes
					clear();
					ajustes();
				}
			}
		}
		if (estadoAtual == 1){ // ajustes
			if(ty >= voltar_y) {
				if(tx >= check_x) {
					estadoAtual = 2;
					clear();
					progresso();
				} else if(tx <= voltar_x+voltar.width) {
					estadoAtual = 0;
					clear();
					home();
				}
			}
		
		}
		if (estadoAtual == 2){ // progresso
			if (portaAberta == 0){
				segundo = 0;
				pio_set(TRANCA_PIO, TRANCA_IDX_MASK);
				if(tx >= stop_x && tx <= stop_x + stop.width) {
					if(ty >= stop_y-10 && ty <= stop_y+stop.height+10) {
						/*estadoAtual = 4; // senha
						clear();
						interrupcao();*/
						pio_clear(TRANCA_PIO, TRANCA_IDX_MASK);
						pio_clear(AGUA_PIO, AGUA_IDX_MASK);
						pio_clear(MOTOR_PIO, MOTOR_IDX_MASK);
						estadoAtual = 0; // home
						clear();
						home();
					} 
				}
			} else{
				estadoAtual = 3;
				clear();
				porta_aberta();
			}
		}
		
		/*if (estadoAtual == 3){ // porta aberta
			clear();
			porta_aberta();
		}*/
		if (estadoAtual == 4){ // interrupcao
			//senha(teclado(tx, ty));
		}
		if (estadoAtual == 5){ // finalizacao
			estadoAtual = 0; // home
			clear();
			home();
			
		}
	}
}

void mxt_handler(struct mxt_device *device)
{
	/* USART tx buffer initialized to 0 */
	char tx_buf[STRING_LENGTH * MAX_ENTRIES] = {0};
	uint8_t i = 0; /* Iterator */

	/* Temporary touch event data struct */
	struct mxt_touch_event touch_event;

	/* Collect touch events and put the data in a string,
	 * maximum 2 events at the time */
	do {
		/* Temporary buffer for each new touch event line */
		char buf[STRING_LENGTH];
	
		/* Read next next touch event in the queue, discard if read fails */
		if (mxt_read_touch_event(device, &touch_event) != STATUS_OK) {
			continue;
		}
		
		 // eixos trocados (quando na vertical LCD)
		uint32_t conv_x = convert_axis_system_x(touch_event.y);
		uint32_t conv_y = convert_axis_system_y(touch_event.x);
		
		/* Format a new entry in the data string that will be sent over USART */
		sprintf(buf, "Nr: %1d, X:%4d, Y:%4d, Status:0x%2x conv X:%3d Y:%3d\n\r",
				touch_event.id, touch_event.x, touch_event.y,
				touch_event.status, conv_x, conv_y);
				
		if (touch_event.status < 60) {
			update_screen(conv_x, conv_y);	
		}
		
		/* Add the new string to the string buffer */
		strcat(tx_buf, buf);
		i++;

		/* Check if there is still messages in the queue and
		 * if we have reached the maximum numbers of events */
	} while ((mxt_is_message_pending(device)) & (i < MAX_ENTRIES));

	/* If there is any entries in the buffer, send them over USART */
	if (i > 0) {
		usart_serial_write_packet(USART_SERIAL_EXAMPLE, (uint8_t *)tx_buf, strlen(tx_buf));
	}
}

void io_init(void) {
	pmc_enable_periph_clk(TRANCA_PIO_ID);
	pio_configure(TRANCA_PIO, PIO_OUTPUT_0, TRANCA_IDX_MASK, PIO_DEFAULT);
	
	pmc_enable_periph_clk(MOTOR_PIO_ID);
	pio_configure(MOTOR_PIO, PIO_OUTPUT_0, MOTOR_IDX_MASK, PIO_DEFAULT);
	
	pmc_enable_periph_clk(AGUA_PIO_ID);
	pio_configure(AGUA_PIO, PIO_OUTPUT_0, AGUA_IDX_MASK, PIO_DEFAULT);
	
	// Inicializa clock do periférico PIO responsavel pelo botao
	pmc_enable_periph_clk(TRAVA_PIO_ID);

	// Configura PIO para lidar com o pino do botão como entrada
	// com pull-up
	pio_configure(TRAVA_PIO, PIO_INPUT, TRAVA_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);

	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: but_callback()
	pio_handler_set(TRAVA_PIO,
	TRAVA_PIO_ID,
	TRAVA_IDX_MASK,
	PIO_IT_FALL_EDGE,
	but_callback);

	// Ativa interrupção
	pio_enable_interrupt(TRAVA_PIO, TRAVA_IDX_MASK);

	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(TRAVA_PIO_ID);
	NVIC_SetPriority(TRAVA_PIO_ID, 4); // Prioridade 4
	
	
	
	// Inicializa clock do periférico PIO responsavel pelo botao
	pmc_enable_periph_clk(PORTA_PIO_ID);

	// Configura PIO para lidar com o pino do botão como entrada
	// com pull-up
	pio_configure(PORTA_PIO, PIO_INPUT, PORTA_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);

	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: but_callback()
	pio_handler_set(PORTA_PIO,
	PORTA_PIO_ID,
	PORTA_IDX_MASK,
	PIO_IT_FALL_EDGE,
	porta_callback);

	// Ativa interrupção
	pio_enable_interrupt(PORTA_PIO, PORTA_IDX_MASK);

	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(PORTA_PIO_ID);
	NVIC_SetPriority(PORTA_PIO_ID, 0); // Prioridade 4
}


int main(void)
{
	struct mxt_device device; /* Device data container */

	/* Initialize the USART configuration struct */
	const usart_serial_options_t usart_serial_options = {
		.baudrate     = USART_SERIAL_EXAMPLE_BAUDRATE,
		.charlength   = USART_SERIAL_CHAR_LENGTH,
		.paritytype   = USART_SERIAL_PARITY,
		.stopbits     = USART_SERIAL_STOP_BIT
	};
	
	buttonpress = 0;
	f_rtt_alarme = true;
	//estadoAtual = 5;
	
	sysclk_init(); /* Initialize system clocks */
	board_init();  /* Initialize board */
	io_init();
	configure_lcd();
	draw_screen();
	//draw_button(0);
	//desenha_icone(laundry, 100, 100);
	home();
	//finalizacao();
	//porta_aberta();
	/* Initialize the mXT touch device */
	mxt_init(&device);
	
	/* Initialize stdio on USART */
	stdio_serial_init(USART_SERIAL_EXAMPLE, &usart_serial_options);

	printf("\n\rmaXTouch data USART transmitter\n\r");
		
	portaAberta = 0;
	while (true) {
		/* Check for any pending messages and run message handler if any
		 * message is found in the queue */
		if (mxt_is_message_pending(&device)) {
			mxt_handler(&device);
		}
		if (f_rtt_alarme && (estadoAtual == 2 || estadoAtual == 3)) {
			
			uint16_t pllPreScale = (int) (((float) 32768) / 2.0);
			uint32_t irqRTTvalue  = 2;
			
			// reinicia RTT para gerar um novo IRQ
			RTT_init(pllPreScale, irqRTTvalue);
			
			
			if (segundo >= 60) {
				minuto += 1;
				segundo = 0;
			} 
			if (minuto >= 60) {
				hora += 1;
				minuto = 0;
			}
			int t = selectCiclo(indice).enxagueTempo*selectCiclo(indice).enxagueQnt+selectCiclo(indice).centrifugacaoTempo;
			if (selectCiclo(indice).heavy == true){
				t = t*1.2;
			}
			sprintf(tempo, "Tempo: %d:%d:%d", hora, minuto, segundo);
			int min_falta = t-1;
			int seg_falta = 59 - segundo;
			sprintf(tempo_faltante, "Tempo ate finalizar: %02d:%02d", min_falta, seg_falta);
			if (estadoAtual == 2) {
				clear_min();
				//printa_texto(tempo, 10, 250);
				printa_texto(tempo_faltante, 5, 230);
				
				if (minuto >= t) {
					pio_clear(TRANCA_PIO, TRANCA_IDX_MASK);
					pio_clear(AGUA_PIO, AGUA_IDX_MASK);
					pio_clear(MOTOR_PIO, MOTOR_IDX_MASK);
					estadoAtual = 5;
					clear();
					finalizacao();
				}	
			}
			segundo += 1;
			
			f_rtt_alarme = false;
		}
	}

	return 0;
}
