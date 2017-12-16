transcript on
onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -divider -height 30
add wave -noupdate -format Logic -radix binary /nco18_v13_tb/clk
add wave -noupdate -format Logic -radix binary /nco18_v13_tb/reset_n
add wave -noupdate -format Logic -radix binary /nco18_v13_tb/clken


add wave -noupdate -divider -height 50 {Phase Increment Value}
add wave -noupdate -format Literal -radix unsigned /nco18_v13_tb/phi

add wave -noupdate -divider -height 30 {Output value}
add wave -noupdate -format Logic -radix binary /nco18_v13_tb/out_valid
add wave -noupdate -color Yellow -format Literal -radix decimal  /nco18_v13_tb/sin_val
add wave -noupdate -color Cyan -format Literal -radix decimal /nco18_v13_tb/cos_val

add wave -noupdate -divider -height 80 {sine waveform}
add wave -noupdate -color Yellow -format Analog-Step -radix decimal -scale 5.0E-4 /nco18_v13_tb/sin_val

add wave -noupdate -divider -height 80 {cosine waveform}
add wave -noupdate -color Cyan -format Analog-Step -radix decimal -scale 5.0E-4 /nco18_v13_tb/cos_val

TreeUpdate [SetDefaultTree]
WaveRestoreCursors {356 ns}
WaveRestoreZoom {0 ns} {2132 ns}
configure wave -namecolwidth 150
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
