// #include <Arduino.h>
#include <inttypes.h>
#include "esp_heap_caps.h"
#include "esp_chip_info.h"
#include "esp_system.h"
#include "esp_log.h"
#include "soc/rtc.h"	//real time clock
#include "esp_pm.h"		//power management
#include "esp_partition.h"
#include "esp_system.h"
#include "spi_flash_mmap.h"

esp_chip_info_t chip_info;

const char* get_chip_model(esp_chip_model_t model);
void print_chip_info(void);
void print_chip_features(void);
void print_freq_info(void);
void print_memory_sizes(void);

void app_main(void) {

    ESP_LOGI("ESP32 MCU Info", "---------------------------------------------------------");

    print_chip_info();

    print_freq_info();

	print_chip_features();

    print_memory_sizes();

    ESP_LOGI("ESP32 MCU Info", "---------------------------------------------------------");

}

void print_chip_info(void) {
    esp_chip_info(&chip_info);

    ESP_LOGI("Chip Info", "Chip Model: %s", get_chip_model(chip_info.model));
    ESP_LOGI("Chip Info", "Cores: %d", chip_info.cores);
    ESP_LOGI("Chip Info", "Revision number: %d", chip_info.revision);
}

void print_freq_info(void) {
    rtc_cpu_freq_config_t freq_config;
    rtc_clk_cpu_freq_get_config(&freq_config);

    // Reporting the CPU clock source
    const char* clk_source_str;
    switch (freq_config.source) {
        case SOC_CPU_CLK_SRC_PLL:
            clk_source_str = "PLL";
            break;
        case SOC_CPU_CLK_SRC_APLL:
            clk_source_str = "APLL";
            break;
        case SOC_CPU_CLK_SRC_XTAL:
            clk_source_str = "XTAL";
            break;
        default:
            clk_source_str = "Unknown";
            break;
    }
    ESP_LOGI("Chip Info", "CPU Clock Source: %s", clk_source_str);
    ESP_LOGI("Chip Info", "Source Clock Frequency: %" PRIu32 " MHz", freq_config.source_freq_mhz);
    ESP_LOGI("Chip Info", "Divider: %" PRIu32, freq_config.div);
    ESP_LOGI("Chip Info", "Effective CPU Frequency: %" PRIu32 " MHz", freq_config.freq_mhz);
}

// determined based on the constants defined in esp_chip_info.h
void print_chip_features(void) {
    uint32_t features = chip_info.features;

    char binary_str[33]; // 32 bits + null terminator
    for (int i = 31; i >= 0; i--) {
        binary_str[31 - i] = (features & (1U << i)) ? '1' : '0';
    }
    binary_str[32] = '\0'; // Null terminate the string
    ESP_LOGI("Chip Info", "Features Bitmap: %s", binary_str);

    ESP_LOGI("Chip Info", "Embedded Flash: %s", (features & CHIP_FEATURE_EMB_FLASH) ? "Yes" : "No");
    ESP_LOGI("Chip Info", "Embedded PSRAM: %s", (features & CHIP_FEATURE_EMB_PSRAM) ? "Yes" : "No");
    ESP_LOGI("Chip Info", "Wi-Fi 2.4GHz support: %s", (features & CHIP_FEATURE_WIFI_BGN) ? "Yes" : "No");
    ESP_LOGI("Chip Info", "IEEE 802.15.4 support: %s", (features & CHIP_FEATURE_IEEE802154) ? "Yes" : "No");
    ESP_LOGI("Chip Info", "Bluetooth Classic support: %s", (features & CHIP_FEATURE_BT) ? "Yes" : "No");
    ESP_LOGI("Chip Info", "Bluetooth LE (BLE) support: %s", (features & CHIP_FEATURE_BLE) ? "Yes" : "No");
}

void print_memory_sizes(void) {

	// uint32_t flash_size = ESP.getFlashChipSize();
	// printf("--------> Flash size: %PRIu32 bytes\n", flash_size);

    // Flash Size
    const esp_partition_t* partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    if (partition) {
        ESP_LOGI("Memory Info", "Found App partition");
        ESP_LOGI("Memory Info", "Partition Label: %s", partition->label);
        ESP_LOGI("Memory Info", "Partition Type: %d", partition->type);
        ESP_LOGI("Memory Info", "Partition Subtype: %d", partition->subtype);
        ESP_LOGI("Memory Info", "Partition Size: %" PRIu32 " bytes", partition->size);
    } else {
        ESP_LOGE("Memory Info", "Failed to get the App partition");
    }

    // Total SPIRAM (PSRAM) Size
    size_t spiram_size = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    if (spiram_size) {
        ESP_LOGI("Memory Info", "PSRAM Size: %zu bytes", spiram_size);
    } else {
        ESP_LOGI("Memory Info", "No PSRAM detected");
    }

    uint32_t total_internal_memory = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
    uint32_t free_internal_memory = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    uint32_t largest_contig_internal_block = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);

    ESP_LOGI("Memory Info", "Total DRAM (internal memory): %"PRIu32" bytes", total_internal_memory);
    ESP_LOGI("Memory Info", "Free DRAM (internal memory): %"PRIu32" bytes", free_internal_memory);
    ESP_LOGI("Memory Info", "Largest free contiguous DRAM block: %"PRIu32" bytes", largest_contig_internal_block);

}

// determined based on the constants defined in esp_chip_info.h
const char* get_chip_model(esp_chip_model_t model) {
    switch (model) {
        case CHIP_ESP32:
            return "ESP32";
        case CHIP_ESP32S2:
            return "ESP32-S2";
        case CHIP_ESP32S3:
            return "ESP32-S3";
        case CHIP_ESP32C3:
            return "ESP32-C3";
        case CHIP_ESP32C2:
            return "ESP32-C2";
        case CHIP_ESP32C6:
            return "ESP32-C6";
        case CHIP_ESP32H2:
            return "ESP32-H2";
        case CHIP_ESP32P4:
            return "ESP32-P4";
        case CHIP_POSIX_LINUX:
            return "POSIX/Linux Simulator";
        default:
            return "Unknown Model";
    }
}

