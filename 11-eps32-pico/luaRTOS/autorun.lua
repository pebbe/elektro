
print("")
print("CPU: "..cpu.model())
print("Board: "..cpu.board())
print("Speed: "..cpu.speed().." MHz")
print("Temp: "..cpu.temperature().." ^C")
print("")
tmr.sleep(10)
print(os.date())
print("")
