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
            meta = artist + ' - ' + title
        else:
            meta = title
    else:
        if name:
            meta = name
        else:
            meta = ''

    return meta


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
        self.console.set_parse_cb(self.cmd_parse)
        self.client.connect(host=host, port=port)
        self.alive = False
        self.notify_thread = None
        self.update_player_info_cb = None
        self.idling = False
        self.cmd_lock = []
        self.player_info = []
        self.last_elapsed = 0

    def parse_cmd(self, cmd):
        print("cmd: " + cmd)
        self.acquire()
        try:
            status = self.client.status()
            elapsed = float(status.get('elapsed', 0))
            state = status.get('state', 'stop')

            if cmd == 'info':
                self.update_player_info()
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
        except mpd.base.CommandError:
            pass
        self.release()

    def acquire(self):
        self.cmd_lock.append(1)
        if self.idling:
            self.idling = False
            self.client.noidle()

    def release(self):
        self.cmd_lock.pop()
        if not self.cmd_lock and not self.idling:
            self.idling = True
            self.client.send_idle()

    def notify_fn(self):
        while self.alive:
            can_read = select([self.client], [], [], 0)[0]
            if can_read and not self.cmd_lock:
                self.idling = False
                changes = self.client.fetch_idle()
                if 'player' in changes:
                    self.update_player_info()
                self.client.send_idle()
                self.idling = True
            self.update_elapsed()
            time.sleep(0.2)

    def start(self):
        self.alive = True
        self.update_player_info()
        self.client.send_idle()
        self.idling = True
        self.console.start()
        self.notify_thread = threading.Thread(target=self.notify_fn, name='notify')
        self.notify_thread.daemon = True
        self.notify_thread.start()

    def join(self):
        self.console.join()
        self.notify_thread.join()

    def update_player_info(self):
        song = self.client.currentsong()
        status = self.client.status()
        print(status)

        self.player_info = {
            'name': song.get('name', ''),
            'title': song.get('title', ''),
            'artist': song.get('artist', ''),
            'elapsed': float(status.get('elapsed', 0)),
            'duration': float(status.get('duration', 0)),
            'is_playing': True if status.get('state') == 'play' else False,
            'timestamp': time.time(),
        }

        print(self.player_info)
        self.send_meta()
        self.send_elapsed(self.player_info['elapsed'])
        self.send_duration()

    def update_elapsed(self):
        if self.player_info.get('is_playing'):
            play_time = time.time() - self.player_info.get('timestamp')
            elapsed = self.player_info.get('elapsed') + play_time
            self.send_elapsed(elapsed)

    def send_meta(self):
        self.console.send('##CLI.META#: ' + do_meta(self.player_info))

    def send_elapsed(self, elapsed):
        elapsed = round(elapsed)
        if elapsed != self.last_elapsed:
            self.last_elapsed = elapsed
            self.console.send('##CLI.ELAPSED#: ' + str(elapsed))

    def send_duration(self):
        self.console.send('##CLI.DURATION#: ' + str(round(self.player_info['duration'])))

    def cmd_parse(self, cmd):
        if cmd.startswith('cli.'):
            command = cmd[len('cli.'):]
            self.parse_cmd(command)


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
