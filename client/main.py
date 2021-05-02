import sys
import threading

from singlifier import *
from browser import *

import storage
import builtins

from logic.login import *
from logic.create_game import *
from logic.connect_game import *
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
            'createGame': create_game,
            'connectGame': connect_game,
            'getLevels': get_levels,
            
            'startServer': start_server,
            'stopServer': stop_server,

            # game
            
            'getGameId': get_session_id,
            'getUserHost': get_user_host,
            'getConnectionCount': get_connection_count,
            'getReadyCount': get_ready_count,
            'getTasks': get_tasks,
            
            'sendReady': send_ready,
            'sendUnready': send_unready,
            'sendDisconnect': send_disconnect,
            'sendPositionUpdate': send_position_update,
            'sendEnemyRequest': send_enemy_request,
            'sendDie': send_die,
        }
    }) # Loading browser on 'index' page
    # [data] NO CODE SHOULD BE HERE, BROWSER BLOCK CODE EXECUTION


if __name__ == '__main__':
    main()