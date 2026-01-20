#include "helper.h"


#define CLOCK 4000000
#define ADC_SAMPLES 8
#define ADC_SAMPLE_PERIOD_MS 2
#define UART_PRINT_PERIOD_MS 100
#define UART_TX_BUF_SIZE 256

void led_bar(uint32_t duty_cycle);
static void uart_tx_enqueue(const char *buf, size_t len);
static void uart_tx_service(void);


uint16_t led1 = D0;
uint16_t led2 = D1;
uint16_t led3 = D2;
uint16_t led4 = D3;
uint16_t led5 = D4;
uint16_t led6 = D5;
uint16_t led7 = D6;
uint16_t led8 = D7;
uint16_t led9 = D8;
uint16_t led10 = D10;


volatile uint32_t s_ticks = 0;
static char s_uart_tx_buf[UART_TX_BUF_SIZE];
static uint16_t s_uart_tx_head = 0;
static uint16_t s_uart_tx_tail = 0;

int main(void)
{
    uart_init(UART1, 115200);
    systick_init(CLOCK / 1000);

    // ADC async setup
    uint16_t pot = A0;
    gpio_set_mode(pot, GPIO_MODE_ANALOG);
    adc_init_async_start(pot);

    // PWM setup
    const uint16_t buzzer = D9;
    struct timer *tim2 = TIM(2);
    gpio_set_mode(buzzer, GPIO_MODE_AF);
    gpio_set_af(buzzer, 1);
    setup_pwm(tim2);

    gpio_set_mode(led1, GPIO_MODE_OUTPUT);
    gpio_set_mode(led2, GPIO_MODE_OUTPUT);
    gpio_set_mode(led3, GPIO_MODE_OUTPUT);
    gpio_set_mode(led4, GPIO_MODE_OUTPUT);
    gpio_set_mode(led5, GPIO_MODE_OUTPUT);
    gpio_set_mode(led6, GPIO_MODE_OUTPUT);
    gpio_set_mode(led7, GPIO_MODE_OUTPUT);
    gpio_set_mode(led8, GPIO_MODE_OUTPUT);
    gpio_set_mode(led9, GPIO_MODE_OUTPUT);
    gpio_set_mode(led10, GPIO_MODE_OUTPUT);

    uint32_t duty_cycle = 0;
    uint32_t adc_timer = 0;
    uint32_t uart_timer = 0;
    uint32_t adc_acc = 0;
    uint16_t adc_avg = 0;
    uint8_t adc_count = 0;
    bool adc_inflight = false;
    bool adc_initialized = false;

    while (1)
    {
        uint32_t now = s_ticks;
        // Poll non-blocking ADC initialization
        if (!adc_initialized) {
            adc_initialized = adc_init_async_poll(now);
        }

        if (adc_initialized) {
            if (!adc_inflight && timer_expired(&adc_timer, ADC_SAMPLE_PERIOD_MS, now)) {
                adc_start();
                adc_inflight = true;
            }
            if (adc_inflight && adc_ready()) {
                uint16_t sample = adc_read_result();
                adc_inflight = false;
                adc_acc += sample;
                adc_count++;
            }
        }

        if (adc_count >= ADC_SAMPLES) {
            adc_avg = (uint16_t)(adc_acc / adc_count);
            adc_acc = 0;
            adc_count = 0;

            duty_cycle = (uint32_t)((adc_avg * 100U) / 4095U);
            led_bar(duty_cycle);
            set_duty_cycle(tim2, duty_cycle);
        }

        // Periodic UART report
        if (timer_expired(&uart_timer, UART_PRINT_PERIOD_MS, now)) {
            char msg[64];
            int len = snprintf(msg, sizeof(msg), "DUTY CYCLE VALUE: %lu\r\n", duty_cycle);
            if (len > 0) uart_tx_enqueue(msg, (size_t) len);
        }

        uart_tx_service();
    }
}

