from machine import Pin, PWM

class motor():
    
    def __init__(self, fwd=12, bwd=13, fwdThr=0, bwdThr=0):
        self.fwd = PWM(Pin(fwd, Pin.OUT))
        self.fwd.freq(1000)
        self.fwdThr = fwdThr *1023/100   # convert from % to [0;1023]
        self.bwd = PWM(Pin(bwd, Pin.OUT))
        self.bwd.freq(1000)
        self.bwdThr = bwdThr             # convert from % to [0;1023]
        self.stop()
        
    def stop(self):
        self.fwd.duty(0)
        self.bwd.duty(0)
    
    def setSpeed(self, speed):
        ''' -100 <= speed <= 100 '''
        if speed < -100 or speed > 100: return
        if speed == 0:
            self.stop()
            return
        if speed > 0:
            self.bwd.duty(0)
            duty = int(self.fwdThr + speed * (1023 - self.fwdThr)/100)
            self.fwd.duty(duty)
        else:
            self.fwd.duty(0)
            duty = int(self.bwdThr + abs(speed) * (1023 - self.bwdThr)/100)
            self.bwd.duty(duty)

class steer(motor):
    
    def __init__(self, left=14, right=15 ):
        super().__init__(fwd=left, bwd=right)
    
    def turn(self, dir):
        self.setSpeed(dir)