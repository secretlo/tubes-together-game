from json.decoder import JSONDecodeError
import sys
import requests
import json
import multiprocessing
from flask import Flask, request
import socket
import ctypes

# this is pointer to the module object itself
this = sys.modules[__name__]

# setting props
this.browser = None
this.id = None
this.level = None
this.connection_count = None

# server props
this.connection_process = None
this.flask = Flask(__name__)
this.port = None
this.host = None
this.server_url = None

this.swap_manager = multiprocessing.Manager()
this.swap = this.swap_manager.dict()
this.swap['session_id'] = None

this.tasks = multiprocessing.Queue(20)
def add_task(js_func_name, *args):
   #print('Add: tasks are', this.tasks)
   this.tasks.put([js_func_name, args])
def get_all_tasks():
   #print('Get: tasks are', this.tasks)
   tasks = []
   while (not this.tasks.empty()) or (not this.tasks.qsize() == 0):
      tasks.append(this.tasks.get())
   return tasks

FLASK_HOST = '127.0.0.1'

PROTOCOL = 'http'
HOST = '127.0.0.1'
PORT = '1234'
SERVER = f'{PROTOCOL}://{HOST}:{PORT}'

def load(path, data):
   res = requests.post(f'{SERVER}{path}', data)
   try:
      res = json.loads(res.text)
   except JSONDecodeError:
      print(f'''Error: parsing JSON on storage.load('{path}', {json.dumps(data, indent=3)}).\nIncoming JSON:\n{res.text}''')
   return res
   
def gameload(action, data):
   final_data = {
      'action': action,
      'id': this.id,
      'session_id': this.swap['session_id'],
   }

   for [key, val] in data.items():
      final_data[key] = val

   return this.load('/game', final_data)


# STOP: Local Server

response_handlers = []

@this.flask.route('/', methods=['POST'])
def _on_response():
   req = request.get_json()
   action = req['action']
   print(f'_on_response: Post with action "{action}" and data {req}')
   response = None
   for [handler_action, handler] in response_handlers:
      if handler_action == action:
         response = handler(req)
         
   if response == None:
      print(f'_on_response: Error, no handler returned response for query with action "{action}"')
      raise ConnectionAbortedError()
   return response

def add_response_handler(action, callback):
   print(f'add_repsonse_handler: addign "{action}" with callback', callback)
   response_handlers.append([action, callback])
   

def run_server():
   this.host = FLASK_HOST
   this.port = _get_free_port()
   process = multiprocessing.Process(target=_connect_stuff, args=(this.host, this.port, this.tasks))
   this.connection_process = process
   process.start()
   this.server_url = f'{this.host}:{this.port}'

def stop_server():
   this.connection_process.terminate()
   
def _connect_stuff(host, port, tasks):
   this.tasks = tasks
   this.flask.run(host, port)
   
def _get_free_port():
   sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
   sock.bind(('localhost', 0))
   port = sock.getsockname()[1]
   sock.close()
   return port