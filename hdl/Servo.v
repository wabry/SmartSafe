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
input SW_IN,
output reg FABINT,
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

// Debounce routine
reg [15:0] PB_cnt_1; // 16-bit counter
reg SW_OUT;
reg sync_1; // used as two flip-flops to synchronize
// button to the clk domain.
// First use two flipflops to synchronize the PB signal the "clk" clock domain
always @(posedge PCLK)
    sync_1 <= {sync_1,~SW_IN};
    wire PB_idle_1 = (SW_OUT==sync_1); // See if we have a new input state for PB
    wire PB_cnt_max_1 = &PB_cnt_1; // true when all bits of PB_cnt are 1's
    always @(posedge PCLK) begin
        if(PB_idle_1)
        PB_cnt_1<= 16'd0; // input and output are the same so clear counter
    else begin
        PB_cnt_1<= PB_cnt_1 + 16'd1; // input different than output, count
        if(PB_cnt_max_1)
        SW_OUT<= ~SW_OUT; // if the counter is maxed out,
        // change PB output
    end
end

//As the only thing we read is the switch, we can constantly drive it!		
always @(posedge PCLK) 	    //register control for APB3 reads
begin
	PRDATA[31:1] <= 30'h00000000;	
    PRDATA[0] <= SW_OUT;
end

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

//shift debounced switch inputs//
reg sw0_pulse[2:0];
always@( posedge PCLK)
begin
	sw0_pulse[0] <= ~SW_OUT;
	sw0_pulse[1] <= sw0_pulse[0];
	sw0_pulse[2] <= sw0_pulse[1];
end

//use shifted switch inputs to create a one clock cycle pulse//
wire sw0_int;
assign sw0_int = (sw0_pulse[1] == 1'b1) & (sw0_pulse[2] == 1'b0);

//create a FABINT pulse if either sw0 or sw1 is pressed//
always@(posedge PCLK)
    if(~PRESERN)
        begin
        FABINT   <= 1'b0;
        end
    else if(sw0_int)
        begin
        FABINT   <= 1;
        end
    else
        begin
        FABINT   <= 0;
        end

// Timer to set FABINT


endmodule