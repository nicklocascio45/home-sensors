#ifndef PIR_H
#define PIR_H

#define PIR_PIN			6

esp_err_t pir_init(EventGroupHandle_t motion_event_group);

#endif // PIR_H
