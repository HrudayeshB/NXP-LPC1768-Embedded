#include <LPC17xx.h>
#include <stdio.h>
#include <string.h>

// ========================= CONSTANTS ==============================

#define THR_EMPTY        (1 << 5)   // UART LSR: Transmit Holding Register empty
#define RDR_READY        (1 << 0)   // UART LSR: Receiver Data Ready

#define CCLK             12000000   // CPU clock 12 MHz

#define DHT11_PIN        (1 << 5)   // P0.5 for DHT11

// ===== User Wifi / Cloud Credentials =====
#define WIFI_SSID        "WIFI_SSID"
#define WIFI_PASS        "WIFI_PASS"
#define THINGSPEAK_KEY   "API_KEY"

// ========================= GLOBAL BUFFERS =========================
// Keep RAM static, avoid local stack usage
char esp_wifi_cmd[120];      // WiFi connect commands
char esp_http_buf[260];      // HTTP GET request
char esp_cmd_buf[40];        // CIP commands
char esp_log_buf[80];        // UART log buffer

// Sensor values
uint8_t humidity_int = 0;
uint8_t temp_int     = 0;

// ===================== SYSTEM CLOCK INIT ==========================
void SystemInit(void) {
    LPC_SC->SCS = (1 << 5);                      // Enable main oscillator
    while (!(LPC_SC->SCS & (1 << 6)));           // Wait for it to be ready

    LPC_SC->CCLKCFG = 0;                         // CCLK = 12 MHz / 1
    LPC_SC->CLKSRCSEL = 1;                       // Clock source = main oscillator
}

// ======================== UART0 (Debug) ===========================
void UART0_Init(void) {
    LPC_SC->PCONP |= (1 << 3);                   // Power UART0
    LPC_SC->PCLKSEL0 &= ~(3 << 6);
    LPC_SC->PCLKSEL0 |=  (1 << 6);               // PCLK = CCLK

    LPC_PINCON->PINSEL0 &= ~(0xF << 4);
    LPC_PINCON->PINSEL0 |=  (0x5 << 4);          // P0.2 TXD0, P0.3 RXD0

    LPC_UART0->LCR = 0x83;                       // 8N1 + DLAB
    LPC_UART0->DLM = 0; 
    LPC_UART0->DLL = 78;                         // 9600 baud @ 12 MHz
    LPC_UART0->LCR = 0x03;                       // Clear DLAB
    LPC_UART0->FCR = 0x07;                       // Enable FIFOs
}

void UART0_SendChar(char c) {
    while (!(LPC_UART0->LSR & THR_EMPTY));
    LPC_UART0->THR = c;
}

void UART0_SendString(const char *s) {
    while (*s) UART0_SendChar(*s++);
}

// ============================= UART2 ==============================
void UART2_Init(void) {
    LPC_SC->PCONP |= (1 << 24);                  // Power UART2

    LPC_PINCON->PINSEL0 &= ~((3 << 20) | (3 << 22));
    LPC_PINCON->PINSEL0 |=  ((1 << 20) | (1 << 22));   // P0.10 TXD2, P0.11 RXD2

    LPC_SC->PCLKSEL1 &= ~(3 << 16);
    LPC_SC->PCLKSEL1 |=  (1 << 16);              // PCLK = CCLK

    LPC_UART2->LCR = 0x83;                       // 8N1 + DLAB
    LPC_UART2->DLM = 0;
    LPC_UART2->DLL = 78;                         // 9600 baud
    LPC_UART2->LCR = 0x03;                       // Clear DLAB
    LPC_UART2->FCR = 0x07;

    /* For 115200 Baudrate at 12 MHz
    LPC_UART2->LCR = 0x83;      // DLAB = 1, 8N1
    LPC_UART2->DLM = 0;
    LPC_UART2->DLL = 4;         // DLL = 4
    LPC_UART2->FDR = (8 << 4) | 5;  // MULVAL=8, DIVADDVAL=5
    LPC_UART2->LCR = 0x03;      // Clear DLAB
    */
}

void UART2_SendChar(char ch) {
    while (!(LPC_UART2->LSR & THR_EMPTY));
    LPC_UART2->THR = ch;
}

void UART2_SendString(const char *s) {
    while (*s) UART2_SendChar(*s++);
}

uint8_t UART2_CharAvailable(void) {
    return (LPC_UART2->LSR & RDR_READY);
}

char UART2_ReceiveChar(void) {
    while (!UART2_CharAvailable());
    return LPC_UART2->RBR;
}

// =============================== TIMER0 ===========================
void Timer0_Init(void) {
    LPC_SC->PCONP |= (1 << 1);
    LPC_SC->PCLKSEL0 &= ~(3 << 2);
    LPC_SC->PCLKSEL0 |=  (1 << 2);               // PCLK = 12 MHz

    LPC_TIM0->CTCR = 0;
    LPC_TIM0->PR = 11;                            // 1us tick
    LPC_TIM0->TCR = 0x02;                         // Reset timer
}

void delay_us(uint32_t us) {
    LPC_TIM0->TCR = 0x02;                         // Reset
    LPC_TIM0->TCR = 0x01;                         // Enable
    while (LPC_TIM0->TC < us);
    LPC_TIM0->TCR = 0x00;                         // Stop
}

void delay_ms(uint32_t ms) {
    delay_us(ms * 1000);
}

