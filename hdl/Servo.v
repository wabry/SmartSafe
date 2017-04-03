module Servo(

/*** APB3 BUS INTERFACE ***/
input PCLK, 				// clock
input PRESERN, 				// system reset
input PSEL, 				// peripheral select
input PENABLE, 				// distinguishes access phase
output wire PREADY, 		// peripheral ready signal
output wire PSLVERR,		// error signal
input PWRITE,				// distinguishes read and write cycles
input [31:0] PADDR,			// I/O address
input wire [31:0] PWDATA,	// data from processor to I/O device (32 bits)
output reg [31:0] PRDATA,	// data to processor from I/O device (32-bits)

/*** I/O PORTS DECLARATION ***/
output reg pwm
); 

`define period 2000000
`define halfsec 50000000

wire PWM_WRITE = PSEL & PWRITE & PENABLE & PADDR[2] && ~PADDR[1] && ~PADDR[0]; // TODO: Add address

assign PSLVERR = 0;
assign PREADY = 1;

//////////////////////////////////
// end of APB3 interface header //
//////////////////////////////////

//As the only thing we read is the switch, we can constantly drive it!		
/*always @(posedge PCLK) 	    //register control for APB3 reads
begin
	PRDATA[31:1] <= 30'h00000000;	
    PRDATA[0] <= SW_OUT;
end*/

reg [23:0] PWM_OUT;

always @(posedge PCLK) 		// register control for APB3 writes
begin
	if(!PRESERN)
        begin
		PWM_OUT[23:0] <= 24'b000000010101111110010000; // 0 Degree Servo
        end
	else if(PWM_WRITE)
        begin
		PWM_OUT[23:0] <= PWDATA[23:0];                 // Write to servo
        end
end

reg [31:0] count;
always @(posedge PCLK)
begin
 if (count == `period)
    count <= 0;
 else
    count <= count + 1;
 if (count < PWM_OUT)
    pwm <= 1;
 else
    pwm <= 0;
end
// Timer to set FABINT


endmodule