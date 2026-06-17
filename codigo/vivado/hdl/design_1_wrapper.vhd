--Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
----------------------------------------------------------------------------------
--Tool Version: Vivado v.2019.1 (win64) Build 2552052 Fri May 24 14:49:42 MDT 2019
--Date        : Tue Jun 16 04:11:02 2026
--Host        : LAPTOP-14KI1LPM running 64-bit major release  (build 9200)
--Command     : generate_target design_1_wrapper.bd
--Design      : design_1_wrapper
--Purpose     : IP block netlist
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity design_1_wrapper is
  port (
    DDR3_addr : out STD_LOGIC_VECTOR ( 13 downto 0 );
    DDR3_ba : out STD_LOGIC_VECTOR ( 2 downto 0 );
    DDR3_cas_n : out STD_LOGIC;
    DDR3_ck_n : out STD_LOGIC_VECTOR ( 0 to 0 );
    DDR3_ck_p : out STD_LOGIC_VECTOR ( 0 to 0 );
    DDR3_cke : out STD_LOGIC_VECTOR ( 0 to 0 );
    DDR3_cs_n : out STD_LOGIC_VECTOR ( 0 to 0 );
    DDR3_dm : out STD_LOGIC_VECTOR ( 1 downto 0 );
    DDR3_dq : inout STD_LOGIC_VECTOR ( 15 downto 0 );
    DDR3_dqs_n : inout STD_LOGIC_VECTOR ( 1 downto 0 );
    DDR3_dqs_p : inout STD_LOGIC_VECTOR ( 1 downto 0 );
    DDR3_odt : out STD_LOGIC_VECTOR ( 0 to 0 );
    DDR3_ras_n : out STD_LOGIC;
    DDR3_reset_n : out STD_LOGIC;
    DDR3_we_n : out STD_LOGIC;
    HDMI_CLK_N : out STD_LOGIC;
    HDMI_CLK_P : out STD_LOGIC;
    HDMI_D0_N : out STD_LOGIC;
    HDMI_D0_P : out STD_LOGIC;
    HDMI_D1_N : out STD_LOGIC;
    HDMI_D1_P : out STD_LOGIC;
    HDMI_D2_N : out STD_LOGIC;
    HDMI_D2_P : out STD_LOGIC;
    IIC_0_scl_io : inout STD_LOGIC;
    IIC_0_sda_io : inout STD_LOGIC;
    LED : out STD_LOGIC_VECTOR ( 3 downto 0 );
    RESETCAM : out STD_LOGIC_VECTOR ( 0 to 0 );
    UART_0_rxd : in STD_LOGIC;
    UART_0_txd : out STD_LOGIC;
    datain : in STD_LOGIC_VECTOR ( 7 downto 0 );
    href : in STD_LOGIC;
    pclk : in STD_LOGIC;
    reset : in STD_LOGIC;
    sys_clk_i : in STD_LOGIC;
    vsync : in STD_LOGIC;
    xclk : out STD_LOGIC
  );
end design_1_wrapper;