void led_bar(uint32_t duty_cycle)
{
    if (duty_cycle >= 90)
    {
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, true);
        gpio_write(led5, true);
        gpio_write(led6, true);
        gpio_write(led7, true);
        gpio_write(led8, true);
        gpio_write(led9, true);
        gpio_write(led10, true);
    }
    else if (duty_cycle >= 80)
    {
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, true);
        gpio_write(led5, true);
        gpio_write(led6, true);
        gpio_write(led7, true);
        gpio_write(led8, true);
        gpio_write(led9, true);
        gpio_write(led10, false);
    }
    else if (duty_cycle >= 70)
    {
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, true);
        gpio_write(led5, true);
        gpio_write(led6, true);
        gpio_write(led7, true);
        gpio_write(led8, true);
        gpio_write(led9, false);
        gpio_write(led10, false);
    }
    else if (duty_cycle >= 60)
    {
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, true);
        gpio_write(led5, true);
        gpio_write(led6, true);
        gpio_write(led7, true);
        gpio_write(led8, false);
        gpio_write(led9, false);
        gpio_write(led10, false);
    }
    else if (duty_cycle >= 50)
    {
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, true);
        gpio_write(led5, true);
        gpio_write(led6, true);
        gpio_write(led7, false);
        gpio_write(led8, false);
        gpio_write(led9, false);
        gpio_write(led10, false);
    }
    else if (duty_cycle >= 40)
    {
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, true);
        gpio_write(led5, true);
        gpio_write(led6, false);
        gpio_write(led7, false);
        gpio_write(led8, false);
        gpio_write(led9, false);
        gpio_write(led10, false);
    }
    else if (duty_cycle >= 30)
    {
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, true);
        gpio_write(led5, false);
        gpio_write(led6, false);
        gpio_write(led7, false);
        gpio_write(led8, false);
        gpio_write(led9, false);
        gpio_write(led10, false);
    }
    else if (duty_cycle >= 20)
    {
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, true);
        gpio_write(led4, false);
        gpio_write(led5, false);
        gpio_write(led6, false);
        gpio_write(led7, false);
        gpio_write(led8, false);
        gpio_write(led9, false);
        gpio_write(led10, false);
    }
    else if (duty_cycle >= 10)
    {
        gpio_write(led1, true);
        gpio_write(led2, true);
        gpio_write(led3, false);
        gpio_write(led4, false);
        gpio_write(led5, false);
        gpio_write(led6, false);
        gpio_write(led7, false);
        gpio_write(led8, false);
        gpio_write(led9, false);
        gpio_write(led10, false);
    }
    else if (duty_cycle >= 5)
    {
        gpio_write(led1, true);
        gpio_write(led2, false);
        gpio_write(led3, false);
        gpio_write(led4, false);
        gpio_write(led5, false);
        gpio_write(led6, false);
        gpio_write(led7, false);
        gpio_write(led8, false);
        gpio_write(led9, false);
        gpio_write(led10, false);
    }
    else
    {
        gpio_write(led1, false);
        gpio_write(led2, false);
        gpio_write(led3, false);
        gpio_write(led4, false);
        gpio_write(led5, false);
        gpio_write(led6, false);
        gpio_write(led7, false);
        gpio_write(led8, false);
        gpio_write(led9, false);
        gpio_write(led10, false);
    }
}

static void uart_tx_enqueue(const char *buf, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        uint16_t next = (uint16_t)((s_uart_tx_head + 1U) % UART_TX_BUF_SIZE);
        if (next == s_uart_tx_tail) break; // drop when buffer is full
        s_uart_tx_buf[s_uart_tx_head] = buf[i];
        s_uart_tx_head = next;
    }
}

static void uart_tx_service(void)
{
    while (s_uart_tx_tail != s_uart_tx_head && uart_write_ready(UART1)) {
        (void)uart_write_byte_nb(UART1, (uint8_t)s_uart_tx_buf[s_uart_tx_tail]);
        s_uart_tx_tail = (uint16_t)((s_uart_tx_tail + 1U) % UART_TX_BUF_SIZE);
    }
}
