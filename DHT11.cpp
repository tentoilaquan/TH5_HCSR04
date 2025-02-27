#include <dht.h>  // Thư viện hỗ trợ DHT

#define DHT11_PIN 2  // Chân kết nối cảm biến DHT11

dht DHT;  // Tạo đối tượng DHT

void setup() {
    Serial.begin(9600);  // Khởi tạo Serial Monitor
    Serial.println("Đọc dữ liệu từ cảm biến DHT11...");
}

void loop() {
    int chk = DHT.read11(DHT11_PIN);  // Đọc dữ liệu từ cảm biến

    Serial.print("Nhiệt độ: ");
    Serial.print(DHT.temperature);  // Hiển thị nhiệt độ
    Serial.println(" °C");

    Serial.print("Độ ẩm: ");
    Serial.print(DHT.humidity);  // Hiển thị độ ẩm
    Serial.println(" %");

    delay(2000);  // Chờ 2 giây trước lần đọc tiếp theo
}
