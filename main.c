#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"

// Definições de pinos
#define BOTAO_A 5
#define JOYSTICK_X 27
#define JOYSTICK_Y 26


#define THINGSPEAK_CHAVE_API "INUQZ0L4U83FUAR9"
#define THINGSPEAK_HOST "api.thingspeak.com"

#define WIFI_SSID "LINE"
#define WIFI_SENHA "SANTANA0411"

// Função para ler a temperatura do sensor interno
float ler_temperatura() {
    adc_select_input(4);  // Canal 4 é o sensor de temperatura interno
    uint16_t valor_bruto = adc_read();
    const float fator_conversao = 3.3f / (1 << 12);
    float tensao = valor_bruto * fator_conversao;
    float temperatura = 27.0f - (tensao - 0.706f) / 0.001721f;
    return temperatura;
}

typedef struct {
    struct tcp_pcb *pcb;
    char requisicao[256];
} requisicao_thingspeak_t;

err_t dados_enviados(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    printf("Dados enviados ao Servidor com sucesso!\n");
    tcp_close(tpcb);
    free(arg);
    return ERR_OK;
}

err_t conexao_estabelecida(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {
        printf("Erro na conexão TCP: %d\n", err);
        tcp_close(tpcb);
        free(arg);
        return err;
    }

    requisicao_thingspeak_t *req = (requisicao_thingspeak_t *)arg;
    tcp_write(tpcb, req->requisicao, strlen(req->requisicao), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    tcp_sent(tpcb, dados_enviados);

    return ERR_OK;
}

void callback_dns(const char *nome, const ip_addr_t *ipaddr, void *arg) {
    if (!ipaddr) {
        printf("Falha na resolução de DNS para %s\n", nome);
        free(arg);
        return;
    }

    requisicao_thingspeak_t *req = (requisicao_thingspeak_t *)arg;
    if (tcp_connect(req->pcb, ipaddr, 80, conexao_estabelecida) != ERR_OK) {
        printf("Erro ao conectar após DNS\n");
        tcp_close(req->pcb);
        free(req);
    }
}

void enviar_para_thingspeak() {
    

    // Lê os estados dos botões
    bool b1 = !gpio_get(BOTAO_A);
   

    printf("Estado dos botão - Botão_A: %d\n", b1);

    float temp = ler_temperatura();

    adc_select_input(1);
    int joy_x = adc_read();
    adc_select_input(0);
    int joy_y = adc_read();

    printf("Joystick X: %d, Joystick Y: %d\n", joy_x, joy_y);

    requisicao_thingspeak_t *req = malloc(sizeof(requisicao_thingspeak_t));
    if (!req) return;

    req->pcb = tcp_new();
    if (!req->pcb) {
        free(req);
        return;
    }

    snprintf(req->requisicao, sizeof(req->requisicao),
        "GET /update?api_key=%s&field1=%d&field3=%.2f&field4=%d&field5=%d HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n\r\n",
        THINGSPEAK_CHAVE_API, b1, temp, joy_x, joy_y, THINGSPEAK_HOST);

    printf("Enviando para ThingSpeak: %s\n", req->requisicao);

    ip_addr_t ipaddr;
    err_t erro_dns = dns_gethostbyname(THINGSPEAK_HOST, &ipaddr, callback_dns, req);

    if (erro_dns == ERR_OK) {
        if (tcp_connect(req->pcb, &ipaddr, 80, conexao_estabelecida) == ERR_OK) {
            tcp_write(req->pcb, req->requisicao, strlen(req->requisicao), TCP_WRITE_FLAG_COPY);
            tcp_output(req->pcb);
            tcp_sent(req->pcb, dados_enviados);
        } else {
            printf("Erro ao conectar (resolução cache)\n");
            tcp_close(req->pcb);
            free(req);
        }
    } else if (erro_dns != ERR_INPROGRESS) {
        printf("Erro ao iniciar resolução de DNS: %d\n", erro_dns);
        tcp_close(req->pcb);
        free(req);
    }
}

int main() {
    stdio_init_all();
    adc_init();

    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);


    adc_gpio_init(JOYSTICK_X);
    adc_gpio_init(JOYSTICK_Y);

    if (cyw43_arch_init()) {
        printf("Erro ao inicializar Wi-Fi\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_SENHA, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        return -1;
    }

    printf("Conectado ao Wi-Fi!\n");

    while (true) {
        enviar_para_thingspeak();
        sleep_ms(15000); 
    }

    return 0;
}