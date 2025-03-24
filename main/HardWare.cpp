//
// Created by Wind on 2025/3/17.
//

#include <esp_check.h>
#include <esp_lcd_panel_st7789.h>
#include <esp_lcd_panel_ops.h>
#include <driver/ledc.h>
#include "HardWare.h"
#include "freertos/FreeRTOS.h"
#include "driver/i2c_master.h"
#include "utilities.h"
#include "esp_lcd_touch_ft5x06.h"
#include "esp_lcd_io_spi.h"
#include "esp_lvgl_port.h"
#include "MsgController.h"

HardWare* hardWare;
uint32_t HardWare::brightness = 0;
uint32_t HardWare::batteryPercent = 100;
const char* TAG = "HardWare";

HardWare::HardWare()
{
    PMU = XPowersAXP2101();
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    if (HardWare::brightness == 0)
    {
        HardWare::brightness = 100;
    }
    ledc_timer_config(&ledc_timer);
    ledc_channel_config_t ledc_channel = {
        .gpio_num = BOARD_TFT_BL,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = brightness,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel);
    ledc_fade_func_install(0);
    i2c_master_bus_config_t Sensor_i2c_bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = GPIO_NUM_10,
        .scl_io_num = GPIO_NUM_11,
        .clk_source = I2C_CLK_SRC_RC_FAST,
        .glitch_ignore_cnt = 7,
        .intr_priority = 1,
        .flags = {
            .enable_internal_pullup = true,
            .allow_pd = false,
        }
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&Sensor_i2c_bus_config, &this->Sensor_i2c_bus_handle));
    i2c_master_bus_config_t TOUCH_i2c_bus_config{};
    TOUCH_i2c_bus_config.i2c_port = I2C_NUM_1;
    TOUCH_i2c_bus_config.sda_io_num = BOARD_TOUCH_SDA;
    TOUCH_i2c_bus_config.scl_io_num = BOARD_TOUCH_SCL;
    TOUCH_i2c_bus_config.clk_source = I2C_CLK_SRC_RC_FAST;
    TOUCH_i2c_bus_config.glitch_ignore_cnt = 7;
    TOUCH_i2c_bus_config.intr_priority = 2;
    TOUCH_i2c_bus_config.flags = {
        .enable_internal_pullup = true,
        .allow_pd = false,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&TOUCH_i2c_bus_config, &this->Touch_i2c_bus_handle));
    ESP_LOGD(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg{};
    this->LCD_spi_bus_handle = SPI2_HOST;
    buscfg.mosi_io_num = BOARD_TFT_MOSI;
    buscfg.miso_io_num = BOARD_TFT_MISO;
    buscfg.sclk_io_num = BOARD_TFT_SCLK;
    buscfg.quadwp_io_num = GPIO_NUM_NC;
    buscfg.quadhd_io_num = GPIO_NUM_NC;
    buscfg.max_transfer_sz = BOARD_TFT_WIDTH * 20;
    ESP_ERROR_CHECK(spi_bus_initialize(this->LCD_spi_bus_handle, &buscfg, SPI_DMA_CH_AUTO));
    xTaskCreate(Irq_Task, "Sensor/Irq_Task", 2048, this, 2, &Irq_Task_Handle);
}

HardWare::~HardWare() = default;


