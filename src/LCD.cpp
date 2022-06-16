#include "Arduino.h"
#include "PinCofigs.h"

#include <SPI.h>
#include "esp32-hal-spi.h"
#include "soc/spi_reg.h"
#include "soc/spi_struct.h"
#include "soc/dport_reg.h"
#include "driver/periph_ctrl.h"

#define LCD_WIDTH 800
#define LCD_HEIGHT 480
#define LCD_PIXEL_CLOCK_HZ (40E6)

#define LCD_ACTIVE() digitalWrite(LCD_CS_PIN, LOW)
#define LCD_INACTIVE() digitalWrite(LCD_CS_PIN, HIGH)

#define LCD_BACKLIGHT_ON() digitalWrite(LCD_BL_PIN, HIGH)
#define LCD_BACKLIGHT_OFF() digitalWrite(LCD_BL_PIN, LOW)

static const uint32_t LCD_DATA_PIN_MASK = ~(
  (1UL << LCD_D0_PIN) |
  (1UL << LCD_D1_PIN) |
  (1UL << LCD_D2_PIN) |
  (1UL << LCD_D3_PIN) |
  (1UL << LCD_D4_PIN) |
  (1UL << LCD_D5_PIN) |
  (1UL << LCD_D6_PIN) |
  (1UL << LCD_D7_PIN)
);

void DirectIOWrite(uint8_t data) {
  GPIO.out = (GPIO.out & LCD_DATA_PIN_MASK) | 
                (
                  (((data >> 0) & 0x01) << LCD_D0_PIN) |
                  (((data >> 1) & 0x01) << LCD_D1_PIN) |
                  (((data >> 2) & 0x01) << LCD_D2_PIN) |
                  (((data >> 3) & 0x01) << LCD_D3_PIN) |
                  (((data >> 4) & 0x01) << LCD_D4_PIN) |
                  (((data >> 5) & 0x01) << LCD_D5_PIN) |
                  (((data >> 6) & 0x01) << LCD_D6_PIN) |
                  (((data >> 7) & 0x01) << LCD_D7_PIN)
                ) |
                (1 << LCD_DE_PIN) // Send data with DE HIGH*/
            ;
  GPIO.out_w1tc = 1 << LCD_DE_PIN; // DE set to LOW
}

void WriteComm(uint8_t data) {
  digitalWrite(LCD_DC_PIN, LOW);
  DirectIOWrite(data);
}

void WriteData(uint8_t data) {
  digitalWrite(LCD_DC_PIN, HIGH);
  DirectIOWrite(data);
}

// LCD-CAM module
volatile spi_dev_t *spi_dev = (volatile spi_dev_t *)(DR_REG_SPI2_BASE);

void SwitchToGPIOMode() ;
void SwitchToSPIMode() ;

