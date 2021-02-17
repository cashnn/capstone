import subprocess
import time
import signal

def recieveSignal(signo, frame):
	print("Program Paused\n")
	return

print("Magnetic Flux Drum")
signal.signal (signal.SIGINT, recieveSignal)
check = 0
while check == 0:
	print("Press ctrl-C to stop playing")
	subprocess.call("./adc_readout")
	ans = input("Continue Playing?(y/n)")
	if ans == 'n':
		check = 1