void HardWare::PMU_init()
{
    bool ret;

    for (int i = 0; i <= 127; i++)
    {
        // 尝试通过向设备发送一个空数据来探测设备
        esp_err_t res = i2c_master_probe(Sensor_i2c_bus_handle, i, 100);
        if (res == ESP_OK)
        {
            ESP_LOGI(TAG, "I2C device found at address 0x%02X", i);
        }
    }
    if (Sensor_i2c_bus_handle)
    {
        ESP_ERROR_CHECK(i2c_master_probe(Sensor_i2c_bus_handle, 0x34, 2000));
        ret = PMU.begin(Sensor_i2c_bus_handle, 0x34);
        if (!ret)
        {
            while (true)
            {
                ESP_LOGI("PMU", "i2c init failed");
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }
    }

    ret = PMU.init();

    if (ret)
    {
        PMU.setVbusVoltageLimit(XPOWERS_AXP2101_VBUS_VOL_LIM_4V36);
        PMU.setVbusCurrentLimit(XPOWERS_AXP2101_VBUS_CUR_LIM_900MA);
        PMU.setSysPowerDownVoltage(2600);
        PMU.setALDO1Voltage(3300);
        PMU.setALDO2Voltage(3300);
        PMU.setALDO3Voltage(3300);
        PMU.setALDO4Voltage(3300);
        PMU.setBLDO2Voltage(3300);
        PMU.setDC3Voltage(3300);
        PMU.enableDC3();
        PMU.disableDC2();
        PMU.disableDC4();
        PMU.disableDC5();
        PMU.disableBLDO1();
        PMU.disableCPUSLDO();
        PMU.enableDLDO1();
        PMU.disableDLDO2();
        PMU.enableALDO1(); //! RTC VBAT
        PMU.disableALDO2(); //! TFT BACKLIGHT   VDD
        PMU.enableALDO3(); //! Screen touch VDD
        PMU.enableALDO4(); //! Radio VDD
        PMU.enableBLDO2(); //! drv2605 enable
        PMU.setPowerKeyPressOffTime(XPOWERS_POWEROFF_10S);
        PMU.setPowerKeyPressOnTime(XPOWERS_POWERON_2S);
        PMU.disableTSPinMeasure();
        PMU.enableBattDetection();
        PMU.enableVbusVoltageMeasure();
        PMU.enableBattVoltageMeasure();
        PMU.enableSystemVoltageMeasure();
        PMU.setChargingLedMode(XPOWERS_CHG_LED_OFF);
        PMU.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
        PMU.enableIRQ(
            XPOWERS_AXP2101_BAT_INSERT_IRQ | XPOWERS_AXP2101_BAT_REMOVE_IRQ | //BATTERY
            XPOWERS_AXP2101_VBUS_INSERT_IRQ | XPOWERS_AXP2101_VBUS_REMOVE_IRQ | //VBUS
            XPOWERS_AXP2101_PKEY_SHORT_IRQ | XPOWERS_AXP2101_PKEY_LONG_IRQ | //POWER KEY
            XPOWERS_AXP2101_BAT_CHG_DONE_IRQ | XPOWERS_AXP2101_BAT_CHG_START_IRQ //CHARGE
        ); //CHARGE
        gpio_config_t pmu_int;
        pmu_int.intr_type = GPIO_INTR_NEGEDGE;
        pmu_int.mode = GPIO_MODE_INPUT;
        pmu_int.pin_bit_mask = (1 << BOARD_PMU_INT);
        pmu_int.pull_up_en = GPIO_PULLUP_DISABLE; // 启用上拉电阻
        pmu_int.pull_down_en = GPIO_PULLDOWN_DISABLE;
        gpio_config(&pmu_int);
        gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
        gpio_isr_handler_add((gpio_num_t)BOARD_PMU_INT, PMU_irq, this);
        ESP_LOGI(TAG, "DCDC=======================================================================\n");
        ESP_LOGI(TAG, "DC1  : %s   Voltage:%u mV \n", PMU.isEnableDC1() ? "+" : "-", PMU.getDC1Voltage());
        ESP_LOGI(TAG, "DC2  : %s   Voltage:%u mV \n", PMU.isEnableDC2() ? "+" : "-", PMU.getDC2Voltage());
        ESP_LOGI(TAG, "DC3  : %s   Voltage:%u mV \n", PMU.isEnableDC3() ? "+" : "-", PMU.getDC3Voltage());
        ESP_LOGI(TAG, "DC4  : %s   Voltage:%u mV \n", PMU.isEnableDC4() ? "+" : "-", PMU.getDC4Voltage());
        ESP_LOGI(TAG, "DC5  : %s   Voltage:%u mV \n", PMU.isEnableDC5() ? "+" : "-", PMU.getDC5Voltage());
        ESP_LOGI(TAG, "ALDO=======================================================================\n");
        ESP_LOGI(TAG, "ALDO1: %s   Voltage:%u mV\n", PMU.isEnableALDO1() ? "+" : "-", PMU.getALDO1Voltage());
        ESP_LOGI(TAG, "ALDO2: %s   Voltage:%u mV\n", PMU.isEnableALDO2() ? "+" : "-", PMU.getALDO2Voltage());
        ESP_LOGI(TAG, "ALDO3: %s   Voltage:%u mV\n", PMU.isEnableALDO3() ? "+" : "-", PMU.getALDO3Voltage());
        ESP_LOGI(TAG, "ALDO4: %s   Voltage:%u mV\n", PMU.isEnableALDO4() ? "+" : "-", PMU.getALDO4Voltage());
        ESP_LOGI(TAG, "BLDO=======================================================================\n");
        ESP_LOGI(TAG, "BLDO1: %s   Voltage:%u mV\n", PMU.isEnableBLDO1() ? "+" : "-", PMU.getBLDO1Voltage());
        ESP_LOGI(TAG, "BLDO2: %s   Voltage:%u mV\n", PMU.isEnableBLDO2() ? "+" : "-", PMU.getBLDO2Voltage());
        ESP_LOGI(TAG, "CPUSLDO====================================================================\n");
        ESP_LOGI(TAG, "CPUSLDO: %s Voltage:%u mV\n", PMU.isEnableCPUSLDO() ? "+" : "-", PMU.getCPUSLDOVoltage());
        ESP_LOGI(TAG, "DLDO=======================================================================\n");
        ESP_LOGI(TAG, "DLDO1: %s   Voltage:%u mV\n", PMU.isEnableDLDO1() ? "+" : "-", PMU.getDLDO1Voltage());
        ESP_LOGI(TAG, "DLDO2: %s   Voltage:%u mV\n", PMU.isEnableDLDO2() ? "+" : "-", PMU.getDLDO2Voltage());
        ESP_LOGI(TAG, "===========================================================================\n");
        PMU.clearIrqStatus();
        PMU.setPrechargeCurr(XPOWERS_AXP2101_PRECHARGE_50MA);
        PMU.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_300MA);
        PMU.setChargerTerminationCurr(XPOWERS_AXP2101_CHG_ITERM_25MA);
        PMU.setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V35);
        PMU.setButtonBatteryChargeVoltage(3300);
        PMU.enableButtonBatteryCharge();
        batteryPercent = PMU.getBatteryPercent();
    }
    else
    {
        while (true)
        {
            ESP_LOGI("PMU", "init failed");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

void HardWare::Drv2605_init()
{
    bool ret = Motor.begin(this->Sensor_i2c_bus_handle,DRV2605_SLAVE_ADDRESS);
    if (ret)
    {
        ESP_LOGI("Drv2605", "Init successfully...");
        Motor.selectLibrary(1);
        Motor.useERM();
        Motor.setWaveform(0, 15); // play effect
        Motor.setWaveform(1, 0); // end waveform
        Motor.run();
    }
    else
    {
        ESP_LOGE("Drv2605", "Init failed...");
    }
}


void HardWare::powerIoctl(enum PowerCtrlChannel ch, bool enable)
{
    switch (ch)
    {
    case WATCH_POWER_DISPLAY_BL:
        enable ? PMU.enableALDO2() : PMU.disableALDO2();
        break;
    case WATCH_POWER_TOUCH_DISP:
        enable ? PMU.enableALDO3() : PMU.disableALDO3();
        break;
    case WATCH_POWER_RADIO:
        enable ? PMU.enableALDO4() : PMU.disableALDO4();
        break;
    case WATCH_POWER_DRV2605:
        enable ? PMU.enableBLDO2() : PMU.disableBLDO2();
        break;
    case WATCH_POWER_GPS:
        enable ? PMU.enableDC3() : PMU.disableDC3();
        break;
    default:
        break;
    }
}

void HardWare::LCD_init()
{
    /* LCD backlight */
    /* LCD initialization */
    ESP_LOGD(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t lcd_io;
    esp_lcd_panel_handle_t lcd_panel;
    esp_lcd_panel_io_spi_config_t io_config{};
    io_config.cs_gpio_num = BOARD_TFT_CS;
    io_config.dc_gpio_num = BOARD_TFT_DC;
    io_config.spi_mode = 0;
    io_config.pclk_hz = 80 * 1000 * 1000;
    io_config.trans_queue_depth = 6;
    io_config.lcd_cmd_bits = 8;
    io_config.lcd_param_bits = 8;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(this->LCD_spi_bus_handle, &io_config, &lcd_io));

    ESP_LOGD(TAG, "Install LCD driver");
    esp_lcd_panel_dev_config_t panel_config{};
    panel_config.reset_gpio_num = BOARD_TFT_RST;
    panel_config.color_space = ESP_LCD_COLOR_SPACE_RGB;
    panel_config.bits_per_pixel = 16;
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(lcd_io, &panel_config, &lcd_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(lcd_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(lcd_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(lcd_panel, false));
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(lcd_panel, 0, 80));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(lcd_panel, true, true));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(lcd_panel, true));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(lcd_panel, true));

    /* LCD backlight on */
    ESP_ERROR_CHECK(gpio_set_level((gpio_num_t)BOARD_TFT_BL, 1));
    lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_cfg.task_affinity = 1;
    lvgl_cfg.task_priority = 1;
    lvgl_port_init(&lvgl_cfg);
    lvgl_port_display_cfg_t disp_cfg{};
    disp_cfg.io_handle = lcd_io;
    disp_cfg.panel_handle = lcd_panel;
    disp_cfg.buffer_size = BOARD_TFT_HEIHT * BOARD_TFT_WIDTH;
    disp_cfg.double_buffer = true;
    disp_cfg.hres = BOARD_TFT_HEIHT;
    disp_cfg.vres = BOARD_TFT_WIDTH;
    disp_cfg.monochrome = false;
#if LVGL_VERSION_MAJOR >= 9
    disp_cfg.color_format = LV_COLOR_FORMAT_RGB565;
#endif
    disp_cfg.rotation = {
        .swap_xy = false,
        .mirror_x = false,
        .mirror_y = true,
    };
    disp_cfg.flags = {
        .buff_dma = true,
        .buff_spiram = true,
        .sw_rotate = true,
#if LVGL_VERSION_MAJOR >= 9
        .swap_bytes = true,
#endif
        .direct_mode = false
    };
    disp_handle = lvgl_port_add_disp(&disp_cfg);
    esp_lcd_panel_io_handle_t tp_io_handle = nullptr;
    esp_lcd_panel_io_i2c_config_t io_cfg = ESP_LCD_TOUCH_IO_I2C_FT5x06_CONFIG();
    io_cfg.scl_speed_hz = 400000;
    esp_lcd_new_panel_io_i2c(Touch_i2c_bus_handle, &io_cfg, &tp_io_handle);
    esp_lcd_touch_handle_t tp;
    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = BOARD_TFT_WIDTH,
        .y_max = BOARD_TFT_HEIHT,
        .rst_gpio_num = GPIO_NUM_NC,
        .int_gpio_num = GPIO_NUM_NC,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };
    esp_lcd_touch_new_i2c_ft5x06(tp_io_handle, &tp_cfg, &tp);
    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = disp_handle,
        .handle = tp,
    };
    lvgl_touch_indev = lvgl_port_add_touch(&touch_cfg);
    this->setBrightness(HardWare::brightness);
    this->standby_flag = false;
    powerIoctl(WATCH_POWER_DISPLAY_BL, true);
}

