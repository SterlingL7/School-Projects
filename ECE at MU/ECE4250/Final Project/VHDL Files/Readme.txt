README for ECE 4250 Power and Area Efficient Approximate Booth Multipliers

each model file below takes in 2 16-bit inputs, multiplies them using
the associated model number and approximation value, and produces a
32-bit approximate product.

the test bench 



Inputs
==================================
for models:
    clk: 	clock signal
    A, B:	16-bit binary numbers

for TestBench:
    clock: clock signal

Outputs
==================================
for models:
    P: 32-bit binary product
for testbench:
    P4: 32-bit binary output
    MRED#: mean relative error distance for m=#
    NMED#: normalized mean error distance for m=#
    
Heirarchy
==================================
Main File:
TestBench					-> file containing loop that generates uniform
							-  random inputs and processes them with one of
							-  the approximate multiplier models below.
                            -  it uses all 4 of the m values for the model.
							-  also produces running error total for analysis.
							-  
						
includes:
	exact booth multiplier:
	radix4boothmultexact.vhd	-> implementation of 16-bit radix-4 booth
								-  multiplier. takes in 2 16-bit inputs and
								-  produces a 32-bit product. the final product
								-  takes 3 clock cycles to produce
	
	Model 1 files
	ABM-M1_4.vhd				-> 4-bit approximation for model 1
	ABM-M1_8.vhd				-> 8-bit approximation for model 1
	ABM-M1_12.vhd				-> 12-bit approximation for model 1
	ABM-M1_16.vhd				-> 16-bit approximation for model 1
								-  each of the above takes in 2 16-bit inputs and
								-  produces a 32-bit product. the final product
								-  takes 3 clock cycles to produce. the
								-  approximated bits are each generated with
								-  PPG-2S from the paper.
								
	Model 2 files
	ABM-M2_2.vhd				-> 2-bit approximation for model 2
	ABM-M2_4.vhd				-> 4-bit approximation for model 2
	ABM-M2_6.vhd				-> 6-bit approximation for model 2
	ABM-M2_8.vhd				-> 8-bit approximation for model 2
								-  each of the above takes in 2 16-bit inputs and
								-  produces a 32-bit product. the final product
								-  takes 3 clock cycles to produce. the
								-  approximation reduces the LSBs of A to a single
								-  value, which is input to PPG-2S from the paper.
								-  the lowest bits are zeros in the final product

	Model 3 files
	ABM-M3_4.vhd				-> 4-bit approximation for model 3
	ABM-M3_8.vhd				-> 8-bit approximation for model 3
	ABM-M3_12.vhd				-> 12-bit approximation for model 3
	ABM-M3_16.vhd				-> 16-bit approximation for model 3
								-  each of the above takes in 2 16-bit inputs and
								-  produces a 32-bit product. the final product
								-  takes 3 clock cycles to produce. the
								-  approximation ORs the LSBs of A to a single
								-  bit, which is input to PPG-1S from the paper.
								-  the lowest bits are zeros in the final product
	
	all model files above include:
		Component files
		HalfAdder.vhd			-> 1-bit half adder
		FullAdder.vhd 			-> 1-bit full adder
		
simulation instructions
==================================
To simulate the one of the models, load all the files listed into a 
new project in ModelSim and compile. There should be no errors. Go to the library tab,
expand work, and select the file you would like to simulate. double click or click 
simulate. in the blue section, highlight the A,B,clk, and P elements, right click, add wave.
this should bring up the wave panel with these 4 values shown. right click clk, select clock,
press OK. For A and B, right click, select force, and enter the desired hexadecimal value in
the field. when both are set, go the upper toolbars to find the simulation button and click 
run 3 times. P should now show the resulting product. 

to simulate the TestBench, follow the steps above, but set the clock period to 10ns. on the 
simulation tool bar, set the run time to 70ms and press run. when running from the student edition
on my home PC, this took up to 2 hours to complete a full million test values. leaving the timer at 
100ns and pressing run should produce 1 output per run. to change which model is being tested,
change the component names to match the format radix4boothmulabmm<model#>m<approx bits>, for example:
radix4boothmulabmm3m8 for the model 3 simulation approximating the lowest 8 bits. these names should
match the entity names for each model as viewed in the work folder of the ibrary tab.
