module HardwareTimer(

/*** APB3 BUS INTERFACE ***/
input PCLK, 				
output reg FABINT
); 

//This period is every 1ms

//WORKS 10000000

`define period 2500000
reg [31:0] counter;


always @(posedge PCLK)
begin
 if (counter == `period)
    begin
    counter <= 0;
    FABINT <= 1;
    end
 else
    begin
    counter <= counter + 1;
    FABINT <= 0;
    end
end


endmodule