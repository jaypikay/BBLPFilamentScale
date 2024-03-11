#include <ESPUI.h>
#include <umm_malloc/umm_heap_select.h>

#include "utils.h"
#include "nfc.h"
#include "scale.h"
#include "ui.h"

const char *LabelNotDetected = "Not detected";
const char *LabelModuleInstalled = "Installed";

uint16_t numberScaleOffset;
uint16_t labelScaleValue;

uint16_t labelSpoolId;

void callbackUpdateLoadCellOffset(Control *sender, int type);

void setupUI()
{
    Serial.println("*UI: Setting up web interface...");
#ifdef DEBUG
    ESPUI.setVerbosity(Verbosity::Verbose);
#endif
#ifdef ESP8266
    {
        HeapSelectIram doAllocationsInIRAM;
#endif

        uint16_t tab_dashboard = ESPUI.addControl(ControlType::Tab, "Dashboard", "Dashboard");

        if (has_scale)
        {
            debug_println("*UI: Adding Scale tab...");
            uint16_t tab_scale = ESPUI.addControl(ControlType::Tab, "Scale", "Scale");

            labelScaleValue = ESPUI.addControl(ControlType::Label, "Load cell value", "", ControlColor::Wetasphalt, tab_scale);
            numberScaleOffset = ESPUI.addControl(ControlType::Number, "Load cell offset", "n/a", ControlColor::Wetasphalt, tab_scale, &callbackUpdateLoadCellOffset);
        }

        if (has_nfc)
        {
            debug_println("*UI: Adding NFC tab...");
            uint16_t tab_nfc = ESPUI.addControl(ControlType::Tab, "NFC", "NFC");

            labelSpoolId = ESPUI.addControl(ControlType::Label, "Spool ID", "n/a", ControlColor::Wetasphalt, tab_nfc);
        }

        uint16_t tab_settings = ESPUI.addControl(ControlType::Tab, "Settings", "Settings");

        ESPUI.addControl(ControlType::Separator, "Modules", "", ControlColor::None, tab_settings);
        ESPUI.addControl(ControlType::Label, "Scale",
                         has_scale ? LabelModuleInstalled : LabelNotDetected,
                         has_scale ? ControlColor::Emerald : ControlColor::Alizarin,
                         tab_settings);
        ESPUI.addControl(ControlType::Label, "NFC Module",
                         has_nfc ? LabelModuleInstalled : LabelNotDetected,
                         has_nfc ? ControlColor::Emerald : ControlColor::Alizarin,
                         tab_settings);

        ESPUI.addControl(ControlType::Separator, "WiFi Settings", "", ControlColor::None, tab_settings);
        ESPUI.addControl(ControlType::Label, "TODO", "TODO", ControlColor::Sunflower, tab_settings);

        ESPUI.begin("BBLP Filament Scale WebUI");

#ifdef ESP8266
    } // HeapSelectIram
#endif
}

void callbackUpdateLoadCellOffset(Control *sender, int type) {
    scale_offset = sender->value.toInt();
}

void requestSpoolUpdate(uint32_t spoolId) 
{
    ESPUI.print(labelSpoolId, String(spoolId));
}

void handleUi() {
    ESPUI.print(labelScaleValue, String(current_weight));
};