// =========================== DHT11 DRIVER =========================
uint8_t DHT11_Read(void) {
    uint8_t data[5] = {0};
    uint32_t timeout;
    uint8_t i, j;

    // INPUT mode + pull-up
    LPC_PINCON->PINSEL0 &= ~(3 << 10); // P0.5 - GPIO
    LPC_PINCON->PINMODE0 &= ~(3 << 10); // on-chip pull-up resistor enabled

    // Start signal
    LPC_GPIO0->FIODIR |= DHT11_PIN;
    LPC_GPIO0->FIOCLR = DHT11_PIN;
    delay_ms(20);
    LPC_GPIO0->FIOSET = DHT11_PIN;
    delay_us(30);

    LPC_GPIO0->FIODIR &= ~DHT11_PIN;              // Input mode

    // Wait for response (low → high → low)
    timeout = 10000;
    while ((LPC_GPIO0->FIOPIN & DHT11_PIN) && --timeout) delay_us(1);
    if (!timeout) return 0;

    timeout = 10000;
    while (!(LPC_GPIO0->FIOPIN & DHT11_PIN) && --timeout) delay_us(1);
    if (!timeout) return 0;

    timeout = 10000;
    while ((LPC_GPIO0->FIOPIN & DHT11_PIN) && --timeout) delay_us(1);
    if (!timeout) return 0;

    // Read 5 bytes
    for (j = 0; j < 5; j++) {
        for (i = 0; i < 8; i++) {

            timeout = 10000;
            while (!(LPC_GPIO0->FIOPIN & DHT11_PIN) && --timeout) delay_us(1);

            delay_us(40);                         // Sample timing

            if (LPC_GPIO0->FIOPIN & DHT11_PIN) {
                data[j] |= (1 << (7 - i));

                timeout = 10000;
                while ((LPC_GPIO0->FIOPIN & DHT11_PIN) && --timeout) delay_us(1);
            }
        }
    }

    // Checksum
    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) == data[4]) {
        humidity_int = data[0];
        temp_int     = data[2];
        return 1;
    }
    return 0;
}

// ========================== ESP COMMANDS ==========================
// Reads and dumps whatever ESP sent
void ESP_ReadResponse(void) {
    UART0_SendString("[ESP] ");

    // Non-blocking flush
    uint32_t safe_timeout
    safe_timeout = 30000;               // Prevent infinite loop

    while (safe_timeout--) {
        if (UART2_CharAvailable()) {
            char c = UART2_ReceiveChar();
            UART0_SendChar(c);
        } else {
            delay_us(100);
        }
    }
    UART0_SendString("\r\n");
}

// ------------------------ WiFi Init ------------------------------
void ESP_Init(void) {
    UART0_SendString("[INFO] Initializing ESP8266...\r\n");

    UART2_SendString("AT\r\n");
    delay_ms(800);
    ESP_ReadResponse();

    UART2_SendString("AT+CWMODE=1\r\n");          // Station mode
    delay_ms(800);
    ESP_ReadResponse();

    // Build join command
    snprintf(esp_wifi_cmd, sizeof(esp_wifi_cmd),
             "AT+CWJAP=\"%s\",\"%s\"\r\n",
             WIFI_SSID, WIFI_PASS);

    UART2_SendString(esp_wifi_cmd);
    delay_ms(5500);                               // ESP needs time
    ESP_ReadResponse();

    UART2_SendString("AT+CWJAP_CUR?\r\n");
    delay_ms(1200);
    ESP_ReadResponse();

    UART0_SendString("[INFO] ESP Ready.\r\n");
}

// ------------------------ Send to ThingSpeak ---------------------
void ESP_SendToThingSpeak(uint8_t field1, uint8_t field2) {

    UART0_SendString("[INFO] Building ThingSpeak HTTP request...\r\n");

    // Build GET request
    snprintf(esp_http_buf, sizeof(esp_http_buf),
        "GET /update?api_key=%s&field1=%d&field2=%d\r\n",
        THINGSPEAK_KEY, field1, field2);

    UART0_SendString("[INFO] HTTP Request Ready.\r\n");
    UART0_SendString("[INFO] Opening TCP connection...\r\n");

    UART2_SendString("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");
    delay_ms(1500);
    ESP_ReadResponse();

    UART0_SendString("[INFO] TCP Connection established.\r\n");
    UART0_SendString("[INFO] Sending AT+CIPSEND length command...\r\n");

    // Send AT+CIPSEND=<length>
    snprintf(esp_cmd_buf, sizeof(esp_cmd_buf),
             "AT+CIPSEND=%d\r\n",
             (int)strlen(esp_http_buf));

    UART2_SendString(esp_cmd_buf);
    delay_ms(600);
    ESP_ReadResponse();

    UART0_SendString("[INFO] Sending HTTP GET request...\r\n");

    UART2_SendString(esp_http_buf);
    delay_ms(1500);
    ESP_ReadResponse();

    UART0_SendString("[INFO] Closing TCP connection...\r\n");

    UART2_SendString("AT+CIPCLOSE\r\n");
    delay_ms(600);
    ESP_ReadResponse();

    UART0_SendString("[INFO] ThingSpeak update complete.\r\n");
}

// ============================= MAIN ==============================
int main(void) {
    SystemInit(); // Changing CCLK to external 12 MHz Crystal
    Timer0_Init();
    UART0_Init();
    UART2_Init();
    ESP_Init();

    UART0_SendString("\r\n--- DHT11 Temperature and Humidity ---\r\n");

    while (1) {
        if (DHT11_Read()) {
            // Log using global buffer
            snprintf(esp_log_buf, sizeof(esp_log_buf),
                     "Temp: %d C, Humidity: %d %%\r\n",
                     temp_int, humidity_int);

            UART0_SendString(esp_log_buf);

            ESP_SendToThingSpeak(temp_int, humidity_int);
        } else {
            UART0_SendString("[ERROR] DHT11 read failed!\r\n");
        }

        delay_ms(15000);  // ThingSpeak limit
    }
}