architecture STRUCTURE of design_1_wrapper is
  component design_1 is
  port (
    reset : in STD_LOGIC;
    sys_clk_i : in STD_LOGIC;
    href : in STD_LOGIC;
    vsync : in STD_LOGIC;
    datain : in STD_LOGIC_VECTOR ( 7 downto 0 );
    pclk : in STD_LOGIC;
    xclk : out STD_LOGIC;
    RESETCAM : out STD_LOGIC_VECTOR ( 0 to 0 );
    LED : out STD_LOGIC_VECTOR ( 3 downto 0 );
    HDMI_CLK_N : out STD_LOGIC;
    HDMI_D0_P : out STD_LOGIC;
    HDMI_CLK_P : out STD_LOGIC;
    HDMI_D2_N : out STD_LOGIC;
    HDMI_D2_P : out STD_LOGIC;
    HDMI_D1_N : out STD_LOGIC;
    HDMI_D1_P : out STD_LOGIC;
    HDMI_D0_N : out STD_LOGIC;
    UART_0_rxd : in STD_LOGIC;
    UART_0_txd : out STD_LOGIC;
    DDR3_dq : inout STD_LOGIC_VECTOR ( 15 downto 0 );
    DDR3_dqs_p : inout STD_LOGIC_VECTOR ( 1 downto 0 );
    DDR3_dqs_n : inout STD_LOGIC_VECTOR ( 1 downto 0 );
    DDR3_addr : out STD_LOGIC_VECTOR ( 13 downto 0 );
    DDR3_ba : out STD_LOGIC_VECTOR ( 2 downto 0 );
    DDR3_ras_n : out STD_LOGIC;
    DDR3_cas_n : out STD_LOGIC;
    DDR3_we_n : out STD_LOGIC;
    DDR3_reset_n : out STD_LOGIC;
    DDR3_ck_p : out STD_LOGIC_VECTOR ( 0 to 0 );
    DDR3_ck_n : out STD_LOGIC_VECTOR ( 0 to 0 );
    DDR3_cke : out STD_LOGIC_VECTOR ( 0 to 0 );
    DDR3_cs_n : out STD_LOGIC_VECTOR ( 0 to 0 );
    DDR3_dm : out STD_LOGIC_VECTOR ( 1 downto 0 );
    DDR3_odt : out STD_LOGIC_VECTOR ( 0 to 0 );
    IIC_0_scl_i : in STD_LOGIC;
    IIC_0_scl_o : out STD_LOGIC;
    IIC_0_scl_t : out STD_LOGIC;
    IIC_0_sda_i : in STD_LOGIC;
    IIC_0_sda_o : out STD_LOGIC;
    IIC_0_sda_t : out STD_LOGIC
  );
  end component design_1;
  component IOBUF is
  port (
    I : in STD_LOGIC;
    O : out STD_LOGIC;
    T : in STD_LOGIC;
    IO : inout STD_LOGIC
  );
  end component IOBUF;
  signal IIC_0_scl_i : STD_LOGIC;
  signal IIC_0_scl_o : STD_LOGIC;
  signal IIC_0_scl_t : STD_LOGIC;
  signal IIC_0_sda_i : STD_LOGIC;
  signal IIC_0_sda_o : STD_LOGIC;
  signal IIC_0_sda_t : STD_LOGIC;
begin
IIC_0_scl_iobuf: component IOBUF
     port map (
      I => IIC_0_scl_o,
      IO => IIC_0_scl_io,
      O => IIC_0_scl_i,
      T => IIC_0_scl_t
    );
IIC_0_sda_iobuf: component IOBUF
     port map (
      I => IIC_0_sda_o,
      IO => IIC_0_sda_io,
      O => IIC_0_sda_i,
      T => IIC_0_sda_t
    );
design_1_i: component design_1
     port map (
      DDR3_addr(13 downto 0) => DDR3_addr(13 downto 0),
      DDR3_ba(2 downto 0) => DDR3_ba(2 downto 0),
      DDR3_cas_n => DDR3_cas_n,
      DDR3_ck_n(0) => DDR3_ck_n(0),
      DDR3_ck_p(0) => DDR3_ck_p(0),
      DDR3_cke(0) => DDR3_cke(0),
      DDR3_cs_n(0) => DDR3_cs_n(0),
      DDR3_dm(1 downto 0) => DDR3_dm(1 downto 0),
      DDR3_dq(15 downto 0) => DDR3_dq(15 downto 0),
      DDR3_dqs_n(1 downto 0) => DDR3_dqs_n(1 downto 0),
      DDR3_dqs_p(1 downto 0) => DDR3_dqs_p(1 downto 0),
      DDR3_odt(0) => DDR3_odt(0),
      DDR3_ras_n => DDR3_ras_n,
      DDR3_reset_n => DDR3_reset_n,
      DDR3_we_n => DDR3_we_n,
      HDMI_CLK_N => HDMI_CLK_N,
      HDMI_CLK_P => HDMI_CLK_P,
      HDMI_D0_N => HDMI_D0_N,
      HDMI_D0_P => HDMI_D0_P,
      HDMI_D1_N => HDMI_D1_N,
      HDMI_D1_P => HDMI_D1_P,
      HDMI_D2_N => HDMI_D2_N,
      HDMI_D2_P => HDMI_D2_P,
      IIC_0_scl_i => IIC_0_scl_i,
      IIC_0_scl_o => IIC_0_scl_o,
      IIC_0_scl_t => IIC_0_scl_t,
      IIC_0_sda_i => IIC_0_sda_i,
      IIC_0_sda_o => IIC_0_sda_o,
      IIC_0_sda_t => IIC_0_sda_t,
      LED(3 downto 0) => LED(3 downto 0),
      RESETCAM(0) => RESETCAM(0),
      UART_0_rxd => UART_0_rxd,
      UART_0_txd => UART_0_txd,
      datain(7 downto 0) => datain(7 downto 0),
      href => href,
      pclk => pclk,
      reset => reset,
      sys_clk_i => sys_clk_i,
      vsync => vsync,
      xclk => xclk
    );
end STRUCTURE;
