idf_component_register(SRCS "checkClass.cpp" "MFRC522.c" "rfid.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES driver freertos esp_system esp_event esp_wifi nvs_flash esp_netif
)