void LCD_init() {
  Serial.begin(115200);

  pinMode(LCD_CS_PIN, OUTPUT);
  LCD_INACTIVE();

  SwitchToGPIOMode();

  pinMode(LCD_DC_PIN, OUTPUT);
  pinMode(LCD_BL_PIN, OUTPUT);

  // Set state of LCD pin
  digitalWrite(LCD_D0_PIN, LOW);
  digitalWrite(LCD_D1_PIN, LOW);
  digitalWrite(LCD_D2_PIN, LOW);
  digitalWrite(LCD_D3_PIN, LOW);
  digitalWrite(LCD_D4_PIN, LOW);
  digitalWrite(LCD_D5_PIN, LOW);
  digitalWrite(LCD_D6_PIN, LOW);
  digitalWrite(LCD_D7_PIN, LOW);
  digitalWrite(LCD_DC_PIN, HIGH); 

  LCD_BACKLIGHT_ON();

  // LCD setup
  LCD_ACTIVE();

  WriteComm(0x2D);
  for (int i = 0; i <= 63; i++) {
    WriteData(i * 8);
  }

  for (int i = 0; i <= 63; i++) {
    WriteData(i * 4);
  }

  for (int i = 0; i <= 63; i++) {
    WriteData(i * 8);
  }

  WriteComm(0xB9); //Set_EXTC
  WriteData(0xFF);
  WriteData(0x83);
  WriteData(0x69);

  WriteComm(0xB1);  //Set Power 
  WriteData(0x85);
	WriteData(0x00);
	WriteData(0x34);
	WriteData(0x0A);
	WriteData(0x00);
	WriteData(0x0F);
	WriteData(0x0F);
	WriteData(0x2A);
	WriteData(0x32);
	WriteData(0x3F);
	WriteData(0x3F);
	WriteData(0x01); //update VBIAS
	WriteData(0x23);
	WriteData(0x01);
	WriteData(0xE6);
	WriteData(0xE6);
	WriteData(0xE6);
	WriteData(0xE6);
	WriteData(0xE6);

  WriteComm(0xB2); // SET Display 480x800
  WriteData(0x00);
  WriteData(0x20);
  WriteData(0x0A);
  WriteData(0x0A);
  WriteData(0x70);
  WriteData(0x00);
  WriteData(0xFF);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x00);
  WriteData(0x03);
  WriteData(0x03);
  WriteData(0x00);
  WriteData(0x01);

  WriteComm(0xB4); // SET Display 480x800
  WriteData(0x00);
  WriteData(0x18);
  WriteData(0x80);
  WriteData(0x10);
  WriteData(0x01);
  WriteComm(0xB6); // SET VCOM
  WriteData(0x2C);
  WriteData(0x2C);

  WriteComm(0xD5); //SET GIP
  WriteData(0x00);
  WriteData(0x05);
  WriteData(0x03);
  WriteData(0x00);
  WriteData(0x01);
  WriteData(0x09);
  WriteData(0x10);
  WriteData(0x80);
  WriteData(0x37);
  WriteData(0x37);
  WriteData(0x20);
  WriteData(0x31);
  WriteData(0x46);
  WriteData(0x8A);
  WriteData(0x57);
  WriteData(0x9B);
  WriteData(0x20);
  WriteData(0x31);
  WriteData(0x46);
  WriteData(0x8A);
  WriteData(0x57);
  WriteData(0x9B);
  WriteData(0x07);
  WriteData(0x0F);
  WriteData(0x02);
  WriteData(0x00);
  WriteComm(0xE0); //SET GAMMA
  WriteData(0x00);
  WriteData(0x08);
  WriteData(0x0D);
  WriteData(0x2D);
  WriteData(0x34);
  WriteData(0x3F);
  WriteData(0x19);
  WriteData(0x38);
  WriteData(0x09);
  WriteData(0x0E);
  WriteData(0x0E);
  WriteData(0x12);
  WriteData(0x14);
  WriteData(0x12);
  WriteData(0x14);
  WriteData(0x13);
  WriteData(0x19);
  WriteData(0x00);
  WriteData(0x08);

  WriteData(0x0D);
  WriteData(0x2D);
  WriteData(0x34);
  WriteData(0x3F);
  WriteData(0x19);
  WriteData(0x38);
  WriteData(0x09);
  WriteData(0x0E);
  WriteData(0x0E);
  WriteData(0x12);
  WriteData(0x14);
  WriteData(0x12);
  WriteData(0x14);
  WriteData(0x13);
  WriteData(0x19);
  WriteComm(0xC1); //set DGC
  WriteData(0x01); //enable DGC function
  WriteData(0x02); //SET R-GAMMA
  WriteData(0x08);
  WriteData(0x12);
  WriteData(0x1A);
  WriteData(0x22);
  WriteData(0x2A);
  WriteData(0x31);
  WriteData(0x36);
  WriteData(0x3F);
  WriteData(0x48);
  WriteData(0x51);
  WriteData(0x58);
  WriteData(0x60);
  WriteData(0x68);
  WriteData(0x70);
  WriteData(0x78);
  WriteData(0x80);
  WriteData(0x88);
  WriteData(0x90);
  WriteData(0x98);
  WriteData(0xA0);
  WriteData(0xA7);
  WriteData(0xAF);
  WriteData(0xB6);
  WriteData(0xBE);
  WriteData(0xC7);
  WriteData(0xCE);
  WriteData(0xD6);
  WriteData(0xDE);
  WriteData(0xE6);
  WriteData(0xEF);
  WriteData(0xF5);
  WriteData(0xFB);
  WriteData(0xFC);
  WriteData(0xFE);
  WriteData(0x8C);
  WriteData(0xA4);
  WriteData(0x19);
  WriteData(0xEC);
  WriteData(0x1B);
  WriteData(0x4C);

  WriteData(0x40);
  WriteData(0x02); //SET G-Gamma
  WriteData(0x08);
  WriteData(0x12);
  WriteData(0x1A);
  WriteData(0x22);
  WriteData(0x2A);
  WriteData(0x31);
  WriteData(0x36);
  WriteData(0x3F);
  WriteData(0x48);
  WriteData(0x51);
  WriteData(0x58);
  WriteData(0x60);
  WriteData(0x68);
  WriteData(0x70);
  WriteData(0x78);
  WriteData(0x80);
  WriteData(0x88);
  WriteData(0x90);
  WriteData(0x98);
  WriteData(0xA0);
  WriteData(0xA7);
  WriteData(0xAF);
  WriteData(0xB6);
  WriteData(0xBE);
  WriteData(0xC7);
  WriteData(0xCE);
  WriteData(0xD6);
  WriteData(0xDE);
  WriteData(0xE6);
  WriteData(0xEF);
  WriteData(0xF5);
  WriteData(0xFB);
  WriteData(0xFC);
  WriteData(0xFE);
  WriteData(0x8C);
  WriteData(0xA4);
  WriteData(0x19);
  WriteData(0xEC);
  WriteData(0x1B);
  WriteData(0x4C);
  WriteData(0x40);
  WriteData(0x02); //SET B-Gamma
  WriteData(0x08);
  WriteData(0x12);
  WriteData(0x1A);
  WriteData(0x22);
  WriteData(0x2A);
  WriteData(0x31);
  WriteData(0x36);
  WriteData(0x3F);
  WriteData(0x48);
  WriteData(0x51);
  WriteData(0x58);
  WriteData(0x60);
  WriteData(0x68);
  WriteData(0x70);
  WriteData(0x78);

  WriteData(0x80);
  WriteData(0x88);
  WriteData(0x90);
  WriteData(0x98);
  WriteData(0xA0);
  WriteData(0xA7);
  WriteData(0xAF);
  WriteData(0xB6);
  WriteData(0xBE);
  WriteData(0xC7);
  WriteData(0xCE);
  WriteData(0xD6);
  WriteData(0xDE);
  WriteData(0xE6);
  WriteData(0xEF);
  WriteData(0xF5);
  WriteData(0xFB);
  WriteData(0xFC);
  WriteData(0xFE);
  WriteData(0x8C);
  WriteData(0xA4);
  WriteData(0x19);
  WriteData(0xEC);
  WriteData(0x1B);
  WriteData(0x4C);
  WriteData(0x40);
  WriteComm(0x3A); //Set COLMOD
  WriteData(0x55);
  WriteComm(0x11); //Sleep Out
  delay(120);
  WriteComm(0x29); //Display On
		
  delay(10);

  WriteComm(0x3A); //pixel format setting
  WriteData(0x55); 
  WriteComm(0x36); //pixel format setting
  WriteData(0x00); 				
		 	             
  WriteComm(0x11);  
  delay(120);
  WriteComm(0x29);  //Display on 

  WriteComm(0x36); // Rotation
  WriteData(0x60);	
  LCD_INACTIVE();

  delay(10);

  Serial.println("Setup with Direct IO OK !");
  
  // Use SPI module
  // Setup SPI
  periph_module_reset(PERIPH_SPI2_MODULE);
  periph_module_enable(PERIPH_SPI2_MODULE);

  // spiInitBus
  spi_dev->clk_gate.clk_en = 1;
  spi_dev->clk_gate.mst_clk_sel = 1;
  spi_dev->clk_gate.mst_clk_active = 1;
  spi_dev->dma_conf.tx_seg_trans_clr_en = 1;
  spi_dev->dma_conf.rx_seg_trans_clr_en = 1;
  spi_dev->dma_conf.dma_seg_trans_en = 0;
  spi_dev->misc.val = 0;
  spi_dev->user.val = 0;
  spi_dev->user1.val = 0;
  spi_dev->ctrl.val = 0;
  spi_dev->clock.val = 0;

  // Set SPI Mode 0 (for 6800)
  spi_dev->misc.ck_idle_edge = 0;
  spi_dev->user.ck_out_edge = 1;

  // Set MSBFIRST
  spi_dev->ctrl.wr_bit_order = 0;
  spi_dev->ctrl.rd_bit_order = 0;

  // Set Clock
  uint32_t _div = spiFrequencyToClockDiv(LCD_PIXEL_CLOCK_HZ); // 40 MHz
  spi_dev->clock.val = _div;

  // Ref : 8.4.6 Access 8-bit I8080/MT6800 LCD in Master Half-Duplex Mode
  // Setup 1 : Add command 0x27
  spi_dev->ctrl.fcmd_oct = 1; // 8-bit data bus (Command)
  spi_dev->user.usr_command = 0;
  spi_dev->user2.usr_command_bitlen = 0;
  spi_dev->user2.usr_command_value = 0;

  // Setup 2 : Disable Address
  spi_dev->user.usr_addr = 0; // Disable Address
  spi_dev->user.usr_dummy = 0;
  spi_dev->user.usr_miso = 0; // Disable MISO

  // Setup 3 : Set dataout
  spi_dev->user.fwrite_oct = 1; // 8-bit data bus (Data)
  spi_dev->user.usr_mosi = 1; // Enable MISO

  // Config CS delay
  spi_dev->user.cs_setup = 0;
  spi_dev->user.cs_hold = 0;
}

