########################################################
#             Connection réseau
#
#   >>> from WifiConnect import WifiConnect
#   >>> WifiConnect(True)
#   ou
#   >>> execfile('WifiConnect.py')
#
########################################################

import network
from machine import Pin
from time import sleep
import sys

def WifiConnect(lan=None):
    credentials = { 'myssid':'mypwd'}

    ap_if = network.WLAN(network.AP_IF)
    ap_if.active(False)                     # disable wifi access point
    sta = network.WLAN(network.STA_IF)
    sta.active(True)
    lanlist = sta.scan()

    for nets in lanlist:
        ssid = nets[0].decode()
        if (not lan and ssid in credentials.keys()) or (ssid==lan):
            print('*** WifiConnect: trying connection to ', ssid)
            sta.connect(ssid, credentials[ssid])
            try:
                while not sta.isconnected():
                    sleep(1)
                    sys.stdout.write(b'.')
                print('\n*** WifiConnect: connection successfull :', sta.ifconfig()[0])
            except KeyboardInterrupt:
                print('\n*** WifiConnect: connection failed')
            return sta
    print('*** WifiConnect ***')
    if lan:
        print('   lan ' + lan + ' is not reachable')
    else:
        print('   no reachable lan')
    print('   discovered lans:')
    for lan in lanlist:
        print(lan)
    return False

if __name__ == '__main__':
    WifiConnect()