void HardWare::setBrightness(uint8_t value, int time)
{
    if (this->standby_flag)
    {
        lvgl_port_resume(); //恢复LVGL任务
        powerIoctl(WATCH_POWER_DISPLAY_BL, true);
        this->standby_flag = false;
    }

    ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, value, time);
    ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
    HardWare::brightness = value;
}

void HardWare::setBrightness(uint8_t value)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, value);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    HardWare::brightness = value;
}

void IRAM_ATTR HardWare::PMU_irq(void* arg)
{
    auto* hardware = static_cast<HardWare*>(arg);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; // 是否会引发任务抢占
    xTaskNotifyFromISR(
        hardware->Irq_Task_Handle, // 要通知的任务
        1, // 通知的值
        eSetBits, // 设置通知值的动作
        &xHigherPriorityTaskWoken // 是否需要上下文切换
    );
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    //    hardware->pmu_irq_flag = true;
}

void HardWare::Irq_Task(void* arg)
{
    auto* hardware = static_cast<HardWare*>(arg);
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        hardware->PMU.getIrqStatus();
        if (hardware->PMU.isPekeyShortPressIrq())
        {
            msgController->push_msg(new Msg(Msg::PkeyShortPress));
        }
        else if (hardware->PMU.isPekeyLongPressIrq())
        {
            msgController->push_msg(new Msg(Msg::PkeyLongPress));
        }
        else if (hardware->PMU.isBatChargeStartIrq())
        {
            msgController->push_msg(new Msg(Msg::ChargeStart));
        }
        else if (hardware->PMU.isBatChargeDoneIrq())
        {
            msgController->push_msg(new Msg(Msg::ChargeDone));
        }
        else if (hardware->PMU.isVbusInsertIrq())
        {
            msgController->push_msg(new Msg(Msg::UsbPlugIn));
        }
        else if (hardware->PMU.isVbusRemoveIrq())
        {
            msgController->push_msg(new Msg(Msg::UsbPlugOut));
        }
        hardware->PMU.clearIrqStatus();
    }
}


