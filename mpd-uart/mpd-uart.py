import getopt
import sys
import threading
import time

import serial
import mpd
from select import select


def do_meta(song):
    title = song.get('title', '')
    name = song.get('name', '')
    artist = song.get('artist', '')

    if title:
        if artist:
            name = artist + ' - ' + title
        else:
            name = title

    return name


class Console(object):
    def __init__(self, port, baudrate):
        self.serial = serial.Serial(port=port, baudrate=baudrate)
        self.alive = False
        self.reader_thread = None
        self.parse_cb = None
        self.write_lock = threading.Lock()

    def set_parse_cb(self, cb):
        self.parse_cb = cb

    def send(self, info):
        self.write_lock.acquire()
        print(">>>: " + info)
        self.serial.write(bytes(info + '\r\n', 'utf-8'))
        self.write_lock.release()

    def start(self):
        self.alive = True
        self.reader_thread = threading.Thread(target=self.reader_fn, name='reader')
        self.reader_thread.daemon = True
        self.reader_thread.start()

    def join(self):
        self.reader_thread.join()

    def reader_fn(self):
        while self.alive:
            str_seq = self.serial.read_until(serial.LF, 256)
            str_seq = str(str_seq, 'utf-8').strip()
            if self.parse_cb:
                self.parse_cb(str_seq)
            if str_seq == 'quit':
                self.alive = False


class Player(object):
    def __init__(self, sport, baudrate, host, port):
        self.client = mpd.MPDClient()
        self.console = Console(port=sport, baudrate=baudrate)
        self.console.set_parse_cb(self.parse_handler)
        self.client.connect(host=host, port=port)
        self.alive = False
        self.notify_thread = None
        self.player_info = []
        self.cmd_queue = []

    def parse_cmd(self, cmd, status):
        elapsed = float(status.get('elapsed', 0))
        state = status.get('state', 'stop')

        if cmd == 'info':
            self.player_info = []
        if cmd == 'play':
            self.client.play()
        if cmd == 'stop':
            self.client.stop()
        if cmd == 'pause':
            print(status)
            if state == 'stop':
                self.client.play()
            else:
                self.client.pause()
        if cmd == 'next':
            if state == 'play':
                self.client.next()
        if cmd == 'previous':
            if state == 'play':
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

    def notify_fn(self):
        while self.alive:
            self.update_player_info()
            time.sleep(0.2)
            pass

    def start(self):
        self.alive = True
        self.console.start()
        self.notify_thread = threading.Thread(target=self.notify_fn, name='notify')
        self.notify_thread.daemon = True
        self.notify_thread.start()

    def join(self):
        self.console.join()
        self.notify_thread.join()

    def update_player_info(self):
        status = self.client.status()
        if self.cmd_queue:
            cmd = self.cmd_queue.pop(0)
            self.parse_cmd(cmd, status)
            status = self.client.status()
        song = self.client.currentsong()

        player_info = {
            'name': song.get('name', ''),
            'artist': song.get('artist', ''),
            'title': song.get('title', ''),
            'elapsed': float(status.get('elapsed', 0)),
            'duration': float(status.get('duration', 0)),
            'is_playing': status.get('state') == 'play',
            'timestamp': time.time(),
        }

        update_all = False
        if not self.player_info:
            self.player_info = player_info
            update_all = True

        update_name = player_info['name'] != self.player_info['name']
        update_title = player_info['title'] != self.player_info['title']
        update_artist = player_info['artist'] != self.player_info['artist']
        update_meta = update_name or update_title or update_artist

        update_elapsed = (player_info['is_playing'] and
                          round(player_info['elapsed']) != round(self.player_info['elapsed']))

        update_duration = player_info['duration'] != self.player_info['duration']

        self.player_info = player_info

        if update_all or update_meta:
            self.send_meta()
        if update_all or update_elapsed:
            self.send_elapsed()
        if update_all or update_duration:
            self.send_duration()

    def send_meta(self):
        self.console.send('##CLI.META#: ' + do_meta(self.player_info))

    def send_elapsed(self):
        self.console.send('##CLI.ELAPSED#: ' + str(round(self.player_info['elapsed'])))

    def send_duration(self):
        self.console.send('##CLI.DURATION#: ' + str(round(self.player_info['duration'])))

    def parse_handler(self, input):
        print("input: " + input)
        if input.startswith('cli.'):
            command = input[len('cli.'):]
            self.cmd_queue.append(command)


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

    player = Player(sport=port, baudrate=baudrate, host="localhost", port=6600)
    player.start()
    player.join()


if __name__ == "__main__":
    main(sys.argv[1:])