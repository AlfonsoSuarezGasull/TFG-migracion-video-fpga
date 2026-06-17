-- ==============================================================
-- Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
-- Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
-- ==============================================================
library ieee; 
use ieee.std_logic_1164.all; 
use ieee.std_logic_unsigned.all;

entity Procesador_Video_cud_rom is 
    generic(
             DWIDTH     : integer := 7; 
             AWIDTH     : integer := 7; 
             MEM_SIZE    : integer := 120
    ); 
    port (
          addr0      : in std_logic_vector(AWIDTH-1 downto 0); 
          ce0       : in std_logic; 
          q0         : out std_logic_vector(DWIDTH-1 downto 0);
          clk       : in std_logic
    ); 
end entity; 


architecture rtl of Procesador_Video_cud_rom is 

signal addr0_tmp : std_logic_vector(AWIDTH-1 downto 0); 
type mem_array is array (0 to MEM_SIZE-1) of std_logic_vector (DWIDTH-1 downto 0); 
signal mem : mem_array := (
    0 => "0111100", 1 to 5=> "1000010", 6 => "0111100", 7 => "0000000", 
    8 => "0001000", 9 => "0011000", 10 to 13=> "0001000", 14 => "0011100", 
    15 => "0000000", 16 => "0111100", 17 => "1000010", 18 => "0000010", 
    19 => "0111100", 20 to 21=> "1000000", 22 => "1111110", 23 => "0000000", 
    24 => "0111100", 25 => "1000010", 26 => "0000010", 27 => "0011100", 
    28 => "0000010", 29 => "1000010", 30 => "0111100", 31 => "0000000", 
    32 => "0000100", 33 => "0001100", 34 => "0010100", 35 => "0100100", 
    36 => "1111110", 37 to 38=> "0000100", 39 => "0000000", 40 => "1111110", 
    41 => "1000000", 42 => "1111100", 43 to 44=> "0000010", 45 => "1000010", 
    46 => "0111100", 47 => "0000000", 48 => "0111100", 49 => "1000000", 
    50 => "1111100", 51 to 53=> "1000010", 54 => "0111100", 55 => "0000000", 
    56 => "1111110", 57 => "0000010", 58 => "0000100", 59 => "0001000", 
    60 to 62=> "0010000", 63 => "0000000", 64 => "0111100", 65 to 66=> "1000010", 
    67 => "0111100", 68 to 69=> "1000010", 70 => "0111100", 71 => "0000000", 
    72 => "0111100", 73 to 74=> "1000010", 75 => "0111110", 76 => "0000010", 
    77 => "1000010", 78 => "0111100", 79 to 82=> "0000000", 83 to 84=> "0011000", 
    85 to 87=> "0000000", 88 => "1000010", 89 => "1100110", 90 => "1011010", 
    91 to 94=> "1000010", 95 => "0000000", 96 => "1111100", 97 => "1000010", 
    98 => "1111100", 99 to 100=> "1000010", 101 => "1111100", 102 to 103=> "0000000", 
    104 => "0000010", 105 => "0000100", 106 => "0001000", 107 => "0010000", 
    108 => "0100000", 109 => "1000000", 110 to 111=> "0000000", 112 => "0111100", 
    113 => "1000000", 114 => "0111100", 115 => "0000010", 116 => "0111100", 
    117 to 119=> "0000000" );

attribute syn_rom_style : string;
attribute syn_rom_style of mem : signal is "select_rom";
attribute ROM_STYLE : string;
attribute ROM_STYLE of mem : signal is "distributed";

begin 


memory_access_guard_0: process (addr0) 
begin
      addr0_tmp <= addr0;
--synthesis translate_off
      if (CONV_INTEGER(addr0) > mem_size-1) then
           addr0_tmp <= (others => '0');
      else 
           addr0_tmp <= addr0;
      end if;
--synthesis translate_on
end process;

p_rom_access: process (clk)  
begin 
    if (clk'event and clk = '1') then
        if (ce0 = '1') then 
            q0 <= mem(CONV_INTEGER(addr0_tmp)); 
        end if;
    end if;
end process;

end rtl;

Library IEEE;
use IEEE.std_logic_1164.all;

entity Procesador_Video_cud is
    generic (
        DataWidth : INTEGER := 7;
        AddressRange : INTEGER := 120;
        AddressWidth : INTEGER := 7);
    port (
        reset : IN STD_LOGIC;
        clk : IN STD_LOGIC;
        address0 : IN STD_LOGIC_VECTOR(AddressWidth - 1 DOWNTO 0);
        ce0 : IN STD_LOGIC;
        q0 : OUT STD_LOGIC_VECTOR(DataWidth - 1 DOWNTO 0));
end entity;

architecture arch of Procesador_Video_cud is
    component Procesador_Video_cud_rom is
        port (
            clk : IN STD_LOGIC;
            addr0 : IN STD_LOGIC_VECTOR;
            ce0 : IN STD_LOGIC;
            q0 : OUT STD_LOGIC_VECTOR);
    end component;



begin
    Procesador_Video_cud_rom_U :  component Procesador_Video_cud_rom
    port map (
        clk => clk,
        addr0 => address0,
        ce0 => ce0,
        q0 => q0);

end architecture;


