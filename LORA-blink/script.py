
import serial
import time
import tkinter as tk
from tkinter import messagebox

# Config
PORT = '/dev/ttyUSB0'  # À adapter
BAUDRATE = 9600

class LoRaApp:
    def __init__(self, root):
        self.root = root
        self.root.title("DX-LR22 LoRa UART")
        self.ser = None
        self.at_mode = False

        # UI
        tk.Label(root, text="Port série :").pack()
        self.port_entry = tk.Entry(root)
        self.port_entry.insert(0, PORT)
        self.port_entry.pack()

        tk.Label(root, text="Fréquence (Hz) :").pack()
        self.freq_entry = tk.Entry(root)
        self.freq_entry.insert(0, "868000000")
        self.freq_entry.pack()

        tk.Label(root, text="Adresse destinataire :").pack()
        self.addr_entry = tk.Entry(root)
        self.addr_entry.insert(0, "1")
        self.addr_entry.pack()

        tk.Label(root, text="Spread Factor (7-12) :").pack()
        self.sf_entry = tk.Entry(root)
        self.sf_entry.insert(0, "7")
        self.sf_entry.pack()

        tk.Button(root, text="Connecter", command=self.connect).pack()

        tk.Label(root, text="Message :").pack()
        self.msg_entry = tk.Entry(root)
        self.msg_entry.pack()
        tk.Button(root, text="Envoyer", command=self.send).pack()
        tk.Button(root, text="Mode AT", command=self.at_mode_on).pack()

        self.log = tk.Text(root, height=10)
        self.log.pack()

    def connect(self):
        try:
            self.ser = serial.Serial(self.port_entry.get(), BAUDRATE, timeout=1)
            freq = self.freq_entry.get()
            sf = self.sf_entry.get()
            self.send_cmd(f'AT+FREQUENCY={freq}')
            self.send_cmd(f'AT+SPREAD={sf}')
            self.send_cmd('AT+ADDRESS=1')
            self.send_cmd('AT+MODE=0')
            self.log.insert(tk.END, f"Connecté ! SF={sf}\n")
            self.root.after(100, self.receive_loop)
        except Exception as e:
            messagebox.showerror("Erreur", str(e))

    def send_cmd(self, cmd):
        if self.ser is None:
            return
        self.ser.write((cmd + '\r\n').encode())
        time.sleep(0.1)
        return self.ser.readline().decode().strip()

    def send(self):
        if self.ser is None:
            messagebox.showerror("Erreur", "Non connecté au port série !")
            return
        if self.at_mode:
            cmd = self.msg_entry.get()
            self.send_cmd(cmd)
            self.log.insert(tk.END, f"AT> {cmd}\n")
        else:
            msg = self.msg_entry.get()
            addr = self.addr_entry.get()
            self.send_cmd(f'AT+SEND={addr},{len(msg)},{msg}')
            self.log.insert(tk.END, f"Envoyé à {addr} : {msg}\n")

    def at_mode_on(self):
        if self.ser is None:
            messagebox.showerror("Erreur", "Non connecté au port série !")
            return
        self.ser.write(b'+++')
        time.sleep(1)
        self.at_mode = True
        self.log.insert(tk.END, "Mode AT activé\n")

    def receive_loop(self):
        if self.ser and self.ser.in_waiting:
            line = self.ser.readline().decode().strip()
            if '+RCV=' in line:
                parts = line.split(',')
                rssi = parts[5]
                self.log.insert(tk.END, f"Reçu de {parts[1]} : {parts[3]} (RSSI: {rssi} dBm)\n")
            elif self.at_mode:
                self.log.insert(tk.END, f"AT> {line}\n")
        self.root.after(100, self.receive_loop)

# Lancer
root = tk.Tk()
app = LoRaApp(root)
root.mainloop()