void SwitchToGPIOMode() {
  pinMode(LCD_D0_PIN, OUTPUT);
  pinMode(LCD_D1_PIN, OUTPUT);
  pinMode(LCD_D2_PIN, OUTPUT);
  pinMode(LCD_D3_PIN, OUTPUT);
  pinMode(LCD_D4_PIN, OUTPUT);
  pinMode(LCD_D5_PIN, OUTPUT);
  pinMode(LCD_D6_PIN, OUTPUT);
  pinMode(LCD_D7_PIN, OUTPUT);

  pinMode(LCD_DE_PIN, OUTPUT);
}

void SwitchToSPIMode() {
// Data bus
  pinMatrixOutAttach(LCD_D0_PIN, FSPID_OUT_IDX, false, false);
  pinMatrixOutAttach(LCD_D1_PIN, FSPIQ_OUT_IDX, false, false);
  pinMatrixOutAttach(LCD_D2_PIN, FSPIWP_OUT_IDX, false, false);
  pinMatrixOutAttach(LCD_D3_PIN, FSPIHD_OUT_IDX, false, false);
  pinMatrixOutAttach(LCD_D4_PIN, FSPIIO4_OUT_IDX, false, false);
  pinMatrixOutAttach(LCD_D5_PIN, FSPIIO5_OUT_IDX, false, false);
  pinMatrixOutAttach(LCD_D6_PIN, FSPIIO6_OUT_IDX, false, false);
  pinMatrixOutAttach(LCD_D7_PIN, FSPIIO7_OUT_IDX, false, false);

  // Control
  pinMatrixOutAttach(LCD_DE_PIN, FSPICLK_OUT_IDX, false, false);
  // pinMatrixOutAttach(LCD_DC_PIN, FSPIC, false, false);
  // pinMatrixOutAttach(LCD_CS_PIN, FSPICS0_OUT_IDX, false, false);
}

