
#include "esp_log.h"

#define LOG_ERROR(format, ...) ESP_LOGE(TAG, format " | [100m%s:%d[0m", ##__VA_ARGS__, __FILE__, __LINE__);
#define LOG_WARNING(format, ...) ESP_LOGW(TAG, format " | [100m%s:%d[0m", ##__VA_ARGS__, __FILE__, __LINE__);
#define LOG_INFO(format, ...) ESP_LOGI(TAG, format " | [100m%s:%d[0m", ##__VA_ARGS__, __FILE__, __LINE__);
#define LOG_VERBOSE(format, ...) ESP_LOGV(TAG, format " | [100m%s:%d[0m", ##__VA_ARGS__, __FILE__, __LINE__);
#define LOG_DEBUG(format, ...) ESP_LOGD(TAG, format " | [100m%s:%d[0m", ##__VA_ARGS__, __FILE__, __LINE__);
