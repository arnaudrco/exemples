###################################################################
#
#   Test ESP32-cam video streaming via websockets with javascript
#   Works with index.html
#
###################################################################

import os
import socket
import network
from websocket import websocket
import websocket_helper
from time import sleep, sleep_ms, ticks_ms
from machine import Pin, reset
import camera
from WifiConnect import WifiConnect
from binascii import b2a_base64
from car import *
#
r = motor(fwdThr=32, bwdThr=32)                  #  right motor
l = motor(fwd=14, bwd=15, fwdThr=22, bwdThr=22)  #  left motor
r.stop()
l.stop()
#
def send_image():
#     print("start sending image: ", ticks_ms() - tim, "ms")
    buf = b2a_base64(camera.capture())
    ws.write(buf)
#     print("stop sending image: ", ticks_ms() - tim, " ms   len: ", len(buf))
#     tim = ticks_ms()
#
def notify(s):
    msg = ws.read()
    try:
        if (lastcmd := msg.strip(b'ready\n').split(b'ready\n')[-1]) != b'':
            lastcmd = lastcmd.split(b'\n')[-1]
            ls, rs = lastcmd.split(b';')    # left speed, right speed
            l.setSpeed(int(ls))   # convert bytes to int
            r.setSpeed(int(rs))   # convert bytes to int
        if b'ready\n' in msg:
            send_image()
    except:
        print(msg)

def serve_page(sock):
    try:
        sock.sendall('HTTP/1.1 200 OK\nConnection: close\nServer: WebSocket Server\nContent-Type: text/html\n')
        length = os.stat("index.html")[6]
        sock.sendall('Content-Length: {}\n\n'.format(length))
        # Process page by lines to avoid large strings
        with open("index.html", 'r') as f:
            for line in f:
                sock.sendall(line)
        print("index.html sent")
    except OSError:
        print("serve_page : error")
        pass

# camera initialization
camera.init(0, format=camera.JPEG, fb_location=camera.PSRAM)
# camera.framesize(camera.FRAME_96X96)
# camera.framesize(camera.FRAME_QVGA)   # (320x240)
camera.framesize(camera.FRAME_QQVGA)   # (160x120)
camera.quality(10)
#
#  wifi access point mode
# ap = network.WLAN(network.AP_IF)
# ap.active(True)
# ap.config(essid="ESP32-cam", password="")
# activate server
# s = socket.socket()
# s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
# s.bind(('192.168.4.1', 80))
# s.listen(1)
#
#   wifi station mode
sta = WifiConnect('KIWI_11091')
# sta = WifiConnect('iPhone de Gilles')
s = socket.socket()
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

ai = socket.getaddrinfo("0.0.0.0", 80)
addr = ai[0][4]
s.bind(addr)
s.listen(1)
#
while True:
    try:
        print('listening...')
        cl, remote_addr = s.accept()
        print(remote_addr, ' connected')
        websocket_helper.server_handshake(cl)
        print('ws ok')
        ws = websocket(cl, True)
        cl.setblocking(False)
        cl.setsockopt(socket.SOL_SOCKET, 20, notify)
        while True:
            sleep_ms(10)
    except OSError:
        serve_page(cl)
    except KeyboardInterrupt:
        cl.setsockopt(socket.SOL_SOCKET, 20, None)
        cl.close()
        break

s.close()
del cl
del s
# reset()