uint8_t dataLen = 0;

void SPIStart() {
  dataLen = 0;
  spi_dev->user.fwrite_oct = 1; // 8-bit data bus (Data)
  spi_dev->user.usr_mosi = 1; // Enable MISO
}

void SPIAddData(uint8_t data) {
  uint8_t buffIndex = dataLen / 4;
  uint8_t bufferShift = (dataLen % 4) * 8;
  spi_dev->data_buf[buffIndex] |= data << bufferShift;
  dataLen++;
}

void SPIEnd() {
  spi_dev->ms_dlen.ms_data_bitlen = (dataLen * 8) - 1;
  
  spi_dev->cmd.update = 1;
  while (spi_dev->cmd.update);
  spi_dev->cmd.usr = 1;
  while (spi_dev->cmd.usr) ;
}

void LCD_drawBitmap(int x_start, int y_start, int x_end, int y_end, uint16_t* color_data) {
  SwitchToGPIOMode();

  LCD_ACTIVE();
  WriteComm(0x2A);
  WriteData((x_start >> 8) & 0xFF);
  WriteData(x_start & 0xFF);
  WriteData((x_end >> 8) & 0xFF);
  WriteData(x_end & 0xFF); 		
  WriteComm(0x2B);
  WriteData((y_start >> 8) & 0xFF);
  WriteData(y_start & 0xFF);
  WriteData((y_end >> 8) & 0xFF);
  WriteData(y_end & 0xFF); 		
  
  WriteComm(0x2C);
  digitalWrite(LCD_DC_PIN, HIGH);

  ///
  SwitchToSPIMode();

  // Write Color data
  spi_dev->user.usr_command = 0; // No Command
  spi_dev->user.usr_mosi = 1; // Has Data
  
  uint32_t index = 0;
  uint32_t len = (x_end - x_start + 1) * (y_end - y_start + 1) * 2u;
  int i;
  uint32_t write_bit;
  while(len) {
    write_bit = min((uint32_t)(len * 8), (uint32_t)(16 * 32));
    spi_dev->ms_dlen.ms_data_bitlen = write_bit - 1;
    for (i=0;i<16;i++) {
      spi_dev->data_buf[i] = (len >= 4 ? (color_data[index + 1] << 16) : 0x0) | color_data[index];
      index += 2;
    }
    spi_dev->cmd.update = 1;
    while (spi_dev->cmd.update);
    spi_dev->cmd.usr = 1;
    len -= (write_bit / 8);
    while(spi_dev->cmd.usr);
  }

  spi_dev->misc.cs_keep_active = 0;
  spi_dev->user.usr_mosi = 0;
  // spi_dev->mosi_dlen.usr_mosi_bit_len = 0;
  spi_dev->cmd.update = 1;
  while (spi_dev->cmd.update);
  spi_dev->cmd.usr = 1;
  while (spi_dev->cmd.usr);
  
  LCD_INACTIVE();
}
