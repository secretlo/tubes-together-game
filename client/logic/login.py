import storage
import time

def send_login(login, password, callback):
   res = storage.load('/login', {
      'login': login,
      'password': password,
      'type': 'login',
   })
   
   if res['status'] == 'OK':
      storage.id = res['id']
   
   callback.Call(res)
   
def send_register(login, password, callback):
   res = storage.load('/login', {
      'login': login,
      'password': password,
      'type': 'register',
   })
   
   if res['status'] == 'OK':
      storage.id = res['id']
   
   callback.Call(res)
   

def get_levels(callback):
   res = storage.load('/login', {
      'id': storage.id,
      'type': 'levels',
      'login': '',
      'password': '',
   })
   
   if res['status'] == 'OK':
      storage.level = res['levels']
      callback.Call(res['levels'])
      
      
def start_server(callback):
   storage.run_server()
   res = storage.gameload('wait', {
      'host': storage.server_url,
      'level': storage.level,
   })
   if res['status'] == 'Connected':
      print(storage)
      storage.swap['session_id'] = res['session_id']
      storage.swap['field'] = res['field']
      storage.swap['players'] = res['players']
      
   callback.Call(res)

def stop_server():
   storage.gameload('unwait', {})
   storage.stop_server()