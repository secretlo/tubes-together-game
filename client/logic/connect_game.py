import storage
import json
import requests
import socket


def connect_game(session_id, callback):
   storage.session_id = session_id
   print('Connect Game: session_id =', storage.session_id)
   
   storage.run_server()

   res = storage.load('/game', {
      'action': 'connect',
      'session_id': storage.session_id,
      'host': f'{storage.host}:{storage.port}',
   })
   
   if res['status'] == 'OK':
      storage.connection_count = res['connection_count']

   callback.Call(res)

