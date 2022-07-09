/* verilator lint_off UNUSED */
/* verilator lint_off MULTIDRIVEN */
module gpioemu(n_reset, saddress[15:0], srd, swr, sdata_in[31:0], sdata_out[31:0], gpio_in[31:0], gpio_latch, gpio_out[31:0], clk, gpio_in_s_insp[31:0]);

	input 				n_reset;
	input 	[15:0] 		saddress;
	input 				srd;
	input 				swr;
	input	[31:0] 		sdata_in; // Szyna danych
	output	[31:0] 		sdata_out; // Szuna danych - doprowadza odpowiednie informacje, gdy zapis do rejestru wyjsciowego
	input 	[31:0] 		gpio_in;
	input				gpio_latch;
	output	[31:0] 		gpio_out;
  output [31:0] gpio_in_s_insp;
	
	// Zmienne pomocnicze w celu wzywania zmiennych w blokach always
	
	reg 	[31:0] 		gpio_in_s;
	reg 	[31:0] 		gpio_out_s;
	reg 	[31:0] 		sdata_out_s;
	
	input 				clk;
	reg     [7:0]       counter;
  reg clk_slowed;
	reg [7:0] count = 0; 
	reg                 INT;
	
	assign gpio_in_s_insp = gpio_in_s;
	// Licznik 8-bitowy:
	
	initial INT = 1'b0;
	initial counter = 8'h2F;
	
	// GLOBALNY RESET	
	always @(negedge n_reset)
	begin
		counter <= 8'h2F;
		INT <= 1'b0;
		gpio_in_s <= 0;
		gpio_out_s <= 0;
		sdata_out_s <= 'h0;
	end
	
	wire [11:0] short_saddress;
	
	assign short_saddress = {saddress[11:0]}; // Zmienna pomocnicza
	
	always @(posedge clk)
  begin 
    count <= count +1;
    if (count == 20)
      begin 
        count <= 0;
        clk_slowed <= ~clk_slowed;
      end
  end
  
  
	always @(posedge clk_slowed)
	begin
		if(counter != 8'h00)
			counter <= counter - 8'h1;
		else
			begin
				counter <= 8'h2F;
				INT <= 1'b1;
			end
	end
	

	
	// obsluga odczytu z GPIO
		
	always @(posedge gpio_latch)
	begin
		gpio_in_s <= gpio_in;
		
	end
			
		
	always @(negedge srd)

   begin
				if (short_saddress == 12'h168)
					sdata_out_s <= {14'h0,gpio_in_s[3:0], 14'b0}; 
					
				else if (short_saddress == 12'h170)
					sdata_out_s <= {26'h0, gpio_in_s[7:4], 2'b0};
					
				else if (short_saddress ==  12'h178)
					sdata_out_s <= {25'h0,INT, 6'h0};
				else
					sdata_out_s <= 'h0; 
			end
	

		
		
	always @(negedge swr)
	begin
		if (short_saddress == 12'h16C)
				gpio_out_s[3:0] <= sdata_in[17:14];
        
		else if (short_saddress ==	12'h174)  // czujnik 1
				gpio_out_s[7:4] <= sdata_in[5:2];
        
		else if (short_saddress == 12'h17C)
				if(sdata_in[6] == 1'b1)  
					begin
						counter <= 8'h2F;
            INT <= 1'b0;
					end
		else
				gpio_out_s <= 'h0;

	end
	
	assign sdata_out = sdata_out_s;
	assign gpio_out = gpio_out_s;

	
		
endmodule