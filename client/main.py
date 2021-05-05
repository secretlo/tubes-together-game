import sys
import threading

from singlifier import *
from browser import *

import storage
import builtins

from logic.login import *
from logic.game import *


def main():
    print('Main: main thread is', threading.get_ident())
    
    global singlifier # Global because store HTML of pages, used by Browser to load_page
    singlifier = PageSinglifier() # Constructor do nothing
    singlifier.run() # Generating html files in cache folder

    # All logic is here
    browser = Browser('login', {
        'function': {
            'sendLogin': send_login,
            'sendRegister': send_register,
            'getLevels': get_levels,
            
            'startServer': start_server,
            'stopServer': stop_server,

            # game
            
            'getGameId': get_session_id,
            'getUserHost': get_user_host,
            'getGameData': get_game_data,
            'getConnectionCount': get_connection_count,
            'getReadyCount': get_ready_count,
            'getTasks': get_tasks,
            
            'sendReady': send_ready,
            'sendUnready': send_unready,
            'sendDisconnect': send_disconnect,
            'sendRotate': send_rotate,
            'sendHover': send_hover,
            'sendUnhover': send_unhover,
            'sendStartWater': send_start_water,
        }
    }) # Loading browser on 'index' page
    # [data] NO CODE SHOULD BE HERE, BROWSER BLOCK CODE EXECUTION


if __name__ == '__main__':
    main()