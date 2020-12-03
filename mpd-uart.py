import getopt
import sys
import threading
import time

import serial
import mpd
from select import select


class Console(object):
    def __init__(self, port, baudrate):
        self.serial = serial.Serial(port=port, baudrate=baudrate)
        self.alive = False
        self.reader_thread = None
        self.parse_cb = None

    def set_parse_cb(self, cb):
        self.parse_cb = cb

    def send(self, info):
        self.serial.write(bytes(info + '\r\n', 'utf-8'))

    def start(self):
        self.alive = True
        self.reader_thread = threading.Thread(target=self.reader, name='reader')
        self.reader_thread.daemon = True
        self.reader_thread.start()

    def join(self):
        self.reader_thread.join()

    def reader(self):
        while self.alive:
            str_seq = self.serial.read_until(serial.LF, 256)
            str_seq = str(str_seq, 'utf-8').strip()
            if self.parse_cb:
                self.parse_cb(str_seq)
            if str_seq == 'quit':
                self.alive = False


class Player(object):
    def __init__(self, host, port):
        self.client = mpd.MPDClient()
        self.client.connect(host=host, port=port)
        self.alive = False
        self.notify_thread = None
        self.song_update_cb = None
        self.idling = False
        self.cmd_lock = False

    def parse_cmd(self, cmd):
        self.acquire()

        status = self.client.status()
        if 'elapsed' in status:
            elapsed = float(status['elapsed'])
        else:
            elapsed = 0
        state = status['state']

        if cmd == 'play':
            self.client.play()
        if cmd == 'stop':
            self.client.stop()
        if cmd == 'pause':
            self.client.pause()
        if cmd == 'next':
            self.client.play()
            self.client.next()
        if cmd == 'previous':
            self.client.play()
            self.client.previous()
        if cmd == 'rewind' and state == 'play':
            pos = elapsed - 5
            if pos < 0:
                pos = 0
            self.client.seekcur(pos)
        if cmd == 'ffwd' and state == 'play':
            pos = elapsed + 5
            self.client.seekcur(pos)
        if cmd == 'load music':
            self.client.clear()
            self.client.load("Music")
        if cmd == 'load radio':
            self.client.clear()
            self.client.load("Radio")
        self.release()

    def acquire(self):
        self.cmd_lock = True
        if self.idling:
            self.client.noidle()
            self.idling = False

    def release(self):
        self.cmd_lock = False
        if not self.cmd_lock and not self.idling:
            self.client.send_idle()
            self.idling = True

    def notify(self):
        self.update_player()
        self.client.send_idle()
        self.idling = True

        while self.alive:
            can_read = select([self.client], [], [], 0)[0]
            if can_read and not self.cmd_lock:
                self.idling = False
                changes = self.client.fetch_idle()
                if 'player' in changes:
                    self.update_player()
                self.client.send_idle()
                self.idling = True
            time.sleep(0.2)

    def start(self):
        self.alive = True
        self.notify_thread = threading.Thread(target=self.notify, name='notify')
        self.notify_thread.daemon = True
        self.notify_thread.start()

    def join(self):
        self.notify_thread.join()

    def update_player(self):
        song = self.client.currentsong()
        if self.song_update_cb:
            self.song_update_cb(song)

    def set_song_update_cb(self, cb):
        self.song_update_cb = cb


class MpdControl(object):
    def __init__(self, sport, baudrate, host, port):
        self.console = Console(port=sport, baudrate=baudrate)
        self.player = Player(host=host, port=port)
        self.last_name = ''

    def cmd_parse(self, cmd):
        if str(cmd).startswith('cli.'):
            command = str(cmd).lstrip('cli.')
            self.player.parse_cmd(command)

    def get_field(self, song, key):
        if key in song:
            return song[key]
        else:
            return ''

    def song_update(self, song):
        title = self.get_field(song, 'title')
        name = self.get_field(song, 'name')
        artist = self.get_field(song, 'artist')

        if title != '':
            if artist != '':
                show_name = artist + ' - ' + title
            else:
                show_name = title
        else:
            if name != '':
                show_name = name
            else:
                show_name = ''

        if (show_name != self.last_name):
            self.console.send(show_name)
            self.last_name = show_name

    def run(self):
        self.console.set_parse_cb(self.cmd_parse)
        self.player.set_song_update_cb(self.song_update)

        self.console.start()
        self.player.start()

        self.console.join()
        self.player.join()


def main(argv):
    port = '/dev/serial0'
    baudrate = 115200
    try:
        opts, args = getopt.getopt(argv, "p:b:", ["port=", "baudrate="])
    except getopt.GetoptError:
        print("Wrong command line arguments")
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-p", "--port"):
            port = arg
        if opt in ("-b", "--baudrate"):
            baudrate = arg

    mc = MpdControl(port, baudrate, "localhost", 6600)
    mc.run()


if __name__ == "__main__":
    main(sys.argv[1:])
