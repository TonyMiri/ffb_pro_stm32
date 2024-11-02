#include "usbd_force_feedback_desc.h"

// Common descriptor field sizes
#define RPT_SIZE_8   0x75, 0x08  // REPORT_SIZE (8)
#define RPT_SIZE_16  0x75, 0x10  // REPORT_SIZE (16)
#define RPT_COUNT_1  0x95, 0x01  // REPORT_COUNT (1)
#define RPT_COUNT_2  0x95, 0x02  // REPORT_COUNT (2)

// Common ranges
#define MINMAX_0_TO_1    0x15, 0x00, 0x25, 0x01  // LOGICAL_MIN(0), LOGICAL_MAX(1)
#define MINMAX_1_TO_40   0x15, 0x01, 0x25, 0x28  // LOGICAL_MIN(1), LOGICAL_MAX(40)
#define MINMAX_0_TO_255  0x15, 0x00, 0x26, 0xFF, 0x00  // LOGICAL_MIN(0), LOGICAL_MAX(255)
#define MINMAX_NEG_255   0x15, 0x00, 0x25, 0xFF  // For signed values -255 to +255

const uint8_t USBD_FFB_ReportDesc[] = {
    // Application Collection (Joystick)
    0x05, 0x01,   // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,   // USAGE (Joystick)
    0xA1, 0x01,   // COLLECTION (Application)

    // Joystick Input Report
    0x85, 0x01,   // REPORT_ID (1)
    0x09, 0x01,   // USAGE (Pointer)
    0xA1, 0x00,   // COLLECTION (Physical)

    // X, Y Axes + Buttons
    0x09, 0x30,   // USAGE (X)
    0x09, 0x31,   // USAGE (Y)
    0x15, 0x00,   // LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x00,  // LOGICAL_MAXIMUM (255)
    0x35, 0x00,   // PHYSICAL_MINIMUM (0)
    0x46, 0xFF, 0x00,  // PHYSICAL_MAXIMUM (255)
    RPT_SIZE_8,   // REPORT_SIZE (8)
    0x95, 0x02,   // REPORT_COUNT (2)
    0x81, 0x02,   // INPUT (Data,Var,Abs)

    // Buttons
    0x05, 0x09,   // USAGE_PAGE (Button)
    0x19, 0x01,   // USAGE_MINIMUM (Button 1)
    0x29, 0x08,   // USAGE_MAXIMUM (Button 8)
    0x15, 0x00,   // LOGICAL_MINIMUM (0)
    0x25, 0x01,   // LOGICAL_MAXIMUM (1)
    0x75, 0x01,   // REPORT_SIZE (1)
    0x95, 0x08,   // REPORT_COUNT (8)
    0x81, 0x02,   // INPUT (Data,Var,Abs)

    0xC0,         // END COLLECTION (Physical)

    // Force Feedback Section
    0x05, 0x0F,   // USAGE_PAGE (Physical Interface)
    0x09, 0x92,   // USAGE (PID State Report)
    0xA1, 0x02,   // COLLECTION (Logical)
    0x85, 0x02,   // REPORT_ID (2)

    // Device Status
    0x09, 0x9F,   // USAGE (Device Paused)
    0x09, 0xA0,   // USAGE (Actuators Enabled)
    0x09, 0xA4,   // USAGE (Safety Switch)
    0x09, 0xA5,   // USAGE (Actuator Override)
    0x09, 0xA6,   // USAGE (Actuator Power)
    MINMAX_0_TO_1,
    0x75, 0x01,   // REPORT_SIZE (1)
    0x95, 0x05,   // REPORT_COUNT (5)
    0x81, 0x02,   // INPUT (Data,Var,Abs)
    
    // Padding
    0x95, 0x03,   // REPORT_COUNT (3)
    0x81, 0x03,   // INPUT (Cnst,Var,Abs)

    // Effect Block Index
    0x09, 0x94,   // USAGE (Effect Playing)
    0x09, 0x22,   // USAGE (Effect Block Index)
    0x15, 0x01,   // LOGICAL_MINIMUM (1)
    0x25, 0x28,   // LOGICAL_MAXIMUM (40)
    0x75, 0x07,   // REPORT_SIZE (7)
    RPT_COUNT_1,  // REPORT_COUNT (1)
    0x81, 0x02,   // INPUT (Data,Var,Abs)

    // Effect Operations
    0x09, 0x21,   // USAGE (Set Effect Report)
    0xA1, 0x02,   // COLLECTION (Logical)
    0x85, 0x01,   // REPORT_ID (1)
    0x09, 0x22,   // USAGE (Effect Block Index)
    MINMAX_1_TO_40,
    RPT_SIZE_8,
    RPT_COUNT_1,
    0x91, 0x02,   // OUTPUT (Data,Var,Abs)

    // Effect Types
    0x09, 0x25,   // USAGE (Effect Type)
    0xA1, 0x02,   // COLLECTION (Logical)
    0x09, 0x26,   // USAGE (ET Constant Force)
    0x09, 0x27,   // USAGE (ET Ramp)
    0x09, 0x30,   // USAGE (ET Square)
    0x09, 0x31,   // USAGE (ET Sine)
    0x09, 0x32,   // USAGE (ET Triangle)
    0x09, 0x33,   // USAGE (ET Sawtooth Up)
    0x09, 0x34,   // USAGE (ET Sawtooth Down)
    0x09, 0x40,   // USAGE (ET Spring)
    0x09, 0x41,   // USAGE (ET Damper)
    0x09, 0x42,   // USAGE (ET Inertia)
    0x09, 0x43,   // USAGE (ET Friction)
    0x25, 0x0B,   // LOGICAL_MAXIMUM (11)
    0x15, 0x01,   // LOGICAL_MINIMUM (1)
    RPT_SIZE_8,
    RPT_COUNT_1,
    0x91, 0x00,   // OUTPUT (Data)
    0xC0,         // END COLLECTION

    // Effect Parameters
    0x09, 0x50,   // USAGE (Duration)
    0x09, 0x54,   // USAGE (Trigger Button)
    0x09, 0x51,   // USAGE (Sample Period)
    0x15, 0x00,   // LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x7F,  // LOGICAL_MAXIMUM (32767)
    RPT_SIZE_16,  // REPORT_SIZE (16)
    0x95, 0x03,   // REPORT_COUNT (3)
    0x91, 0x02,   // OUTPUT (Data,Var,Abs)

    0x09, 0x52,   // USAGE (Gain)
    0x15, 0x00,   // LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x00,  // LOGICAL_MAXIMUM (255)
    RPT_SIZE_8,   // REPORT_SIZE (8)
    RPT_COUNT_1,  // REPORT_COUNT (1)
    0x91, 0x02,   // OUTPUT (Data,Var,Abs)

    // Direction
    0x09, 0x57,   // USAGE (Direction)
    0x15, 0x00,   // LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x00,  // LOGICAL_MAXIMUM (255)
    RPT_SIZE_8,   // REPORT_SIZE (8) 
    RPT_COUNT_1,  // REPORT_COUNT (1)
    0x91, 0x02,   // OUTPUT (Data,Var,Abs)

    0xC0,         // END COLLECTION (Logical)

    // Effect Operations
    0x09, 0xAB,   // USAGE (Effect Operation Report)
    0xA1, 0x02,   // COLLECTION (Logical)
    0x85, 0x0A,   // REPORT_ID (10)
    0x09, 0x22,   // USAGE (Effect Block Index)
    0x15, 0x01,   // LOGICAL_MINIMUM (1)
    0x25, 0x28,   // LOGICAL_MAXIMUM (40)
    RPT_SIZE_8,
    RPT_COUNT_1,
    0x91, 0x02,   // OUTPUT (Data,Var,Abs)

    0x09, 0x5B,   // USAGE (Operation)
    0x15, 0x01,   // LOGICAL_MINIMUM (1)
    0x25, 0x03,   // LOGICAL_MAXIMUM (3)
    RPT_SIZE_8,
    RPT_COUNT_1,
    0x91, 0x02,   // OUTPUT (Data,Var,Abs)
    0xC0,         // END COLLECTION

    // Device Control
    0x09, 0x96,   // USAGE (DC Device Control)
    0xA1, 0x02,   // COLLECTION (Logical)
    0x85, 0x0C,   // REPORT_ID (12)
    0x09, 0x97,   // USAGE (DC Device Control)
    0x15, 0x01,   // LOGICAL_MINIMUM (1)
    0x25, 0x04,   // LOGICAL_MAXIMUM (4)
    RPT_SIZE_8,
    RPT_COUNT_1,
    0x91, 0x02,   // OUTPUT (Data,Var,Abs)
    0xC0,         // END COLLECTION

    // Device Gain
    0x09, 0x98,   // USAGE (Device Gain Report)
    0xA1, 0x02,   // COLLECTION (Logical)
    0x85, 0x0D,   // REPORT_ID (13)
    0x09, 0x99,   // USAGE (Device Gain)
    0x15, 0x00,   // LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x00,  // LOGICAL_MAXIMUM (255)
    RPT_SIZE_8,
    RPT_COUNT_1,
    0x91, 0x02,   // OUTPUT (Data,Var,Abs)
    0xC0,         // END COLLECTION

    0xC0          // END COLLECTION (Application)
};

const uint16_t USBD_FFB_ReportDesc_Size = sizeof(USBD_FFB_ReportDesc);
