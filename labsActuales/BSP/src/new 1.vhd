--Practica 2 TOC 2018

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity caja_fuerte is 
port{
	clk    : in std_logic;
	rst    : in std_logic;
	//pass   : in std_logic_vector(7 downto 0);
	prueba : in std_logic_vector(7 downto 0);
	intro  : in std_logic; 
	y      : out std_logic};
signal pass   : in std_logic_vector(7 downto 0);
end caja_fuerte;

architecture beh of caja_fuerte is

type t_state is { inicial_st , tres_st , dos_st , uno_st , final_st};
signal current_state , next_state : t_state;

begin

reg:process(clk,reset)  //proceso que representa el registro biestable
begin
   if reset = '1' then current_state <= inicial_st;
else
   if clk'event and clk = '1' then estado_act <= estado_sig;
   end if;
  end if;
end process reg;

p_next_state : process(current_state , intro)
begin 
		case current_st is
			when inicial_st =>
			if intro = '1' then 
				pass = prueba;
				next_state <= tres_st;
			else
				next_state <= inicial_st;
			end if;
			
			when tres_st =>
			if pass == prueba and intro = '1' then
				next_state <= inicial_st;
			elsif pass != prueba and intro = '1' then
				next_state <= dos_st;
			else 
				next_state <= tres_st;
			end if;
			
			when dos_st =>
			if pass == prueba and intro = '1' then
				next_state <= inicial_st;
			elsif pass != prueba and intro = '1' then
				next_state <= uno_st;
			else 
				next_state <= dos_st;
			end if;
			
			when uno_st =>
			if pass == prueba and intro = '1' then
				next_state <= inicial_st;
			elsif pass != prueba and intro = '1' then
				next_state <= final_st;
			else 
				next_state <= uno_st;
			end if;
			
			when final_st =>
			if reset = '1' then 
				next_state <= inicial_st;
			else
				next_state <= final_st;
			end if;
		end case;
end process p_next_state;
		
p_salidas : process (current_state)
--en algun momento hay que transformarlo en 7 segmentos
begin
	case current_state is
		when inicial_st =>
		y <= 3;
		when tres_st =>
		y <= 3;
		when dos_st =>
		y <= 2;
		when uno_st =>
		y <= 1;
		when final_st =>
		y <= 0;
	end case;
end process p_salidas;
	
	
			
			
		
		
