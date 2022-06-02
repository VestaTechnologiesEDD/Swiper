import serial
import time

class CommunicationsModule:

    def __init__(self, port='/dev/ttyACM0'):
        self.ser = serial.Serial(port, 115200, timeout=1)
        self._readline()

    def get_device_name(self):
        self._write_cmd('get_device_name')
        return self._readline()

    def read_encoder(self, idx):
        self._write_cmd(f'read_encoder {idx}')
        return self._readline()

    def _readline(self):
        return self.ser.readline().decode('utf-8').strip('\n')

    def _write_cmd(self, message):
        self.ser.write(bytes(f'{message}\r\n', 'utf-8'))

cm = CommunicationsModule(port='COM14')

print(cm.get_device_name())

while 1:
    print(cm.read_encoder(0))
    time.sleep(0.2)
