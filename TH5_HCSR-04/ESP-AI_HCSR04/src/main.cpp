#include <Arduino.h>
#include <ai-hcsr04_inferencing.h>  // Thư viện Edge Impulse bạn đã cài

#define FREQUENCY_HZ   60
#define INTERVAL_MS    (1000 / (FREQUENCY_HZ + 1))

#define TRIG_PIN 13
#define ECHO_PIN 14

unsigned long last_interval_ms = 0;

// Buffer AI với kích thước cố định từ Edge Impulse
float ei_buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
size_t buffer_index = 0;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  delay(50);

  Serial.println("HC-SR04 + Edge Impulse ready!");
}

float readDistanceCM() {
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);
  if (duration == 0) return NAN;

  return (duration / 2.0) / 29.1;
}

void loop() {
  unsigned long now = millis();
  if (now - last_interval_ms >= INTERVAL_MS) {
    last_interval_ms = now;

    float distance_cm = readDistanceCM();
    if (!isnan(distance_cm)) {
      ei_buffer[buffer_index++] = distance_cm;
      // Đã loại bỏ dòng in khoảng cách
    }

    if (buffer_index >= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
      signal_t signal;
      int err = numpy::signal_from_buffer(ei_buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
      if (err != 0) {
        Serial.printf("Lỗi tạo tín hiệu: %d\n", err);
        buffer_index = 0;
        return;
      }

      ei_impulse_result_t result;
      EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
      if (res != EI_IMPULSE_OK) {
        Serial.printf("Lỗi phân loại: %d\n", res);
        buffer_index = 0;
        return;
      }

      // Chỉ in kết quả phân loại, không in buffer
      for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        Serial.printf("%s: %.2f\n", result.classification[i].label, result.classification[i].value);
      }
      Serial.println("--------------------");

      buffer_index = 0;
    }
  }
}
